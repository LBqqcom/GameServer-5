#ifndef	CREGIONEVENT_H_
#define CREGIONEVENT_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCRegionEvent : public CCGameMesEvent
{
public:
	CCRegionEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCRegionEvent(void);
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
	CT_VOID	DisposeRegionUnlock(CT_CHAR* pBuf,CT_DWORD len);
};

#endif