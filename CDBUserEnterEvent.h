/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBUserEnterEvent.h
Version:		1.0
Description:    ���ݿ�������
Author:			pengwen
Create Date:	2012-8-16
History:        ����DB�����¼�   

******************************************************************/
#ifndef CCDBUSERENTEREVENT_H_
#define CCDBUSERENTEREVENT_H_


#include "CGameMesEvent.h"
#include "CThreadMes.h"
#include<list>

class CCDBConnector;
struct MSC_DB_UserEnter;
struct User_Fighting;
struct MasterData;
struct SkillData;
struct EquipmentData;
struct BuffData;
struct MasterFightAdd;

// ���ڼ�������������ս��
struct PTRLinkUserData;

class CCDBUserEnterEvent :
	public CCGameMesEvent
{
    struct OnLinkUserData
    {
        MasterData	masterInfo;
        MSC_DB_BattleArray  BattleArraydata;
        CT_INT32 VipGrade;
        CCUserTalent::DataMap talentMap;
        CCUserSkill::DataMap skillMap;
        CCUserItem::DataMap itemMap;
        CCUserEquipment::DataMap equipMap;
        CCUserBuff::DataMap buffMap;
        CCUserArmy::DataMap armyMap;
        CCUserEnchant::DataMap enchantMap;
        CCUserBattleFlag::DataMap flagMap;
        CCUserItem::DataMap elmMap;
        CCUserElmAttr::DataMap elmAttrMap;
		CCUserTower::AwardStatusMap towerMap;
        CCUserFightTeamNember::DataMap scienceMap;
		CCUserEnchant::EnchantRewardVector enchantRewardVector;
		PetInfo pi;
		CCUserTitle::DataMap titleMap;
		CT_INT32 SeekArray[POST_COUNT/2];
		WallAdditionMap wamap;
		CCUserBase::MasterFightAddMap mfamap;
		CCUserPet::PetChangeMap pcmap;
    };
private:
	CCDBUserEnterEvent();
private: 
	CCDBConnector& m_dbConnector;
public:
	CCDBUserEnterEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector );
	~CCDBUserEnterEvent(void);
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
								CT_CHAR* pBuf,   CT_DWORD len,
								CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{
			
		char* p = 0;
	}

	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
			char* p = 0;
	}
private:
	//��������Ϣ
	CT_VOID DisposeLoginSubEvent( CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);

	//�������û���Ϣ
private: 
	//�������û���Ϣ
	CT_VOID GetNoLinkUserInfo(CT_DWORD *dwUserID,UserFightingStatus *Userinfo,LookUserInfo *LookUserinfo,CT_INT32 iUserCount);
	//������а���Ϣ
	CT_VOID GetTopInfo(CT_CHAR* pBuf, CT_DWORD len);
	//��ö�����Ϣ
	CT_BOOL GetChalleng(CT_CHAR* pBuf, CT_DWORD len);
	//�鿴������Ϣ
	CT_BOOL GetAnother(CT_CHAR* pBuf, CT_DWORD len);
	//�鿴ս�ӳ�Աս������
	CT_BOOL GetFTFightingData(CT_CHAR* pBuf, CT_DWORD len);
	//��Ӹ����Զ�ƥ��
	CT_VOID GetTeamDuplicateAutoMatch(CT_CHAR* pBuf, CT_DWORD len);
	//������Ӫ����������ȡ״̬
	CT_VOID UpdatePresent(CT_CHAR* pBuf, CT_DWORD len);
	//��̽������Ϣ��ȡ
	CT_VOID GetSeekerInfo(CT_CHAR* pBuf,CT_DWORD len);
	// �õ����˵ĳ�����Ϣ
	CT_VOID DisposeGetOtherTown(CT_CHAR* pBuf, CT_INT32 iLen); 

private:
	//
	CT_VOID GetPTRLinkUserData(OnLinkUserData &UserData,PTRLinkUserData &linkUserData);
	// ����������ս������
	CT_VOID UpdateBattleStatus(OnLinkUserData &UserData);

    /*
    * �����ݿ��ȡ����
    */
    // �õ�������Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, MasterData &md);
    // �õ�Ǳ������
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserTalent::DataMap &talentMap);
    // �õ���������
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserSkill::DataMap &skillMap);
    // �õ�������Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserItem::DataMap &itemMap);
    // �õ�װ����Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserEquipment::DataMap &equipMap);
    // �õ���������
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserBuff::DataMap &buffMap);
    // �õ�������Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserArmy::DataMap &armyMap);
    // �õ�����Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, MSC_DB_BattleArray &battleArray);
    // ��ȡ������Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, PetInfo &pi);
    // ��ȡvip��Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CT_INT32 &vipGrade, CT_INT32 &vipTime, CT_INT32 &vipGift);
    // ��ȡ��ħ��Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserEnchant::DataMap &enchantMap);
    // �õ�ս����Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserBattleFlag::DataMap &flagMap);
    // �õ�Ԫ��֮����Ϣ
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserItem::DataMap &elmMap, CCUserElmAttr::DataMap &elmAttrMap);
	// �õ�Ԫ�������ý����ӳ�
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserTower::AwardStatusMap &towerMap);
    // �õ�ս�ӿƼ�
    CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserFightTeamNember::DataMap &scienceMap);
	// �õ�ͷ����Ϣ
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserTitle::DataMap& titleMap);
	// �õ���ħ������Ϣ
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserEnchant::EnchantRewardVector& enchantRewardVec);
	// �õ�̽�þ���
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CT_INT32 *pSeekArray);
	// �õ�̽�ó�ǽ�ӳ�
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, WallAdditionMap &wallAdditionMap);
	// �õ�ս���ӳ�
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserBase::MasterFightAddMap &mfaMap);
	// �õ�����û�
	CT_BOOL ReadDBData(const CT_DWORD dwUserID, CCUserPet::PetChangeMap &pcMap);

private:
	//�õ�������Ϣ
	CT_BOOL GetMasterInfo(const MSC_DB_UserEnter*   pTUE,CT_DWORD subID);
	//�õ�����������������Ϣ
	CT_BOOL GetUpgradeBuildingInfo(const MSC_DB_UserEnter*   pTUE);
	//�õ�������Ϣ
	CT_BOOL GetBuildingInfo(const MSC_DB_UserEnter*   pTUE);
	//�õ����������Ľ���
	CT_BOOL GetProductingBuilding(const MSC_DB_UserEnter*   pTUE);
    // Begin [9/20/2012 John]
    // �õ�Ǳ������
    CT_BOOL GetTalentInfo(const MSC_DB_UserEnter*   pTUE);
    // �õ���������
    CT_BOOL GetSkillInfo(const MSC_DB_UserEnter*   pTUE);
    // �õ���������
    CT_BOOL GetItemInfo(const MSC_DB_UserEnter* pTUE);
    // �õ�װ����Ϣ
    CT_BOOL GetEquipInfo(const MSC_DB_UserEnter* pTUE);
    // �õ���������
    CT_BOOL GetBuffInfo(const MSC_DB_UserEnter* pTUE);
    // �õ���ȴ��Ϣ
    CT_BOOL GetCooldownInfo(const MSC_DB_UserEnter *pTUE);
    // End [9/20/2012 John]
	//�õ��û���������
	CT_BOOL GetRegionInfo(const MSC_DB_UserEnter* pTUE);
	//�õ��û����������
	CT_BOOL GetTaskFinishedData(const MSC_DB_UserEnter* pTUE);
	//�õ��û��ڽ�������
	CT_BOOL	GetTaskProgressData(const MSC_DB_UserEnter* pTUE);
    // �õ��ճ����������Ϣ
    CT_BOOL GetDailyQuestInfo(const MSC_DB_UserEnter* pTUE);
    // �õ�ǩ����Ϣ
    CT_BOOL GetDailySignInfo(const  MSC_DB_UserEnter* pTUE);
    // �õ���Ծ����Ϣ
    CT_BOOL GetDailyActiveInfo(const  MSC_DB_UserEnter* pTUE);
	// Begin [10/22/2012 wangchen]
	// �õ����糡������
	CT_BOOL GetWorldSceneInfo(const MSC_DB_UserEnter*   pTUE);
	// �õ���������
	CT_BOOL GetSceneInfo(const MSC_DB_UserEnter*   pTUE);
	//�õ�������Ϣ
	CT_BOOL	GetArmyInfo(const MSC_DB_UserEnter*   pTUE);
	//�õ�����Ϣ
	CT_BOOL	GetBattleArray(const MSC_DB_UserEnter*   pTUE);
	//��������Ϣ
	CT_BOOL GetPVPInfo(const  MSC_DB_UserEnter* pTUE);
	//������ս��
	CT_BOOL GetPVPRecord(const  MSC_DB_UserEnter* pTUE);
	//��ȡ�̵�״̬
	CT_BOOL GetMarketFunctionStatus(const  MSC_DB_UserEnter* pTUE);
	//��ȡ�̵�����޹�
	CT_BOOL GetMarketPersonal(const  MSC_DB_UserEnter* pTUE);
	//��ȡ�ڵ���Ϣ
	CT_BOOL	GetBlackMarketInfo(const  MSC_DB_UserEnter* pTUE);
	//��ȡ������Ϣ
	CT_BOOL GetPetInfo(const  MSC_DB_UserEnter* pTUE);
	//��ȡ�ʼ���Ϣ
	CT_BOOL GetEmailInfo(const  MSC_DB_UserEnter* pTUE);
	//��ȡvip��Ϣ
	CT_BOOL	GetVipInfo(const  MSC_DB_UserEnter* pTUE);
	//��ȡ���Ϣ
	CT_BOOL GetUserActiveEventInfo(const MSC_DB_UserEnter* pTUE);
	//��ȡ������Ϣ
	CT_BOOL GetUserWorshipInfo(const MSC_DB_UserEnter* pTUE);
	//��ȡɨ����Ϣ
	CT_BOOL GetRaidsInfo(const MSC_DB_UserEnter* pTUE);
	//��ȡ������Ϣ
	CT_BOOL GetFriendInfo(const MSC_DB_UserEnter* pTUE);
	//��ȡ��Ӹ�������
	CT_BOOL GetTeamDuplicateSceneInfo(const MSC_DB_UserEnter* pTUE);
	//��ȡ��Ӫ���߽���
	CT_BOOL GetPresent(const MSC_DB_UserEnter* pTUE);
	// End [10/22/2012 wangchen]
    // ��ȡ��ħ��Ϣ
    CT_BOOL GetEnchantInfo(const MSC_DB_UserEnter *pTUE);
	//��ȡ��ħ������Ϣ
	CT_BOOL GetEnchantRewardInfo(const MSC_DB_UserEnter *pTUE);
    // ���Ԫ��֮������
    CT_BOOL GetElmPowerInfo(const MSC_DB_UserEnter *pTUE);
    // ���ս������ 
    CT_BOOL GetBattleFlagInfo(const MSC_DB_UserEnter *pTUE);  
	//�õ�Ԫ��֮������ս������
	CT_VOID GetTowerOffLineUserFightData(CT_CHAR* pBuf, CT_INT32 len);
	//Ԫ��֮��
	CT_BOOL GetTower(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerHelp(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerSlotReward(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerEvent(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetTowerSlotFightRecord(const MSC_DB_UserEnter *pTUE);

	//ϵͳ����
	CT_BOOL GetMSG(const MSC_DB_UserEnter *pTUE);
	CT_BOOL GetBossRecord(const MSC_DB_UserEnter *pTUE);
	//ս�Ӹ��˿Ƽ�
	CT_BOOL GetFightTeamScience(const MSC_DB_UserEnter *pTUE);

	//�õ�ͷ����Ϣ
	CT_BOOL GetTitleInfo(const MSC_DB_UserEnter *pTUE);
	// ��ȡ��������
	CT_BOOL GetDragonIslandData(const MSC_DB_UserEnter *pTUE);
	//��ȡ��ǽ����
	CT_BOOL GetWallAddition(const MSC_DB_UserEnter *pTUE);
	//��ȡ��ǽ����
	CT_BOOL GetWallArmy(const MSC_DB_UserEnter *pTUE);
	//��ȡ�����̽�����б�
	CT_BOOL GetSeekRandList(const MSC_DB_UserEnter *pTUE);
	//��ȡ��̽�õ��˵��б�
	CT_BOOL GetHadSeekedList(const MSC_DB_UserEnter *pTUE);
	//��ȡ̽����Ϣ
	CT_BOOL GetSeekInfo(const MSC_DB_UserEnter *pTUE);
	//��ȡ̽��ս��
	CT_BOOL GetSeekFightRecord(const MSC_DB_UserEnter *pTUE,std::string strIndex);
	//��ȡ����ս���ӳ�
	CT_BOOL GetMasterFightAdd(const MSC_DB_UserEnter *pTUE);

	// �õ�������Ϣ
	CT_VOID GetBuildingDate(CT_DWORD dwUserID, CCUserBuilding::BuildingDataMap& buildingMap);
	// �õ�����������Ϣ
	CT_VOID GetBuildingProduct(CT_DWORD dwUserID, CCUserBuilding::ProductingtManualBuildingMap& productBuildingMap);
	// �õ�������������Ϣ
	CT_VOID GetBuildingUpdate(CT_DWORD dwUserID, CCUserBuilding::UpgradeingBuildDataMap& upgradeBuildingMap);
	// �õ�������Ϣ
	CT_VOID GetTeamFlag(CT_DWORD dwUserID, TeamSeekMap& teamFlagMap);
	// �õ�������Ϣ
	CT_VOID GetKidnapeeInfo(CT_DWORD dwUserID, KidnapMap& girlMap, KidnapMap& workerMap, KidnapMap& petMap);
	// �����������
	CT_INT32 CalMaxWorker(CCUserBuilding::BuildingDataMap buildingDataMap);
	// �õ����������
	CT_VOID GetTaskFinish(CT_DWORD dwUserID, CCUserTask::TaskCompleteVector& taskFinishVector);
	// �õ��ѿ����Ĺ���
	CT_VOID GetOpenFunction(CT_INT32* pBuf, CT_INT32& iLen, CT_DWORD dwUserID, CT_INT32 iMaterLevel);
	// �õ����˵ĳ�����Ϣ
	CT_VOID GetOtherTownInfo(MSC_DB_EnterOtherTown* pOtherTown);
	// �õ�����ܵĳ���û�ID
	CT_VOID GetKidnapPet(CT_INT32* pBuf, CT_INT32& iLen, CT_DWORD dwUserID);
	// �õ�ʣ��ĳ���û�ID
	CT_VOID GetLastPetChangeID(CT_INT32* pBuf, CT_INT32& iLen,CT_DWORD dwUserID);

	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);
};
#endif