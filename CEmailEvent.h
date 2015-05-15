#ifndef CEmailEvent_H_
#define CEmailEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCEmailEvent: public CCGameMesEvent
{
public:
	CCEmailEvent(CCGameMesManager *pMesManager,CT_DWORD id);
	~CCEmailEvent();
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID){}
private:
	CT_VOID DisposeUserEmail(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID GetEmailSendResult(CT_CHAR *pBuf,CT_DWORD len);
	CT_VOID TransformMes(CT_CHAR *pBuf,CT_INT32 len,CT_DWORD aimUserID);
};

#endif