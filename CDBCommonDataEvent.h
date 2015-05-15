/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBCityUserBuildEvent.h
Version:		1.0
Description:    基本数据db类，保存基本数据，而且数据不需通知调用服务者
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
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){  }
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//保存领主数据
	CT_VOID DisposeMasterSaveData(const MSC_DB_MasterSaveData& masterSaveData);
	CT_VOID DisposeMasterSaveDataEx(const MSC_DB_MasterSaveDataEx& masterSaveDataEx);
    // 保存冷却信息
    CT_VOID DisposeSaveData(const MSC_DB_W_CooldownData &data);
	//修改领主名称
	CT_VOID DisposeChangeMasterName(MSC_DB_MasterName &mn);
	//保存领主加成数据
	CT_VOID DisposeMasterFightSaveData(const MSC_DB_MasterFightSaveData& mfsd);
};

#endif

