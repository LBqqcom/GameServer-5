/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCTowerEvent.h
Version:		1.0
Description:    元素之塔事件服务
Author:			pengwen
Create Date:	2013-8-14
History:        处理用户事件     

******************************************************************/

#ifndef CCTOWEREVNET_H_
#define CCTOWEREVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCTowerEvent:public CCGameMesEvent
{
public:
	CCTowerEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCTowerEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//处理元素之塔的网络消息
	CT_VOID DisposeElementTower(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//处理塔信息请求
	CT_VOID DisposeElementTowerInfoApply(CT_CHAR* pBuf,CT_DWORD len);
	//处理元素之塔的网络消息
	CT_VOID DisposeElementTowerOccupy(CT_CHAR* pBuf,CT_DWORD len);
	//处理用户离线数据
	CT_VOID DisposeTowerOfflineData(CT_CHAR* pBuf,CT_DWORD len);
	//处理战斗线程发来的信息
	CT_VOID DisposTowerPVPFromFight(CT_CHAR* pBuf,CT_DWORD len);
	//助理朋友协助
	CT_VOID DisposeTowerFriendHelpApply(CT_CHAR* pBuf,CT_DWORD len);
	//查看战斗
	CT_VOID DisposeTowerLookFighting(CT_CHAR* pBuf,CT_DWORD len);
	//用户退出
	CT_VOID DisposeTowerUserExit(CT_CHAR* pBuf,CT_DWORD len);

	//用户放弃
	CT_VOID DisposeElementTowerGiveupApply(CT_CHAR* pBuf,CT_DWORD len);

	//关卡战斗请求
	CT_VOID DisposeTowerPassApply(CT_CHAR* pBuf,CT_DWORD len);
	//扫荡
	CT_VOID DisposeTowerRaids(CT_CHAR* pBuf,CT_DWORD len);
	//使用宝箱
	CT_VOID DisposeTowerUseTreasure(CT_CHAR* pBuf,CT_DWORD len);
	//战斗结果
	CT_VOID DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//查看系统留言
	CT_VOID DisposeTowerMSGRead(CT_CHAR* pBuf,CT_DWORD len);
	//查看最早战斗攻略
	CT_VOID DisposeTowerLookFight(CT_CHAR* pBuf,CT_DWORD len);
	//
	CT_VOID DisposeTowerSlotAwardGet(CT_CHAR* pBuf,CT_DWORD len);
	//
	CT_VOID DisposeTowerTreasureAbandon(CT_CHAR* pBuf,CT_DWORD len);


private:
	//占领成功
	CT_VOID DisposeOccupySuc(const ElemTowerUserInfo& userInfo, CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlotID,const CMD_G_NormalReward&  costRes, CT_INT32 iOccupyTime);
	//占领失败
	CT_VOID DisposeOccupyLose(CT_DWORD dwUserID, CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlotID,const CMD_G_NormalReward&  costRes);

};


#endif

