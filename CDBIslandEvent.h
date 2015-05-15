/****************************************************
	created:	2014/07/22		14:02
	file base:	DBIslandEvent
	file ext:	h
	author:		zhouhuan

	purpose:
*****************************************************/

#ifndef CDBISLANDEVENT_H
#define CDBISLANDEVENT_H

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBIslandEvent: public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;

public:
	CCDBIslandEvent(CCGameMesManager *pMesManager, CT_DWORD id, CCDBConnector& dbConnect);
	~CCDBIslandEvent();

private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID);
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	CT_VOID UpdateIslandData(CT_CHAR* pBuf,CT_INT32 iLen);
	CT_VOID AddIslandGiveTimes(CT_CHAR* pBuf,CT_INT32 iLen);
	CT_VOID AddIslandScoreReward(CT_CHAR* pBuf,CT_INT32 iLen);
	// 处理队友不在线时赠送次数
	CT_VOID DisposeGiveTimes(CT_CHAR* pBuf, CT_INT32 iLen);
	// 处理队友不在线时更新战队积分
	CT_VOID UpdateFTMemberScore(CT_CHAR* pBuf, CT_INT32 iLen);
	// 每天六点更新龙岛
	CT_VOID UpdateIslandOnClock();

};

#endif