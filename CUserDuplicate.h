/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    用户技能数据
Author:			wangchen
Create Date:	2012-10-19
History:        用户副本    

******************************************************************/
#ifndef CCUSERDUPLICATE_H_
#define CCUSERDUPLICATE_H_
#include <map>
#include <list>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"

//场景信息
struct SceneInfo
{
	CT_INT32		iWorldSceneID;		//世界ID
	CT_INT32		iSceneID;			//场景ID
	CT_INT32		iDifficulty;		//难度
	CT_INT32		iSceneStar;			//场景星级
	CT_INT32		iFightingNum;		//战斗次数
};

//扫荡信息
struct RaidsInfo
{
	CT_INT32	iWorldID;			//世界场景
	CT_INT32	iSceneID;			//场景ID
	CT_INT32	iDifficulty;		//难度
	CT_INT32	iAllTime;			//总次数
	CT_INT32	iCurrentTime;		//当前次数
	CT_INT64	lBeginTime;			//开始时间
	CT_INT32	iLastStamina;		//剩余体力
};

//世界奖励
struct WorldReward
{
	CT_INT32	iWorldID;			//世界ID
	CT_INT32	iRewardID;			//奖励ID
};

class CCUser;
class CCMutex;
class CCUserDuplicate1
{
	typedef	 std::map<CT_INT32,WorldReward>				RewardMap;//KEY 世界场景ID,奖励次数
	typedef	 std::map<CT_INT32,WorldReward>::iterator		RD_IT;
	typedef  std::map<CT_INT32,SceneInfo>				DifficyltyMap;
	typedef  std::map<CT_INT32,SceneInfo>::iterator		DM_IT;
	typedef  std::map<CT_INT32,DifficyltyMap>			SceneMap;
	typedef  std::map<CT_INT32,DifficyltyMap>::iterator	SM_IT;
	typedef  std::map<CT_INT32,SceneMap>				WorldMap;
	typedef	 std::map<CT_INT32,SceneMap>::iterator		WM_IT;
	typedef std::list<CMD_G_RaidsReward>				RaidsRewardList;
	typedef std::list<CMD_G_RaidsReward>::iterator		RR_IT;


private:
	CT_INT32		m_iBuyFightingTime;			//购买战斗次数
	RewardMap		m_RewardMap;				//领奖次数
	WorldMap		m_SceneMap;					//场景信息

	RaidsInfo		m_RaidsInfo;				//扫荡信息
	RaidsRewardList	m_RaidsRewardList;			//扫荡奖励数目


private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;

public:
	CCUserDuplicate1(void);
	~CCUserDuplicate1(void);

public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

public:
	//插入场景
	CT_VOID InsterScene(SceneInfo &Sceneinfo);
	//插入奖励
	CT_VOID InsterReward(CT_INT32 iWorldID,CT_INT32 iCount);
	//插入扫荡信息
	CT_VOID InsterRaidsInfo(RaidsInfo &Raidsinfo);
	//插入扫荡记录
	CT_VOID InsertRaidsReward(const CMD_G_RaidsReward RaidsReward);
	//插入购买战斗次数
	CT_VOID InsertBuyFighting(CT_INT32 iBuyFighting){_AUTOMUTEX(m_pMutex);m_iBuyFightingTime=iBuyFighting;};

public:
	//设置场景信息
	CT_VOID SetSceneInfo(SceneInfo &Sceneinfo);

public:
	//获取场景信息
	CT_BOOL GetSceneInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	CT_BOOL GetSceneInfo(CT_INT32 iWorldID,CT_INT32 iSceneID,CT_INT32 iDifficultyID,SceneInfo &Sceneinfo);
	//任务功能增加，判断用户是否通过制定关卡(通过指定关卡且评星达到指定数	例:如通哈卡并且评级至少为3星)
	CT_BOOL GetUserPassedDuplicate(CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iLevel);  //iLevel通关星级
	//获取场景分数
	CT_INT32 GetSceneScore(CT_INT32 iWorldID);
	//获取奖励
	CT_INT32 GetWorldReward(const CT_INT32 iWorldID,const CT_INT32 iRewardID);
	//获取购买次数
	CT_INT32 GetBuyFightingTime(){_AUTOMUTEX(m_pMutex);return m_iBuyFightingTime;};

public:
	//增加购买次数
	CT_VOID AddBuyFightingTime();
	//增加战斗次数
	CT_VOID AddFightingNum(SceneInfo &Sceneinfo);


public:
	//更新场景信息
	CT_VOID UpdataSceneInfo(SceneInfo &Sceneinfo);
	//刷新副本
	CT_VOID UpdataDuplicate();
	//更新奖励
	CT_VOID UpdataSceneReward(const CT_INT32 iWorldID,const CT_INT32 iRewardID);

public:
	//设置扫荡信息
	CT_INT32 SetRaidsInfo(RaidsInfo &Raidsinfo);
	//获取扫荡信息
	CT_VOID GetRaidsInfo(RaidsInfo &Raidsinfo);
	//更新扫荡
	CT_INT32 UpdataRaids();
	//清除扫荡信息
	CT_VOID ClearRaidsInfo();
	//获取扫荡时间
	CT_INT64 GetLastTime(time_t t=0);
	//插入奖励
	CT_BOOL InsertReward();
	//获取奖励
	CT_BOOL GetRaidsReward(CT_CHAR *pBuf,CT_INT32 &iLen,CT_BOOL bAllReward=CT_FALSE);


};
#endif