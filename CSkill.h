/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CSkill.h
Version:		1.0
Description:    技能类
Author:			wangchen
Create Date:	2012-9-13
History:             

******************************************************************/
#ifndef CCSKILL_H_
#define CCSKILL_H_

#include "CSingleton.h"
#include "CTType.h"

struct FightingUserInfo;
class CCSkill:public CCSingleton<CCSkill>
{
public:
	CCSkill(void);
	~CCSkill(void);

public:
	//
	CT_BOOL GetSkillMain();
	//获取法力消耗
	CT_INT32 GetMpCost(CT_INT32 iSkillID);
	//攻击位置
	CT_INT32 GetAttickPos(CT_INT32 iSkillID,CT_INT32 *pPosition,CT_INT32 iMePosition,FightingUserInfo *UserInfo);
};
#endif