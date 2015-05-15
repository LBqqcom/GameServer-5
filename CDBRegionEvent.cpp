#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBRegionEvent.h"
#include "CDBLogMes.h"

CCDBRegionData::CCDBRegionData(CCGameMesManager* pMesManager, CT_DWORD id, 
							   CCDBConnector& dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_REGION_UPDATE,MSC_SUB_DB_REGION_RECORD_DATA);
}

CCDBRegionData::~CCDBRegionData(void)
{
}

CT_VOID CCDBRegionData::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_DB_REGION_UPDATE:
		if (subID==MSC_SUB_DB_REGION_RECORD_DATA)
		{
			if (len != sizeof(MSC_DB_W_RegionUnlockData))
			{
				G_DisposeErrorMes(0, subID, "CCDBRegionData::OnUserEvent: len == sizeof(MSC_DB_W_RegionUnlockData)");
				return ;
			}
			MSC_DB_W_RegionUnlockData *pRegionUnlockData = (MSC_DB_W_RegionUnlockData *)pBuf;
			DisposeRegionDataSave(*pRegionUnlockData);
		}
		break;
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBRegionData::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCDBRegionData::DisposeRegionDataSave(const MSC_DB_W_RegionUnlockData& regionUnlockData)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MasterAddArea");
	m_dbConnector.AddParm("_uid",regionUnlockData.dwUserID);
	m_dbConnector.AddParm("_aid",regionUnlockData.iRegionID);
	m_dbConnector.AddParm("_money",regionUnlockData.iGold);
	m_dbConnector.AddParm("_diamod",regionUnlockData.iDiamod);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(regionUnlockData.dwUserID,"SP_MasterAddArea","_uid:%d _aid:%d _money:%d _diamod:%d",regionUnlockData.dwUserID,regionUnlockData.iRegionID,regionUnlockData.iGold,regionUnlockData.iDiamod);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}