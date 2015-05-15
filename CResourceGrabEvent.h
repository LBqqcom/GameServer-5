/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCResourceGrabMgr.h
Version:		1.0
Description:    资源争夺系统消息处理
Author:			LinQ
Create Date:	2013-05-08
History:        资源争夺系统类消息处理

//用户需要显示的信息
6 战队放弃了
7 抢走了
8 的
9  ,掠夺了
10 金币
11 战队成功的抢走了
12 战队:
13 成功的抢走了您的
14 您被掠夺了

15 战队攻击
16 失败
17 战队成功的防御
18 对
19 的攻击
20 ,记着报仇哦!
21 战队成功的占领
22 战队占领
23 的时候被怪物打败
24 战队成员
25 驻守在
26 上
27 收获了
28 已经过期，启动放弃占领

******************************************************************/
#ifndef _CRESOURCE_GRAB_EVENT_H_
#define _CRESOURCE_GRAB_EVENT_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include<map>

class CCFightTeam;
struct G_ResourceQuality;
struct G_ResourceArea;

class CCResourceGrabEvent : public CCGameMesEvent
{
	std::map<CT_INT32,CT_INT32>		m_ResActiveMap;
public:
	CCResourceGrabEvent(CCGameMesManager *pMesManager, CT_WORD id);
	~CCResourceGrabEvent(void);
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	//处理资源争夺
	CT_VOID DisposeResGrab(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//处理资源争夺
	CT_VOID DisposeResFight(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//处理用户场景申请
	CT_VOID DisposeScenceApple(CT_CHAR* pBuf, CT_DWORD len);
	//处理占领请求
	CT_VOID DisposeOccupyApply(CT_CHAR* pBuf, CT_DWORD len);
	//资源生产
	CT_VOID DisposeSpotProductApply(CT_CHAR* pBuf, CT_DWORD len);
	//资源防守
	CT_VOID DisposeSpotDependApply(CT_CHAR* pBuf, CT_DWORD len);
	//用户退出场景
	CT_VOID DisposeSecneExit(CT_CHAR* pBuf, CT_DWORD len);
	//放弃资源
	CT_VOID DisposeSpotGiveUpApply(CT_CHAR* pBuf, CT_DWORD len);
	//资源收获
	CT_VOID DisposeSpotHarvest(CT_CHAR* pBuf, CT_DWORD len);
	//组队pvp战斗
	CT_VOID DisposeResFightPVP(CT_CHAR* pBuf, CT_DWORD len);
	//组队pve战斗
	CT_VOID DisposeResFightPVE(CT_CHAR* pBuf, CT_DWORD len);
	//资源点活动开启
	CT_VOID DisposeResActiveOpen(CT_CHAR* pBuf, CT_DWORD len);
	//资源点读取完成
	CT_VOID DisposeResActiveReadFinish(CT_CHAR* pBuf, CT_DWORD len);
	//资源点插入
	CT_VOID DisposeResInsert(CT_CHAR* pBuf, CT_DWORD len);

	//记录日志
	CT_VOID DisposePvPWinEventLog(CCFightTeam* pChanllageTeam,  CCFightTeam* pFightTeam, 
								  G_ResourceQuality& resQuality,  
								  CT_INT32 iWinIndex, CT_INT32 iLoseIndex, CT_INT32 iResOccupy);

	//处理资源占领记录
	CT_VOID DisposeResOccupyRecord(CT_DWORD dwTeamID, CT_INT32 iScenceID, CT_INT32 iSpotID, CT_INT32 iFood, CT_INT32 iOccupyGold);

	//记录日志
	CT_VOID DisposePvPLoseEventLog(CCFightTeam* pChanllageTeam,  CCFightTeam* pFightTeam,   G_ResourceQuality& resQuality,  
		CT_INT32 iLoseIndex, CT_INT32  iWinIndex);

	//记录日志
	CT_VOID DisposePvEWinEventLog(CCFightTeam* pChanllageTeam, G_ResourceArea& ra, G_ResourceQuality& resQuality,  
		CT_INT32 iFightIndex);

	//记录日志
	CT_VOID DisposePvELoseEventLog(CCFightTeam* pChanllageTeam, G_ResourceArea& ra, G_ResourceQuality& resQuality,  
		CT_INT32 iFightIndex);


private:
	//发送攻打申请成功
	CT_VOID SendOccupySuc(CCFightTeam* pTeam, CT_DWORD dwUserID, CT_INT32 iScenceID, CT_INT32 iSpotID);
	//处理占领数据(无调用)
	CT_VOID SetOccupyData(CT_INT32 iScenceID, CT_INT32 iSpotID, CT_DWORD dwTeamID, CT_INT32 iProtectTime);

private: 
	//计算下次活动时间
	CT_INT64 NextActiveTime(CT_INT32 AcviteList[100]);

	//保存数据
	CT_VOID SaveDataToDB();

};

#endif