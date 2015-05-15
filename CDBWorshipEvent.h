#ifndef CDBWorshipEvent_H_
#define CDBWorshipEvent_H_

#include "CGameMesEvent.h"

struct MSC_DB_WorshipStatus;

class CCDBWorshipEvent : public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBWorshipEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBWorshipEvent();
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID){}
private:
	virtual CT_VOID UpdateWorshipStatus(MSC_DB_WorshipStatus &worshipStatus);
};

#endif
