#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
//#include "GlobeScriptInterface.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CUserManager.h"
#include "CFriendEvent.h"

#define MAX_FRIEND_COUNT		1024

CCFriendEvent::CCFriendEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_REQUEST);		//�ͻ��˺�������
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_ACCEPT);		//�ͻ��˽��ܺ�������
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_ACCEPT_EX);		//�ͻ��˽��ܲ���ӶԷ�Ϊ����
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_REJECT);		//�ͻ��˾ܾ�
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_DELETE);		//�ͻ���ɾ������
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_BLACK);			//�ͻ�����Ӻ�����
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_BLACK_DELETE);	//�ͻ���ɾ��������
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_PUSH);			//�ͻ������ͺ���
	RegistMessage(MAIN_G_FRIEND_EVENT, SUB_G_FRIEND_LINE);			//��ȡ��������״̬

	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_FRIEND_REQUST_S);	//�Զ˺�������
	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_FRIEND_ACCEPT_S);	//�Զ˽��ܺ�������
	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_FRIEND_REJECT_S);	//�Զ˾ܾ�
	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_FRIEND_BLACK_S);	//�Զ���Ӻ�����
	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_DB_FRIEND_FIND);	//���ݿⷵ�ز�����Ϣ
	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_DB_FRIEND_FIND_BLACK);//���ݿⷵ�ز�����Ϣ
	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_FRIEND_DELETE_S);
	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_FRIEND_SEND_UNLINK);//���ݿⷵ�ز����ߺ��ѵ�ͷ����Ϣ

	RegistMessage(MSC_MAIN_FRIEND_EVENT_DATA, MSC_SUB_FRIEND_EXIT_EVENT);	//��������
}

CCFriendEvent::~CCFriendEvent(void)
{
}

//�����û���Ϣ
CT_VOID CCFriendEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_FRIEND_EVENT:
		{
			switch (subID)
			{
			case SUB_G_FRIEND_REQUEST:
				DisposeFriendRequestBat(pBuf,len);
				break;
			case SUB_G_FRIEND_ACCEPT:
				DisposeFriendAcceptBat(pBuf,len);
				break;
			case SUB_G_FRIEND_ACCEPT_EX:
				DisposeFriendAcceptEX(pBuf, len);
				break;
			case SUB_G_FRIEND_REJECT:
				DisposeFriendRejectBat(pBuf,len);
				break;
			case SUB_G_FRIEND_DELETE:
				DisposeFriendDelete(pBuf,len);
				break;
			case SUB_G_FRIEND_BLACK:
				DisposeFriendBlack(pBuf,len);
				break;
			case SUB_G_FRIEND_BLACK_DELETE:
				DisposeFriendBlackDelete(pBuf,len);
				break;
			case SUB_G_FRIEND_PUSH:
				DisposeFriendPush(pBuf,len);
				break;
			case SUB_G_FRIEND_LINE:
				DisposeFriendLine(pBuf,len);
				break;
			default:
				CCLogMeg::getSingleton().WriteError("CCFriendEvent::OnUserEvent subID %d",subID);
				break;
			}
			break;
		}
	case MSC_MAIN_FRIEND_EVENT_DATA:
		switch (subID)
		{
		case MSC_SUB_FRIEND_REQUST_S:
			DisposeFriendRequest_S(pBuf,len);
			break;
		case MSC_SUB_FRIEND_ACCEPT_S:
			DisposeFriendAccept_S(pBuf,len);
			break;
		case MSC_SUB_FRIEND_REJECT_S:
			DisposeFriendReject_S(pBuf,len);
			break;
		case MSC_SUB_FRIEND_BLACK_S:
			DisposeFriendBlack_S(pBuf,len);
			break;
		case MSC_SUB_FRIEND_DELETE_S:
			DisposeFriendDelete_S(pBuf,len);
			break;
		case MSC_SUB_DB_FRIEND_FIND:
			DisposeFriendFind(pBuf, len);
			break;
		case MSC_SUB_DB_FRIEND_FIND_BLACK:
			DisposeFriendFindBlack(pBuf, len);
			break;
		case MSC_SUB_FRIEND_EXIT_EVENT:
			DisposeFriendExit(pBuf, len);
			break;
		case MSC_SUB_FRIEND_SEND_UNLINK:
			DisposeFriendNoLinkSend(pBuf, len);
			break;
		default:
			CCLogMeg::getSingleton().WriteError("CCFriendEvent::OnUserEvent subID %d",subID);
			break;
		}
		break;
	default:
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::OnUserEvent mainid %d",mainID);
		break;
	}
	return;
}

//�û���ʱ������
CT_VOID CCFriendEvent::OnTime(CT_DWORD timeID)
{
	return;
}

// һ����������(����)
CT_VOID CCFriendEvent::DisposeFriendRequestBat(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len == 0 || 0 != len % sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendRequestBat: len != sizeof(CMD_G_Friend_ARG)*n");
		return;
	}


	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendRequestBat,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	CT_INT32 iCount = len / sizeof(CMD_G_Friend_ARG);
	
	pUser->m_userFriend.SetRSPCount(iCount);
	for (CT_INT32 i=0; i<iCount; ++i)
	{
		DisposeFriendRequest(pBuf + sizeof(CMD_G_Friend_ARG)*i, sizeof(CMD_G_Friend_ARG));
	}
}

// ������ӶԷ�Ϊ����
CT_VOID CCFriendEvent::DisposeFriendRequest(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendRequest: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendRequest,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	if (pUser->m_userFriend.CheckFriendLimit(FRIEND_DATA))		// ��������
	{
//		CT_INT32 n = 1;
//		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_LIMIT, &n, sizeof(CT_INT32));
		CMD_G_Friend_RSP friendRsp = {0};
		memcpy(&friendRsp, pBuf, len);
		friendRsp.iResult = 1;
		pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_REQUEST_S,friendRsp);
		return;
	}

	CCUser *pFUser=0;

	if(CCUserManager::getSingleton().Find(pFriendArg->cF_NickName,&pFUser))	//�ж�Ŀ���Ƿ�����
	{
		if (pUser->GetUserID() == pFUser->GetUserID())	//��������Լ�Ϊ����
		{
			CMD_G_Friend_RSP friendRsp = {0};
			memcpy(&friendRsp, pBuf, len);
			friendRsp.iResult = 4;
			pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_REQUEST_S, friendRsp);
			return;
		}
		pFriendArg->dwF_UID	= pFUser->GetUserID();
		if(pUser->m_userFriend.CheckBlackList(pFUser->GetUserID()))
		{
			CMD_G_Friend_RSP friendRsp = {0};
			memcpy(&friendRsp, pBuf, len);
			friendRsp.iResult = 3;
			pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_REQUEST_S, friendRsp);
			return;
		}
		DisposeFriendFind(pBuf, len);

		CMD_G_FriendData	friendData = {0};
		friendData.dwUserID	= pFUser->GetUserID();		//����UID
		friendData.dwF_UID	= pUser->GetUserID();		//����UID
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);	//�����Լ�����Ϣ���Է�
		memcpy(friendData.cF_NickName, masterData.cNickName, NAME_SIZE);
		friendData.iF_Occupation	= masterData.iOccupation;
		friendData.iF_Grade			= masterData.mvd.stGrade.iLevel;
		friendData.iF_Status		= 1;
		friendData.iF_FightingPower	= masterData.mvd.stRes.iFightingPower;
		friendData.iTitleID			= pUser->m_userTitle.GetUseTitleID();
		CT_DWORD threadID = GAME_THREAD_CITY + friendData.dwF_UID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadID,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_REQUST_S,(CT_CHAR*)&friendData,sizeof(CMD_G_FriendData));
	}
	else
	{
		CT_DWORD threadID = GAME_THREAD_DATABASE + pFriendArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_FIND,pBuf,len);
	}
	return;
}

CT_VOID CCFriendEvent::DisposeFriendFind(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendFind: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendFind,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	if (pFriendArg->dwF_UID < 1 || pFriendArg->dwF_UID == pFriendArg->dwUserID)
	{
		//pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_NO_FOUND, (CT_CHAR*)pFriendArg, sizeof(CMD_G_Friend_ARG));
		CMD_G_Friend_RSP friendRsp = {0};
		memcpy(&friendRsp, pBuf, len);
		friendRsp.iResult = 3;
		pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_REQUEST_S, friendRsp);
		return;
	}
	else
	{
		if (pUser->m_userFriend.CheckBlackList(pFriendArg->dwF_UID))	//�Ѿ��Ǻ�����
		{
			//pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_NO_FOUND, (CT_CHAR*)pFriendArg, sizeof(CMD_G_Friend_ARG));
			CMD_G_Friend_RSP friendRsp = {0};
			memcpy(&friendRsp, pBuf, len);
			friendRsp.iResult = 3;
			pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_REQUEST_S, friendRsp);
			return;
		}
		if (pUser->m_userFriend.CheckIsFriend(pFriendArg->dwF_UID))	//�Ѿ��Ǻ���
		{
			CMD_G_Friend_RSP friendRsp = {0};
			memcpy(&friendRsp, pBuf, len);
			friendRsp.iResult = 0;
			pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_REQUEST_S, friendRsp);
			return;
		}
		CMD_G_Friend_RSP friendRsp = {0};
		memcpy(&friendRsp, pBuf, len);
		friendRsp.iResult = 0;
		pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_REQUEST_S, friendRsp);
//		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_REQUEST, (CT_CHAR*)pFriendArg, sizeof(CMD_G_Friend_ARG));
		MSC_DB_FRIEND dbFriend = {0};
		dbFriend.dwU_ID = pFriendArg->dwF_UID;		//����ID
		dbFriend.dwF_UID = pFriendArg->dwUserID;	//����ID
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);	//�����Լ�����Ϣ���Է�
		memcpy(dbFriend.cF_NickName, masterData.cNickName, NAME_SIZE);
		dbFriend.iF_Occupation	= masterData.iOccupation;
		dbFriend.iF_Type = FRIEND_REQ;
		CT_DWORD threadID = GAME_THREAD_DATABASE + dbFriend.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_UPDATE,(CT_CHAR*)&dbFriend,sizeof(MSC_DB_FRIEND));
	}
}

// ֪ͨ�ͻ����յ��������󣬲������ڴ��¼
CT_VOID CCFriendEvent::DisposeFriendRequest_S(CT_CHAR* pBuf,CT_DWORD len)		//ֻ��˫�������߲Żᴥ��
{
	if(len != sizeof(CMD_G_FriendData))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendRequest_S: len != sizeof(CMD_G_FriendData)");
		return;
	}

	CMD_G_FriendData	*pFriendData = (CMD_G_FriendData*)pBuf;
	CCUser *pFUser=0;
	

	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendData->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendRequest_S,can't find user %d",pFriendData->dwUserID);
		return;
	}
	if(pUser->m_userFriend.CheckBlackList(pFriendData->dwF_UID))
		return;
/*	DisposeFriendRequest() �������
	if(CCUserManager::getSingleton().Find(pFriendData->dwF_UID,&pFUser))
	{
		MasterData masterData = {0};
		pFUser->m_userBase.GetMasterData(masterData);
		pFriendData->iF_Grade			= masterData.mvd.stGrade.iLevel;
		pFriendData->iF_Status			= 1;
		pFriendData->iF_FightingPower	= masterData.mvd.stRes.iFightingPower;
	} */
	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_REQUEST, (CT_CHAR*)pFriendData, sizeof(CMD_G_FriendData));

	pUser->m_userFriend.InsertFriendReq(*pFriendData);
	
	return;
}

// һ�����ܺ�������(����)
CT_VOID CCFriendEvent::DisposeFriendAcceptBat(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len == 0 || 0 != len % sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendAcceptBat: len != sizeof(CMD_G_Friend_ARG)*n");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendAcceptBat,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	CT_INT32 iCount = len / sizeof(CMD_G_Friend_ARG);

	if (iCount == 1 && pFriendArg->dwF_UID == 0)
	{
		CMD_G_FriendData friendData[256] = {0};
		CMD_G_Friend_ARG friendArg = {0};
		CT_INT32 iLen=256 * sizeof(CMD_G_Friend_ARG);
		if (pUser->m_userFriend.GetFriendReq((CT_CHAR*)friendData, iLen))
		{
			iCount = iLen / sizeof(CMD_G_FriendData);
			pUser->m_userFriend.SetRSPCount(iCount);
			for (CT_INT32 i=0; i<iCount; ++i)
			{
				friendArg.dwUserID = friendData[i].dwUserID;
				friendArg.dwF_UID = friendData[i].dwF_UID;
				DisposeFriendAccept((CT_CHAR*)&friendArg, sizeof(CMD_G_Friend_ARG));
			}
		}
	}
	else
	{
		pUser->m_userFriend.SetRSPCount(iCount);
		for (CT_INT32 i=0; i<iCount; ++i)
		{
			DisposeFriendAccept(pBuf + sizeof(CMD_G_Friend_ARG)*i, sizeof(CMD_G_Friend_ARG));
		}
	}
	
// 	CT_CHAR buf[1024*15] = {0};
// 	len = 1024*15;
// 	if (pUser->m_userFriend.GetFriendRSP(buf, len))
// 	{
// 		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_ACCEPT_S, buf, len);
// 	}
}
//���ܺ������� 1.ɾ���Լ��ĺ��������¼(���ݿ���ڴ�) 2.�������󷽵ĺ��Ѽ�¼(���ݿ�) 3.����Է����ߣ�֪ͨ�Է���Ӻ��ѳɹ�
CT_VOID CCFriendEvent::DisposeFriendAccept(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendAccept: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendAccept,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	if (pUser->m_userFriend.CheckBlackList(pFriendArg->dwF_UID))
	{
		CMD_G_Friend_RSP friendRsp = {0};
		memcpy(&friendRsp, pBuf, len);
		friendRsp.iResult = 3;
		pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_ACCEPT_S, friendRsp);
		return;
	}

	if (pUser->m_userFriend.CheckFriendLimit(FRIEND_DATA))		// ��������
	{
//		CT_INT32 n = 1;
//		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_LIMIT, &n, sizeof(CT_INT32));
		CMD_G_Friend_RSP friendRsp = {0};
		memcpy(&friendRsp, pBuf, len);
		friendRsp.iResult = 1;
		pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_ACCEPT_S, friendRsp);
		return;
	}

	CCUser *pFUser=0;
	if (CCUserManager::getSingleton().Find(pFriendArg->dwF_UID,&pFUser))
	{
		if (pFUser->m_userFriend.CheckFriendLimit(FRIEND_DATA))		// �Է���������
		{
//			CT_INT32 n = 2;
//			pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_LIMIT, &n, sizeof(CT_INT32));
			CMD_G_Friend_RSP friendRsp = {0};
			memcpy(&friendRsp, pBuf, len);
			friendRsp.iResult = 2;
			pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_ACCEPT_S, friendRsp);
			return;
		}
	}

	CMD_G_Friend_RSP friendRsp = {0};
	memcpy(&friendRsp, pBuf, len);
	friendRsp.iResult = 0;
	pUser->m_userFriend.InsertFriendRSP(SUB_G_FRIEND_ACCEPT_S, friendRsp);

/*	��Ϊ���Ϊ˫�����
	// 1.ɾ���Լ��ĺ��������¼ 
	MSC_DB_FRIEND_DELETE dbFriendDelete = {0};
	dbFriendDelete.dwU_ID = pFriendArg->dwUserID;
	dbFriendDelete.dwF_UID = pFriendArg->dwF_UID;
	dbFriendDelete.iF_Type = FRIEND_REQ;
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFriendDelete.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_DELETE,(CT_CHAR*)&dbFriendDelete,sizeof(MSC_DB_FRIEND_DELETE));
	pUser->m_userFriend.DeleteFriend(pFriendArg->dwF_UID, FRIEND_REQ);
*/

	// ��ӶԷ�Ϊ�Լ�����
	MSC_DB_FRIEND	dbFriend = {0};
	dbFriend.dwU_ID = pFriendArg->dwUserID;
	dbFriend.dwF_UID = pFriendArg->dwF_UID;
	dbFriend.iF_Type = FRIEND_DATA;
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFriend.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_UPDATE,(CT_CHAR*)&dbFriend,sizeof(MSC_DB_FRIEND));
	CMD_G_FriendData	*pFriendData = 0;
	if(pUser->m_userFriend.GetFriendReq(pFriendArg->dwF_UID, &pFriendData))
		pUser->m_userFriend.InsertFriendData(*pFriendData);
	pUser->m_userFriend.DeleteFriend(pFriendArg->dwF_UID, FRIEND_REQ);

	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);

	// 2.�������󷽵ĺ��Ѽ�¼ 
	dbFriend.dwU_ID = pFriendArg->dwF_UID;
	dbFriend.dwF_UID = pFriendArg->dwUserID;
	memcpy(dbFriend.cF_NickName, masterData.cNickName, NAME_SIZE);
	dbFriend.iF_Occupation = masterData.iOccupation;
	dbFriend.iF_Type = FRIEND_DATA;
	threadID = GAME_THREAD_DATABASE + dbFriend.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_UPDATE,(CT_CHAR*)&dbFriend,sizeof(MSC_DB_FRIEND));

	// 3.����Է����ߣ�֪ͨ�Է���Ӻ��ѳɹ�
	if (CCUserManager::getSingleton().Find(pFriendArg->dwF_UID,&pFUser))
	{
		CMD_G_FriendData	friendData = {0};
		friendData.dwUserID	= pFriendArg->dwF_UID;		//����UID	�����Լ�����Ϣ���Է�
		friendData.dwF_UID	= pFriendArg->dwUserID;		//����UID
		memcpy(friendData.cF_NickName, masterData.cNickName, NAME_SIZE);
		friendData.iF_Occupation	= masterData.iOccupation;
		friendData.iF_Grade			= masterData.mvd.stGrade.iLevel;
		friendData.iF_Status		= 1;
		friendData.iF_FightingPower	= masterData.mvd.stRes.iFightingPower;
		friendData.iTitleID			= pUser->m_userTitle.GetUseTitleID();
		CT_DWORD threadID = GAME_THREAD_CITY + pFriendArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadID,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_ACCEPT_S,(CT_CHAR*)&friendData,sizeof(CMD_G_FriendData));
	}

	return;
}

// ֪ͨ�ͻ�����Ӻ��ѳɹ����������ڴ��¼
CT_VOID CCFriendEvent::DisposeFriendAccept_S(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FriendData))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendAccept_S: len != sizeof(CMD_G_FriendData)");
		return;
	}

	CMD_G_FriendData *pFriendData=(CMD_G_FriendData*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendData->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendAccept_S,can't find user %d",pFriendData->dwUserID);
		return;
	}

	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_ACCEPT, (CT_CHAR*)pFriendData, sizeof(CMD_G_FriendData));
	pUser->m_userFriend.InsertFriendData(*pFriendData);

	return;
}

//���ܺ�������EX�� 1.ɾ���Լ��ĺ��������¼(���ݿ���ڴ�) 2.�������󷽵ĺ��Ѽ�¼(���ݿ�) 3.����Է����ߣ�֪ͨ�Է���Ӻ��ѳɹ� 4.�������ӶԷ�Ϊ�Լ��ĺ���
CT_VOID CCFriendEvent::DisposeFriendAcceptEX(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendAcceptEX: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendAcceptEX,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	DisposeFriendAccept(pBuf,len);

	DisposeFriendRequest(pBuf,len);

	return;
}

// һ���ܾ���������(����)
CT_VOID CCFriendEvent::DisposeFriendRejectBat(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len == 0 || 0 != len % sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendAcceptBat: len != sizeof(CMD_G_Friend_ARG)*n");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendAcceptBat,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_REJECT_S, pBuf, len);

	CT_INT32 iCount = len / sizeof(CMD_G_Friend_ARG);

	if (iCount == 1 && pFriendArg->dwF_UID == 0)
	{
		CMD_G_FriendData friendData[256] = {0};
		CMD_G_Friend_ARG friendArg = {0};
		CT_INT32 iLen=256 * sizeof(CMD_G_Friend_ARG);
		if (pUser->m_userFriend.GetFriendReq((CT_CHAR*)friendData, iLen))
		{
			iCount = iLen / sizeof(CMD_G_FriendData);
			for (CT_INT32 i=0; i<iCount; ++i)
			{
				friendArg.dwUserID = friendData[i].dwUserID;
				friendArg.dwF_UID = friendData[i].dwF_UID;
				DisposeFriendReject((CT_CHAR*)&friendArg, sizeof(CMD_G_Friend_ARG));
			}
		}
	}
	else
	{
		for (CT_INT32 i=0; i<iCount; ++i)
		{
			DisposeFriendReject(pBuf + sizeof(CMD_G_Friend_ARG)*i, sizeof(CMD_G_Friend_ARG));
		}
	}
	
}

//�ܾ��������� 1.ɾ���Լ��ĺ��������¼(���ݿ���ڴ�) 2.����Է����ߣ�֪ͨ�Է���Ӻ��ѱ��ܾ�
CT_VOID CCFriendEvent::DisposeFriendReject(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendAccept: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendAccept,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	// 1.ɾ���Լ��ĺ��������¼ 
	MSC_DB_FRIEND_DELETE dbFriendDelete = {0};
	dbFriendDelete.dwU_ID = pFriendArg->dwUserID;
	dbFriendDelete.dwF_UID = pFriendArg->dwF_UID;
	dbFriendDelete.iF_Type = FRIEND_REQ;
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFriendDelete.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_DELETE,(CT_CHAR*)&dbFriendDelete,sizeof(MSC_DB_FRIEND_DELETE));
	pUser->m_userFriend.DeleteFriend(pFriendArg->dwF_UID, FRIEND_REQ);

	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);

	// 2.����Է����ߣ�֪ͨ�Է���Ӻ��ѱ��ܾ�
	CCUser *pFUser=0;
	if (CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pFUser))
	{
		CMD_G_FriendData	friendData = {0};
		friendData.dwUserID	= pFriendArg->dwF_UID;		//����UID	�����Լ�����Ϣ���Է�
		friendData.dwF_UID	= pFriendArg->dwUserID;		//����UID
		memcpy(friendData.cF_NickName, masterData.cNickName, NAME_SIZE);
		friendData.iF_Occupation	= masterData.iOccupation;
		friendData.iF_Grade			= masterData.mvd.stGrade.iLevel;
		friendData.iF_Status		= 1;
		friendData.iF_FightingPower	= masterData.mvd.stRes.iFightingPower;
		CT_DWORD threadID = GAME_THREAD_CITY + pFriendArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadID,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_REJECT_S,(CT_CHAR*)&friendData,sizeof(CMD_G_FriendData));
	}
	return;
}

//֪ͨ�ͻ�����Ӻ��ѱ��ܾ�
CT_VOID CCFriendEvent::DisposeFriendReject_S(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FriendData))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendReject_S: len != sizeof(CMD_G_FriendData)");
		return;
	}

	CMD_G_FriendData *pFriendData=(CMD_G_FriendData*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendData->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendReject_S,can't find user %d",pFriendData->dwUserID);
		return;
	}

	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_REJECT, (CT_CHAR*)pFriendData, sizeof(CMD_G_FriendData));

	return;
}

// ɾ������	(���ݿ���ڴ�)������Ӧ�ͻ���
CT_VOID CCFriendEvent::DisposeFriendDelete(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CMD_G_Friend_ARG: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendDelete,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	// ɾ������ �Լ� ���ݿ�
	MSC_DB_FRIEND_DELETE dbFriendDelete = {0};
	dbFriendDelete.dwU_ID = pFriendArg->dwUserID;
	dbFriendDelete.dwF_UID = pFriendArg->dwF_UID;
	dbFriendDelete.iF_Type = FRIEND_DATA;
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFriendDelete.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_DELETE,(CT_CHAR*)&dbFriendDelete,sizeof(MSC_DB_FRIEND_DELETE));
	pUser->m_userFriend.DeleteFriend(pFriendArg->dwF_UID, FRIEND_DATA);
	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_DELETE_S, pBuf, len);

	// ɾ������ �Է� ���ݿ�
	dbFriendDelete.dwU_ID = pFriendArg->dwF_UID;
	dbFriendDelete.dwF_UID = pFriendArg->dwUserID;
	dbFriendDelete.iF_Type = FRIEND_DATA;
	threadID = GAME_THREAD_DATABASE + dbFriendDelete.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_DELETE,(CT_CHAR*)&dbFriendDelete,sizeof(MSC_DB_FRIEND_DELETE));

	CCUser *pFUser = 0;				// ����Է�����, ֪ͨ�Է�ɾ������(�ڴ� + �ͻ���)
	if (CCUserManager::getSingleton().Find(pFriendArg->dwF_UID,&pFUser))
	{
		CT_DWORD dwTemp = pFriendArg->dwUserID;
		pFriendArg->dwUserID = pFriendArg->dwF_UID;
		pFriendArg->dwF_UID = dwTemp;
		CT_DWORD threadID2 = GAME_THREAD_CITY + pFriendArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadID2,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_DELETE_S,pBuf,len);
	}

	return;
}

// �Զ�ɾ���Լ�����		1.ɾ������(�ڴ�)  2.֪ͨ�ͻ���
CT_VOID CCFriendEvent::DisposeFriendDelete_S(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CMD_G_Friend_ARG: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendDelete_S,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	pUser->m_userFriend.DeleteFriend(pFriendArg->dwF_UID, FRIEND_DATA);
	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_DELETE, pBuf, len);

	return;
}

CT_VOID CCFriendEvent::DisposeFriendBlack(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CMD_G_Friend_ARG: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendBlack,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	if (pUser->m_userFriend.CheckFriendLimit(FRIEND_BLACKNAME))			// ����������
	{
		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_BLACK_LIMIT);
	}

	CCUser *pFUser=0;
	if(CCUserManager::getSingleton().Find(pFriendArg->cF_NickName,&pFUser))	//�ж�Ŀ���Ƿ�����
	{
		if (pUser->GetUserID() == pFUser->GetUserID())	//��������Լ�Ϊ������
		{
			return;
		}
		CMD_G_FriendData friendData = {0};
		friendData.dwUserID = pFriendArg->dwUserID;
		friendData.dwF_UID = pFUser->GetUserID();
		MasterData masterData = {0};
		pFUser->m_userBase.GetMasterData(masterData);
		friendData.iF_Occupation	= masterData.iOccupation;
		friendData.iF_Grade			= masterData.mvd.stGrade.iLevel;
		friendData.iF_Status		= 1;
		friendData.iF_FightingPower	= masterData.mvd.stRes.iFightingPower;
		friendData.iTitleID			= pFUser->m_userTitle.GetUseTitleID();
		memcpy(friendData.cF_NickName, pFriendArg->cF_NickName, NAME_SIZE);

		DisposeFriendFindBlack((CT_CHAR*)&friendData, sizeof(CMD_G_FriendData));
	}
	else
	{
		CT_DWORD threadID = GAME_THREAD_DATABASE + pFriendArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_FIND_BLACK,pBuf,len);
	}

	return;
}

// ����ҵ�����Ӻ�������Ϣ(���ݿ���ڴ�), ��ɾ���Է�������Ϣ, ����Է����ߣ���ӵ��Է��ڴ���Ϣ
CT_VOID CCFriendEvent::DisposeFriendFindBlack(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FriendData))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendFindBlack: len != sizeof(CMD_G_FriendData)");
		return;
	}

	CMD_G_FriendData *pFriendData=(CMD_G_FriendData*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendData->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendFindBlack,can't find user %d",pFriendData->dwUserID);
		return;
	}

	if (pFriendData->dwF_UID < 1 || pFriendData->dwF_UID == pFriendData->dwUserID)
	{
		CMD_G_Friend_ARG friendArg = {0};
		friendArg.dwUserID = pFriendData->dwUserID;
		friendArg.dwF_UID = pFriendData->dwF_UID;
		memcpy(friendArg.cF_NickName, pFriendData->cF_NickName, NAME_SIZE);
		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_NO_FOUND, (CT_CHAR*)&friendArg, sizeof(CMD_G_Friend_ARG));
		return;
	}
	else
	{
		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_BLACK, pBuf, len);

		pUser->m_userFriend.DeleteFriend(pFriendData->dwF_UID, FRIEND_DATA);	//�ȴ��ڴ����ɾ��
		pUser->m_userFriend.DeleteFriend(pFriendData->dwF_UID, FRIEND_REQ);	//�ȴ��ڴ����ɾ��
		pUser->m_userFriend.InsertBlackName(*pFriendData);

		MSC_DB_FRIEND dbFriend = {0};
		dbFriend.dwU_ID = pFriendData->dwUserID;
		dbFriend.dwF_UID = pFriendData->dwF_UID;
		memcpy(dbFriend.cF_NickName, pFriendData->cF_NickName, NAME_SIZE);
		dbFriend.iF_Type = FRIEND_BLACKNAME;
		dbFriend.iF_Occupation	= pFriendData->iF_Occupation;
		CT_DWORD threadID = GAME_THREAD_DATABASE + dbFriend.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_UPDATE,(CT_CHAR*)&dbFriend,sizeof(MSC_DB_FRIEND));

		MSC_DB_FRIEND_DELETE dbFriendDelete = {0};
		dbFriendDelete.dwU_ID = pFriendData->dwF_UID;	//����UID
		dbFriendDelete.dwF_UID = pFriendData->dwUserID;
		dbFriendDelete.iF_Type = FRIEND_DATA;
		threadID = GAME_THREAD_DATABASE + dbFriend.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_DELETE,(CT_CHAR*)&dbFriendDelete,sizeof(MSC_DB_FRIEND_DELETE));

		CCUser *pFUser = 0;
		if (CCUserManager::getSingleton().Find(pFriendData->dwF_UID,&pFUser))
		{
			CMD_G_Friend_ARG friendARG = {0};
			friendARG.dwUserID = pFriendData->dwF_UID;	//����UID
			friendARG.dwF_UID = pFriendData->dwUserID;
			CT_DWORD threadID2 = GAME_THREAD_CITY + pFriendData->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(threadID2,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_BLACK_S,(CT_CHAR*)&friendARG,sizeof(CMD_G_Friend_ARG));
		}

	}
	return;
}

//	�Զ�����Լ�Ϊ������	1.ɾ������(�ڴ�) 2.֪ͨ�ͻ���ɾ������ 3.��ӷ��������
CT_VOID CCFriendEvent::DisposeFriendBlack_S(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendBlack_S: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendBlack_S,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	pUser->m_userFriend.DeleteFriend(pFriendArg->dwF_UID, FRIEND_DATA);					//ɾ������(�ڴ�)
	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_DELETE, pBuf, len);	//֪ͨ�ͻ���ɾ������
	pUser->m_userFriend.InsertBlackRev(pFriendArg->dwF_UID);							//��ӷ��������

	return;
}

CT_VOID CCFriendEvent::DisposeFriendBlackDelete(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CMD_G_Friend_ARG: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendBlackDelete,can't find user %d",pFriendArg->dwUserID);
		return;
	}

	MSC_DB_FRIEND_DELETE dbFriendDelete = {0};
	dbFriendDelete.dwU_ID = pFriendArg->dwUserID;
	dbFriendDelete.dwF_UID = pFriendArg->dwF_UID;
	dbFriendDelete.iF_Type = FRIEND_BLACKNAME;
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFriendDelete.dwU_ID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_DELETE,(CT_CHAR*)&dbFriendDelete,sizeof(MSC_DB_FRIEND_DELETE));
	pUser->m_userFriend.DeleteFriend(pFriendArg->dwF_UID, FRIEND_BLACKNAME);
	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_BLACK_DELETE, pBuf, len);

	CCUser *pFUser=0;
	if(CCUserManager::getSingleton().Find(pFriendArg->dwF_UID,&pFUser))	//����Է����ߣ����¶Է�����������б�
	{
		pFUser->m_userFriend.DeleteFriend(pFriendArg->dwUserID, FRIEND_BLACKREV);
	}

	return;
}


CT_VOID CCFriendEvent::DisposeFriendPush(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CT_DWORD))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendPush: len != sizeof(CT_DWORD)");
		return;
	}

	CT_DWORD UserID=0;
	memcpy(&UserID, pBuf, sizeof(CT_DWORD));
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(UserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendLine,can't find user %d",UserID);
		return;
	}

	CMD_G_FriendData	friendData[50] = {0};
	CT_INT32	iBufLen = sizeof(CMD_G_FriendData) * 50;
	CCUserManager::getSingleton().GetOnlineUserData(UserID, (CT_CHAR*)&friendData, iBufLen, 1);
	pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_PUSH, (CT_CHAR*)&friendData, iBufLen);

	return;
}

CT_VOID CCFriendEvent::DisposeFriendLine(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Friend_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendLine: len != sizeof(CMD_G_Friend_ARG)");
		return;
	}

	CMD_G_Friend_ARG *pFriendArg=(CMD_G_Friend_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendLine,can't find user %d",pFriendArg->dwUserID);
		return;
	}

//	pUser->m_userFriend.SendFriendLine(pFriendArg->dwF_UID);
	CCUser *pFUser=0;
	if (pFriendArg->dwF_UID == 0)
	{
		CT_INT32 iCount=MAX_FRIEND_COUNT;
		CT_DWORD dwFList[MAX_FRIEND_COUNT] = {0};
		CT_INT32 iGradeList[MAX_FRIEND_COUNT] = {0};
		CT_INT64 iExitTimeList[MAX_FRIEND_COUNT] = {0};
		
		if (pUser->m_userFriend.GetFriendList(dwFList,iGradeList, iExitTimeList, iCount))
		{
			CMD_G_FriendDataLine buf[1024*15] = {0};
			pFUser = 0;

			if(iCount*sizeof(CMD_G_FriendDataLine) >= 1024*15)
			{
				CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendLine iCount*CMD_G_FriendDataLine >= 1024*15");
				return;
			}
			for (CT_INT32 i=0; i<iCount; ++i)
			{
				buf[i].dwUserID = pFriendArg->dwUserID;
				buf[i].dwF_UID = dwFList[i];
				if (CCUserManager::getSingleton().Find(dwFList[i],&pFUser))
				{
					MasterData masterData = {0};
					pFUser->m_userBase.GetMasterData(masterData);
					buf[i].iF_Grade = masterData.mvd.stGrade.iLevel;
					buf[i].iF_Status = 1;
					buf[i].iF_FightingPower = masterData.mvd.stRes.iFightingPower;
					buf[i].iLastExitTime = 0;
					buf[i].iTitleID = pFUser->m_userTitle.GetUseTitleID();
					pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_LINE, (CT_CHAR*)&(buf[i]), sizeof(CMD_G_FriendDataLine));
				}
				else
				{
					buf[i].iF_Grade = iGradeList[i];
					buf[i].iF_Status = 0;
					buf[i].iF_FightingPower = 0;
					buf[i].iLastExitTime = iExitTimeList[i];
					CT_DWORD threadID = GAME_THREAD_DATABASE + pFriendArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
					G_SendThreadInfo(threadID,MSC_MAIN_DB_FRIEND_DATA,MSC_SUB_DB_FRIEND_GET_TITLE,(CT_CHAR*)&(buf[i]),sizeof(buf[i]));
				}
			}
//			pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_LINE, (CT_CHAR*)buf, sizeof(CMD_G_FriendDataLine) * iCount);

		}		
	}
	else					//ָ������״̬
	{
		CMD_G_FriendDataLine	friendDataLine = {0};
		friendDataLine.dwUserID = pFriendArg->dwUserID;
		friendDataLine.dwF_UID = pFriendArg->dwF_UID;
		if (CCUserManager::getSingleton().Find(pFriendArg->dwF_UID,&pFUser))
		{
			MasterData masterData = {0};
			pFUser->m_userBase.GetMasterData(masterData);
			friendDataLine.iF_Grade = masterData.mvd.stGrade.iLevel;
			friendDataLine.iF_Status = 1;
			friendDataLine.iF_FightingPower = masterData.mvd.stRes.iFightingPower;
			friendDataLine.iTitleID = pFUser->m_userTitle.GetUseTitleID();
			//friendDataLine.iLastExitTime = 0;
		}
		else
		{
			friendDataLine.iF_Grade = 0;
			friendDataLine.iF_Status = 0;
			friendDataLine.iF_FightingPower = 0;
			//friendDataLine.iLastExitTime = 0;
		}
		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_LINE, (CT_CHAR*)&friendDataLine, sizeof(CMD_G_FriendDataLine));
	}
	return;
}

CT_VOID CCFriendEvent::DisposeFriendExit(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(MSC_FriendExit))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFriendExit: len != sizeof(MSC_FriendExit)");
		return;
	}

	MSC_FriendExit *pFriendExit=(MSC_FriendExit*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFriendExit->dwUserID,&pUser))
	{
//		CCLogMeg::getSingleton().WriteError("DisposeFriendExit,can't find user %d",pFriendExit->dwUserID);
		return;
	}
	pUser->m_userFriend.UserExitEvent(pFriendExit->dwFriendID, pFriendExit->iExitTime, pFriendExit->iFType);

}

CT_VOID CCFriendEvent::DisposeFriendNoLinkSend(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FriendDataLine))
	{
		CCLogMeg::getSingleton().WriteError("CCFriendEvent::DisposeFriendNoLinkSend len != sizeof(CMD_G_FriendDataLine)");
		return;
	}

	CMD_G_FriendDataLine *pFriendData = (CMD_G_FriendDataLine *)pBuf;

	CCUser* pUser;
	if (CCUserManager::getSingleton().Find(pFriendData->dwUserID, &pUser))
	{
		pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,SUB_G_FRIEND_LINE, (CT_CHAR*)pFriendData, sizeof(CMD_G_FriendDataLine));
	}
}