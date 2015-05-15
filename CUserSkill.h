/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserSkill.h
Version:		1.0
Description:    用户技能数据
Author:			pengwen
Create Date:	2012-9-29
History:        用户技能数据，包括技能，潜能     

******************************************************************/
#ifndef CCUSERSKILL_H_
#define CCUSERSKILL_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CUserData.h"
#include "CUserBase.h"
#include "GlobeScriptInterface.h"

// Begin [9/17/2012 John]
// 技能信息
struct SkillData
{
	CT_INT32 iID;                   // 技能ID
	CT_CHAR  szName[NAME_SIZE];     // 技能名称
    CT_INT32 iSkillSoul;            // 技魂数量
	CT_INT32 Used;                  // 是否装备    
};

class CCUserSkill : public CCUserData<CT_INT32, SkillData>
{
protected:
    virtual CT_INT32 transData(const SkillData &src, CT_CHAR *pDst) const; 
public:
	// 得到装备的技能列表
	CT_BOOL GetEquipedSkillData(CT_CHAR *pBuf, CT_INT32 &bufLen) const;
    // 装备技能
    CT_INT32 EquipSkill(const CT_INT32 skillIDs[]);
    // 学会技能
    CT_INT32 LearnSkill(const CMD_G_LearnSkill &learn);
};


#endif