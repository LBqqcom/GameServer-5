#include "CDBBuildFunEvent.h"
#include "CDatabaseMes.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "CDBLogMes.h"

CCDBBuildFunEvent::CCDBBuildFunEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector ) : CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_PRODUCTING_DATA);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_SYS_PROPDUCTING);
	
}

CCDBBuildFunEvent::~CCDBBuildFunEvent(void)
{
}


//处理用户消息
CT_VOID CCDBBuildFunEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf,   CT_DWORD len, CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case  MSC_MAIN_DB_SAVE_DATA:
		{
			DisposeSaveData(subID, pBuf, len);
			break;
		}
	default: 
		{
			break;
		}
	}
}

//保存数据
CT_VOID CCDBBuildFunEvent::DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,   CT_DWORD len)
{
	switch (subID)
	{
	case  MSC_SUB_DB_PRODUCTING_DATA:
		{
            if (len != sizeof(MSC_DB_SaveProductingData))
            {
				CCLogMeg::getSingleton().WriteError(" CCDBBuildFunEvent::DisposeSaveData : len != sizeof(MSC_DB_SaveProductingData ");
				return ;
            }
			
			MSC_DB_SaveProductingData* pData = (MSC_DB_SaveProductingData*)pBuf;
			DisposeSaveProductingData(*pData);
			break;
		}
	case  MSC_SUB_DB_SYS_PROPDUCTING:
		{
			if (len != sizeof(MSC_DB_SysProductingData))
			{
				CCLogMeg::getSingleton().WriteError(" CCDBBuildFunEvent::DisposeSaveData : len != sizeof(MSC_DB_SysProductingData ");
				return ;
			}

			MSC_DB_SysProductingData* pData = (MSC_DB_SysProductingData*)pBuf;
			DisposeSysProductingData(*pData);
			break;
		}

	default:
		{
			break;
		}
	}
}


//同步生产的建筑
CT_VOID CCDBBuildFunEvent::DisposeSysProductingData(const MSC_DB_SysProductingData& sysProductingData)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ProductingBuildingSys");
	m_dbConnector.AddParm("_uid",			sysProductingData.dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}

	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(sysProductingData.dwUserID,"SP_ProductingBuildingSys","_uid:%d", sysProductingData.dwUserID);
		G_DBError(m_dbConnector, m_dwThread);  
	}
}


//保存生产数据
CT_VOID CCDBBuildFunEvent::DisposeSaveProductingData(const MSC_DB_SaveProductingData& data)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ProductingBuildingSave");
	m_dbConnector.AddParm("_uid",			data.dwUserID );
	m_dbConnector.AddParm("_cid",			data.productingBuilding.iBType );
	m_dbConnector.AddParm("_bid",           data.productingBuilding.iBid);
	m_dbConnector.AddParm("_beginTime",		data.productingBuilding.llTime );
	m_dbConnector.AddParm("_type",          data.productingBuilding.iType);
	m_dbConnector.AddParm("_sendCanHavest", data.productingBuilding.bSendCanHavest);
	m_dbConnector.AddParm("_lastingTime",   data.productingBuilding.iLastingTime );
	m_dbConnector.AddParm("_workers",		data.productingBuilding.iWorker );
	m_dbConnector.AddParm("_bGrade",        data.productingBuilding.iBGrade);
	m_dbConnector.AddParm("_diamod",        data.productingBuilding.iDiamod);
	m_dbConnector.AddParm("_SeekAddition",  data.productingBuilding.fSeekAddition);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		if (pResult)
		{
			pResult->Release();
		}
		
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(data.dwUserID,"SP_SaveProductingBuilding","_uid:%d _cid:%d _bid:%d _beginTime:%d _type:%d _sendCanHavest:%d _lastingTime:%d _workers:%d _bGrade:%d",\
				data.dwUserID,data.productingBuilding.iBType,data.productingBuilding.iBid,data.productingBuilding.iLastingTime,data.productingBuilding.iType,data.productingBuilding.bSendCanHavest,\
				data.productingBuilding.iLastingTime,data.productingBuilding.iWorker,data.productingBuilding.iBGrade);
		G_DBError(m_dbConnector, m_dwThread);  
	}
}
