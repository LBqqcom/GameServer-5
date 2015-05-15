#include <cassert>
#include "CLogDBThread.h"
#include "CDatabaseMes.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "CMesBlock.h"
#include "CTimerThread.h"
#include "CGlobeData.h"
#include "CThreadManager.h"
#include "CNetInterface.h"
#include "CGlobeFunc.h"
#include "CLogDBEvent.h"

CCLogDBThread::CCLogDBThread(void):m_pLogDBEvent(0)
{
}

CCLogDBThread::~CCLogDBThread(void)
{
	delete m_pLogDBEvent;
	m_pLogDBEvent=0;
}

//打开
CT_VOID CCLogDBThread::OnOpen()
{
	m_pLogDBEvent=G_CreateGameEvent<CCLogDBEvent>(&m_GameMesManager, m_id,  m_dbConnector);

	m_dbConnector.Init();
	CT_BOOL ret =  m_dbConnector.Connect( ((CCGameConfig&)CCConfig::getSingleton()).m_LogdbIP.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_LogdbUser.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_LogdbKey.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_LogdbName.c_str(),
		((CCGameConfig&)CCConfig::getSingleton()).m_LogdbPort
		);
	if (!ret)
	{
		G_ValueRange vr = {0};
		if(G_GetValueRange(VALUE_TYPE_DATABASERECONNECTTIME,  &vr))
		{
			CCTimerThread::getSingleton().SetTimer(IDI_DB_CONNECT_TIMER, this, (CT_INT32)(vr.fMaxVal*1000));
		}
	}

	return;
}

//定时器
CT_VOID CCLogDBThread::OnTimer(CT_DWORD timeID)
{
	if (timeID ==  IDI_DB_CONNECT_TIMER )
	{
		CT_BOOL ret =  m_dbConnector.Connect( ((CCGameConfig&)CCConfig::getSingleton()).m_LogdbIP.c_str(),
			((CCGameConfig&)CCConfig::getSingleton()).m_LogdbUser.c_str(),
			((CCGameConfig&)CCConfig::getSingleton()).m_LogdbKey.c_str(),
			((CCGameConfig&)CCConfig::getSingleton()).m_LogdbName.c_str(),
			((CCGameConfig&)CCConfig::getSingleton()).m_LogdbPort
			);
		if (!ret)
		{
			CCTimerThread::getSingleton().KillTimer(IDI_DB_CONNECT_TIMER, this);
		}
		return;
	}
	m_GameMesManager.DeliverTime(timeID);
	return;
}
