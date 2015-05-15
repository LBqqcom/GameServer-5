/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CBossManager.h
Version:		1.0
Description:    世界BOSS管理
Author:			zhoujun
Create Date:	2013-12-9
History:            

******************************************************************/
#ifndef CCBOSS_MANAGER_H_
#define CCBOSS_MANAGER_H_
#include <map>
#include <string>
#include "CNetInterface.h"
#include "CSingleton.h"
#include "CMD_Game.h"
#include "CMutex.h"

#include "CThreadMes.h"

struct BossHPHurt
{
	CT_DWORD	dwUID;
	CT_INT32	iHP;
	CT_CHAR		cUserName[NAME_SIZE];
	CT_FLOAT	fHurtPercent;
	CT_INT64	iBattleTime;
	CT_INT64	iActTime;		//上一次激励时间
	CT_INT32	iActCount;		//激励层数
	CT_BOOL		bBattleFlag;	//可战斗
	CT_BOOL		bActCoolFlag;	//激励已冷却
	CT_BOOL		bInActive;
};

struct BattleUser
{
	CT_DWORD	dwUID;
	CT_INT32	iDiamod;			//1.强攻扣钻石数
};

typedef std::map<CT_DWORD, BossHPHurt>			UserHurtMap;	//当前伤害列表 dwUID:HPHurt
typedef std::map<CT_INT32, BossHPHurt>			TopHurtMap;		//BOSS子榜 iTop:HPHurt
typedef std::map<CT_INT32, TopHurtMap>		BossHurtMap;		//所有BOSS排行榜 AEID:TopHurtMap
typedef std::multimap<CT_INT32, CT_DWORD>	HPTopMap;			//当前排名 iHP:dwUID
typedef std::map<CT_DWORD, CMD_G_WorldBossActiveReward> BossRewardMap;

//BOSS管理类
class CCBossManager : public CCSingleton<CCBossManager>
{
private:
	//互斥锁
	CCMutex								m_mutex;  

	CT_INT64							m_lBeginTime;			//开始时间
	CT_INT64							m_lEndTime;				//实际结束时间
	CT_INT64							m_lLastTime;			//时长
	CT_INT32							m_iBUffID;				//全服BUFF
	CT_INT32							m_iAEID;				//活动ID
	CT_BOOL								m_bActive;				//活动状态
	CT_INT32							m_iPreFinishTime;		//上次完成时间(活动开始不清除,活动完成重置)
	CT_INT32							m_iPrePlayer;			//上次参与人数(活动开始不清除,活动完成重置)
	CT_INT32							m_iBossGrade;			//BOSS等级(需保存)
	CT_INT32							m_iBossHPMax;
	CT_INT32							m_iBossHP;

	UserHurtMap							m_HPHurtMap;			//当前BOSS伤害
	BossHurtMap							m_BossHurtMap;			//所有BOSS伤害榜
	HPTopMap							m_HPTopMap;				//HP, U_ID
	BossRewardMap						m_RewardMap;			//奖励

	std::list<BattleUser>				m_BattleList;			//战斗排队

public:
	CCBossManager(CT_VOID);
	~CCBossManager(CT_VOID);

public:
	CT_VOID SetActive(CT_BOOL bActive);
	CT_INT32 GetBossGrade(){_AUTOMUTEX(&m_mutex);return m_iBossGrade;};
	//获取活动
	CT_BOOL GetActive(){_AUTOMUTEX(&m_mutex);return m_bActive;};
	//设置剩余时间
	CT_VOID SetLastTime(CT_INT64 lLastTime){_AUTOMUTEX(&m_mutex);m_lLastTime=lLastTime;};
	//获取剩余时间
	CT_INT64 GetLastTime(){_AUTOMUTEX(&m_mutex);return m_lLastTime;};
	//设置活动ID
	CT_VOID SetAEID(CT_INT32 iAEID){_AUTOMUTEX(&m_mutex);m_iAEID=iAEID;};
	//获取活动ID
	CT_INT32 GetAEID(){_AUTOMUTEX(&m_mutex);return m_iAEID;};
	//获取当前伤害排行
	CT_BOOL GetHPHurt(CT_DWORD dwUID, CT_CHAR *pBuf, CT_INT32 bufLen);
	//发送实时数据
	CT_VOID SendHPHurt();
	//获取所有伤害排行
	CT_BOOL GetBossHurt(CT_DWORD dwUID, CT_CHAR *pBuf, CT_INT32 &bufLen);
	//参加活动
	CT_VOID JoinActive(CT_DWORD dwUID);
	CT_VOID ExitActive(CT_DWORD dwUID);
	//激励
	CT_VOID Encourage(CT_DWORD dwUID, CT_INT32 iType);
	//请求战斗 iFlog==1为强攻
	CT_VOID BattleApply(CT_DWORD dwUID, CT_INT32 iFlag);
	//战斗线程战斗返回
	CT_VOID BattleResult(CT_DWORD dwUID, CT_INT32 iHP);

	
	CT_VOID SendReward(CT_DWORD dwUID);		//发送奖励
	CT_VOID RecvReward(CT_DWORD dwUID);		//领取奖励
	CT_VOID UserLogin(CT_DWORD dwUID);

	CT_BOOL BossIsDie() {_AUTOMUTEX(&m_mutex); return (m_iBossHP<=0);};
	CT_INT32 GetBossHP() {_AUTOMUTEX(&m_mutex); return m_iBossHP;};
	CT_INT32 GetBossHPMax() {_AUTOMUTEX(&m_mutex); return m_iBossHPMax;};
	CT_VOID RefCurrTop();
	CT_VOID RefPlayer();

	CT_VOID InitBossHurt(MSC_DB_WorldBossTop &dbBossTop);
	CT_VOID SaveDB();
	CT_VOID RefTop();

private:
	CT_VOID RefBoss(CT_INT32 iBossID);
};


#endif