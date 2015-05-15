/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBBossEvent.h
Version:		1.0
Description:    BOSS���ݿ�
Author:			zhoujun
Create Date:	2013-12-20
History:        ����DB BOSS�¼�   

******************************************************************/
#ifndef CCDBBOSSEVENT_H_
#define CCDBBOSSEVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBBossEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBBossEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBBossEvent(void);

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
	CT_VOID GetBossDB();

	CT_VOID DisposeTopClear(CT_CHAR* pBuf,CT_DWORD len);	//���ĳ����

	CT_VOID DisposeTopUpdate(CT_CHAR* pBuf,CT_DWORD len);	//

	CT_VOID DisposeRecordAdd(CT_CHAR* pBuf,CT_DWORD len);	//
};
#endif