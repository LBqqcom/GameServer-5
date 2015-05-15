/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserExitEvent.h
Version:		1.0
Description:    用户退出服务
Author:			pengwen
Create Date:	2012-11-28
History:        用户退出事件   

******************************************************************/

#ifndef CCUSEREXITEVENT_H_
#define CCUSEREXITEVENT_H_
#include "CGameMesEvent.h"

class CCUser;
class  CCNetInterface;

class CCUserExitEvent : public CCGameMesEvent
{
public:
	CCUserExitEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCUserExitEvent(void);
private:
	//用户离开
	CT_BOOL DisposeLeave(CCNetInterface* pNetHandle,CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len);

private:
	//处理用户消息  最后的2个参数为备用
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len, 
		CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{

	}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{

	}
};

#endif