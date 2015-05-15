#ifndef CCFIGHTINGLOGIC_H_
#define CCFIGHTINGLOGIC_H_
#include <list>
#include <map>
#include "CMD_Game.h"
#include "CGlobeData.h"
#include "CRand.h"

#define				ROUNDD_MAX					15				//最大回合

struct G_SkillAffect;
struct MSC_Fighting_Info;
struct G_MonsterBaseInfo;
struct MasterCombatStatus;
struct MSC_PVP_Fighting_Info;
struct UserFightingStatus;
struct G_RankStatus;

class CCFightingLogic
{
private:
	//状态标识
	enum TargetType
	{
		//ALL=255,//全部状态
		Null=0,//空状态
		Heal=1,//治疗
		Damage=2,//伤害
		Critical=4,//暴击
		Dead=8,//死亡
		SkillAffect1=16,//效果1
		SkillAffect2=32,//效果2
		SkillAffect3=64,//效果3
		SkillAffect4=128,//效果4

	};

	enum EquipSkillType
	{
		Skill_Type_1 = 1,	//光之纹章
		Skill_Type_2 = 2,	//暗之纹章
	};

	enum EquipSkillTime
	{
		Skill_Time_1 = 1,	//回合前技能
		Skill_Time_2 = 2,	//死亡后技能
	};

	//技能效果
	struct SkillAffect
	{
		CT_INT32 iAttackIncrease;				//攻击力增加
		CT_INT32 iDefenceIncrease;				//防御力增加
		CT_INT32 iStrengthIncrease;				//力量增加
		CT_INT32 iDexterityIncrease;			//敏捷增加
		CT_INT32 iIntelligenceIncrease;			//智力增加
		CT_INT32 iSpiritIncrease;				//精神增加
		CT_FLOAT fAimIncrease;					//命中增加
		CT_FLOAT fDodgeIncrease;				//闪避增加
		CT_FLOAT fCriticalHitIncrease;			//暴击增加
		CT_FLOAT fConstitutionIncrease;			//韧性增加
		CT_FLOAT fDamageIncrease;				//伤害增加增加
		CT_FLOAT fAbsorbDamageIncrease;			//伤害减免增加
		CT_INT32 iFaint;						//昏迷
		CT_INT32 iExpire;						//气绝
	};


	
public:
	//战斗角色信息(可用领主和怪物)
	struct FightingUserInfo
	{
		CT_INT32		iHP;											//当前HP	
		CT_INT32		iMP;											//当前MP
		CT_INT32		iHPMax;											//HP上限	
		CT_INT32		iMPMax;											//MP上限
		CT_INT32		iHpReply;										//HP回复
		CT_INT32		iMpReply;										//MP回复

		CT_INT32		iSkillStatus[STATUSEX];							//状态
		CT_INT32		iHpEffect[STATUSEX];							//技能效果
		CT_INT32		iMpEffect[STATUSEX];							//技能效果
		SkillAffect		AddStatus[STATUSEX];							//效果加成
		CT_INT32		iStatusTime[STATUSEX];							//状态时间
		CT_BYTE			cbStatusCount;									//状态数目

		CT_INT32		iAttack;										//用户攻击
		CT_INT32		iDefence;										//用户防御

		CT_INT32		iStrenth;										//用户力量
		CT_INT32		iAgility;										//用户敏捷
		CT_INT32		iIntellent;										//用户智力
		CT_INT32		iSpirit;										//精神
		CT_FLOAT		fHitRate;										//命中率
		CT_FLOAT		fDodgeRate;										//闪避
		CT_FLOAT		fCritRate;										//暴击
		CT_FLOAT		fResilient;										//韧性
		CT_FLOAT		fDmgIncPercent;									//造成伤害增加的百分比
		CT_FLOAT		fDmgDecPercent;									//受到伤害减少的百分比
		CT_FLOAT		fSpeed;											//速度

		CT_INT32		iFaint;											//昏迷
		CT_INT32		iExpire;										//气绝

		CT_FLOAT		fHPPersent;										// 生命上限百分比

		CT_INT32		iSkill1;										//普通攻击
		CMD_G_SkillData	iSkill2;										//技能2
		CMD_G_SkillData	iSkill3;										//技能3
		CMD_G_SkillData	iSkill4;										//技能4

		EnuipSkillInfo	SkillEquip;										//装备技能

		CMD_G_SkillData PetSkill;										//宠物技能

		CT_INT32		iPosition;										//位置
		CT_DWORD		dwUserID;										//用户ID
		CT_INT32		iGrade;											//用户等级
		CT_INT32		iOccupation;									//职业
	};

	typedef	std::list<FightingSave>							FightingSaveList;
	typedef	std::list<FightingSave>::iterator				FightingSaveIT;

private:
	CT_INT32			m_cbRound;					//回合
	CT_INT32			m_iWiner;					//胜利方
	CT_DWORD			m_dwUserID[2];				//双方队长ID
	CT_INT32			m_iActionPos;				//动作位置
	CT_INT32			m_iActionSort[GAME_PLAYER];	//动作排序(发起方0-4，接受方5-9)
	CT_INT32			m_iNoActionUser[GAME_PLAYER];	//未行动用户
	CT_INT32			m_iFightingRound;			//战斗回合
	CT_INT32			m_iFightingScore;			//战斗分数
	CT_INT32			m_iMaxHp[GAME_PLAYER];		//最大生命值（统计副本分数用）

	FightingUserInfo	m_UserInfo[POST_COUNT];		//用户信息(发起方0-4，接受方5-9)
	FightingSaveList	m_FightingList;				//战斗记录器
	CCRand				*m_pRand;					//尾随机
	CT_BOOL				m_bOrder;					//战斗顺序，正序：左方先行动，倒序：右边先行动

public:
	CCFightingLogic(void);
	~CCFightingLogic(void);

	//重置函数
private:
	//重置函数
	CT_VOID ResetVariable();
	//重置函数
	CT_VOID ResetVariable(CT_INT32 iWiner);

public:
	//初始化
	CT_VOID	Init(CT_INT32 iWiner=WINER_NULL);

public:
	//设置用户信息
	CT_VOID	SetUserInfo(CT_INT32 iPosition,const FightingUserInfo &UserInfo);

public:
	//获取胜利信息
	CT_INT32 GetWiner(){return m_iWiner;};
	//获取战报
	FightingSaveList &GetFightingLis(){return m_FightingList;};
	//随机数
	CT_INT32 GetRand(){return m_pRand->rand();};
	//获取战斗分数
	CT_INT32 GetFightingScore();
	//设置顺序
	CT_VOID SetOrder(const CT_BOOL bOrder){/*m_bOrder=bOrder;*/};
	//获取顺序
	CT_BOOL GetOrder(){return CT_TRUE;/*m_bOrder;*/};
	//设置种子
	CT_VOID SetSeed(CT_INT32 iSeed){m_pRand->srand(iSeed);};
	//拷贝世界BOSS信息
	CT_VOID GetBossInfo(UserFightingStatus &FightingData);

	//战斗逻辑
public:
	//战斗信息
	CT_BOOL FigheingInfo();

private:
	//攻击用户
	CT_INT32 GetActionUser();
	//目标用户
	CT_INT32 GetTargetUser(CT_INT32 *pDst, const FightingUserInfo &UserInfo,CT_INT32 iAffectID,CT_INT32 iPosStatus[POST_COUNT]=0);
	//使用技能数
	CT_INT32 UseSkillCount(const FightingUserInfo &UserInfo);
	//获取胜利者
	CT_INT32 GetWiner(FightingUserInfo &fuInfo,CT_INT32 iHurt);
	//设置单位死亡
	CT_VOID SetDie(FightingUserInfo &fuInfo);
	//目标数目
	CT_INT32 GetPostCount(const CT_INT32 *iPosition);
	//目标验证
	CT_VOID	ValidatePost(CT_INT32 *iPosition,CT_INT32 iMePosition,CT_BOOL bMonomer,CT_INT32 iPosStatus[POST_COUNT],CT_BOOL bRandPos);
	//列位置验证
	CT_BOOL	ValidateRow(CT_INT32 *iPosition);
	//设置加成状态
	CT_VOID SetAddStatus(const G_SkillAffect &Affect,SkillAffect &MeAffect);
	//战斗排序
	CT_VOID	ActionSort(CT_INT32 *pActionSort,CT_INT32 iBegin);
	//攻击位置
	CT_INT32 GetAttickPos(CT_INT32 iSkillID,CT_INT32 *pPosition,CT_INT32 iMePosition,FightingUserInfo *UserInfo);
	//获取法力消耗
	CT_INT32 GetMpCost(CT_INT32 iSkillID);
	//单体位置
	CT_BOOL MonomerPos(CT_INT32 iPosition[POST_COUNT],CT_INT32 iOppositePos,CT_INT32 iDstPos,CT_INT32 iMePosition,CT_INT32 iStatus);
	//宠物技能攻击
	CT_VOID PetSkillAct(FightingUserInfo &fuInfo);

public:
	//提取战斗信息（提取领主）
	CT_VOID GetFightingInfo(const UserFightingStatus &FightingInfo,FightingUserInfo &UserInfo);
	//提取战斗信息（提取士兵）
	CT_VOID GetFightingInfo(const UserFightingStatus &FightingInfo,FightingUserInfo &UserInfo,CT_INT32 iIndex);
	//提取战斗信息（提取怪物）
	CT_VOID GetFightingInfo(G_MonsterBaseInfo &MonsterInfo,FightingUserInfo &UserInfo/*,G_RankStatus &RankStatus*/);

	//技能攻击
private:
	//技能攻击效果
	CT_VOID SkillActAffect(CT_INT32 iPosition,CT_INT32 iAffectID,CT_INT32 iMePosition,CT_INT32 iHurt);
	//装备技能
	CT_VOID SkillEquipAffect(const CT_INT32 iSkillTime,FightingUserInfo &fuInfo);
	//技能命中
	CT_BOOL SkillAffectHit(const CT_INT32 iAffectID,FightingUserInfo &AttackUserInfo,FightingUserInfo &DstUser,CT_INT32 &iEffect,const CT_FLOAT fAddHurt,FightingSave &FightingSaveInfo);
	//技能效果
	CT_VOID SkillAffectInfo(CT_INT32 iSkillID,FightingUserInfo &AttackUserInfo,CT_INT32 iPosition[POST_COUNT],CT_BOOL bPetSkill=CT_FALSE);

	//伤害函数
private:
	//伤害计算
	CT_INT32 HurtCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//命中计算
	CT_BOOL HitCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//暴击计算
	CT_FLOAT CritCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//治疗计算
	CT_INT32 TreatmentCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//状态计算
	CT_BOOL StatusCompute(FightingUserInfo &UserInfo,FightingUserInfo &AddStaus,CT_BOOL bMeRound);

private:
	CT_VOID TextInfo(FightingSave &FightingSaveInfo,CT_INT32 iPos,CT_INT32 iDstPos,CT_INT32 iWin);



};
#endif