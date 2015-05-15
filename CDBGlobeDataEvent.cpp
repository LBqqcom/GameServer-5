#include "CDBGlobeDataEvent.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "../sdk/include/CDBLogMes.h"

CCDBGlobeData::CCDBGlobeData(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_GLOBEDATA, MSC_SUB_DB_GLOBEDATA);
	RegistMessage(MSC_MAIN_DB_GLOBEDATA, MSC_SUB_DB_GLOBEDATA_READ);
}

CCDBGlobeData::~CCDBGlobeData(void)
{
}

//处理用户消息
CT_VOID CCDBGlobeData::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
							  CT_CHAR* pBuf,		CT_DWORD len,
							  CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_GLOBEDATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBGlobeData::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_GLOBEDATA:
		{
			DisposeGlobeData(pBuf,len);
			break;
		}
	case MSC_SUB_DB_GLOBEDATA_READ:
		{
			//读取全局信息
			DisposeReadGlobeData();
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBGlobeData::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
			break;
		}
	}
}

//全局数据
CT_VOID CCDBGlobeData::DisposeGlobeData(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_GlobeData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCDBGlobeData::DisposeGlobeData sizeof(MSC_DB_GlobeData) != iLen");
		return;
	}

	MSC_DB_GlobeData *pGlobeData=(MSC_DB_GlobeData*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GlobeDataUpdate");

	m_dbConnector.AddParm("_type", pGlobeData->iType+1);
	m_dbConnector.AddParm("_value",pGlobeData->iValue);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_DWORD dwUserID=0;
			while (pResult->MoveNextRow())
			{
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//读取全局信息
CT_VOID CCDBGlobeData::DisposeReadGlobeData()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GlobeDataGet");

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while(pResult->MoveNextRow())
			{
				CT_INT32 iType=0;
				pResult->GetValue(0, iType);
				if(iType > 0 && iType <= GLOBEDATA_TYPE_MAX)
					pResult->GetValue(1, g_iGlobeData[iType-1]);
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}
