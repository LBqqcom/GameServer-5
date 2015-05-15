/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTopEvent.h
Version:		1.0
Description:    排行榜事件类
Author:			wangchen
Create Date:	2013-5-23
History:             

******************************************************************/
#ifndef CCTOPEVNET_H_
#define CCTOPEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
class CCTopEvent:public CCGameMesEvent
{
private: 
	CT_CHAR				*m_pBuffer;			//排行榜缓冲
public:
	CCTopEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCTopEvent(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	//处理排行榜的更新 彭文整理
	CT_VOID DisposeUpdataTop();
	//处理数据发来的领主排行数据 彭文整理
	CT_VOID DisposeMasterInfo(CT_CHAR* pBuf, CT_DWORD len);
	//领主信息发完了  彭文整理
	CT_VOID DisposeMasterFinish();
	//处理用户消息， 彭文整理
	CT_VOID DisposeUserMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//处理排序信息
	CT_VOID DisposeTopInfo(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//挑战返回
	CT_VOID DisposeTopFighting(CT_CHAR* pBuf, CT_DWORD len);

private:
	//军衔排行
	CT_VOID DisposeRanking(CT_CHAR *pBuf,CT_INT32 iLen);
	//等级排行
	CT_VOID DisposeGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//战斗力排行
	CT_VOID DisposeFightingPower(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队排行
	CT_VOID DisposeFightingTeam(CT_CHAR *pBuf,CT_INT32 iLen);
	//连胜排行
	CT_VOID DisposeContinuousWin(CT_CHAR *pBuf,CT_INT32 iLen);
	//查看玩家
	CT_VOID DisposeViewUser(CT_CHAR *pBuf,CT_INT32 iLen);
	//战旗排行
	CT_VOID DisposeFightingFlagPower(CT_CHAR *pBuf,CT_INT32 iLen);
	//排行榜挑战
	CT_VOID DisposeChallenge(CT_CHAR *pBuf,CT_INT32 iLen);
	//掠夺排行
	CT_VOID DisposePlunderRes(CT_CHAR* pBuf, CT_INT32 iLen);

	//排行榜奖励
	CT_VOID DisposeTopReward(CT_CHAR* pBuf,CT_INT32 iLen);

	//发送奖励领取状态
	CT_VOID SendTopRewardStatus(CCUser* pUser, CT_INT32 iType);

private:
	//挑战验证
	CT_INT32 TopChallengeValidate(CCUser *pUser,CT_INT32 iDiamod,CT_INT32 iCoolTime,CMD_G_ReturnFail &rf);

};
#endif
