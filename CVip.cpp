#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CMasterScriptSetting.h"
#include "CBuildingScriptSetting.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "CVip.h"
#include "GlobeScriptInterface.h"
#include "CBroadcastScriptSetting.h"

CCUserVip::CCUserVip()
{

}

CCUserVip::~CCUserVip()
{

}


//这里的参数iTime单位为天
CT_VOID CCUserVip::VipRenewal(CT_INT32 iGrade,CT_INT32 iTime,CT_INT32 iPropID,CT_INT32 iDiamod)
{
	_AUTOMUTEX(m_pMutex);
	//修改内存数据,如果vip等级发生变化，相应的特全需要修改数据
	if (iGrade>m_VipGrade)
	{
		//礼包重新发送
		m_Gift = VIP_GIFT_UN_GET;
		if (m_VipGrade==0 || iGrade == 4)
		{
			m_llExpireTime = time(0)+iTime*24*3600;
		}
		else
		{
			m_llExpireTime += iTime*24*3600;
		}
		m_VipGrade=iGrade;
		//计算领主战力
		m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
		//广播
		MasterData masterData = {0};
		RollBroadcast rb[2]={0};
		m_pUser->m_userBase.GetMasterData(masterData);
		rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
		rb[1].SetValue(iGrade, "");
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{33}<1:%d:%s>{34}<11:%d>",m_pUser->GetUserID(), masterData.cNickName,iGrade);
		G_SendRollBroadecast(33,1,1,(CT_CHAR*)rb,sizeof(rb));
	}
	else
	{
		m_llExpireTime+=iTime*24*3600;
	}
	//修改数据库信息
	MSC_DB_VipApply		dbVA = {0};
	dbVA.dwUserID = m_pUser->GetUserID();
	dbVA.iGrade = m_VipGrade;
	dbVA.llTime = m_llExpireTime;
	dbVA.iGift = m_Gift;

	CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_VIP_DATA,MSC_SUB_DB_VIP_RENEWAL,(CT_CHAR*)&dbVA,sizeof(dbVA));

	//数据记录
	MSC_LOGDB_VipApply LOGDBVipApply={0};
	LOGDBVipApply.dwUserID=m_pUser->GetUserID();
	LOGDBVipApply.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
	LOGDBVipApply.iDays=iTime;
	LOGDBVipApply.iDiamod=iDiamod;
	LOGDBVipApply.iGradeOpen=iGrade;
	LOGDBVipApply.iPropID=iPropID;
	LOGDBVipApply.iPropNum=(iPropID == 0?0:1);
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_VIPAPPLY,(CT_CHAR*)&LOGDBVipApply,sizeof(LOGDBVipApply));


	MasterVariableData mvd = {0};
	m_pUser->m_userBase.GetMasterData(mvd);
	CMD_G_VipRenewalSuc vrs = {0};
	vrs.dwUserID = m_pUser->GetUserID();   
	vrs.iDiamod = mvd.stRes.iDiamod;
	vrs.iGrade = m_VipGrade;
	vrs.iTime =  (CT_INT32)(m_llExpireTime-time(0));
	vrs.iGift = m_Gift;
	m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_RENEWALS_SUC,(CT_CHAR*)&vrs,sizeof(CMD_G_VipRenewalSuc));
}

CT_VOID CCUserVip::VipTimeMonitor()
{
	_AUTOMUTEX(m_pMutex);
	if (m_VipGrade>0&&m_llExpireTime-time(0)<=0)
	{
		m_VipGrade=0;
		m_llExpireTime=0;
		CMD_G_VipExpire ve = {0};
		ve.dwUserID = m_pUser->GetUserID();
		ve.iGrade=0;
		ve.iTime=0;
		m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_TIME_EXPIRE,(CT_CHAR*)&ve,sizeof(ve));
		//vip到期，计算领主战斗力
		m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
		m_pUser->UpdateFactorData();
		//修改数据库
		CT_DWORD dwUserID = m_pUser->GetUserID();
		CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_VIP_DATA,MSC_SUB_DB_VIP_END,(CT_CHAR*)&dwUserID,sizeof(CT_DWORD));
	}
}

CT_INT32 CCUserVip::VipGradeGet()
{
	_AUTOMUTEX(m_pMutex);
	return m_VipGrade;
}

CT_INT32 CCUserVip::VipGiftStatusGet()
{
	_AUTOMUTEX(m_pMutex);
	return m_Gift;
}

CT_VOID CCUserVip::VipGiftSend()
{
	_AUTOMUTEX(m_pMutex);
	//CT_DWORD threadid = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	if (m_pUser->m_userItem.CalcEmptySlot()<=0)
	{
		CCLogMeg::getSingleton().WriteError("CCUserVip::VipGiftSend bag full");
		m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_BAG_FULL);
		return;
	}
	G_VipType vipType = {0};
	if(!G_GetVipTypeInfo(m_VipGrade,&vipType))
	{
		CCLogMeg::getSingleton().WriteError("CCUserVip::VipGiftSend data error G_GetVipTypeInfo %d",m_VipGrade);
		m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_DATA_ERROR);
		return;
	}
	CT_INT32 iNum = 1;
	//m_pUser->m_userItem.AutoAddItemData(threadid,vipType.iGiftID,iNum);
	m_pUser->LootItems(SAVETAG_VIP_RENEWAL, &(vipType.iGiftID), &iNum);
	//修改内存数据
	m_Gift=0;
	//修改数据库信息
	MSC_DB_VipGiftGet dbVGG = {0};
	dbVGG.dwUserID = m_pUser->GetUserID();
	dbVGG.iGiftGet = 0;
	CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_VIP_DATA,MSC_SUB_DB_GIFT_GET,(CT_CHAR*)&dbVGG,sizeof(dbVGG));
	m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_GIFT_SEND_SUC);
}

CT_BOOL CCUserVip::VipHasPrivilege(int iPriID)
{
	G_VipPriList	vipPriList = {0};
	if (G_GetVipPriList(m_VipGrade,&vipPriList))
	{
		for (int i=0;i<vipPriList.iPriNum;i++)
		{
			if (iPriID==vipPriList.iPriList[i])
			{
				return CT_TRUE;
			}
		}
	}
	return CT_FALSE;
}

CT_BOOL CCUserVip::VipHasPrivilegeType(int iPriType)
{
    G_VipPriList vipPriList = {0};
    if (G_GetVipPriList(m_VipGrade,&vipPriList))
    {
        for (int i=0;i<vipPriList.iPriNum;i++)
        {
            G_VipPrivilege priType = {0};
            G_GetVipPrivilege(vipPriList.iPriList[i], &priType);
            if (priType.iPriType == iPriType){
                return CT_TRUE;
            }
        }
    }
    return CT_FALSE;
}

CT_VOID CCUserVip::VipGetDBInfo(CT_INT32 iGrade,CT_INT32 iTime,CT_INT32 iGift)
{
	_AUTOMUTEX(m_pMutex);
	m_VipGrade=iGrade;
	m_llExpireTime=iTime;
	m_Gift=iGift;
}

CT_VOID CCUserVip::VipSendInfo()
{
	_AUTOMUTEX(m_pMutex);
	CMD_G_VipInfo	vipInfo = {0};
	vipInfo.dwUserID=m_pUser->GetUserID();
	vipInfo.iGift=m_Gift;
	vipInfo.iGrade=m_VipGrade;
	vipInfo.iTime= m_VipGrade>0? (CT_INT32)(m_llExpireTime-time(0)) :0;
	m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_INFO,(CT_CHAR*)&vipInfo,sizeof(vipInfo));
	//登陆时，如果剩余时间小于阀值，发消息
	G_ValueRange vr={0};
	if (G_GetValueRange(VALUE_TYPE_VIP_EXPIRE_TIME,&vr))
	{
		if(m_VipGrade>0&&vipInfo.iTime<vr.fMaxVal)
		{
			CMD_G_VipExpire ve = {0};
			ve.dwUserID = m_pUser->GetUserID();
			ve.iGrade=m_VipGrade;
			ve.iTime=vipInfo.iTime;
			m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_TIME_EXPIRE,(CT_CHAR*)&ve,sizeof(ve));
		}
	}
}

CT_INT32 CCUserVip::VipPriTimeGet(CT_INT32 iPriType)
{
	G_VipPriList	vipPriList = {0};
	if (G_GetVipPriList(m_VipGrade,&vipPriList))
	{
		for (int i=0;i<vipPriList.iPriNum;i++)
		{
			G_VipPrivilege vp = {0};
			if (G_GetVipPrivilege(vipPriList.iPriList[i],&vp))
			{
				if (vp.iPriType==iPriType)
				{
					return int(vp.fPriProperty);
				}
			}
		}
	}
	return 0;
}

CT_BOOL CCUserVip::VipRoleAddGet(G_VipRoleAdd *vipRoleAdd)
{
	CT_BOOL bSuc=CT_FALSE;
	G_VipPriList	vipPriList = {0};
	if (G_GetVipPriList(m_VipGrade,&vipPriList))
	{
		for (int i=0;i<vipPriList.iPriNum;i++)
		{
			G_VipPrivilege vp = {0};
			if (G_GetVipPrivilege(vipPriList.iPriList[i],&vp))
			{
				if (vp.iPriType==VIP_PRIVILEGE_ROLE)
				{
					G_VipRoleAdd vra = {0};
					if (G_GetVipRoleAdd(vipPriList.iPriList[i],&vra))
					{
						vipRoleAdd->fBattleFactor+=vra.fBattleFactor;
						vipRoleAdd->fDamage+=vra.fDamage;
						vipRoleAdd->fDamageReduce+=vra.fDamageReduce;
						vipRoleAdd->fLifeCF+=vra.fLifeCF;
						vipRoleAdd->fProductFactor+=vra.fProductFactor;
						vipRoleAdd->iAgility+=vra.iAgility;
						vipRoleAdd->iIntellect+=vra.iIntellect;
						vipRoleAdd->iPriID+=vra.iPriID;
						vipRoleAdd->iSpirit+=vra.iSpirit;
						vipRoleAdd->iStrength+=vra.iStrength;
						bSuc=CT_TRUE;
					}
				}
			}
		}
	}
	return bSuc;
}

CT_VOID CCUserVip::VipGiftPush()
{
	_AUTOMUTEX(m_pMutex);
	if (m_VipGrade>0)
	{
		m_Gift = VIP_GIFT_UN_GET;
		m_pUser->GetNetInterface()->Send(MAIN_G_VIP_EVENT_CITY,SUB_G_VIP_GIFT_PUSH);
	}
}

CT_VOID CCUserVip::UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iGrade)
{
    G_VipPriList vipPriList = {0};
    if (G_GetVipPriList(iGrade, &vipPriList))
    {
        for (int i = 0; i < vipPriList.iPriNum; i++)
        {
            G_VipPrivilege vp = {0};
            if (0 == G_GetVipPrivilege(vipPriList.iPriList[i], &vp)) continue;            
            if (vp.iPriType != VIP_PRIVILEGE_ROLE) continue;
            G_VipRoleAdd vra = {0};
            if (G_GetVipRoleAdd(vipPriList.iPriList[i], &vra))
            {
                mcs.iStrength       += vra.iStrength;
                mcs.iAgility        += vra.iAgility;
                mcs.iIntellect      += vra.iIntellect;
                mcs.iSpirit         += vra.iSpirit;
                mcs.fHPPersent      += vra.fLifeCF;
                mcs.fDmgIncPercent  += vra.fDamage;
                mcs.fDmgDecPercent  += vra.fDamageReduce;
            }
        }
    }
}