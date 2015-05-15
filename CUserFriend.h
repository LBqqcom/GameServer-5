#ifndef CFriend_H_
#define CFriend_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

// struct FriendData
// {
// 	CT_DWORD	dwUserID;					// id
// 	CT_DWORD	dwF_UID;					// 好友ID
// 	CT_CHAR		cF_NickName[NAME_SIZE];		// 昵称
// 	CT_INT32	iF_Occupation;				// 职业
// 	CT_INT32	iF_Grade;					// 等级
// 	CT_INT32	iF_Status;					// 在线状态
// 	CT_INT32	iF_FightingPower;			// 战斗力(离线状态不计算战斗力)
// 	CT_INT64	iLastExitTime;				// 最后下线时间
// };

typedef CMD_G_FriendData FriendData;

enum
{
	FRIEND_REQ			=	(1),		//好友请求
	FRIEND_DATA			=	(2),		//好友
	FRIEND_BLACKNAME	=	(3),		//黑名单
	FRIEND_BLACKREV		=	(4),		//反向黑名单
};
 


class CCUserFriend
{
private:
	CCMutex*    m_pMutex;
	CCUser*     m_pUser;
	typedef std::map<CT_DWORD, FriendData>					FriendDataMap;			// 好友数据容器
	typedef std::map<CT_DWORD, FriendData>::iterator		FD_IT;
	FriendDataMap											m_FriendDataMap;		// 好友列表
	FriendDataMap											m_FriendReqMap;			// 好友请求列表
	FriendDataMap											m_BlackNameDataMap;		// 黑名单列表
	std::set<CT_INT32>										m_BlackNameRev;			// 反向黑名单	自己在其他用户黑名单列表
	CT_BOOL													m_bEnable;

	std::map<CT_DWORD, CMD_G_Friend_RSP>					m_FriendRSP;
	CT_INT32												m_RSPCount;
public:
	CCUserFriend();
	~CCUserFriend(void);

	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

	CT_BOOL IsEnable() {_AUTOMUTEX(m_pMutex);   return m_bEnable;}
	
	CT_BOOL GetFriendData(CT_CHAR *pBuf, CT_INT32 &bufLen);			// 得到好友信息
	
	CT_VOID InsertFriendData(const CMD_G_FriendData &data);			// 插入好友数据
	
	CT_BOOL GetFriendReq(CT_CHAR *pBuf, CT_INT32 &bufLen);			// 得到好友请求数据
	
	CT_VOID InsertFriendReq(const CMD_G_FriendData &data);			// 插入好友请求数据

	CT_BOOL GetBlackName(CT_CHAR *pBuf, CT_INT32 &bufLen);			// 得到黑名单数据
	
	CT_VOID InsertBlackName(const CMD_G_FriendData &data);			// 插入黑名单数据

	CT_VOID InsertBlackRev(const CT_INT32 iUID);					// 插入反向黑名单

	CT_VOID DeleteFriend(CT_INT32 iF_UID, CT_INT32 iType);			// 删除请求/好友/黑名单(内存)

	CT_BOOL CheckBlackList(CT_INT32 iF_UID);						// 检查用户黑名单列表

	CT_BOOL CheckIsFriend(CT_INT32 iF_UID);							// 检查是否已经是自己的好友

	CT_BOOL GetFriendList(CT_DWORD *dwFList, CT_INT32 *iGradeList, CT_INT64 *iExitTimeList, CT_INT32 &iCount);		// 

	CT_BOOL CheckFriendLimit(CT_INT32 iType);						// 检查好友/黑名单已满

	CT_BOOL GetFriendReq(CT_DWORD iF_UID, CMD_G_FriendData **pFriendData);			// 得到好友请求数据 by FUID

	CT_BOOL GetFriendRSP(CT_CHAR *pBuf, CT_DWORD &bufLen);
	CT_VOID InsertFriendRSP(const CT_DWORD dwSubID, const CMD_G_Friend_RSP &data);

	CT_VOID UserExitEvent(CT_INT64 iExitTime = 0);					//自己退出
	CT_VOID UserExitEvent(CT_DWORD dwFriendID, CT_INT64 iExitTime = 0, CT_INT32 iFType = FRIEND_DATA);		//好友退出

	CT_VOID Broadcast(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);	//好友广播
	CT_VOID SetRSPCount(CT_INT32 iCount);
};

#endif