/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingRecord.h
Version:		1.0
Description:    排行榜
Author:			wangchen
Create Date:	2012-10-25
History:             

******************************************************************/

#ifndef CCTOP_H_
#define CCTOP_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CGDefine.h"
#include "CThreadMes.h"
#include<list>
#include<map>

struct User_Fighting;
class CCMutex;


//等级排行信息
struct TopGrade
{
	CT_DWORD	dwUserID;					//用户ID
	CT_CHAR		cName[NAME_SIZE];			//用户名称
	CT_INT32	iGrade;						//用户等级
	CT_INT32	iOccupation;				//用户职业
	CT_CHAR		cFightingTeam[NAME_SIZE];	//战队
	CT_INT32	iFightingPower;				//战斗力
	CT_INT32	iTopGrade;					//等级排行
};

//军衔排行榜
struct TopRanking
{
	CT_DWORD	dwUserID;					//用户ID
	CT_CHAR		cName[NAME_SIZE];			//用户名称
	CT_INT32	iGrade;						//用户等级
	CT_INT32	iOccupation;				//用户职业
	CT_INT32	iRankingGrade;				//军衔等级	
	CT_INT32	iFightingPower;				//战斗力
	CT_INT32	iWinTimes;					//胜利次数
	CT_INT32	iTopRanking;				//军衔排行榜
};

//战力排行榜
struct TopFightingPower
{
	CT_DWORD	dwUserID;					//用户ID
	CT_CHAR		cName[NAME_SIZE];			//用户名称
	CT_INT32	iGrade;						//用户等级
	CT_INT32	iOccupation;				//用户职业
	CT_CHAR		cFightingTeam[NAME_SIZE];	//战队
	CT_INT32	iFightingPower;				//战斗力
	CT_INT32	iTopFightingPower;			//战斗力排行榜
};

//战队排行榜
struct TopFightingTeam
{
	CT_DWORD	dwFightingTeamID;				//战队ID
	CT_INT32	iTopFightingTeam;				//战队排行榜
	CT_CHAR		cFightingTeamName[NAME_SIZE];	//战队名称
	CT_CHAR		cMasterName[NAME_SIZE];			//首领名称
	CT_INT32	iPlayerNum;						//成员数目
	CT_INT32	iFightingPower;				//战斗力
};

//连胜排行榜
struct TopContinuousWin
{
	CT_DWORD	dwUserID;					//用户ID
	CT_CHAR		cName[NAME_SIZE];			//用户名称
	CT_INT32	iGrade;						//用户等级
	CT_INT32	iOccupation;				//用户职业
	CT_INT32	iContinuousWinNum;			//连胜次数
	CT_INT32	iFightingPower;				//战斗力
};

//战旗排行榜
struct TopFightingFlag
{
	CT_DWORD	dwUserID;
	CT_CHAR		cName[NAME_SIZE];
	CT_INT32	iGrade;
	CT_INT32	iOccupation;
	CT_INT32	iFightingFlagPower;
	CT_CHAR		cFightingTeam[NAME_SIZE];
};

//掠夺资源排行榜
struct TopPlunderRes 
{
	CT_DWORD	dwUserID;
	CT_CHAR		cName[NAME_SIZE];
	CT_INT32	iGrade;
	CT_INT32	iOccupation;
	CT_INT32	iPlunderNum;
	CT_CHAR		cFightingTeam[NAME_SIZE];
};

//排序
struct MaterInfo
{
	CT_DWORD	dwUserID;					//用户id
	CT_INT32	iGrade;						//等级
	CT_INT32	iExp;						//经验
	CT_INT32	iFightingPower;				//战斗力
	CT_INT32	iCurrContWin;				//当前连胜
	CT_INT32	iRankGrade;					//军衔等级
	CT_INT32	iRankExp;					//军衔经验
	CT_INT32	iTopGrade;					//等级排行
	CT_INT32	iTopRank;					//军衔等级
	CT_INT32	iTopFightingPower;			//战斗力排行
	CT_INT32	iTopContWin;				//连胜排行
	CT_INT32	iNowTopGrade;				//当前等级排名
	CT_INT32	iNowTopRank;				//当前军衔排名
	CT_INT32	iNowTopFightingPower;		//当前战斗力排名
	CT_INT32	iNowTopContWin;				//当前连胜排名
	CT_INT32	iFightingFlagPower;			//战旗战斗力
	CT_INT32	iNowTopFightingFlagPower;	//当前战旗排名
	CT_INT32	iTopFightingFlagPower;		//战旗排行榜
	CT_INT32	iPlunderScore;				//掠夺积分
	CT_INT32	iTopPlunder;				//掠夺排行
	CT_INT32	iNowTopPlunder;				//当前掠夺排行
};

class CCTop : public CCSingleton<CCTop>
{
	typedef std::map<CT_DWORD,LookUserInfo>				UserDataMap;
	typedef std::map<CT_DWORD,LookUserInfo>::iterator		UD_IT;
	typedef std::list<TopGrade>								GradeList;
	typedef std::list<TopGrade>::iterator					GL_IT;
	typedef std::list<TopRanking>							RankingList;
	typedef std::list<TopRanking>::iterator					RL_IT;
	typedef std::list<TopFightingPower>						FightingPowerList;
	typedef std::list<TopFightingPower>::iterator			FP_IT;
	typedef std::list<TopFightingTeam>						FightingTeamList;
	typedef std::list<TopFightingTeam>::iterator			FT_IT;
	typedef std::list<TopContinuousWin>						ContinuousWinList;
	typedef std::list<TopContinuousWin>::iterator			CW_IT;
	typedef std::list<TopFightingFlag>						FightingFlagList;
	typedef std::list<TopFightingFlag>::iterator			FF_IT;
	typedef std::list<TopPlunderRes>						PlunderResList;
	typedef std::list<TopPlunderRes>::iterator				PR_IT;

public:
	typedef std::map<CT_DWORD,MaterInfo>					MaterInfoMap;
	typedef std::map<CT_DWORD,MaterInfo>::iterator			MI_IT;
	typedef std::multimap<CT_INT64,CT_DWORD>				SequenceMap;
	typedef std::multimap<CT_INT64,CT_DWORD>::iterator		SM_IT;

private:
	CCMutex			m_Mutex;
	CT_INT32		m_iIndex;								//
	CT_DWORD		m_dwUserID[TOP_COUNT];			//用户id
	User_Fighting	m_UserInfo[TOP_COUNT];			//

	UserDataMap			m_UserDataMap;							//用户信息
	GradeList			m_GradeList;							//等级排行榜
	RankingList			m_RankingList;							//军衔排行榜
	FightingPowerList	m_FightingPowerList;					//战斗力排行榜
	FightingTeamList	m_FightingTeamList;						//战队排行榜
	ContinuousWinList	m_ContinuousWinList;					//连胜排行榜
	FightingFlagList	m_FightingFlagList;						//战旗排行榜
	PlunderResList		m_PlunderResList;						//掠夺资源排行榜

	MaterInfoMap		m_MasterInfoMap;						//领主信息

	CT_BOOL				m_bInit;								//排行榜最后读取，做为服务初始化完成标致


public:
	CCTop(void);
	~CCTop(void);

public:
	//插入战队排名
	CT_VOID InsertTopFightingTeam(CT_DWORD *pTeamID,CT_INT32 iCount);
	//插入用户信息(提取排行榜所用信息)
	CT_VOID InsertTopUserInfo(LookUserInfo *LookData,CT_INT32 iCount,ENUM_TOP Type);


public: 
	//获取等级排名信息
	CT_VOID GetTopGradeInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取军衔排名信息
	CT_VOID GetTopRankingInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取战力排名信息
	CT_VOID GetTopFightingPowerInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取战队排名信息
	CT_VOID GetTopFightingTeamInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取连胜排名信息
	CT_VOID GetTopContinuousWinInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取战旗排名信息
	CT_VOID GetTopFightingFlagInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//获取掠夺资源排名信息
	CT_VOID GetTopPlunderResInfo(CT_CHAR* pBuf, CT_INT32& iLen);
	//获取用户信息
	CT_VOID GetTopUserInfo(const CT_DWORD dwUserID,LookUserInfo &UserInfo);
	//前50名等级平均
	CT_INT32 GetTopGradeAverage();

private:
	//转换用户信息
	CT_VOID ChangeUserInfo(const User_Fighting &FightingData,LookUserInfo &UserData);

public:
	//重置函数
	CT_VOID ResetVariable();
	//清理masterinfo
	CT_VOID ClearMasterInfo();

public:
	//
	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);
	//插入领主信息
	CT_VOID InsterMasterInfo(MaterInfo &mi);
	//排序
	CT_VOID SequenceTop();
	//
	MaterInfoMap &GetTopMasterInfo(){_AUTOMUTEX(&m_Mutex);return m_MasterInfoMap;};
	//查找排行榜用户
	CT_BOOL FindTopUser(CT_DWORD dwUserID);

public:
	CT_VOID SetInit(CT_BOOL bInit){m_bInit=bInit;};
	CT_BOOL GetInit(){return m_bInit;};
};
#endif