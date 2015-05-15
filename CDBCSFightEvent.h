/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFightingThread.h
Version:		1.0
Description:    ���ݿ�������
Author:			wangchen
Create Date:	2013-2-25
History:        ����DB����¼�   

******************************************************************/
#ifndef CCDBCSFIGHTEVENT_H_
#define CCDBCSFIGHTEVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBCSFightEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBCSFightEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBCSFightEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){};
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//�������
	CT_VOID DisposeDBCSReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//����֧��
	CT_VOID DisposeCSSupport(CT_CHAR *pBuf,CT_INT32 iLen);
	//ɾ������
	CT_VOID DisposeDelReward(CT_CHAR *pBuf,CT_INT32 iLen);
	
public:
	//��ȡ����
	CT_VOID ReadCSReward();
	//��ȡ����֧��
	CT_VOID ReadCSSupport();
};
#endif
