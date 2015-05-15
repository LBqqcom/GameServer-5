#ifndef CDBMarketEvent_H_
#define CDBMarketEvent_H_

#include "CGameMesEvent.h"

class CCGameMesManager;
class CCDBConnector;

class CCDBMarketEvent : public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBMarketEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBMarketEvent(){}
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//CT_VOID	UpdateMarketShoppingData(MSC_DB_ShoppingRecord &shoppingRecord);
	CT_VOID RefreshBlackMarketDB(MSC_DB_BlackMarketRefresh &reBlackMarket);
	CT_VOID BlackMarketShopping(MSC_DB_BlackMarketShopping &blackShopping);
	CT_VOID	UpdateMarketFunction(MSC_DB_MarketFunction &marketFunc);
	CT_VOID DisposeMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen);
	//更新个人限购
	CT_VOID UpdataMarketPersonal();
};

#endif