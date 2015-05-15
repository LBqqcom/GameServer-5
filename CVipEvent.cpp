#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CVipEvent.h"
#include "CActive.h"

CCVipEvent::CCVipEvent(CCGameMesManager *pMesManager,CT_WORD id) : CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_RENEWALS_REQUEST);
	RegistMessage(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_GIFT_RECEIVE_REQUEST);
	RegistMessage(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_HARVEST_ONCE_REQUEST);
	RegistMessage(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_ACTIVE);
	G_ValueRange vr = {0};
	if(G_GetValueRange(VALUE_TYPE_VIP_TIME,  &vr))
	{
		RegistTimer(IDI_VIP_TIMER);
		SetTimer(IDI_VIP_TIMER, (CT_INT32)(vr.fMaxVal * 1000));
	}
	memset(&vr,0,sizeof(vr));
	if (G_GetValueRange(VALUE_TYPE_VIP_GIFT_CLOCK,&vr))
	{
		RegistClock(IDI_VIP_GIFT_CLOCK);
		SetClock(IDI_VIP_GIFT_CLOCK, (CT_BYTE)vr.fMinVal,0,0);
	}
}

CCVipEvent::~CCVipEvent()
{

}

CT_VOID	CCVipEvent::OnTime(CT_DWORD timeID)
{
	if (timeID==IDI_VIP_TIMER)
	{
		CCUserManager::getSingleton().VipTimeMonitor();
	}
}

CT_VOID CCVipEvent::OnClock(CT_DWORD timeID)
{
	if (timeID==IDI_VIP_GIFT_CLOCK)
	{
		CCUserManager::getSingleton().VipGiftPush();
	}
}

CT_VOID CCVipEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_VIP_EVENT_CITY:
		{
			if (subID==SUB_G_VIP_RENEWALS_REQUEST)
			{
				DisposeVipRenewals(pBuf,len);
			}
			else if (subID==SUB_G_VIP_HARVEST_ONCE_REQUEST)
			{
				DisposeVipHarvestOnce(pBuf,len);
			}
			else if (subID==SUB_G_VIP_GIFT_RECEIVE_REQUEST)
			{
				DisposeVipGift(pBuf,len);
			}
			else if(subID == SUB_G_VIP_ACTIVE)
			{
				DisposeVipActive(pBuf,len);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCVipEvent::OnUserEvent : subID=%i", subID);
				return;
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::OnUserEvent : mainID=%i", mainID);
			break;
		}
	}
}

CT_VOID CCVipEvent::DisposeVipRenewals(CT_CHAR *pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_VipRenewal))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals mess len error %d",len);
		return;
	}
	CMD_G_VipRenewal *vipRenewal = (CMD_G_VipRenewal *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(vipRenewal->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,can't find user %d",vipRenewal->dwUserID);
		return;
	}
	if ((vipRenewal->iItemID<=0&&vipRenewal->iType<=0)||(vipRenewal->iItemID>0&&vipRenewal->iType>0))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,request invalid type %d item %d",vipRenewal->iType,vipRenewal->iItemID);
		pUser->CloseNet();
		return;
	}
	if (vipRenewal->iType>0)
	{
		G_VipType vipType = {0};
		if (!G_GetVipTypeInfo(vipRenewal->iType,&vipType))
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,get vip type %d fail",vipRenewal->iType);
			pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
			return;
		}
		if(pUser->m_userVip.VipGradeGet() == 4)
		{
			pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
			return;
		}
		//vip4消耗计算
		if(vipRenewal->iType == 4)
		{
			CT_INT64 lTime=pUser->m_userVip.GetVIPExpireTime()-time(0);
			lTime=(lTime<0?0:lTime);
			G_VIP4Cost(vipType.iDiamod,&vipType.iDiamod,&lTime);
			vipType.iTime=(lTime != 0?lTime:vipType.iTime);
		}

		MasterVariableData mvd = {0};
		pUser->m_userBase.GetMasterData(mvd);
		if (mvd.stRes.iDiamod+vipType.iDiamod<0)
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,diamod lack %d",vipType.iDiamod);
			pUser->CloseNet();
			return;
		}



		memset(&mvd,0,sizeof(mvd));
		mvd.stRes.iDiamod+=vipType.iDiamod;
		pUser->m_userBase.AddMastData(mvd,SAVETAG_VIP_RENEWAL);
		pUser->m_userVip.VipRenewal(vipType.iGrade,vipType.iTime,0,vipType.iDiamod);
        pUser->UpdateFactorData();
	}
	else
	{
		G_VipItem vipItem = {0};
		if (!G_GetVipItemInfo(vipRenewal->iItemID,&vipItem))
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,get vip item %d fail",vipRenewal->iItemID);
			pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
			return;
		}
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		if (!pUser->m_userItem.AutoExpendItem(threadid,vipRenewal->iItemID,1))
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,no vip item %d",vipRenewal->iItemID);
			pUser->CloseNet();
			return;
		}
		pUser->m_userVip.VipRenewal(vipItem.iGrade,vipItem.iTime,vipRenewal->iItemID,0);
        pUser->UpdateFactorData();
	}
}

CT_VOID CCVipEvent::DisposeVipHarvestOnce(CT_CHAR *pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_VipRequest))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipHarvestOnce mess len error %d",len);
		return;
	}
	CMD_G_VipRequest *vipRequest = (CMD_G_VipRequest *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(vipRequest->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipHarvestOnce,can't find user %d",vipRequest->dwUserID);
		return;
	}
	//判断该用户vip是否有该特权
	G_VipPriList  vipList={0};
	if (pUser->m_userVip.VipGradeGet()==0)
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipHarvestOnce user %d has no pri for once",vipRequest->dwUserID);
		pUser->CloseNet();
		return;
	}
	if (!G_GetVipPriList(pUser->m_userVip.VipGradeGet(),&vipList))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipHarvestOnce,get vip list %d fail",pUser->m_userVip.VipGradeGet());
		pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
		return;
	}
	//遍历vip特权list，查看是否能够一键收获
	for (int i=0;i<vipList.iPriNum;i++)
	{
		G_VipPrivilege		vipPri={0};
		if (G_GetVipPrivilege(vipList.iPriList[i],&vipPri))
		{
			if (vipPri.iPriType==VIP_PRIVILEGE_ONCE)
			{
				pUser->m_userBuilding.DisposeHarvestAutoOnce();
				return;
			}
		}
	}
	//该用户没有一键收获特权
	CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipHarvestOnce user %d has no pri for once",vipRequest->dwUserID);
	pUser->CloseNet();
	return;
}

CT_VOID CCVipEvent::DisposeVipGift(CT_CHAR *pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_VipRequest))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipGift mess len error %d",len);
		return;
	}
	CMD_G_VipRequest *vipRequest = (CMD_G_VipRequest *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(vipRequest->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipGift,can't find user %d",vipRequest->dwUserID);
		return;
	}
	if (pUser->m_userVip.VipGiftStatusGet()!=VIP_GIFT_UN_GET)
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipGift no gift");
		pUser->CloseNet();
		return;
	}
	pUser->m_userVip.VipGiftSend();
}

CT_VOID CCVipEvent::DisposeVipActive(CT_CHAR *pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_VipRenewal))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipActive mess len error %d",len);
		return;
	}

	CMD_G_VipRenewal *vipRenewal = (CMD_G_VipRenewal *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(vipRenewal->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,can't find user %d",vipRenewal->dwUserID);
		return;
	}

	//判断活动是否开启
	CT_INT32 AcviteList[100] = {0};
	CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_VIP,AcviteList);
	if(n <= 0)
		pUser->CloseNet();

	if ((vipRenewal->iItemID<=0&&vipRenewal->iType<=0)||(vipRenewal->iItemID>0&&vipRenewal->iType>0))
	{
		CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,request invalid type %d item %d",vipRenewal->iType,vipRenewal->iItemID);
		pUser->CloseNet();
		return;
	}

	if (vipRenewal->iType>0)
	{
		G_VipType vipType = {0};
		if (!G_GetVipTypeInfo(vipRenewal->iType,&vipType))
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,get vip type %d fail",vipRenewal->iType);
			pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
			return;
		}
		CT_INT32 iOldVipGrade=pUser->m_userVip.VipGradeGet();
		G_VipType vipOldType = {0};
		if (!G_GetVipTypeInfo(iOldVipGrade,&vipOldType))
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,get vip type %d fail",iOldVipGrade);
			pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
			return;
		}

		MasterVariableData mvd = {0};
		CT_INT64 lAddTime=0;
		pUser->m_userBase.GetMasterData(mvd);
		if(vipRenewal->iType != 4)
		{
			vipType.iDiamod=vipType.iDiamod-vipOldType.iDiamod;
			if (mvd.stRes.iDiamod+vipType.iDiamod<0)
			{
				CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,diamod lack %d",vipType.iDiamod);
				pUser->CloseNet();
				return;
			}

			//计算vip时间差值
			lAddTime=vipType.iTime-vipOldType.iTime;
		}
		else
		{
			lAddTime=pUser->m_userVip.GetVIPExpireTime()-time(0);
			G_VIP4Cost(vipType.iDiamod,&vipType.iDiamod,&lAddTime);
			vipType.iTime=(lAddTime != 0?lAddTime:vipType.iTime);
		}

		memset(&mvd,0,sizeof(mvd));
		mvd.stRes.iDiamod+=vipType.iDiamod;
		pUser->m_userBase.AddMastData(mvd,SAVETAG_VIP_RENEWAL);
		pUser->m_userVip.VipRenewal(vipType.iGrade,vipType.iTime,0,vipType.iDiamod);
		pUser->UpdateFactorData();
	}
	else
	{
		G_VipItem vipItem = {0};
		if (!G_GetVipItemInfo(vipRenewal->iItemID,&vipItem))
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,get vip item %d fail",vipRenewal->iItemID);
			pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
			return;
		}
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		if (!pUser->m_userItem.AutoExpendItem(threadid,vipRenewal->iItemID,1))
		{
			CCLogMeg::getSingleton().WriteError("CCVipEvent::DisposeVipRenewals,no vip item %d",vipRenewal->iItemID);
			pUser->CloseNet();
			return;
		}
		pUser->m_userVip.VipRenewal(vipItem.iGrade,vipItem.iTime,vipRenewal->iItemID,0);
		pUser->UpdateFactorData();
	}
	return;
}