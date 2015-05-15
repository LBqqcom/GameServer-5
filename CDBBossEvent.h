/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBBossEvent.h
Version:		1.0
Description:    BOSS数据库
Author:			zhoujun
Create Date:	2013-12-20
History:        处理DB BOSS事件   

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
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	CT_VOID GetBossDB();

	CT_VOID DisposeTopClear(CT_CHAR* pBuf,CT_DWORD len);	//清除某个榜

	CT_VOID DisposeTopUpdate(CT_CHAR* pBuf,CT_DWORD len);	//

	CT_VOID DisposeRecordAdd(CT_CHAR* pBuf,CT_DWORD len);	//
};
#endif