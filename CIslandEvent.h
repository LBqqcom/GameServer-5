/****************************************************
	created:	2014/07/10		10:06
	file base:	IslandEvent
	file ext:	h
	author:		zhouhuan

	purpose:
*****************************************************/

#ifndef CISLANDEVENT_H
#define CISLANDEVENT_H

#include "CGameMesEvent.h"

class CCIslandEvent : public CCGameMesEvent
{
public:
	CCIslandEvent(CCGameMesManager* pMsg, CT_DWORD iID);
	~CCIslandEvent();

private:
	//处理用户信息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	// 处理投掷骰子
	CT_VOID DisposeThrow(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理事件
	CT_VOID DisposeFightEvent(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理立即完成
	CT_VOID DisposeCompleteImmediately(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理放弃事件
	CT_VOID DisposeEventGiveup(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理奖励领取
	CT_VOID DisposeReward(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理投掷次数赠送
	CT_VOID DisposeGiveThrowTimes(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理战斗事件结果
	CT_VOID DisposeFightOver(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理离线赠送失败
	CT_VOID DisposeOutLineGiveFail(CT_CHAR* pBuf, CT_INT32 iLen);
	// 开启龙岛功能
	CT_VOID DisposeOpenIsland(CT_DWORD dwUserID);
	// 处理任务完成
	CT_VOID DisposeTaskComplete(CT_CHAR* pBuf, CT_INT32 iLen);

};

#endif 