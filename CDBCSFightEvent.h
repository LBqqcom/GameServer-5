/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFightingThread.h
Version:		1.0
Description:    数据库跨服服务
Author:			wangchen
Create Date:	2013-2-25
History:        处理DB跨服事件   

******************************************************************/
#ifndef CCDBCSFIGHTEVENT_H_
#define CCDBCSFIGHTEVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBCSFightEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBCSFightEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBCSFightEvent(void);

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
	//跨服奖励
	CT_VOID DisposeDBCSReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//个人支持
	CT_VOID DisposeCSSupport(CT_CHAR *pBuf,CT_INT32 iLen);
	//删除奖励
	CT_VOID DisposeDelReward(CT_CHAR *pBuf,CT_INT32 iLen);
	
public:
	//读取奖励
	CT_VOID ReadCSReward();
	//读取个人支持
	CT_VOID ReadCSSupport();
};
#endif
