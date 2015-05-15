/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CSkill.h
Version:		1.0
Description:    ������
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
	//��ȡ��������
	CT_INT32 GetMpCost(CT_INT32 iSkillID);
	//����λ��
	CT_INT32 GetAttickPos(CT_INT32 iSkillID,CT_INT32 *pPosition,CT_INT32 iMePosition,FightingUserInfo *UserInfo);
};
#endif