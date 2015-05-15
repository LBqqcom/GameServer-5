/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFriendEvent.h
Version:		1.0
Description:    好友数据库
Author:			zhoujun
Create Date:	2013-04-25
History:        处理DB好友事件   

******************************************************************/
#ifndef CCDBFRIENDEVENT_H_
#define CCDBFRIENDEVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBFriendEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBFriendEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBFriendEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	CT_VOID DisposeFriendUpdate(CT_CHAR* pBuf,CT_DWORD len);	//增加/更改请求/好友/黑名单
	
//	CT_VOID DisposeFriendGet(CT_CHAR* pBuf,CT_DWORD len);		//获取请求/好友/黑名单信息
	
	CT_VOID DisposeFriendDelete(CT_CHAR* pBuf,CT_DWORD len);	//删除请求/好友/黑名单

	CT_VOID DisposeFriendFind(CT_CHAR* pBuf, CT_DWORD len);		//添加新好友时,根据名称得到UID

	CT_VOID DisposeFriendFindBlack(CT_CHAR* pBuf, CT_DWORD len);		//添加黑名单时,根据名称得到UID

	CT_VOID DisposeFriendGetTitle(CT_CHAR* pBuf, CT_DWORD len);	//获得不在线好友的头衔信息
};
#endif