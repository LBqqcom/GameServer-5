/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDuplicateEvent.h
Version:		1.0
Description:    阵法事件类
Author:			wangchen
Create Date:	2012-11-8
History:             

******************************************************************/
#ifndef CCBATTLEARRAYEVENT_H_
#define CCBATTLEARRAYEVENT_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;

class CCBattleArrayEvent:public CCGameMesEvent
{
public:
	CCBattleArrayEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCBattleArrayEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	//网络消息
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//线程消息
	CT_VOID DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//打开阵法
	CT_VOID DisposeOpenBattleArray(CT_CHAR* pBuf,CT_DWORD len);
	//阵法信息
	CT_VOID DisposeBattleArrayInfo(CT_CHAR* pBuf,CT_DWORD len);

private:
	//阵法验证
	CT_BOOL	ArmyValidate(CCUser *pUser,CT_INT32 iPosition[POST_COUNT/2]);
};
#endif