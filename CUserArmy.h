/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    用户军队数据
Author:			wangchen
Create Date:	2012-10-19
History:        用户军队    

******************************************************************/
#ifndef CCUSERARMY_H_
#define CCUSERARMY_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserBase.h"
#include "CUserData.h"

#define ARMY_ID2TYPE(id) (id / 10)

class CCUser;

class CCUserArmy: public CCUserData<CT_INT32, ArmyInfo>
{
public:
	typedef		std::map<CT_INT32,ArmyInfo>				AtmyInfoMap;
	typedef		std::map<CT_INT32,ArmyInfo>::iterator	AI_IT;

public:
	CCUserArmy(void);
	~CCUserArmy(void);

protected:
	virtual CT_INT32 transData(const ArmyInfo &src, CT_CHAR *pDst) const;
public:
	virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE){};

public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

	//插入数据
public:
	//插入军队信息
	CT_VOID InsertBufferData(CT_INT32 iArmyID,CT_INT32 iArmyGrade);

	//获得信息
public:
	//获得军队信息
	CT_BOOL GetArmyInfo(CT_INT32 iArmyID,ArmyInfo &ArmyGrade);
	//获得军队信息
	CT_BOOL GetArmyInfo(CT_CHAR *pBuf,CT_INT32 &iLen);	
    // 获得军队技能
    CT_VOID GetEquipedSkill(CMD_G_SkillData *pBuf, CT_INT32 &iLen, const CT_INT32 iArmyID, const CT_INT32 iArmyGrade);
public:
	//更新升级信息
	CT_BOOL UpDataArmyData(CT_INT32 iArmyID);
	//更新进阶信息
	CT_BOOL UpDataArmyAdvanced(const CT_INT32 iArmyID,CT_INT32 iAdcancedSoldier);
	//开启更新
	CT_VOID UpDataOpenArmy(CT_INT32 iGrade);
	//发送军队信息
	CT_VOID SendArmyInfo(CT_DWORD dwMain=MAIN_G_MASTER_EVENT_CITY);
	//删除上阵boss
	CT_VOID DeleteBoss(CT_BOOL bSeek=CT_FALSE);
	//判断是否拥有某级别的军队
	CT_INT32 OwnLevelArm(CT_INT32 iLevel);
	//判断是否拥有某个id军队
	CT_BOOL OwnIDArm(CT_INT32 iArmID);
	//判断是否拥有同类进阶后的军队	zhoujun
	CT_BOOL OwnArmyAdvanced(CT_INT32 iArmID);
	//判断是否拥有此阶或更高阶的军队	阶位,兵种类型	zhoujun
	CT_BOOL OwnArmyAdvanNum(CT_INT32 iAdvanNum, CT_INT32 iArmType = 0);
    //是否拥有某种类型的兵种 (兵种类型=兵种ID/10)
    CT_BOOL OwnArmyOfType(CT_INT32 armyType) const;
public:
     //更新战斗信息
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iArmyID, const CT_INT32 iArmyGrade);
};

#endif
