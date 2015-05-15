#include "ServerEvent.h"
#include "CMD_Center.h"
#include "CMD_Game.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "CTimerThread.h"
#include "CThreadManager.h"

CServerEvent::CServerEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	//ע����Ϣ
	RegistMessage(MAIN_C_GAME,SUB_C_REGISTER_FINISH);

	RegistTimer(IDI_REGISTER_GAME);
	SetTimer(IDI_REGISTER_GAME,TIME_REGISTER_GAME);
	RegistTimer(IDI_SYNCHRONOUS);
	SetTimer(IDI_SYNCHRONOUS,TIME_SYNCHRONOUS);
}

CServerEvent::~CServerEvent(void)
{
}

//�����û���Ϣ
CT_VOID CServerEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_C_GAME:
		{
			if(SUB_C_REGISTER_FINISH == subID)
			{
				ServerRegisterFinish(pBuf,len,(CCNetInterface*)reserve);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("�����¼�!  main:%d  sub:%d",mainID,subID);
			return;
		}
	}
	return;
}

//�û���ʱ������
CT_VOID CServerEvent::OnTime(CT_DWORD timeID)
{
	//ע�����
	if(IDI_REGISTER_GAME == timeID)
	{
		CMD_C_ServerType ServerType;
		memset(&ServerType,0,sizeof(ServerType));

		ServerType.cbServerType=GAME_SERVER;
		ServerType.wPort=((CCGameConfig&)CCConfig::getSingleton()).m_gamePort;
		CT_INT32 minlen = ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.size() < ROOM_NAME-1 ?  ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.size(): ROOM_NAME-1; 
		memcpy(ServerType.cServerAddr,((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.c_str(), minlen);
		minlen = ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.size() < ROOM_NAME-1 ?  ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.size() : ROOM_ADDR-1; 
		memcpy(ServerType.cServerName,((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.c_str(), minlen);

		if(g_pPCentServer != NULL)
			g_pPCentServer->Send(MAIN_C_GAME,SUB_C_REGISTER_TYPE,(CT_CHAR*)&ServerType,sizeof(ServerType));
	}

	//ͬ������
	if(IDI_SYNCHRONOUS == timeID)
	{
		CMD_C_Synchronous Synchronous;
		memset(&Synchronous,0,sizeof(Synchronous));

		Synchronous.iServerID=g_iServerID;
		Synchronous.iPlayerCount=CCUserManager::getSingleton().GetUserCount();
		Synchronous.iMaxPlayerCount=CCConfig::getSingleton().m_maxClientNum;

		if(g_pPCentServer != NULL)
			g_pPCentServer->Send(MAIN_C_GAME,SUB_C_SYNCHRONOUS,(CT_CHAR*)&Synchronous,sizeof(Synchronous));
	}
	return;
}

//����ע�����
CT_BOOL	CServerEvent::ServerRegisterFinish(CT_CHAR* pBuf,CT_DWORD len,CCNetInterface* pNetHandle)
{
	//������֤
	if(len != sizeof(CMD_C_RegisterFinish))				//ע�����
	{
		CCLogMeg::getSingleton().WriteError("ע�������֤����!");
		return false;
	}

	CMD_C_RegisterFinish *pRegisterFinish=(CMD_C_RegisterFinish*)pBuf;
	g_iServerID=pRegisterFinish->iServerID;

	//ɾ��ʱ��
	CCThread* pThread = CCThreadManager::getSingleton().GetTaskThread(m_dwThread);
	CT_ASSERT(pThread != 0);
	CCTimerThread::getSingleton().KillTimer(IDI_REGISTER_GAME,pThread);

	return true;
}
