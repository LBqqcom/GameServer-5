/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserBase.h
Version:		1.0
Description:    用户基础数据
Author:			pengwen
Create Date:	2012-9-29
History:        用户基础数据 ，处理生命值，攻击力等等  

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
    EMI_LEVEL = 1,              // 领主等级信息
    EMI_CIVILI = (1 << 1),      // 文明等级信息
    EMI_RANK = (1 << 2),        // 军衔等级信息
    EMI_CITY = (1 << 3),        // 城市资源信息（金币，钻石，食物）
    EMI_WORKER = (1 << 4),      // 工人信息
    EMI_TALENT = (1 << 5),      // 潜能点数
    EMI_SKILLSOUL = (1 << 6),   // 技魂点数
    EMI_STAMINA = (1 << 7),     // 体力

    EMI_BATLLE = (1 << 30),     // 战斗数据
};

// 所有资源
#define EMI_RESOURCE (EMI_LEVEL | EMI_CIVILI | EMI_RANK | EMI_CITY | EMI_TALENT | EMI_SKILLSOUL | EMI_STAMINA)

// 货币资源
struct MasterResource
{
	CT_INT32  iGold;                // 金币
	CT_INT32  iDiamod;              // 钻石
	CT_INT32  iFood;                // 食物

	CT_INT32  iMaxWorker;           // 最大工人数目
	CT_INT32  iWorker;              // 可用工人

	CT_INT32  iTalentNum;			// 潜能点数
	CT_INT32  iSkillSoul;           // 技魂值 [9/19/2012 John]
	CT_INT32  iStamina;				// 体力值
	CT_INT32  iStaminaBuyTimes;     // 购买体力的次数
	CT_INT32  iFightingPower;		// 战斗力
	CT_INT32  iFightingFlagPower;	//战旗战力
	CT_INT32  iTeamDuplicateFightingTimes;//组队副本战斗次数

	CT_INT32  iCurrentHP;			// 当前生命值
	CT_INT32  iCurrentMP;			// 当前法力值
    CT_INT32  iBagSlots;            // 背包数量
    CT_INT32  iSkillSlots;          // 技能栏数量
    CT_INT32  iElmSlots;            // 元素纹章包数量
};

// 等级信息
struct MasterLevelInfo
{
	CT_INT32	iLevel;				// 当前等级
	CT_INT32	iCurrentExp;		// 当前经验
};

//为概率
struct FakeProData
{
	CT_INT32	iFPTowerTime;					//元素塔
	CT_INT32	iFPWorshipTime[WORSHIP_COUNT];	//(为概率:fp)祭祀次数
	CT_INT32	iFPChestTime;					//魔盒次数
	CT_INT32	iCaveTime;						//付费洞穴次数
	CT_INT32	iPVPChest;						//PVP宝箱
	CT_INT32	iFTChest;						//召唤宝箱
	CT_INT32	iTurntable[TURNTABLE_COUNT];	//转盘
};

//扩展信息
struct MasterDataEx
{
	CT_INT64	iLeaveTeamTime;					//最后一次离开战队时间
	CT_INT32	iTowerHelpTime;					//当天祝福好友占领点次数
	CT_INT32	iChargeDiamod;					//充值
	CT_INT32	iCostDiamod;					//消费
	FakeProData fpd;							//为概率信息
};

// 计算系数
struct MasterFactor
{
    CT_FLOAT fProductFactor;        // 内政收益系数
    CT_FLOAT fBattleFactor;         // 征战收益系数
};




// End [9/11/2012 John]

struct CMD_G_SkillData;

// 领主战斗属性
struct MasterCombatStatus
{
	CT_INT32 iMaxHP;			    // 生命值上限
	CT_INT32 iMaxMP;			    // 法力值上限
	CT_INT32 iCurrentHP;            // 当前生命值
	CT_INT32 iCurrentMP;            // 当前法力值

	CT_INT32 iStrength;             // 力量
	CT_INT32 iAgility;              // 敏捷
	CT_INT32 iIntellect;            // 智力
	CT_INT32 iSpirit;               // 精神
	CT_FLOAT fHitRate;              // 命中率
	CT_FLOAT fDodgeRate;            // 闪避率
	CT_FLOAT fCritRate;             // 暴击率
	CT_FLOAT fResilient;            // 任性
	CT_FLOAT fDmgIncPercent;        // 造成伤害增加的百分比
	CT_FLOAT fDmgDecPercent;        // 受到伤害减少的百分比
	CT_INT32 iAttack;               // 攻击力
	CT_INT32 iDefence;              // 防御力

	CT_INT32 iHPRecover;            // 生命回复
	CT_INT32 iMPRecover;            // 生命回复

	CT_INT32 iStatus[BUFF_SLOTS];	// 增/减益效果
	CT_FLOAT fHPPersent;            // 生命上限百分比
	CT_INT32 exFightingPower;
	
	CT_FLOAT fEquipPercent;			// 装备能力加成百分比

	CT_FLOAT fSpeed;				//速度
};


// 战斗单位 [8/23/2013 John]
struct CombatUnitData
{
	CT_INT32 type;                                  // 领主：职业ID；士兵：士兵ID；怪物：怪物ID
	CT_INT32 level;                                 // 单位的等级
	CT_CHAR  name[NAME_SIZE];                      // 名称
	CT_INT32 position;                              // 参战位置
	MasterCombatStatus mcs;                         // 战斗属性
	CMD_G_SkillData Skills[1 + EQUIPED_SKILL_COUNT];    // 技能信息
	EnuipSkillInfo SkillEnuip;			//装备技能
	CMD_G_SkillData PetSkill;			//宠物技能
	CT_INT32 iPetChangeID;
};


//pvp信息
struct PVPData
{
	CT_INT32 iVictoryTimes;        //胜利次数
	CT_INT32 iFightTimes;          //战斗次数
	CT_INT32 iVictoryRate;        //胜率
	CT_INT32 iCurrentContinuousWin;//当前连胜
};
struct RankingListStatus
{
	ENUM_RANKING_LIST_TYPE iType;        //排行榜类型
	CT_INT32 iRankingCur;                //当前排名
	CT_INT32 iRankingChange;             //变化排名
	CT_INT32 iRewardStatus;				 //排行榜奖励是否领取
};

//排名信息
struct TopData
{
	RankingListStatus TopInfo[TOP_TYPE_MAX];			//排名信息
	CT_INT32 iMaxContinuousWin;					//最大连胜
};

// 掠夺数据
struct PlunderData 
{
	CT_INT32 iPlunderScore;			// 掠夺积分
};

// 领主可变数据
struct  MasterVariableData
{
	// Begin [9/11/2012 John]
	MasterResource	stRes;			// 货币资源
	MasterLevelInfo stGrade;		// 等级数据
	MasterLevelInfo stCivili;		// 文明数据
	MasterLevelInfo stRank;			// 军衔数据
	PVPData         pvpData;        // pvp数据
	TopData			topData;		//排名数据
	PlunderData		plunderData;	// 掠夺数据
	// End [9/11/2012 John]
};

//领主数据
struct MasterData
{
	CT_DWORD			dwUserID;              //用户id
	CT_CHAR				cNickName[NAME_SIZE];  //昵称
	CT_INT32			iOccupation;		   //职业类型
	CT_DWORD			dwFTID;				   //战队ID
	

	MasterVariableData	mvd;                   //用户数据
	MasterVariableData  primeMvd;              //用户原始数据
    MasterFactor        factor;                // 领主系数
	// Begin [9/10/2012 John]
	MasterCombatStatus	mcs;				   // 战斗状态的领主属性
	// End [9/10/2012 John]
	CT_INT64            llLastExitTime;        //上次退出时间，主要是处理自动生产的问题（比如民居）
    CT_INT64            llLastLoginTime;       //上次登陆时间
    CT_INT32            iChannel;             // 渠道ID
	MasterDataEx		mex;			// 扩展数据
};

//领主战斗属性加成
struct MasterFightAdd
{
	CT_DWORD			dwUserID;
	CT_INT32			iType;
	MasterCombatStatus	mcs;				   // 战斗属性
};

// 能力冷却数据
#pragma pack(push, 4)
struct CooldownData
{    
    CT_INT32 iType;                 // 冷却类型
    CT_INT64 leftTime;              // 剩余冷却时间
    CT_INT64 toggleTime;            // 时间记录起点
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
	MasterData			m_masterData;			//领主信息
	CT_INT64            m_llLastSysTime; //上次想数据库更新的时间
    
    typedef std::map<CT_INT32, CooldownData>				CooldownDataList;            
    typedef std::map<CT_INT32, CooldownData>::iterator      CD_IT;
public:
	typedef std::map<CT_INT32,MasterFightAdd>				MasterFightAddMap;

private:
    CooldownDataList					                    m_CooldownDataList; // 能力冷却列表
	MasterFightAddMap										m_MasterFightAddMap;//战斗属性加成

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
	//设置可变数据数据 
	CT_VOID SetVariableData(MasterVariableData& desMastData, const MasterVariableData& addValue);	
    /** 同步数据到数据库
     * @savaTag 进行同步的原因
     */
    CT_VOID UpdateToDatabase(CT_INT32 saveTag);
public:
	//设置领主信息
	CT_VOID SetMastData(const MasterData& md);
	//设置战斗加成数据
	CT_VOID SetMasterFightAdd(const MasterFightAdd &mfa);

	/** 同步领主信息, 信息会加上变化量，然后同步到内存和数据库。
     ** 完成同步后，检查经验/声望/文明度，当满足升级条件时自动升级。
     * @mvd     信息*变化量*, 
     * @saveTag 进行信息同步的原因
     */
	CT_VOID AddMastData(MasterVariableData& mvd, CT_INT32 saveTag);
	//增加战斗属性
	CT_VOID AddMastFightData(const CT_INT32 iType,MasterCombatStatus& mcs, CT_INT32 saveTag);
	//得到领主数据
	CT_VOID GetMasterData(MasterData& md){_AUTOMUTEX(m_pMutex);  md = m_masterData; }
	//得到领主数据
	CT_VOID GetMasterData(MasterVariableData& mvd){_AUTOMUTEX(m_pMutex);  mvd = m_masterData.mvd; }

	CT_VOID SetMastDataEx(CT_INT64 iValue, CT_INT32 saveTagEx, CT_BOOL bDBSave = CT_TRUE);
	CT_VOID SetMastDataEx(CT_INT32 iValue, CT_INT32 saveTagEx, CT_BOOL bDBSave = CT_TRUE);
	CT_VOID GetMasterDataEx(MasterDataEx& mex){_AUTOMUTEX(m_pMutex);  mex = m_masterData.mex; }
	//得到领主数据
	CT_BOOL GetMasterData(CT_CHAR* pBuf, CT_INT32& bufLen);
	//得到领主战斗属性
	CT_BOOL GetMasterFightAdd(CT_CHAR* pBuf, CT_INT32& bufLen);
	//得到领主战斗属性
	CT_BOOL GetMasterFightAdd(CT_INT32 iType,MasterFightAdd &mfa);
    // 得到背包格子数量
    CT_INT32 GetSlotCount(CT_INT32 slotType) {
        _AUTOMUTEX(m_pMutex); 
        return slotType == ITEM_SLOT_BAG ? 
            m_masterData.mvd.stRes.iBagSlots : m_masterData.mvd.stRes.iElmSlots;
    }

    // 得到技能栏数量
    CT_INT32 GetSkillSlot(void) {_AUTOMUTEX(m_pMutex); return m_masterData.mvd.stRes.iSkillSlots;}
	//得到文明
	CT_VOID GetMasterCivili(MasterLevelInfo& civil) {_AUTOMUTEX(m_pMutex); civil = m_masterData.mvd.stCivili;  }
	//得到等级
	CT_VOID GetMasterGrade(MasterLevelInfo&  grade) {_AUTOMUTEX(m_pMutex); grade = m_masterData.mvd.stGrade; }
	//得到领主排行数据
	CT_BOOL GetMasterTopInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	// 更新等级信息
    CT_VOID UpdateMasterLevel(const MasterLevelInfo &stGrade) { _AUTOMUTEX(m_pMutex); m_masterData.mvd.stGrade = stGrade;}
	// 更新军衔信息
    CT_VOID UpdateMasterRank(const MasterLevelInfo &stRank) {_AUTOMUTEX(m_pMutex); m_masterData.mvd.stRank = stRank;}
	// 更新文明信息
	CT_VOID UpdateMasterCivili(const MasterLevelInfo &stCivili) {_AUTOMUTEX(m_pMutex);m_masterData.mvd.stCivili = stCivili;}    
    // 更新背包格子
    CT_VOID UpdateBagSlot(const CT_INT32 nSlot) {_AUTOMUTEX(m_pMutex);m_masterData.mvd.stRes.iBagSlots += nSlot;}
	//设置战队ID
	CT_VOID SetFightTeamID(CT_DWORD dwFTID){_AUTOMUTEX(m_pMutex);m_masterData.dwFTID=dwFTID;};

    // 更新领主能力冷却
    CT_VOID UpdateCooldown(CT_INT32 iType, CT_INT64 addTime);
    // 刷新领主能力冷却
    CT_VOID RefreshCooldown(void);
    // 退出时同步领主冷却数据 
    CT_VOID SyncCooldown(time_t currTime);
    // 插入能力冷却信息
    CT_VOID InsertCooldownData(CooldownData &data);
    // 读取能力冷却时间
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
    // 发放军衔俸禄
    CT_VOID PushRankSalary();
    // 恢复体力值
    CT_VOID RecoverStamina();
    // 技能栏增加
    CT_VOID IncSkillSlot(CT_INT32 nSlot = 1);
public:
	//得到用户需要保存的可变数据
	CT_VOID GetMasterSaveData(MSC_DB_MasterSaveData& mvd);

public:
    /** 领主升级，检查是否满足条件升级，若满足则自动升级。
     * @bSendFailMsg 此值若为真: 当条件不足时，发送消息通知客户端升级失败；
     *                           条件允许升级时，只会升级一次。
     *               此值若为假: 条件不足不会通知客户端升级失败；条件允许
     *                           升级时，会连续升级，直到经验值不足。
     */
    CT_VOID MasterUpgrage(CT_BOOL bSendFailMsg = true);
    CT_VOID CivilUpgrage(CT_BOOL bSendFailMsg = true);
    CT_VOID RankUpgrage(CT_BOOL bSendFailMsg = true);  
	// 同步体力值
	CT_VOID SyncStamina(void);
	//更新组队副本战斗次数
	CT_VOID UpdataTeamFightingTimes();
	//更新体力购买次数
	CT_VOID UpdataBuyStaminaTimes();
	//更新排行榜奖励领取状态
	CT_VOID UpdateTopRewardStatus(CT_INT32 iType, CT_INT32 iStatus);

public:
	//更新数据到数据库
	CT_VOID UpdataData(CT_INT64 lTime);
    // 消耗某种资源
    CT_BOOL CheckUserRes(CT_INT32 iResID, CT_INT32 iResCnt);
	//修改领主名称
	CT_VOID ChangeMasterName(CT_CHAR *pName);
private:
    //规范数值范围防止数值出界
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