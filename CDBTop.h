/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBTop.h
Version:		1.0
Description:    ���ݿ����а����
Author:			wangchen
Create Date:	2013-5-24
History:        ����DB���а��¼�   

******************************************************************/
#ifndef CCDBTOP_H_
#define CCDBTOP_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBTop:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBTop(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBTop(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//��������Ϣ �������
	CT_VOID DisposeMain( CT_DWORD subID, CT_CHAR* pBuf,  CT_DWORD len);
	//�õ����а�
	CT_VOID DisposeTopGet();
	//�����������ݵ����ݿ� ��������
	CT_VOID DisposeTopSaveDB(CT_CHAR* pBuf, CT_DWORD len);
	//�������а���״̬
	CT_VOID UpdateTopRewardStatus(CT_CHAR* pBuf, CT_DWORD len);
};

#endif
