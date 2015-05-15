/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		#include "CCustomerFeedBackEvent.h"
Version:		1.0
Description:    ��Ϣ����
Author:			wangchen
Create Date:	2013-4-11
History:        ��Ϣ����   

******************************************************************/

#ifndef CCCUSTOMERFEEDBACKEVENT_H_
#define CCCUSTOMERFEEDBACKEVENT_H_
#include "CGameMesEvent.h"

class CCCustomerFeedBackEvent : public CCGameMesEvent
{
public:
	CCCustomerFeedBackEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCCustomerFeedBackEvent(void);

private:
	//�����û���Ϣ  ����2������Ϊ����
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len, 
		CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{

	}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{

	}

private:
	//��Ϣ����
	CT_VOID DisposeFeedBack(CT_CHAR *pBuf,const CT_INT32 iLen);

};

#endif