#ifndef CDBVipEvent_H_
#define CDBVipEvent_H_

#include "CGameMesEvent.h"

struct MSC_DB_VipApply;
struct MSC_DB_VipGiftGet;

class CCDBVipEvent : public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBVipEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBVipEvent();
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	CT_VOID UpdateVipGiftGet(MSC_DB_VipGiftGet &vipGiftGet);
	CT_VOID	UpdateVipRenewal(MSC_DB_VipApply &vipApply);
	CT_VOID	UpdateVipEnd(CT_DWORD dwUserID);
};

#endif