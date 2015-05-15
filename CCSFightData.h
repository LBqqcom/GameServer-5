/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCSFightData.h
Version:		1.0
Description:    跨服战
Author:			wangchen
Create Date:	2014-2-17
History:             

******************************************************************/

#ifndef CCCSFIGHTDATA_H_
#define CCCSFIGHTDATA_H_
#include "CTType.h"
#include "CGDefine.h"
#include "CMD_Game.h"
#include "CThreadMes.h"
#include "CCrossServerFight.h"


enum CSSeat
{
	Seat_False			= 0,			//无资格
	Seat_True			= 1,			//有资格
};

enum CSPosInfo
{
	Pos_Null			= 0,			//位置空
	Pos_Me				= 1,			//位置自己
	Pos_Other			= 2,			//位置其他人
};

enum CSFinalWin
{
	CS_LOST				= 1,			//
	CS_WIN				= 2,			//
};

class CCCSFightData : public CCCrossServerFight,public CCSingleton<CCCSFightData>
{
public:
	struct CSUserSupport
	{
		CT_DWORD	dwUserID;
		CT_INT32	iResID;
		CT_INT32	iNum;
		CT_DWORD	dwFTID;
		CT_INT32	iGSID;
		CT_INT32	iIndex;
	};

	struct CSUserReward
	{
		CT_INT32	iType;
		CT_INT32	iResID;
		CT_INT32	iNum;
	};
public:
	typedef std::map<CT_INT64,CSUserSupport>								CSSupportMap;	//个人支持
	typedef std::map<CT_DWORD,CT_INT64>										CSFTCoolTimeMap;//冷却时间
	typedef std::map<CT_INT64,CSUserReward>									CSRewardMap;
	typedef std::map<CT_DWORD,CSRewardMap>									CSUserRewardMap;
private:
	CCMutex					m_Mutex;
	CSSupportMap			m_CSSupport;						//个人支持
	CSFTCoolTimeMap			m_CSFTCoolTime;						//冷却时间
	CSUserRewardMap			m_CSUserReward;						//用户奖励
	CT_INT32				m_iSeat;							//资格
	CT_INT32				m_iStatus;							//状态
public:
	CCCSFightData(void);
	~CCCSFightData(void);

public:
	//插入个人支持
	CT_VOID InsertSupport(CSUserSupport &csus);
	//插入冷却时间
	CT_VOID InsertCoolTime(CT_DWORD dwFTID,CT_INT64 lTime);
	//插入资格
	CT_VOID InsertSeat(CT_INT32 iSeat);
	//插入奖励
	CT_VOID InsertReward(CT_DWORD dwUserID,CSUserReward &csur,CT_BOOL bSave=CT_TRUE);
	
	
public:
	//设置状态
	CT_VOID SetStatus(CT_INT32 iStatus){_AUTOMUTEX(&m_Mutex);m_iStatus=iStatus;};

public:
	//获取个人支持
	CT_BOOL GetSupport(CT_DWORD dwUserID,CT_INT32 iStatus,CSUserSupport &csus);
	//获取冷却时间
	CT_INT64 GetFTCoolTime(CT_DWORD dwFTID);
	//获取资格
	CT_INT32 GetSeat(CT_DWORD dwFTID);
	//获取资格
	CT_INT32 GetSeat(){_AUTOMUTEX(&m_Mutex);return m_iSeat;};
	//获取对手位置
	CT_INT32 GetOpponentPos(CT_DWORD dwFTID,CT_INT32 iGSID,CT_INT32 iStatus);
	//获取状态
	CT_INT32 GetStatus(){_AUTOMUTEX(&m_Mutex);return m_iStatus;};
	//获取奖励
	CT_BOOL GetUserReward(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//支持奖励计算
	CT_VOID SupportReward();
	//8强奖励计算
	CT_VOID FinalEightReward(CT_INT32 iWin,CT_DWORD dwUserID);
	//获取战队资格战战报
	CT_BOOL GetCSFightRecord(CT_DWORD dwFTID,CT_INT32 iGSID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取8强战报
	CT_BOOL GetCSFightRecord(CT_CHAR cIndex[],CT_INT32 iIndexLen,CSFightRecord &csfr);

public:
	//清楚支持数据
	CT_VOID ClearSupport(){_AUTOMUTEX(&m_Mutex);m_CSSupport.clear();};
	//清除奖励数据
	CT_VOID ClearReward();
	//清楚奖励
	CT_VOID ClearReward(CT_DWORD dwUserID);
};
#endif
