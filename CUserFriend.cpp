#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUserManager.h"
#include "CDatabaseMes.h"
//#include "CMasterScriptSetting.h"
//#include "CBuildingScriptSetting.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "CUserFriend.h"
#include "GlobeScriptInterface.h"
//#include "CBroadcastScriptSetting.h"
//#include "CRandEventManager.h"



CCUserFriend::CCUserFriend()
{
	m_bEnable = CT_FALSE;
	m_RSPCount = 0;
}
CCUserFriend::~CCUserFriend(void)
{
}
CT_VOID CCUserFriend::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser = pUser;
	m_pMutex = pMutex;
	m_FriendDataMap.clear();
	m_FriendReqMap.clear();
	m_BlackNameDataMap.clear();
	m_BlackNameRev.clear();
	m_FriendRSP.clear();
	m_RSPCount = 0;
}

//清理用户数据
CT_VOID CCUserFriend::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_FriendDataMap.clear();
	m_FriendReqMap.clear();
	m_BlackNameDataMap.clear();
	m_BlackNameRev.clear();
	m_FriendRSP.clear();
}

// 得到好友信息
CT_BOOL CCUserFriend::GetFriendData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);
	G_ValueRange vr1={0},vr2={0};
	if (G_GetValueRange(VALUE_TYPE_FRIEND_LEVEL,&vr1)&&G_GetValueRange(VALUE_TYPE_FRIEND_TASK,&vr2))
	{
		MasterVariableData mvd ={0};
		m_pUser->m_userBase.GetMasterData(mvd);
		//领主等级到达，并且任务完成
		if (m_pUser->m_userTask.ViewCompleteTask((CT_INT32)vr2.fMaxVal)&&mvd.stGrade.iLevel>=vr1.fMaxVal)
		{
			m_bEnable = CT_TRUE;;
		}
	}

	if (!m_bEnable)
	{
		return CT_FALSE;
	}
	CT_INT32 len = m_FriendDataMap.size() * sizeof(CMD_G_FriendData);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	FD_IT it = m_FriendDataMap.begin();
	for (int i = 0; it != m_FriendDataMap.end(); ++it, ++i)
	{
//		CMD_G_FriendData friendData = {0};
		memcpy(pBuf + sizeof(CMD_G_FriendData) * i, &(it->second), sizeof(CMD_G_FriendData));	//因为结构相同, 可以直接拷贝
	}
	bufLen = len;

	return CT_TRUE;
}
// 插入好友数据
CT_VOID CCUserFriend::InsertFriendData(const CMD_G_FriendData &data)
{
	_AUTOMUTEX(m_pMutex);
	FriendData	FData;

	memcpy(&FData, &data, sizeof(FriendData));				//因为结构相同, 可以直接拷贝
	FD_IT it = m_FriendDataMap.find(data.dwF_UID);
	if (it == m_FriendDataMap.end())
	{
		m_FriendDataMap.insert(FriendDataMap::value_type(data.dwF_UID, FData));
	}
}

// 得到好友请求数据
CT_BOOL CCUserFriend::GetFriendReq(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);
	if (!m_bEnable)
	{
		return CT_FALSE;
	}
	CT_INT32 len = m_FriendReqMap.size() * sizeof(CMD_G_FriendData);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	FD_IT it = m_FriendReqMap.begin();
	for (int i = 0; it != m_FriendReqMap.end(); ++it, ++i)
	{
		//		CMD_G_FriendData friendData = {0};
		memcpy(pBuf + sizeof(CMD_G_FriendData) * i, &(it->second), sizeof(CMD_G_FriendData));	//因为结构相同, 可以直接拷贝
	}
	bufLen = len;

	return CT_TRUE;
}

// 插入好友请求数据
CT_VOID CCUserFriend::InsertFriendReq(const CMD_G_FriendData &data)
{
	_AUTOMUTEX(m_pMutex);
	FriendData	FData;

	memcpy(&FData, &data, sizeof(FriendData));				//因为结构相同, 可以直接拷贝

	FD_IT it = m_FriendReqMap.find(data.dwF_UID);
	if (it == m_FriendReqMap.end())
	{
		m_FriendReqMap.insert(FriendDataMap::value_type(data.dwF_UID, FData));
	}
}

// 得到黑名单数据
CT_BOOL CCUserFriend::GetBlackName(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);
	if (!m_bEnable)
	{
		return CT_FALSE;
	}
	CT_INT32 len = m_BlackNameDataMap.size() * sizeof(CMD_G_FriendData);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	FD_IT it = m_BlackNameDataMap.begin();
	for (int i = 0; it != m_BlackNameDataMap.end(); ++it, ++i)
	{
		//		CMD_G_FriendData friendData = {0};
		memcpy(pBuf + sizeof(CMD_G_FriendData) * i, &(it->second), sizeof(CMD_G_FriendData));	//因为结构相同, 可以直接拷贝
	}
	bufLen = len;

	return CT_TRUE;
}

// 插入黑名单数据
CT_VOID CCUserFriend::InsertBlackName(const CMD_G_FriendData &data)
{
	_AUTOMUTEX(m_pMutex);
	FriendData	FData;

	memcpy(&FData, &data, sizeof(FriendData));				//因为结构相同, 可以直接拷贝

	FD_IT it = m_BlackNameDataMap.find(data.dwF_UID);
	if (it == m_BlackNameDataMap.end())
	{
		m_BlackNameDataMap.insert(FriendDataMap::value_type(data.dwF_UID, FData));
	}
}

// 插入反向黑名单数据
CT_VOID CCUserFriend::InsertBlackRev(const CT_INT32 iUID)
{
	_AUTOMUTEX(m_pMutex);
	m_BlackNameRev.insert(iUID);
}

// 删除好友，请求，黑名单, 反向黑名单
CT_VOID CCUserFriend::DeleteFriend(CT_INT32 iF_UID, CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	switch (iType)
	{
	case FRIEND_DATA:
		m_FriendDataMap.erase(iF_UID);
		break;
	case FRIEND_REQ:
		m_FriendReqMap.erase(iF_UID);
		break;
	case FRIEND_BLACKNAME:
		m_BlackNameDataMap.erase(iF_UID);
		break;
	case FRIEND_BLACKREV:
		m_BlackNameRev.erase(iF_UID);
		break;
	default:
		return;
	}
}

CT_BOOL CCUserFriend::CheckBlackList(CT_INT32 iF_UID)					// 检查自己在其他用户黑名单列表
{
	_AUTOMUTEX(m_pMutex);
	if (m_BlackNameRev.find(iF_UID) != m_BlackNameRev.end() || m_BlackNameDataMap.find(iF_UID) != m_BlackNameDataMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCUserFriend::CheckIsFriend(CT_INT32 iF_UID)					// 检查是否已经是自己的好友
{
	_AUTOMUTEX(m_pMutex);
	if (m_FriendDataMap.find(iF_UID) != m_FriendDataMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCUserFriend::GetFriendList(CT_DWORD *dwFList, CT_INT32 *iGradeList, CT_INT64 *iExitTimeList, CT_INT32 &iCount)
{
	_AUTOMUTEX(m_pMutex);
	if (m_FriendDataMap.size() > iCount)
	{
		return CT_FALSE;
	}

	CT_INT32 n=0;
	FD_IT it = m_FriendDataMap.begin();
	FD_IT it_end = m_FriendDataMap.end();
	while (it != it_end)
	{
		dwFList[n] = it->first;
		iGradeList[n] = it->second.iF_Grade;
		iExitTimeList[n++] = it->second.iLastExitTime;
		it++;
	}
	iCount = n;
	return CT_TRUE;
}

CT_BOOL CCUserFriend::CheckFriendLimit(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	G_ValueRange vr={0};
	G_GetValueRange(VALUE_TYPE_FRIEND_COUNT,&vr);
	CT_DWORD iLimit = 0;

	if (vr.fMaxVal < 1)
	{
		iLimit = 100;
	} 
	else
	{
		iLimit = (CT_DWORD)vr.fMaxVal;
	}

	if (iType == FRIEND_DATA)
	{
		if (m_FriendDataMap.size() < iLimit)
		{
			return CT_FALSE;
		} 
		else
		{
			return CT_TRUE;
		}
	}

	if (iType == FRIEND_BLACKNAME)
	{
		if (m_BlackNameDataMap.size() < iLimit)
		{
			return CT_FALSE;
		} 
		else
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_BOOL CCUserFriend::GetFriendReq(CT_DWORD iF_UID, CMD_G_FriendData **pFriendData)
{
	_AUTOMUTEX(m_pMutex);
	FD_IT it = m_FriendReqMap.find(iF_UID);
	if (it != m_FriendReqMap.end())
	{
		*pFriendData = (CMD_G_FriendData*)&(it->second);
		return CT_TRUE;
	}
	return CT_FALSE;

}

CT_BOOL CCUserFriend::GetFriendRSP(CT_CHAR *pBuf, CT_DWORD &bufLen)
{
	_AUTOMUTEX(m_pMutex);
	CT_DWORD len = m_FriendRSP.size() * sizeof(CMD_G_Friend_RSP);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	bufLen = 0;
	std::map<CT_DWORD, CMD_G_Friend_RSP>::iterator it = m_FriendRSP.begin();
	for (int i = 0; it != m_FriendRSP.end(); ++it, ++i)
	{
		memcpy(pBuf + sizeof(CMD_G_Friend_RSP) * i, &(it->second), sizeof(CMD_G_Friend_RSP));
		bufLen += sizeof(CMD_G_Friend_RSP);

	}

	m_FriendRSP.clear();
	m_RSPCount = 0;

	return CT_TRUE;
}

CT_VOID CCUserFriend::InsertFriendRSP(const CT_DWORD dwSubID, const CMD_G_Friend_RSP &data)
{
	_AUTOMUTEX(m_pMutex);
	std::map<CT_DWORD, CMD_G_Friend_RSP>::iterator it = m_FriendRSP.find(data.dwF_UID);
	if (it == m_FriendRSP.end())
	{
		m_FriendRSP.insert(std::map<CT_DWORD, CMD_G_Friend_RSP>::value_type(data.dwF_UID, data));
	}

	if (m_RSPCount == m_FriendRSP.size())
	{
		CT_CHAR buf[1024*15] = {0};
		CT_DWORD len = 1024*15;
		if (GetFriendRSP(buf, len))
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_FRIEND_EVENT,dwSubID, buf, len);
		}
	}
}

CT_VOID CCUserFriend::UserExitEvent(CT_INT64 iExitTime)
{
	_AUTOMUTEX(m_pMutex);
	if (iExitTime = 0)
	{
		iExitTime = time(0);
	}

	FD_IT it = m_FriendDataMap.begin();
	FD_IT it_end = m_FriendDataMap.end();
	while (it != it_end)
	{
//		if (CCUserManager::getSingleton().Find(it->first))
//		{
			MSC_FriendExit friendExit = {0};
			friendExit.dwUserID = it->first;	//交换UID
			friendExit.dwFriendID = it->second.dwUserID;
			friendExit.iExitTime = iExitTime;
			friendExit.iFType = FRIEND_DATA;
			CT_DWORD threadID = GAME_THREAD_CITY + friendExit.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(threadID,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_EXIT_EVENT,(CT_CHAR*)&friendExit,sizeof(MSC_FriendExit));
//		}
		it++;
	}

	std::set<CT_INT32>::iterator itRev = m_BlackNameRev.begin();
	std::set<CT_INT32>::iterator itRev_end = m_BlackNameRev.end();
	while (itRev != itRev_end)
	{
//		if (CCUserManager::getSingleton().Find(it->first))
//		{
			MSC_FriendExit friendExit = {0};
			friendExit.dwUserID = *itRev;	//交换UID
			friendExit.dwFriendID = m_pUser->GetUserID();
			friendExit.iExitTime = iExitTime;
			friendExit.iFType = FRIEND_BLACKNAME;
			CT_DWORD threadID = GAME_THREAD_CITY + friendExit.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(threadID,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_EXIT_EVENT,(CT_CHAR*)&friendExit,sizeof(MSC_FriendExit));
//		}
		itRev++;
	}

// 	it = m_FriendReqMap.begin();	好友请求只有单方数据
// 	it_end = m_FriendReqMap.end();
// 	while (it != it_end)
// 	{
// //		if (CCUserManager::getSingleton().Find(it->first))
// //		{
// 		MSC_FriendExit friendExit = {0};
// 		friendExit.dwUserID = it->first;	//交换UID
// 		friendExit.dwFriendID = it->second.dwUserID;
// 		friendExit.iExitTime = iExitTime;
// 		friendExit.iFType = FRIEND_REQ;
// 		CT_DWORD threadID = GAME_THREAD_CITY + friendExit.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
// 		G_SendThreadInfo(threadID,MSC_MAIN_FRIEND_EVENT_DATA,MSC_SUB_FRIEND_EXIT_EVENT,(CT_CHAR*)&friendExit,sizeof(MSC_FriendExit));
// //		}
// 		it++;
// 	}
}

CT_VOID CCUserFriend::UserExitEvent(CT_DWORD dwFriendID, CT_INT64 iExitTime, CT_INT32 iFType)
{
	_AUTOMUTEX(m_pMutex);
	if (iExitTime = 0)
	{
		iExitTime = time(0);
	}

	FD_IT it;
	switch (iFType)
	{
	case FRIEND_DATA:
		it = m_FriendDataMap.begin();
		break;
	case FRIEND_REQ:
		it = m_FriendReqMap.begin();
		break;
	case FRIEND_BLACKNAME:
		it = m_BlackNameDataMap.begin();
		break;
	default:
		return;
	}

	FD_IT it_end;
	switch (iFType)
	{
	case FRIEND_DATA:
		it_end = m_FriendDataMap.end();
		break;
	case FRIEND_REQ:
		it_end = m_FriendReqMap.end();
		break;
	case FRIEND_BLACKNAME:
		it_end = m_BlackNameDataMap.end();
		break;
	default:
		return;
	}

	if (it != it_end)
	{
		it->second.iLastExitTime = iExitTime;
	}

}

CT_VOID CCUserFriend::Broadcast(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	_AUTOMUTEX(m_pMutex);
	for (FD_IT it = m_FriendDataMap.begin(); it != m_FriendDataMap.end(); ++it)
	{
		CCUser *pUser =0;
		if (CCUserManager::getSingleton().Find(it->first, &pUser))
		{
			pUser->GetNetInterface()->Send(mainID,	subID, pBuf, len);
		}
	}
}

CT_VOID CCUserFriend::SetRSPCount(CT_INT32 iCount)
{
	_AUTOMUTEX(m_pMutex);
	m_RSPCount = iCount;
	m_FriendRSP.clear();
}
