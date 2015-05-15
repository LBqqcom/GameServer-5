/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserPVPInfo.h
Version:		1.0
Description:    �������û���
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCUSERPVPINFO_H_
#define CCUSERPVPINFO_H_
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserBase.h"
#include "CUserPet.h"
#include "CUserEquipment.h"
#include "CUserEnchant.h"
#include "PVPInfo.h"

//�û�ս������
struct User_Fighting
{
	MasterData				MasterInfo;								//������Ϣ
	CMD_G_ItemData			EquipedItemData[EQUIPED_ITEM_COUNT];		//װ����Ϣ
	CMD_G_EquipData			EquipmentInfo[EQUIPED_ITEM_COUNT];			//ǿ����Ϣ
	CMD_G_SkillData			iSkillID[EQUIPED_SKILL_COUNT];			//�û�����
	CT_INT32				iPosition[POST_COUNT/2];				//λ��	
	ArmyInfo				ArmyData[POST_COUNT/2];					//������Ϣ
	CT_INT32				iTalentNum;								//Ǳ����Ŀ

	PetInfo					pi;								//������Ϣ

    
    EnchantData enchantData[EQUIP_COUNTS * ENCHANT_ATTR_CNT];  // ��ħ����
    CT_INT32 iVIPLevel;  // VIP�ȼ�
    CT_CHAR cFightTeam[NAME_SIZE];   // ����ս��
};

// ս������ [8/26/2013 John]
struct UserFightingStatus
{
	CT_DWORD dwUserID;                      // ����ID
	CombatUnitData units[POST_COUNT / 2];   // ս������
};

//pvpս������
struct MSC_PVP_Fighting_Info
{
	CT_DWORD		dwUserID;							//�����û�ID
	CT_INT32		iSceneID;							//����ID
	CT_INT32		iSeed;								//�������
	CT_INT32		iDifficulty;						//�Ѷ�
	CT_INT32		iWiner;								//ʤ����

	UserFightingStatus FightingData[PVP_USER_COUNT];	//ս������
	LookUserInfo	LookData;							//�鿴�û�����
};

//pvp����
struct MSC_PVP_FightingReward
{
	CT_DWORD		dwUserID;
};


class CCUserPVPInfo
{
private:
	PVPInfo						m_PVPInfo;								//��������Ϣ
	PVPFightingInfo				m_PVPRecord[PVP_FIGHTING_COUNT];		//��������¼
	UserFightingStatus			m_ChallengeFight[PVP_CHALLENGE_COUNT];	//��ս�û�ս����Ϣ
	LookUserInfo				m_ChallengeLook[PVP_CHALLENGE_COUNT];	//��ս�û��鿴��Ϣ

	MSC_PVP_Fighting_Info		m_RecordFightData[PVP_FIGHTING_COUNT];			//ս��ս����Ϣ
	//LookUserInfo				m_RecordLookData[PVP_FIGHTING_COUNT];		//ս���û���Ϣ
	CT_INT32					m_iFightingCount;						//ս����¼��Ŀ

private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;


public:
	CCUserPVPInfo(void);
	~CCUserPVPInfo(void);

public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();

public:
	//���뾺������Ϣ
	CT_VOID InsertBufferData(const PVPInfo &PVPinfo);
	//�������״̬
	CT_VOID InsertChallengUser(const PVPInfo &PVPinfo);
	//����ս����¼
	CT_VOID InsertRecordInfo(CT_INT32 iIndex,const PVPFightingInfo &FightingInfo);
	//����ս����Ϣ
	CT_VOID	InsertRecordFighting(MSC_PVP_Fighting_Info &fi);
	//����߼���
	CT_VOID InsertHUserID(CT_INT32 iIndex,CT_DWORD dwUserID);
	//����ͼ���
	CT_VOID InsertLUserID(CT_INT32 iIndex,CT_DWORD dwUserID);

public:
	//��ȡ��������Ϣ
	CT_BOOL GetPVPInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ��������Ϣ
	CT_VOID GetPVPInfo(PVPInfo &PVPinfo,CT_BOOL bIsServer=CT_FALSE);
	//��ȡ��������¼
	//CT_BOOL GetPVPRectord(PVPFightingInfo PVPFightingRecord[PVP_FIGHTING_COUNT]);
	CT_BOOL GetPVPRectord(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ��ս����
	CT_INT32 GetFightingCount(){_AUTOMUTEX(m_pMutex); return m_PVPInfo.iFightingCount;};
	//��ö�����Ϣ
	CT_BOOL GetFightingUser(CT_INT32 iIndex,CT_DWORD dwUserID,UserFightingStatus &FightingUser);
	CT_BOOL GetFightingUser(CT_DWORD dwUserID,LookUserInfo &UserData);
	//��ö���ս����Ϣ
	CT_BOOL GetFightingRectordUser(CT_INT32 iIndex,CT_DWORD dwUserID,LookUserInfo &UserData);
	//��ȡ�Ѷ�
	CT_INT32 GetDifficulty(CT_DWORD dwUserID);
	//�����ȴʱ��
	CT_BOOL GetCoolTime(CT_INT64 lTime);
	//�����ս״��(��)
	CT_VOID GetChallengeInfo(CT_INT32 iDifficulty,CT_BOOL bWiner[PVP_CHALLENGE_COUNT/2]);
	//�����սID(��)
	CT_VOID GetChallengeID(CT_INT32 iDifficulty,CT_DWORD dwUserID[PVP_CHALLENGE_COUNT/2]);
	//��ȡս��
	CT_BOOL GetFightingInfo(CT_INT32 iIndex,MSC_PVP_Fighting_Info &FightingInfo);
	//��ö���״��
	CT_BOOL GetChallengeStatce(CT_INT32 iIndex);

public:
	//������ȴʱ��
	CT_VOID SetBeginCoolTime(CT_INT64 lTime); 
	//������ȴʱ��
	CT_VOID SetAddCoolTime(CT_INT64 lTime){_AUTOMUTEX(m_pMutex); m_PVPInfo.lRefreshTime+=lTime;};
	//������ս����
	CT_VOID SetFightingCount(){m_PVPInfo.iFightingCount++;};
	//���ö���
	CT_VOID SetChallengInfo(CT_INT32 iIndex,const UserFightingStatus &FightData,LookUserInfo &LookData);
	//���ö���ս����Ϣ
	CT_BOOL SetFightingRectordUser(const User_Fighting &FightingUser);
	//���þ�����
	CT_INT32 SetPVPDegree(const CT_INT32 iDegree,CT_BOOL bSend=CT_TRUE);
	//����ս��
	CT_VOID SaveFightingReacord();

public:
	//���¾�����������Ϣ
	CT_VOID UpdataChallengUser(CT_DWORD dwUserID,CT_INT32 iDifficulty,CT_BYTE cbStatus);
	//���¾�������¼
	CT_VOID	UpdataPVPRectord(const PVPFightingInfo &PVPFightingRecord);
	//����ս������
	CT_VOID UpdataFighting(CT_CHAR *pBuf,CT_INT32 iLen);
	//������ȴʱ��(�����˳�����)
	CT_INT64 UpdataPVPCoolTime();
	//�����콱����
	CT_BOOL UpDataChestNum(CT_BOOL bReset=CT_FALSE);
	//12����¾�����
	CT_VOID UpdataPVPData(CT_CHAR *pBuf,CT_INT32 iLen);
};

#endif
