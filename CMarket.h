#ifndef CMarket_H_
#define CMarket_H_

#include "CSingleton.h"
#include "CTType.h"
#include <map>
#include "GlobeScriptInterface.h"
#include "CMutex.h"
#include "CUser.h"

//�����޹��ܱ�
struct MarketPersonalAll
{
	CT_INT32 iGoodsID;				//��Ʒid
	CT_INT32 iGoodsCount;			//��������
	CT_INT32 iType;					//��Դ����
	CT_INT32 iNum;					//��Դ��Ŀ
	CT_INT32 iGoodsStatus;			//��Ʒ״̬ENUM_MARKET_PERSONAL
	
};

typedef std::map<CT_INT32 ,CT_INT32>				MarketNormalStatusMap;  //��¼��ͨ�̵�������Ʒ����
typedef std::map<CT_INT32 ,CT_INT32>::iterator		MarketNormalStatusMap_IT;

typedef std::map<CT_INT32 ,CT_INT32>				MarketMallStatusMap;	//��¼�̳�������Ʒ����
typedef std::map<CT_INT32 ,CT_INT32>::iterator		MarketMallStatusMap_IT;

typedef std::map<CT_INT32 ,MarketPersonalAll>				MarketPersonalAllMap;	//��¼�̳Ǹ����޹�
typedef std::map<CT_INT32 ,MarketPersonalAll>::iterator		MarketPersonalAllMap_IT;

class CCMarket : public CCSingleton<CCMarket>
{
private:
	CCMutex   m_mutex; 
public:
	CCMarket();
	~CCMarket();
private:
	MarketNormalStatusMap		m_MarketNormalStatusMap;
	MarketMallStatusMap			m_MarketMallStatusMap;
	MarketPersonalAllMap		m_MarketPersonalAllMap;
	CT_INT64					m_iRefreshBlackMarket;
public:
	//������ͨ�̵�������Ʒ״̬
	CT_BOOL		UpdateMarketGoodsStaus(CT_INT32 iGoodsID,CT_INT32 iNum,CT_INT32 &lastNum);
	//�����̳�������Ʒ״̬
	CT_BOOL		UpdateMallGoodsStatus(CT_INT32 iGoodsID,CT_INT32 iNum,CT_INT32 &lastNum);
	//ˢ���̵���Ʒ״̬
	CT_VOID		RefreshMarketGoodsStatus();
	//ˢ���̳���Ʒ״̬
	CT_VOID		RefreshMallGoodsStatus(CT_BOOL bZero = CT_FALSE);
	//�鿴��ͨ��Ʒ�Ƿ���������Ʒ
	CT_BOOL		ViewMarketGoodsStatus(CT_INT32 iGoodsID);
	//�鿴�̳���Ʒ�Ƿ�����
	CT_BOOL		ViewMallGoodsStatus(CT_INT32 iGoodsID);
	//��ȡ��Ʒ
	CT_BOOL		GetMarketNomalGoods(CT_INT32 iGoodsID,G_MarketNormalGoods *marketNormalGoods);
	CT_BOOL		GetMarketMallGoods(CT_INT32 iGoodsID,G_MarketMallGoods *marketMallGoods);
	CT_BOOL		GetMarketBlackGoods(CT_INT32 iGoodsID, CT_INT32 iPost, CT_INT32 iGrade,G_MarketBlackGoods *marketBlackGoods);
	CT_BOOL		GetMarketBlackPayGoods(CT_INT32 iGoodsID,G_MarketBlackGoods *marketBlackGoods);
	CT_VOID		GetMarketBlackList(CT_INT32 *pGoodsList,CT_INT32 iPost, CT_INT32 iGrade);
	CT_VOID		GetMarketBlackPayList(CT_INT32 *pGoodsList);
	//�û���½�����̵���Ϣ
	CT_VOID		SendUserMarketInfo(CCUser *pUser);
	//�����̳���Ʒ��Ϣ
	CT_VOID		SendMallMarketInfo(CCUser *pUser);
	//������ͨ�̵���Ϣ
	CT_VOID		SendNormalMarketInfo(CCUser *pUser);

	CT_VOID		SetRefreshBlackMarket(CT_INT64 refreshBlackMarket) {_AUTOMUTEX(&m_mutex);  m_iRefreshBlackMarket = refreshBlackMarket;}
	CT_INT64	GetRefreshBlackMarket() {_AUTOMUTEX(&m_mutex);  return m_iRefreshBlackMarket;}

	//���¸����޹�
	CT_VOID		UpdataMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen);
	//�����Ʒ
	CT_BOOL		GetMarketPersonalGoods(CT_INT32 iGoodsID,MarketPersonalAll &mpa);
	//�����Ʒ
	CT_BOOL		GetMarketPersonalGoods(CT_CHAR *pBuf,CT_INT32 &iLen);
};

#endif