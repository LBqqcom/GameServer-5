#include "CDBArmy.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "../sdk/include/CDBLogMes.h"

CCDBArmy::CCDBArmy(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
					CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_ARMY_DATA,MSC_SUB_DB_R_ARMY_UPGRADE);
	RegistMessage(MSC_MAIN_DB_ARMY_DATA,MSC_SUB_DB_R_ARMY_ADVANCED);
}

CCDBArmy::~CCDBArmy(void)
{
}

//处理用户消息
CT_VOID CCDBArmy::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
							CT_CHAR* pBuf,		CT_DWORD len,
							CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_ARMY_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBArmy::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_R_ARMY_UPGRADE:				//军队升级
		{
			DisposeArmyGrade(pBuf,len);
			break;
		}
	case MSC_SUB_DB_R_ARMY_ADVANCED:			//军队进阶
		{
			DisposeArmyAdvanced(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBArmy::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBArmy::OnTime(CT_DWORD timeID)
{
	return;
}

//军队升级消息
CT_VOID CCDBArmy::DisposeArmyGrade(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_R_ArmyGrade))
	{
		CCLogMeg::getSingleton().WriteError("CCDBArmy::DisposeArmyGrade : len != sizeof(MSC_DB_R_ArmyGrade)");
		return;
	}

	MSC_DB_R_ArmyGrade *pArmyGrade=(MSC_DB_R_ArmyGrade*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyUpdateGrade");

	m_dbConnector.AddParm("_uid", pArmyGrade->dwUserID);
	m_dbConnector.AddParm("_aid", pArmyGrade->iArmyID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pArmyGrade->dwUserID, "SP_ArmyUpdateGrade", "_uid:%d, _aid:%d", pArmyGrade->dwUserID, pArmyGrade->iArmyID );
		return;
	}
	return;
}

//军队进阶消息
CT_VOID CCDBArmy::DisposeArmyAdvanced(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_R_ArmyAdvanced))
	{
		CCLogMeg::getSingleton().WriteError("CCDBArmy::DisposeArmyGrade : len != sizeof(MSC_DB_R_ArmyGrade)");
		return;
	}

	MSC_DB_R_ArmyAdvanced *pArmyAdvanced=(MSC_DB_R_ArmyAdvanced*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyUpdateRank");

	m_dbConnector.AddParm("_uid", pArmyAdvanced->dwUserID);
	m_dbConnector.AddParm("_aidold", pArmyAdvanced->iOldArmyID);
	m_dbConnector.AddParm("_aidnew", pArmyAdvanced->iNewArmyID);
	m_dbConnector.AddParm("_grade", pArmyAdvanced->iArmyGrade);


	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pArmyAdvanced->dwUserID, "SP_ArmyUpdateRank", "_uid:%d, _aidold:%d, _aidnew:%d ", pArmyAdvanced->dwUserID,pArmyAdvanced->iOldArmyID,pArmyAdvanced->iNewArmyID );
		return;
	}

	return;
}
