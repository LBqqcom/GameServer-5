#ifndef CDBEmailEvent_H_
#define CDBEmailEvent_H_

#include "CGameMesEvent.h"

class CCGameMesManager;
class CCDBConnector;

class CCDBEmailEvent:public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBEmailEvent(CCGameMesManager *pMesManager,CT_DWORD id,CCDBConnector& dbConnector);
	~CCDBEmailEvent(){}
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	CT_VOID DisopseUserEmail(CT_CHAR *pBuf,CT_DWORD len);
};

#endif