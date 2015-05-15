/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDataBaseThread.h
Version:		1.0
Description:    日志数据库服务
Author:			wangchen
Create Date:	2012-2-4
History:        处理数据库事件     

******************************************************************/

#ifndef CCLOGDATABASETHREAD_H_
#define CCLOGDATABASETHREAD_H_
#include "CTaskThread.h"
#include "CDBConnector.h"
#include "CGameMesEvent.h"

class CCLogDBThread:public CCTaskThread
{
private:
	//日志事件处理器
	CCGameMesEvent*  m_pLogDBEvent;
private:
	CCDBConnector m_dbConnector; 
public:
	CCLogDBThread(CT_VOID);
	~CCLogDBThread(CT_VOID);

public:
	//打开
	virtual CT_VOID OnOpen(); 
	//定时器
	CT_VOID OnTimer(CT_DWORD timeID);
	CT_VOID OnClock(CT_DWORD timeID){return;};
};
#endif