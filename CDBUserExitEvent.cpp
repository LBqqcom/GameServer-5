#include "CDBUserExitEvent.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CDBConnector.h"
#include "CDBLogMes.h"


CCDBUserExitEvent::CCDBUserExitEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector) :
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_USER_EXIT,      0);
}

CCDBUserExitEvent::~CCDBUserExitEvent(void)
{
}


//处理用户消息
CT_VOID CCDBUserExitEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
										CT_CHAR* pBuf, CT_DWORD len,
										CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case  MSC_MAIN_DB_USER_EXIT:
		{
			DisposeUserExitEvent(subID, pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBUserExitEvent::OnUserEvent find wrong mainID: %i", mainID);
			break;
		}
	}
}



//处理退出消息
CT_VOID CCDBUserExitEvent::DisposeUserExitEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{

	if (len != sizeof(MSC_DB_UserExit))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserExitEvent::DisposeUserExitEvent: Len is wrong: len == sizeof(MSC_DB_UserExit)");
		return ;
	}

	MSC_DB_UserExit* pUserExit = (MSC_DB_UserExit*) pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UserExit");
	m_dbConnector.AddParm("_uid",     pUserExit->dwUserID );
	m_dbConnector.AddParm("_timeNow", pUserExit->llTimeNow);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();

	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pUserExit->dwUserID,"SP_UserExit","_uid:%d _timeNow:%d",pUserExit->dwUserID,pUserExit->llTimeNow);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

