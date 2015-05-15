#ifndef CUSERTOWER_H_
#define CUSERTOWER_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"


struct TowerData
{
	CT_INT32	iCPID;
	CT_INT32	iTime;						// 当天已通关次数
	CT_INT32	iTreasure;					// 通关成绩(0表示已激活但未通关过)
};

struct TowerSlotAward
{
	CT_DWORD  dwUserID;                 //用户id
	CT_INT32  iFloor;				    //层
	CT_INT32  iSlot;                    //资源点
	G_NormalReward gReward;			//奖励
};

struct TowerSlotAwardKey
{
	CT_DWORD dwUserID; 
	CT_INT32 iCPID; 	//关卡
	CT_INT32 iSlot; 	//占领点

	TowerSlotAwardKey()
	{
		memset(this, 0, sizeof(TowerSlotAwardKey));
	}

	TowerSlotAwardKey(const TowerSlotAwardKey &data)
	{
		memcpy(this, &data, sizeof(TowerSlotAwardKey));
	}

	TowerSlotAwardKey& operator=(const TowerSlotAwardKey &data)
	{
		memcpy(this, &data, sizeof(TowerSlotAwardKey));
	}

	bool operator<(const TowerSlotAwardKey& data) const
	{
		if (memcmp(this, &data, sizeof(TowerSlotAwardKey)) < 0)
		{
			return true;
		}
		return false;
	}
};

struct TowerMSG
{
	CT_DWORD	dwUserID;                 //用户id
	CT_INT32	iType;					// 1.元素塔 2.竞技场
	CT_INT64	iTime;				  
	CT_CHAR		cMSG[255]; 
};

typedef CMD_G_ElementEvent TowerEvent;

struct MSC_TowerPVP_ResRobbery;
class CCUserTower
{
private:
	CCMutex*    m_pMutex;
	CCUser*     m_pUser;
	typedef std::map<CT_INT32, TowerData>						TowerDataMap;		// 关卡ID
	typedef std::map<CT_INT32, TowerData>::iterator				TD_IT;

	typedef std::map<TowerSlotAwardKey, G_NormalReward>			TowerSlotAwardMap;	// 占领点收获数据
	typedef std::map<CT_INT64, TowerMSG>						TowerMSGMap;
	typedef std::map<CT_INT64, TowerEvent>						TowerEventMap;
	typedef std::map<CT_INT64, MSC_TowerPVP_ResRobbery>			TowerPVPMap;
	typedef std::set<CT_DWORD>									TowerHelpSet;

	CT_INT32												m_iMaxCPID;
	CT_BOOL													m_bEnable;
public:
	typedef std::map<CT_INT32, CT_FLOAT>						AwardStatusMap;		// 属性类型, 属性值

public:
	CCUserTower();
	~CCUserTower(void);

	TowerDataMap											m_TowerData;		// 已通关的关卡
	AwardStatusMap											m_AwardStatus;		// 首通奖励的永久属性

	TowerSlotAwardMap										m_TowerSlotAward;	// 占领点收获
	TowerMSGMap												m_TowerMSG;
	TowerEventMap											m_TowerEvent;
	TowerPVPMap												m_TowerPVP;
	TowerHelpSet											m_TowerHelp;

public:
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

	static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const AwardStatusMap &awardStatusMap);
public:
	CT_BOOL IsEnable() {_AUTOMUTEX(m_pMutex);   return m_bEnable;}

	CT_VOID InsertTowerData(const TowerData &data, CT_BOOL bInit = CT_FALSE);

	CT_BOOL GetTowerData(CT_CHAR *pBuf, CT_INT32 &bufLen);			// 得到关卡信息
	CT_INT32 GetTowerMaxCP(){_AUTOMUTEX(m_pMutex);   return m_iMaxCPID;}

	CT_BOOL CheckPointPass(CT_INT32 iCPID, CT_INT32 iType=1);		// 检查某关是否已经通过 iType:0.曾经通过1.当天通关过
	CT_BOOL CheckPointRaids(CT_INT32 iEndCPID);			// 检查关卡是否可以扫荡
	CT_VOID UpdateRaids(CT_INT32 iBeginCPID, CT_INT32 iEndCPID);				// 更新扫荡
	CT_BOOL CheckHelpArea(CT_INT32 iCPID);

	CT_VOID UpdateCheckPoint(CT_INT32 iCPID, CT_INT32 iScore);		// 通关后更新关卡数据 & DB

	CT_VOID InsertTowerSlotAward(TowerSlotAward &towerSlotAward, CT_INT32 iDBInit=0);	// 插入占领点生产奖励
	CT_BOOL GetTowerSlotAward(CT_CHAR *pBuf, CT_INT32 &bufLen);
	CT_VOID RecvTowerSlotAward(TowerSlotAwardKey &towerSlotAwardKey);		// 删除占领点生产奖励,比如领取后
	CT_BOOL CheckTowerSlotAward(TowerSlotAwardKey &towerSlotAwardKey);		// 检查占领点生产奖励
	CT_BOOL IsEmptySlotAward();										// 是否有未领取的生产奖励

	CT_VOID InsertTowerMSG(const TowerMSG &msg);
	CT_VOID DeleteTowerMSG(CT_INT32 iType, CT_INT64 iTime);
	CT_BOOL GetTowerMSG(CT_CHAR *pBuf, CT_INT32 &bufLen);			// 得到关卡信息

	CT_INT32 GetPosByRandNum(CT_INT32 iTreasureGroupID);
	CT_VOID TreasureAbandon(CT_INT32 iCPID, CT_INT32 iTreasure);	//放弃宝箱
	CT_VOID TreasureUse(CT_INT32 iCPID, CT_INT32 iTreasure,CT_INT32 iUseType);	//打开宝箱
	CT_BOOL HaveTreasure();		//检查是否有未处理宝箱

	CT_VOID RefreshTowerPass();

	CT_VOID InsertPVPInfo(const MSC_TowerPVP_ResRobbery &PVPInfo);
	CT_BOOL GetPVPInfo(CT_INT64 iFID, MSC_TowerPVP_ResRobbery *PVPInfo);
	CT_VOID  InsertEvent(const TowerEvent &towerEvent, CT_BOOL bLeftWin = CT_FALSE);
	CT_VOID InitPVPInfo(const MSC_TowerPVP_ResRobbery &PVPInfo);
	CT_VOID InitEvent(TowerEvent &towerEvent);
	CT_BOOL GetEventData(CT_CHAR *pBuf, CT_INT32 &bufLen);

	CT_VOID InitHelp(CT_DWORD dwFID) {_AUTOMUTEX(m_pMutex);   m_TowerHelp.insert(dwFID);}
	CT_VOID InsertHelp(CT_DWORD dwFID);
	CT_BOOL CheckHelp(CT_DWORD dwFID);
	CT_BOOL GetHelpData(CT_CHAR *pBuf, CT_INT32 &bufLen);

};
#endif

