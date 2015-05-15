#ifndef  SERVEREVNT_H_
#define  SERVEREVNT_H_

#include "CGameMesEvent.h"

class CServerEvent:public CCGameMesEvent
{
private:
	CT_INT32	m_iPlayerCount;				//游戏人数

public:
	CServerEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CServerEvent(void);

public:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len,
		CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

	//事件处理
private:
	//服务注册完成
	CT_BOOL	ServerRegisterFinish(CT_CHAR* pBuf,CT_DWORD len,CCNetInterface* pNetHandle);
};

#endif
