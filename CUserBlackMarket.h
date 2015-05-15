#ifndef CUserBlackMarket_H_
#define CUserBlackMarket_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

struct MarketPersonalAll;
struct BlackGoodsStatus
{
	CT_INT32	iGoodsID;		//物品id
	CT_INT32	iNum;			//物品数量
};
typedef BlackGoodsStatus	MarketPersonal;

typedef std::map<CT_INT32,BlackGoodsStatus>				BlackGoodsStatusMap;
typedef std::map<CT_INT32,BlackGoodsStatus>::iterator	BlackGoodsStatusMap_IT;

typedef std::map<CT_INT32,MarketPersonal>						MarketPersonalMap;
typedef std::map<CT_INT32,MarketPersonal>::iterator			MP_IT;

typedef std::vector<CT_INT32>						GoodsVector;	//商品的id vector

struct MSC_DB_MarketFunction;
struct MSC_DB_BlackMarketRefresh;

class CCUserBlackMarket
{
	friend class CCUser;
private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;
	
	BlackGoodsStatusMap		m_blackGoodsStatusMap;		//用户黑店商品map
	BlackGoodsStatusMap		m_blackGoodsSeekMap;		//用户探访黑店商品map
	CT_INT64			iLastRefTime;					//最后一次系统刷新时间	zhoujun

	CT_INT32			iMall;							//商城功能状态
	CT_INT32			iBlackMarket;					//黑店功能状态
	CT_INT32			iRefreshTime;					//剩余付费刷新次数
	GoodsVector			m_GoodsIDVector;				//商品的id vector，用来根据权重随机获取物品
	GoodsVector			m_GoodsWeightVector;			//商品权重vector

	MarketPersonalMap	m_MarketPersonalMap;			//个人限购
public:
	CCUserBlackMarket();
	~CCUserBlackMarket();
public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		iMall=0;
		iBlackMarket=0;
		iRefreshTime=0;
		m_blackGoodsStatusMap.clear();
		m_MarketPersonalMap.clear();
		m_blackGoodsSeekMap.clear();
	}

	//清理用户数据
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		iMall=0;
		iBlackMarket=0;
		iRefreshTime=0;
		m_blackGoodsStatusMap.clear();
		m_MarketPersonalMap.clear();
		m_blackGoodsSeekMap.clear();
	}
private:
	CT_INT32 RandGoodsBlackMarket(CT_INT32 iNum);
	CT_VOID	GetGoodsWeightVector(GoodsVector &v_id,GoodsVector &v_weight,CT_INT32 iGoodsIDList[],CT_INT32 &iWeight,CT_INT32 iNum,CT_INT32 iGrade);

public:
	//插入个人限购
	CT_VOID InsertMarketPersonal(const CT_INT32 iGoodsID,const CT_INT32 iNum);
	//获取个人限购
	CT_BOOL GetMarketPersonal(CT_CHAR *pBuf,CT_INT32 &iLen);
	//更新个人限购
	CT_VOID UpdataMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen);
	//购买物品
	CT_INT32 BuyGood(CT_INT32 iGoods,const CT_INT32 iNum);

public:
	//根据随机数获取pos
	CT_INT32 GetPosByRandNum(GoodsVector &v_weight,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand);
	//黑店系统刷新，获取物品推送给每个用户
	CT_VOID RefreshBlackMarket(CT_INT32 iType);
	//黑店购买
	CT_BOOL BlackMarketShopping(CT_INT32 iPosition,CT_INT32 iNum,CT_INT32 iMarketType);
	//探访黑店购买
	CT_BOOL SeekBlackMarketShopping(CT_INT32 iPosition,CT_INT32 iNum,CT_INT32 iMarketType);
	CT_INT32 GetBlackMarketGoodsID(CT_INT32 iPosition);
	CT_INT32 GetSeekBlackMarketGoodsID(CT_INT32 iPosition);
	//消耗用户刷新次数
	CT_BOOL RefreshOnce();
	//判断黑店是否开启
	CT_BOOL	ViewBlackMarket();
	//判断商城是否开启
	CT_BOOL	ViewMallMarket();
	//发送用户黑店信息
	CT_VOID SendBlackMarketInfo();
	//发送用户剩余刷新次数
	CT_VOID SendBlackRefreshTime();
	//开启商店功能
	CT_VOID OpenMallMarketFunction();
	//开启黑店功能
	CT_VOID OpenBlackMarketFucntion();
	//登陆获取商店状态
	CT_VOID GetMarketStatus(MSC_DB_MarketFunction &marketFun);
	//登陆获取黑店状态
	CT_VOID GetBlackMarketInfo(MSC_DB_BlackMarketRefresh &blackMarketInfo);
	//刷新商城和商店物品
	CT_VOID RefreshMarketGoods();
	//购买商品时，刷新商城，商店限量物品个数
	CT_VOID UpdateSingleGoodsStatus(CMD_G_MarketGoodsInfo &marketGoodsInfo);
	//重新计算商店黑点冷却时间
//	CT_VOID RefreshShopTime();;
	//探访黑市
	CT_VOID SendSeekBlackMarketInfo();
};

#endif