#include "CDBBossEvent.h"
#include "CBossManager.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "../sdk/include/CDBLogMes.h"

CCDBBossEvent::CCDBBossEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_WORLDBOSS,MSC_SUB_DB_BOSS_TOPCLEAR);
	RegistMessage(MSC_MAIN_DB_WORLDBOSS,MSC_SUB_DB_BOSS_TOPADD);
	RegistMessage(MSC_MAIN_DB_WORLDBOSS,MSC_SUB_DB_BOSS_RECORDADD);

	if (m_dwThread == GAME_THREAD_DATABASE)
	{
		RegistTimer(IDI_BOSS_READ_INFO);
		SetTimer(IDI_BOSS_READ_INFO,  TIME_BOSS_READ_TOP);
	}
}


CCDBBossEvent::~CCDBBossEvent(void)
{
}

//处理用户消息
CT_VOID CCDBBossEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
									 CT_CHAR* pBuf,		CT_DWORD len,
									 CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_WORLDBOSS)
	{
		CCLogMeg::getSingleton().WriteError("CCDBBossEvent::::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_BOSS_TOPCLEAR:
		{
			DisposeTopClear(pBuf,len);
			break;
		}
	case MSC_SUB_DB_BOSS_TOPADD:
		{
			DisposeTopUpdate(pBuf, len);
			break;
		}
	case MSC_SUB_DB_BOSS_RECORDADD:
		{
			DisposeRecordAdd(pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBBossEvent::::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBBossEvent::OnTime(CT_DWORD timeID)
{
	if (timeID == IDI_BOSS_READ_INFO)
	{
		KillTimer(IDI_BOSS_READ_INFO);
		GetBossDB();
		return;
	}
}

CT_VOID CCDBBossEvent::GetBossDB()
{
	MSC_DB_WorldBossTop dbBossTop = {0};

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BossTopGet");	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbBossTop.iAEID);
				pResult->GetValue(1, dbBossTop.iTop);
				pResult->GetValue(2, dbBossTop.dwUID);
				pResult->GetValue(3, dbBossTop.cUserName, NAME_SIZE);
				pResult->GetValue(4, dbBossTop.iHP);
				pResult->GetValue(5, dbBossTop.fHurtPercent);
				CCBossManager::getSingleton().InitBossHurt(dbBossTop);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_BossTopGet", "--");
		return;
	}
}

CT_VOID CCDBBossEvent::DisposeTopClear(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_WorldBossTopClear))
	{
		CCLogMeg::getSingleton().WriteError("CCDBBossEvent::DisposeTopClear : len != sizeof(MSC_DB_WorldBossTopClear)");
		return;
	}

	MSC_DB_WorldBossTopClear *pTopClear=(MSC_DB_WorldBossTopClear*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BossTopClear");
	m_dbConnector.AddParm("_aeid", pTopClear->iAEID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pTopClear->iAEID, "SP_BossTopClear", "_aeid:%d", pTopClear->iAEID);
		return;
	}
}

CT_VOID CCDBBossEvent::DisposeTopUpdate(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_WorldBossTop))
	{
		CCLogMeg::getSingleton().WriteError("CCDBBossEvent::DisposeTopUpdate : len != sizeof(MSC_DB_WorldBossTop)");
		return;
	}

	MSC_DB_WorldBossTop *pBossTop=(MSC_DB_WorldBossTop*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BossTopUpdate");
	m_dbConnector.AddParm("_aeid", pBossTop->iAEID);
	m_dbConnector.AddParm("_top", pBossTop->iTop);
	m_dbConnector.AddParm("_uid", pBossTop->dwUID);
	m_dbConnector.AddParm("_name", pBossTop->cUserName);
	m_dbConnector.AddParm("_hp", pBossTop->iHP);
	m_dbConnector.AddParm("_percent", pBossTop->fHurtPercent);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pBossTop->iAEID, "SP_BossTopUpdate", "_aeid:%d", pBossTop->iAEID);
		return;
	}
}

CT_VOID CCDBBossEvent::DisposeRecordAdd(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_WorldBossRecord))
	{
		CCLogMeg::getSingleton().WriteError("DisposeRecordAdd::DisposeTopUpdate : len != sizeof(MSC_DB_WorldBossRecord)");
		return;
	}

	MSC_DB_WorldBossRecord *pRecord=(MSC_DB_WorldBossRecord*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BossRecordAdd");
	m_dbConnector.AddParm("_uid", pRecord->dwUID);
	m_dbConnector.AddParm("_aid", pRecord->iAEID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pRecord->dwUID, "SP_BossRecordAdd", "_aeid:%d", pRecord->iAEID);
		return;
	}
}