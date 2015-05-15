#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CRegionEvent.h"
#include "CRegionScriptSetting.h"


CCRegionEvent::CCRegionEvent(CCGameMesManager* pMesManager, CT_DWORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_REGION_EVENT_CITY,SUB_G_REGION_UNLOCK);
}

CCRegionEvent::~CCRegionEvent(void)
{
}

CT_VOID CCRegionEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_REGION_EVENT_CITY:
		{
			if (subID==SUB_G_REGION_UNLOCK)
			{
				DisposeRegionUnlock(pBuf,len);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCRegionEvent::OnUserEvent : subID=%i", subID);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCRegionEvent::OnUserEvent : mainID=%i", mainID);
			break;
		}
	}
}

CT_VOID CCRegionEvent::DisposeRegionUnlock(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len!=sizeof(CMD_G_RegionUnlock))
	{                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
		CCLogMeg::getSingleton().WriteError("CCRegionEvent::DisposeRegionUnlock mess len error!");
		return;
	}
	CMD_G_RegionUnlock *ru = (CMD_G_RegionUnlock *)pBuf;
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(ru->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCRegionEvent::DisposeRegionUnlock,can't find user %d",ru->dwUserID);
		return;
	}
	//查看区域是否已解锁
	if (pUser->m_userRegion.GetRegionStatus(ru->iRegionID))
	{
		CMD_G_RegionUnlockFail ruf = {0};
		ENUM_RegionUnlockFail enFail = ERU_REGION_UNLOCK;
		ruf.dwUserID = ru->dwUserID;
		ruf.iRegionID = ru->iRegionID;
		ruf.enFail = enFail;
		pUser->GetNetInterface()->Send(MAIN_G_REGION_EVENT_CITY,SUB_G_REGION_UNLOCK_FAIL,(CT_CHAR*)&ruf,sizeof(ruf));
		return;
	}
	//查看区域是否存在
	/*int iRegionList[REGION_LENEX];
	memset(iRegionList,0,sizeof(int)*REGION_LENEX);*/
	G_RegionList regionList = {0};
	CCRegionScriptSetting::getSingleton().GetRegionList(&regionList);
	if (!pUser->m_userRegion.GetRegionExist(ru->iRegionID,regionList.iRegionList))
	{
		CMD_G_RegionUnlockFail ruf = {0};
		ENUM_RegionUnlockFail enFail = ERU_NO_REGION;
		ruf.dwUserID = ru->dwUserID;
		ruf.iRegionID = ru->iRegionID;
		ruf.enFail = enFail;
		pUser->GetNetInterface()->Send(MAIN_G_REGION_EVENT_CITY,SUB_G_REGION_UNLOCK_FAIL,(CT_CHAR*)&ruf,sizeof(ruf));
		return;
	}
	//查看区域邻接区域是否解锁
	/*int iRegionLink[REGION_LENEX];
	memset(iRegionLink,0,sizeof(int)*REGION_LENEX);*/
	G_RegionList regionLink = {0};
	CCRegionScriptSetting::getSingleton().GetRegionLink(ru->iRegionID,&regionLink);
	if(!pUser->m_userRegion.GetRegionLink(regionLink.iRegionList))
	{
		CMD_G_RegionUnlockFail ruf = {0};
		ENUM_RegionUnlockFail enFail = ERU_NO_LINK_REGION;
		ruf.dwUserID = ru->dwUserID;
		ruf.iRegionID = ru->iRegionID;
		ruf.enFail = enFail;
		pUser->GetNetInterface()->Send(MAIN_G_REGION_EVENT_CITY,SUB_G_REGION_UNLOCK_FAIL,(CT_CHAR*)&ruf,sizeof(ruf));
		return;
	}
	//查看领主等级是否达到
	G_RegionResource regionRe = {0};
	memset(&regionRe,0,sizeof(G_RegionResource));
	CCRegionScriptSetting::getSingleton().GetRegionResource(ru->iRegionID,&regionRe);
	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);
	if (masterData.mvd.stGrade.iLevel<regionRe.iMaterL)
	{
		CMD_G_RegionUnlockFail ruf = {0};
		ENUM_RegionUnlockFail enFail = ERU_MASTER_LEVEL;
		ruf.dwUserID = ru->dwUserID;
		ruf.iRegionID = ru->iRegionID;
		ruf.enFail = enFail;
		pUser->GetNetInterface()->Send(MAIN_G_REGION_EVENT_CITY,SUB_G_REGION_UNLOCK_FAIL,(CT_CHAR*)&ruf,sizeof(ruf));
		return;
	}
	if (ru->iReType!=regionRe.iItemID1&&ru->iReType!=regionRe.iItemID2)
	{
		CCLogMeg::getSingleton().WriteError("CCRegionEvent::DisposeRegionUnlock, Error resource type %d",ru->iReType);
		return;
	}
	//根据选择资源类型判断扣除相应的资源
	if ((ru->iReType==regionRe.iItemID1&&masterData.mvd.stRes.iGold<(-regionRe.iNum1))||(ru->iReType==regionRe.iItemID2&&masterData.mvd.stRes.iDiamod<(-regionRe.iNum2)))
	{
		CMD_G_RegionUnlockFail ruf = {0};
		ENUM_RegionUnlockFail enFail;
		if (ru->iReType==regionRe.iItemID1)
		{
			 enFail = ERU_GLOD_LACK;
		}
		else
		{
			 enFail = ERU_DIAMOD_LACK;
		}
		ruf.dwUserID = ru->dwUserID;
		ruf.iRegionID = ru->iRegionID;
		ruf.enFail = enFail;
		pUser->GetNetInterface()->Send(MAIN_G_REGION_EVENT_CITY,SUB_G_REGION_UNLOCK_FAIL,(CT_CHAR*)&ruf,sizeof(ruf));
		return;
	}
    MasterVariableData mvd = {0};
	if (ru->iReType==regionRe.iItemID1)
	{
		masterData.mvd.stRes.iGold += regionRe.iNum1;
        mvd.stRes.iGold = regionRe.iNum1;
	}
	else
	{
		masterData.mvd.stRes.iDiamod += regionRe.iNum2;
        mvd.stRes.iDiamod = regionRe.iNum2;
	}
	//pUser->m_userBase.SetMastData(masterData);
	//G_SaveMastVariableData(pUser,SAVETAG_REGION_UNLOCK_FINISH);
    pUser->m_userBase.AddMastData(mvd, SAVETAG_REGION_UNLOCK_FINISH);

	CMD_G_RegionUnlockSuc rus = {0};
	rus.dwUserID = ru->dwUserID;
	rus.iRegionID = ru->iRegionID;
	rus.iReType = ru->iReType;

	pUser->m_userBase.GetMasterData(mvd);
	if (ru->iReType==regionRe.iItemID1)
	{
		rus.iReNum = mvd.stRes.iGold;
	}
	else
	{
		rus.iReNum = mvd.stRes.iDiamod;
	}
	pUser->m_userRegion.InsertRegionUnlock(ru->iRegionID);
	pUser->GetNetInterface()->Send(MAIN_G_REGION_EVENT_CITY,SUB_G_REGION_UNLOCK_SUC,(CT_CHAR*)&rus,sizeof(rus));

	MSC_DB_W_RegionUnlockData rud = {0};
	rud.dwUserID = ru->dwUserID;
	rud.iRegionID = ru->iRegionID;
	if (ru->iReType==regionRe.iItemID1)
	{
		rud.iGold = regionRe.iNum1;
		rud.iDiamod = 0;
	}
	else
	{
		rud.iGold = 0;
		rud.iDiamod = regionRe.iNum2;
	}
	CT_DWORD threadID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_REGION_UPDATE,MSC_SUB_DB_REGION_RECORD_DATA,(CT_CHAR*)&rud,sizeof(rud));

	//数据记录
	MSC_LOGDB_RegionUnlockData UnLockData={0};
	UnLockData.dwUserID=ru->dwUserID;
	UnLockData.iUserGrade = mvd.stGrade.iLevel;
	UnLockData.iDiamod=rud.iDiamod;
	UnLockData.iGold=rud.iGold;
	UnLockData.iRegionID=ru->iRegionID;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_MASTERAREA,(CT_CHAR*)&UnLockData,sizeof(UnLockData));

	
	//解锁区域成功，增加区域任务判断
	//add by caihan 11-13
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_REGION;
	taskEventInfo.iSubType = ru->iRegionID;
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
	return;
}