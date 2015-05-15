/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicateEevnt.h
Version:		1.0
Description:    组队副本事件类
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCTEAMDUPLICATEEVNET_H_
#define CCTEAMDUPLICATEEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
class CCTeamDuplicateEvent:public CCGameMesEvent
{
public:
	CCTeamDuplicateEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCTeamDuplicateEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//创建房间
	CT_VOID DisposeCreateRoom(CT_CHAR* pBuf,CT_DWORD len);
	//加入房间
	CT_VOID DisposeAddRoom(CT_CHAR* pBuf,CT_DWORD len);
	//快速加入
	CT_VOID DisposeFastAddRoom(CT_CHAR* pBuf,CT_DWORD len);
	//阵位调整
	CT_VOID DisposePos(CT_CHAR* pBuf,CT_DWORD len);
	//玩家准备
	CT_VOID DisposeReady(CT_CHAR* pBuf,CT_DWORD len);
	//玩家开始
	CT_VOID DisposeBegin(CT_CHAR* pBuf,CT_DWORD len);
	//房间信息调整
	CT_VOID DisposeChangeRoomInfo(CT_CHAR* pBuf,CT_DWORD len);
	//邀请玩家
	CT_VOID DisposeInvitePlayer(CT_CHAR* pBuf,CT_DWORD len);
	//自动匹配
	CT_VOID DiposeAutoMatch(CT_CHAR* pBuf,CT_DWORD len);
	//玩家离开
	CT_VOID DisposeLeave(CT_CHAR* pBuf,CT_DWORD len);
	//进入组队副本
	CT_VOID DiposeEnter(CT_CHAR* pBuf,CT_DWORD len,CT_INT32 iSubID);
	//拒绝邀请
	CT_VOID DiposeRefuse(CT_CHAR* pBuf,CT_DWORD len);
	//战斗结束
	CT_VOID DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//自动匹配
	CT_VOID DisposeAutoMatchInfo(CT_CHAR* pBuf,CT_DWORD len);
	//解锁房间
	CT_VOID DisposeClear(CT_CHAR *pBuf,CT_DWORD len);

private:
	//更新场景
	CT_VOID UpdataScene(CT_DWORD dwUserID,CT_INT32 iScene,CT_INT32 iDifficlty);
	//组队副本奖励
	CT_VOID TeamDuplicateReward(CCUser *pUser,const CT_INT32 iSceneID,const CT_INT32 iDifficulty);
};

#endif
