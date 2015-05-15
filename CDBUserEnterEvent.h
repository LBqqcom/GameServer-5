/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBUserEnterEvent.h
Version:		1.0
Description:    数据库登入服务
Author:			pengwen
Create Date:	2012-8-16
History:        处理DB登入事件   

******************************************************************/
#ifndef CCDBUSERENTEREVENT_H_
#define CCDBUSERENTEREVENT_H_


#include "CGameMesEvent.h"
#include "CThreadMes.h"
#include<list>

class CCDBConnector;
struct MSC_DB_UserEnter;
struct User_Fighting;
struct MasterData;
struct SkillData;
struct EquipmentData;
struct BuffData;
struct MasterFightAdd;

// 用于计算领主的最终战力
struct PTRLinkUserData;

class CCDBUserEnterEvent :
	public CCGameMesEvent
{
    struct OnLinkUserData
    {
        MasterData	masterInfo;
        MSC_DB_BattleArray  BattleArraydata;
        CT_INT32 VipGrade;
        CCUserTalent::DataMap talentMap;
        CCUserSkill::DataMap skillMap;
        CCUserItem::DataMap itemMap;
        CCUserEquipment::DataMap equipMap;
        CCUserBuff::DataMap buffMap;
        CCUserArmy::DataMap armyMap;
        CCUserEnchant::DataMap enchantMap;
        CCUserBattleFlag::DataMap flagMap;
        CCUserItem::DataMap elmMap;
        CCUserElmAttr::DataMap elmAttrMap;
		CCUserTower::AwardStatusMap towerMap;
        CCUserFightTeamNember::DataMap scienceMap;
		CCUserEnchant::EnchantRewardVector enchantRewardVector;
		PetInfo pi;
		CCUserTitle::DataMap titleMap;
		CT_INT32 SeekArray[POST_COUNT/2];
		WallAdditionMap wamap;
		CCUserBase::MasterFightAddMap mfamap;
		CCUserPet::PetChangeMap pcmap;
    };
private:
	CCDBUserEnterEvent();
private: 
	CCDBConnector& m_dbConnector;
public:
	CCDBUserEnterEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector );
	~CCDBUserEnterEvent(void);
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
								CT_CHAR* pBuf,   CT_DWORD len,
								CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{
			
		char* p = 0;
	}

	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
			char* p = 0;
	}
private:
	//处理子消息
	CT_VOID DisposeLoginSubEvent( CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);

	//非在线用户信息
private: 
	//非在线用户信息
	CT_VOID GetNoLinkUserInfo(CT_DWORD *dwUserID,UserFightingStatus *Userinfo,LookUserInfo *LookUserinfo,CT_INT32 iUserCount);
	//获得排行榜信息
	CT_VOID GetTopInfo(CT_CHAR* pBuf, CT_DWORD len);
	//获得对手信息
	CT_BOOL GetChalleng(CT_CHAR* pBuf, CT_DWORD len);
	//查看他人信息
	CT_BOOL GetAnother(CT_CHAR* pBuf, CT_DWORD len);
	//查看战队成员战斗数据
	CT_BOOL GetFTFightingData(CT_CHAR* pBuf, CT_DWORD len);
	//组队副本自动匹配
	CT_VOID GetTeamDuplicateAutoMatch(CT_CHAR* pBuf, CT_DWORD len);
	//更新运营奖励道具领取状态
	CT_VOID UpdatePresent(CT_CHAR* pBuf, CT_DWORD len);
	//可探访者信息读取
	CT_VOID GetSeekerInfo(CT_CHAR* pBuf,CT_DWORD len);
	// 得到别人的城镇信息
	CT_VOID DisposeGetOtherTown(CT_CHAR* pBuf, CT_INT32 iLen); 

private:
	//
	CT_VOID GetPTRLinkUserData(OnLinkUserData &UserData,PTRLinkUserData &linkUserData);
	// 更新领主的战斗数据
	CT_VOID UpdateBattleStatus(OnLinkUserData &UserData);

    /*
    * 从数据库读取数据
    */
    // 得到领主信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, MasterData &md);
    // 得到潜能数据
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserTalent::DataMap &talentMap);
    // 得到技能数据
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserSkill::DataMap &skillMap);
    // 得到道具信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserItem::DataMap &itemMap);
    // 得到装备信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserEquipment::DataMap &equipMap);
    // 得到增益数据
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserBuff::DataMap &buffMap);
    // 得到军队信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserArmy::DataMap &armyMap);
    // 得到阵法信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, MSC_DB_BattleArray &battleArray);
    // 获取宠物信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, PetInfo &pi);
    // 获取vip信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CT_INT32 &vipGrade, CT_INT32 &vipTime, CT_INT32 &vipGift);
    // 获取附魔信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserEnchant::DataMap &enchantMap);
    // 得到战旗信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserBattleFlag::DataMap &flagMap);
    // 得到元素之力信息
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserItem::DataMap &elmMap, CCUserElmAttr::DataMap &elmAttrMap);
	// 得到元素塔永久奖励加成
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserTower::AwardStatusMap &towerMap);
    // 得到战队科技
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserFightTeamNember::DataMap &scienceMap);
	// 得到头衔信息
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserTitle::DataMap& titleMap);
	// 得到附魔奖励信息
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserEnchant::EnchantRewardVector& enchantRewardVec);
	// 得到探访军队
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CT_INT32 *pSeekArray);
	// 得到探访城墙加成
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, WallAdditionMap &wallAdditionMap);
	// 得到战斗加成
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserBase::MasterFightAddMap &mfaMap);
	// 得到宠物幻化
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserPet::PetChangeMap &pcMap);

private:
	//得到领主信息
	CT_BOOL GetMasterInfo(const MSC_DB_UserEnter*   pTUE,CT_DWORD subID);
	//得到正在升级建筑的信息
	CT_BOOL GetUpgradeBuildingInfo(const MSC_DB_UserEnter*   pTUE);
	//得到建筑信息
	CT_BOOL GetBuildingInfo(const MSC_DB_UserEnter*   pTUE);
	//得到正在生产的建筑
	CT_BOOL GetProductingBuilding(const MSC_DB_UserEnter*   pTUE);
    // Begin [9/20/2012 John]
    // 得到潜能数据
    CT_BOOL GetTalentInfo(const MSC_DB_UserEnter*   pTUE);
    // 得到技能数据
    CT_BOOL GetSkillInfo(const MSC_DB_UserEnter*   pTUE);
    // 得到道具数据
    CT_BOOL GetItemInfo(const MSC_DB_UserEnter* pTUE);
    // 得到装备信息
    CT_BOOL GetEquipInfo(const MSC_DB_UserEnter* pTUE);
    // 得到增益数据
    CT_BOOL GetBuffInfo(const MSC_DB_UserEnter* pTUE);
    // 得到冷却信息
    CT_BOOL GetCooldownInfo(const MSC_DB_UserEnter *pTUE);
    // End [9/20/2012 John]
	//得到用户解锁区域
	CT_BOOL GetRegionInfo(const MSC_DB_UserEnter* pTUE);
	//得到用户已完成任务
	CT_BOOL GetTaskFinishedData(const MSC_DB_UserEnter* pTUE);
	//得到用户在进行任务
	CT_BOOL	GetTaskProgressData(const MSC_DB_UserEnter* pTUE);
    // 得到日常任务进度信息
    CT_BOOL GetDailyQuestInfo(const MSC_DB_UserEnter* pTUE);
    // 得到签到信息
    CT_BOOL GetDailySignInfo(const  MSC_DB_UserEnter* pTUE);
    // 得到活跃度信息
    CT_BOOL GetDailyActiveInfo(const  MSC_DB_UserEnter* pTUE);
	// Begin [10/22/2012 wangchen]
	// 得到世界场景数据
	CT_BOOL GetWorldSceneInfo(const MSC_DB_UserEnter*   pTUE);
	// 得到场景数据
	CT_BOOL GetSceneInfo(const MSC_DB_UserEnter*   pTUE);
	//得到军队信息
	CT_BOOL	GetArmyInfo(const MSC_DB_UserEnter*   pTUE);
	//得到阵法信息
	CT_BOOL	GetBattleArray(const MSC_DB_UserEnter*   pTUE);
	//竞技场信息
	CT_BOOL GetPVPInfo(const  MSC_DB_UserEnter* pTUE);
	//竞技场战报
	CT_BOOL GetPVPRecord(const  MSC_DB_UserEnter* pTUE);
	//获取商店状态
	CT_BOOL GetMarketFunctionStatus(const  MSC_DB_UserEnter* pTUE);
	//获取商店个人限购
	CT_BOOL GetMarketPersonal(const  MSC_DB_UserEnter* pTUE);
	//获取黑店信息
	CT_BOOL	GetBlackMarketInfo(const  MSC_DB_UserEnter* pTUE);
	//获取宠物信息
	CT_BOOL GetPetInfo(const  MSC_DB_UserEnter* pTUE);
	//获取邮件信息
	CT_BOOL GetEmailInfo(const  MSC_DB_UserEnter* pTUE);
	//获取vip信息
	CT_BOOL	GetVipInfo(const  MSC_DB_UserEnter* pTUE);
	//获取活动信息
	CT_BOOL GetUserActiveEventInfo(const MSC_DB_UserEnter* pTUE);
	//获取祭祀信息
	CT_BOOL GetUserWorshipInfo(const MSC_DB_UserEnter* pTUE);
	//获取扫荡信息
	CT_BOOL GetRaidsInfo(const MSC_DB_UserEnter* pTUE);
	//获取好友信息
	CT_BOOL GetFriendInfo(const MSC_DB_UserEnter* pTUE);
	//获取组队副本场景
	CT_BOOL GetTeamDuplicateSceneInfo(const MSC_DB_UserEnter* pTUE);
	//获取运营道具奖励
	CT_BOOL GetPresent(const MSC_DB_UserEnter* pTUE);
	// End [10/22/2012 wangchen]
    // 获取附魔信息
    CT_BOOL GetEnchantInfo(const MSC_DB_UserEnter *pTUE);
	//获取附魔奖励信息
	CT_BOOL GetEnchantRewardInfo(const MSC_DB_UserEnter *pTUE);
    // 获得元素之力数据
    CT_BOOL GetElmPowerInfo(const MSC_DB_UserEnter *pTUE);
    // 获得战旗数据 
    CT_BOOL GetBattleFlagInfo(const MSC_DB_UserEnter *pTUE);  
	//得到元素之塔离线战斗数据
	CT_VOID GetTowerOffLineUserFightData(CT_CHAR* pBuf, CT_INT32 len);
	//元素之塔
	CT_BOOL GetTower(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerHelp(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerSlotReward(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerEvent(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerSlotFightRecord(const MSC_DB_UserEnter *pTUE);

	//系统留言
	CT_BOOL GetMSG(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetBossRecord(const MSC_DB_UserEnter *pTUE);
	//战队个人科技
	CT_BOOL GetFightTeamScience(const MSC_DB_UserEnter *pTUE);

	//得到头衔信息
	CT_BOOL GetTitleInfo(const MSC_DB_UserEnter *pTUE);
	// 获取龙岛数据
	CT_BOOL GetDragonIslandData(const MSC_DB_UserEnter *pTUE);
	//获取城墙技能
	CT_BOOL GetWallAddition(const MSC_DB_UserEnter *pTUE);
	//获取城墙军队
	CT_BOOL GetWallArmy(const MSC_DB_UserEnter *pTUE);
	//获取可随机探访者列表
	CT_BOOL GetSeekRandList(const MSC_DB_UserEnter *pTUE);
	//获取已探访的人的列表
	CT_BOOL GetHadSeekedList(const MSC_DB_UserEnter *pTUE);
	//获取探访信息
	CT_BOOL GetSeekInfo(const MSC_DB_UserEnter *pTUE);
	//获取探访战报
	CT_BOOL GetSeekFightRecord(const MSC_DB_UserEnter *pTUE,std::string strIndex);
	//获取领主战斗加成
	CT_BOOL GetMasterFightAdd(const MSC_DB_UserEnter *pTUE);

	// 得到建筑信息
	CT_VOID GetBuildingDate(CT_DWORD dwUserID, CCUserBuilding::BuildingDataMap& buildingMap);
	// 得到建筑生产信息
	CT_VOID GetBuildingProduct(CT_DWORD dwUserID, CCUserBuilding::ProductingtManualBuildingMap& productBuildingMap);
	// 得到建筑的升级信息
	CT_VOID GetBuildingUpdate(CT_DWORD dwUserID, CCUserBuilding::UpgradeingBuildDataMap& upgradeBuildingMap);
	// 得到旗帜信息
	CT_VOID GetTeamFlag(CT_DWORD dwUserID, TeamSeekMap& teamFlagMap);
	// 得到人质信息
	CT_VOID GetKidnapeeInfo(CT_DWORD dwUserID, KidnapMap& girlMap, KidnapMap& workerMap, KidnapMap& petMap);
	// 计算最大工人数
	CT_INT32 CalMaxWorker(CCUserBuilding::BuildingDataMap buildingDataMap);
	// 得到已完成任务
	CT_VOID GetTaskFinish(CT_DWORD dwUserID, CCUserTask::TaskCompleteVector& taskFinishVector);
	// 得到已开启的功能
	CT_VOID GetOpenFunction(CT_INT32* pBuf, CT_INT32& iLen, CT_DWORD dwUserID, CT_INT32 iMaterLevel);
	// 得到他人的城镇信息
	CT_VOID GetOtherTownInfo(MSC_DB_EnterOtherTown* pOtherTown);
	// 得到被绑架的宠物幻化ID
	CT_VOID GetKidnapPet(CT_INT32* pBuf, CT_INT32& iLen, CT_DWORD dwUserID);
	// 得到剩余的宠物幻化ID
	CT_VOID GetLastPetChangeID(CT_INT32* pBuf, CT_INT32& iLen,CT_DWORD dwUserID);

	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);
};
#endif