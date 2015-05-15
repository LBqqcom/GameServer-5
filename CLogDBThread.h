/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDataBaseThread.h
Version:		1.0
Description:    ��־���ݿ����
Author:			wangchen
Create Date:	2012-2-4
History:        �������ݿ��¼�     

******************************************************************/

#ifndef CCLOGDATABASETHREAD_H_
#define CCLOGDATABASETHREAD_H_
#include "CTaskThread.h"
#include "CDBConnector.h"
#include "CGameMesEvent.h"

class CCLogDBThread:public CCTaskThread
{
private:
	//��־�¼�������
	CCGameMesEvent*  m_pLogDBEvent;
private:
	CCDBConnector m_dbConnector; 
public:
	CCLogDBThread(CT_VOID);
	~CCLogDBThread(CT_VOID);

public:
	//��
	virtual CT_VOID OnOpen(); 
	//��ʱ��
	CT_VOID OnTimer(CT_DWORD timeID);
	CT_VOID OnClock(CT_DWORD timeID){return;};
};
#endif