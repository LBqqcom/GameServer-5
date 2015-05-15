/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingThread.h
Version:		1.0
Description:    战斗线程
Author:			wangchen
Create Date:	2012-9-13
History:        处理战斗事件     

******************************************************************/

#ifndef CCFIGHTINGTHREAD_H_
#define CCFIGHTINGTHREAD_H_
#include <cassert>
#include "CTaskThread.h"
#include "CGameMesEvent.h"

class CCFightingThread:public CCTaskThread
{
private:
	//战斗处理器
	CCGameMesEvent* m_pFightingEvent;

public:
	CCFightingThread(void);
	~CCFightingThread(void);

public:
	//打开
	virtual CT_VOID OnOpen(); 
	
};
#endif