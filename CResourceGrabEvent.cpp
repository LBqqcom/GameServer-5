/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCResourceGrabMgr.h
Version:		1.0
Description:    ��Դ����ϵͳ��Ϣ����
Author:			LinQ
Create Date:	2013-05-08
History:        ��Դ����ϵͳ����Ϣ����

******************************************************************/
#include "CResourceGrabEvent.h"
#include "../sdk/include/CLogMeg.h"
#include "CUser.h"
#include "CUserManager.h"
#include "CResourceGrabMgr.h"
#include "CFightTeam.h"
#include "CFightTeamManager.h"
#include "../sdk/include/GlobeScriptInterface.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "CThreadMes.h"
#include "../sdk/include/CNetInterface.h"
#include "CActive.h"
#include <time.h>
#include "CBroadcastScriptSetting.h"

extern CT_INT32 G_ResSpotMaxProduct;
#define MAX_BUFFER		15*1024

CCResourceGrabEvent::CCResourceGrabEvent(CCGameMesManager *pMesManager, CT_WORD id):CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_RES_GRAB, SUB_G_RESSPOT_SCENCE_APPLE);
	RegistMessage(MAIN_G_RES_GRAB, SUB_G_RESSPOT_OCCUPY_APPLY);
	RegistMessage(MAIN_G_RES_GRAB, SUB_G_RESSPOT_PRODUCT_APPLY);
	RegistMessage(MAIN_G_RES_GRAB, SUB_G_RESSPOT_DEPEND_APPLY);
	RegistMessage(MAIN_G_RES_GRAB, SUB_G_RESSPOT_SCENCE_EXIT);
	RegistMessage(MAIN_G_RES_GRAB, SUB_G_RESSPOT_GIVEUP_APPLY);
	RegistMessage(MAIN_G_RES_GRAB, SUB_G_RESSPOT_HARVEST_APPLY);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT, MSC_SUB_RESREBBERY_TEAMPVP);
	RegistMessage(MSC_MAIN_FIGHTING_EVENT, MSC_SUB_RESREBBERY_TEAMPVE);
	RegistMessage(MSC_MAIN_RES_OCCUPY,MSC_SUB_RESACTVIE_OPEN);
	RegistMessage(MSC_MAIN_RES_OCCUPY,MSC_SUB_RESACTVIE_READFINISH);
	RegistMessage(MSC_MAIN_RES_OCCUPY,MSC_SUB_INSERT_RESACTIVE);

	if (m_dwThread == GAME_THREAD_CITY)
	{
		RegistTimer(IDI_RESOURCE_SPOT_PRODUCT);
		SetTimer(IDI_RESOURCE_SPOT_PRODUCT,    TIME_RESOURCE_PRODUCT);

		RegistTimer(IDI_RESOURCE_SAVE_DB);
		SetTimer(IDI_RESOURCE_SAVE_DB, TIME_RESOURCE_SAVE_DB);

		RegistMessage( MSC_MAIN_RES_OCCUPY, MSC_SUB_SYS_EXIT);
	}

	for(CT_INT32 i=0;i<RES_ACTIVE_MAXNUM;++i)
	{
		RegistClock(IDI_RESACTIVE_OPEN+i);
	}
	RegistTimer(IDI_RESACTIVE_KEEP);
	RegistTimer(IDI_RESACTIVE_CHANGE);	

	m_ResActiveMap.clear();
}

CCResourceGrabEvent::~CCResourceGrabEvent(void)
{
}

CT_VOID CCResourceGrabEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_RES_GRAB:
		{
			DisposeResGrab(subID, pBuf, len);
			break;
		}
	case MSC_MAIN_FIGHTING_EVENT:
	case MSC_MAIN_RES_OCCUPY:
		{
			DisposeResFight(subID, pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::OnUserEvent ����Ϣ����=%i", mainID);
			break;
		}
	}

   //CMD_G_ResSpotInfo
}

//������Դ���� 
CT_VOID CCResourceGrabEvent::DisposeResGrab(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch(subID)
	{
	case SUB_G_RESSPOT_SCENCE_APPLE:
		{
			DisposeScenceApple(pBuf, len);
			break;
		}
	case SUB_G_RESSPOT_OCCUPY_APPLY:
		{
			DisposeOccupyApply(pBuf, len);
			break;
		}
	case SUB_G_RESSPOT_PRODUCT_APPLY:
		{
			DisposeSpotProductApply(pBuf, len);
			break;
		}
	case SUB_G_RESSPOT_DEPEND_APPLY:
		{
			DisposeSpotDependApply(pBuf, len);
			break;	
		}
	case  SUB_G_RESSPOT_SCENCE_EXIT:
		{
			DisposeSecneExit(pBuf, len);
			break;
		}
	case  SUB_G_RESSPOT_GIVEUP_APPLY:
		{
			DisposeSpotGiveUpApply(pBuf, len);
			break;
		}
	case SUB_G_RESSPOT_HARVEST_APPLY:
		{
			DisposeSpotHarvest(pBuf, len);
			break;
		}

	default:  
		{
			CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResGrab ����Ϣ��Ϣ����=%i", subID);
			break;
		}
	}
}


//������Դ����
CT_VOID CCResourceGrabEvent::DisposeResFight(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch(subID)
	{
	case MSC_SUB_RESREBBERY_TEAMPVP:
		{
			DisposeResFightPVP(pBuf, len);
			break;
		}
	case MSC_SUB_RESREBBERY_TEAMPVE:
		{
			DisposeResFightPVE(pBuf, len);
			break;
		}
	case MSC_SUB_RESACTVIE_OPEN:
		{
			DisposeResActiveOpen(pBuf, len);
			break;
		}
	case MSC_SUB_RESACTVIE_READFINISH:
		{
			DisposeResActiveReadFinish(pBuf, len);
			break;
		}
	case MSC_SUB_INSERT_RESACTIVE:
		{
			DisposeResInsert(pBuf,len);
			break;
		}
	case MSC_SUB_SYS_EXIT:
		{
			SaveDataToDB();
			break;
		}
	default:
		{
			break;
		}
	}
}

//�û���ʱ������
CT_VOID CCResourceGrabEvent::OnTime(CT_DWORD timeID)
{
	if (IDI_RESOURCE_SPOT_PRODUCT == timeID)
	{
		CCResourceGrabMgr::getSingleton().UpdateProduct();
	}
	else if(IDI_RESACTIVE_KEEP == timeID && m_dwThread == GAME_THREAD_CITY)
	{
		//�رջ
		KillTimer(timeID);
		KillTimer(IDI_RESACTIVE_CHANGE);
		CCResourceGrabMgr::getSingleton().ResActiveHavest();
		CCResourceGrabMgr::getSingleton().SetResActive(CT_FALSE);
		

		//���ͻ�´ο���ʱ��
		CT_INT32 AcviteList[100] = {0};
		CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_RES,AcviteList);

		CT_INT64 lTime=NextActiveTime(AcviteList);
		CCResourceGrabMgr::getSingleton().SetLastTime(lTime);

		CMD_G_ResActive ra={0};
		ra.iTime=(CT_INT32)CCResourceGrabMgr::getSingleton().GetLastTime();
		ra.iTime=(CT_INT32)(ra.iTime <= -1?-1:ra.iTime-time(0));
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_RES_GRAB,SUB_G_RESSPOT_ACTIVE_CLOSE,(CT_CHAR*)&ra,sizeof(ra));

		CCLogMeg::getSingleton().WriteError("res avtive end!");
	}else if (timeID == IDI_RESOURCE_SAVE_DB)
	{
		SaveDataToDB();
	}
	else if(IDI_RESACTIVE_CHANGE == timeID)
	{
		if(!CCResourceGrabMgr::getSingleton().GetResActive())
		{
			KillTimer(timeID);
			CCLogMeg::getSingleton().WriteError("res avtive end!");
			return;
		}

		//�����������
		CT_CHAR cBuf[MAX_BUFFER]={0};
		CT_INT32 iLen=MAX_BUFFER-sizeof(CMD_G_ResActive);
		time_t t=time(0);
		CMD_G_ResActive ra={0};
		ra.iTime=(CT_INT32)CCResourceGrabMgr::getSingleton().GetLastTime();
		ra.iTime=(CT_INT32)(ra.iTime <= -1?-1:ra.iTime-t);
		memcpy(cBuf,&ra,sizeof(ra));
		CCResourceGrabMgr::getSingleton().ResActiveHavest();
		CCResourceGrabMgr::getSingleton().RandProductType();
		CCResourceGrabMgr::getSingleton().GetResActiveSpot(cBuf+sizeof(ra),iLen);
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_RES_GRAB,SUB_G_RESSPOT_ACTIVE_OPEN,cBuf,iLen+sizeof(ra));

		CCLogMeg::getSingleton().WriteError("res adctive type change!");
	}
}

//�û�ʱ�Ӵ���
CT_VOID CCResourceGrabEvent::OnClock(CT_DWORD timeID)
{
	if(IDI_RESACTIVE_OPEN <= timeID && timeID < IDI_RESACTIVE_OPEN+RES_ACTIVE_MAXNUM && m_dwThread == GAME_THREAD_CITY)
	{
		KillClock(timeID);

		std::map<CT_INT32,CT_INT32>::iterator it=m_ResActiveMap.find(timeID);
		if(it != m_ResActiveMap.end())
		{
			G_ActiveEventData aet={0};
			G_ValueRange vr={0};
			if(G_GetActiveEvent((*it).second,&aet) && G_GetValueRange(VALUE_TYPE_RES_ACTIVE_CHANGE,&vr))
			{
				KillTimer(IDI_RESACTIVE_KEEP);
				CCResourceGrabMgr::getSingleton().ClearSpot();//�������Ϣ
				G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_RES_OCCUPY, MSC_SUB_DB_RES_ACTIVE, 0, 0);
				SetTimer(IDI_RESACTIVE_KEEP,aet.iProperty1*1000);
				SetTimer(IDI_RESACTIVE_CHANGE,vr.fMaxVal*1000);
				CCResourceGrabMgr::getSingleton().ResActiveHavest();
				CCResourceGrabMgr::getSingleton().SetResActive(CT_TRUE);
				CCResourceGrabMgr::getSingleton().SetLastTime(time(0)+aet.iProperty1);

				m_ResActiveMap.erase(it);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::OnClock G_GetActiveEvent %d",(*it).second);
				CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::OnClock G_GetValueRange VALUE_TYPE_RES_ACTIVE_CHANGE");
			}
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::OnClock it == m_ResActiveMap.end()");
		}
		
	}
}


//�����û���������
CT_VOID CCResourceGrabEvent::DisposeScenceApple(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(CMD_G_ResSenceApply) != len)
	{
		CCLogMeg::getSingleton().WriteError("sizeof(CMD_G_ResSenceApply) == len");
		return ;
	}

	CMD_G_ResSenceApply* pResApply = (CMD_G_ResSenceApply*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pResApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeScenceApple û�з����û� %i", pResApply->dwUserID);
		return ;
	}

	G_ResourceScene rs = {0};
	if (!G_GetResourceScene(pResApply->iSceneID, &rs))
	{
		CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::SysResourceProduct: û�з��������Դ���� uid=%i, senceID=%i,",  pUser->GetUserID(),  pResApply->iSceneID);
		return;
	}

	G_ResourceArea   ra = {0};
	if (!G_GetResourceArea(pResApply->iAreaID, &ra ))
	{
		CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::SysResourceProduct: û�з��������Դ�� uid=%i, iAreaID=%i,",  pUser->GetUserID(), pResApply->iAreaID );
		return;
	}

	CCResourceGrabMgr::getSingleton().EnterSence(pResApply->iSceneID, pUser);
}

//����ռ������
CT_VOID CCResourceGrabEvent::DisposeOccupyApply(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(CMD_G_ResSpotOccupyApply) != len)
	{
		CCLogMeg::getSingleton().WriteError("sizeof(CMD_G_ResSpotOccupyApply) == len");
		return ;
	}

	CMD_G_ResSpotOccupyApply* pResOccupyApply = (CMD_G_ResSpotOccupyApply*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pResOccupyApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply û�з����û� %i", pResOccupyApply->dwUserID);
		return ;
	}

	G_ResourceScene rs = {0};
	if (!G_GetResourceScene(pResOccupyApply->iScenceID, &rs))
	{
		CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::DisposeOccupyApply: û�з��������Դ���� uid=%i, senceID=%i,",  pUser->GetUserID(), pResOccupyApply->iScenceID);
		return;
	}

	G_ResourceArea   ra = {0};
	if (!G_GetResourceArea(rs.iAreaID, &ra ))
	{
		CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::DisposeOccupyApply: û�з��������Դ�� uid=%i, iAreaID=%i,",  pUser->GetUserID(), pResOccupyApply->iScenceID );
		return;
	}

	//G_ResourceSpot resSpot = {0};
	//if(!G_GetResourceSpot(pResOccupyApply->iScenceID, pResOccupyApply->iSpotID, &resSpot))
	//{
	//	CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::DisposeOccupyApply: û�з��������Դ�� uid=%i, iSenceID=%i,iSpot=%i", 
	//										pUser->GetUserID(), pResOccupyApply->iScenceID, pResOccupyApply->iSpotID);
	//	return;
	//}

	//G_ResourceQuality resQuality = {0};
	//if(!G_GetResourceQuality(resSpot.iQuality, &resQuality))
	//{
	//	CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::DisposeOccupyApply: û�з��������ԴƷ�� uid=%i, quality=%i", 
	//		pUser->GetUserID(), resQuality.iQuality);
	//	return;
	//}

	CT_INT32 iResType=CLIENT_OBJ_RES;
	G_ResourceQuality resQuality={0};
	if(!CCResourceGrabMgr::getSingleton().GetResourceQuality(pResOccupyApply->iScenceID,pResOccupyApply->iSpotID,resQuality,iResType))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply GetResourceQuality:û�з��������ԴƷ�� uid=%i, quality=%i", 
					pUser->GetUserID(), resQuality.iQuality);
		return;
	}

	
	CCFightTeam* pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pResOccupyApply->dwTeamID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply û�з���ս�� %i", pResOccupyApply->dwTeamID);
		return ;
	}

	CMD_G_ResSpotOccupyApplyLost rsoal = {0};
	rsoal.dwUserID = pResOccupyApply->dwUserID;
	rsoal.iScenceID = pResOccupyApply->iScenceID;
	rsoal.iSpotID   = pResOccupyApply->iSpotID;
	rsoal.dwTeamID   = pResOccupyApply->dwTeamID;
	if (pFightTeam->GetFightTeamBossID() != pResOccupyApply->dwUserID)
	{  
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply �����û�%i����ս��%i������ ", pResOccupyApply->dwUserID, pResOccupyApply->dwTeamID);
		rsoal.iLostCode = OCCUPY_LOST_TEAMLEADER;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
		return;
	}

#if 0 //�߻�Ҫ��ص�18���󱣻�����
	if (rs.iAreaID == 1)
	{
		FightTeamMemberUID ftUid = {0};
		CMD_G_FightTeamMember ftmember = {0};
		pFightTeam->GetFightTeamMemberUID(&ftUid);
		for (CT_INT32 iMem = 0;iMem < ftUid.dwCount;++iMem)
		{
			memset(&ftmember, 0, sizeof(ftmember));
			pFightTeam->GetFightTeamMember(1, ftUid.dwMemberUID[iMem], &ftmember);
			if (ftmember.iGrade < ra.iLevel  || ftmember.iGrade > ra.iMaxLevel)
			{
				rsoal.iLostCode = OCCUPY_LOST_LEVEL;
				pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
				return;
			}
		}
	}
	else
	{
		MasterData md = {0};
		pUser->m_userBase.GetMasterData(md);
		if (md.mvd.stGrade.iLevel < ra.iLevel  || md.mvd.stGrade.iLevel > ra.iMaxLevel)
		{
			rsoal.iLostCode = OCCUPY_LOST_LEVEL;
			pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
			return;
		}
	}
#endif
	
	CT_INT32 resSpotNum = CCResourceGrabMgr::getSingleton().GetResSpotNum(pResOccupyApply->dwTeamID);
	if ( resSpotNum >= SPOT_RES_SIZE)
	{
		rsoal.iLostCode = OCCUPY_LOST_FULLLIMIT;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
		return;
	}

	if(!pFightTeam->CheckResCountGrade(resSpotNum + 1))
	{
		rsoal.iLostCode = OCCUPY_LOST_GRADE;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
		return;
	}

	if (CCResourceGrabMgr::getSingleton().DoResSpotOccupy(pResOccupyApply->dwTeamID, pResOccupyApply->iScenceID, pResOccupyApply->iSpotID))
	{
		rsoal.iLostCode = OCCUPY_LOST_OCCUPY;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
		return;
	}
		
	ResourceBase protectBase = {0};
	CT_INT32 iFood=0;
	if (CCResourceGrabMgr::getSingleton().GetResourceBase(pResOccupyApply->iScenceID, pResOccupyApply->iSpotID, protectBase) && protectBase.dwFTID!=0 && protectBase.iLeftProtectTime >  0)
	{
		//ǿ���۶ӳ���ʯ
		CT_INT32 iDiamod=0;
		switch (pResOccupyApply->iResType)
		{
		case VALUE_TYPE_DIAMOND:
			iDiamod = (CCResourceGrabMgr::getSingleton().GetActiveResSpot(pResOccupyApply->iScenceID,pResOccupyApply->iSpotID)?ra.iFocusOccupyACTV:ra.iFocusOccupyNomal);
			iFood = (CCResourceGrabMgr::getSingleton().GetActiveResSpot(pResOccupyApply->iScenceID,pResOccupyApply->iSpotID)?0:ra.iExpend);
			break;
		case VALUE_TYPE_FOOD:
			{
				ResActive resactive={0};
				iFood = (CCResourceGrabMgr::getSingleton().GetActiveResSpot(pResOccupyApply->iScenceID,pResOccupyApply->iSpotID,resactive)?resactive.iFocusOccupyACTVNum:ra.iFocusOccupyNomalFood);
			}
			break;
		default:
			rsoal.iLostCode = OCCUPY_LOST_RESTYPE_ERR;
			pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
			return;
		}

		if (pFightTeam->GetResource() + iFood < 0)
		{
			rsoal.iLostCode = OCCUPY_LOST_RESLESS;
			pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
			return;
		}
		
		MasterVariableData mvd={0};
		pUser->m_userBase.GetMasterData(mvd);
		if(mvd.stRes.iDiamod + iDiamod < 0)
		{
				rsoal.iLostCode = OCCUPY_LOST_DIAMOD;
				pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
				return;
		}
		memset(&mvd,0,sizeof(mvd));
		mvd.stRes.iDiamod+=iDiamod;
		//mvd.stRes.iFood+=iFood;
		pUser->m_userBase.AddMastData(mvd,SAVETAG_RESOURCE_OCCUPYAPPLY);

		//if (protectBase.iLeftProtectTime > 0)
		//{
		//	rsoal.iLostCode = OCCUPY_LOST_PROTECTTIME;
		//	pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
		//	return;
		//}
	}
	else	
	{
		if(!CCResourceGrabMgr::getSingleton().GetActiveResSpot(pResOccupyApply->iScenceID,pResOccupyApply->iSpotID))
			iFood = ra.iExpend;
		if (!CCResourceGrabMgr::getSingleton().GetResActive() && pFightTeam->GetResource() + iFood < 0)
		{
			rsoal.iLostCode = OCCUPY_LOST_RESLESS;
			pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_FALSE, (CT_CHAR*)&rsoal, sizeof(rsoal) );
			return;
		}
	}

	CCFightTeam* pProtectTeam = 0;
	if (protectBase.dwFTID > 0)
	{
		if(!CCFightTeamManager::getSingleton().Find(protectBase.dwFTID, &pProtectTeam))
		{
			CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply û�з��ֱ�����Դ��ս�� %i", protectBase.dwFTID);
			protectBase.dwFTID = 0;
		}
	}
	

	//���͵�ս���߳���
	if(protectBase.dwFTID == 0) //�͹���ս��
	{
		MSC_TeamPVE_ResRobbery pve = {0};
		pve.dwUserID  = pResOccupyApply->dwUserID;
		pve.iResSpotQuality = resQuality.iQuality;
		pve.iSeed           = rand() % RAND_MAX_EX;
		pve.iFood     =  iFood;
		pve.iScenceID = pResOccupyApply->iScenceID;
		pve.iSpotID   = pResOccupyApply->iSpotID;
		pve.dwTeamID  = pResOccupyApply->dwTeamID;
		if(!pFightTeam->GetUserFightData(pve.FightingData,  SPOT_GUARD_MEMBER,CT_TRUE))
		{
			//CT_ASSERT(false);
			CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply pveû�з���ս�ӳ�Ա��ս������ fightTeam=%i",  protectBase.dwFTID );
			return;
		}

		CT_DWORD dwTheadID = GAME_THREAD_FIGHT + pve.dwUserID %  ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		G_SendThreadInfo(dwTheadID, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_RESREBBERY_TEAMPVE,(CT_CHAR*)&pve, sizeof(pve));
		
	}else //�����ս��
	{
		MSC_TeamPVP_ResRobbery pvp = {0};
		pvp.dwUserID  = pResOccupyApply->dwUserID;
		pvp.iSeed     = rand() % RAND_MAX_EX;
		pvp.iFood     = iFood;
		pvp.iScenceID = pResOccupyApply->iScenceID;
		pvp.iSpotID   = pResOccupyApply->iSpotID;
		pvp.dwTeamID  = pResOccupyApply->dwTeamID;
		pvp.bLookLeft = CT_TRUE;
		if(!pFightTeam->GetUserFightData(pvp.LeftFightingData,  SPOT_GUARD_MEMBER,CT_TRUE))
		{
			//CT_ASSERT(false);
			CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply pvpû�з���ս�ӳ�Ա��ս������ fightTeam=%i",  protectBase.dwFTID );
			return;
		}
		
		GuardTag guardTag = {0};
		CCResourceGrabMgr::getSingleton().TeamFindResSpotGuard(pResOccupyApply->iScenceID, pResOccupyApply->iSpotID, guardTag);
		CT_INT32 countUser = 0;

		for (CT_INT32 i=0; i< SPOT_GUARD_MEMBER; ++i)
		{
			if (guardTag.dwUserID[i] !=0 )
			{
				if(!pProtectTeam->GetUserFightData(guardTag.dwUserID[i], pvp.RightFightingData[countUser]))  //����ս������ 
				{
					//CT_ASSERT(false);  //��������Ҫ��ϸ����
					CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply û�з���ս�ӳ�Ա��ս������ fightTeam=%i, UID=%i",  protectBase.dwFTID, guardTag.dwUserID[countUser] );
					continue;
				}
				++countUser;
			}
		}

		if (countUser != 0)
		{
			CT_DWORD dwTheadID = GAME_THREAD_FIGHT + pvp.dwUserID %  ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
			G_SendThreadInfo(dwTheadID, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_RESREBBERY_TEAMPVP, (CT_CHAR*)&pvp, sizeof(pvp));
		}else
		{
		    //��Դ����	
			pvp.iWin = WINER_LIFT;
			DisposeResFightPVP((CT_CHAR*)&pvp, sizeof(pvp));
		}
	}
}


//��Դ����
CT_VOID CCResourceGrabEvent::DisposeSpotProductApply(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(CMD_G_ResSpotApply) != len)
	{
		CCLogMeg::getSingleton().WriteError("sizeof(CMD_G_ResSpotApply) == len");
		return ;
	}

	CMD_G_ResSpotApply* pSpotApply = (CMD_G_ResSpotApply*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pSpotApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotProductApply û�з����û� %i", pSpotApply->dwUserID);
		return ;
	}
	 
	
	CMD_G_ResSpotApplyLost  rsal = {0};
	rsal.dwUserID   = pSpotApply->dwUserID;
	rsal.iScenceID  = pSpotApply->iScenceID;
	rsal.iSpotID    = pSpotApply->iSpotID;
	
	CCFightTeam* pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pSpotApply->dwTeamID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotProductApply û�з���ս�� %i", pSpotApply->dwTeamID);
		rsal.iLostCode = SPOT_PRODUCT_LOSE_TEAM;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_PRODUCT_APPLY_LOST, (CT_CHAR*)&rsal, sizeof(rsal));
		return ;
	}

	if(pSpotApply->dwTeamID !=  CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pSpotApply->dwUserID))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotProductApply ս��%iû�з��ֳ�Ա %i", pSpotApply->dwTeamID, pSpotApply->dwUserID);
		rsal.iLostCode = SPOT_PRODUCT_LOSE_USER_NOT_IN_TEAM;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_PRODUCT_APPLY_LOST, (CT_CHAR*)&rsal, sizeof(rsal));

		return ;
	}

	if (!CCResourceGrabMgr::getSingleton().DoResSpotOccupy(pSpotApply->dwTeamID, pSpotApply->iScenceID, pSpotApply->iSpotID) )
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotProductApply ����ս��%iû��ռ����Դ��sc=%i, spot=%i", pSpotApply->dwTeamID, pSpotApply->iScenceID, pSpotApply->iSpotID);
		rsal.iLostCode = SPOT_PRODUCT_LOSE_RES_NOT_FIND;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_PRODUCT_APPLY_LOST, (CT_CHAR*)&rsal, sizeof(rsal) );
		return ;
	}

	
	CCResourceGrabMgr::getSingleton().SpotProduct(pUser, pSpotApply->iScenceID, pSpotApply->iSpotID, pSpotApply->dwTeamID);
}

CT_VOID CCResourceGrabEvent::DisposeSpotDependApply(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(CMD_G_ResSpotDepend) != len)
	{
		CCLogMeg::getSingleton().WriteError("sizeof(CMD_G_ResSpotDepend) == len");
		return ;
	}

	CMD_G_ResSpotDepend* pSpotDependApply = (CMD_G_ResSpotDepend*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pSpotDependApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DiposeSpotDependApply û�з����û� %i", pSpotDependApply->dwUserID);
		return ;
	}

	CMD_G_ResSpotDependFail rdf = {0};
	rdf.dwUserID  = pSpotDependApply->dwUserID;
	rdf.iScenceID = pSpotDependApply->iScenceID;
	rdf.iSpotID   = pSpotDependApply->iSpotID;
	rdf.dwTeamID   = pSpotDependApply->dwTeamID;
    //�鿴�û��Ƿ��������ս��
	CT_DWORD iTeamID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pSpotDependApply->dwUserID);
	if (iTeamID != pSpotDependApply->dwTeamID)
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DiposeSpotDependApply û�з���ս��%i ӵ���û� %i", pSpotDependApply->dwTeamID, pSpotDependApply->dwUserID);
		rdf.iLoseCode = SPOT_DEPEND_LOSE_USER_IN_TEAM;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_FAIL, (CT_CHAR*)&rdf, sizeof(rdf) );
		return ;
	}

	//��������
	CCFightTeam* pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pSpotDependApply->dwTeamID, &pFightTeam))
	{
		rdf.iLoseCode = SPOT_DEPEND_LOSE_TEAM;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_FAIL, (CT_CHAR*)&rdf, sizeof(rdf) );
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DiposeSpotDependApply û�з���ս�� %i", pSpotDependApply->dwTeamID);
		return ;
	}

	if (pFightTeam->GetFightTeamBossID() != pSpotDependApply->dwUserID)
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DiposeSpotDependApply �����û�%i����ս��%i������ ", pSpotDependApply->dwUserID, pSpotDependApply->dwTeamID);
		rdf.iLoseCode = SPOT_DEPEAD_LOSE_TEAMLEADER;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_FAIL, (CT_CHAR*)&rdf, sizeof(rdf) );
		return ;
	}

	//�鿴ս���ǲ����������Դ��
	if (!CCResourceGrabMgr::getSingleton().DoResSpotOccupy(pSpotDependApply->dwTeamID, pSpotDependApply->iScenceID, pSpotDependApply->iSpotID))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DiposeSpotDependApply ս��%iû��ӵ�������Դ��,iSenceID=%i, iSpot=%i", pSpotDependApply->dwTeamID, pSpotDependApply->iScenceID, pSpotDependApply->iSpotID);
		rdf.iLoseCode = SPOT_DEPEND_LOSE_SPOT;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_FAIL, (CT_CHAR*)&rdf, sizeof(rdf) );
		return ;
	}

	ResSpotTag resSpotTag = {0};
	if (0 == CCResourceGrabMgr::getSingleton().TeamFindResSpot(pSpotDependApply->dwTeamID, resSpotTag))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DiposeSpotDependApply ս��%iû��ӵ�������Դ��", pSpotDependApply->dwTeamID);
		rdf.iLoseCode = SPOT_DEPEND_LOSE_SPOT;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_FAIL, (CT_CHAR*)&rdf, sizeof(rdf) );
		return ;
	}
	//�鿴�Ƿ��Ѿ�פ����
	for(CT_INT32 i=0; i<SPOT_RES_SIZE; ++i)
	{
		GuardTag guardTag = {0};
		if(CCResourceGrabMgr::getSingleton().TeamFindResSpotGuard(resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID, guardTag))
		{
			for (CT_INT32 j=0; j<SPOT_GUARD_MEMBER; ++j)
			{
				if (  guardTag.dwUserID[j]!= 0 &&
					 (guardTag.dwUserID[j] == pSpotDependApply->dwDependPlayers[0] ||
					  guardTag.dwUserID[j] == pSpotDependApply->dwDependPlayers[1] ||
					  guardTag.dwUserID[j] == pSpotDependApply->dwDependPlayers[2] ) && 
					  G_Combine64( resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID) != G_Combine64(pSpotDependApply->iScenceID, pSpotDependApply->iSpotID)
					)
				{
					CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DiposeSpotDependApply ��Ա%i�Ѿ�פ���ڱ����Դ����", pSpotDependApply->dwUserID);
					rdf.iLoseCode = SPOT_DEPEAD_LOSE_USER_DEPEND;
					pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_FAIL, (CT_CHAR*)&rdf, sizeof(rdf) );
					return ;
				}
			}
		}
	}

	// פ���ظ�
	for (CT_INT32 i = 0; i < SPOT_GUARD_MEMBER; ++i)
	{
		for (CT_INT32 j = i+1; j < SPOT_GUARD_MEMBER; ++j)
		{
			if (pSpotDependApply->dwDependPlayers[i]>0 && pSpotDependApply->dwDependPlayers[i] == pSpotDependApply->dwDependPlayers[j])
			{
				rdf.iLoseCode = SPOT_DEPEAD_LOSE_USER_DEPEND;
				pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_FAIL, (CT_CHAR*)&rdf, sizeof(rdf) );
				return;
			}
		}
	}

	//����פ����Ա
	GuardTag gdPrimary = {0};
	CCResourceGrabMgr::getSingleton().TeamFindResSpotGuard(pSpotDependApply->iScenceID, pSpotDependApply->iSpotID, gdPrimary);

	GuardTag gd = {0};
	for (CT_INT32 i=0; i<SPOT_GUARD_MEMBER; ++i)
	{
		gd.dwUserID[i] = pSpotDependApply->dwDependPlayers[i];
	}

	//֪ͨս�ӣ���Աפ��
	pFightTeam->SetUsersGuard(gdPrimary,  gd);

	ResourceBase rBase = {0};
	if( CCResourceGrabMgr::getSingleton().GetResourceBase(pSpotDependApply->iScenceID, pSpotDependApply->iSpotID, rBase))
	{
		CCResourceGrabMgr::getSingleton().TeamSetResSpotGuard(pFightTeam, pSpotDependApply->iScenceID, pSpotDependApply->iSpotID, gd);
		memcpy(rBase.szNote, pSpotDependApply->cVictorySign, SPOT_VECTORY_SIGN_SIZE);
		rBase.szNote[SPOT_VECTORY_SIGN_SIZE - 1] = 0;
		CCResourceGrabMgr::getSingleton().SetResourceBase(pSpotDependApply->iScenceID, pSpotDependApply->iSpotID, rBase);
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_SUC, (CT_CHAR*)pSpotDependApply, sizeof(CMD_G_ResSpotDepend));
		pFightTeam->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_DEPEND_APPLY_SUC, (CT_CHAR*)pSpotDependApply, sizeof(CMD_G_ResSpotDepend));
		
		CCResourceGrabMgr::getSingleton().BroadcastSenceGuard(pSpotDependApply->iScenceID, pSpotDependApply->iSpotID, pFightTeam, pSpotDependApply->cVictorySign, pSpotDependApply->dwDependPlayers);
	}

	
}

CT_VOID CCResourceGrabEvent::DisposeSecneExit(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(CMD_G_ResScencExit) != len)
	{
		CCLogMeg::getSingleton().WriteError("sizeof(CMD_G_ResSpotDepend) == len");
		return ;
	}


	CMD_G_ResScencExit* pResSE = (CMD_G_ResScencExit*)pBuf;
	CCUser* pUser =0;
	if (!CCUserManager::getSingleton().Find(pResSE->dwUserID, &pUser))
	{
		return;
	}

	CCResourceGrabMgr::getSingleton().UserExitScence(pResSE->iScenceID, pResSE->dwUserID);
	
	pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_SCENCE_EXIT_SUC, pBuf, len);

	return;
}


CT_VOID CCResourceGrabEvent::DisposeSpotGiveUpApply(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(CMD_G_ResSpotApply) != len)
	{
		CCLogMeg::getSingleton().WriteError("sizeof(CMD_G_ResSpotApply) == len");
		return ;
	}

    CMD_G_ResSpotApply* pResGiveUp = (CMD_G_ResSpotApply*)pBuf;
	
	CCUser* pUser  = 0;
	if (!CCUserManager::getSingleton().Find(pResGiveUp->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotGiveUpApply û�з����û� %i", pResGiveUp->dwUserID);
		return ;
	}

	CMD_G_ResSpotGiveUpFaiL resSpotGiveUpFail = {0};
	resSpotGiveUpFail.dwUserID  = pResGiveUp->dwUserID;
	resSpotGiveUpFail.iScenceID = pResGiveUp->iScenceID;
	resSpotGiveUpFail.iSpotID   = pResGiveUp->iSpotID;
	//��������
	CCFightTeam* pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pResGiveUp->dwTeamID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotGiveUpApply û�з���ս�� %i", pResGiveUp->dwTeamID);
		resSpotGiveUpFail.iLoseCode = SPOT_GIVEUP_LOSE_NO_TEAM;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_GIVEUP_APPLY_FAIL, (CT_CHAR*)&resSpotGiveUpFail, sizeof(resSpotGiveUpFail) );
		return ;
	}

	if (pFightTeam->GetFightTeamBossID() != pResGiveUp->dwUserID)
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotGiveUpApply �����û�%i����ս��%i������ ", pResGiveUp->dwUserID, pResGiveUp->dwTeamID);
		resSpotGiveUpFail.iLoseCode = SPOT_GIVEUP_LOSE_TEAMLEADER;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_GIVEUP_APPLY_FAIL, (CT_CHAR*)&resSpotGiveUpFail, sizeof(resSpotGiveUpFail) );
		return ;
	}

	//�õ���Դ������Ϣ
	if (!CCResourceGrabMgr::getSingleton().DoResSpotOccupy(pResGiveUp->dwTeamID, pResGiveUp->iScenceID, pResGiveUp->iSpotID))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotGiveUpApply ����ս��%i��ӵ����Դ��iSence=%i,iSpot=%i ",
			                                 pResGiveUp->dwTeamID,
											 pResGiveUp->iScenceID,
											 pResGiveUp->iSpotID );
		resSpotGiveUpFail.iLoseCode = SPOT_GIVEUP_LOSE_SPOT;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_GIVEUP_APPLY_FAIL, (CT_CHAR*)&resSpotGiveUpFail, sizeof(resSpotGiveUpFail) );
		return;
	}

	//֪ͨս��פ����Ա��Ϣ
	GuardTag gdPrimary = {0};
	if (CCResourceGrabMgr::getSingleton().TeamFindResSpotGuard(pResGiveUp->iScenceID, pResGiveUp->iSpotID, gdPrimary))
	{
		GuardTag gd = {0};
		pFightTeam->SetUsersGuard(gdPrimary,  gd);
	}

	//�������
	CCResourceGrabMgr::getSingleton().TeamGiveUpSpot(*pResGiveUp);	
}

CT_VOID CCResourceGrabEvent::DisposeSpotHarvest(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(CMD_G_ResSpotApply) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotHarvest sizeof(CMD_G_ResSpotApply) == len");
		return ;
	}

	CMD_G_ResSpotApply* pHavestApply = (CMD_G_ResSpotApply*)pBuf;
	
	CCUser* pUser  = 0;
	if (!CCUserManager::getSingleton().Find(pHavestApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotHarvest û�з����û� %i", pHavestApply->dwUserID);
		return ;
	}

	CMD_G_ResSpotHarvestLost   resSpotHarvestFail = {0};
	resSpotHarvestFail.dwUserID  = pHavestApply->dwUserID;
	resSpotHarvestFail.iScenceID = pHavestApply->iScenceID;
	resSpotHarvestFail.iSpotID   = pHavestApply->iSpotID;
	//��������
	CCFightTeam* pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pHavestApply->dwTeamID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotHarvest û�з���ս�� %i", pHavestApply->dwTeamID);
		resSpotHarvestFail.iLostCode = SPOT_HAVEST_LOSE_NO_TEAM;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_HARVEST_APPLY_LOST, (CT_CHAR*)&resSpotHarvestFail, sizeof(resSpotHarvestFail) );
		return ;
	}

	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pHavestApply->dwUserID)  != pHavestApply->dwTeamID)
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotHarvest ����ս��%iû�г�Ա%i", pHavestApply->dwTeamID, pHavestApply->dwUserID);
		resSpotHarvestFail.iLostCode = SPOT_HAVEST_LOSE_NOUSER;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_HARVEST_APPLY_LOST, (CT_CHAR*)&resSpotHarvestFail, sizeof(resSpotHarvestFail) );
		return ;
	}

	if (!CCResourceGrabMgr::getSingleton().DoResSpotOccupy(pHavestApply->dwTeamID, pHavestApply->iScenceID, pHavestApply->iSpotID) )
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeSpotHarvest ����ս��%iû��ռ����Դ��sc=%i, spot=%i", pHavestApply->dwTeamID, pHavestApply->iScenceID, pHavestApply->iSpotID);
		resSpotHarvestFail.iLostCode = SPOT_HAVEST_LOSE_SPOT;
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_HARVEST_APPLY_LOST, (CT_CHAR*)&resSpotHarvestFail, sizeof(resSpotHarvestFail) );
		return ;
	}


	CCResourceGrabMgr::getSingleton().UserHavestSpotRes(pFightTeam, pUser, *pHavestApply);
}


CT_VOID CCResourceGrabEvent::DisposeResFightPVP(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(MSC_TeamPVP_ResRobbery) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResFightPVP sizeof(MSC_TeamPVP_ResRobbery) == len");
		return ;
	}

	MSC_TeamPVP_ResRobbery* pvp = (MSC_TeamPVP_ResRobbery*)pBuf;

	CCFightTeam* pFightTeam = 0;
	if (!CCFightTeamManager::getSingleton().Find(pvp->dwTeamID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResFightPVP û�з�����սս��%i", pvp->dwTeamID);
		return;
	}

	ResourceBase resBase = {0};
	if(!CCResourceGrabMgr::getSingleton().GetResourceBase(pvp->iScenceID, pvp->iSpotID, resBase))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResFightPVP û�з���iScenceID=%i, iSpot=%i", pvp->iScenceID, pvp->iSpotID);
		return;
	}

	CCFightTeam* pLoser = 0;
	if (!CCFightTeamManager::getSingleton().Find(resBase.dwFTID, &pLoser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResFightPVP û�з��ֱ���սս��%i", resBase.dwFTID);
		return;
	}

	G_ResourceScene rs = {0};
	if (!G_GetResourceScene(pvp->iScenceID, &rs))
	{
		return;
	}

	G_ResourceArea   ra = {0};
	if (!G_GetResourceArea(rs.iAreaID, &ra ))
	{
		return;
	}
	
	//G_ResourceSpot resSpot = {0};
	//if(!G_GetResourceSpot(pvp->iScenceID, pvp->iSpotID, &resSpot))
	//{
	//	return;
	//}

	//G_ResourceQuality resQuality= {0};
	//if(!G_GetResourceQuality(resSpot.iQuality, &resQuality))
	//{
	//	return;
	//}

	CT_INT32 iResType=CLIENT_OBJ_RES;
	G_ResourceQuality resQuality={0};
	if(!CCResourceGrabMgr::getSingleton().GetResourceQuality(pvp->iScenceID,pvp->iSpotID,resQuality,iResType))
	{
		//CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply GetResourceQuality:û�з��������ԴƷ�� uid=%i, quality=%i", 
			//pUser->GetUserID(), resQuality.iQuality);
		return;
	}

	//�۳�ս�ӵ�ʳ��
	//if(!CCResourceGrabMgr::getSingleton().GetActiveResSpot(pvp->iScenceID,pvp->iSpotID))
		pFightTeam->AddResource(pvp->iFood);

	FightTeamFightData ftf = {0};
	ftf.bPVP = CT_TRUE;
	ftf.iSeed    = pvp->iSeed;
	memcpy(ftf.userFightingLeft, pvp->LeftFightingData, sizeof(ftf.userFightingLeft));
	memcpy(ftf.userFightingRight, pvp->RightFightingData, sizeof(ftf.userFightingRight));
	ftf.dwLeftFTID = pvp->dwTeamID;
	CT_INT32 retWinIndex  = pFightTeam->ADDFightData(ftf);
	CT_INT32 retLoseIndex = pLoser->ADDFightData(ftf);
    CT_INT32 iResOccupy = 0;

	if (WINER_LIFT == pvp->iWin)
	{
		CMD_G_ResSpotOccupyChange resSpot = {0};
		resSpot.teamInfo.dwTeamID = pvp->dwTeamID;
		pFightTeam->GetFightTeamName(resSpot.teamInfo.cName);
		resSpot.iChangeReaSon = SPOT_OCCUPY_CHANGE_PLAYER;     
		resSpot.iScenceID     = pvp->iScenceID;
		resSpot.iSpotID       = pvp->iSpotID;

		//֪ͨս��פ����Ա��Ϣ
		GuardTag gdPrimary = {0};
		if (CCResourceGrabMgr::getSingleton().TeamFindResSpotGuard(pvp->iScenceID, pvp->iSpotID, gdPrimary))
		{
			GuardTag gd = {0};
			pLoser->SetUsersGuard(gdPrimary,  gd);
		}

        iResOccupy = CCResourceGrabMgr::getSingleton().ResSpotOccupy(pvp->iScenceID, pvp->iSpotID, pvp->dwTeamID);
		//����
		pFightTeam->ResReward(iResOccupy);
		DisposePvPWinEventLog(pFightTeam, pLoser, resQuality, retWinIndex, retLoseIndex, iResOccupy);

		CCResourceGrabMgr::getSingleton().BroadcastSence(resSpot.iScenceID, MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_CHANGE,  (CT_CHAR*)&resSpot, sizeof(resSpot));
		pLoser->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_CHANGE,  (CT_CHAR*)&resSpot, sizeof(resSpot));
		pFightTeam->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_CHANGE,  (CT_CHAR*)&resSpot, sizeof(resSpot));


	}else if(WINER_RIGHT ==  pvp->iWin)
	{
		CMD_G_ResSpotOccupyLost rsoas = {0};
		rsoas.dwUserID =  pvp->dwUserID;
		rsoas.iScenceID = pvp->iScenceID;
		rsoas.iSpotID   = pvp->iSpotID;
		rsoas.dwTeamID   = pvp->dwTeamID;
		DisposePvPLoseEventLog(pFightTeam, pLoser, resQuality, retLoseIndex, retWinIndex );
		pFightTeam->Broadcast( MAIN_G_RES_GRAB, SUB_G_RESSPOT_OCCUPY_LOST, (CT_CHAR*)&rsoas, sizeof(rsoas));
	}

	DisposeResOccupyRecord(pvp->dwTeamID, pvp->iScenceID, pvp->iSpotID, pvp->iFood, iResOccupy);
	SendOccupySuc(pFightTeam, pvp->dwUserID, pvp->iScenceID, pvp->iSpotID);

}

//������Դռ���¼
CT_VOID CCResourceGrabEvent::DisposeResOccupyRecord(CT_DWORD dwTeamID, CT_INT32 iScenceID, CT_INT32 iSpotID, CT_INT32 iFood, CT_INT32 iOccupyGold)
{
	MSC_DB_ResourceOccupyRecord resOccupyRecord = {0};
	resOccupyRecord.dwFID       = dwTeamID;
	resOccupyRecord.iScenceID   = iScenceID;
	resOccupyRecord.iSpotID     = iSpotID;
	resOccupyRecord.iFTeamRes   = iFood;
	resOccupyRecord.iOccupyGold = iOccupyGold;
	CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + dwTeamID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_LOGDB_DATA, MSC_SUB_DBLOG_RES_OCCUPY_SAVE, (CT_CHAR*)&resOccupyRecord, sizeof(resOccupyRecord));
}



CT_VOID CCResourceGrabEvent::DisposePvPWinEventLog(CCFightTeam* pChanllageTeam,  CCFightTeam* pFightTeam, G_ResourceQuality& resQuality,  
												CT_INT32 iWinIndex, CT_INT32 iLoseIndex,  CT_INT32 iResOccupy)
{
	CMD_G_FightTeamEventLog eventLog = {0};
	eventLog.lDate   = time(0);
	eventLog.iReport = iWinIndex;
	CT_CHAR buf[DESCRIPTION_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	RollBroadcast rb[3]={0};

	CT_CHAR winBuf[NAME_SIZE] = {0};
	CT_CHAR loseBuf[NAME_SIZE] = {0};
	pChanllageTeam->GetFightTeamName(winBuf);
	pFightTeam->GetFightTeamName(loseBuf);

	rb[0].SetValue(0,loseBuf);
	rb[1].SetValue(0,resQuality.cName);
	rb[2].SetValue(iResOccupy,"");

	//sprintf(buf, "{11}<%s>{8}%s{9}%i{10}", loseBuf, resQuality.cName, iResOccupy);
	G_GetRollBroadecastData(106,buf,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
	memcpy(eventLog.cEventDesc, buf, DESCRIPTION_SIZE-1);
	pChanllageTeam->AddEventLog(eventLog);
	

	iEventDescLen=DESCRIPTION_SIZE;
	memset(rb,0,sizeof(rb));
	memset(buf,0,sizeof(buf));
	rb[0].SetValue(0,winBuf);
	rb[1].SetValue(0,resQuality.cName);
	rb[2].SetValue(iResOccupy,"");
	//sprintf(buf, "{12}<%s>{13}%s, {14}%i{10}", winBuf, resQuality.cName,  iResOccupy);
	G_GetRollBroadecastData(107,buf,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
	eventLog.iReport = iLoseIndex;	
	memcpy(eventLog.cEventDesc, buf, DESCRIPTION_SIZE-1);
	pFightTeam->AddEventLog(eventLog);
}

CT_VOID CCResourceGrabEvent::DisposePvPLoseEventLog(CCFightTeam* pChanllageTeam,  CCFightTeam* pFightTeam,   G_ResourceQuality& resQuality,  
													CT_INT32 iLoseIndex, CT_INT32  iWinIndex)
{
	CMD_G_FightTeamEventLog eventLog = {0};
	eventLog.lDate   = time(0);
	eventLog.iReport = iWinIndex;
	CT_CHAR buf[DESCRIPTION_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	RollBroadcast rb[2]={0};

	CT_CHAR winBuf[NAME_SIZE] = {0};
	CT_CHAR loseBuf[NAME_SIZE] = {0};

	pChanllageTeam->GetFightTeamName(winBuf);
	pFightTeam->GetFightTeamName(loseBuf);
	rb[0].SetValue(0,loseBuf);
	rb[1].SetValue(0,resQuality.cName);


	//sprintf(buf, "{15}<%s>{8}%s{16}", loseBuf, resQuality.cName);
	G_GetRollBroadecastData(108,buf,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
	memcpy(eventLog.cEventDesc, buf, DESCRIPTION_SIZE-1);
	pChanllageTeam->AddEventLog(eventLog);

	iEventDescLen=DESCRIPTION_SIZE;
	memset(rb,0,sizeof(rb));
	memset(buf,0,sizeof(buf));
	rb[0].SetValue(0,winBuf);
	rb[1].SetValue(0,resQuality.cName);
	//sprintf(buf, "{17}<%s>{18}%s{19}{20}", winBuf, resQuality.cName);
	G_GetRollBroadecastData(109,buf,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
	eventLog.iReport = iLoseIndex;
	memcpy(eventLog.cEventDesc, buf, DESCRIPTION_SIZE-1);
	pFightTeam->AddEventLog(eventLog);
}

CT_VOID CCResourceGrabEvent::DisposePvEWinEventLog(CCFightTeam* pChanllageTeam, G_ResourceArea& ra, G_ResourceQuality& resQuality,  
												   CT_INT32 iFightIndex)
{
	CMD_G_FightTeamEventLog eventLog = {0};
	eventLog.lDate   = time(0);
	eventLog.iReport = iFightIndex;
	CT_CHAR buf[DESCRIPTION_SIZE] = {0};
	CT_CHAR winBuf[NAME_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	RollBroadcast rb[2]={0};
	pChanllageTeam->GetFightTeamName(winBuf);
	rb[0].SetValue(0,ra.cName);
	rb[1].SetValue(0,resQuality.cName);

	//sprintf(buf, "{21}<%s>{8}%s", ra.cName, resQuality.cName);
	G_GetRollBroadecastData(110,buf,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
	memcpy(eventLog.cEventDesc, buf, DESCRIPTION_SIZE-1);
	pChanllageTeam->AddEventLog(eventLog);
}

CT_VOID CCResourceGrabEvent::DisposePvELoseEventLog(CCFightTeam* pChanllageTeam, G_ResourceArea& ra, G_ResourceQuality& resQuality,  
												   CT_INT32 iFightIndex)
{
	CMD_G_FightTeamEventLog eventLog = {0};
	eventLog.lDate   = time(0);
	eventLog.iReport = iFightIndex;
	CT_CHAR buf[DESCRIPTION_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	CT_CHAR winBuf[NAME_SIZE] = {0};
	RollBroadcast rb[2]={0};
	pChanllageTeam->GetFightTeamName(winBuf);
	rb[0].SetValue(0,ra.cName);
	rb[1].SetValue(0,resQuality.cName);

	//sprintf(buf, "{22}<%s>{8}%s{23}", ra.cName, resQuality.cName);
	G_GetRollBroadecastData(111,buf,iEventDescLen,(CT_CHAR*)rb,sizeof(rb));
	memcpy(eventLog.cEventDesc, buf, DESCRIPTION_SIZE-1);
	pChanllageTeam->AddEventLog(eventLog);
}



CT_VOID CCResourceGrabEvent::DisposeResFightPVE(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(MSC_TeamPVE_ResRobbery) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResFightPVE sizeof(MSC_TeamPVE_ResRobbery) == len");
		return ;
	}

	MSC_TeamPVE_ResRobbery* pve = (MSC_TeamPVE_ResRobbery*)pBuf;

	CCFightTeam* pFightTeam = 0;
	if (!CCFightTeamManager::getSingleton().Find(pve->dwTeamID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResFightPVE û�з���ս��%i", pve->dwTeamID);
		return;
	}

	G_ResourceScene rs = {0};
	if (!G_GetResourceScene(pve->iScenceID, &rs))
	{
		return;
	}

	G_ResourceArea   ra = {0};
	if (!G_GetResourceArea(rs.iAreaID, &ra ))
	{
		return;
	}

	//G_ResourceSpot resSpot = {0};
	//if(!G_GetResourceSpot(pve->iScenceID, pve->iSpotID, &resSpot))
	//{
	//	return;
	//}

	//G_ResourceQuality resQuality = {0};
	//if(!G_GetResourceQuality(resSpot.iQuality, &resQuality))
	//{
	//	return;
	//}

	CT_INT32 iResType=CLIENT_OBJ_RES;
	G_ResourceQuality resQuality={0};
	if(!CCResourceGrabMgr::getSingleton().GetResourceQuality(pve->iScenceID,pve->iSpotID,resQuality,iResType))
	{
		//CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeOccupyApply GetResourceQuality:û�з��������ԴƷ�� uid=%i, quality=%i", 
		//pUser->GetUserID(), resQuality.iQuality);
		return;
	}

	//�۳�ս�ӵ�ʳ��
	if(!CCResourceGrabMgr::getSingleton().GetActiveResSpot(pve->iScenceID,pve->iSpotID))
		pFightTeam->AddResource(pve->iFood);

	FightTeamFightData ftf = {0};
	ftf.bPVP = CT_FALSE;
	//ftf.dwUserID = pve->dwUserID;
	ftf.iSeed    = pve->iSeed;
	memcpy(ftf.userFightingLeft, pve->FightingData, sizeof(ftf.userFightingLeft));
	ftf.iSpotQuality    = pve->iResSpotQuality;
	ftf.dwLeftFTID = pve->dwTeamID;
	CT_INT32 retIndex   = pFightTeam->ADDFightData(ftf);
	CT_INT32 iResOccupy = 0;

	if (WINER_LIFT ==   pve->iWin )
	{
		CMD_G_ResSpotOccupyChange resSpot = {0};
		resSpot.teamInfo.dwTeamID = pve->dwTeamID;
		pFightTeam->GetFightTeamName(resSpot.teamInfo.cName);

		resSpot.iChangeReaSon = SPOT_OCCUPY_CHANGE_PLAYER;     
		resSpot.iScenceID     = pve->iScenceID;
		resSpot.iSpotID       = pve->iSpotID;

		iResOccupy = CCResourceGrabMgr::getSingleton().ResSpotOccupy(pve->iScenceID, pve->iSpotID, pve->dwTeamID);
		DisposePvEWinEventLog(pFightTeam, ra, resQuality, retIndex);
		CCResourceGrabMgr::getSingleton().BroadcastSence(resSpot.iScenceID, MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_CHANGE,  (CT_CHAR*)&resSpot, sizeof(resSpot));
		pFightTeam->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_CHANGE,  (CT_CHAR*)&resSpot, sizeof(resSpot));

	}else if(WINER_RIGHT == pve->iWin )
	{
		CMD_G_ResSpotOccupyLost rsoas = {0};
		rsoas.dwUserID   =  pve->dwUserID;
		rsoas.iScenceID  =  pve->iScenceID;
		rsoas.iSpotID    =  pve->iSpotID;
		rsoas.dwTeamID   =  pve->dwTeamID;

		DisposePvELoseEventLog(pFightTeam, ra, resQuality, retIndex);
		pFightTeam->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_LOST,  (CT_CHAR*)&rsoas, sizeof(rsoas));
	}

	DisposeResOccupyRecord(pve->dwTeamID, pve->iScenceID, pve->iSpotID, pve->iFood, iResOccupy);
	SendOccupySuc(pFightTeam, pve->dwUserID, pve->iScenceID, pve->iSpotID);
}

//��Դ������
CT_VOID CCResourceGrabEvent::DisposeResActiveOpen(CT_CHAR* pBuf, CT_DWORD len)
{
	time_t t=time(0);
	tm *ptm=localtime(&t);
	CT_INT32 AcviteList[100] = {0};
	CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_RES,AcviteList);
	if(n > 0)
	{
		m_ResActiveMap.clear();

		//ʱ��ת��
		tm Temptm=(*ptm);

		//�����
		CT_INT32 iActiveNum=0;
		CT_INT64 lMinTime=0;
		for(CT_INT32 i=0;i<100;++i)
		{
			if(AcviteList[i] != 0)
			{
				G_ActiveEventData aet={0};
				if(G_GetActiveEvent(AcviteList[i],&aet))
				{
					CT_INT32 iTimeDay=(CT_INT32)(aet.iProperty2/(3600*24));
					if(ptm->tm_wday == iTimeDay)
					{
						CT_INT32 iProperty2=(CT_INT32)aet.iProperty2;
						CT_INT32 iHour=iProperty2%(3600*24)/3600;
						CT_INT32 iMin=iProperty2%(3600*24)%3600/60;
						CT_INT32 iClockID=IDI_RESACTIVE_OPEN+iActiveNum;

						if(iHour <= ptm->tm_hour && iMin < ptm->tm_min || iHour < ptm->tm_hour)continue;

						SetClock(iClockID,iHour,iMin,0);
						
						std::map<CT_INT32,CT_INT32>::iterator it=m_ResActiveMap.find(iClockID);
						if(it != m_ResActiveMap.end())
						{
							m_ResActiveMap.erase(it);
							m_ResActiveMap.insert(std::map<CT_INT32,CT_INT32>::value_type(iClockID,AcviteList[i]));
						}
						else
							m_ResActiveMap.insert(std::map<CT_INT32,CT_INT32>::value_type(iClockID,AcviteList[i]));

						Temptm.tm_hour = iHour;
						Temptm.tm_min = iMin;
						Temptm.tm_sec = 0;

						if((lMinTime > mktime(&Temptm) && mktime(&Temptm) != -1) || lMinTime == 0)
						{
							lMinTime=mktime(&Temptm);
						}

						iActiveNum++;
					}
				}
				else
				{
					CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::DisposeResActiveOpen G_GetActiveEvent %d",AcviteList[i]);
				}
			}
		}

		if(lMinTime != 0)
			CCResourceGrabMgr::getSingleton().SetLastTime(lMinTime);
		else
		{
			CT_INT64 lTime=NextActiveTime(AcviteList);
			CCResourceGrabMgr::getSingleton().SetLastTime(lTime);
		}

		//���ͻ�´ο���ʱ��
		CMD_G_ResActive ra={0};
		ra.iTime=(CT_INT32)CCResourceGrabMgr::getSingleton().GetLastTime();
		ra.iTime=(CT_INT32)(ra.iTime <= -1?-1:ra.iTime-t);
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_RES_GRAB,SUB_G_RESSPOT_ACTIVE_CLOSE,(CT_CHAR*)&ra,sizeof(ra));
	}
	return;
}

//��Դ���ȡ���
CT_VOID CCResourceGrabEvent::DisposeResInsert(CT_CHAR* pBuf, CT_DWORD len)
{
	//�������Ϣ
	CCResourceGrabMgr::getSingleton().InsterResActive(pBuf, len);
	return;
}

//��Դ���ȡ���
CT_VOID CCResourceGrabEvent::DisposeResActiveReadFinish(CT_CHAR* pBuf, CT_DWORD len)
{
	//���ͻ��Դ��
	CT_CHAR cBuf[MAX_BUFFER]={0};
	CT_INT32 iLen=MAX_BUFFER-sizeof(CMD_G_ResActive);
	time_t t=time(0);
	CMD_G_ResActive ra={0};
	ra.iTime=(CT_INT32)CCResourceGrabMgr::getSingleton().GetLastTime();
	ra.iTime=(CT_INT32)(ra.iTime <= -1?-1:ra.iTime-t);
	memcpy(cBuf,&ra,sizeof(ra));
	CCResourceGrabMgr::getSingleton().RandProductType();
	CCResourceGrabMgr::getSingleton().GetResActiveSpot(cBuf+sizeof(ra),iLen);
	CCUserManager::getSingleton().SendUserMessage(MAIN_G_RES_GRAB,SUB_G_RESSPOT_ACTIVE_OPEN,cBuf,iLen+sizeof(ra));

	CCLogMeg::getSingleton().WriteError("send res active keep time sec:%d",ra.iTime);
	CCLogMeg::getSingleton().WriteError("max spot:%d current spot:%d",MAX_BUFFER/sizeof(ResActive),iLen/sizeof(ResActive));

	return;
}

//���͹�������
CT_VOID CCResourceGrabEvent::SendOccupySuc(CCFightTeam* pTeam, CT_DWORD dwUserID, CT_INT32 iScenceID, CT_INT32 iSpotID)
{
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(dwUserID, &pUser) )
	{
		return;
	}
	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);
	//��ռ����ɹ�
	CMD_G_ResSpotOccupyApplySuc rsoas = {0};
	rsoas.dwUserID  =  dwUserID;
	rsoas.iFood     =  pTeam->GetResource();
	rsoas.iScenceID =  iScenceID;
	rsoas.iSpotID   =  iSpotID;
	rsoas.dwTeamID  =  pTeam->GetFightTeamID();   
	rsoas.iDiamod	=  mvd.stRes.iDiamod;
	rsoas.iMFood	= mvd.stRes.iFood;
	pUser->GetNetInterface()->Send( MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_APPLY_SUC,  (CT_CHAR*)&rsoas, sizeof(rsoas));
	pTeam->Broadcast(MAIN_G_RES_GRAB, SUB_G_RESSPOT_OCCUPY_APPLY_SUC_POST, (CT_CHAR*)&rsoas, sizeof(rsoas), dwUserID);
}

//����ռ������
CT_VOID CCResourceGrabEvent::SetOccupyData(CT_INT32 iScenceID, CT_INT32 iSpotID, CT_DWORD dwTeamID, CT_INT32 iProtectTime)
{
	CCResourceGrabMgr::getSingleton().ResetResourceInfo(iScenceID, iSpotID);
	ResourceBase rb     = {0};
	rb.dwFTID           = dwTeamID;
	rb.iLeftOccupyTime  = G_ResSpotMaxProduct;
	rb.iLeftProtectTime = iProtectTime;
	CCResourceGrabMgr::getSingleton().SetResourceBase(iScenceID, iSpotID, rb);
}

//�����´λʱ��
CT_INT64 CCResourceGrabEvent::NextActiveTime(CT_INT32 AcviteList[100])
{
	time_t t=time(0);
	tm *ptm=localtime(&t);
	CT_INT64 lMinTime=0;

	//����ʱ��֮�����
	for(CT_INT32 i=0;i<100;++i)
	{
		if(AcviteList[i] != 0)
		{
			G_ActiveEventData aet={0};
			if(G_GetActiveEvent(AcviteList[i],&aet))
			{
				CT_INT32 iTimeDay=(CT_INT32)(aet.iProperty2/(3600*24));
				if(ptm->tm_wday <= iTimeDay)
				{
					CT_INT32 iProperty2=(CT_INT32)aet.iProperty2;
					CT_INT32 iHour=iProperty2%(3600*24)/3600;
					CT_INT32 iMin=iProperty2%(3600*24)%3600/60;

					if((iHour <= ptm->tm_hour && iMin < ptm->tm_min || iHour < ptm->tm_hour) && ptm->tm_wday >= iTimeDay)continue;

					if((lMinTime > iProperty2) || lMinTime == 0)
					{
						lMinTime=iProperty2;
					}
				}
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::NextActiveTime G_GetActiveEvent %d",AcviteList[i]);
			}
		}

		if(lMinTime != 0)
		{
			CT_INT32 iTimeDay=(CT_INT32)(lMinTime/(3600*24));
			CT_INT32 iHour=lMinTime%(3600*24)/3600;
			CT_INT32 iMin=lMinTime%(3600*24)%3600/60;
			lMinTime=t+(iTimeDay-ptm->tm_wday)*24*3600+(iHour-ptm->tm_hour)*3600+(iMin-ptm->tm_min)*60;
			return lMinTime;
		}

	}

	//�������쿪ʼ����
	if(lMinTime == 0)
	{
		for(CT_INT32 i=0;i<100;++i)
		{
			if(AcviteList[i] != 0)
			{
				G_ActiveEventData aet={0};
				if(G_GetActiveEvent(AcviteList[i],&aet))
				{
					CT_INT32 iTimeDay=(CT_INT32)(aet.iProperty2/(3600*24));
					if(ptm->tm_wday > iTimeDay)
					{
						CT_INT32 iProperty2=(CT_INT32)aet.iProperty2;
						if((lMinTime > iProperty2) || lMinTime == 0)
						{
							lMinTime=iProperty2; 
						}
					}
				}
				else
				{
					CCLogMeg::getSingleton().WriteError("CCResourceGrabEvent::NextActiveTime G_GetActiveEvent %d",AcviteList[i]);
				}
			}
		}

		if(lMinTime != 0)
		{
			CT_INT32 iTimeDay=(CT_INT32)(lMinTime/(3600*24));
			CT_INT32 iHour=lMinTime%(3600*24)/3600;
			CT_INT32 iMin=lMinTime%(3600*24)%3600/60;
			lMinTime=t+(7-(ptm->tm_wday-iTimeDay))*24*3600+(iHour-ptm->tm_hour)*3600+(iMin-ptm->tm_min)*60;
			return lMinTime;
		}
	}

	return -1;
}

//��������
CT_VOID CCResourceGrabEvent::SaveDataToDB()
{
	std::string info;
	CCResourceGrabMgr::getSingleton().GetAllInfoToSave(info);
	G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_RES_OCCUPY, MSC_SUB_DB_RES_SAVE, info.c_str(), info.size());
}
