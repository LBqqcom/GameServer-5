/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFriendEvent.h
Version:		1.0
Description:    �������ݿ�
Author:			zhoujun
Create Date:	2013-04-25
History:        ����DB�����¼�   

******************************************************************/
#ifndef CCDBFRIENDEVENT_H_
#define CCDBFRIENDEVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBFriendEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBFriendEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBFriendEvent(void);

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
	CT_VOID DisposeFriendUpdate(CT_CHAR* pBuf,CT_DWORD len);	//����/��������/����/������
	
//	CT_VOID DisposeFriendGet(CT_CHAR* pBuf,CT_DWORD len);		//��ȡ����/����/��������Ϣ
	
	CT_VOID DisposeFriendDelete(CT_CHAR* pBuf,CT_DWORD len);	//ɾ������/����/������

	CT_VOID DisposeFriendFind(CT_CHAR* pBuf, CT_DWORD len);		//����º���ʱ,�������Ƶõ�UID

	CT_VOID DisposeFriendFindBlack(CT_CHAR* pBuf, CT_DWORD len);		//��Ӻ�����ʱ,�������Ƶõ�UID

	CT_VOID DisposeFriendGetTitle(CT_CHAR* pBuf, CT_DWORD len);	//��ò����ߺ��ѵ�ͷ����Ϣ
};
#endif