/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBUserExitEvent.h
Version:		1.0
Description:    ���ݿ��û��˳�����
Author:			pengwen
Create Date:	2012-11-28
History:        ����DB�û��˳��¼�   

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
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len,
		CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{

		char* p = 0;
	}

	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
		char* p = 0;
	}

private:

	//�����˳���Ϣ
	CT_VOID DisposeUserExitEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);


};

#endif
