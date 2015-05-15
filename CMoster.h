/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CMoster.h
Version:		1.0
Description:    怪物类
Author:			wangchen
Create Date:	2012-9-13
History:             

******************************************************************/
#ifndef CCMOSTER_H_
#define CCMOSTER_H_

#include "CGDefine.h"

//怪物基础属性
struct MonsterBaseInfo
{
	CT_INT32		iID;					//怪物ID
	CT_INT32		iType;					//怪物类型
	CT_INT32		iGrade;					//等级
	CT_INT32		iBaseHP;				//基础生命
	CT_INT32		iBaseMP;				//基础法力
	CT_INT32		iBasePower;				//基础力量
	CT_INT32		iBaseAgile;				//基础敏捷
	CT_INT32		iBaseIntelligence;		//基础智力
	CT_INT32		iBaseSprite;			//基础精神
	CT_INT32		iBaseAttack;			//基础攻击力
	CT_INT32		iBaseDefense;			//基础防御力
	CT_INT32		iHitRate;				//命中
	CT_INT32		iDodgeRate;				//闪避
	CT_INT32		iToughness;				//韧性
	CT_INT32		iCritRate;				//暴击
	CT_INT32		iCritCoefficient;		//暴击系数
	CT_INT32		iSkill1;				//技能1
	CT_INT32		iSkill2;				//技能2
	CT_INT32		iSkill3;				//技能3
	CT_INT32		iSkill4;				//技能4
	CT_INT32		iPosition;				//位置
};

class CCMoster
{
private:
	MonsterBaseInfo		m_MosterInfo;			//怪物信息
public:
	CCMoster(void);
	~CCMoster(void);

public:
	//获取怪物信息
	MonsterBaseInfo GetMosterInfo(CT_INT32 iMosterID);
	//获取怪物位置
	CT_INT32   GetMosterPos(){return m_MosterInfo.iPosition;};
	//获得怪物信息
	CT_BOOL	GetMosterData(MonsterBaseInfo *pMosterInfo);
};

#endif