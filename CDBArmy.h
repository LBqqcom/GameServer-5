/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBArmy.h
Version:		1.0
Description:    数据库军队服务
Author:			wangchen
Create Date:	2012-10-25
History:        处理DB军队事件   

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
	//军队升级消息
	CT_VOID DisposeArmyGrade(CT_CHAR* pBuf,CT_DWORD len);
	//军队升级消息
	CT_VOID DisposeArmyAdvanced(CT_CHAR* pBuf,CT_DWORD len);
	//军队开启消息
	CT_VOID DisposeArmy(CT_CHAR* pBuf,CT_DWORD len);
};
#endif