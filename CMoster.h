/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CMoster.h
Version:		1.0
Description:    ������
Author:			wangchen
Create Date:	2012-9-13
History:             

******************************************************************/
#ifndef CCMOSTER_H_
#define CCMOSTER_H_

#include "CGDefine.h"

//�����������
struct MonsterBaseInfo
{
	CT_INT32		iID;					//����ID
	CT_INT32		iType;					//��������
	CT_INT32		iGrade;					//�ȼ�
	CT_INT32		iBaseHP;				//��������
	CT_INT32		iBaseMP;				//��������
	CT_INT32		iBasePower;				//��������
	CT_INT32		iBaseAgile;				//��������
	CT_INT32		iBaseIntelligence;		//��������
	CT_INT32		iBaseSprite;			//��������
	CT_INT32		iBaseAttack;			//����������
	CT_INT32		iBaseDefense;			//����������
	CT_INT32		iHitRate;				//����
	CT_INT32		iDodgeRate;				//����
	CT_INT32		iToughness;				//����
	CT_INT32		iCritRate;				//����
	CT_INT32		iCritCoefficient;		//����ϵ��
	CT_INT32		iSkill1;				//����1
	CT_INT32		iSkill2;				//����2
	CT_INT32		iSkill3;				//����3
	CT_INT32		iSkill4;				//����4
	CT_INT32		iPosition;				//λ��
};

class CCMoster
{
private:
	MonsterBaseInfo		m_MosterInfo;			//������Ϣ
public:
	CCMoster(void);
	~CCMoster(void);

public:
	//��ȡ������Ϣ
	MonsterBaseInfo GetMosterInfo(CT_INT32 iMosterID);
	//��ȡ����λ��
	CT_INT32   GetMosterPos(){return m_MosterInfo.iPosition;};
	//��ù�����Ϣ
	CT_BOOL	GetMosterData(MonsterBaseInfo *pMosterInfo);
};

#endif