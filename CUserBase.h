/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserBase.h
Version:		1.0
Description:    �û���������
Author:			pengwen
Create Date:	2012-9-29
History:        �û��������� ����������ֵ���������ȵ�  

******************************************************************/
#ifndef CCUSERBASE_H_
#define CCUSERBASE_H_
#include <map>
#include <list>
#include <ctime>
#include "CGDefine.h"
#include "CMutex.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"


class CCMutex;
class CCUser;
struct MSC_DB_MasterSaveData;

enum ENUM_MASTERDATA
{
    EMI_LEVEL = 1,              // �����ȼ���Ϣ
    EMI_CIVILI = (1 << 1),      // �����ȼ���Ϣ
    EMI_RANK = (1 << 2),        // ���εȼ���Ϣ
    EMI_CITY = (1 << 3),        // ������Դ��Ϣ����ң���ʯ��ʳ�
    EMI_WORKER = (1 << 4),      // ������Ϣ
    EMI_TALENT = (1 << 5),      // Ǳ�ܵ���
    EMI_SKILLSOUL = (1 << 6),   // �������
    EMI_STAMINA = (1 << 7),     // ����

    EMI_BATLLE = (1 << 30),     // ս������
};

// ������Դ
#define EMI_RESOURCE (EMI_LEVEL | EMI_CIVILI | EMI_RANK | EMI_CITY | EMI_TALENT | EMI_SKILLSOUL | EMI_STAMINA)

// ������Դ
struct MasterResource
{
	CT_INT32  iGold;                // ���
	CT_INT32  iDiamod;              // ��ʯ
	CT_INT32  iFood;                // ʳ��

	CT_INT32  iMaxWorker;           // �������Ŀ
	CT_INT32  iWorker;              // ���ù���

	CT_INT32  iTalentNum;			// Ǳ�ܵ���
	CT_INT32  iSkillSoul;           // ����ֵ [9/19/2012 John]
	CT_INT32  iStamina;				// ����ֵ
	CT_INT32  iStaminaBuyTimes;     // ���������Ĵ���
	CT_INT32  iFightingPower;		// ս����
	CT_INT32  iFightingFlagPower;	//ս��ս��
	CT_INT32  iTeamDuplicateFightingTimes;//��Ӹ���ս������

	CT_INT32  iCurrentHP;			// ��ǰ����ֵ
	CT_INT32  iCurrentMP;			// ��ǰ����ֵ
    CT_INT32  iBagSlots;            // ��������
    CT_INT32  iSkillSlots;          // ����������
    CT_INT32  iElmSlots;            // Ԫ�����°�����
};

// �ȼ���Ϣ
struct MasterLevelInfo
{
	CT_INT32	iLevel;				// ��ǰ�ȼ�
	CT_INT32	iCurrentExp;		// ��ǰ����
};

//Ϊ����
struct FakeProData
{
	CT_INT32	iFPTowerTime;					//Ԫ����
	CT_INT32	iFPWorshipTime[WORSHIP_COUNT];	//(Ϊ����:fp)�������
	CT_INT32	iFPChestTime;					//ħ�д���
	CT_INT32	iCaveTime;						//���Ѷ�Ѩ����
	CT_INT32	iPVPChest;						//PVP����
	CT_INT32	iFTChest;						//�ٻ�����
	CT_INT32	iTurntable[TURNTABLE_COUNT];	//ת��
};

//��չ��Ϣ
struct MasterDataEx
{
	CT_INT64	iLeaveTeamTime;					//���һ���뿪ս��ʱ��
	CT_INT32	iTowerHelpTime;					//����ף������ռ������
	CT_INT32	iChargeDiamod;					//��ֵ
	CT_INT32	iCostDiamod;					//����
	FakeProData fpd;							//Ϊ������Ϣ
};

// ����ϵ��
struct MasterFactor
{
    CT_FLOAT fProductFactor;        // ��������ϵ��
    CT_FLOAT fBattleFactor;         // ��ս����ϵ��
};




// End [9/11/2012 John]

struct CMD_G_SkillData;

// ����ս������
struct MasterCombatStatus
{
	CT_INT32 iMaxHP;			    // ����ֵ����
	CT_INT32 iMaxMP;			    // ����ֵ����
	CT_INT32 iCurrentHP;            // ��ǰ����ֵ
	CT_INT32 iCurrentMP;            // ��ǰ����ֵ

	CT_INT32 iStrength;             // ����
	CT_INT32 iAgility;              // ����
	CT_INT32 iIntellect;            // ����
	CT_INT32 iSpirit;               // ����
	CT_FLOAT fHitRate;              // ������
	CT_FLOAT fDodgeRate;            // ������
	CT_FLOAT fCritRate;             // ������
	CT_FLOAT fResilient;            // ����
	CT_FLOAT fDmgIncPercent;        // ����˺����ӵİٷֱ�
	CT_FLOAT fDmgDecPercent;        // �ܵ��˺����ٵİٷֱ�
	CT_INT32 iAttack;               // ������
	CT_INT32 iDefence;              // ������

	CT_INT32 iHPRecover;            // �����ظ�
	CT_INT32 iMPRecover;            // �����ظ�

	CT_INT32 iStatus[BUFF_SLOTS];	// ��/����Ч��
	CT_FLOAT fHPPersent;            // �������ްٷֱ�
	CT_INT32 exFightingPower;
	
	CT_FLOAT fEquipPercent;			// װ�������ӳɰٷֱ�

	CT_FLOAT fSpeed;				//�ٶ�
};


// ս����λ [8/23/2013 John]
struct CombatUnitData
{
	CT_INT32 type;                                  // ������ְҵID��ʿ����ʿ��ID���������ID
	CT_INT32 level;                                 // ��λ�ĵȼ�
	CT_CHAR  name[NAME_SIZE];                      // ����
	CT_INT32 position;                              // ��սλ��
	MasterCombatStatus mcs;                         // ս������
	CMD_G_SkillData Skills[1 + EQUIPED_SKILL_COUNT];    // ������Ϣ
	EnuipSkillInfo SkillEnuip;			//װ������
	CMD_G_SkillData PetSkill;			//���＼��
	CT_INT32 iPetChangeID;
};


//pvp��Ϣ
struct PVPData
{
	CT_INT32 iVictoryTimes;        //ʤ������
	CT_INT32 iFightTimes;          //ս������
	CT_INT32 iVictoryRate;        //ʤ��
	CT_INT32 iCurrentContinuousWin;//��ǰ��ʤ
};
struct RankingListStatus
{
	ENUM_RANKING_LIST_TYPE iType;        //���а�����
	CT_INT32 iRankingCur;                //��ǰ����
	CT_INT32 iRankingChange;             //�仯����
	CT_INT32 iRewardStatus;				 //���а����Ƿ���ȡ
};

//������Ϣ
struct TopData
{
	RankingListStatus TopInfo[TOP_TYPE_MAX];			//������Ϣ
	CT_INT32 iMaxContinuousWin;					//�����ʤ
};

// �Ӷ�����
struct PlunderData 
{
	CT_INT32 iPlunderScore;			// �Ӷ����
};

// �����ɱ�����
struct  MasterVariableData
{
	// Begin [9/11/2012 John]
	MasterResource	stRes;			// ������Դ
	MasterLevelInfo stGrade;		// �ȼ�����
	MasterLevelInfo stCivili;		// ��������
	MasterLevelInfo stRank;			// ��������
	PVPData         pvpData;        // pvp����
	TopData			topData;		//��������
	PlunderData		plunderData;	// �Ӷ�����
	// End [9/11/2012 John]
};

//��������
struct MasterData
{
	CT_DWORD			dwUserID;              //�û�id
	CT_CHAR				cNickName[NAME_SIZE];  //�ǳ�
	CT_INT32			iOccupation;		   //ְҵ����
	CT_DWORD			dwFTID;				   //ս��ID
	

	MasterVariableData	mvd;                   //�û�����
	MasterVariableData  primeMvd;              //�û�ԭʼ����
    MasterFactor        factor;                // ����ϵ��
	// Begin [9/10/2012 John]
	MasterCombatStatus	mcs;				   // ս��״̬����������
	// End [9/10/2012 John]
	CT_INT64            llLastExitTime;        //�ϴ��˳�ʱ�䣬��Ҫ�Ǵ����Զ����������⣨������ӣ�
    CT_INT64            llLastLoginTime;       //�ϴε�½ʱ��
    CT_INT32            iChannel;             // ����ID
	MasterDataEx		mex;			// ��չ����
};

//����ս�����Լӳ�
struct MasterFightAdd
{
	CT_DWORD			dwUserID;
	CT_INT32			iType;
	MasterCombatStatus	mcs;				   // ս������
};

// ������ȴ����
#pragma pack(push, 4)
struct CooldownData
{    
    CT_INT32 iType;                 // ��ȴ����
    CT_INT64 leftTime;              // ʣ����ȴʱ��
    CT_INT64 toggleTime;            // ʱ���¼���
};
#pragma pack(pop)


class CCUserBase
{
	friend class CCUserBuilding;
	friend class CCUser;
	friend class CCUserEvent;
	friend class CCUserDaily;
	friend class CCUserEquipment;
	friend class CCUserVip;
	friend class CCUserResourceGrab;
private:
	CCUser*             m_pUser;     
	CCMutex*			m_pMutex;
	MasterData			m_masterData;			//������Ϣ
	CT_INT64            m_llLastSysTime; //�ϴ������ݿ���µ�ʱ��
    
    typedef std::map<CT_INT32, CooldownData>				CooldownDataList;            
    typedef std::map<CT_INT32, CooldownData>::iterator      CD_IT;
public:
	typedef std::map<CT_INT32,MasterFightAdd>				MasterFightAddMap;

private:
    CooldownDataList					                    m_CooldownDataList; // ������ȴ�б�
	MasterFightAddMap										m_MasterFightAddMap;//ս�����Լӳ�

public:
	CCUserBase();
	~CCUserBase(void);
    static const int RawHPPencent = 1;
    static const int RawProductFactor = 1;
    static const int RawBattleFactor = 1;

public:
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser  = pUser;
		m_pMutex = pMutex;
		memset(&m_masterData, 0, sizeof(m_masterData));
		m_CooldownDataList.clear();
		m_MasterFightAddMap.clear();
		m_llLastSysTime = time(0);
	}

	
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		memset(&m_masterData, 0, sizeof(m_masterData));
		m_CooldownDataList.clear();
		m_MasterFightAddMap.clear();
		m_llLastSysTime = time(0);
	}
private:
	//���ÿɱ��������� 
	CT_VOID SetVariableData(MasterVariableData& desMastData, const MasterVariableData& addValue);	
    /** ͬ�����ݵ����ݿ�
     * @savaTag ����ͬ����ԭ��
     */
    CT_VOID UpdateToDatabase(CT_INT32 saveTag);
public:
	//����������Ϣ
	CT_VOID SetMastData(const MasterData& md);
	//����ս���ӳ�����
	CT_VOID SetMasterFightAdd(const MasterFightAdd &mfa);

	/** ͬ��������Ϣ, ��Ϣ����ϱ仯����Ȼ��ͬ�����ڴ�����ݿ⡣
     ** ���ͬ���󣬼�龭��/����/�����ȣ���������������ʱ�Զ�������
     * @mvd     ��Ϣ*�仯��*, 
     * @saveTag ������Ϣͬ����ԭ��
     */
	CT_VOID AddMastData(MasterVariableData& mvd, CT_INT32 saveTag);
	//����ս������
	CT_VOID AddMastFightData(const CT_INT32 iType,MasterCombatStatus& mcs, CT_INT32 saveTag);
	//�õ���������
	CT_VOID GetMasterData(MasterData& md){_AUTOMUTEX(m_pMutex);  md = m_masterData; }
	//�õ���������
	CT_VOID GetMasterData(MasterVariableData& mvd){_AUTOMUTEX(m_pMutex);  mvd = m_masterData.mvd; }

	CT_VOID SetMastDataEx(CT_INT64 iValue, CT_INT32 saveTagEx, CT_BOOL bDBSave = CT_TRUE);
	CT_VOID SetMastDataEx(CT_INT32 iValue, CT_INT32 saveTagEx, CT_BOOL bDBSave = CT_TRUE);
	CT_VOID GetMasterDataEx(MasterDataEx& mex){_AUTOMUTEX(m_pMutex);  mex = m_masterData.mex; }
	//�õ���������
	CT_BOOL GetMasterData(CT_CHAR* pBuf, CT_INT32& bufLen);
	//�õ�����ս������
	CT_BOOL GetMasterFightAdd(CT_CHAR* pBuf, CT_INT32& bufLen);
	//�õ�����ս������
	CT_BOOL GetMasterFightAdd(CT_INT32 iType,MasterFightAdd &mfa);
    // �õ�������������
    CT_INT32 GetSlotCount(CT_INT32 slotType) {
        _AUTOMUTEX(m_pMutex); 
        return slotType == ITEM_SLOT_BAG ? 
            m_masterData.mvd.stRes.iBagSlots : m_masterData.mvd.stRes.iElmSlots;
    }

    // �õ�����������
    CT_INT32 GetSkillSlot(void) {_AUTOMUTEX(m_pMutex); return m_masterData.mvd.stRes.iSkillSlots;}
	//�õ�����
	CT_VOID GetMasterCivili(MasterLevelInfo& civil) {_AUTOMUTEX(m_pMutex); civil = m_masterData.mvd.stCivili;  }
	//�õ��ȼ�
	CT_VOID GetMasterGrade(MasterLevelInfo&  grade) {_AUTOMUTEX(m_pMutex); grade = m_masterData.mvd.stGrade; }
	//�õ�������������
	CT_BOOL GetMasterTopInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	// ���µȼ���Ϣ
    CT_VOID UpdateMasterLevel(const MasterLevelInfo &stGrade) { _AUTOMUTEX(m_pMutex); m_masterData.mvd.stGrade = stGrade;}
	// ���¾�����Ϣ
    CT_VOID UpdateMasterRank(const MasterLevelInfo &stRank) {_AUTOMUTEX(m_pMutex); m_masterData.mvd.stRank = stRank;}
	// ����������Ϣ
	CT_VOID UpdateMasterCivili(const MasterLevelInfo &stCivili) {_AUTOMUTEX(m_pMutex);m_masterData.mvd.stCivili = stCivili;}    
    // ���±�������
    CT_VOID UpdateBagSlot(const CT_INT32 nSlot) {_AUTOMUTEX(m_pMutex);m_masterData.mvd.stRes.iBagSlots += nSlot;}
	//����ս��ID
	CT_VOID SetFightTeamID(CT_DWORD dwFTID){_AUTOMUTEX(m_pMutex);m_masterData.dwFTID=dwFTID;};

    // ��������������ȴ
    CT_VOID UpdateCooldown(CT_INT32 iType, CT_INT64 addTime);
    // ˢ������������ȴ
    CT_VOID RefreshCooldown(void);
    // �˳�ʱͬ��������ȴ���� 
    CT_VOID SyncCooldown(time_t currTime);
    // ����������ȴ��Ϣ
    CT_VOID InsertCooldownData(CooldownData &data);
    // ��ȡ������ȴʱ��
    CT_BOOL GetCooldownData(CT_CHAR* pBuf, CT_INT32& bufLen);
    CT_BOOL GetCooldownData(CT_INT32 cdType, CooldownData &cdData) 
    {
        _AUTOMUTEX(m_pMutex);
        CD_IT it = m_CooldownDataList.find(cdType);
        if (it != m_CooldownDataList.end())
        {
            cdData = it->second;
            return CT_TRUE;
        }
        return CT_FALSE;
    }
    // ���ž���ٺ»
    CT_VOID PushRankSalary();
    // �ָ�����ֵ
    CT_VOID RecoverStamina();
    // ����������
    CT_VOID IncSkillSlot(CT_INT32 nSlot = 1);
public:
	//�õ��û���Ҫ����Ŀɱ�����
	CT_VOID GetMasterSaveData(MSC_DB_MasterSaveData& mvd);

public:
    /** ��������������Ƿ������������������������Զ�������
     * @bSendFailMsg ��ֵ��Ϊ��: ����������ʱ��������Ϣ֪ͨ�ͻ�������ʧ�ܣ�
     *                           ������������ʱ��ֻ������һ�Ρ�
     *               ��ֵ��Ϊ��: �������㲻��֪ͨ�ͻ�������ʧ�ܣ���������
     *                           ����ʱ��������������ֱ������ֵ���㡣
     */
    CT_VOID MasterUpgrage(CT_BOOL bSendFailMsg = true);
    CT_VOID CivilUpgrage(CT_BOOL bSendFailMsg = true);
    CT_VOID RankUpgrage(CT_BOOL bSendFailMsg = true);  
	// ͬ������ֵ
	CT_VOID SyncStamina(void);
	//������Ӹ���ս������
	CT_VOID UpdataTeamFightingTimes();
	//���������������
	CT_VOID UpdataBuyStaminaTimes();
	//�������а�����ȡ״̬
	CT_VOID UpdateTopRewardStatus(CT_INT32 iType, CT_INT32 iStatus);

public:
	//�������ݵ����ݿ�
	CT_VOID UpdataData(CT_INT64 lTime);
    // ����ĳ����Դ
    CT_BOOL CheckUserRes(CT_INT32 iResID, CT_INT32 iResCnt);
	//�޸���������
	CT_VOID ChangeMasterName(CT_CHAR *pName);
private:
    //�淶��ֵ��Χ��ֹ��ֵ����
    static CT_VOID CheckValueRand(CT_INT32 tagValue, CT_INT32& value);
    static CT_VOID CheckValueRand(CT_INT32 tagValue, CT_FLOAT& value);
public:    
    static inline CT_VOID LimiteBattleStatus(MasterCombatStatus &mcs)
    {
        CheckValueRand(VALUE_TYPE_HEALTH, mcs.iMaxHP);
        CheckValueRand(VALUE_TYPE_MANA, mcs.iMaxMP);
        CheckValueRand(VALUE_TYPE_ATTACK, mcs.iAttack);
        CheckValueRand(VALUE_TYPE_DEFENCE, mcs.iDefence);
        CheckValueRand(VALUE_TYPE_STRENTH, mcs.iStrength);
        CheckValueRand(VALUE_TYPE_AGILITY, mcs.iAgility);
        CheckValueRand(VALUE_TYPE_INTELLENT, mcs.iIntellect);
        CheckValueRand(VALUE_TYPE_SPIRIT, mcs.iSpirit);
        CheckValueRand(VALUE_TYPE_HITRATE, mcs.fHitRate);
        CheckValueRand(VALUE_TYPE_DODGERATE, mcs.fDodgeRate);
        CheckValueRand(VALUE_TYPE_CRITRATE, mcs.fCritRate);
        CheckValueRand(VALUE_TYPE_RESILIENT, mcs.fResilient);
        CheckValueRand(VALUE_TYPE_DMGINCPERCENT, mcs.fDmgIncPercent);
        CheckValueRand(VALUE_TYPE_DMGDECPERCENT, mcs.fDmgDecPercent);
    } 
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iClass, const CT_INT32 iLevel, const CT_INT32 iRank);
    static inline CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iAttrID, const CT_FLOAT fValue)
    {
        switch (iAttrID) 
        {
        case VALUE_TYPE_HEALTH:         mcs.iMaxHP += (int)fValue; break;
        case VALUE_TYPE_HPPENCENT:      mcs.fHPPersent += fValue; break;
        case VALUE_TYPE_MANA:           mcs.iMaxMP += (int)fValue; break;
        case VALUE_TYPE_ATTACK:         mcs.iAttack += (int)fValue; break;
        case VALUE_TYPE_DEFENCE:        mcs.iDefence += (int)fValue; break;
        case VALUE_TYPE_STRENTH:        mcs.iStrength += (int)fValue; break;
        case VALUE_TYPE_AGILITY:        mcs.iAgility += (int)fValue; break;
        case VALUE_TYPE_INTELLENT:      mcs.iIntellect += (int)fValue; break;
        case VALUE_TYPE_SPIRIT:         mcs.iSpirit += (int)fValue; break;
        case VALUE_TYPE_HITRATE:        mcs.fHitRate += fValue; break;
        case VALUE_TYPE_DODGERATE:      mcs.fDodgeRate += fValue; break;
        case VALUE_TYPE_CRITRATE:       mcs.fCritRate += fValue; break;
        case VALUE_TYPE_RESILIENT:      mcs.fResilient += fValue; break;
        case VALUE_TYPE_DMGINCPERCENT:  mcs.fDmgIncPercent += fValue; break;
        case VALUE_TYPE_DMGDECPERCENT:  mcs.fDmgDecPercent += fValue; break;
		case VALUE_TYPE_EQUIPPERCENT:   mcs.fEquipPercent += fValue;break;
		case VALUE_TYPE_SPEED:			mcs.fSpeed += fValue;break;
        }
    }
    static CT_VOID UpdateCooldownData(CT_DWORD dwUserID, CooldownData &data);
};

#endif