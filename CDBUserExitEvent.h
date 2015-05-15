/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBUserExitEvent.h
Version:		1.0
Description:    数据库用户退出服务
Author:			pengwen
Create Date:	2012-11-28
History:        处理DB用户退出事件   

******************************************************************/

#ifndef CCDBUSEREXITEVENT_H_
#define CCDBUSEREXITEVENT_H_
#include "CGameMesEvent.h"
#include "CThreadMes.h"

class CCDBConnector;
class CCDBUserExitEvent  :
	public CCGameMesEvent
{
private: 
	CCDBConnector& m_dbConnector;

public:
	CCDBUserExitEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBUserExitEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len,
		CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{

		char* p = 0;
	}

	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
		char* p = 0;
	}

private:

	//处理退出消息
	CT_VOID DisposeUserExitEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);


};

#endif
