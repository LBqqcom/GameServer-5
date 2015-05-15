/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CBuildFunEvent.h
Version:		1.0
Description:    处理城市建筑功能DB事件
Author:			pengwen
Create Date:	2012-9-18
History:        处理城市建筑功能DB事件     

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
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf,   CT_DWORD len, CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){ }
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//保存数据
	CT_VOID DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,   CT_DWORD len);
	//保存生产数据
	CT_VOID DisposeSaveProductingData(const MSC_DB_SaveProductingData& pData);
	//同步生产的建筑
	CT_VOID DisposeSysProductingData(const MSC_DB_SysProductingData& sysProductingData);


public:
	CCDBBuildFunEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector );
	~CCDBBuildFunEvent(void);
};

#endif
