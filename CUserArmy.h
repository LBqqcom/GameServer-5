/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    �û���������
Author:			wangchen
Create Date:	2012-10-19
History:        �û�����    

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
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();

	//��������
public:
	//���������Ϣ
	CT_VOID InsertBufferData(CT_INT32 iArmyID,CT_INT32 iArmyGrade);

	//�����Ϣ
public:
	//��þ�����Ϣ
	CT_BOOL GetArmyInfo(CT_INT32 iArmyID,ArmyInfo &ArmyGrade);
	//��þ�����Ϣ
	CT_BOOL GetArmyInfo(CT_CHAR *pBuf,CT_INT32 &iLen);	
    // ��þ��Ӽ���
    CT_VOID GetEquipedSkill(CMD_G_SkillData *pBuf, CT_INT32 &iLen, const CT_INT32 iArmyID, const CT_INT32 iArmyGrade);
public:
	//����������Ϣ
	CT_BOOL UpDataArmyData(CT_INT32 iArmyID);
	//���½�����Ϣ
	CT_BOOL UpDataArmyAdvanced(const CT_INT32 iArmyID,CT_INT32 iAdcancedSoldier);
	//��������
	CT_VOID UpDataOpenArmy(CT_INT32 iGrade);
	//���;�����Ϣ
	CT_VOID SendArmyInfo(CT_DWORD dwMain=MAIN_G_MASTER_EVENT_CITY);
	//ɾ������boss
	CT_VOID DeleteBoss(CT_BOOL bSeek=CT_FALSE);
	//�ж��Ƿ�ӵ��ĳ����ľ���
	CT_INT32 OwnLevelArm(CT_INT32 iLevel);
	//�ж��Ƿ�ӵ��ĳ��id����
	CT_BOOL OwnIDArm(CT_INT32 iArmID);
	//�ж��Ƿ�ӵ��ͬ����׺�ľ���	zhoujun
	CT_BOOL OwnArmyAdvanced(CT_INT32 iArmID);
	//�ж��Ƿ�ӵ�д˽׻���߽׵ľ���	��λ,��������	zhoujun
	CT_BOOL OwnArmyAdvanNum(CT_INT32 iAdvanNum, CT_INT32 iArmType = 0);
    //�Ƿ�ӵ��ĳ�����͵ı��� (��������=����ID/10)
    CT_BOOL OwnArmyOfType(CT_INT32 armyType) const;
public:
     //����ս����Ϣ
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iArmyID, const CT_INT32 iArmyGrade);
};

#endif
