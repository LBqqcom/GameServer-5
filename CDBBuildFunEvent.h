/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CBuildFunEvent.h
Version:		1.0
Description:    ������н�������DB�¼�
Author:			pengwen
Create Date:	2012-9-18
History:        ������н�������DB�¼�     

******************************************************************/


#ifndef CCDBBUILDFUNEVENT_H_
#define CCDBBUILDFUNEVENT_H_

#include "CGameMesEvent.h"

class  CCDBConnector;
struct MSC_DB_SaveProductingData;
struct MSC_DB_SysProductingData;
class CCDBBuildFunEvent :
	public CCGameMesEvent
{
private: 
	CCDBConnector& m_dbConnector;
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf,   CT_DWORD len, CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){ }
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//��������
	CT_VOID DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,   CT_DWORD len);
	//������������
	CT_VOID DisposeSaveProductingData(const MSC_DB_SaveProductingData& pData);
	//ͬ�������Ľ���
	CT_VOID DisposeSysProductingData(const MSC_DB_SysProductingData& sysProductingData);


public:
	CCDBBuildFunEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector );
	~CCDBBuildFunEvent(void);
};

#endif
