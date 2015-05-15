/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUser.h
Version:		1.0
Description:    用户数据
Author:			pengwen
Create Date:	2012-8-22
History:        用户数据     

******************************************************************/
#ifndef CCUSER_H_
#define CCUSER_H_
#include <map>
#include <set>
#include "CGDefine.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CMutex.h"
#include "CUserBase.h"
#include "CUserBuilding.h"
#include "CUserItem.h"
#include "CUserEvent.h"
#include "CUserSkill.h"
#include "CUserTalent.h"
#include "CUserBuff.h"
#include "CUserEquipment.h"
#include "CRegion.h"
#include "CUserDuplicate.h"
#include "CUserArmy.h"
#include "CUserBattleArray.h"
#include "CTask.h"
#include "CUserDaily.h"
#include "CUserBlackMarket.h"
#include "CUserPet.h"
#include "CUserPVPInfo.h"
#include "CUserActive.h"
#include "CUserWorship.h"
#include "CVip.h"
#include "CUserFriend.h"
#include "CUserTeamDuplicate.h"
#include "CUserTower.h"
#include "CUserBattleFlag.h"
#include "CUserElmAttr.h"
#include "CUserBoss.h"
#include "CUserFightTeamNember.h"
#include "CUserTitle.h"
#include "CDragonIsland.h"
#include "CUserSeek.h"
#include <time.h>

class CCNetInterface;
struct MSC_DB_MasterSaveData;
struct MSC_DB_InsertWallArmy;
struct PTRLinkUserData
{
    MasterData *pMd;
    CT_INT32 *pArray;
    CT_INT32 *pVipGrade;
    CCUserTalent::DataMap *pTalentMap;
    CCUserSkill::DataMap *pSkillMap;
    CCUserItem::DataMap *pItemMap;
    CCUserEquipment::DataMap *pEquipMap;
    CCUserBuff::DataMap *pBuffMap;
    CCUserEnchant::DataMap *pEnchantMap;
    CCUserBattleFlag::DataMap *pFlagMap;
    CCUserItem::DataMap *pElmMap;
    CCUserElmAttr::DataMap *pElmAttrMap;
    CCUserArmy::DataMap *pArmyMap;
	CCUserTower::AwardStatusMap *pTowerMap;
    CCUserFightTeamNember::DataMap *pScienceMap;
	CCUserEnchant::EnchantRewardVector *pEnchatRewardVector;
	PetInfo	*pPetInfo;
	CCUserTitle::DataMap *pTitleMap;
	CT_INT32 *pSeekArray;
	WallAdditionMap *pWallAdditionMap;
	CCUserBase::MasterFightAddMap *pMasterFightAddMap;
	CCUserPet::PetChangeMap *pcmap;
};

struct FightingPower
{
	CT_INT32 iAllFightingPower;
	CT_INT32 iMasterFightingPower;
	CT_INT32 iArmyFightingPower;
	CT_INT32 iFightingFlagPower;
};

enum ENUM_LootItem {
    LOOT_SUC,
    LOOT_BAG_FULL,
    LOOT_ELM_FULL,
    LOOT_NO_ITEM,
    LOOT_NO_ELM,
};

//用户数据
class CCUser 
{	
	friend class CCUserBuilding;
	friend class CCUserBase;
	friend class CCUserEvent;
private:
	CCMutex							m_mutex;
private:
	CT_DWORD						m_dwID;					//用户id
	CCNetInterface*					m_pNetInterface;		//用户接口
	CT_INT64						m_lHeartBeat;			//心跳
	CT_INT32						m_iHeartCount;			//心跳停止次数
	CT_BOOL							m_bTalk;				//禁言
	CT_INT32						m_iChatTime;			//聊天次数
	CT_BOOL							m_bInit;				//数据初始化

public:
	CCUserBase                      m_userBase;             //用户基础数据
	CCUserBuilding                  m_userBuilding;         //用户建筑数据
	CCUserEvent                     m_userEvent;            //用户事件数据
	CCUserSkill                     m_userSkill;            //用户技能数据
    CCUserTalent                    m_userTalent;           //用户潜能（神器）
	CCUserItem                      m_userItem;             //用户物品装备数据
    CCUserEquipment                 m_userEquip;            //用户装备信息
    CCUserEnchant                   m_userEnchant;          //用户附魔信息
    CCUserBuff                      m_userBuff;             //用户增益数据
	CCUserRegion					m_userRegion;			//用户区域信息
	CCUserDuplicate1				m_userDuplicate;		//用户副本数据
	CCUserArmy						m_userArmy;				//用户军队数据
	CCUserBattleArray				m_userBattleArray;		//用户阵法数据
	CCUserTask						m_userTask;				//用户任务
    CCUserDaily                     m_userDaily;            //用户日常活动
	CCUserBlackMarket				m_userBlackMarket;		//用户黑店数据
	CCUserPet						m_userPet;				//用户宠物数据
	CCUserPVPInfo					m_userPVPInfo;			//用户竞技场数据
	CCUserVip						m_userVip;				//用户vip信息
	CCUserActive					m_userActive;			//用户活动信息
	CCUserWorship					m_userWorship;			//用户祭祀信息
	CCUserFriend					m_userFriend;			//用户好友信息
	CCUserTower						m_userTower;			//用户元素之塔
	CCUserTeamDuplicate				m_userTeamDuplicate;	//用户组队副本
    CCUserItem                      m_userElmPower;         //用户元素之力数据
    CCUserBattleFlag                m_userBattleFlag;       //战旗数据
    CCUserElmAttr                   m_userElmAttr;			// 元素属性
	CCUserBoss						m_userBoss;				//世界BOSS
	CCUserFightTeamNember			m_userFightTeamNember;	//战队成员
	CCUserTitle						m_userTitle;			//用户头衔
	CCUserIsland					m_userIsland;			//用户龙岛
	CCUserSeek						m_userSeek;				//用户探访

public:
	//初始化
	CT_VOID Init();
	//清理数据,用户退出使用，防止内存占用过高
	CT_VOID Clear();

public:
	//设置用户id 
	CT_VOID SetUserID(CT_DWORD id) {_AUTOMUTEX(&m_mutex);   m_dwID = id;}
	//设置用户网络接口
	CT_VOID SetNetInterface(CCNetInterface* pNetHandle) {_AUTOMUTEX(&m_mutex);   m_pNetInterface = pNetHandle; }
	//设置心跳时间
	CT_VOID SetHeartBeat(){_AUTOMUTEX(&m_mutex);   m_lHeartBeat = time(0); m_iHeartCount=0;};
	//得到用户id
	CT_DWORD        GetUserID() {_AUTOMUTEX(&m_mutex);  return m_dwID; }
	//得到用户网络接口
	CCNetInterface* GetNetInterface() {_AUTOMUTEX(&m_mutex);  return m_pNetInterface; }
	//心跳更新
	CT_VOID	UpdataHeartBeat(CT_INT64 lTime);
	//竞技度更新
	CT_VOID UpdataPVPDegree(CT_INT32 iDegree);
	//关闭连接
	CT_VOID	CloseNet();
	//获得禁言
	CT_BOOL GetTalk(){return m_bTalk;};
	//设置禁言
	CT_VOID SetTalk(CT_BOOL bTalk){m_bTalk=bTalk;};
	//记录聊天次数
	CT_VOID AddChatTime(){_AUTOMUTEX(&m_mutex); m_iChatTime++;}
	//获取聊天ishu
	CT_INT32 GetChatTime(){return m_iChatTime;}
	//设置数据初始化
	CT_VOID  SetInit(CT_BOOL bInit){m_bInit=bInit;};
	//设置数据初始化
	CT_BOOL  GetInit(){return m_bInit;};
	//
	CT_BOOL  GetPTRLinkUserData(PTRLinkUserData &linkUserData);

	CT_VOID UserExitEvent(CT_INT64 iExitTime = 0);
    // 重置数据同步时间
    inline CT_VOID ResetSyncTime(time_t now) 
    {
        m_userItem.m_lastSyncTime = now;
        m_userEquip.m_lastSyncTime = now;
        m_userElmPower.m_lastSyncTime = now;
        m_userElmAttr.m_lastSyncTime = now;
        m_userBuff.m_lastSyncTime = now;
    }


public:
	//计算最大工作人数
	CT_VOID CalMaxWorker();
	//计算空闲工人数目
	CT_VOID CountFreeWorker();

// Begin [9/10/2012 John]
public:
    // 更新领主的战斗数值
    CT_INT32 UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 target = UNIT_TYPE_PLAYER/*, CT_INT32 *buffList = NULL, CT_INT32 buffCount = 0*/);
	//得到战斗状态数据
	CT_BOOL GetBattleStatusData(CT_CHAR* pBuf, CT_INT32 &bufLel);
    // 更新领主的系数数据
    CT_VOID UpdateFactorData(void);
    // 更新领主数据到客户端
    CT_VOID UpdateAndSendMasterData(CT_UINT32 flag = 0xffffffff);
	//获得功能开启
	CT_BOOL GetFunctionOpen(CT_CHAR *pBuf,CT_INT32 &iLen);

	// 得到宠物数据
	CT_BOOL GetPetData(CT_CHAR *pBuf, CT_INT32 &bufLen);
	// 得到任务列表
	CT_BOOL GetQuestData(CT_CHAR *pBuf, CT_INT32 &bufLen); 

	// Begin [10/22/2012 wangchen]
	// 得到副本数据
	CT_BOOL GetDuplicateData(CT_CHAR *pBuf, CT_INT32 &bufLen); 
	// End [10/22/2012 wangchen]

    // 保存数据到数据库
    inline CT_VOID SaveData( const CT_DWORD mainID, const CT_DWORD subID, CT_VOID *buf, CT_INT32 bufLen )
    {
        _AUTOMUTEX(&m_mutex);
        CT_INT32 threadID = GAME_THREAD_DATABASE + m_dwID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
        G_SendThreadInfo(threadID, mainID, subID, (CT_CHAR *)buf, bufLen);
    }

    // 记录到日志数据库
    inline CT_VOID SaveLogData( const CT_DWORD mainID, const CT_DWORD subID, CT_VOID *buf, CT_INT32 bufLen )
    {
        _AUTOMUTEX(&m_mutex);
        CT_INT32 threadID = GAME_THREAD_LOGDATABASE + m_dwID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
        G_SendThreadInfo(threadID, mainID, subID, (CT_CHAR *)buf, bufLen);
    }
public:    
    // 拾取道具
    ENUM_LootItem LootItems(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], const CT_INT32 itemTypes=1, const ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);
public:
	CCUser();
	~CCUser();
private:
	CCUser(const CCUser&);
	CCUser operator = (const CCUser& user);
public:
    // 根据领主的属性数据，计算领主的战斗属性，返回战斗力数值
	static CT_INT32 CalcUserFinalCombatStatus(MasterCombatStatus &mcs, const PTRLinkUserData &linkUserData,FightingPower &Power);
    // 根据领主的属性数据，计算指定的军队的战斗属性，返回战斗力数值
    static CT_INT32 CalcArmyFinalCombatStatus(MasterCombatStatus &mcs, const PTRLinkUserData &linkUserData, const ArmyInfo &army);
    // 根据领主的属性数据，计算领主出战队伍的战斗数据（战斗属性和技能）
    static CT_VOID GetFightingStatusData(UserFightingStatus &data, const PTRLinkUserData &linkUserData,CT_BOOL bSeekFight=CT_FALSE);
}; 

#endif
