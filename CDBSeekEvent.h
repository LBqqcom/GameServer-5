#ifndef CDBSEEKEVENT_H_
#define CDBSEEKEVENT_H_

#include "CGameMesEvent.h"

class CCGameMesManager;
#include "CThreadMes.h"

class CCDBConnector;

class CCDBSeekEvnet : public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBSeekEvnet(CCGameMesManager *pMesManager, CT_DWORD id, CCDBConnector& dbConnect);
	~CCDBSeekEvnet();

private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID);
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//城墙加成信息
	CT_VOID DisposeWallAddition(CT_CHAR* pBuf, CT_INT32 iLen);
	//城墙军队
	CT_VOID DisposeWallArmy(CT_CHAR* pBuf, CT_INT32 iLen);
	// 被访者次数更新
	CT_VOID DisposeSeekedTimes(CT_CHAR* pBuf, CT_INT32 iLen);
	// 已探访状态
	CT_VOID DisposeSeekedPlayerInsert(CT_CHAR* pBuf, CT_INT32 iLen);
	// 探访界面更新
	CT_VOID DisposeSeekListUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// 更新被探访者的基本信息
	CT_VOID DisposeSeekedDataUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// 更新探访者信息
	CT_VOID DisposeSeekDateUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// 更新战旗
	CT_VOID DisposeTeamFlag(CT_CHAR* pBuf, CT_INT32 iLen);
	// 更新礼物
	CT_VOID DisposeSeekPresent(CT_CHAR* pBuf, CT_INT32 iLen);
	// 删除礼物
	CT_VOID DisposeSeekPresentDel(CT_CHAR* pBuf, CT_INT32 iLen);
	// 删除战旗
	CT_VOID DisposeTeamFlagDelete(CT_CHAR* pBuf, CT_INT32 iLen);
	// 插入人质
	CT_VOID DisposeKidnapUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// 删除人质
	CT_VOID DisposeKidnapDelete(CT_CHAR* pBuf, CT_INT32 iLen);
	// 添加访客记录
	CT_VOID DisposeSeekVistorInfo(CT_CHAR* pBuf, CT_INT32 iLen);
	//添加探访战报
	CT_VOID DisposeSeekFightRecord(CT_CHAR* pBuf, CT_INT32 iLen);
	// 记录掠夺的金币和粮食
	CT_VOID DisposePlunderResUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
};
#endif