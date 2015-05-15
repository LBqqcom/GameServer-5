#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CGlobeData.h"
#include "CUserPet.h"
#include "GlobeScriptInterface.h"
#include "CNetInterface.h"
#include "CItemScriptSetting.h"

CCUserPet::CCUserPet()
{
	memset(&m_PetInfo,0,sizeof(m_PetInfo));
	m_userPetInfoMap.clear();
}

CCUserPet::~CCUserPet()
{
	m_userPetInfoMap.clear();
}

CT_VOID CCUserPet::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser = pUser;
	m_pMutex = pMutex;
	m_userPetInfoMap.clear();
	memset(&m_PetInfo,0,sizeof(m_PetInfo));
}

//清理用户数据
CT_VOID CCUserPet::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_userPetInfoMap.clear();
}

//插入宠物信息
CT_VOID CCUserPet::InsertPetBase(PetInfo & uri,CT_BOOL bSave)
{
	_AUTOMUTEX(m_pMutex);
	if(m_PetInfo.iPetID != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::InsertPetBase m_PetInfo.iPetID != 0");
		return;
	}

	memcpy(&m_PetInfo,&uri,sizeof(uri));

	if(bSave)
	{
		MSC_DB_PetBornInfo pbi={0};
		pbi.dwUserID=m_pUser->GetUserID();
		memcpy(&pbi.pi,&m_PetInfo,sizeof(m_PetInfo));
		CT_DWORD dwThreadID=GAME_THREAD_DATABASE+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum);
		G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_INFO,(CT_CHAR*)&pbi,sizeof(pbi)); 
	}
	return;
}

//插入宠物幻化信息
CT_VOID CCUserPet::InsertPetChange(PetChange &pc,CT_BOOL bSave)
{
	_AUTOMUTEX(m_pMutex);

	PetChangeMap::iterator it=m_userPetInfoMap.find(pc.iChangeID);
	if(it == m_userPetInfoMap.end())
	{
		m_userPetInfoMap.insert(PetChangeMap::value_type(pc.iChangeID,pc));

		if(m_PetInfo.iChangeID == pc.iChangeID)
		{
			m_PetInfo.iSkillID[0]=pc.iSkillID;
		}

		if(bSave)
		{
			MSC_DB_PetChange dbpc={0};
			dbpc.dwUserID=m_pUser->GetUserID();
			dbpc.iChangeID=pc.iChangeID;
			dbpc.iSkillID=pc.iSkillID;
			CT_DWORD dwThreadID=GAME_THREAD_DATABASE+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum);
			G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_CHANGE,(CT_CHAR*)&dbpc,sizeof(dbpc)); 
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::InsertPetChange it != m_userPetInfoMap.end()");
		return;
	}
	return;
}

CT_VOID CCUserPet::SetChangeID(CT_INT32 iID)
{
	_AUTOMUTEX(m_pMutex);
	PetChangeMap::iterator it=m_userPetInfoMap.find(iID);
	if(it == m_userPetInfoMap.end())
		return;
	G_PetAddition pa={0};
	if(!G_GetPetAddition(iID,&pa))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeUseEgg G_GetPetAddition iEquipSkill:%d",iID);
	}

	m_PetInfo.iChangeID=iID;
	m_PetInfo.iSkillID[0]=(*it).second.iSkillID;
	m_PetInfo.iSkillID[1]=pa.iPassivitySkill;

	MSC_DB_PetBornInfo pbi={0};
	pbi.dwUserID=m_pUser->GetUserID();
	memcpy(&pbi.pi,&m_PetInfo,sizeof(m_PetInfo));
	CT_DWORD dwThreadID=GAME_THREAD_DATABASE+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum);
	G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_INFO,(CT_CHAR*)&pbi,sizeof(pbi)); 
}

//设置主技能
CT_VOID CCUserPet::SetSkillID()
{
	_AUTOMUTEX(m_pMutex);
	G_PetSkillUpGrade pg={0};
	if(!G_GetPetSkillUpGradeEx(m_PetInfo.iSkillID[0],&pg))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::SetSkillID Skill:%d",m_PetInfo.iSkillID[0]);
		return;
	}
	if(rand()%100 <= pg.fSucPercent*100)
	{
		PetChangeMap::iterator it=m_userPetInfoMap.find(m_PetInfo.iChangeID);
		if(m_userPetInfoMap.end() != it)
		{
			m_PetInfo.iSkillID[0]=pg.iNextSkillID;
			(*it).second.iSkillID=pg.iNextSkillID;
		}

		MSC_DB_PetChange dbpc={0};
		dbpc.dwUserID=m_pUser->GetUserID();
		dbpc.iChangeID=m_PetInfo.iChangeID;
		dbpc.iSkillID=pg.iNextSkillID;
		CT_DWORD dwThreadID=GAME_THREAD_DATABASE+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum);
		G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_CHANGE,(CT_CHAR*)&dbpc,sizeof(dbpc)); 

	}
	return;
}

//获取宠物信息
CT_BOOL CCUserPet::GetPetInfo(PetInfo &uri)
{
	_AUTOMUTEX(m_pMutex);
	if(m_PetInfo.iPetID != 0)
	{
		memcpy(&uri,&m_PetInfo,sizeof(m_PetInfo));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获取幻化ID
CT_BOOL CCUserPet::GetChangeID(CT_INT32 iID)
{
	_AUTOMUTEX(m_pMutex);
	PetChangeMap::iterator it=m_userPetInfoMap.find(iID);
	if(it != m_userPetInfoMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获取宠物主动技能
CT_INT32 CCUserPet::GetPetSkill(CT_INT32 iID)
{
	_AUTOMUTEX(m_pMutex);
	PetChangeMap::iterator it=m_userPetInfoMap.find(iID);
	if(it != m_userPetInfoMap.end())
	{
		return (*it).second.iSkillID;
	}
	return 0;
}

//获取宠物信息
CT_BOOL CCUserPet::GetPetInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);

	if(iLen <= m_userPetInfoMap.size()*sizeof(PetChange))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::GetPetInfo iLen <= m_userPetInfoMap.size()*sizeof(PetChange)");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	for(PetChangeMap::iterator it=m_userPetInfoMap.begin();it != m_userPetInfoMap.end() && iSize < MAX_MES_MAX_BUFF_LEN;++it)
	{
		memcpy(pBuf+iSize,&((*it).second),sizeof((*it).second));
		iSize+=sizeof((*it).second);
	}
	iLen=iSize;
	return CT_TRUE;
}

//添加经验
CT_VOID CCUserPet::AddPetExp(CT_INT32 iExp)
{
	_AUTOMUTEX(m_pMutex);
	PetInfo pi={0};
	memcpy(&pi,&m_PetInfo,sizeof(pi));
	pi.iExp=0;
	m_PetInfo.iExp+=iExp;
	while(CT_TRUE)
	{
		G_PetUpgrade pu={0};
		if(!G_GetPetUpgrade(m_PetInfo.iGrade+1,&pu))
		{
			CCLogMeg::getSingleton().WriteError("CCUserPet::AddPetExp iGrade:%d",m_PetInfo.iGrade+1);
			break;
		}

		if(m_PetInfo.iExp + pu.iExp < 0)
		{
			break;
		}
		else
		{
			m_PetInfo.iGrade+=1;
			m_PetInfo.iExp+=pu.iExp;
			pi.iExp+=pu.iExp;
		}
	}
	MSC_DB_PetBornInfo pbi={0};
	pbi.dwUserID=m_pUser->GetUserID();
	memcpy(&pbi.pi,&m_PetInfo,sizeof(m_PetInfo));
	CT_DWORD dwThreadID=GAME_THREAD_DATABASE+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum);
	G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_INFO,(CT_CHAR*)&pbi,sizeof(pbi)); 

	MSC_LOGDB_PetUpGrade pug={0};
	pug.dwUserID=m_pUser->GetUserID();
	pug.iPetGrade=m_PetInfo.iGrade-pi.iGrade;
	pug.iExp=pi.iExp;
	dwThreadID=GAME_THREAD_LOGDATABASE+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum);
	G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PETUPGRADE,(CT_CHAR*)&pug,sizeof(pug)); 

	if(pug.iPetGrade != 0)
		m_pUser->UpdateAndSendMasterData(EMI_BATLLE);

	return;
}

//学习技能
CT_VOID CCUserPet::LearnSkill(CT_INT32 iSkillID,CT_INT32 iSkillBar)
{
	_AUTOMUTEX(m_pMutex);

	if(iSkillBar < SKILL_MIN || iSkillBar > SKILL_MAX)
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::LearnSkill iSkillBar < SKILL_MIN || iSkillBar > SKILL_MAX");
		return;
	}

	G_PetSkill ps1={0};
	G_PetSkill ps2={0};
	G_ValueRange vr1={0};//
	G_ValueRange vr2={0};//
	if(!G_GetPetSkill(iSkillID,&ps1))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::LearnSkill G_GetPetSkill iSkillID:%d",iSkillID);
		return;
	}
	if(!G_GetPetSkill(m_PetInfo.iSkillID[iSkillBar],&ps2))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::LearnSkill G_GetPetSkill iSkillID:%d",m_PetInfo.iSkillID[iSkillBar]);
		return;
	}
	if(!G_GetValueRange(VALUE_TYPE_PET_SUC_COEFFICIENT,&vr1))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::LearnSkill G_GetValueRange VALUE_TYPE_PET_SUC_COEFFICIENT");
		return;
	}

	if(!G_GetValueRange(VALUE_TYPE_PET_FIXATION_SUC,&vr2))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::LearnSkill G_GetValueRange VALUE_TYPE_PET_FIXATION_SUC");
		return;
	}

	CT_INT32 iRand=rand()%100;
	if(m_PetInfo.iSkillID[iSkillBar] == 0)
	{
		if(iRand > G_GetPetLearnSkill(ps1.fSkillCoefficient[iSkillBar-SKILL_MIN],ps1.fValue,vr1.fMinVal)*100)
			return;
	}
	else if(GetPetSkillGrade(m_PetInfo.iSkillID[iSkillBar]) < GetPetSkillGrade(iSkillID))	
	{
		if(iRand > G_GetPetSkillUpGrade(vr2.fMinVal,ps2.fValue,ps1.fValue,vr1.fMinVal)*100)
			return;
	}
	else
	{
		if(iRand > vr2.fMinVal*100)
			return;
	}

	m_PetInfo.iSkillID[iSkillBar]=iSkillID;

	MSC_DB_PetBornInfo pbi={0};
	pbi.dwUserID=m_pUser->GetUserID();
	memcpy(&pbi.pi,&m_PetInfo,sizeof(m_PetInfo));
	CT_DWORD dwThreadID=GAME_THREAD_DATABASE+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum);
	G_SendThreadInfo(dwThreadID,MSC_MAIN_DB_PET_DATA,MSC_SUB_DB_PET_INFO,(CT_CHAR*)&pbi,sizeof(pbi)); 

	m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
	return;
}

//发送宠物信息
CT_VOID CCUserPet::SendUserPetInfo()
{
	_AUTOMUTEX(m_pMutex);
	m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_PET_INFO,(CT_CHAR*)&m_PetInfo,sizeof(m_PetInfo));

	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iSize=MAX_MES_MAX_BUFF_LEN;

	if(GetPetInfo(cBuf,iSize))
		m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_PET_CHANGE_INFO,cBuf,iSize);
	return;
}

// 更新宠物提供的战斗属性
CT_INT32 CCUserPet::UpdateBattleStatus(MasterCombatStatus &mcs,const PetInfo &pi,CT_INT32 iVipGrade,CT_INT32 iType,PetChangeMap &pcmap)
{
	if(pi.iPetID == 0)return 0;

	MasterCombatStatus mcsFlag = {0};
	G_MonsterBaseInfo mbi={0};
	G_PetAddition pa={0};
	if(!G_GetMonsterBaseInfoData((pi.iPetID<<16)+pi.iGrade,&mbi))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::UpdateBattleStatus petID:%d",pi.iPetID);
		return 0;
	}
	if(!G_GetPetAddition(pi.iChangeID,&pa))
	{
		CCLogMeg::getSingleton().WriteError("CCUserPet::UpdateBattleStatus ChangeID:%d",pi.iChangeID);
		return 0;
	}

	mcsFlag.iAttack=mbi.iBaseAttack+mbi.iBaseAttack*pa.fAckCoefficient;
	mcsFlag.iDefence=mbi.iBaseDefense+mbi.iBaseDefense*pa.fDefCoefficient;
	mcsFlag.iMaxHP=mbi.iBaseHP+mbi.iBaseHP*pa.fHPCoefficient;
	mcsFlag.fSpeed=mbi.fSpeed+mbi.fSpeed*pa.fSepCoefficient;


	G_VipPriList	vipPriList = {0};
	G_VipPrivilege	vp = {0};
	if (G_GetVipPriList(iVipGrade,&vipPriList))
	{
		for (CT_INT32 i=0;i<vipPriList.iPriNum;i++)
		{
			if (G_GetVipPrivilege(vipPriList.iPriList[i],&vp))
			{
				if (vp.iPriType==VIP_PRIVILEGE_PETSKILL)
				{
					break;
				}
			}
		}
	}

	//技能加成
	CT_INT32 iSkillCount=(vp.fPriProperty == 0?SKILL_MAX-1:SKILL_MAX);
	for(CT_INT32 i=1;i<=iSkillCount;++i)
	{
		G_SkillMain sm={0};
		if(pi.iSkillID[i] == 0)continue;
		if(G_GetSkillMainData(pi.iSkillID[i],&sm))
		{
			CT_INT32 iSkillAffect[]={sm.iAffectID1,sm.iAffectID2,sm.iAffectID3,sm.iAffectID4};
			for(CT_INT32 j=0;j<sizeof(iSkillAffect)/sizeof(CT_INT32);++j)
			{
				G_SkillAffect sa={0};
				if(iSkillAffect[j] == 0)continue;
				if(G_GetSkillAffectData(iSkillAffect[j],&sa))
				{
					if((!(sa.iEffectType == CLASS_TYPE_MAX && iType <= CLASS_TYPE_MAX) && 
						!(sa.iEffectType == UNIT_ALL_ARMY && iType > CLASS_TYPE_MAX) && 
						sa.iEffectType != UNIT_TYPE_ARRAY_ALL) && iType != sa.iEffectType)continue;

					mcsFlag.iStrength      += sa.iStrengthIncrease;
					mcsFlag.iAgility       += sa.iDexterityIncrease;
					mcsFlag.iIntellect     += sa.iIntelligenceIncrease;
					mcsFlag.iSpirit        += sa.iSpiritIncrease;
					mcsFlag.fHitRate       += sa.fAimIncrease;
					mcsFlag.fDodgeRate     += sa.fDodgeIncrease;
					mcsFlag.fCritRate      += sa.fCriticalHitIncrease;
					mcsFlag.fResilient     += sa.fConstitutionIncrease;
					mcsFlag.fDmgIncPercent += sa.fDamageIncrease;
					mcsFlag.fDmgDecPercent += sa.fAbsorbDamageIncrease;
					//mcsFlag.iMaxHP         += sa.iMaxHP;
					mcsFlag.iMaxMP         += sa.iMpIncrease;
					mcsFlag.iAttack        += sa.iAttackIncrease;
					mcsFlag.iDefence       += sa.iDefenceIncrease;
					mcsFlag.fSpeed		   += sa.fSpeed;

					CCUserBase::UpdateBattleStatus(mcsFlag, sa.iAddType, sa.fAddValue);

					switch (sa.iAddType) 
					{
					case VALUE_TYPE_HEALTH:         mcs.iMaxHP += mcs.iMaxHP*sa.fAddValue; break;
					case VALUE_TYPE_HPPENCENT:      mcs.fHPPersent += sa.fAddValue; break;
					case VALUE_TYPE_MANA:           mcs.iMaxMP += mcs.iMaxMP*sa.fAddValue; break;
					case VALUE_TYPE_ATTACK:         mcs.iAttack += mcs.iAttack*sa.fAddValue; break;
					case VALUE_TYPE_DEFENCE:        mcs.iDefence += mcs.iDefence*sa.fAddValue; break;
					case VALUE_TYPE_STRENTH:        mcs.iStrength += mcs.iStrength*sa.fAddValue; break;
					case VALUE_TYPE_AGILITY:        mcs.iAgility += mcs.iAgility*sa.fAddValue; break;
					case VALUE_TYPE_INTELLENT:      mcs.iIntellect += mcs.iIntellect*sa.fAddValue; break;
					case VALUE_TYPE_SPIRIT:         mcs.iSpirit += mcs.iSpirit*sa.fAddValue; break;
					case VALUE_TYPE_HITRATE:        mcs.fHitRate += mcs.fHitRate*sa.fAddValue; break;
					case VALUE_TYPE_DODGERATE:      mcs.fDodgeRate += mcs.fDodgeRate*sa.fAddValue; break;
					case VALUE_TYPE_CRITRATE:       mcs.fCritRate += mcs.fCritRate*sa.fAddValue; break;
					case VALUE_TYPE_RESILIENT:      mcs.fResilient += mcs.fResilient*sa.fAddValue; break;
					case VALUE_TYPE_DMGINCPERCENT:  mcs.fDmgIncPercent += mcs.fDmgIncPercent*sa.fAddValue; break;
					case VALUE_TYPE_DMGDECPERCENT:  mcs.fDmgDecPercent += mcs.fDmgDecPercent*sa.fAddValue; break;
					case VALUE_TYPE_EQUIPPERCENT:   mcs.fEquipPercent += mcs.fEquipPercent*sa.fAddValue;break;
					case VALUE_TYPE_SPEED:			mcs.fSpeed += mcs.fSpeed*sa.fAddValue;break;
					}

				}
				else
				{
					CCLogMeg::getSingleton().WriteError("CCUserPet::UpdateBattleStatus G_GetSkillAffectData iSkillAffect[j]:%d",iSkillAffect[j]);
				}
			}
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCUserPet::UpdateBattleStatus G_GetSkillMainData pi.iSkillID:%d",pi.iSkillID);
		}
	}

	//宠物BUFF
	for(PetChangeMap::iterator it=pcmap.begin();it != pcmap.end();++it)
	{
		for(CT_INT32 i=VALUE_TYPE_HEALTH;i<VALUE_TYPE_SPEED;++i)
		{
			G_PetBuffer pb={0};
			if(G_GetPetBuffer((*it).second.iChangeID,VALUE_TYPE_HEALTH,&pb))
			{
				switch (pb.iType) 
				{
				case VALUE_TYPE_HEALTH:         mcs.iMaxHP += mcs.iMaxHP*pb.fValue; break;
				case VALUE_TYPE_HPPENCENT:      mcs.fHPPersent += pb.fValue; break;
				case VALUE_TYPE_MANA:           mcs.iMaxMP += mcs.iMaxMP*pb.fValue; break;
				case VALUE_TYPE_ATTACK:         mcs.iAttack += mcs.iAttack*pb.fValue; break;
				case VALUE_TYPE_DEFENCE:        mcs.iDefence += mcs.iDefence*pb.fValue; break;
				case VALUE_TYPE_STRENTH:        mcs.iStrength += mcs.iStrength*pb.fValue; break;
				case VALUE_TYPE_AGILITY:        mcs.iAgility += mcs.iAgility*pb.fValue; break;
				case VALUE_TYPE_INTELLENT:      mcs.iIntellect += mcs.iIntellect*pb.fValue; break;
				case VALUE_TYPE_SPIRIT:         mcs.iSpirit += mcs.iSpirit*pb.fValue; break;
				case VALUE_TYPE_HITRATE:        mcs.fHitRate += mcs.fHitRate*pb.fValue; break;
				case VALUE_TYPE_DODGERATE:      mcs.fDodgeRate += mcs.fDodgeRate*pb.fValue; break;
				case VALUE_TYPE_CRITRATE:       mcs.fCritRate += mcs.fCritRate*pb.fValue; break;
				case VALUE_TYPE_RESILIENT:      mcs.fResilient += mcs.fResilient*pb.fValue; break;
				case VALUE_TYPE_DMGINCPERCENT:  mcs.fDmgIncPercent += mcs.fDmgIncPercent*pb.fValue; break;
				case VALUE_TYPE_DMGDECPERCENT:  mcs.fDmgDecPercent += mcs.fDmgDecPercent*pb.fValue; break;
				case VALUE_TYPE_EQUIPPERCENT:   mcs.fEquipPercent += mcs.fEquipPercent*pb.fValue;break;
				case VALUE_TYPE_SPEED:			mcs.fSpeed += mcs.fSpeed*pb.fValue;break;
				}
			}
		}
	}


	mcs.iStrength      += mcsFlag.iStrength;
	mcs.iAgility       += mcsFlag.iAgility;
	mcs.iIntellect     += mcsFlag.iIntellect;
	mcs.iSpirit        += mcsFlag.iSpirit;
	mcs.fHitRate       += mcsFlag.fHitRate;
	mcs.fDodgeRate     += mcsFlag.fDodgeRate;
	mcs.fCritRate      += mcsFlag.fCritRate;
	mcs.fResilient     += mcsFlag.fResilient;
	mcs.fDmgIncPercent += mcsFlag.fDmgIncPercent;
	mcs.fDmgDecPercent += mcsFlag.fDmgDecPercent;
	mcs.iMaxHP         += mcsFlag.iMaxHP;
	mcs.iMaxMP         += mcsFlag.iMaxMP;
	mcs.iHPRecover	   += mcsFlag.iHPRecover;
	mcs.iMPRecover	   += mcsFlag.iMPRecover;
	mcs.iAttack        += mcsFlag.iAttack;
	mcs.iDefence       += mcsFlag.iDefence;
	mcs.fHPPersent     += mcsFlag.fHPPersent;
	mcs.iHPRecover     += mcsFlag.iHPRecover;
	mcs.iMPRecover     += mcsFlag.iMPRecover;
	mcs.exFightingPower += mcsFlag.exFightingPower;
	mcs.fSpeed			+= mcsFlag.fSpeed;

	
	return G_GetFightingFlagPower(mcsFlag.iMaxHP,mcsFlag.iMaxMP + mcsFlag.iMPRecover,
		mcsFlag.iAttack,mcsFlag.iDefence,mcsFlag.iStrength, mcsFlag.iAgility,mcsFlag.iIntellect,mcsFlag.iSpirit,
		mcsFlag.fHitRate,mcsFlag.fDodgeRate,mcsFlag.fCritRate,mcsFlag.fResilient,
		mcsFlag.fDmgIncPercent,mcsFlag.fDmgDecPercent,mcsFlag.fHPPersent) + mcs.exFightingPower;
}

#if 1
CT_VOID CCUserPet::UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserItem::DataMap &itemMap, const CCUserEquipment::DataMap &equipMap)
{
	for (CCUserItem::DataMap_CONSTIT itItem = itemMap.begin(); itItem != itemMap.end(); ++itItem)
	{
		G_ItemBase itBase = {0};
		G_GetItemBase(itItem->second.iID, &itBase);
		if (itBase.iTID != ITEM_TYPE_PET_EQUIPMENT)
		{
			continue;
		}

		if (itItem->second.iIndex < ITEM_SLOT_BAG) 	// 忽略背包内的装备
		{
			G_Equipment equip = {0};
			EquipStatus Status[STATUS_TYPES + FORGE_ATTRS + GEM_COUNTS * GEM_ATTRS + GEM_ATTRS] = {0}; //装备能力加成符文会增加两个属性加成
			CCItemScriptSetting::getSingleton().GetEquipment(itItem->second.iID, equip);
			int k = 0;
			// 基本属性                
			for (int n = 0; k < STATUS_TYPES; ++n, ++k)
			{
				Status[k] = equip.Status[n];
			}                

			CT_INT32 iLevel = 0;
			CCUserEquipment::DataMap_CONSTIT itEquip = equipMap.find(itItem->second.key);
			if (itEquip != equipMap.end()) 
			{
				EquipmentData eqData = itEquip->second;
				// 强化
				iLevel = eqData.iLevel;
				// 洗练
				for (int n = 0; k < STATUS_TYPES + FORGE_ATTRS; ++n, ++k)
				{
					Status[k].iSID = eqData.Forge[n].iAID;
					Status[k].fValue = eqData.Forge[n].fValue;
				}
				// 镶嵌
				for (int n = 0; k < STATUS_TYPES + FORGE_ATTRS + GEM_COUNTS * GEM_ATTRS; ++n)
				{
					G_ItemGem gem = {0};
					CCItemScriptSetting::getSingleton().GetItemGem(eqData.Gem[n], gem);
					for (int t = 0; t < GEM_ATTRS; ++t)
					{
						if (gem.Attr[t].iAID == VALUE_TYPE_EQUIPPERCENT)
						{
							// 符文为装备能力加成时，在装备当前值的基础上增加对应的百分比
							G_ValueRange vr = {0};
							if (!G_GetValueRange(VALUE_TYPE_EQUIPPERCENT, &vr) || gem.Attr[t].fValue > vr.fMaxVal || gem.Attr[t].fValue < vr.fMinVal)
							{
								CCLogMeg::getSingleton().WriteError("CCUserEquipment::UpdateBattleStatus VALUE_TYPE_EQUIPPERCENT data error.fvalue:%f", gem.Attr[t].fValue);
								gem.Attr[t].fValue = 0;
							}
							for(int iq = 0;iq < STATUS_TYPES;++iq)
							{
								Status[k].iSID = equip.Status[iq].iSID;
								Status[k].fValue =(equip.Status[iq].fValue + equip.Status[iq].fIncValue * iLevel) * gem.Attr[t].fValue;
								k++;
							}
						}
						else
						{
							Status[k].iSID = gem.Attr[t].iAID;
							Status[k].fValue = gem.Attr[t].fValue;
							k++;
						}
					}
				}
			}

			for (unsigned int j = 0; j < k; ++j)
			{
				CCUserBase::UpdateBattleStatus(mcs, Status[j].iSID, Status[j].fValue + Status[j].fIncValue * iLevel);
			}
		}
	}
}
#endif

// 得到所有的幻化ID
CT_BOOL CCUserPet::GetAllChangID(CT_INT32* pChangeID,CT_INT32& iSize)
{
	_AUTOMUTEX(m_pMutex);

	if (iSize < sizeof(CT_INT32) * m_userPetInfoMap.size())
	{
		return CT_FALSE;
	}

	PetChangeMap::iterator it = m_userPetInfoMap.begin();
	CT_INT32 i = 0;
	for (;it != m_userPetInfoMap.end();++it,++i)
	{
		pChangeID[i] = it->second.iChangeID;
	}

	iSize = i * sizeof(CT_INT32);
	return CT_TRUE;
}