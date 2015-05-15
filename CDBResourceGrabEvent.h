/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBResourceGrabEvent.h
Version:		1.0
Description:    ��Դ�������ݿ�
Author:			pengwen
Create Date:	2013-05-13
History:        ����DB�����¼�   

******************************************************************/

#ifndef CCDBRESOURCEGRABEVENT_H_
#define CCDBRESOURCEGRABEVENT_H_

#include "CGameMesEvent.h"

class CCDBResourceGrabEvent :
	public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBResourceGrabEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBResourceGrabEvent();
private:
	//�û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	//��ʱ��
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);
	//�õ�ȫ����Դ��Ϣ
	CT_VOID GetAllResSpot();
	//�õ�ȫ����Դ������Ϣ
	CT_VOID GetAllResSpotProduct();
	//������Դ�������ݵ����ݿ�
	CT_VOID SaveResourceData(CT_CHAR* pBuf, CT_DWORD len);
	//��ȡ��Դ��
	CT_VOID DisposeResActive();

	CT_VOID GetAllScene();
};

#endif