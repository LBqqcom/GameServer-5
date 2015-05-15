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

//可操控被动技能栏ID范围
enum ENUM_SKILL_RANGE
{
	SKILL_MIN	=2,
	SKILL_MAX	=5,
};

//宠物幻化
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
	PetInfo				m_PetInfo;				//宠物信息
	PetChangeMap		m_userPetInfoMap;		//用户宠物幻化信息
public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();
	

public:
	CCUserPet();
	~CCUserPet();
public:
	//插入宠物信息
	CT_VOID InsertPetBase(PetInfo & uri,CT_BOOL bSave=CT_FALSE);
	//插入宠物幻化信息
	CT_VOID InsertPetChange(PetChange &pc,CT_BOOL bSave=CT_FALSE);

public:
	//设置幻化ID
	CT_VOID SetChangeID(CT_INT32 iID);
	//设置主技能
	CT_VOID SetSkillID();

public:
	//获取宠物信息
	CT_BOOL GetPetInfo(PetInfo &uri);
	//获取幻化ID
	CT_BOOL GetChangeID(CT_INT32 iID);
	//获取宠物主动技能
	CT_INT32 GetPetSkill(CT_INT32 iID);
	//获取宠物信息
	CT_BOOL GetPetInfo(CT_CHAR *pBuf,CT_INT32 &iLen);

public:
	//添加经验
	CT_VOID AddPetExp(CT_INT32 iExp);
	//学习技能
	CT_VOID LearnSkill(CT_INT32 iSkillID,CT_INT32 iSkillBar);
	//获取技能类型
	CT_INT32 GetPetSkillType(CT_INT32 iSkillID){return iSkillID/10;};
	//获取技能等级
	CT_INT32 GetPetSkillGrade(CT_INT32 iSkillID){return iSkillID%10;};
	//发送宠物信息
	CT_VOID SendUserPetInfo();
	// 更新战旗提供的战斗属性
	static CT_INT32 UpdateBattleStatus(MasterCombatStatus &mcs,const PetInfo &pi,CT_INT32 iVipGrade,CT_INT32 iType,PetChangeMap &pcmap);
	// 更新宠物装备的防御属性
	static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserItem::DataMap &itemMap, const CCUserEquipment::DataMap &equipMap);

	// 得到所有的幻化ID
	CT_BOOL GetAllChangID(CT_INT32* pChangeID, CT_INT32& iSize);
};

#endif