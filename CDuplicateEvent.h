/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDuplicateEvent.h
Version:		1.0
Description:    副本事件类
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCDUPLICATEEVNET_H_
#define CCDUPLICATEEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCDuplicateEvent1:public CCGameMesEvent
{
public:
	CCDuplicateEvent1(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCDuplicateEvent1(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID){};
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//网络消息
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//线程消息
	CT_VOID DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//进入场景
	CT_BOOL DisposeEnterScene(CT_CHAR* pBuf,CT_DWORD len);
	//领取奖励
	CT_BOOL DisposeGetReward(CT_CHAR* pBuf,CT_DWORD len);
	//观战
	CT_VOID	DisposeLookFighting(CT_CHAR* pBuf,CT_DWORD len);
	//副本扫荡
	CT_VOID	DisposeDuplicateRaids(CT_CHAR* pBuf,CT_DWORD len);
	//扫荡加速
	CT_VOID	DisposeRaidsSpeed(CT_CHAR* pBuf,CT_DWORD len);
	//请求奖励
	CT_VOID	DisposeRequestReward(CT_CHAR* pBuf,CT_DWORD len);
	//扫荡失败
	CT_VOID	DisposeRaidsFail(CT_CHAR* pBuf,CT_DWORD len);
	//同步加速钻石
	CT_VOID DisposeRaidsSpeedDiamod(CT_CHAR* pBuf,CT_DWORD len);


private:
	//战斗结束
	CT_BOOL DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//扫荡奖励
	CT_VOID DisposeRaidsReward(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeFightingReward(CT_CHAR* pBuf,CT_DWORD len);
public:
	//进入场景验证
	CT_INT32 EnterSceneValidate(SceneInfo &Sceneinfo,CCUser *pUser,CT_INT32 iDiamod);
	//扫荡验证
	CT_INT32 RaidsValidate(RaidsInfo &Raidsinfo,CCUser *pUser,CT_INT32 iTime=1);
	//扫荡记录 
	CT_VOID RaidsSave(CMD_G_RaidsReward &Reward,RaidsInfo &Raidsinfo);



};

#endif
