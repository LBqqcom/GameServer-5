#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBLogMes.h"
#include "CDBWorshipEvent.h"
#include <vector>

CCDBWorshipEvent::CCDBWorshipEvent(CCGameMesManager* pMesManager, CT_DWORD id, 
								   CCDBConnector& dbConnector): CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_WORSHIP_EVENT_DATA,MSC_SUB_WORSHIP_STATUS_UPDATE);
}

CCDBWorshipEvent::~CCDBWorshipEvent()
{

}

CT_VOID CCDBWorshipEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_WORSHIP_EVENT_DATA:
		{
			if (subID==MSC_SUB_WORSHIP_STATUS_UPDATE)
			{
				if (len != sizeof(MSC_DB_WorshipStatus))
				{
					G_DisposeErrorMes(0, subID, "CCDBWorshipEvent::OnUserEvent: len == sizeof(MSC_DB_WorshipStatus)");
					return ;
				}
				MSC_DB_WorshipStatus *p = (MSC_DB_WorshipStatus *)pBuf;
				UpdateWorshipStatus(*p);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBWorshipEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCDBWorshipEvent::UpdateWorshipStatus(MSC_DB_WorshipStatus &worshipStatus)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorshipStatusUpdate");
	m_dbConnector.AddParm("_uid",worshipStatus.dwUserID);
	m_dbConnector.AddParm("_type",worshipStatus.iType);
	m_dbConnector.AddParm("_status",worshipStatus.iStatus);
	m_dbConnector.AddParm("_xp",worshipStatus.iXp);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(worshipStatus.dwUserID,"SP_WorshipStatusUpdate","_uid:%d _type:%d _status:%d _xp:%d ",\
			worshipStatus.dwUserID,worshipStatus.iType,worshipStatus.iStatus,worshipStatus.iXp);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}