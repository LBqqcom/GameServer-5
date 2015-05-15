#include "CDBCityUserBuildEvent.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CMD_Game.h"
#include "CGlobeFunc.h"
#include "CDBLogMes.h"

CCDBCityUserBuildEvent::CCDBCityUserBuildEvent(CCGameMesManager* pMesManager, CT_DWORD id,
											   CCDBConnector& dbConnector) : CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_SAVE_BUILDING, MSC_SUB_DB_SYS_BUILDING_UPGRADINT);
	RegistMessage(MSC_MAIN_DB_SAVE_BUILDING, MSC_SUB_DB_SAVE_BUILDING);
	RegistMessage(MSC_MAIN_DB_SAVE_BUILDING, MSC_SUB_DB_SAVE_UPGRADING_BUILDING);
}

CCDBCityUserBuildEvent::~CCDBCityUserBuildEvent(void)
{
}

//处理用户消息
CT_VOID CCDBCityUserBuildEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
											CT_CHAR* pBuf,	    CT_DWORD len,
											CT_LONGLONG reserve )
{
	switch (mainID)
	{
	case  MSC_MAIN_DB_SAVE_BUILDING:
		{
			DisposeBuildSaveInfo(subID, pBuf, len);
			break;
		}

	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBCityUserBuildEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}


CT_VOID CCDBCityUserBuildEvent::DisposeBuildSaveInfo(CT_DWORD subID,  CT_CHAR* pBuf,	    CT_DWORD len)
{
	switch (subID)
	{
	case MSC_SUB_DB_SYS_BUILDING_UPGRADINT:
		{
			if ( len != sizeof(MSC_DB_SysBuildingUpgrading))
			{
				G_DisposeErrorMes(0, subID, "CCCityUserBuildEvent::DisposeBuildSaveInfo: len == sizeof(MSC_DB_SysBuildingUpgrading)");
				return ;
			}                                            
			MSC_DB_SysBuildingUpgrading* pSysBuildingUpgrade = (MSC_DB_SysBuildingUpgrading*)pBuf;
			DisposeSysBuildingUpgrading(*pSysBuildingUpgrade);
			break;
		}
	case MSC_SUB_DB_SAVE_BUILDING:
		{
			if ( len != sizeof(MSC_DB_BuildingInfo))
			{
				G_DisposeErrorMes(0, subID, "CCCityUserBuildEvent::DisposeBuildSaveInfo: len == sizeof(MSC_DB_BuildingInfo)");
				return ;
			}                                            
			MSC_DB_BuildingInfo* pBuildingInfo = (MSC_DB_BuildingInfo*)pBuf;
			DisposeSaveBuildingInfo(*pBuildingInfo);
			break;
		}
	case MSC_SUB_DB_SAVE_UPGRADING_BUILDING:
		{
			if ( len != sizeof(MSC_DB_BuildingUpgradingInfo))
			{
				G_DisposeErrorMes(0, subID, "CCCityUserBuildEvent::DisposeBuildSaveInfo: len == sizeof(MSC_DB_BuildingUpgradingInfo)");
				return ;
			}                                            
			MSC_DB_BuildingUpgradingInfo* buildingUpgradingInfo = (MSC_DB_BuildingUpgradingInfo*)pBuf;
			DisposeSaveBuildingUpgradingInfo(*buildingUpgradingInfo);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBCityUserBuildEvent::DisposeBuildSaveInfo :subID=%i", subID);
			break;
		}
		
	}
}


CT_VOID CCDBCityUserBuildEvent::DisposeSysBuildingUpgrading(const MSC_DB_SysBuildingUpgrading& sysBuildingUpgrade)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BuildingUpgradingSys");
	m_dbConnector.AddParm("_uid", sysBuildingUpgrade.dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
}

CT_VOID CCDBCityUserBuildEvent::DisposeSaveBuildingInfo(const MSC_DB_BuildingInfo& buildingInfo)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BuildingSave");
	m_dbConnector.AddParm("_uid", buildingInfo.dwUserID);
	m_dbConnector.AddParm("_bid", buildingInfo.iBid);
    m_dbConnector.AddParm("_grade", buildingInfo.iGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
}


CT_VOID CCDBCityUserBuildEvent::DisposeSaveBuildingUpgradingInfo(const MSC_DB_BuildingUpgradingInfo&  bui)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BuildingUpgradingSave");
	m_dbConnector.AddParm("_uid", bui.dwUserID);
	m_dbConnector.AddParm("_bid", bui.iID);
	m_dbConnector.AddParm("_beginTime", bui.llBeginTime);
	m_dbConnector.AddParm("_lasting", bui.iNeedTime);
	m_dbConnector.AddParm("_grade",   bui.buildingUpgrade.iGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
}


