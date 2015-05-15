#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "GlobeScriptInterface.h"
#include "CUserWorship.h"

#define  WORSHIP_MES_BUFF_LEN		(1024*5)

CCUserWorship::CCUserWorship()
{

}

CCUserWorship::~CCUserWorship()
{

}

CT_BOOL	CCUserWorship::CheckAbilityToWorship(CT_INT32 iType)
{
	WorshipStatusMap_IT	it = m_WorshipStatusMap.find(iType);
	if (it==m_WorshipStatusMap.end()||it->second.iStatus==WORSHIP_COMPLETED)
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}


CT_VOID CCUserWorship::UpdateWorshipStatus(CT_INT32 iType,CT_INT32 iXp)
{
	_AUTOMUTEX(m_pMutex);
	G_WorshipType  worshipType = {0};
	if (!G_GetWorshipType(iType,&worshipType))
	{
		CCLogMeg::getSingleton().WriteError("CCUserWorship::UpdateWorshipStatus G_GetWorshipType error %d",iType);
		m_pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_DATA_ERROR);
		return;
	}
	m_WorshipStatusMap[iType].iXP += iXp;
	if (m_WorshipStatusMap[iType].iXP>=worshipType.iXpUp)
	{
		m_WorshipStatusMap[iType].iXP = worshipType.iXpUp;
		m_WorshipStatusMap[iType].iStatus = WORSHIP_COMPLETED;
	}
	CMD_G_WorshipStatusUpdate  wsu = {0};
	CMD_G_WorshipCountUpdate	wcu = {0};
	if (iType > 100)
	{
		wcu.iType=iType;
		wcu.iCount=m_WorshipStatusMap[iType].iXP;
		m_pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_COUNT_UPDATE,(CT_CHAR*)&wcu,sizeof(CMD_G_WorshipCountUpdate));
	}
	else
	{
		wsu.iType=iType;
		wsu.iXp=m_WorshipStatusMap[iType].iXP;
		wsu.iStatus=m_WorshipStatusMap[iType].iStatus;
		m_pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_STATUS_UPDATE,(CT_CHAR*)&wsu,sizeof(CMD_G_WorshipStatusUpdate));
	}

	
	//修改数据库信息
	CT_DWORD DBthreadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	MSC_DB_WorshipStatus  worshipStatus={0};
	worshipStatus.dwUserID=m_pUser->GetUserID();
	worshipStatus.iType=iType;
	worshipStatus.iStatus=wsu.iStatus;
	worshipStatus.iXp=wsu.iXp;
	G_SendThreadInfo(DBthreadid,MSC_MAIN_WORSHIP_EVENT_DATA,MSC_SUB_WORSHIP_STATUS_UPDATE,(CT_CHAR*)&worshipStatus,sizeof(worshipStatus));
	
	//完成祭祀，添加祭祀任务
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_WORSHIP;
	taskEventInfo.iSubType = iType;
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	m_pUser->m_userTask.DisposeTask(taskEventInfo);
}

CT_BOOL CCUserWorship::CheckWorshipComplete(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	if (m_WorshipStatusMap[iType].iStatus==WORSHIP_COMPLETED)
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_VOID CCUserWorship::WorshipGetReward(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	m_WorshipStatusMap[iType].iStatus=WORSHIP_UNCOMPLETE;
	m_WorshipStatusMap[iType].iXP=0;
}

CT_VOID CCUserWorship::GetWorshipByMasterLevel(CT_INT32 iLevel)
{
	_AUTOMUTEX(m_pMutex);
	G_WorshipList	worshipList = {0};
	G_GetWorshipList(iLevel,&worshipList);
	if (worshipList.iNum<=0)
	{
		return;
	}
	for (CT_INT32 i=0;i<worshipList.iNum;i++)
	{
		WorshipStatusMap_IT it = m_WorshipStatusMap.find(worshipList.iWorshipList[i]);
		if (it==m_WorshipStatusMap.end())
		{
			//添加新的祭祀类型
			WorshipStatus	status = {0};
			m_WorshipStatusMap[worshipList.iWorshipList[i]]=status;
			/*m_WorshipStatusMap[worshipList.iWorshipList[i]].iStatus=WORSHIP_UNCOMPLETE;
			m_WorshipStatusMap[worshipList.iWorshipList[i]].iXP=0;*/
			//发送客户端
			CMD_G_WorshipStatusUpdate  wsu = {0};
			CMD_G_WorshipCountUpdate  wcu = {0};
			wsu.iType=worshipList.iWorshipList[i];
			wcu.iType=worshipList.iWorshipList[i];
			//发送客户端消息
			if (worshipList.iWorshipList[i] > 100)
			{
				m_pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_COUNT_UPDATE,(CT_CHAR*)&wcu,sizeof(CMD_G_WorshipCountUpdate));
			}
			else
			{
				m_pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_STATUS_UPDATE,(CT_CHAR*)&wsu,sizeof(CMD_G_WorshipStatusUpdate));
			}
			//修改数据库信息
			CT_DWORD DBthreadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			MSC_DB_WorshipStatus  worshipStatus={0};
			worshipStatus.dwUserID=m_pUser->GetUserID();
			worshipStatus.iType=worshipList.iWorshipList[i];
			G_SendThreadInfo(DBthreadid,MSC_MAIN_WORSHIP_EVENT_DATA,MSC_SUB_WORSHIP_STATUS_UPDATE,(CT_CHAR*)&worshipStatus,sizeof(worshipStatus));
		}
	}
}

CT_VOID CCUserWorship::GetWorshipStatus(CT_INT32 iType,CT_INT32 iStatus,CT_INT32 iXp)
{
	_AUTOMUTEX(m_pMutex);
	WorshipStatus	status = {0};
	status.iStatus=iStatus;
	status.iXP=iXp;
	m_WorshipStatusMap[iType]=status;
}

CT_VOID CCUserWorship::SendUserWorshipInfo()
{
	_AUTOMUTEX(m_pMutex);

	if (m_WorshipStatusMap.size()>0)
	{
		CT_CHAR buf[WORSHIP_MES_BUFF_LEN] = {0};
		CT_CHAR buf2[WORSHIP_MES_BUFF_LEN] = {0};	//钻石祭祀
		CT_INT32 iLen=0;
		CT_INT32 iLen2=0;
		WorshipStatusMap_IT it = m_WorshipStatusMap.begin();
		while(it!=m_WorshipStatusMap.end())
		{
			CMD_G_WorshipStatusUpdate  wsu = {0};
			CMD_G_WorshipCountUpdate  wcu = {0};
			if (it->first > 100)		//钻石祭祀
			{
				wcu.iType = it->first;
				wcu.iCount = it->second.iXP;
				if(WORSHIP_MES_BUFF_LEN > iLen2+sizeof(CMD_G_WorshipCountUpdate))
				{
					memcpy(buf2+iLen2,&wcu,sizeof(CMD_G_WorshipCountUpdate));
					iLen2+=sizeof(CMD_G_WorshipCountUpdate);
				}
				else
					CCLogMeg::getSingleton().WriteError("CCUserWorship::SendUserWorshipInfo WORSHIP_MES_BUFF_LEN > iLen2+sizeof(CMD_G_WorshipCountUpdate)");
			}
			else
			{
				wsu.iType = it->first;
				wsu.iStatus=it->second.iStatus;
				wsu.iXp = it->second.iXP;
				if(WORSHIP_MES_BUFF_LEN > iLen+sizeof(CMD_G_WorshipStatusUpdate))
				{
					memcpy(buf+iLen,&wsu,sizeof(CMD_G_WorshipStatusUpdate));
					iLen+=sizeof(CMD_G_WorshipStatusUpdate);
				}
				else
					CCLogMeg::getSingleton().WriteError("CCUserWorship::SendUserWorshipInfo WORSHIP_MES_BUFF_LEN > iLen+sizeof(CMD_G_WorshipStatusUpdate)");
			}
			it++;
		}
		if (iLen > 0)
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_STATUS_UPDATE,buf,iLen);
		}
		if (iLen2 > 0)
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_WORSHIP_EVENT_CITY,SUB_G_WORSHIP_COUNT_UPDATE,buf2,iLen2);	//钻石祭祀
		}		
	}
}

CT_BOOL CCUserWorship::CheckWorshipCount(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	G_ValueRange vr={0};
	G_GetValueRange(VALUE_TYPE_WORSHIP_TIME,&vr);
	if (m_WorshipStatusMap[iType].iXP < m_pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_WORSHIP) + vr.fMaxVal)
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_VOID CCUserWorship::WorshipCount(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	m_WorshipStatusMap[iType].iXP += 1;
}

CT_INT32 CCUserWorship::GetWorshipCount(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	return m_WorshipStatusMap[iType].iXP;
}

CT_VOID CCUserWorship::UpdateWorshipData()
{
	_AUTOMUTEX(m_pMutex);
	WorshipStatusMap_IT it = m_WorshipStatusMap.begin();
	while(it!=m_WorshipStatusMap.end())
	{
		if (it->first > 100)
		{
			it->second.iXP = 0;
		}
		++it;
	}
	SendUserWorshipInfo();
}