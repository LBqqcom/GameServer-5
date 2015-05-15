#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBTaskEvent.h"
#include "CTask.h"
#include "CDBLogMes.h"

CCDBTaskEvent::CCDBTaskEvent(CCGameMesManager *pMesManager, CT_DWORD id,
							 CCDBConnector &dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_DATA);
	RegistMessage(MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_RECORD_DATA);
}

CCDBTaskEvent::~CCDBTaskEvent()
{
	
}

CT_VOID CCDBTaskEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_DB_TASK_UPDATE:
		{
			if (subID==MSC_SUB_DB_TASK_DATA)
			{
				if (len != sizeof(MSC_DB_TaskProgressData))
				{
					G_DisposeErrorMes(0, subID, "CCDBTaskEvent::OnUserEvent: len == sizeof(MSC_DB_TaskProgressData)");
					return ;
				}
				MSC_DB_TaskProgressData *tpd = (MSC_DB_TaskProgressData *)pBuf;
				UpdateTaskProgressData(*tpd);
			}
			if (subID==MSC_SUB_DB_TASK_RECORD_DATA)
			{
				if (len != sizeof(TaskFinish))
				{
					G_DisposeErrorMes(0, subID, "CCDBTaskEvent::OnUserEvent: len == sizeof(TaskFinish)");
					return ;
				}
				TaskFinish *tr = (TaskFinish*)pBuf;
				UpdateTaskFinishData(*tr);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBTaskEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCDBTaskEvent::UpdateTaskProgressData(MSC_DB_TaskProgressData &taskProgressData)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TaskUpdate");
	m_dbConnector.AddParm("_uid",taskProgressData.dwUserID);
	m_dbConnector.AddParm("_mid",taskProgressData.iTaskID);
	m_dbConnector.AddParm("_sid",taskProgressData.iSubTaskID);
	m_dbConnector.AddParm("_num",taskProgressData.iTime);
	m_dbConnector.AddParm("_bFinish",taskProgressData.iCompleted);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(taskProgressData.dwUserID,"SP_TaskUpdate","_uid:%d _mid:%d _sid:%d _num:%d _bFinish:%d",\
								taskProgressData.dwUserID,taskProgressData.iTaskID,taskProgressData.iSubTaskID,taskProgressData.iTime,taskProgressData.iCompleted);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBTaskEvent::UpdateTaskFinishData(TaskFinish &taskReward)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TaskFinishAdd");
	m_dbConnector.AddParm("_uid",taskReward.dwUserID);
	m_dbConnector.AddParm("_taskmid",taskReward.iTaskID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(taskReward.dwUserID,"SP_TaskFinishAdd","_uid:%i _taskmid:%d",
																taskReward.dwUserID,taskReward.iTaskID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TaskFinish");
	m_dbConnector.AddParm("_uid",taskReward.dwUserID);
	m_dbConnector.AddParm("_mid",taskReward.iTaskID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(taskReward.dwUserID,"SP_TaskFinish","_uid:%d _mid:%d",
			taskReward.dwUserID,taskReward.iTaskID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}