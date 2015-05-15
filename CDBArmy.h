/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBArmy.h
Version:		1.0
Description:    ���ݿ���ӷ���
Author:			wangchen
Create Date:	2012-10-25
History:        ����DB�����¼�   

******************************************************************/
#ifndef CCDBARMY_H_
#define CCDBARMY_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBArmy:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBArmy(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBArmy(void);

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
	//����������Ϣ
	CT_VOID DisposeArmyGrade(CT_CHAR* pBuf,CT_DWORD len);
	//����������Ϣ
	CT_VOID DisposeArmyAdvanced(CT_CHAR* pBuf,CT_DWORD len);
	//���ӿ�����Ϣ
	CT_VOID DisposeArmy(CT_CHAR* pBuf,CT_DWORD len);
};
#endif