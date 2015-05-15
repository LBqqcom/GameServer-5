#include <cstdlib>
#include <time.h>
#include "CFightingThread.h"
#include "CGlobeFunc.h"
#include "CMD_Game.h"
#include "CDatabaseMes.h"
#include "CFightingEvnet.h"

CCFightingThread::CCFightingThread(void)
{
}

CCFightingThread::~CCFightingThread(void)
{
}
//´ò¿ª
CT_VOID CCFightingThread::OnOpen()
{
	m_pFightingEvent   = G_CreateGameEvent<CCFightingEvnet1>(&m_GameMesManager, m_id) ;
}

