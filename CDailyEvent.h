/********************************************************************
	created:	2012/11/14
	created:	14:11:2012   15:18
	file base:	CDailyEvent
	file ext:	h
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#ifndef CDAILYEVENT_H_
#define CDAILYEVENT_H_
#include "CGameMesEvent.h"
#include "CUserManager.h"

class CCDailyEvent : public CCGameMesEvent
{
public:
    CCDailyEvent(CCGameMesManager* pMesManager, CT_DWORD id);
    ~CCDailyEvent(void);
private:
    //处理用户消息
    virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
    //用户定时器处理
    virtual CT_VOID OnTime(CT_DWORD timeID);
    //用户时钟处理
    virtual CT_VOID OnClock(CT_DWORD timeID);
private:
    //处理日常事件
    CT_VOID DisposeDailyEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
private:
    // 处理领取签到奖励
    CT_VOID DisposeSignReward(const CMD_G_SignReward &reward);
    // 处理领取日常奖励
    CT_VOID DisposeDailyReward(const CMD_G_ActiveReward &reward);
};
#endif // CDAILYEVENT_H_