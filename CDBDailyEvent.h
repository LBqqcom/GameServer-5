/********************************************************************
	created:	2012/11/14
	created:	14:11:2012   15:17
	file base:	CDBDailyEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#ifndef CDBDAILYEVENT_H_
#define CDBDAILYEVENT_H_

#include "CGameMesEvent.h"

class CCDBConnector;
class CCGameMesManager;
struct MSC_DB_W_SignInfo;
struct MSC_DB_W_DailyQuest;
struct MSC_DB_W_ActiveInfo;
struct MSC_DB_W_SignReward;
struct MSC_DB_W_ActiveReward;
struct MSC_DB_D_SignInfo;
struct MSC_DB_D_DailyInfo;

class CCDBDailyEvent : public CCGameMesEvent
{
private:
    CCDBConnector& m_dbConnector;
public:
    CCDBDailyEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
    ~CCDBDailyEvent(void);
private:
    //处理用户消息
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //用户定时器处理
    virtual CT_VOID OnTime(CT_DWORD timeID){  }
    //用户时钟处理
    virtual CT_VOID OnClock(CT_DWORD timeID){  }

    // 处理信息保存
    CT_VOID DisposeSaveData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len);
    // 处理信息删除
    CT_VOID DisposeDelData(CT_DWORD subID, CT_CHAR* pBuf,	CT_DWORD len);
private:
    // 记录签到信息
    CT_VOID DisposeSaveData(const MSC_DB_W_SignInfo &info);
    // 记录日常进度
    CT_VOID DisposeSaveData(const MSC_DB_W_DailyQuest &daily);
    // 记录活跃度信息
    CT_VOID DisposeSaveData(const MSC_DB_W_ActiveInfo &info);

    // 记录领取活跃度奖励
    CT_VOID DisposeSaveData(const MSC_DB_W_ActiveReward &reward);

    // 清除连续登陆记录
    CT_VOID DisposeDelData(const MSC_DB_D_SignInfo &info);
    // 清除日常记录
    CT_VOID DisposeDelData(const MSC_DB_D_DailyInfo &info);
};
#endif // CDBDAILYEVENT_H_