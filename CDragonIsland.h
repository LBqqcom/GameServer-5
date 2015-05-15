/****************************************************
	created:	2014/07/10		14:52
	file base:	DragonIsland
	file ext:	h
	author:		zhouhuan

	purpose:
*****************************************************/

#ifndef CDRAGONISLAND_H
#define CDRAGONISLAND_H
 
#include "CMutex.h"
#include "CGDefine.h"
#include <vector>

class CCUser;

struct Point 
{
	CT_INT32	iPointID;		// 棋点ID
	CT_INT32	iType;			// 事件类型
	CT_INT32	iEventID;		// 事件ID
	CT_INT32	iStatus;		// 事件状态
	CT_INT32	iStep;			// 投掷点数
	CT_INT32	iBeginChest;	// 起点宝箱ID，没有为0
	CT_INT32	iEventChest;	// 事件奖励
};

struct DragonPlayer
{
	CT_INT32	iIsOpen;		// 龙岛是否开启
	CT_INT32	iTotalTimes;	// 拥有投掷次数
	CT_INT32	iThrowTimes;	// 已投掷次数		
	CT_INT32	iBeginTimes;	// 经过起始点的次数
	CT_INT32	iGetTimes;		// 接收次数
	CT_INT32	iFightTeamID;	// 所在战队ID
	CT_INT32	iScore;			// 战队积分
	CT_INT32	iRewardTimes;	// 奖励获得投掷次数
};

class CCUserIsland
{
private:
	typedef std::vector<CT_INT32>	GiveUserIDVector;
	typedef std::vector<CT_INT32>	TeamChestIDVector;

	typedef std::vector<std::string>	GetPresentNameVector;

	CCUser*			m_pUser;
	CCMutex*		m_pMutex;
	Point			m_Point;
	DragonPlayer	m_DPlayer;
	GiveUserIDVector		m_GiveUserIDVector;
	TeamChestIDVector		m_TeamChestIDVector;
	GetPresentNameVector	m_GetPresentNameVector;

public:
	CCUserIsland();
	~CCUserIsland();

public:
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		memset(&m_Point, 0, sizeof(m_Point));
		memset(&m_DPlayer, 0, sizeof(m_DPlayer));
		m_GiveUserIDVector.clear();
		m_TeamChestIDVector.clear();
		m_GetPresentNameVector.clear();
	}
	CT_VOID Clear()
	{
		memset(&m_Point, 0, sizeof(m_Point));
		memset(&m_DPlayer, 0, sizeof(m_DPlayer));
		m_GiveUserIDVector.clear();
		m_TeamChestIDVector.clear();
		m_GetPresentNameVector.clear();
	}

public:
	// 获取龙岛数据
	CT_BOOL GetIslandData(CT_CHAR* pBuf, CT_INT32& iLen, CT_INT32 iFTID);
	// 读入已赠送玩家ID
	CT_VOID InsertGiveUserID(CT_DWORD dwFID) { _AUTOMUTEX(m_pMutex); m_GiveUserIDVector.push_back(dwFID); }
	// 读入已领取战队宝箱
	CT_VOID InsertTeamChest(CT_INT32 iTeamLimit) { _AUTOMUTEX(m_pMutex); m_TeamChestIDVector.push_back(iTeamLimit); }
	// 读入赠送自己次数的玩家昵称
	CT_VOID InsertPresentName(std::string &strName) {  _AUTOMUTEX(m_pMutex); m_GetPresentNameVector.push_back(strName);	}
	// 掷骰子
	CT_INT32 Throw();
	// 判断并完成当前任务
	CT_VOID CheckAndDoTask(CT_INT32 iTaskID,CT_INT32 iFTID);
	// 战斗事件
	CT_INT32 FightEvent(CT_INT32 iEventID);
	// 立即完成
	CT_INT32 CompleteImmediately(CT_INT32& iEventID,CT_INT32 iFTID);
	// 放弃事件
	CT_VOID GiveUpEvent();
	// 领取宝箱
	CT_INT32 SendReward(CT_INT32 iType, CT_INT32 iChestID, CT_INT32 iLimit, CT_INT32 iFTID);
	// 获取当前棋点信息
	CT_VOID GetPoint(Point& point){ point = m_Point; }
	// 获取玩家信息
	CT_VOID GetPlayerInfo(DragonPlayer& player){ player = m_DPlayer; }
	// 改变棋点
	CT_VOID SetPoint(Point point);
	// 更新玩家信息
	CT_VOID SetPlayerInfo(DragonPlayer player);
	// 退出战队
	CT_VOID TeamOut();
	// 加入战队
	CT_VOID TeamIn(CT_INT32 iFTID);
	// 检查当天是否已赠送同一玩家
	CT_BOOL CheckGiveUid(CT_DWORD dwFUID);
	// 检查战队积分宝箱是否已领取
	CT_BOOL CheckScoreChest(CT_INT32 iChestID);
	// 开启龙岛
	CT_VOID OpenFunction(CT_INT32 iFTID);
	// 定时更新龙岛数据
	CT_VOID UpdateIslandOnClock(CT_INT32 iFTID);
	// 查看龙岛是否开启
	CT_BOOL ViewIsland() { return m_DPlayer.iIsOpen == 0?CT_FALSE:CT_TRUE; }
	// 增加玩家投掷次数
	CT_VOID AddThrowTimes(CT_INT32 iCount);
	// 更新战队积分
	CT_VOID UpdateTeamScore(CT_INT32 iTeamID);

private:
	// 更新玩家战队积分
	CT_VOID UpdateFTMemberScore(CT_INT32 iScore, CT_INT32 iTeamID);
	// 更新数据到数据库
	CT_VOID UpdateIslandData();
	// 更新数据库日志
	CT_VOID UpdateIslandDataLog();
	// 发送战队积分公告
	CT_VOID TeamScoreBroadcast(CT_INT32 iLimit, CT_INT32 iIndex, CT_CHAR* pName);
};

#endif