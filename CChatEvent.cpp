#include "CUserEnterEvent.h"
#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CChatEvent.h"
#include <map>

CChatEvent::CChatEvent(CCGameMesManager *pMesManager,CT_DWORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_CHAT_CITY,SUB_G_PRIVATE_CHAT);
	RegistMessage(MAIN_G_CHAT_CITY,SUB_G_WORLD_CHAT);
	RegistMessage(MAIN_G_CHAT_CITY,SUB_G_FIGHTTEAM_CHAT);

	if (GAME_THREAD_CITY == id  )
	{
		G_ValueRange vr = {0};
		if(G_GetValueRange(VALUE_TYPE_ONLINE_STATISTIC_TIME,  &vr))
		{
			RegistTimer(IDI_ONLINE_STATIC_TIMER);
			SetTimer(IDI_ONLINE_STATIC_TIMER, (CT_INT32)(vr.fMaxVal * 1000));
		}
	}

}

CChatEvent::~CChatEvent(void)
{
}

CT_VOID CChatEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
								CT_CHAR* pBuf,		CT_DWORD len,
								CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MAIN_G_CHAT_CITY: 
		{
			DisposeChatEvent(subID,pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CChatEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
	return;
}

CT_VOID CChatEvent::OnTime(CT_DWORD timeID)
{
	if (timeID==IDI_ONLINE_STATIC_TIMER)
	{
		CCUserManager::getSingleton().GetOnlineUserNum();
	}
}

CT_VOID CChatEvent::DisposeChatEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case SUB_G_PRIVATE_CHAT:
		{
			ForwardingPrivateChat(pBuf,len);
			break;
		}
	case SUB_G_WORLD_CHAT:
		{
			ForwardingWorldChat(pBuf,len);
			break;
		}
	case SUB_G_FIGHTTEAM_CHAT:
		{
			ForwardingFightTeamChat(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CChatEvent::DisposeChatEvent : subID=%i", subID);
			break;
		}
	}
	return;
}

CT_BOOL CChatEvent::ForwardingPrivateChat(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len>sizeof(CMD_G_PrivateChat))
	{
		CCLogMeg::getSingleton().WriteError("CChatEvent::ForwardingPrivateChat mess len %d",len);
		return CT_FALSE;
	}
	CMD_G_PrivateChat *pPrivateChat = (CMD_G_PrivateChat *)pBuf;
	if (len > sizeof(CMD_G_PrivateChat))
	{
		return CT_FALSE;
	}
	CCUser *pMeUser=0;
	if(!CCUserManager::getSingleton().Find(pPrivateChat->dwUserID,&pMeUser))
	{
		CCLogMeg::getSingleton().WriteError("CChatEvent::ForwardingPrivateChat No Find User %d",pPrivateChat->dwUserID);
		return CT_FALSE;
	}

	//用户禁言
	if(pMeUser->GetTalk())
	{
		G_ServerChat ServerChat={0};
		if(!G_GetServerChat(SERVER_CHAT_TALK,&ServerChat))
		{
			CCLogMeg::getSingleton().WriteError("CChatEvent::ForwardingWorldChat ");
		}
		CMD_G_GMMessage GmMessage={0};
		memcpy(GmMessage.cGMMes,ServerChat.cChat,sizeof(ServerChat.cChat));
		CCUserManager::getSingleton().SendServerMessage(GmMessage,sizeof(GmMessage),pPrivateChat->dwUserID);		
		return CT_TRUE;
	}


	CCUser *pUser= NULL;
	if (pPrivateChat->dwAimID==0)
	{
		if(!CCUserManager::getSingleton().Find(pPrivateChat->cAimNickName,&pUser))
		{
			CMD_G_PrivateChatFail pcf = {0};
			pcf.dwAimID = pPrivateChat->dwAimID;
			strncpy(pcf.cAimNickName,pPrivateChat->cAimNickName,NAME_SIZE);
			CCUserManager::getSingleton().Find(pPrivateChat->dwUserID, &pUser);
			pUser->GetNetInterface()->Send(MAIN_G_CHAT_CITY,SUB_G_PRIVATE_CHAT_FAIL,(CT_CHAR*)&pcf,sizeof(pcf));
			return CT_FALSE;
		}
		pPrivateChat->dwAimID = pUser->GetUserID();
	}
	else
	{
		if(!CCUserManager::getSingleton().Find(pPrivateChat->dwAimID, &pUser))
		{
			CMD_G_PrivateChatFail pcf = {0};
			pcf.dwAimID = pPrivateChat->dwAimID;
			strncpy(pcf.cAimNickName,pPrivateChat->cAimNickName,NAME_SIZE);
			CCUserManager::getSingleton().Find(pPrivateChat->dwUserID, &pUser);
			pUser->GetNetInterface()->Send(MAIN_G_CHAT_CITY,SUB_G_PRIVATE_CHAT_FAIL,(CT_CHAR*)&pcf,sizeof(pcf));
			return CT_FALSE;
		}
	}

	//检查黑名单
	if (pMeUser->m_userFriend.CheckBlackList(pPrivateChat->dwAimID))
	{
		CMD_G_PrivateChatFail pcf = {0};
		pcf.dwAimID = pPrivateChat->dwAimID;
		strncpy(pcf.cAimNickName,pPrivateChat->cAimNickName,NAME_SIZE);
		pMeUser->GetNetInterface()->Send(MAIN_G_CHAT_CITY,SUB_G_PRIVATE_CHAT_FAIL,(CT_CHAR*)&pcf,sizeof(pcf));
		return CT_FALSE;
	}

	//增加用户聊天次数
	pMeUser->AddChatTime();
	CCNetInterface* pNetHandle=pUser->GetNetInterface();
	if (CCUserManager::getSingleton().IsGM(pPrivateChat->dwUserID))
		pNetHandle->Send(MAIN_G_CHAT_CITY,SUB_G_GMPRIVATE_CHAT,pBuf, len);
	else
		pNetHandle->Send(MAIN_G_CHAT_CITY,SUB_G_PRIVATE_CHAT,pBuf, len);
	return CT_TRUE;
}

CT_VOID CChatEvent::ForwardingWorldChat(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len>sizeof(CMD_G_WorldChat))
	{
		CCLogMeg::getSingleton().WriteError("CChatEvent::ForwardingWorldChat mess len %d",len);
		return ;
	}
	CMD_G_WorldChat *pWorldChat = (CMD_G_WorldChat *)pBuf;
	if (len > sizeof(CMD_G_WorldChat))
	{
		return;
	}

	//禁言判断
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pWorldChat->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CChatEvent::ForwardingWorldChat No Find User %d",pWorldChat->dwUserID);
		return;
	}
	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(!pUser->GetTalk())
	{
		pUser->AddChatTime();
		CCUserManager::getSingleton().SendWorldChat(pWorldChat,len);
	}
	else
	{
// 		G_ServerChat ServerChat={0};
// 		if(!G_GetServerChat(SERVER_CHAT_TALK,&ServerChat))
// 		{
// 			CCLogMeg::getSingleton().WriteError("CChatEvent::ForwardingWorldChat ");
// 		}
// 		CMD_G_GMMessage GmMessage={0};
// 		memcpy(GmMessage.cGMMes,ServerChat.cChat,sizeof(ServerChat.cChat));
// 		CCUserManager::getSingleton().SendServerMessage(GmMessage,sizeof(GmMessage),pWorldChat->dwUserID);

	}
	return;
}

CT_VOID CChatEvent::ForwardingFightTeamChat(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len>sizeof(CMD_G_FightTeamChat))
	{
		CCLogMeg::getSingleton().WriteError("ForwardingFightTeamChat mess len %d",len);
		return ;
	}
	CMD_G_FightTeamChat *pFightTeamChat = (CMD_G_FightTeamChat *)pBuf;
	if (len > sizeof(CMD_G_FightTeamChat))
	{
		return;
	}
	//禁言判断
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightTeamChat->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("ForwardingFightTeamChat No Find User %d",pFightTeamChat->dwUserID);
		return;
	}
	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(!pUser->GetTalk())
	{
		pUser->AddChatTime();
		CCFightTeamManager::getSingleton().SendFightTeamChat(pFightTeamChat,len);
	}
	else
	{
		G_ServerChat ServerChat={0};
		if(!G_GetServerChat(SERVER_CHAT_TALK,&ServerChat))
		{
			CCLogMeg::getSingleton().WriteError("CChatEvent::ForwardingWorldChat ");
		}
		CMD_G_GMMessage GmMessage={0};
		memcpy(GmMessage.cGMMes,ServerChat.cChat,sizeof(ServerChat.cChat));
		CCUserManager::getSingleton().SendServerMessage(GmMessage,sizeof(GmMessage),pFightTeamChat->dwUserID);
	}
	return;
}

//CT_VOID CChatEvent::BroadCastWorldMes(CT_CHAR* pBuf,CT_DWORD len)
//{
//	CMD_G_BroadCastMes *pBroadCast = (CMD_G_BroadCastMes *)pBuf;
//	CCUserManager::getSingleton().BroadCastWorldMessage(pBroadCast,len);
//	return;
//}
