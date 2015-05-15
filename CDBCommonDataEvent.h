/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBCityUserBuildEvent.h
Version:		1.0
Description:    ��������db�࣬����������ݣ��������ݲ���֪ͨ���÷�����
Author:			pengwen
Create Date:	2012-8-16
History:             

******************************************************************/
#ifndef CCDBCOMMONDATAEVENT_H_
#define CCDBCOMMONDATAEVENT_H_

#include "CGameMesEvent.h"
#include "CThreadMes.h"

class CCDBConnector;
class CCGameMesManager;
struct MSC_DB_MasterSaveData;
struct MSC_DB_MasterSaveDataEx;
struct MSC_DB_W_CooldownData;
struct MSC_DB_MasterName;

class CCDBCommonDataEvent :
	public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBCommonDataEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBCommonDataEvent(void);
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){  }
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//������������
	CT_VOID DisposeMasterSaveData(const MSC_DB_MasterSaveData& masterSaveData);
	CT_VOID DisposeMasterSaveDataEx(const MSC_DB_MasterSaveDataEx& masterSaveDataEx);
    // ������ȴ��Ϣ
    CT_VOID DisposeSaveData(const MSC_DB_W_CooldownData &data);
	//�޸���������
	CT_VOID DisposeChangeMasterName(MSC_DB_MasterName &mn);
	//���������ӳ�����
	CT_VOID DisposeMasterFightSaveData(const MSC_DB_MasterFightSaveData& mfsd);
};

#endif

