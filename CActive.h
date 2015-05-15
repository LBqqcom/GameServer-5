#ifndef CActive_H_
#define CActive_H_

#include "CSingleton.h"
#include "CTType.h"
#include <map>
#include <vector>
#include "GlobeScriptInterface.h"
#include "CMutex.h"
#include "CUser.h"

//活动描述
struct ActiveDsc
{
	CT_INT32		iType;
	CT_CHAR			cBuf[256];
};

struct TurntableItem
{
	CT_INT32	iOpen;							//是否开启
	CT_INT32	iWeight[TURNTABLE_ITEM_COUNT];	//权重
	CT_INT32	iRewardID[TURNTABLE_ITEM_COUNT];
	CT_INT32	iFakeProID[TURNTABLE_ITEM_COUNT];	//伪概率ID
	G_NomalValue gReward[TURNTABLE_ITEM_COUNT];		//奖励
};

struct TimeActiveStatus
{
	CT_INT32	iAEID;		//当前/最近活动ID
	CT_INT32	iStatus;	//0.关闭 1.开启
	CT_INT64	iTime;		//
};

typedef std::vector<CT_INT32>				ActiveEventVector;
typedef std::map<CT_INT32, TurntableItem>	TurntableMap;		//TurntableID:Turntable
typedef	std::map<CT_INT32,ActiveDsc>		ActiveDscMap;	// 活动描述
typedef std::map<CT_INT32,G_ActiveEventReward> ActiveRewardMap;
typedef std::map<CT_INT32, TimeActiveStatus> TimeActiveStatusMap;

struct MSC_DB_TurntableRelation;
class CCActive : public CCSingleton<CCActive>
{
private:
	CCMutex   m_mutex;
public:
	CCActive();
	~CCActive();
private:
	ActiveEventVector		m_ActiveEventVector;
	TurntableMap			m_TurntableMap;
	ActiveDscMap			m_ActiveDscMap;
	ActiveRewardMap			m_RewardMap;
	TimeActiveStatusMap		m_TimeActiveStatusMap;
public:
	//插入活动描述
	CT_VOID		InsterActiveDsc(CT_CHAR *pBuf,CT_INT32 iLen);
	//插入补充奖励池
	CT_VOID		InsertReward(G_ActiveEventReward &reward);
	CT_BOOL		GetReward(CT_INT32 iRewardID, G_ActiveEventReward *pReward);
	CT_BOOL		GetReward(CT_CHAR *pBuf, CT_INT32 &bufLen);
	//获取活动描述
	CT_BOOL		GetActiveDsc(CT_CHAR *pBuf,CT_INT32 &iLen);
	//定时刷新活动
	CT_VOID		UpdateActive(ActiveEventVector &v_active,MSC_DB_ActiveEventChange &activeEventDelete, MSC_DB_ActiveEventChange &activeEventAdd);
	//获取现有的活动id
	CT_VOID		GetServerActive(MSC_DB_ActiveEventChange &activeList);
	//活动进行
	CT_VOID		ActiveProcess(CT_INT32 iType,CT_INT32 iQuanity,CCUser *pUser, CT_INT32 iAID=0);
	//商品打折活动，获取商品的折扣
	CT_FLOAT	GetMarketDiscount(CT_INT32	iGoodsID);
	//获取某类型活动的活动列表
	CT_INT32	GetActiveByType(CT_INT32 iType, CT_INT32 *ActiveList);
	//通过活动ID得到活动类型
	CT_INT32	GetActiveType(CT_INT32 iAID);
	//删除转盘活动
	CT_VOID		DelTurntableActive(CT_INT32 iAID);
	//增加转盘活动
	CT_VOID		NewTurntableActive(CT_INT32 iAID);
	CT_VOID		RefTurntable();

	CT_VOID		GetTurntableItem(CT_INT32 iTurntableID, TurntableItem *Item);
	CT_VOID		SetTurntableItem(MSC_DB_TurntableRelation &ttr);

	CT_VOID		SetTimeActiveStatus(CT_INT32 iType, CT_INT32 iAEID, CT_INT64 iTime, CT_INT32 iStatus=0);
	CT_VOID		GetTimeActiveStatus(CT_INT32 iType, CT_INT32 &iAEID, CT_INT64 &iTime, CT_INT32 &iStatus);
private:
	//随机转盘奖池
	CT_INT32	GetPosByRandNum(G_TurntableRelation *ttrList,CT_INT32 begin,CT_INT32 end,CT_INT32 iRand);

};

#endif