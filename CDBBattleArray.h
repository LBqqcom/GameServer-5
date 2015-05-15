/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBArmy.h
Version:		1.0
Description:    数据库阵法服务
Author:			wangchen
Create Date:	2012-10-25
History:        处理DB阵法事件   

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
	//阵法信息
	CT_VOID DisposeBattleArray(CT_CHAR* pBuf,CT_DWORD len);
};

#endif
