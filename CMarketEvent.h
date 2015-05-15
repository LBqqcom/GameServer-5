#ifndef CMarketEvent_H_
#define CMarketEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCMarketEvent:public CCGameMesEvent
{
private:


public:
	CCMarketEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCMarketEvent();
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	CT_VOID DisposeShopping(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID	DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len);
	CT_BOOL DisposeNormalMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo);
	CT_BOOL DisposeMallMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo);
	CT_BOOL DisposeBlackMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo);
	CT_BOOL DisposeSeekBlackMarketShopping(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo);
	CT_BOOL DisposeMarketPersonal(CCUser *pUser,CMD_G_MarketShoppingInfo *pGoodsInfo);
	//��������
	CT_VOID DisposeBuyStatmina(CT_CHAR* pBuf,CT_DWORD len);
};

#endif