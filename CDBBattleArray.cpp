#include "CDBBattleArray.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "CDBLogMes.h"

CCDBBattleArray::CCDBBattleArray(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
									CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_BATTLE_ARRAY_DATA,MSC_SUB_DB_W_BATTLE_ARRAY_INFO);
}

CCDBBattleArray::~CCDBBattleArray(void)
{
}

//处理用户消息
CT_VOID CCDBBattleArray::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
							CT_CHAR* pBuf,		CT_DWORD len,
							CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_BATTLE_ARRAY_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBBattleArray::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_W_BATTLE_ARRAY_INFO:
		{
			DisposeBattleArray(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBBattleArray::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBBattleArray::OnTime(CT_DWORD timeID)
{
	return;
}

//阵法信息
CT_VOID CCDBBattleArray::DisposeBattleArray(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_BattleArray))
	{
		CCLogMeg::getSingleton().WriteError("CCDBArmy::DisposeBattleArray : len != sizeof(MSC_DB_BattleArray)");
		return;
	}

	MSC_DB_BattleArray *pBattleArray=(MSC_DB_BattleArray*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyUpdatePos");

	m_dbConnector.AddParm("_uid",  (CT_INT32)pBattleArray->dwUserID);
	m_dbConnector.AddParm("_pos0", (CT_INT32)pBattleArray->iPosition[0]);
	m_dbConnector.AddParm("_pos1", (CT_INT32)pBattleArray->iPosition[1]);
	m_dbConnector.AddParm("_pos2", (CT_INT32)pBattleArray->iPosition[2]);
	m_dbConnector.AddParm("_pos3", (CT_INT32)pBattleArray->iPosition[3]);
	m_dbConnector.AddParm("_pos4", (CT_INT32)pBattleArray->iPosition[4]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo((CT_INT32)pBattleArray->dwUserID,"SP_ArmyUpdatePos","_uid:%d _pos0:%d _pos1:%d _pos2:%d _pos3:%d _pos4:%d",\
					pBattleArray->dwUserID,pBattleArray->iPosition[0],pBattleArray->iPosition[1],pBattleArray->iPosition[2],pBattleArray->iPosition[3],pBattleArray->iPosition[4]);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}
