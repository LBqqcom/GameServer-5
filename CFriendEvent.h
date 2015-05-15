#ifndef CCFRIENDEVNET_H_
#define CCFRIENDEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCFriendEvent:public CCGameMesEvent
{
public:
	CCFriendEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCFriendEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	
private:
	CT_VOID DisposeFriendRequestBat(CT_CHAR* pBuf,CT_DWORD len);	//一键好友请求(批量)
	CT_VOID DisposeFriendRequest(CT_CHAR* pBuf,CT_DWORD len);		//好友请求
	CT_VOID DisposeFriendRequest_S(CT_CHAR* pBuf,CT_DWORD len);		//好友请求 对端
	CT_VOID DisposeFriendFind(CT_CHAR* pBuf,CT_DWORD len);			//从数据库查找对方信息,by F_NickName

	CT_VOID DisposeFriendAcceptBat(CT_CHAR* pBuf,CT_DWORD len);		//一键接受好友请求(批量)
	CT_VOID DisposeFriendAccept(CT_CHAR* pBuf,CT_DWORD len);		//接受好友请求
	CT_VOID DisposeFriendAccept_S(CT_CHAR* pBuf,CT_DWORD len);		//接受好友请求 对端
	CT_VOID DisposeFriendAcceptEX(CT_CHAR* pBuf,CT_DWORD len);		//接受好友请求并添加对方为好友

	CT_VOID DisposeFriendRejectBat(CT_CHAR* pBuf,CT_DWORD len);		//一键拒绝好友请求(批量)
	CT_VOID DisposeFriendReject(CT_CHAR* pBuf,CT_DWORD len);		//拒绝好友请求
	CT_VOID DisposeFriendReject_S(CT_CHAR* pBuf,CT_DWORD len);		//拒绝好友请求 对端

	CT_VOID DisposeFriendDelete(CT_CHAR* pBuf,CT_DWORD len);		//删除好友
	CT_VOID DisposeFriendDelete_S(CT_CHAR* pBuf,CT_DWORD len);		//删除好友 对端

	CT_VOID DisposeFriendBlack(CT_CHAR* pBuf,CT_DWORD len);			//加入黑名单
	CT_VOID DisposeFriendBlack_S(CT_CHAR* pBuf,CT_DWORD len);		//加入黑名单 对端
	CT_VOID DisposeFriendFindBlack(CT_CHAR* pBuf,CT_DWORD len);		//从数据库查找对方信息,by F_NickName

	CT_VOID DisposeFriendBlackDelete(CT_CHAR* pBuf,CT_DWORD len);	//删除黑名单
//	CT_VOID DisposeFriendBlackDelete_S(CT_CHAR* pBuf,CT_DWORD len);	//删除黑名单 对端

	CT_VOID DisposeFriendPush(CT_CHAR* pBuf,CT_DWORD len);			//推送好友
	CT_VOID DisposeFriendLine(CT_CHAR* pBuf, CT_DWORD len);			//获取在线状态

	CT_VOID DisposeFriendNoLinkSend(CT_CHAR* pBuf, CT_DWORD len);	//打开界面时，发送不在线好友的信息

	CT_VOID DisposeFriendExit(CT_CHAR* pBuf, CT_DWORD len);	
};
#endif