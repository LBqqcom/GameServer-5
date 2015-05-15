/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserSkill.h
Version:		1.0
Description:    �û���������
Author:			pengwen
Create Date:	2012-9-29
History:        �û��������ݣ��������ܣ�Ǳ��     

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
// ������Ϣ
struct SkillData
{
	CT_INT32 iID;                   // ����ID
	CT_CHAR  szName[NAME_SIZE];     // ��������
    CT_INT32 iSkillSoul;            // ��������
	CT_INT32 Used;                  // �Ƿ�װ��    
};

class CCUserSkill : public CCUserData<CT_INT32, SkillData>
{
protected:
    virtual CT_INT32 transData(const SkillData &src, CT_CHAR *pDst) const; 
public:
	// �õ�װ���ļ����б�
	CT_BOOL GetEquipedSkillData(CT_CHAR *pBuf, CT_INT32 &bufLen) const;
    // װ������
    CT_INT32 EquipSkill(const CT_INT32 skillIDs[]);
    // ѧ�Ἴ��
    CT_INT32 LearnSkill(const CMD_G_LearnSkill &learn);
};


#endif