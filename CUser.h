/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUser.h
Version:		1.0
Description:    �û�����
Author:			pengwen
Create Date:	2012-8-22
History:        �û�����     

******************************************************************/
#ifndef CCUSER_H_
#define CCUSER_H_
#include <map>
#include <set>
#include "CGDefine.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CMutex.h"
#include "CUserBase.h"
#include "CUserBuilding.h"
#include "CUserItem.h"
#include "CUserEvent.h"
#include "CUserSkill.h"
#include "CUserTalent.h"
#include "CUserBuff.h"
#include "CUserEquipment.h"
#include "CRegion.h"
#include "CUserDuplicate.h"
#include "CUserArmy.h"
#include "CUserBattleArray.h"
#include "CTask.h"
#include "CUserDaily.h"
#include "CUserBlackMarket.h"
#include "CUserPet.h"
#include "CUserPVPInfo.h"
#include "CUserActive.h"
#include "CUserWorship.h"
#include "CVip.h"
#include "CUserFriend.h"
#include "CUserTeamDuplicate.h"
#include "CUserTower.h"
#include "CUserBattleFlag.h"
#include "CUserElmAttr.h"
#include "CUserBoss.h"
#include "CUserFightTeamNember.h"
#include "CUserTitle.h"
#include "CDragonIsland.h"
#include "CUserSeek.h"
#include <time.h>

class CCNetInterface;
struct MSC_DB_MasterSaveData;
struct MSC_DB_InsertWallArmy;
struct PTRLinkUserData
{
    MasterData *pMd;
    CT_INT32 *pArray;
    CT_INT32 *pVipGrade;
    CCUserTalent::DataMap *pTalentMap;
    CCUserSkill::DataMap *pSkillMap;
    CCUserItem::DataMap *pItemMap;
    CCUserEquipment::DataMap *pEquipMap;
    CCUserBuff::DataMap *pBuffMap;
    CCUserEnchant::DataMap *pEnchantMap;
    CCUserBattleFlag::DataMap *pFlagMap;
    CCUserItem::DataMap *pElmMap;
    CCUserElmAttr::DataMap *pElmAttrMap;
    CCUserArmy::DataMap *pArmyMap;
	CCUserTower::AwardStatusMap *pTowerMap;
    CCUserFightTeamNember::DataMap *pScienceMap;
	CCUserEnchant::EnchantRewardVector *pEnchatRewardVector;
	PetInfo	*pPetInfo;
	CCUserTitle::DataMap *pTitleMap;
	CT_INT32 *pSeekArray;
	WallAdditionMap *pWallAdditionMap;
	CCUserBase::MasterFightAddMap *pMasterFightAddMap;
	CCUserPet::PetChangeMap *pcmap;
};

struct FightingPower
{
	CT_INT32 iAllFightingPower;
	CT_INT32 iMasterFightingPower;
	CT_INT32 iArmyFightingPower;
	CT_INT32 iFightingFlagPower;
};

enum ENUM_LootItem {
    LOOT_SUC,
    LOOT_BAG_FULL,
    LOOT_ELM_FULL,
    LOOT_NO_ITEM,
    LOOT_NO_ELM,
};

//�û�����
class CCUser 
{	
	friend class CCUserBuilding;
	friend class CCUserBase;
	friend class CCUserEvent;
private:
	CCMutex							m_mutex;
private:
	CT_DWORD						m_dwID;					//�û�id
	CCNetInterface*					m_pNetInterface;		//�û��ӿ�
	CT_INT64						m_lHeartBeat;			//����
	CT_INT32						m_iHeartCount;			//����ֹͣ����
	CT_BOOL							m_bTalk;				//����
	CT_INT32						m_iChatTime;			//�������
	CT_BOOL							m_bInit;				//���ݳ�ʼ��

public:
	CCUserBase                      m_userBase;             //�û���������
	CCUserBuilding                  m_userBuilding;         //�û���������
	CCUserEvent                     m_userEvent;            //�û��¼�����
	CCUserSkill                     m_userSkill;            //�û���������
    CCUserTalent                    m_userTalent;           //�û�Ǳ�ܣ�������
	CCUserItem                      m_userItem;             //�û���Ʒװ������
    CCUserEquipment                 m_userEquip;            //�û�װ����Ϣ
    CCUserEnchant                   m_userEnchant;          //�û���ħ��Ϣ
    CCUserBuff                      m_userBuff;             //�û���������
	CCUserRegion					m_userRegion;			//�û�������Ϣ
	CCUserDuplicate1				m_userDuplicate;		//�û���������
	CCUserArmy						m_userArmy;				//�û���������
	CCUserBattleArray				m_userBattleArray;		//�û�������
	CCUserTask						m_userTask;				//�û�����
    CCUserDaily                     m_userDaily;            //�û��ճ��
	CCUserBlackMarket				m_userBlackMarket;		//�û��ڵ�����
	CCUserPet						m_userPet;				//�û���������
	CCUserPVPInfo					m_userPVPInfo;			//�û�����������
	CCUserVip						m_userVip;				//�û�vip��Ϣ
	CCUserActive					m_userActive;			//�û����Ϣ
	CCUserWorship					m_userWorship;			//�û�������Ϣ
	CCUserFriend					m_userFriend;			//�û�������Ϣ
	CCUserTower						m_userTower;			//�û�Ԫ��֮��
	CCUserTeamDuplicate				m_userTeamDuplicate;	//�û���Ӹ���
    CCUserItem                      m_userElmPower;         //�û�Ԫ��֮������
    CCUserBattleFlag                m_userBattleFlag;       //ս������
    CCUserElmAttr                   m_userElmAttr;			// Ԫ������
	CCUserBoss						m_userBoss;				//����BOSS
	CCUserFightTeamNember			m_userFightTeamNember;	//ս�ӳ�Ա
	CCUserTitle						m_userTitle;			//�û�ͷ��
	CCUserIsland					m_userIsland;			//�û�����
	CCUserSeek						m_userSeek;				//�û�̽��

public:
	//��ʼ��
	CT_VOID Init();
	//��������,�û��˳�ʹ�ã���ֹ�ڴ�ռ�ù���
	CT_VOID Clear();

public:
	//�����û�id 
	CT_VOID SetUserID(CT_DWORD id) {_AUTOMUTEX(&m_mutex);   m_dwID = id;}
	//�����û�����ӿ�
	CT_VOID SetNetInterface(CCNetInterface* pNetHandle) {_AUTOMUTEX(&m_mutex);   m_pNetInterface = pNetHandle; }
	//��������ʱ��
	CT_VOID SetHeartBeat(){_AUTOMUTEX(&m_mutex);   m_lHeartBeat = time(0); m_iHeartCount=0;};
	//�õ��û�id
	CT_DWORD        GetUserID() {_AUTOMUTEX(&m_mutex);  return m_dwID; }
	//�õ��û�����ӿ�
	CCNetInterface* GetNetInterface() {_AUTOMUTEX(&m_mutex);  return m_pNetInterface; }
	//��������
	CT_VOID	UpdataHeartBeat(CT_INT64 lTime);
	//�����ȸ���
	CT_VOID UpdataPVPDegree(CT_INT32 iDegree);
	//�ر�����
	CT_VOID	CloseNet();
	//��ý���
	CT_BOOL GetTalk(){return m_bTalk;};
	//���ý���
	CT_VOID SetTalk(CT_BOOL bTalk){m_bTalk=bTalk;};
	//��¼�������
	CT_VOID AddChatTime(){_AUTOMUTEX(&m_mutex); m_iChatTime++;}
	//��ȡ����ishu
	CT_INT32 GetChatTime(){return m_iChatTime;}
	//�������ݳ�ʼ��
	CT_VOID  SetInit(CT_BOOL bInit){m_bInit=bInit;};
	//�������ݳ�ʼ��
	CT_BOOL  GetInit(){return m_bInit;};
	//
	CT_BOOL  GetPTRLinkUserData(PTRLinkUserData &linkUserData);

	CT_VOID UserExitEvent(CT_INT64 iExitTime = 0);
    // ��������ͬ��ʱ��
    inline CT_VOID ResetSyncTime(time_t now) 
    {
        m_userItem.m_lastSyncTime = now;
        m_userEquip.m_lastSyncTime = now;
        m_userElmPower.m_lastSyncTime = now;
        m_userElmAttr.m_lastSyncTime = now;
        m_userBuff.m_lastSyncTime = now;
    }


public:
	//�������������
	CT_VOID CalMaxWorker();
	//������й�����Ŀ
	CT_VOID CountFreeWorker();

// Begin [9/10/2012 John]
public:
    // ����������ս����ֵ
    CT_INT32 UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 target = UNIT_TYPE_PLAYER/*, CT_INT32 *buffList = NULL, CT_INT32 buffCount = 0*/);
	//�õ�ս��״̬����
	CT_BOOL GetBattleStatusData(CT_CHAR* pBuf, CT_INT32 &bufLel);
    // ����������ϵ������
    CT_VOID UpdateFactorData(void);
    // �����������ݵ��ͻ���
    CT_VOID UpdateAndSendMasterData(CT_UINT32 flag = 0xffffffff);
	//��ù��ܿ���
	CT_BOOL GetFunctionOpen(CT_CHAR *pBuf,CT_INT32 &iLen);

	// �õ���������
	CT_BOOL GetPetData(CT_CHAR *pBuf, CT_INT32 &bufLen);
	// �õ������б�
	CT_BOOL GetQuestData(CT_CHAR *pBuf, CT_INT32 &bufLen); 

	// Begin [10/22/2012 wangchen]
	// �õ���������
	CT_BOOL GetDuplicateData(CT_CHAR *pBuf, CT_INT32 &bufLen); 
	// End [10/22/2012 wangchen]

    // �������ݵ����ݿ�
    inline CT_VOID SaveData( const CT_DWORD mainID, const CT_DWORD subID, CT_VOID *buf, CT_INT32 bufLen )
    {
        _AUTOMUTEX(&m_mutex);
        CT_INT32 threadID = GAME_THREAD_DATABASE + m_dwID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
        G_SendThreadInfo(threadID, mainID, subID, (CT_CHAR *)buf, bufLen);
    }

    // ��¼����־���ݿ�
    inline CT_VOID SaveLogData( const CT_DWORD mainID, const CT_DWORD subID, CT_VOID *buf, CT_INT32 bufLen )
    {
        _AUTOMUTEX(&m_mutex);
        CT_INT32 threadID = GAME_THREAD_LOGDATABASE + m_dwID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
        G_SendThreadInfo(threadID, mainID, subID, (CT_CHAR *)buf, bufLen);
    }
public:    
    // ʰȡ����
    ENUM_LootItem LootItems(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], const CT_INT32 itemTypes=1, const ITEM_OP_MODE enMode = ITEM_OP_MODE_CKEXE);
public:
	CCUser();
	~CCUser();
private:
	CCUser(const CCUser&);
	CCUser operator = (const CCUser& user);
public:
    // �����������������ݣ�����������ս�����ԣ�����ս������ֵ
	static CT_INT32 CalcUserFinalCombatStatus(MasterCombatStatus &mcs, const PTRLinkUserData &linkUserData,FightingPower &Power);
    // �����������������ݣ�����ָ���ľ��ӵ�ս�����ԣ�����ս������ֵ
    static CT_INT32 CalcArmyFinalCombatStatus(MasterCombatStatus &mcs, const PTRLinkUserData &linkUserData, const ArmyInfo &army);
    // �����������������ݣ�����������ս�����ս�����ݣ�ս�����Ժͼ��ܣ�
    static CT_VOID GetFightingStatusData(UserFightingStatus &data, const PTRLinkUserData &linkUserData,CT_BOOL bSeekFight=CT_FALSE);
}; 

#endif
