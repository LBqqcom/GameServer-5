#ifndef CDBRegionEvent_H_
#define CDBRegionEvent_H_

#include "CGameMesEvent.h"

class CCDBConnector;
class CCGameMesManager;
struct MSC_DB_W_RegionUnlockData;

class CCDBRegionData : public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBRegionData(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBRegionData(void);
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	CT_VOID DisposeRegionDataSave(const MSC_DB_W_RegionUnlockData& regionUnlockData);
};

#endif