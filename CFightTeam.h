/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightTeam.h 
Version:		1.0
Description:    战队数据
Author:			zhoujun
Create Date:	2013-05-03
History:        战队数据     

******************************************************************/
#ifndef CCFIGHTTEAM_H_
#define CCFIGHTTEAM_H_
#include <map>
#include "CUser.h"
#include "CGDefine.h"
#include "CMutex.h"
#include "CThreadMes.h"
#include "CResourceGrabMgr.h"
#include <time.h>

//威能
struct CSInspore
{
	CT_DWORD	dwUserID;
	CT_INT32	iGrade;
	CT_INT32	iPowerID;
	CT_INT32	iExp;
};

struct FightTeamBase	//如果要增加大于4字节类型的字段,请将下面的 pragma pack(push, 4) 移到之行之上,另外不要忘记DB类
{
	CT_DWORD	dwFTID;							// 战队ID
	CT_CHAR		cFightTeamName[NAME_SIZE];		// 战队名称
	CT_CHAR		cDescription[DESCRIPTION_SIZE]; // 战队介绍
	CT_DWORD	dwBossUID;						// 首领ID
	CT_CHAR		cBossName[NAME_SIZE];			// 首领名称
	CT_INT32	iFightingPower;					// 总战斗力
	CT_INT32	iTop;							// 排名
	CT_INT32	iPreTop;						// 上次排名(排名动态)
	CT_INT32	iResource;						// 资源数
	CT_INT32	iIcon;							// 
	CT_DWORD	dwElite[CLAN_CRUSADE_MEMBER];	// 出战精英
	CT_INT32	iResCount;						//资源点数

	CT_INT32	iGrade;							//等级
	CT_INT32	iItemCount;						//道具资源数
	CT_INT32	iSummonCount;					//召唤次数
	CT_INT32	iBossMaxIndex;					//最大BOSS索引
	CT_INT32	iTimeBossMaxIndex;				//时限boss最大索引
};

#  pragma pack(push, 4)  
struct FightTeamMember
{
	CT_DWORD	dwFTID;							// 战队ID
	CT_DWORD	dwUserID;						// 队员ID
	CT_CHAR		cNickName[NAME_SIZE];			// 队员名称
	CT_INT32	iFightingPower;					// 队员战斗力
	CT_INT32	iOccupation;					// 职业
	CT_INT32	iGrade;							// 等级
	CT_INT32	iRank;							// 军衔
	CT_INT32	iContribution;					// 贡献
	CT_INT32	iStatus;						// 状态 0.空闲,不在线 1.驻守,不在线 2.空闲,在线 3驻守,在线
	CT_INT64	iTime;							// 加入时间/申请时间/邀请时间
	CT_INT32	iTowerCPID;						// 元素塔最大通关层数
	CT_INT32	iContributeCount[CONTRIBUTE_TYPE];//捐献次数
	CT_INT32	iContributeGross;					//捐献总量
	CT_INT64	iLastExitTime;
	CT_INT32	iTitleID;						//头衔ID
};
#  pragma pack(pop)  

//战队BOSS
struct FightTeamSummonBoss
{
	CT_DWORD			dwFTID;							//战队ID
	CT_DWORD			dwUserID[SUMMON_MAX];			//用户id
	CT_INT32			iBossIndex;						//BOSS索引
	CT_DWORD			dwChestIndex[SUMMON_MAX*2];		//宝箱索引
	CT_INT32			iWiner;							//召唤战斗胜利
	CT_BOOL				bFightingStatus;				//战斗状态
	CMD_G_NormalReward	Res[SUMMON_MAX];				//资源
};

//道具奖励信息
typedef CMD_G_FTTBRewardInfo FightTeamSummonItem ;
//struct FightTeamSummonItem
//{
//	CT_DWORD			dwUserID;						//用户id
//	CT_INT32			iChestIndex;					//宝箱索引
//	CT_INT32			iItemID;						//道具ID
//	CT_INT32			iCount;							//道具数量
//};

//时限BOSS信息
struct FTTimeBossInfo
{
	CT_INT32			iIndex;							//索引
	CT_INT32			iSummonBeginTime;					//召唤开始时间
	CT_INT32			iHelpUser[HELP_SUMMON_COUNT];			//助阵用户
	CT_INT32			iCurrentHP;						//当前hp
};

//时限BOSS成员信息
struct FTTimeBossMember
{
	CT_DWORD			dwUserID;
	CT_INT32			iHurt;								//伤害
	CT_INT32			iFTFood;							//战队粮食
	CT_INT32			iFTContribute;						//战队贡献
	CT_INT32			iFightNum;							//战斗次数
};

typedef std::map<CT_DWORD, FightTeamMember>		FTMember;				//成员()
typedef std::map<CT_DWORD, UserFightingStatus>	FTMemberFightingData;	//成员战斗力数据
typedef std::multimap<CT_INT64, CMD_G_FightTeamEventLog>	FTEventData;		//战队动态
typedef std::map<CT_INT32, FightTeamFightData>	FTFightData;			//战斗数据
typedef std::multimap<CT_DWORD,FightTeamSummonItem>	FTSummonRewardMap;		//奖励key:userid
typedef std::map<CT_INT32,CSInspore>		CSInsporeMap;			//威能信息
typedef std::map<CT_DWORD,CSInsporeMap>		CSUserInsporeMap;		//个人威能信息
typedef std::map<CT_DWORD,FTTimeBossMember> TBMemberMap;			//时限boss成员信息



//战队数据
class CCFightTeam 
{	
public:
	enum FIGHT_MEMBER_TYPE
	{
		FIGHTTEAM_UNKNOW		=	(0),		//未知
		FIGHTTEAM_BOSS			=	(1),		//队长
		FIGHTTEAM_ELITE			=	(2),		//精英
		FIGHTTEAM_MEMBER		=	(3),		//普通成员
	};

	enum FIGHT_CONTRIBUTE_TYPE
	{
		FIGHTTEAM_FOOD			=	(0),		//食物捐献
		FIGHTTEAM_DIAMOD		=	(1),		//钻石捐献
		FIGHTTEAM_ITEM			=	(2),		//道具捐献
	};

	friend class CCFightTeamManager;
	friend class CCFightTeamEvent;
private:
	CCMutex							m_mutex;
	CT_INT64						m_iLastActTime;
	CT_INT64						m_iLastUpdTime;
	CT_INT32						m_iResCount;

	FightTeamBase					m_stBase;				// 战队主信息
	FTMember						m_FTMemberMap;			// 战队成员信息
	//FTMemberFightingData			m_FTMemberFightingData;	// 成员战斗力信息
	FTMember						m_FTMemberReqMap;		// 编外: 申请待审核
	FTMember						m_FTMemberRspMap;		// 编外: 邀请待答复
	FTSummonRewardMap				m_FTSummonRewardMap;	// 召唤奖励
	FightTeamSummonBoss				m_FTSummonInfo;			//召唤信息
	CSUserInsporeMap				m_FTCSInspore;			//个人威能
	FTTimeBossInfo					m_FTTimeBossInfo;		//时限boss信息
	TBMemberMap						m_TBMemberMap;			//时限boss成员信息
	FTSummonRewardMap				m_TBRewardMap;			//时限boss奖励

	FTEventData						m_FTEventMap;			// 战队动态
	FTFightData						m_FTFightMap;			// 战队战斗

public:
	//初始化
	CT_VOID		Init();
	CT_VOID		Clear();

public:
	//设置战队id 
	CT_VOID		SetFightTeamID(CT_DWORD id) {_AUTOMUTEX(&m_mutex);   m_stBase.dwFTID = id;}

	CT_DWORD	GetFightTeamID() {_AUTOMUTEX(&m_mutex);  return m_stBase.dwFTID; }
	CT_VOID		GetFightTeamName(CT_CHAR* pName) {_AUTOMUTEX(&m_mutex); memcpy(pName, m_stBase.cFightTeamName, NAME_SIZE);}
	CT_VOID		SetFTName(CT_CHAR *pName){_AUTOMUTEX(&m_mutex); memcpy(m_stBase.cFightTeamName, pName, NAME_SIZE-1);}

	CT_DWORD	GetFightTeamBossID() {_AUTOMUTEX(&m_mutex);  return m_stBase.dwBossUID; }
	CT_VOID		GetFightTeamBossName(CT_CHAR* pName) {_AUTOMUTEX(&m_mutex);  memcpy(pName, m_stBase.cBossName, NAME_SIZE); }
	CT_VOID		SetFightTeamBossName(CT_CHAR* pName) {_AUTOMUTEX(&m_mutex);  memcpy(m_stBase.cBossName,pName,  NAME_SIZE); }

	CT_VOID		SetFightTeam(const CMD_G_FightTeamBase &data)					// 设置战队主信息
	{
		_AUTOMUTEX(&m_mutex); memcpy(&m_stBase, &data, sizeof(FightTeamBase));
	}

	CT_VOID		InitInsertMember(CT_INT32 iType, const CMD_G_FightTeamMember &data);	// 初始化插入战队成员
	CT_INT32	InsertMember(CT_INT32 iType, const CMD_G_FightTeamMember &data);		// 插入战队成员
	CT_VOID		InsertMemberFightingData(MSC_DB_FTMemberFightingData &dbFTMemberFightingData);	//插入成员战斗力
	CT_VOID		InsertSummonInfo(FightTeamSummonBoss &FTSB);
	CT_VOID		InsertSummonReward(FightTeamSummonItem &FTSM);
	CT_VOID		InsertInspore(CSInspore &csi);											//插入威能
	CT_VOID		InsertTimeBoss(FTTimeBossInfo &tbi);									//插入时限boss
	CT_VOID		InsertTimeBossMember(FTTimeBossMember &bm);								//插入时限boss成员信息
	CT_VOID		InsertTimeBossRewardr(FightTeamSummonItem &si,CT_BOOL bSave=CT_TRUE);							//插入时限boss奖励

	CT_INT32	DeleteMember(CT_INT32 iType, CT_DWORD dwMUID);								// 删除战队成员

	CT_BOOL		GetFightTeamBase(CMD_G_FightTeamBase *pFightTeamBase);
	CT_BOOL		GetFightTeamInfo(CMD_G_FightTeamInfo *pFightTeamInfo);

	CT_BOOL		GetFightTeamMember(CT_CHAR *pBuf, CT_DWORD &bufLen);
	CT_BOOL		GetFightTeamReqMember(CT_CHAR *pBuf, CT_DWORD &bufLen);

	CT_BOOL		GetFightTeamMember(CT_INT32 iType, CT_DWORD dwUser, CMD_G_FightTeamMember* pFightTeamMember);
	//获得威能(目前只有一个，先做单一处理)
	CT_BOOL		GetInspore(CT_DWORD dwUserID,CSInspore &csi);
	//威能升级
	CT_VOID		UpGradeInspore(CT_DWORD dwUserID,CSInspore &csi);

	CT_BOOL		CheckFightTeamFull();	// 检查战队是否已经满员

	CT_VOID		ClearReqRsp(CT_DWORD dwUser);		// 当某用户已加入某战队后, 清除掉他在其他战队的邀请和申请

	CT_INT32	GetResource() {_AUTOMUTEX(&m_mutex);  return m_stBase.iResource; }

	CT_VOID		AddResource(CT_INT32 iResource) {_AUTOMUTEX(&m_mutex);  m_stBase.iResource += iResource; }

	CT_INT32	Contribute(const CMD_G_FightTeam_Donate &Donate);	// 队员捐献
	CT_VOID		Contribute(const CT_DWORD dwUserID,const CT_INT32 iDonate);	// 队员捐献

	CT_VOID		UpdateData(CT_INT64 lTime);	//定时保存数据

	CT_INT32	GetEliteIndex(CT_DWORD dwUser);		// 如果是精英,则返回精英序号, 否则 -1

	CT_VOID		GetMemberName(CT_DWORD dwMemberID, CT_CHAR* pName);

	CT_VOID		InitEventLog(const CMD_G_FightTeamEventLog &fightTeamEventLog);	//初始化插入战队动态
	CT_VOID		AddEventLog(const CMD_G_FightTeamEventLog &fightTeamEventLog);	//增加战队动态
	CT_VOID		GetEventLog(CT_CHAR *pBuf, CT_INT32 &bufLen);					//获取战队动态
	CT_INT32	ADDFightData(const FightTeamFightData &ftFightData);			//插入战报,返回事件ID

	CT_VOID		Broadcast(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_DWORD dwExcep = 0);	//群广播
	CT_VOID		FightTeamChat(CMD_G_FightTeamChat *pFightTeamChat, CT_DWORD len);	//战队聊天

	CT_VOID		GetFightTeamMemberUID(FightTeamMemberUID *ftMemberUID);		// 获取战队成员UID

	CT_BOOL		GetUserFightData(UserFightingStatus *userFightingData, CT_INT32 iCount,CT_BOOL bFight=CT_FALSE);	// 获取精英战斗数据

	CT_BOOL		GetUserFightData(CT_DWORD dwUserID, UserFightingStatus &rightFightingData,CT_BOOL bFight=CT_FALSE);	// 获取指定成员战斗数据

	CT_VOID		UserExitEvent(CCUser *pUser);		// 用户下线,更新战队成员数据
	CT_VOID		UserLoginEvent(CT_DWORD dwUser);	// 用户上线, 推送成员在线状态

	CT_VOID		ResReward(CT_INT32 iResource);		// 分资源奖励

	FIGHT_MEMBER_TYPE	GetMemberPosition(CT_DWORD dwUserID);	// 获取队员职位 1.队长 2.精英 3.普通成员 0.未找到成员

	CT_BOOL		CheckResCountGrade(CT_INT32 iResCount);		// 检查占领第N个资源点所需要的队长等级是否满足

	CT_VOID		SetUsersGuard(const GuardTag &gdPrimary,const GuardTag &gd); // 驻守人员变更
	CT_VOID		SendMemberGuard(const CT_DWORD dwUserID);

	CT_VOID		MemberRef(const CT_DWORD dwUser, const CT_INT32 iGrade, const CT_INT32 iFightingPower);

	CT_VOID		SetResCount(const CT_INT32 iResCount) {_AUTOMUTEX(&m_mutex);  m_stBase.iResCount = iResCount; }

	CT_VOID		FightTeamUpGrade();//战队升级
	CT_VOID		UpdataFightTeamInfo(CT_DWORD dwUserID=0);	//更新战队信息
	CT_VOID		CostContribute(CT_DWORD dwUserID,CT_INT32 iContribute);//队员贡献消耗
	CT_VOID		SetSummonInfo(FightTeamSummonBoss &FTSB);		//设置召唤信息
	CT_INT32	GetSummonCount();				//获取召唤人数
	CT_BOOL		GetSummonInfo(FightTeamSummonBoss &FTSB);		//获取召唤信息
	CT_INT32	HelpSummonLeave(CT_DWORD dwUserID,FightTeamSummonBoss &FTSB);//助阵离开
	CT_VOID     SendSummonMessage(CT_DWORD dwUser[SUMMON_MAX],CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);//给召唤人员发送信息
	CT_VOID		SetSummonReward(CT_INT32 iUserIndex,CT_INT32 iChestIndex,CMD_G_NormalReward &Reward,CT_INT32 iItem[GOODS_COUNT],CT_INT32 iItemCount[GOODS_COUNT]);//设置召唤奖励
	CT_BOOL		GetSummonRewardItem(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &len);	//获取战队召唤奖励
	CT_BOOL		GetSummonRewardRes(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &len);	//获取战队召唤奖励
	CT_VOID		SetSummonWin(CT_INT32 iBossIndex);				//设置召唤胜利
	CT_VOID     ClearSummonData();								//清理召唤数据
	CT_VOID		SetSummonFightingStatus(CT_BOOL bFighting){m_FTSummonInfo.bFightingStatus=bFighting;};
	CT_BOOL		GetSummonFightingStatus(){return m_FTSummonInfo.bFightingStatus;};
	CT_VOID		GetTimeBossInfo(FTTimeBossInfo &tbi){_AUTOMUTEX(&m_mutex);memcpy(&tbi,&m_FTTimeBossInfo,sizeof(m_FTTimeBossInfo));};
	CT_VOID		SetTimeBossInfo(FTTimeBossInfo &tbi);
	CT_VOID		UpdataTimeBossMember(FTTimeBossMember &bm);								//更新时限boss成员信息
	CT_BOOL		GetTimeBossMember(CT_DWORD dwUserID,FTTimeBossMember &bm);				//获取时限boss成员信息
	CT_VOID		DeleteTBMember(CT_DWORD dwUserID);										//删除时限boss成员信息
	CT_VOID		ClearTBMemberFightCount();						//清楚时限BOSS战斗次数
	CT_BOOL		GetTBReward(CT_INT32 iIndex,FightTeamSummonItem &si);	//时限boss奖励
	CT_BOOL		GetTBReward(CT_CHAR *pBuf,CT_INT32 &iLen);	//时限boss奖励
	CT_INT32	GetTBCurrentHP();							//时限BOSS当前血量
	CT_INT32    GetTBMaxHP();								//时限BOSS最大血量
	CT_VOID		SetTBIndex(CT_INT32 iIndex);				//设置时限boss最大索引
	CT_VOID		ClearTBInfo();								//清理时限boss信息

	CT_VOID		SetMemberTowerCP(CT_DWORD dwUID, CT_INT32 iMaxCPID);	//更新通关数据
	CT_INT32	GetTowerPassCount(CT_INT32 iCPID);			//获取战队内已通关人数

public:
	CCFightTeam();
	~CCFightTeam();

	CCFightTeam(const CCFightTeam &fightTeam) 
	{
		m_iLastActTime = fightTeam.m_iLastActTime;
		m_iLastUpdTime = fightTeam.m_iLastUpdTime;

		m_stBase = fightTeam.m_stBase;
		m_FTMemberMap = fightTeam.m_FTMemberMap;
		m_FTMemberReqMap = fightTeam.m_FTMemberReqMap;
		m_FTMemberRspMap = fightTeam.m_FTMemberRspMap;
		m_TBMemberMap=fightTeam.m_TBMemberMap;
		m_TBRewardMap=fightTeam.m_TBRewardMap;
		memcpy(&m_FTSummonInfo, &fightTeam.m_FTSummonInfo, sizeof(m_FTSummonInfo));
		memcpy(&m_FTTimeBossInfo,&fightTeam.m_FTTimeBossInfo,sizeof(m_FTTimeBossInfo));
	}
	CCFightTeam operator = (const CCFightTeam& fightTeam) 
	{
		m_iLastActTime = fightTeam.m_iLastActTime;
		m_iLastUpdTime = fightTeam.m_iLastUpdTime;

		m_stBase = fightTeam.m_stBase;	
		m_FTMemberMap = fightTeam.m_FTMemberMap;
		m_FTMemberReqMap = fightTeam.m_FTMemberReqMap;
		m_FTMemberRspMap = fightTeam.m_FTMemberRspMap;
		m_TBMemberMap=fightTeam.m_TBMemberMap;
		m_TBRewardMap=fightTeam.m_TBRewardMap;
		memcpy(&m_FTSummonInfo, &fightTeam.m_FTSummonInfo, sizeof(m_FTSummonInfo));
		memcpy(&m_FTTimeBossInfo,&fightTeam.m_FTTimeBossInfo,sizeof(m_FTTimeBossInfo));
	}

private:
	CT_VOID ComputeFightingPower();		//更新战斗力
	CT_VOID ReCheckMember();
}; 

#endif
