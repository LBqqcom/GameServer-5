#include "CGmOrder.h"
#include "CLogMeg.h"
#include "ProtocolGame.h"
#include "CUserManager.h"
#include "CNetInterface.h"
#include "CThreadMes.h"
#include "CGlobeFunc.h"

//gm主消息
#define  GM_M_EXIT              (1)   //强退
#define  GM_M_CHAT				(2)   //gm聊天主消息


//gm子消息
#define  GM_S_BROADCAST			(1)   //广播消息
#define  GM_S_CHAT              (2)   //私聊
#define  GM_S_EXIT_USER         (1)   //强退用户


//消息标志
#define  TAG_GM_INFO   ("info")		//信息标志
#define  TAG_GM_UID    ("uid")		//uid标志

#define  TAG_RETURN_INFO_SUC          ("HTTP/1.1 200 OK\r\nContent-Type: html/text\r\nContent-Length:1\r\nConnection: close\r\n\r\n1")
#define  TAG_RETURN_INFO_FAIL         ("HTTP/1.1 200 OK\r\nContent-Type: html/text\r\nContent-Length:1\r\nConnection: close\r\n\r\n0")
CCGmOrder::CCGmOrder(void)
{
}

CCGmOrder::~CCGmOrder(void)
{
}

CT_VOID CCGmOrder::DisposeGMOrder(CT_DWORD mainID, CT_DWORD subID, const Json::Value& info, CCNetInterface* pNetface )
{
	switch (mainID)
	{
	case GM_M_CHAT:
		{
			DisposeGMChat(subID, info, pNetface);
			break;
		}
	case  GM_M_EXIT:
		{
			DisposeGMExit(subID, info, pNetface);
			break;
		}

	default:
		{
			CCLogMeg::getSingleton().WriteError("错误的Gm消息CCGmOrder::DisposeGMOrder mainID=%i subID=%i", mainID, subID);
			break;
		}
	}
}

CT_VOID CCGmOrder::DisposeGMChat(CT_DWORD subID, const Json::Value& info, CCNetInterface* pNetface)
{

	switch (subID)
	{
	case GM_S_BROADCAST:
		{
			std::string strInfo;
			strInfo = info[TAG_GM_INFO].asString();
			
			CMD_G_GMMessage gm = {0};
			strncpy(gm.cGMMes, strInfo.c_str(),  CHAT_LEN - 1);
			
			MSC_Broadcast_GM  broadcastGm = {0};
			broadcastGm.dwMainID = MAIN_G_CHAT_CITY;
			broadcastGm.dwSubID  = SUB_G_GM_MESSAGE;
			broadcastGm.gm       = gm;
			G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST,  MSC_SUB_BROADCAST_GM, (CT_CHAR*)&broadcastGm, sizeof(broadcastGm));
		
			pNetface->Send(TAG_RETURN_INFO_SUC, strlen(TAG_RETURN_INFO_SUC));
			break;
		}
	case  GM_S_CHAT:
		{

			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("错误的gm消息CCGmOrder::DisposeGMChat, subid=%i", subID);
			break;
		}
	}
}


CT_VOID CCGmOrder::DisposeGMExit(CT_DWORD subID, const Json::Value& info, CCNetInterface* pNetface)
{
	switch (subID)
	{
	case GM_S_EXIT_USER:
		{
			CT_DWORD uid = 0;
			uid = atoi(info[TAG_GM_UID].asCString());
			if (uid == 0)
			{
				CCLogMeg::getSingleton().WriteError("没有发现用户的id=0, CCGmOrder::DisposeGMExit");
				return;
			}
			
			CCUser* pUser = 0;
			if (!CCUserManager::getSingleton().Find(uid, &pUser))
			{
				pNetface->Send(TAG_RETURN_INFO_FAIL, strlen(TAG_RETURN_INFO_FAIL));
				return;
			}
			

			pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_FORCE_EXIT);
			CMD_G_LeaveGame LeaveGame={0};
			LeaveGame.dwUserID= uid;

			CT_DWORD dwThreadID=GAME_THREAD_CITY+(LeaveGame.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum);
			G_SendThreadInfo(dwThreadID,MSC_MAIN_USER_EXIT,0,(CT_CHAR*)&LeaveGame,sizeof(LeaveGame));
			pNetface->Send(TAG_RETURN_INFO_SUC, strlen(TAG_RETURN_INFO_SUC));
			break;
		}

	default:
		{
			CCLogMeg::getSingleton().WriteError("错误的gm消息CCGmOrder::DisposeGMExit, subid=%i", subID);
			break;
		}
	}
}