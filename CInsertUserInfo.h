/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserPVPInfo.h
Version:		1.0
Description:    用户信息插入类
Author:			wangchen
Create Date:	2013-2-20
History:             

******************************************************************/
#ifndef CCINSERTUSERINFO_H_
#define CCINSERTUSERINFO_H_
#include "CGameMesEvent.h"

class CCUser;
class CCInsertUserInfo : public CCGameMesEvent
{
public:
	CCInsertUserInfo(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCInsertUserInfo(void);

private:
	//处理用户消息  最后的2个参数为备用
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len, 
		CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{

	}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{

	}

private:
	//领主信息
	CT_VOID DisposeMasterInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到正在升级建筑的信息
	CT_VOID DisposeUpgradeBuildingInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到潜能数据
	CT_VOID DisposeTalentInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到技能数据
	CT_VOID DisposeSkillData(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到道具数据
	CT_VOID DisposeItemInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到装备信息
	CT_VOID DisposeEquipInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到增益数据
	CT_VOID DisposeBuffInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到冷却信息
	CT_VOID DisposeCoolDownInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到军队信息
	CT_VOID DisposeArmyInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到正在生产的建筑
	CT_VOID DisposeProducting(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到世界场景数据
	CT_VOID DisposeWorldScene(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到场景数据
	CT_VOID DisposeSceneInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到用户解锁区域
	CT_VOID DisposeRrgionInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到阵法信息
	CT_VOID DisposeBattleArray(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到用户已完成任务
	CT_VOID DisposeTaskFinish(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到用户在进行任务
	CT_VOID DisposeTaskPorgress(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到日常任务进度信息
	CT_VOID DisposeDailyQuest(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到活跃度信息
	CT_VOID DisposeDailyActive(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到建筑信息
	CT_VOID DisposeBuildingInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到商店状态
	CT_VOID DisposeMarketFunction(CT_CHAR *pBuf,CT_INT32 iLen);
	//获取黑店信息
	CT_VOID DisposeMarketInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到宠物信息
	CT_VOID DisposePetInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//获取vip信息
	CT_VOID DisposeVipInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//竞技场信息
	CT_VOID DisposePVPInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//排行榜
	CT_VOID DisposeTop(CT_CHAR *pBuf,CT_INT32 iLen);
	//战报记录
	CT_VOID DisposePVPRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到签到信息
	CT_VOID DisposeSignInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//获取附魔信息
	CT_VOID DisposeEnchantdata(CT_CHAR *pBuf,CT_INT32 iLen);
	//获取附魔奖励信息
	CT_VOID DisposeEnchantReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//得到购买战斗次数
	CT_VOID DisposeBuyFightingTime(CT_CHAR *pBuf,CT_INT32 iLen);
	//扫荡信息
	CT_VOID DisposeRaidsInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//扫荡奖励
	CT_VOID DisposeRaidsReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//好友信息
	CT_VOID DisposeFriendInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//组队副本
	CT_VOID DisposeTeamDuplicate(CT_CHAR *pBuf,CT_INT32 iLen);
	//自动领取运营道具奖励
	CT_VOID DisposePresent(CT_CHAR *pBuf,CT_INT32 iLen);
	//个人限购
	CT_VOID DisposeMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen);
    //元素之力数据
    CT_VOID DisposeElmPowerInfo(CT_CHAR *pBuf,CT_INT32 iLen);
    // 战旗
    CT_VOID DisposeBattleFlagInfo(CT_CHAR *pBuf,CT_INT32 iLen);
    // 元素属性
    CT_VOID DisposeElmAttrInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	// 元素塔
	CT_VOID DisposeTower(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerSlotReward(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerEvent(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerSlotFighting(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerMSG(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeTowerHelp(CT_CHAR *pBuf,CT_INT32 iLen);
	CT_VOID DisposeBossRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队科技
	CT_VOID DisposeFightTeamScience(CT_CHAR *pBuf,CT_INT32 iLen);
	//宠物幻化
	CT_VOID DisposePetChange(CT_CHAR *pBuf,CT_INT32 iLen);
	//获取头衔信息
	CT_VOID DisposeTitleData(CT_CHAR* pBuf, CT_INT32 iLen);
	//获取龙岛信息
	CT_VOID DisposeIslandData(CT_CHAR* pBuf, CT_INT32 iLen);
	//获取已赠送次数的玩家ID
	CT_VOID DisposeIslandGiveUid(CT_CHAR* pBuf, CT_INT32 iLen);
	//获取已领取的战队积分宝箱
	CT_VOID DisposeIslandScoreReward(CT_CHAR* pBuf, CT_INT32 iLen);
	//获取赠送次数给自己的玩家昵称
	CT_VOID DipsoseIslandPresentName(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入城墙加成
	CT_VOID DisposeSeekWallAddition(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入城墙军队
	CT_VOID DisposeWallArmy(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入可探访随机列表
	CT_VOID DisposeSeekRandPlayer(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入已探访的列表
	CT_VOID DisposeHadSeekedPlayer(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入战队旗帜
	CT_VOID DisposeTeamFlagInsert(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入圣女人质信息
	CT_VOID DisposeSaintessKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入工人人质信息
	CT_VOID DisposeWorkerKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入初始善访人员列表
	CT_VOID DisposeWillSeekerList(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入初始恶访人员列表
	CT_VOID DisposeSpiteSeekerList(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入探访基本数据
	CT_VOID DisposeSeekDataInsert(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入探访记录
	CT_VOID DisposeSeekVisitorInfo(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入探访战报
	CT_VOID DisposeSeekFightRecord(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入领主战斗信息
	CT_VOID DisposeMasterFightAdd(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入探访别人的信息
	CT_VOID DisposeSeekOthersInfo(CT_CHAR* pBuf, CT_INT32 iLen);
	//插入宠物肉票
	CT_VOID DisposePetKidnap(CT_CHAR* pBuf, CT_INT32 iLen);
};
#endif
