#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBVipEvent.h"
#include "CDBLogMes.h"

CCDBVipEvent::CCDBVipEvent(CCGameMesManager* pMesManager, CT_DWORD id, 
						   CCDBConnector& dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_VIP_DATA,MSC_SUB_DB_GIFT_GET);
	RegistMessage(MSC_MAIN_DB_VIP_DATA,MSC_SUB_DB_VIP_RENEWAL);
	RegistMessage(MSC_MAIN_DB_VIP_DATA,MSC_SUB_DB_VIP_END);
}

CCDBVipEvent::~CCDBVipEvent()
{

}

CT_VOID CCDBVipEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_DB_VIP_DATA:
		{
			if (subID==MSC_SUB_DB_GIFT_GET)
			{
				if (len != sizeof(MSC_DB_VipGiftGet))
				{
					G_DisposeErrorMes(0, subID, "CCDBVipEvent::OnUserEvent: len == sizeof(MSC_DB_VipGiftGet)");
					return ;
				}
				MSC_DB_VipGiftGet *vgg = (MSC_DB_VipGiftGet *)pBuf;
				UpdateVipGiftGet(*vgg);
			}
			if (subID==MSC_SUB_DB_VIP_RENEWAL)
			{
				if (len != sizeof(MSC_DB_VipApply))
				{
					G_DisposeErrorMes(0, subID, "CCDBVipEvent::OnUserEvent: len == sizeof(MSC_DB_VipApply)");
					return ;
				}
				MSC_DB_VipApply *va = (MSC_DB_VipApply*)pBuf;
				UpdateVipRenewal(*va);
			}
			if (subID==MSC_SUB_DB_VIP_END)
			{
				if (len != sizeof(CT_DWORD))
				{
					G_DisposeErrorMes(0, subID, "CCDBVipEvent::OnUserEvent: len == sizeof(int)");
					return ;
				}
				CT_DWORD *id = (CT_DWORD *)pBuf;
				UpdateVipEnd(*id);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBVipEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCDBVipEvent::UpdateVipGiftGet(MSC_DB_VipGiftGet &vipGiftGet)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_VipUpdate");
	m_dbConnector.AddParm("_uid",vipGiftGet.dwUserID);
	m_dbConnector.AddParm("_bGetGift ",vipGiftGet.iGiftGet);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(vipGiftGet.dwUserID,"SP_VipGet","_uid:%d _bGetGift:%d ",vipGiftGet.dwUserID,vipGiftGet.iGiftGet);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBVipEvent::UpdateVipRenewal(MSC_DB_VipApply &vipApply)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_VipApply");
	m_dbConnector.AddParm("_uid",vipApply.dwUserID);
	m_dbConnector.AddParm("_grade ",vipApply.iGrade);
	m_dbConnector.AddParm("_endTime",vipApply.llTime);
	m_dbConnector.AddParm("_bGetGift",vipApply.iGift);
	/*m_dbConnector.AddParm("_propID",vipApply.iPropID);
	m_dbConnector.AddParm("_propNum ",vipApply.iPropNum);
	m_dbConnector.AddParm("_diamod ",vipApply.iDiamod);
	m_dbConnector.AddParm("_days",vipApply.iDays);
	m_dbConnector.AddParm("_gradeNew",vipApply.iGradeOpen);*/


	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(vipApply.dwUserID,"SP_VipApply","_uid:%d _grade:%d _endTime:%d _bGetGift:%d",\
				vipApply.dwUserID,vipApply.iGrade,vipApply.llTime,vipApply.iGift);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


CT_VOID CCDBVipEvent::UpdateVipEnd(CT_DWORD dwUserID)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_VipEnd");
	m_dbConnector.AddParm("_uid",dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_VipEnd","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}