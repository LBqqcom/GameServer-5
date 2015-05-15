/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserFightTeamNember.h
Version:		1.0
Description:    用户战队数据
Author:			wangchen
Create Date:	2012-10-19
History:        战队    

******************************************************************/
#ifndef CCUSERFIGHTTRAMNEMBER_H_
#define CCUSERFIGHTTRAMNEMBER_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserData.h"

//成员科技
struct NemberScience
{
	CT_DWORD	dwUserID;						//用户ID
	CT_INT32	iScienceID;						//科技ID
	CT_INT32	iScienceGrade;					//科技等级
};

typedef std::map<CT_INT32,NemberScience>	ScienceInfoMap;		//科技信息

class CCUser;
struct MasterCombatStatus;
class CCUserFightTeamNember:public CCUserData<CT_INT32, NemberScience>
{
public:
	CCUserFightTeamNember(void);
	~CCUserFightTeamNember(void);

protected:
	virtual CT_INT32 transData(const NemberScience &src, CT_CHAR *pDst) const;
public:
	virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE){};

public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

public:
	//插入成员科技
	CT_VOID	InsertMemberScience(NemberScience & Ftns);	
	//插入离队时间
	CT_VOID InsertLeaveTeamTime(CT_INT64 lTime);

public:
	//获取成员科技
	CT_BOOL	GetNemberScience(CT_CHAR *pBuf,CT_INT32 &iLen);	
	//获取科技信息
	CT_BOOL GetScienceInfo(CT_INT32 iScienceID,NemberScience &ScienceInfo);
	//获取加成效果
	CT_VOID GetAddAffect(CT_DWORD dwFTID,CT_INT32 iOccupation,MasterCombatStatus &mcs);

private:
	//类型加成
	CT_VOID AddFightingType(CT_INT32 iType,MasterCombatStatus &mcs,G_FightTeamBless &ftb);

public:
	//科技升级
	CT_INT32 ScienceUpGrade(CT_INT32 iScienceID,CT_INT32 iItemCount);
	//科技降级
	CT_VOID ScienceDownGrade();

public:
	//科技建筑加成效果
	CT_INT32 ScienceBulidAdd(const CT_DWORD dwTeamID,const CT_INT32 iProduct,const CT_INT32 iBuildType);
    //战斗力加成
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs,CT_DWORD dwTeamID, CT_DWORD dwUserID,const CT_INT32 arrayTargets[], const CT_INT32 nTarget, const ScienceInfoMap &scienceMap); 
};

#endif
