/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBArmy.h
Version:		1.0
Description:    ���ݿ��󷨷���
Author:			wangchen
Create Date:	2012-10-25
History:        ����DB���¼�   

******************************************************************/
#ifndef CCDBBATTLEARRAY_H_
#define CCDBBATTLEARRAY_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBBattleArray:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;

public:
	CCDBBattleArray(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBBattleArray(void);

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
	//����Ϣ
	CT_VOID DisposeBattleArray(CT_CHAR* pBuf,CT_DWORD len);
};

#endif
