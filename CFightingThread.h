/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingThread.h
Version:		1.0
Description:    ս���߳�
Author:			wangchen
Create Date:	2012-9-13
History:        ����ս���¼�     

******************************************************************/

#ifndef CCFIGHTINGTHREAD_H_
#define CCFIGHTINGTHREAD_H_
#include <cassert>
#include "CTaskThread.h"
#include "CGameMesEvent.h"

class CCFightingThread:public CCTaskThread
{
private:
	//ս��������
	CCGameMesEvent* m_pFightingEvent;

public:
	CCFightingThread(void);
	~CCFightingThread(void);

public:
	//��
	virtual CT_VOID OnOpen(); 
	
};
#endif