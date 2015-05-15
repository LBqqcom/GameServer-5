#ifndef _CPet_H_
#define _CPet_H_

#include <map>
#include <vector>
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"
#include "CUserData.h"
#include "CUserBase.h"
#include "CUserItem.h"
#include "CUserEquipment.h"

//�ɲٿر���������ID��Χ
enum ENUM_SKILL_RANGE
{
	SKILL_MIN	=2,
	SKILL_MAX	=5,
};

//����û�
struct PetChange
{
	CT_INT32	iChangeID;
	CT_INT32	iSkillID;
};

class CCUserPet
{
	friend class CCUser;
public:
	typedef std::map<CT_INT32,PetChange>			PetChangeMap;

private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;
	PetInfo				m_PetInfo;				//������Ϣ
	PetChangeMap		m_userPetInfoMap;		//�û�����û���Ϣ
public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();
	

public:
	CCUserPet();
	~CCUserPet();
public:
	//���������Ϣ
	CT_VOID InsertPetBase(PetInfo & uri,CT_BOOL bSave=CT_FALSE);
	//�������û���Ϣ
	CT_VOID InsertPetChange(PetChange &pc,CT_BOOL bSave=CT_FALSE);

public:
	//���ûû�ID
	CT_VOID SetChangeID(CT_INT32 iID);
	//����������
	CT_VOID SetSkillID();

public:
	//��ȡ������Ϣ
	CT_BOOL GetPetInfo(PetInfo &uri);
	//��ȡ�û�ID
	CT_BOOL GetChangeID(CT_INT32 iID);
	//��ȡ������������
	CT_INT32 GetPetSkill(CT_INT32 iID);
	//��ȡ������Ϣ
	CT_BOOL GetPetInfo(CT_CHAR *pBuf,CT_INT32 &iLen);

public:
	//��Ӿ���
	CT_VOID AddPetExp(CT_INT32 iExp);
	//ѧϰ����
	CT_VOID LearnSkill(CT_INT32 iSkillID,CT_INT32 iSkillBar);
	//��ȡ��������
	CT_INT32 GetPetSkillType(CT_INT32 iSkillID){return iSkillID/10;};
	//��ȡ���ܵȼ�
	CT_INT32 GetPetSkillGrade(CT_INT32 iSkillID){return iSkillID%10;};
	//���ͳ�����Ϣ
	CT_VOID SendUserPetInfo();
	// ����ս���ṩ��ս������
	static CT_INT32 UpdateBattleStatus(MasterCombatStatus &mcs,const PetInfo &pi,CT_INT32 iVipGrade,CT_INT32 iType,PetChangeMap &pcmap);
	// ���³���װ���ķ�������
	static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserItem::DataMap &itemMap, const CCUserEquipment::DataMap &equipMap);

	// �õ����еĻû�ID
	CT_BOOL GetAllChangID(CT_INT32* pChangeID, CT_INT32& iSize);
};

#endif