#include "CUserEnterEvent.h"
#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CDBChatEvent.h"
#include "CDBConnector.h"
#include <map>

CCDBChatEvent::CCDBChatEvent(CCGameMesManager *pMesManager,CT_WORD id,CCDBConnector &dbConnector):CCGameMesEvent(pMesManager,id),m_dbConnector(dbConnector)
{
	if (id == GAME_THREAD_DATABASE)
	{
		G_ValueRange vr = {0};
		if (G_GetValueRange(VALUE_TYPE_GMMESSAGETIME,&vr))
		{
			RegistTimer(IDI_GM_MESSAGE_TIMER);
			SetTimer(IDI_GM_MESSAGE_TIMER, (CT_INT32)(vr.fMaxVal * 1000));
		}
	}
}

CCDBChatEvent::~CCDBChatEvent(void)
{
}

CT_VOID CCDBChatEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	return;
}

CT_VOID CCDBChatEvent::OnTime(CT_DWORD timeID)
{
	if(timeID == IDI_GM_MESSAGE_TIMER)
	{
		DisposeGMMessage();
	}
	return;
}

CT_VOID CCDBChatEvent::DisposeGMMessage()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BroadcastGet");
	CMD_G_GMMessage gm = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				memset(&gm,0,sizeof(gm));
				pResult->GetValue(0,gm.iMesID);
				pResult->GetValue(1,gm.cGMMes,CHAT_LEN );
     
				MSC_Broadcast_GM  broadcastGm = {0};
				broadcastGm.dwMainID = MAIN_G_CHAT_CITY;
				broadcastGm.dwSubID  = SUB_G_GM_MESSAGE;
				broadcastGm.gm       = gm;

				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST,  MSC_SUB_BROADCAST_GM, (CT_CHAR*)&broadcastGm, sizeof(broadcastGm));
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}