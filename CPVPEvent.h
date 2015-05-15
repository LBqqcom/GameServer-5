/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CPVPEvent.h
Version:		1.0
Description:    竞技场事件类
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCPVPEVNET_H_
#define CCPVPEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
struct MSC_Fighting_End;
struct MSC_PVP_Fighting_Info;
struct MSC_DB_W_PVPFightingInfo;

class CCPVPEvent:public CCGameMesEvent
{
private:
	CT_CHAR* m_pBuffer;
public:
	CCPVPEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCPVPEvent(void);

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
	//网络消息
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//线程消息
	CT_VOID DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);

private:
	//刷新对手
	CT_VOID DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len);
	//打开宝箱
	CT_VOID DisposeChest(CT_CHAR* pBuf,CT_DWORD len);
	//竞技场战斗
	CT_VOID DisposePVPFighting(CT_CHAR* pBuf,CT_DWORD len);
	//查看战报
	CT_VOID DisposePVPLookFighting(CT_CHAR* pBuf,CT_DWORD len);

private:
	//用户信息
	CT_VOID DisposeMSCUserInfo(CT_CHAR* pBuf,CT_DWORD len);
	//战斗结束
	CT_VOID DisposeMSCFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//竞技场战斗奖励
	CT_VOID PVPFightingReward(CCUser *pUser,const MSC_PVP_Fighting_Info &FightingInfo,MSC_DB_W_PVPFightingInfo &PVPFighting);
	//连胜记录
	CT_VOID ContinuousWin(CCUser *pUser,const MSC_PVP_Fighting_Info &FightingInfo);

private:
	//竞技场验证
	CT_BOOL PVPFightingValidate(CCUser *pUser,CT_INT32 iDiamond);
	//插入对手数据
	CT_VOID DisposeChallgerInsert(CT_CHAR* pBuf, CT_DWORD len, CT_BOOL bHign );
	//对手数据插入完成
	CT_VOID DisposeInsertChallgerFinish(CT_CHAR* pBuf, CT_DWORD len);
};
#endif