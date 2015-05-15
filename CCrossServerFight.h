/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicate.h
Version:		1.0
Description:    跨服战
Author:			wangchen
Create Date:	2014-2-17
History:             

******************************************************************/

#ifndef CCCROSSSERVERFIGHT_H_
#define CCCROSSSERVERFIGHT_H_
#include "CTType.h"
#include "CGDefine.h"
#include "CMD_Game.h"
#include "../GameServer/CUserPVPInfo.h"
#include "../GameServer/CFightTeam.h"

class CCMutex;
struct UserFightingStatus;
struct CSInspore;
//8强战报索引位置
/*
	1				3
		5	7	6	
	2				4
*/
enum CSFightRecordType
{
	Current_Record		= 0,			//当前记录
	Last_Rcord			= 1,			//上次记录
};

enum CSStatus
{
	Final_Eight			= 0,			//8强前
	Final_Four			= 1,			//4强前
	Final_Two			= 2,			//半决赛前
	Final_One			= 3,			//决赛
	Seat_Fight			= 4,			//资格赛
	Seat_Fight_Befor	= 5,			//资格赛前
	CS_Close			= 255,			//关闭状态
};

enum CSFightRecordIndex
{
	Current_Index		= 1,			//当前索引
	Last_Index			= 8,			//上一轮索引
};

class CCCrossServerFight
{
public:
	struct FinalEightData
	{
		FinalEight			fe;											//位置信息
		CT_CHAR				cUserName[CLAN_CRUSADE_MEMBER][NAME_SIZE];	//出阵人员姓名
		CT_DWORD			dwUserID[CLAN_CRUSADE_MEMBER];				//出阵人员id
		UserFightingStatus	ufs[CLAN_CRUSADE_MEMBER];					//战斗数据
		CSInspore			csi[CLAN_CRUSADE_MEMBER];					//个人威能
		CT_CHAR cIndex[CROSS_SERVER_RECORD_INDEX+1];					//战报索引(8强需要)
	};

	struct CSFightRecord
	{
		CT_CHAR cIndex[CROSS_SERVER_RECORD_INDEX+1];
		CT_CHAR cFightTeamName[2][NAME_SIZE+1];
		CT_INT32 iGameServerID[2];
		CT_DWORD dwFTID[2];
		CT_INT32 iWiner;
		CT_INT32 iSeed;
		UserFightingStatus ufs[2][CLAN_CRUSADE_MEMBER];
	};

	struct CSSupport
	{
		CT_INT32	iResID;
		CT_INT32	iNum;
		CT_INT32	iPos;
		CT_INT32	iStatus;
	};

	//战报索引
	struct RecordIndex
	{
		CT_DWORD	dwFTID;
		CT_CHAR		cIndex[CROSS_SERVER_RECORD_INDEX+1];
	};

	typedef std::map<CT_INT32,FinalEightData>			FinalEightDataMap; //位置为key
	typedef std::map<CT_INT64,CSSeatFightReady>			SeatFightTeamMap;	//资格战战队
	typedef std::map<std::string,CSFightRecord>			FightRecordMap;	//md5做key
	typedef std::list<RecordIndex>						RecordList;		//战报索引
	typedef std::map<CT_DWORD,RecordList>				CSFightRecordMap;//战队ID
	typedef std::map<CT_INT64,CSSupport>				CSSupportDataMap;//阶段投注信息

private: 
	CCMutex						m_Mutex;
	FinalEightDataMap			m_FinalEightData;			//当前8个位置数据（包含了8强战）
	FinalEightDataMap			m_LastFinaEightData;		//上期8个位置
	CSFightRecordMap			m_FinalEightRecord;			//当前8个位置战斗记录（资格战,游戏服务独立储存）
	CSFightRecordMap			m_LastFinalEightRecord;		//上期8个位置战斗记录（资格战,游戏服务独立储存）
	FightRecordMap				m_FightRecord;				//战报记录
	SeatFightTeamMap			m_SeatFightTeam;			//资格列表
	CSSupportDataMap			m_SupportData;				//支持信息(当前状态0)

public:
	CCCrossServerFight(void);
	~CCCrossServerFight(void);

	CCCrossServerFight(const CCCrossServerFight &cssf){};

public:
	//插入资格战队
	CT_VOID InsertSeatFightTeam(CSSeatFightReady &cssfr);
	//插入8强数据
	CT_VOID InsertFinalEightData(CT_INT32 iIndex,FinalEightData &fed);
	//插入资格战斗记录
	CT_VOID InsertCSFightRecord(CT_INT32 iIndex,CSFightRecord &csfr);
	//插入资格战斗记录
	CT_VOID InsertCSFightRecordUser(CT_CHAR cIndex[],CT_INT32 iGroup,CT_INT32 iPos,UserFightingStatus &ufs);
	//插入8强战斗记录(进击赛索引特殊定义1-7:当前8强 8-14赛程回顾)
	CT_VOID InsertCSFinalEightRecord(CSFightRecord &csfr);
	//插入8强战斗记录用户数据
	CT_VOID InsertCSFightEightRecordUser(CT_CHAR cIndex[],CT_INT32 iGroup,CT_INT32 iPos,UserFightingStatus &ufs);
	//插入支持信息
	CT_VOID InsertCSSupportData(CT_INT32 iStatus,CSSupport &css);


public:
	//获取资格赛战队
	CT_BOOL GetSeatFightTeam(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取8强数据
	CT_BOOL GetFinalEightData(CT_INT32 iIndex,CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取8强数据
	CT_BOOL GetFinalEightData(CT_INT32 iIndex,CT_INT32 iPos,FinalEightData &fed);
	//获取8个位置战报
	CT_BOOL GetCSFightRecord(CT_INT32 iIndex,CT_DWORD dwFTID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取8强战报
	CT_BOOL GetCSFightRecord(CT_CHAR cIndex[],CT_INT32 iIndexLen,CSFightRecord &csfr);
	//获取战队资格战战报
	CT_BOOL GetCSFightRecord(CT_DWORD dwFTID,CT_CHAR *pBuf,CT_INT32 &iLen);
	//获得支持
	CT_BOOL GetCSSupportData(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获得支持
	CT_BOOL GetCSSupportData(CT_INT32 iStatus,CT_INT32 iPos,CSSupport &css);
	//获取战队信息
	CT_BOOL GetFTInfo(CT_DWORD dwFTID,CT_INT32 iGSID,FinalEightData &fed);

public:
	//更新用户信息
	CT_VOID UpdateUserData(CT_INT32 iGSID,UserFightingStatus  &ufs,CSInspore &csi);
	//更新阵法信息
	CT_VOID UpdateFTPos(CT_DWORD dwFTID,CT_INT32 iGSID,UserFightingStatus *pufs,CT_INT32 iCount=CLAN_CRUSADE_MEMBER);

public:
	//
	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);
	//新比赛
	CT_VOID NewMatch();
	//获取领主战斗属性
	CT_VOID GetMasterFightingData(const UserFightingStatus &data,CombatUnitData &masterdata);
};
#endif
