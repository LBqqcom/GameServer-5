/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicateEevnt.h
Version:		1.0
Description:    跨服战事件类
Author:			wangchen
Create Date:	2014-2-17
History:             

******************************************************************/
#ifndef CCCROSSSERVERFIGHTEVENT_H_
#define CCCROSSSERVERFIGHTEVENT_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
class CCFightTeam;
struct CSInspore;
struct UserFightingStatus;
class CCCrossServerFightEvent:public CCGameMesEvent
{
public:
	CCCrossServerFightEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCCrossServerFightEvent(void);

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
	//线程消息
	CT_VOID OnMscUserEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//网络消息
	CT_VOID OnNetUserEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//赛程回顾
	CT_VOID DisposeScheduleReviewApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//领奖请求	
	CT_VOID DisposeRewardApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//查看活动	
	CT_VOID DisposeCSLookActive(CT_CHAR *pBuf,CT_INT32 iLen);
	//支持请求	
	CT_VOID DisposeCSSupportApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//人气排行请求	
	CT_VOID DisposeCSPopTopApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//准备信息请求	
	CT_VOID DisposeCSReadyInfoApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//资格赛前准备请求
	CT_VOID DisposeSeatBeforApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//鼓舞请求
	CT_VOID DisposeCSInsporeApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//同步当前8强
	CT_VOID DisposeCurrentFinalEight(CT_CHAR *pBuf,CT_INT32 iLen);
	//同步上届8强
	CT_VOID DisposeLastFinalEight(CT_CHAR *pBuf,CT_INT32 iLen);
	//返回状态
	CT_VOID DisposeReturnStatus(CT_CHAR *pBuf,CT_INT32 iLen);
	//资格赛战队
	CT_VOID DisposeSeatFightTeam(CT_CHAR *pBuf,CT_INT32 iLen);
	//资格战斗
	CT_VOID DisposeSeatFighting(CT_CHAR *pBuf,CT_INT32 iLen,CT_DWORD dwSubID);
	//战斗结果返回
	CT_VOID DisposeSeatEndingReturn(CT_CHAR *pBuf,CT_INT32 iLen);
	//同步用户数据
	CT_VOID DipsoseUpdateUserData(CT_CHAR *pBuf,CT_INT32 iLen);
	//跨服返回同步用户数据
	CT_VOID DisposeCSUpdateUserData(CT_CHAR *pBuf,CT_INT32 iLen);
	//查看战报
	CT_VOID DisposeCSLookFightRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//战队阵法同步
	CT_VOID DisposeCSChangeFTPos(CT_CHAR *pBuf,CT_INT32 iLen);
	//同步战报
	CT_VOID DisposeCSSynFightRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//同步战报用户
	CT_VOID DisposeRecordUser(CT_CHAR *pBuf,CT_INT32 iLen);

private:
	//活动开启
	CT_VOID DisposeActiveBegin(CT_CHAR *pBuf,CT_INT32 iLen);
	//战斗结果
	CT_VOID DisposeCSFightingEnd(CT_CHAR *pBuf,CT_INT32 iLen);
	//8强战斗结果
	CT_VOID DisposeCSFinelEightFightingEnd(CT_CHAR *pBuf,CT_INT32 iLen);

private:
	//支持资源验证
	CT_INT32 SupportVaidate(CCUser *pUser,CMD_G_CSSupportApply &CSSa,G_CSFightBet &csb);
	//8强战斗
	CT_VOID FinalEightFight(CT_INT32 iSeed);
	//8强战报索引转换
	CT_VOID FinalEightFightChangeIndex(CT_INT32 iStatus,CT_INT32 iPos,CT_CHAR *pIndex);
	//鼓舞资源验证
	CT_INT32 InsporeValidate(CCUser *pUser,CCFightTeam *pFT,CT_INT32 iID,CMD_G_NormalReward &nr);
	//8强奖励
	CT_VOID FinalEightReward();
	//全服奖励
	CT_VOID FullServerReward();
	//新比赛
	CT_VOID NewMatch();
	//威能加成
	CT_VOID AddInsport(UserFightingStatus &msc,CSInspore &csi);
	//冠军公告
	CT_VOID ChampionBroadcast();
	//删除BOSS
	CT_VOID DeleteBoss(const CT_INT32 iPos);

};
#endif
