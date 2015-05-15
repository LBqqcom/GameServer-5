/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserPVPInfo.h
Version:		1.0
Description:    竞技场用户类
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCUSERPVPINFO_H_
#define CCUSERPVPINFO_H_
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserBase.h"
#include "CUserPet.h"
#include "CUserEquipment.h"
#include "CUserEnchant.h"
#include "PVPInfo.h"

//用户战斗数据
struct User_Fighting
{
	MasterData				MasterInfo;								//领主信息
	CMD_G_ItemData			EquipedItemData[EQUIPED_ITEM_COUNT];		//装备信息
	CMD_G_EquipData			EquipmentInfo[EQUIPED_ITEM_COUNT];			//强化信息
	CMD_G_SkillData			iSkillID[EQUIPED_SKILL_COUNT];			//用户技能
	CT_INT32				iPosition[POST_COUNT/2];				//位置	
	ArmyInfo				ArmyData[POST_COUNT/2];					//军队信息
	CT_INT32				iTalentNum;								//潜能数目

	PetInfo					pi;								//宠物信息

    
    EnchantData enchantData[EQUIP_COUNTS * ENCHANT_ATTR_CNT];  // 附魔数据
    CT_INT32 iVIPLevel;  // VIP等级
    CT_CHAR cFightTeam[NAME_SIZE];   // 所属战队
};

// 战斗数据 [8/26/2013 John]
struct UserFightingStatus
{
	CT_DWORD dwUserID;                      // 领主ID
	CombatUnitData units[POST_COUNT / 2];   // 战斗数据
};

//pvp战斗数据
struct MSC_PVP_Fighting_Info
{
	CT_DWORD		dwUserID;							//发起用户ID
	CT_INT32		iSceneID;							//场景ID
	CT_INT32		iSeed;								//随机种子
	CT_INT32		iDifficulty;						//难度
	CT_INT32		iWiner;								//胜利方

	UserFightingStatus FightingData[PVP_USER_COUNT];	//战斗数据
	LookUserInfo	LookData;							//查看用户数据
};

//pvp奖励
struct MSC_PVP_FightingReward
{
	CT_DWORD		dwUserID;
};


class CCUserPVPInfo
{
private:
	PVPInfo						m_PVPInfo;								//竞技场信息
	PVPFightingInfo				m_PVPRecord[PVP_FIGHTING_COUNT];		//竞技场记录
	UserFightingStatus			m_ChallengeFight[PVP_CHALLENGE_COUNT];	//挑战用户战斗信息
	LookUserInfo				m_ChallengeLook[PVP_CHALLENGE_COUNT];	//挑战用户查看信息

	MSC_PVP_Fighting_Info		m_RecordFightData[PVP_FIGHTING_COUNT];			//战报战斗信息
	//LookUserInfo				m_RecordLookData[PVP_FIGHTING_COUNT];		//战报用户信息
	CT_INT32					m_iFightingCount;						//战斗记录数目

private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;


public:
	CCUserPVPInfo(void);
	~CCUserPVPInfo(void);

public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

public:
	//插入竞技场信息
	CT_VOID InsertBufferData(const PVPInfo &PVPinfo);
	//插入对手状态
	CT_VOID InsertChallengUser(const PVPInfo &PVPinfo);
	//插入战报记录
	CT_VOID InsertRecordInfo(CT_INT32 iIndex,const PVPFightingInfo &FightingInfo);
	//插入战斗信息
	CT_VOID	InsertRecordFighting(MSC_PVP_Fighting_Info &fi);
	//插入高级组
	CT_VOID InsertHUserID(CT_INT32 iIndex,CT_DWORD dwUserID);
	//插入低级组
	CT_VOID InsertLUserID(CT_INT32 iIndex,CT_DWORD dwUserID);

public:
	//获取竞技场信息
	CT_BOOL GetPVPInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取竞技场信息
	CT_VOID GetPVPInfo(PVPInfo &PVPinfo,CT_BOOL bIsServer=CT_FALSE);
	//获取竞技场记录
	//CT_BOOL GetPVPRectord(PVPFightingInfo PVPFightingRecord[PVP_FIGHTING_COUNT]);
	CT_BOOL GetPVPRectord(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取挑战次数
	CT_INT32 GetFightingCount(){_AUTOMUTEX(m_pMutex); return m_PVPInfo.iFightingCount;};
	//获得对手信息
	CT_BOOL GetFightingUser(CT_INT32 iIndex,CT_DWORD dwUserID,UserFightingStatus &FightingUser);
	CT_BOOL GetFightingUser(CT_DWORD dwUserID,LookUserInfo &UserData);
	//获得对手战报信息
	CT_BOOL GetFightingRectordUser(CT_INT32 iIndex,CT_DWORD dwUserID,LookUserInfo &UserData);
	//获取难度
	CT_INT32 GetDifficulty(CT_DWORD dwUserID);
	//获得冷却时间
	CT_BOOL GetCoolTime(CT_INT64 lTime);
	//获得挑战状况(组)
	CT_VOID GetChallengeInfo(CT_INT32 iDifficulty,CT_BOOL bWiner[PVP_CHALLENGE_COUNT/2]);
	//获得挑战ID(组)
	CT_VOID GetChallengeID(CT_INT32 iDifficulty,CT_DWORD dwUserID[PVP_CHALLENGE_COUNT/2]);
	//获取战报
	CT_BOOL GetFightingInfo(CT_INT32 iIndex,MSC_PVP_Fighting_Info &FightingInfo);
	//获得对手状况
	CT_BOOL GetChallengeStatce(CT_INT32 iIndex);

public:
	//设置冷却时间
	CT_VOID SetBeginCoolTime(CT_INT64 lTime); 
	//增加冷却时间
	CT_VOID SetAddCoolTime(CT_INT64 lTime){_AUTOMUTEX(m_pMutex); m_PVPInfo.lRefreshTime+=lTime;};
	//设置挑战次数
	CT_VOID SetFightingCount(){m_PVPInfo.iFightingCount++;};
	//设置对手
	CT_VOID SetChallengInfo(CT_INT32 iIndex,const UserFightingStatus &FightData,LookUserInfo &LookData);
	//设置对手战报信息
	CT_BOOL SetFightingRectordUser(const User_Fighting &FightingUser);
	//设置竞技度
	CT_INT32 SetPVPDegree(const CT_INT32 iDegree,CT_BOOL bSend=CT_TRUE);
	//保存战报
	CT_VOID SaveFightingReacord();

public:
	//更新竞技场对手信息
	CT_VOID UpdataChallengUser(CT_DWORD dwUserID,CT_INT32 iDifficulty,CT_BYTE cbStatus);
	//更新竞技场记录
	CT_VOID	UpdataPVPRectord(const PVPFightingInfo &PVPFightingRecord);
	//更新战斗数据
	CT_VOID UpdataFighting(CT_CHAR *pBuf,CT_INT32 iLen);
	//更新冷却时间(用于退出保存)
	CT_INT64 UpdataPVPCoolTime();
	//更新领奖次数
	CT_BOOL UpDataChestNum(CT_BOOL bReset=CT_FALSE);
	//12点更新竞技场
	CT_VOID UpdataPVPData(CT_CHAR *pBuf,CT_INT32 iLen);
};

#endif
