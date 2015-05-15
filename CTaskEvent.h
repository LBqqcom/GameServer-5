#ifndef CTaskEvent_H_
#define CTaskEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCTaskEvent : public CCGameMesEvent
{
public:
	CCTaskEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCTaskEvent(void);
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//处理奖励请求
	CT_VOID DisposeRewardRequest(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeTaskFastComplete(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeTaskInquiry(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DispostTaskRole(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeFightTeamUpgrade(CT_CHAR* pBuf, CT_DWORD len);
};

#endif