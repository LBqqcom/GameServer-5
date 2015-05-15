#include "CDBFriendEvent.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "../sdk/include/CDBLogMes.h"

CCDBFriendEvent::CCDBFriendEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_UPDATE);
//	RegistMessage(MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_GET);
	RegistMessage(MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_DELETE);
	RegistMessage(MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_FIND);
	RegistMessage(MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_FIND_BLACK);
	RegistMessage(MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_GET_TITLE);
}

CCDBFriendEvent::~CCDBFriendEvent(void)
{
}

//处理用户消息
CT_VOID CCDBFriendEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
							  CT_CHAR* pBuf,		CT_DWORD len,
							  CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_FRIEND_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_FRIEND_UPDATE:			//增加/更改请求/好友/黑名单
		{
			DisposeFriendUpdate(pBuf,len);
			break;
		}
//	case MSC_SUB_DB_FRIEND_GET:				//获取请求/好友/黑名单信息
//		{
//			DisposeFriendGet(pBuf,len);
//			break;
//		}
	case MSC_SUB_DB_FRIEND_DELETE:			//删除请求/好友/黑名单
		{
			DisposeFriendDelete(pBuf, len);
			break;
		}
	case MSC_SUB_DB_FRIEND_FIND:
		{
			DisposeFriendFind(pBuf, len);	//添加新好友时,根据名称得到UID
			break;
		}
	case MSC_SUB_DB_FRIEND_FIND_BLACK:
		{
			DisposeFriendFindBlack(pBuf, len);	//添加黑名单时,根据名称得到UID
			break;
		}
	case MSC_SUB_DB_FRIEND_GET_TITLE:
		{
			DisposeFriendGetTitle(pBuf, len);   //打开界面时，得到不在线好友的头衔信息
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBFriendEvent::OnTime(CT_DWORD timeID)
{
	return;
}

//增加/更改请求/好友/黑名单
CT_VOID CCDBFriendEvent::DisposeFriendUpdate(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FRIEND))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::DisposeFriendUpdate : len != sizeof(MSC_DB_FRIEND)");
		return;
	}

	MSC_DB_FRIEND *pFriendData=(MSC_DB_FRIEND*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FriendUpdate");

	m_dbConnector.AddParm("_uid", pFriendData->dwU_ID);
	m_dbConnector.AddParm("_fuid", pFriendData->dwF_UID);
	m_dbConnector.AddParm("_nickname", pFriendData->cF_NickName);
	m_dbConnector.AddParm("_occupation", pFriendData->iF_Occupation);
	m_dbConnector.AddParm("_type", pFriendData->iF_Type);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pFriendData->dwU_ID, "SP_FriendUpdate", "_uid:%d, _fuid:%d, _NickName:%s, _occupation:%d, _type:%d", 
			pFriendData->dwU_ID, pFriendData->dwF_UID, pFriendData->cF_NickName, pFriendData->iF_Occupation, pFriendData->iF_Type );
		return;
	}
	return;
}

//删除请求/好友/黑名单
CT_VOID CCDBFriendEvent::DisposeFriendDelete(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FRIEND_DELETE))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::DisposeFriendDelete : len != sizeof(MSC_DB_FRIEND_DELTE)");
		return;
	}

	MSC_DB_FRIEND_DELETE *pFriendDelete=(MSC_DB_FRIEND_DELETE*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FriendDelete");

	m_dbConnector.AddParm("_uid", pFriendDelete->dwU_ID);
	m_dbConnector.AddParm("_fuid", pFriendDelete->dwF_UID);
	m_dbConnector.AddParm("_type", pFriendDelete->iF_Type);


	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pFriendDelete->dwU_ID, "SP_FriendDelete", "_uid:%d, _fuid:%d ", pFriendDelete->dwU_ID,pFriendDelete->dwF_UID );
		return;
	}

	return;
}

CT_VOID CCDBFriendEvent::DisposeFriendFind(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::DisposeFriendFind : len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendARG=(CMD_G_Friend_ARG*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetUIDByName");
	m_dbConnector.AddParm("_nickname", pFriendARG->cF_NickName);
	
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			if (pResult->MoveNextRow())
			{
				pResult->GetValue(0, pFriendARG->dwF_UID);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_GetUIDByName", "cNickName:%s", pFriendARG->cF_NickName );
	}
	CT_DWORD threadid = GAME_THREAD_CITY + pFriendARG->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_DB_FRIEND_FIND,pBuf,sizeof(CMD_G_Friend_ARG));					
		
}

CT_VOID CCDBFriendEvent::DisposeFriendFindBlack(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::DisposeFriendFindBlack : len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendARG=(CMD_G_Friend_ARG*)pBuf;
	CMD_G_FriendData friendData = {0};

	friendData.dwUserID = pFriendARG->dwUserID;
	memcpy(friendData.cF_NickName, pFriendARG->cF_NickName, NAME_SIZE);
	friendData.iF_Status = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetUIDByName");
	m_dbConnector.AddParm("_nickname", pFriendARG->cF_NickName);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			if (pResult->MoveNextRow())
			{
				pResult->GetValue(0, friendData.dwF_UID );
				pResult->GetValue(1, friendData.iF_Grade);
				pResult->GetValue(2, friendData.iF_Occupation);
				pResult->GetValue(5, friendData.iTitleID);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_GetUIDByName", "cNickName:%s", pFriendARG->cF_NickName );
	}
	CT_DWORD threadid = GAME_THREAD_CITY + pFriendARG->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_DB_FRIEND_FIND_BLACK,(CT_CHAR*)&friendData,sizeof(CMD_G_FriendData));					

}

//打开界面时，获得不在线好友信息
CT_VOID CCDBFriendEvent::DisposeFriendGetTitle(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FriendDataLine))
	{
		CCLogMeg::getSingleton().WriteError("CCDBFriendEvent::DisposeFriendGetInfo len != sizeof(CMD_G_FriendData)");
		return ;
	}

	CMD_G_FriendDataLine* pFriendData = (CMD_G_FriendDataLine*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetTitle");
	m_dbConnector.AddParm("_uid", pFriendData->dwF_UID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCurrent = time(0);
			while (pResult->MoveNextRow())
			{
				CT_INT32 iTitleID = 0;
				CT_INT32 iStatus  = 0;
				CT_INT32 iActiveTime = 0;
				CT_INT32 iEffectTime = 0;
				pResult->GetValue(0, iTitleID);
				pResult->GetValue(1, iStatus);
				
				if (iStatus == 2)
				{
					pFriendData->iTitleID = iTitleID;
					break;
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "SP_GetTitle", "uid:%d", pFriendData->dwUserID );
	}
	CT_DWORD threadid = GAME_THREAD_CITY + pFriendData->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_SEND_UNLINK,(CT_CHAR*)pFriendData,sizeof(CMD_G_FriendDataLine));					

}