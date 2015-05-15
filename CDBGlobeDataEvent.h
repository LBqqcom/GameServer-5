/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBArmy.h
Version:		1.0
Description:    数据库全局数据服务
Author:			wangchen
Create Date:	2013-10-25
History:        处理DB全局数据事件   

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
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){};
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//全局记录
	CT_VOID DisposeGlobeData(CT_CHAR *pBuf,CT_INT32 iLen);

public:
	//插入全局变量
	CT_VOID DisposeReadGlobeData();
};

#endif
