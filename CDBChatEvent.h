#ifndef CDBChatEvent_H_
#define CDBChatEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCDBConnector;
class CCDBChatEvent : public CCGameMesEvent
{
public:
	CCDBChatEvent(CCGameMesManager *pMesManager,CT_WORD id,CCDBConnector &dbConnector);
	~CCDBChatEvent(void);
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	CCDBConnector& m_dbConnector;
private:
	CT_VOID DisposeGMMessage();
};

#endif