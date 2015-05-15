/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDataEvent.h
Version:		1.0
Description:    用户数据事件类
Author:			wangchen
Create Date:	2014-1-8
History:             

******************************************************************/
#ifndef CCUSERDATAEVENT_H_
#define CCUSERDATAEVENT_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUserDataEvent:public CCGameMesEvent
{
public:
	CCUserDataEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCUserDataEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){};
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//插入数据
	CT_VOID DisposeInsertData(CT_CHAR *pBuf,CT_DWORD len);
	//插入被访者信息
	CT_VOID DisposeSeekedInfo(CT_CHAR* pBuf, CT_INT32 iLen);
};
#endif
