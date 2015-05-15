#include "CDBTop.h"
#include "CGlobeData.h"
#include "CTimerThread.h"
#include "CDBConnector.h"
#include "CGDefine.h"
#include "CGlobeFunc.h"
#include "CThreadMes.h"
#include "CFightTeamManager.h"
#include "CTop.h"

CCDBTop::CCDBTop(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	if (m_dwThread == GAME_THREAD_DATABASE)
	{
		RegistMessage(MSC_MAIN_TOP_DB_INFO, MSC_SUB_DB_TOP_GET);
		RegistMessage(MSC_MAIN_TOP_DB_INFO, MSC_SUB_DB_TOP_SAVE);
		RegistMessage(MSC_MAIN_TOP_DB_INFO, MSC_SUB_DB_TOP_REWARD);
	}
}


CCDBTop::~CCDBTop(void)
{
}
//用户定时器处理
CT_VOID CCDBTop::OnTime(CT_DWORD timeID)
{
}


//处理用户消息
CT_VOID CCDBTop::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf,	 CT_DWORD len, CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case  MSC_MAIN_TOP_DB_INFO:
		{
			DisposeMain(subID, pBuf, len);
			break;
		}
	default:
		break;
	}
}

//处理主消息 彭文添加
CT_VOID CCDBTop::DisposeMain( CT_DWORD subID, CT_CHAR* pBuf,  CT_DWORD len)
{
	switch (subID)
	{
	case  MSC_SUB_DB_TOP_GET:
		{
			DisposeTopGet();
			break;
		}
	case MSC_SUB_DB_TOP_SAVE:
		{
			DisposeTopSaveDB(pBuf, len);
			break;
		}
	case MSC_SUB_DB_TOP_REWARD:
		{
			UpdateTopRewardStatus(pBuf, len);
			break;
		}
	default:
		break;
	}
}


//更新排行榜
CT_VOID CCDBTop::DisposeTopGet()	
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TopinfoGet");

	
	if(m_dbConnector.Exec())
	{
		CT_INT32 iUserCount=0;
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				MaterInfo mi={0};
				pResult->GetValue(0, mi.dwUserID);
				pResult->GetValue(1, mi.iGrade);
				pResult->GetValue(2, mi.iExp);
				pResult->GetValue(3, mi.iRankGrade);
				pResult->GetValue(4, mi.iRankExp);
				pResult->GetValue(5, mi.iFightingPower);
				pResult->GetValue(6, mi.iCurrContWin);
				pResult->GetValue(7, mi.iTopContWin);
				pResult->GetValue(8, mi.iTopFightingPower);
				pResult->GetValue(9, mi.iTopGrade);
				pResult->GetValue(10, mi.iTopRank);
				pResult->GetValue(11, mi.iFightingFlagPower);
				pResult->GetValue(12, mi.iTopFightingFlagPower);
				pResult->GetValue(13, mi.iPlunderScore);
				pResult->GetValue(14, mi.iTopPlunder);
				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_TOP_INFO, MSC_SUB_TOP_MASTER_INFO, (CT_CHAR*)&mi, sizeof(mi) );
			}
		}
		pResult->Release();

		G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_TOP_INFO, MSC_SUB_TOP_MASTER_FINISH, 0, 0);
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	return;
}

//保存排名数据岛数据库 彭文整理
CT_VOID CCDBTop::DisposeTopSaveDB(CT_CHAR* pBuf, CT_DWORD len )	
{
	m_dbConnector.Reset();
	if(m_dbConnector.Exec(pBuf, len))
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;
	}
}

//更新排行榜奖励状态
CT_VOID CCDBTop::UpdateTopRewardStatus(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(MSC_DB_TopReward))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTop::UpdateTopRewardStatus len != sizeof(MSC_DB_TopReward)");
		return;
	}

	MSC_DB_TopReward* pReward = (MSC_DB_TopReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TopRewardUpdate");
	m_dbConnector.AddParm("_uid", pReward->dwUserID);
	m_dbConnector.AddParm("_type", pReward->iType);
	m_dbConnector.AddParm("_status", pReward->iStatus);
	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;
	}
}