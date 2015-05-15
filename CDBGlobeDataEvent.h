/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBArmy.h
Version:		1.0
Description:    ���ݿ�ȫ�����ݷ���
Author:			wangchen
Create Date:	2013-10-25
History:        ����DBȫ�������¼�   

******************************************************************/
#ifndef CCCDGLOBEDATA_H_
#define CCCDGLOBEDATA_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBGlobeData:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBGlobeData(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBGlobeData(void);

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
	//ȫ�ּ�¼
	CT_VOID DisposeGlobeData(CT_CHAR *pBuf,CT_INT32 iLen);

public:
	//����ȫ�ֱ���
	CT_VOID DisposeReadGlobeData();
};

#endif
