#ifndef CCFIGHTINGLOGIC_H_
#define CCFIGHTINGLOGIC_H_
#include <list>
#include <map>
#include "CMD_Game.h"
#include "CGlobeData.h"
#include "CRand.h"

#define				ROUNDD_MAX					15				//���غ�

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
	//״̬��ʶ
	enum TargetType
	{
		//ALL=255,//ȫ��״̬
		Null=0,//��״̬
		Heal=1,//����
		Damage=2,//�˺�
		Critical=4,//����
		Dead=8,//����
		SkillAffect1=16,//Ч��1
		SkillAffect2=32,//Ч��2
		SkillAffect3=64,//Ч��3
		SkillAffect4=128,//Ч��4

	};

	enum EquipSkillType
	{
		Skill_Type_1 = 1,	//��֮����
		Skill_Type_2 = 2,	//��֮����
	};

	enum EquipSkillTime
	{
		Skill_Time_1 = 1,	//�غ�ǰ����
		Skill_Time_2 = 2,	//��������
	};

	//����Ч��
	struct SkillAffect
	{
		CT_INT32 iAttackIncrease;				//����������
		CT_INT32 iDefenceIncrease;				//����������
		CT_INT32 iStrengthIncrease;				//��������
		CT_INT32 iDexterityIncrease;			//��������
		CT_INT32 iIntelligenceIncrease;			//��������
		CT_INT32 iSpiritIncrease;				//��������
		CT_FLOAT fAimIncrease;					//��������
		CT_FLOAT fDodgeIncrease;				//��������
		CT_FLOAT fCriticalHitIncrease;			//��������
		CT_FLOAT fConstitutionIncrease;			//��������
		CT_FLOAT fDamageIncrease;				//�˺���������
		CT_FLOAT fAbsorbDamageIncrease;			//�˺���������
		CT_INT32 iFaint;						//����
		CT_INT32 iExpire;						//����
	};


	
public:
	//ս����ɫ��Ϣ(���������͹���)
	struct FightingUserInfo
	{
		CT_INT32		iHP;											//��ǰHP	
		CT_INT32		iMP;											//��ǰMP
		CT_INT32		iHPMax;											//HP����	
		CT_INT32		iMPMax;											//MP����
		CT_INT32		iHpReply;										//HP�ظ�
		CT_INT32		iMpReply;										//MP�ظ�

		CT_INT32		iSkillStatus[STATUSEX];							//״̬
		CT_INT32		iHpEffect[STATUSEX];							//����Ч��
		CT_INT32		iMpEffect[STATUSEX];							//����Ч��
		SkillAffect		AddStatus[STATUSEX];							//Ч���ӳ�
		CT_INT32		iStatusTime[STATUSEX];							//״̬ʱ��
		CT_BYTE			cbStatusCount;									//״̬��Ŀ

		CT_INT32		iAttack;										//�û�����
		CT_INT32		iDefence;										//�û�����

		CT_INT32		iStrenth;										//�û�����
		CT_INT32		iAgility;										//�û�����
		CT_INT32		iIntellent;										//�û�����
		CT_INT32		iSpirit;										//����
		CT_FLOAT		fHitRate;										//������
		CT_FLOAT		fDodgeRate;										//����
		CT_FLOAT		fCritRate;										//����
		CT_FLOAT		fResilient;										//����
		CT_FLOAT		fDmgIncPercent;									//����˺����ӵİٷֱ�
		CT_FLOAT		fDmgDecPercent;									//�ܵ��˺����ٵİٷֱ�
		CT_FLOAT		fSpeed;											//�ٶ�

		CT_INT32		iFaint;											//����
		CT_INT32		iExpire;										//����

		CT_FLOAT		fHPPersent;										// �������ްٷֱ�

		CT_INT32		iSkill1;										//��ͨ����
		CMD_G_SkillData	iSkill2;										//����2
		CMD_G_SkillData	iSkill3;										//����3
		CMD_G_SkillData	iSkill4;										//����4

		EnuipSkillInfo	SkillEquip;										//װ������

		CMD_G_SkillData PetSkill;										//���＼��

		CT_INT32		iPosition;										//λ��
		CT_DWORD		dwUserID;										//�û�ID
		CT_INT32		iGrade;											//�û��ȼ�
		CT_INT32		iOccupation;									//ְҵ
	};

	typedef	std::list<FightingSave>							FightingSaveList;
	typedef	std::list<FightingSave>::iterator				FightingSaveIT;

private:
	CT_INT32			m_cbRound;					//�غ�
	CT_INT32			m_iWiner;					//ʤ����
	CT_DWORD			m_dwUserID[2];				//˫���ӳ�ID
	CT_INT32			m_iActionPos;				//����λ��
	CT_INT32			m_iActionSort[GAME_PLAYER];	//��������(����0-4�����ܷ�5-9)
	CT_INT32			m_iNoActionUser[GAME_PLAYER];	//δ�ж��û�
	CT_INT32			m_iFightingRound;			//ս���غ�
	CT_INT32			m_iFightingScore;			//ս������
	CT_INT32			m_iMaxHp[GAME_PLAYER];		//�������ֵ��ͳ�Ƹ��������ã�

	FightingUserInfo	m_UserInfo[POST_COUNT];		//�û���Ϣ(����0-4�����ܷ�5-9)
	FightingSaveList	m_FightingList;				//ս����¼��
	CCRand				*m_pRand;					//β���
	CT_BOOL				m_bOrder;					//ս��˳�����������ж��������ұ����ж�

public:
	CCFightingLogic(void);
	~CCFightingLogic(void);

	//���ú���
private:
	//���ú���
	CT_VOID ResetVariable();
	//���ú���
	CT_VOID ResetVariable(CT_INT32 iWiner);

public:
	//��ʼ��
	CT_VOID	Init(CT_INT32 iWiner=WINER_NULL);

public:
	//�����û���Ϣ
	CT_VOID	SetUserInfo(CT_INT32 iPosition,const FightingUserInfo &UserInfo);

public:
	//��ȡʤ����Ϣ
	CT_INT32 GetWiner(){return m_iWiner;};
	//��ȡս��
	FightingSaveList &GetFightingLis(){return m_FightingList;};
	//�����
	CT_INT32 GetRand(){return m_pRand->rand();};
	//��ȡս������
	CT_INT32 GetFightingScore();
	//����˳��
	CT_VOID SetOrder(const CT_BOOL bOrder){/*m_bOrder=bOrder;*/};
	//��ȡ˳��
	CT_BOOL GetOrder(){return CT_TRUE;/*m_bOrder;*/};
	//��������
	CT_VOID SetSeed(CT_INT32 iSeed){m_pRand->srand(iSeed);};
	//��������BOSS��Ϣ
	CT_VOID GetBossInfo(UserFightingStatus &FightingData);

	//ս���߼�
public:
	//ս����Ϣ
	CT_BOOL FigheingInfo();

private:
	//�����û�
	CT_INT32 GetActionUser();
	//Ŀ���û�
	CT_INT32 GetTargetUser(CT_INT32 *pDst, const FightingUserInfo &UserInfo,CT_INT32 iAffectID,CT_INT32 iPosStatus[POST_COUNT]=0);
	//ʹ�ü�����
	CT_INT32 UseSkillCount(const FightingUserInfo &UserInfo);
	//��ȡʤ����
	CT_INT32 GetWiner(FightingUserInfo &fuInfo,CT_INT32 iHurt);
	//���õ�λ����
	CT_VOID SetDie(FightingUserInfo &fuInfo);
	//Ŀ����Ŀ
	CT_INT32 GetPostCount(const CT_INT32 *iPosition);
	//Ŀ����֤
	CT_VOID	ValidatePost(CT_INT32 *iPosition,CT_INT32 iMePosition,CT_BOOL bMonomer,CT_INT32 iPosStatus[POST_COUNT],CT_BOOL bRandPos);
	//��λ����֤
	CT_BOOL	ValidateRow(CT_INT32 *iPosition);
	//���üӳ�״̬
	CT_VOID SetAddStatus(const G_SkillAffect &Affect,SkillAffect &MeAffect);
	//ս������
	CT_VOID	ActionSort(CT_INT32 *pActionSort,CT_INT32 iBegin);
	//����λ��
	CT_INT32 GetAttickPos(CT_INT32 iSkillID,CT_INT32 *pPosition,CT_INT32 iMePosition,FightingUserInfo *UserInfo);
	//��ȡ��������
	CT_INT32 GetMpCost(CT_INT32 iSkillID);
	//����λ��
	CT_BOOL MonomerPos(CT_INT32 iPosition[POST_COUNT],CT_INT32 iOppositePos,CT_INT32 iDstPos,CT_INT32 iMePosition,CT_INT32 iStatus);
	//���＼�ܹ���
	CT_VOID PetSkillAct(FightingUserInfo &fuInfo);

public:
	//��ȡս����Ϣ����ȡ������
	CT_VOID GetFightingInfo(const UserFightingStatus &FightingInfo,FightingUserInfo &UserInfo);
	//��ȡս����Ϣ����ȡʿ����
	CT_VOID GetFightingInfo(const UserFightingStatus &FightingInfo,FightingUserInfo &UserInfo,CT_INT32 iIndex);
	//��ȡս����Ϣ����ȡ���
	CT_VOID GetFightingInfo(G_MonsterBaseInfo &MonsterInfo,FightingUserInfo &UserInfo/*,G_RankStatus &RankStatus*/);

	//���ܹ���
private:
	//���ܹ���Ч��
	CT_VOID SkillActAffect(CT_INT32 iPosition,CT_INT32 iAffectID,CT_INT32 iMePosition,CT_INT32 iHurt);
	//װ������
	CT_VOID SkillEquipAffect(const CT_INT32 iSkillTime,FightingUserInfo &fuInfo);
	//��������
	CT_BOOL SkillAffectHit(const CT_INT32 iAffectID,FightingUserInfo &AttackUserInfo,FightingUserInfo &DstUser,CT_INT32 &iEffect,const CT_FLOAT fAddHurt,FightingSave &FightingSaveInfo);
	//����Ч��
	CT_VOID SkillAffectInfo(CT_INT32 iSkillID,FightingUserInfo &AttackUserInfo,CT_INT32 iPosition[POST_COUNT],CT_BOOL bPetSkill=CT_FALSE);

	//�˺�����
private:
	//�˺�����
	CT_INT32 HurtCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//���м���
	CT_BOOL HitCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//��������
	CT_FLOAT CritCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//���Ƽ���
	CT_INT32 TreatmentCompute(const FightingUserInfo& UserInfo,const FightingUserInfo &DstUser,CT_INT32 iSkillID);
	//״̬����
	CT_BOOL StatusCompute(FightingUserInfo &UserInfo,FightingUserInfo &AddStaus,CT_BOOL bMeRound);

private:
	CT_VOID TextInfo(FightingSave &FightingSaveInfo,CT_INT32 iPos,CT_INT32 iDstPos,CT_INT32 iWin);



};
#endif