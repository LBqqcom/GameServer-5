#include <ctime>
#include <list>
#include "CMutex.h"
#include "CUser.h"
#include "CMD_Game.h"
#include "CGameConfig.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "GlobeScriptInterface.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "BuildFuntion.h"
#include "CBuildingScriptSetting.h"
#include "CMasterScriptSetting.h"
#include "CItemScriptSetting.h"
#include "CFightingScriptSetting.h"
#include "CNetInterface.h"
#include "CBattleArray.h"
#include "CFightTeamManager.h"
#include "CResourceGrabMgr.h"

CCUser::CCUser() : m_mutex()
{
	//m_uns  = UNS_OUT_GAME;
	m_dwID = 0;
	m_pNetInterface = 0;
	m_lHeartBeat=0;
	m_iHeartCount=0;
	m_bTalk=CT_FALSE;
	m_iChatTime=0;
	
}

CCUser::~CCUser()
{
	//m_uns  = UNS_OUT_GAME;
	m_dwID = 0;
	m_pNetInterface = 0;
	m_lHeartBeat=0;
	m_iHeartCount=0;
	m_bTalk=CT_FALSE;
	m_iChatTime=0;
}

//初始化
CT_VOID CCUser::Init()
{
	m_dwID = 0;
	m_pNetInterface = 0;
	m_lHeartBeat=0;
	m_bTalk=CT_FALSE;
	m_bInit=CT_FALSE;
	m_userBase.Init(this, &m_mutex);
	m_userBuilding.Init(this, &m_mutex);
	m_userEvent.Init(this, &m_mutex);
	m_userSkill.Init(this, &m_mutex);
    m_userTalent.Init(this, &m_mutex);
	m_userItem.Init(this, &m_mutex, ITEM_SLOT_BAG);
    m_userEquip.Init(this, &m_mutex);
    m_userEnchant.Init(this, &m_mutex);
    m_userBuff.Init(this, &m_mutex);
	m_userRegion.Init(this, &m_mutex);
	m_userDuplicate.Init(this,&m_mutex);
	m_userArmy.Init(this,&m_mutex);
	m_userBattleArray.Init(this,&m_mutex);
	m_userTask.Init(this,&m_mutex);
    m_userDaily.Init(this, &m_mutex);
	m_userPVPInfo.Init(this,&m_mutex);
	m_userBlackMarket.Init(this,&m_mutex);
	m_userPet.Init(this,&m_mutex);
	m_userVip.Init(this,&m_mutex);
	m_userActive.Init(this,&m_mutex);
	m_userWorship.Init(this,&m_mutex);
	m_userFriend.Init(this,&m_mutex);
	m_userTeamDuplicate.Init(this,&m_mutex);
	m_userTower.Init(this, &m_mutex);
    m_userElmPower.Init(this,&m_mutex, ITEM_SLOT_ELM);
    m_userBattleFlag.Init(this,&m_mutex);
    m_userElmAttr.Init(this, &m_mutex);
	m_userBoss.Init(&m_mutex);
	m_userFightTeamNember.Init(this,&m_mutex);
	m_userTitle.init(this, &m_mutex);
	m_userIsland.Init(this, &m_mutex);
	m_userSeek.Init(this,&m_mutex);
}

CT_VOID CCUser::Clear()
{
	m_bInit=CT_FALSE;
	m_userBuilding.Clear();
	m_userEvent.Clear();
	m_userSkill.Clear();
    m_userTalent.Clear();
	m_userItem.Clear();
	m_userEquip.Clear();
    m_userEnchant.Clear();
	m_userBuff.Clear();
	m_userRegion.Clear();
	m_userDuplicate.Clear();
	m_userArmy.Clear();
	m_userBattleArray.Clear();
	m_userTask.Clear();
	m_userDaily.Clear();
	m_userPVPInfo.Clear();
	m_userBlackMarket.Clear();
	m_userPet.Clear();
	m_userVip.Clear();
	m_userActive.Clear();
	m_userWorship.Clear();
	m_userFriend.Clear();
	m_userTeamDuplicate.Clear();
	m_userTower.Clear();
    m_userElmPower.Clear();
    m_userBattleFlag.Clear();
    m_userElmAttr.Clear();
	m_userBoss.Clear();
	m_userFightTeamNember.Clear();
	m_userTitle.Clear();
	m_userIsland.Clear();
	m_userSeek.Clear();

	m_userBase.Clear(); // 用户基本数据放在最后清除
}

//心跳更新
CT_VOID	CCUser::UpdataHeartBeat(CT_INT64 lTime)
{
	 _AUTOMUTEX(&m_mutex);
	if(lTime-m_lHeartBeat > g_lHeartBeat)
	{
		//if(m_iHeartCount >= HEART_COUNT)
		{
			//断线处理
			CMD_G_LeaveGame LeaveGame={0};
			LeaveGame.dwUserID=m_dwID;

			CT_DWORD dwThreadID=GAME_THREAD_CITY+(LeaveGame.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum);
			G_SendThreadInfo(dwThreadID,MSC_MAIN_USER_EXIT,0,(CT_CHAR*)&LeaveGame,sizeof(LeaveGame));
			printf("ID:%d 断线处理\n",m_dwID);
		}
		//else
		//{
		//	//发送心跳
		//	CMD_G_Heart_Beat  HeartBeat={0};
		//	HeartBeat.dwUserID=m_dwID;
		//	m_pNetInterface->Send(MAIN_G_HEART_BEAT,0,(CT_CHAR*)&HeartBeat,sizeof(HeartBeat));
		//	//printf("server\n");

		//	m_iHeartCount++;
		//}
	}
	else
	{
		//发送心跳
		CMD_G_Heart_Beat  HeartBeat={0};
		HeartBeat.dwUserID=m_dwID;
		m_pNetInterface->Send(MAIN_G_HEART_BEAT,0,(CT_CHAR*)&HeartBeat,sizeof(HeartBeat));
		//printf("server\n");
	}

	return;
}

//竞技度更新
CT_VOID CCUser::UpdataPVPDegree(CT_INT32 iDegree)
{
	m_userPVPInfo.SetPVPDegree(iDegree);
	return;
}

//计算最大工作人数
CT_VOID CCUser::CalMaxWorker()
{
	 _AUTOMUTEX(&m_mutex);
	m_userBase.m_masterData.mvd.stRes.iMaxWorker  = 0;
	CCUserBuilding::BuildingDataMap::iterator it    = m_userBuilding.m_buildingDataMap.begin();
	CCUserBuilding::BuildingDataMap::iterator itEnd = m_userBuilding.m_buildingDataMap.end();
	for (; it!= itEnd ; ++it)
	{
		G_BuildingType bt = {0};
		if(CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID((*it).second.iID, &bt) && bt.iID == CITY_TYPE_RESIDENTIAL)
		{
			G_BuildingProduct bp = {0};
			if(CCBuildingScriptSetting::getSingleton().GetProductType(bt.iID, (*it).second.iGrade, &bp ))
			{
				G_BuildingUpgrdeEffect bue = {0};
				if(CCBuildingScriptSetting::getSingleton().GetBuildingUpgradeEffectData(bt.iID, (*it).second.iGrade, &bue))
				{
					m_userBase.m_masterData.mvd.stRes.iMaxWorker += bue.iWorkers;
				}
			}
		}
	}
}

//计算空闲工人数目
CT_VOID CCUser::CountFreeWorker()
{
	 _AUTOMUTEX(&m_mutex);
	CT_INT32 iWorker = 0;
	CCUserBuilding::UpgradeingBuildDataMap::iterator it = m_userBuilding.m_upgradeBuildDataMap.begin();
	CCUserBuilding::UpgradeingBuildDataMap::iterator itEnd = m_userBuilding.m_upgradeBuildDataMap.end();
	for (; it!= itEnd; ++it)
	{
		iWorker += (*it).second.buildingUpgrade.iWorker;
	}

	CCUserBuilding::ProductingtManualBuildingMap::iterator itt    = m_userBuilding.m_productingManualBuildingMap.begin();
	CCUserBuilding::ProductingtManualBuildingMap::iterator ittEnd = m_userBuilding.m_productingManualBuildingMap.end();
	for (; itt != ittEnd ; ++itt)
	{
		if (!(*itt).second.bSendCanHavest)
		{
			iWorker += (*itt).second.iWorker;
		}
	}

	//计算宠物训练人数
	/*std::map<CT_INT32,MSC_DB_UserPetInfo>::iterator  ittt = m_userPet.m_userPetInfoMap.begin();
	std::map<CT_INT32,MSC_DB_UserPetInfo>::iterator  itttEnd = m_userPet.m_userPetInfoMap.end();
	for (;ittt!=itttEnd;++ittt)
	{
		if (ittt->second.iStatus==PET_STATUS_TRAIN)
		{
			G_PetField pf = {0};
			if (G_GetPetField(ittt->second.iTrainType,&pf))
			{
				iWorker += pf.iWorker;
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCUser::CountFreeWorker not training type %d",ittt->second.iTrainType);
				m_pNetInterface->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_DATA_ERROR);
				return;
			}
		}
	}*/

	// 除掉被绑架的工人
	SeekData sd = {0};
	m_userSeek.GetSeekData(sd);

	m_userBase.m_masterData.mvd.stRes.iWorker = m_userBase.m_masterData.mvd.stRes.iMaxWorker + iWorker - sd.iFarmarNum;
}

CT_VOID CCUser::UserExitEvent(CT_INT64 iExitTime)
{
	_AUTOMUTEX(&m_mutex);
	if (iExitTime = 0)
	{
		iExitTime = time(0);
	}
	m_userFriend.UserExitEvent(iExitTime);
}

//得到战斗状态数据
CT_BOOL CCUser::GetBattleStatusData(CT_CHAR* pBuf, CT_INT32 &bufLel)
{
	_AUTOMUTEX(&m_mutex);
    // 先更新一次
    UpdateBattleStatus(m_userBase.m_masterData.mcs);

	CMD_G_BattleStatusData dsd;
	memset(&dsd, 0, sizeof(dsd));
	if(bufLel < sizeof(dsd))
	{
		return CT_FALSE;
	}
	dsd.dwUser        = m_dwID;
	dsd.iAgility      = m_userBase.m_masterData.mcs.iAgility;
	dsd.iAttack       = m_userBase.m_masterData.mcs.iAttack;
	dsd.fCritRate     = m_userBase.m_masterData.mcs.fCritRate;
	dsd.iDefence      = m_userBase.m_masterData.mcs.iDefence;
	dsd.fDmgDecPercent= m_userBase.m_masterData.mcs.fDmgDecPercent;
	dsd.fDmgIncPercent= m_userBase.m_masterData.mcs.fDmgIncPercent;
	dsd.fDodgeRate    = m_userBase.m_masterData.mcs.fDodgeRate;
	dsd.fHitRate      = m_userBase.m_masterData.mcs.fHitRate;
	dsd.iIntellent    = m_userBase.m_masterData.mcs.iIntellect;
	dsd.iMaxHP        = (CT_INT32)(m_userBase.m_masterData.mcs.iMaxHP * m_userBase.m_masterData.mcs.fHPPersent);
	dsd.iMaxMP        = m_userBase.m_masterData.mcs.iMaxMP;
    dsd.iCurrentHP    = m_userBase.m_masterData.mcs.iCurrentHP;
    dsd.iCurrentMP    = m_userBase.m_masterData.mcs.iCurrentMP;
	dsd.fResilient    = m_userBase.m_masterData.mcs.fResilient;
	dsd.iSpirit       = m_userBase.m_masterData.mcs.iSpirit;
	dsd.iStrenth      = m_userBase.m_masterData.mcs.iStrength;
	dsd.fSpeed      = m_userBase.m_masterData.mcs.fSpeed;

	dsd.iFightingPower      = m_userBase.m_masterData.mvd.stRes.iFightingPower;
	memcpy(pBuf, &dsd, sizeof(dsd));
	bufLel = sizeof(dsd);
	return CT_TRUE;
}

// 更新领主的系数数据
CT_VOID CCUser::UpdateFactorData(void)
{
	 _AUTOMUTEX(&m_mutex);
	// 原始系数
	MasterFactor factor = {m_userBase.RawProductFactor, m_userBase.RawBattleFactor};

    // 文明等级加成
    MasterVariableData mvd = {0};
    m_userBase.GetMasterData(mvd);
    G_CivilUpgrade cu = {0};
    if (0 != G_GetCivilUpgrade(mvd.stCivili.iLevel, &cu))
    {
        factor.fProductFactor += cu.fDomesticRat;
        factor.fBattleFactor += cu.fWarRat;
    }

	// 增益加成
	{
		CMD_G_BuffData buffDatas[BUFF_SLOTS] = {0};
		CT_INT32 bufLen = sizeof(buffDatas);
		m_userBuff.GetAllData((CT_CHAR*)buffDatas, bufLen);
		for (unsigned int i = 0; i < bufLen / sizeof(*buffDatas); ++i)
		{
			G_ItemBuff buff = {0};
			CCItemScriptSetting::getSingleton().GetItemBuff(buffDatas[i].iID, buff);
			for (unsigned int j = 0; j < ATTR_TYPES; ++j)
			{
				switch (buff.Attr[j].iAID) {
				case VALUE_TYPE_PRODUCTFACTOR: factor.fProductFactor += buff.Attr[j].fValue; break;
				case VALUE_TYPE_BATTLEFACTOR:  factor.fBattleFactor += buff.Attr[j].fValue; break;
				}
			}
		}
	}
	//vip add 属性加成
	G_VipRoleAdd vra={0};
	if (m_userVip.VipRoleAddGet(&vra))
	{
		factor.fProductFactor += vra.fProductFactor;
		factor.fBattleFactor += vra.fBattleFactor;
	}

	//装备属性加成
	for (CCUserItem::DataMap_CONSTIT itItem = m_userItem.m_DataMap.begin(); itItem != m_userItem.m_DataMap.end(); ++itItem)
	{
		if (itItem->second.iIndex < ITEM_SLOT_BAG) 	// 忽略背包内的装备
		{
			G_Equipment equip = {0};
			EquipStatus Status[STATUS_TYPES + FORGE_ATTRS + GEM_COUNTS * GEM_ATTRS] = {0};
			CCItemScriptSetting::getSingleton().GetEquipment(itItem->second.iID, equip);
			int k = 0;               
			// 基本属性                
			for (int n = 0; k < STATUS_TYPES; ++n, ++k)
			{
				if(VALUE_TYPE_PRODUCTFACTOR == equip.Status[n].iSID)
					factor.fProductFactor += equip.Status[n].fValue;
				else if(VALUE_TYPE_BATTLEFACTOR == equip.Status[n].iSID)
					factor.fBattleFactor += equip.Status[n].fValue;
			}                
		}
	}

	//宠物加成
	SeekData sd={0};
	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iSize=MAX_MES_MAX_BUFF_LEN;
	m_userSeek.GetSeekData(sd);
	m_userPet.GetPetInfo(cBuf,iSize);

	for(CT_INT32 i=0;i<iSize/sizeof(PetChange);++i)
	{
		PetChange pc={0};
		memcpy(&pc,cBuf+i*sizeof(PetChange),sizeof(PetChange));

		G_PetBuffer pb={0};
		if(G_GetPetBuffer(pc.iChangeID,VALUE_TYPE_PRODUCTFACTOR,&pb))
		{
			factor.fProductFactor+=pb.fValue;
		}
		memset(&pb,0,sizeof(pb));
		if(G_GetPetBuffer(pc.iChangeID,VALUE_TYPE_BATTLEFACTOR,&pb))
		{
			factor.fBattleFactor+=pb.fValue;
		}
	}

	m_userBase.m_masterData.factor = factor;
}
// 更新领主/军队的战斗数据
CT_INT32 CCUser::UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 target /*, CT_INT32 *buffList, CT_INT32 buffCount*/)
{
    _AUTOMUTEX(&m_mutex);
    CT_INT32 iTotalFightingPower = 0;
    memset(&mcs, 0, sizeof(mcs));
    PTRLinkUserData linkUserData = {0};
    GetPTRLinkUserData(linkUserData);
	UpdateFactorData();

	FightingPower Power={0};
    if (target == UNIT_TYPE_PLAYER) 
	{
		CalcUserFinalCombatStatus(mcs, linkUserData,Power);
        m_userBase.m_masterData.mvd.stRes.iFightingPower = Power.iMasterFightingPower;
		m_userBase.m_masterData.mvd.stRes.iFightingFlagPower = Power.iFightingFlagPower;
        iTotalFightingPower += m_userBase.m_masterData.mvd.stRes.iFightingPower;
    } else {
        ArmyInfo army = {0};
        if (m_userArmy.GetArmyInfo(target, army)) {
            iTotalFightingPower += CalcArmyFinalCombatStatus(mcs, linkUserData, army);
        }
    }
#if 0
    // 战斗中增/减益效果
    for (CT_INT32 i = 0; i < buffCount; ++i)
    {
        G_SkillAffect Skillaffect={0};
        CCFightingScriptSetting::getSingleton().GetSkillAffectData(buffList[i], Skillaffect);

        mcs.iMaxHP         += 0;
        mcs.fHPPersent     += 0;
        mcs.iMaxMP         += 0;
        mcs.iAttack        += Skillaffect.iAttackIncrease;
        mcs.iDefence       += Skillaffect.iDefenceIncrease;
        mcs.iStrength      += Skillaffect.iStrengthIncrease;
        mcs.iAgility       += Skillaffect.iDexterityIncrease;
        mcs.iIntellect     += Skillaffect.iIntelligenceIncrease;
        mcs.iSpirit        += Skillaffect.iSpiritIncrease;
        mcs.fHitRate       += Skillaffect.fAimIncrease;
        mcs.fDodgeRate     += Skillaffect.fDodgeIncrease;
        mcs.fCritRate      += Skillaffect.fCriticalHitIncrease;
        mcs.fResilient     += Skillaffect.fConstitutionIncrease;
        mcs.fDmgIncPercent += Skillaffect.fDamageIncrease;
        mcs.fDmgDecPercent += Skillaffect.fAbsorbDamageIncrease;
    }
	
    // 上限限制    
    CCUserBase::LimiteBattleStatus(mcs);

    // 当前血量/法力值不能高过上限 - 同步血量 (当前血量在战斗外已无意义)
    mcs.iCurrentHP = mcs.iMaxHP;
    mcs.iCurrentMP = mcs.iMaxMP;
#endif
    return iTotalFightingPower;
}

// 更新领主数据到客户端
CT_VOID CCUser::UpdateAndSendMasterData(CT_UINT32 flag /* = 0xffffffff */)
{
	 _AUTOMUTEX(&m_mutex);
    if (flag & EMI_RESOURCE)
    {
        CMD_G_MasterResourceData resData = {0};
        CT_INT32 bufLen = sizeof(resData);

        MasterVariableData mvd = {0};
        m_userBase.GetMasterData(mvd);
        resData.iGradeXp = mvd.stGrade.iCurrentExp;
        resData.iRank = mvd.stRank.iLevel;
        resData.iRankXp = mvd.stRank.iCurrentExp;
        resData.iGold = mvd.stRes.iGold;
        resData.iDiamond = mvd.stRes.iDiamod;
        resData.iFood = mvd.stRes.iFood;
        resData.iTalentNum = mvd.stRes.iTalentNum;
        resData.iStamina = mvd.stRes.iStamina;
        GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_RESOURCE, 
            (CT_CHAR*)&resData, sizeof(resData));
    }
    
    if (flag & EMI_BATLLE)
    {
        // 发送领主战斗数据
        CMD_G_BattleStatusData battleData = {0};
        CT_INT32 bufLen = sizeof(battleData);
        GetBattleStatusData((CT_CHAR *)&battleData, bufLen);
        GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_BUTTLE_STATUS_INFO, 
            (CT_CHAR*)&battleData, sizeof(battleData), false, true);
    }
}

//获得功能开启
CT_BOOL  CCUser::GetFunctionOpen(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	CT_INT32 i=0;
	CT_INT32 len=0;
	SeekData sd = {0};
	m_userSeek.GetSeekData(sd);
	while(CT_TRUE)
	{
		G_FunctionOpen FunctionOpen={0};
		if(!G_GetFunctionOpen(i++,&FunctionOpen))
		{
			iLen=len;
			return len>0?CT_TRUE:CT_FALSE;
		}
		else
		{
			CMD_G_FUNCTIONOPEN fo={0};
			MasterVariableData mvd={0};
			m_userBase.GetMasterData(mvd);
			fo.iTaskID=(CT_INT32)(!m_userTask.ViewCompleteTask(FunctionOpen.iOpenTask)&&FunctionOpen.iOpenTask != 0);
			if(fo.iTaskID != 0 || mvd.stGrade.iLevel < FunctionOpen.iOpenGrade)
			{
				fo.iType=FunctionOpen.iOpenType;
				fo.iTaskID=(fo.iTaskID != 0?FunctionOpen.iOpenTask:0);
				fo.iGrade=FunctionOpen.iOpenGrade;
				memcpy(pBuf+len,&fo,sizeof(fo));
				len+=sizeof(fo);
			}
		}
	}
	return CT_FALSE;
}

// 得到宠物数据
CT_BOOL CCUser::GetPetData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
    // TODO:
    return CT_FALSE;
}
// End [9/18/2012 John]

// Begin [10/22/2012 wangchen]
// 得到副本数据
CT_BOOL CCUser::GetDuplicateData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	return CT_FALSE;
}
// End [10/22/2012 wangchen]

CT_VOID CCUser::CloseNet()
{
#ifndef _TEST_CLOSE_NET
	 _AUTOMUTEX(&m_mutex);
	m_pNetInterface->Close();
#endif 
}

CT_BOOL  CCUser::GetPTRLinkUserData(PTRLinkUserData &linkUserData)
{
	 _AUTOMUTEX(&m_mutex);
	linkUserData.pMd = &m_userBase.m_masterData;
	linkUserData.pTalentMap = &m_userTalent.m_DataMap;
	linkUserData.pItemMap = &m_userItem.m_DataMap;
	linkUserData.pEquipMap = &m_userEquip.m_DataMap;
	linkUserData.pVipGrade = &m_userVip.m_VipGrade;
	linkUserData.pEnchantMap = &m_userEnchant.m_DataMap;
	linkUserData.pFlagMap = &m_userBattleFlag.m_DataMap;
	linkUserData.pElmMap = &m_userElmPower.m_DataMap;
	linkUserData.pElmAttrMap = &m_userElmAttr.m_DataMap;
	linkUserData.pBuffMap = &m_userBuff.m_DataMap;
	linkUserData.pArray = m_userBattleArray.m_iPosition;
	linkUserData.pTowerMap = &m_userTower.m_AwardStatus;
	linkUserData.pArmyMap = &m_userArmy.m_DataMap;
	linkUserData.pSkillMap = &m_userSkill.m_DataMap;
	linkUserData.pScienceMap = &m_userFightTeamNember.m_DataMap;
	linkUserData.pEnchatRewardVector = &m_userEnchant.m_EnchantRewardVector;
	linkUserData.pTitleMap = &m_userTitle.m_DataMap;
	linkUserData.pPetInfo = &m_userPet.m_PetInfo;
	linkUserData.pSeekArray = m_userSeek.m_iPos;
	linkUserData.pWallAdditionMap = &m_userSeek.m_WallAdditionMap;
	linkUserData.pMasterFightAddMap = &m_userBase.m_MasterFightAddMap;
	linkUserData.pcmap = &m_userPet.m_userPetInfoMap;

	return CT_TRUE;
}

ENUM_LootItem CCUser::LootItems(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], const CT_INT32 itemTypes, ITEM_OP_MODE enMode /*= ITEM_OP_MODE_CKEXE*/ )
{
    struct ItemSt 
    {
        CT_INT32 iIDs[GOODS_COUNT];
        CT_INT32 iCounts[GOODS_COUNT];
        CT_INT32 nCnt;
    };
    ItemSt itemAdd = {0};
    ItemSt itemExpend = {0};
    ItemSt elmAdd = {0};
    ItemSt elmExpend = {0};

    // 分类: 背包和元素包
    for (CT_INT32 i = 0; i < itemTypes; ++i)
    {
        G_ItemBase itBase = {0};
        if (0 != G_GetItemBase(iItemIDs[i], &itBase))
        {            
            if (itBase.iTID == ITEM_TYPE_ELMMARROW || itBase.iTID == ITEM_TYPE_ELMRUBBLE)
            {
                if (Counts[i] > 0) 
                {
                    elmAdd.iIDs[elmAdd.nCnt] = iItemIDs[i];
                    elmAdd.iCounts[elmAdd.nCnt] = Counts[i];
                    elmAdd.nCnt++;
                }
                else if (Counts[i] < 0)
                {
                    elmExpend.iIDs[elmExpend.nCnt] = iItemIDs[i];
                    elmExpend.iCounts[elmExpend.nCnt] = -Counts[i];
                    elmExpend.nCnt++;
                }
            }
            else
            {
                if (Counts[i] > 0) 
                {
                    itemAdd.iIDs[itemAdd.nCnt] = iItemIDs[i];
                    itemAdd.iCounts[itemAdd.nCnt] = Counts[i];
                    itemAdd.nCnt++;
                }
                else if (Counts[i] < 0)
                {
                    itemExpend.iIDs[itemExpend.nCnt] = iItemIDs[i];
                    itemExpend.iCounts[itemExpend.nCnt] = -Counts[i];
                    itemExpend.nCnt++;
                }
            }
        }
    }

    ENUM_LootItem enLoot = LOOT_SUC;
    if (enMode & ITEM_OP_MODE_CHECK) {
        if (!m_userItem.AutoAddItemData(saveTag, itemAdd.iIDs, itemAdd.iCounts, itemAdd.nCnt, ITEM_OP_MODE_CHECK)) {
            enLoot = LOOT_BAG_FULL;
            goto LOOT_FAIL;
        }
        if (!m_userElmPower.AutoAddItemData(saveTag, elmAdd.iIDs, elmAdd.iCounts, elmAdd.nCnt, ITEM_OP_MODE_CHECK)) {
            enLoot = LOOT_ELM_FULL;
            goto LOOT_FAIL;
        }
        if (!m_userItem.AutoExpendItem(saveTag, itemExpend.iIDs, itemExpend.iCounts, itemExpend.nCnt, ITEM_OP_MODE_CHECK)) {
            enLoot = LOOT_NO_ITEM;
            goto LOOT_FAIL;
        }
        if (!m_userElmPower.AutoExpendItem(saveTag, elmExpend.iIDs, elmExpend.iCounts, elmExpend.nCnt, ITEM_OP_MODE_CHECK)) {
            enLoot = LOOT_NO_ELM;
            goto LOOT_FAIL;
        }        
    }
	if (enMode & ITEM_OP_MODE_EXEC) {
		m_userItem.AutoAddItemData(saveTag, itemAdd.iIDs, itemAdd.iCounts, itemAdd.nCnt, ITEM_OP_MODE_EXEC);
		m_userElmPower.AutoAddItemData(saveTag, elmAdd.iIDs, elmAdd.iCounts, elmAdd.nCnt, ITEM_OP_MODE_EXEC);
		m_userItem.AutoExpendItem(saveTag, itemExpend.iIDs, itemExpend.iCounts, itemExpend.nCnt, ITEM_OP_MODE_EXEC);
		m_userElmPower.AutoExpendItem(saveTag, elmExpend.iIDs, elmExpend.iCounts, elmExpend.nCnt, ITEM_OP_MODE_EXEC);
	}

LOOT_FAIL:
    return enLoot;
}

// 根据领主的属性数据，计算领主的战斗属性，返回战斗力数值
CT_INT32 CCUser::CalcUserFinalCombatStatus(MasterCombatStatus &mcs, const PTRLinkUserData &linkUserData,FightingPower &Power)
{
    CT_INT32 fightingPower = 0;
    mcs.fHPPersent = CCUserBase::RawHPPencent;

    // 基本属性：职业/等级/军衔
    CCUserBase::UpdateBattleStatus(mcs, linkUserData.pMd->iOccupation, linkUserData.pMd->mvd.stGrade.iLevel,
        linkUserData.pMd->mvd.stRank.iLevel);
    // 潜能/潜能组
    {
		CT_INT32 arrayPos = 0;
		for (CT_INT32 i = 0; i < POST_COUNT / 2; ++i)
		{
			if (linkUserData.pArray[i] == linkUserData.pMd->iOccupation) 
			{
				if (i < 2) {
					arrayPos = UNIT_TYPE_ARRAY_FRONT;
				} else if (i == 2) {
					arrayPos = UNIT_TYPE_ARRAY_MIDDLE;
				} else {
					arrayPos = UNIT_TYPE_ARRAY_BACK;
				}
				
			}
		}
        CT_INT32 arrayTargets[] = {UNIT_TYPE_PLAYER, GET_TARGET_TYPE(linkUserData.pMd->iOccupation), arrayPos, UNIT_TYPE_ARRAY_ALL};
        CCUserTalent::UpdateBattleStatus(mcs, arrayTargets, sizeof(arrayTargets) / sizeof(*arrayTargets), *linkUserData.pTalentMap);
    }    
    // 装备
    CCUserEquipment::UpdateBattleStatus(mcs, *linkUserData.pItemMap, *linkUserData.pEquipMap);
    // VIP属性加成
    CCUserVip::UpdateBattleStatus(mcs, *linkUserData.pVipGrade);
    // 附魔属性加成
    CCUserEnchant::UpdateBattleStatus(mcs, *linkUserData.pEnchantMap, *linkUserData.pEnchatRewardVector);
	//元素塔永久奖励加成
	CCUserTower::UpdateBattleStatus(mcs, *linkUserData.pTowerMap);
   
    // 战队科技属性 - 玩家在战队中才生效
    {
        CT_INT32 arrayTargets[] = {UNIT_TYPE_PLAYER, GET_TARGET_TYPE(linkUserData.pMd->iOccupation)};
        CCUserFightTeamNember::UpdateBattleStatus(mcs, linkUserData.pMd->dwFTID,linkUserData.pMd->dwUserID,arrayTargets, sizeof(arrayTargets) / sizeof(*arrayTargets), *linkUserData.pScienceMap);
    }
    // 增/减益属性
    CCUserBuff::UpdateBattleStatus(mcs, *linkUserData.pBuffMap);
    // 阵法加成
    CCUserBattleArray::UpdateBattleStatus(mcs, linkUserData.pMd->iOccupation, linkUserData.pArray, POST_COUNT / 2);
	// 头衔加成
	CCUserTitle::UpdateBattleStatus(mcs, *linkUserData.pTitleMap);
	//宠物加成
	CT_INT32 iPetf=CCUserPet::UpdateBattleStatus(mcs,*linkUserData.pPetInfo,*linkUserData.pVipGrade,linkUserData.pMd->iOccupation,*linkUserData.pcmap);
	//战斗力加成
	for(CCUserBase::MasterFightAddMap::iterator it=linkUserData.pMasterFightAddMap->begin();it != linkUserData.pMasterFightAddMap->end();++it)
	{
		if(linkUserData.pMd->iOccupation == (*it).second.iType)
		{
			mcs.iMaxHP         += (*it).second.mcs.iMaxHP;        
			mcs.iMaxMP         += (*it).second.mcs.iMaxMP;        
			mcs.iStrength      += (*it).second.mcs.iStrength;    
			mcs.iAgility       += (*it).second.mcs.iAgility;      
			mcs.iIntellect     += (*it).second.mcs.iIntellect;    
			mcs.iSpirit        += (*it).second.mcs.iSpirit;       
			mcs.fHitRate       += (*it).second.mcs.fHitRate;      
			mcs.fDodgeRate     += (*it).second.mcs.fDodgeRate;    
			mcs.fCritRate      += (*it).second.mcs.fCritRate;     
			mcs.fResilient     += (*it).second.mcs.fResilient;    
			mcs.fDmgIncPercent += (*it).second.mcs.fDmgIncPercent;
			mcs.fDmgDecPercent += (*it).second.mcs.fDmgDecPercent;
			mcs.iAttack     += (*it).second.mcs.iAttack; 
			mcs.iDefence     += (*it).second.mcs.iDefence; 
			mcs.fSpeed     += (*it).second.mcs.fSpeed; 
			break;
		}
	}

	//宠物BUFF
	for(CCUserPet::PetChangeMap::iterator it=linkUserData.pcmap->begin();it != linkUserData.pcmap->end();++it)
	{
		for(CT_INT32 i=VALUE_TYPE_HEALTH;i<VALUE_TYPE_SPEED;++i)
		{
			G_PetBuffer pb={0};
			if(G_GetPetBuffer((*it).second.iChangeID,i,&pb))
			{
				switch (pb.iType) 
				{
				case VALUE_TYPE_HEALTH:         mcs.iMaxHP += pb.fValue; break;
				case VALUE_TYPE_HPPENCENT:      mcs.fHPPersent += pb.fValue; break;
				case VALUE_TYPE_MANA:           mcs.iMaxMP += pb.fValue; break;
				case VALUE_TYPE_ATTACK:         mcs.iAttack += pb.fValue; break;
				case VALUE_TYPE_DEFENCE:        mcs.iDefence += pb.fValue; break;
				case VALUE_TYPE_STRENTH:        mcs.iStrength += pb.fValue; break;
				case VALUE_TYPE_AGILITY:        mcs.iAgility += pb.fValue; break;
				case VALUE_TYPE_INTELLENT:      mcs.iIntellect += pb.fValue; break;
				case VALUE_TYPE_SPIRIT:         mcs.iSpirit += pb.fValue; break;
				case VALUE_TYPE_HITRATE:        mcs.fHitRate += pb.fValue; break;
				case VALUE_TYPE_DODGERATE:      mcs.fDodgeRate += pb.fValue; break;
				case VALUE_TYPE_CRITRATE:       mcs.fCritRate += pb.fValue; break;
				case VALUE_TYPE_RESILIENT:      mcs.fResilient += pb.fValue; break;
				case VALUE_TYPE_DMGINCPERCENT:  mcs.fDmgIncPercent += pb.fValue; break;
				case VALUE_TYPE_DMGDECPERCENT:  mcs.fDmgDecPercent += pb.fValue; break;
				case VALUE_TYPE_EQUIPPERCENT:   mcs.fEquipPercent += pb.fValue;break;
				case VALUE_TYPE_SPEED:			mcs.fSpeed += pb.fValue;break;
				}
			}
		}
	}

    //战斗力计算 - 领主    
    CCUserBase::LimiteBattleStatus(mcs);
    fightingPower = G_GetFightingPower(mcs.iMaxHP,mcs.iMaxMP + mcs.iMPRecover,
        mcs.iAttack,mcs.iDefence,mcs.iStrength, mcs.iAgility,mcs.iIntellect,mcs.iSpirit,
        mcs.fHitRate,mcs.fDodgeRate,mcs.fCritRate,mcs.fResilient,
        mcs.fDmgIncPercent,mcs.fDmgDecPercent,mcs.fHPPersent - CCUserBase::RawHPPencent,0);//最后一个0为装备能力加成百分比，暂时不用

	Power.iMasterFightingPower=fightingPower;

    // 战旗属性加成
    {
        BattleFlag flag = {0};
        CCUserBattleFlag::DataMap_IT it = linkUserData.pFlagMap->find(GET_FLAG_TYPE(linkUserData.pMd->iOccupation));
        if (it != linkUserData.pFlagMap->end()) 
        {
            flag = it->second;
            MasterCombatStatus bfmcs={0};
            Power.iFightingFlagPower=CCUserBattleFlag::UpdateBattleStatus(mcs, flag, *linkUserData.pElmMap, *linkUserData.pElmAttrMap);
        }
    } 

	Power.iMasterFightingPower += Power.iFightingFlagPower;
    //总战斗力计算 - 出阵军队
    for (CT_INT32 i = 0; i < POST_COUNT / 2; ++i) {
        MasterCombatStatus mcsArmy = {0};
        ArmyInfo army = {0};
        CCUserArmy::DataMap_IT itArmy = linkUserData.pArmyMap->find(linkUserData.pArray[i]);
        if (itArmy != linkUserData.pArmyMap->end()) 
		{
			Power.iArmyFightingPower+=CalcArmyFinalCombatStatus(mcsArmy, linkUserData, itArmy->second);
        }
    }
	
	Power.iAllFightingPower=Power.iArmyFightingPower+Power.iMasterFightingPower;
   

    CCUserBase::LimiteBattleStatus(mcs);

    return fightingPower;
}

// 根据领主的属性数据，计算指定的军队的战斗属性，返回战斗力数值
CT_INT32 CCUser::CalcArmyFinalCombatStatus(MasterCombatStatus &mcs, const PTRLinkUserData &linkUserData, const ArmyInfo &army)
{
    // 基础数据
    CCUserArmy::UpdateBattleStatus(mcs, army.iArmyID, army.iArmyGrade);
    // 称号影响
    CCUserBase::UpdateBattleStatus(mcs, 0, 0, linkUserData.pMd->mvd.stRank.iLevel); 
    // 潜能影响
    {
		CT_INT32 arrayPos = 0;
		for (CT_INT32 i = 0; i < POST_COUNT / 2; ++i)
		{
			if (linkUserData.pArray[i] == army.iArmyID) 
			{
				if (i < 2) {
					arrayPos = UNIT_TYPE_ARRAY_FRONT;
				} else if (i == 2) {
					arrayPos = UNIT_TYPE_ARRAY_MIDDLE;
				} else {
					arrayPos = UNIT_TYPE_ARRAY_BACK;
				}

			}
		}
        CT_INT32 arrayTargets[] = {UNIT_ALL_ARMY, GET_TARGET_TYPE(army.iArmyID), army.iArmyID, arrayPos, UNIT_TYPE_ARRAY_ALL};
        CCUserTalent::UpdateBattleStatus(mcs, arrayTargets, sizeof(arrayTargets) / sizeof(*arrayTargets), *linkUserData.pTalentMap);
    }    
        
	// 战队科技影响 - 玩家在战队中才生效
	{
		CT_INT32 arrayTargets[] = {UNIT_ALL_ARMY, GET_TARGET_TYPE(army.iArmyID)};
		CCUserFightTeamNember::UpdateBattleStatus(mcs, linkUserData.pMd->dwFTID,linkUserData.pMd->dwUserID,arrayTargets, sizeof(arrayTargets) / sizeof(*arrayTargets), *linkUserData.pScienceMap);
	}

	// 阵法加成
    CCUserBattleArray::UpdateBattleStatus(mcs, army.iArmyID, linkUserData.pArray, POST_COUNT / 2);
	// 头衔加成
	CCUserTitle::UpdateBattleStatus(mcs, *linkUserData.pTitleMap);

	//宠物加成
	CCUserPet::UpdateBattleStatus(mcs,*linkUserData.pPetInfo,*linkUserData.pVipGrade,GET_TARGET_TYPE(army.iArmyID),*linkUserData.pcmap);

	//宠物装备加成
	CCUserPet::UpdateBattleStatus(mcs, *linkUserData.pItemMap, *linkUserData.pEquipMap);

	//战斗力加成
	for(CCUserBase::MasterFightAddMap::iterator it=linkUserData.pMasterFightAddMap->begin();it != linkUserData.pMasterFightAddMap->end();++it)
	{
		if(GET_TARGET_TYPE(army.iArmyID) == (*it).second.iType)
		{
			mcs.iMaxHP         += (*it).second.mcs.iMaxHP;        
			mcs.iMaxMP         += (*it).second.mcs.iMaxMP;        
			mcs.iStrength      += (*it).second.mcs.iStrength;    
			mcs.iAgility       += (*it).second.mcs.iAgility;      
			mcs.iIntellect     += (*it).second.mcs.iIntellect;    
			mcs.iSpirit        += (*it).second.mcs.iSpirit;       
			mcs.fHitRate       += (*it).second.mcs.fHitRate;      
			mcs.fDodgeRate     += (*it).second.mcs.fDodgeRate;    
			mcs.fCritRate      += (*it).second.mcs.fCritRate;     
			mcs.fResilient     += (*it).second.mcs.fResilient;    
			mcs.fDmgIncPercent += (*it).second.mcs.fDmgIncPercent;
			mcs.fDmgDecPercent += (*it).second.mcs.fDmgDecPercent;
			mcs.iAttack     += (*it).second.mcs.iAttack; 
			mcs.iDefence     += (*it).second.mcs.iDefence; 
			mcs.fSpeed     += (*it).second.mcs.fSpeed; 
			break;
		}
	}

	//宠物BUFF
	for(CCUserPet::PetChangeMap::iterator it=linkUserData.pcmap->begin();it != linkUserData.pcmap->end();++it)
	{
		for(CT_INT32 i=VALUE_TYPE_HEALTH;i<VALUE_TYPE_SPEED;++i)
		{
			G_PetBuffer pb={0};
			if(G_GetPetBuffer((*it).second.iChangeID,VALUE_TYPE_HEALTH,&pb))
			{
				switch (pb.iType) 
				{
				case VALUE_TYPE_HEALTH:         mcs.iMaxHP += mcs.iMaxHP*pb.fValue; break;
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

    //战斗力计算   
    CCUserBase::LimiteBattleStatus(mcs);
    CT_INT32 fightingPower = G_GetFightingPower(mcs.iMaxHP,mcs.iMaxMP + mcs.iMPRecover,
        mcs.iAttack,mcs.iDefence,mcs.iStrength, mcs.iAgility,mcs.iIntellect,mcs.iSpirit,
        mcs.fHitRate,mcs.fDodgeRate,mcs.fCritRate,mcs.fResilient,
        mcs.fDmgIncPercent,mcs.fDmgDecPercent,mcs.fHPPersent,0);//最后一个为装备能力加成百分比，暂时不用

    // 战旗影响
    {
        BattleFlag flag = {0};
        CCUserBattleFlag::DataMap_IT it = linkUserData.pFlagMap->find(GET_FLAG_TYPE(army.iArmyID));
        if (it != linkUserData.pFlagMap->end()) {
            flag = it->second;
            fightingPower += CCUserBattleFlag::UpdateBattleStatus(mcs, flag, *linkUserData.pElmMap, *linkUserData.pElmAttrMap);
        }
    }

    CCUserBase::LimiteBattleStatus(mcs);

    return fightingPower;
}

CT_VOID CCUser::GetFightingStatusData(UserFightingStatus &data, const PTRLinkUserData &linkUserData,CT_BOOL bSeekFight/*=CT_FALSE*/)
{
    memset(&data, 0, sizeof(data));
    data.dwUserID = linkUserData.pMd->dwUserID;
	CT_INT32 iPos[POST_COUNT/2]={0};
	if(bSeekFight)
	{
		memcpy(iPos,linkUserData.pSeekArray,sizeof(iPos));
	}
	else
	{
		memcpy(iPos,linkUserData.pArray,sizeof(iPos));
	}

    for (CT_INT32 i = 0; i < POST_COUNT / 2; ++i) {
        if (iPos[i] != 0) {
			data.units[i].type = (bSeekFight?linkUserData.pSeekArray[i]:linkUserData.pArray[i]);
            data.units[i].position = i;
            if (linkUserData.pMd->iOccupation == data.units[i].type) 
			{             
                // 领主
				FightingPower Power={0};
				data.units[i].iPetChangeID=linkUserData.pPetInfo->iChangeID;
                data.units[i].level = linkUserData.pMd->mvd.stGrade.iLevel;
                memcpy(data.units[i].name, linkUserData.pMd->cNickName, sizeof(data.units[i].name));
                CalcUserFinalCombatStatus(data.units[i].mcs, linkUserData,Power);   // 刷新领主战斗属性
                data.units[i].Skills[0].iID = (linkUserData.pMd->iOccupation - 1) * 20 + 1; // 普通攻击
				data.units[i].PetSkill.iID = linkUserData.pPetInfo->iSkillID[0];
                for (CCUserSkill::DataMap_IT it = linkUserData.pSkillMap->begin(); it != linkUserData.pSkillMap->end(); ++it) 
				{
					if(it->second.Used == 0)continue;
                    data.units[i].Skills[it->second.Used].iID = it->second.iID; // 领主的技能
                }

				CT_INT32 iNum=0;
				for(CCUserItem::DataMap::iterator it=linkUserData.pElmMap->begin();it != linkUserData.pElmMap->end();++it)
				{
					if(!IS_ELMIDX_EQUIPPED((*it).second.iIndex) || GET_FLAG_BY_INDEX((*it).second.iIndex) != linkUserData.pMd->iOccupation)
						continue;

					G_ItemBase ib={0};
					CT_INT32 iSkillID[2]={0};
					CCUserElmAttr::DataMap::iterator eait=linkUserData.pElmAttrMap->find((*it).second.key);
					if(eait == linkUserData.pElmAttrMap->end())
						continue;
					if(!G_GetItemBase((*it).second.iID,&ib))
						continue;
					if(!G_GetEquipSkill(ib.iEquipSkill,(*eait).second.iLevel,&iSkillID[0]))
						continue;
					G_GetEquipSkill(ib.iBuyPrice,(*eait).second.iLevel,&iSkillID[1]);

					for(CT_INT32 j=0;j<2;++j)
					{
						if(iSkillID[j] == 0)continue;
						data.units[i].SkillEnuip.iSkillEquip[iNum]=iSkillID[j];

						if(iNum < SKILL_ENUIP_COUNT-1)
							iNum++;
					}
				}
            } else {
                // 士兵
                ArmyInfo army = {0};
                G_Army armyData = {0};
                G_ArmyFighting armyFighting = {0};
                CCUserArmy::AI_IT it = linkUserData.pArmyMap->find(data.units[i].type);
                if (it != linkUserData.pArmyMap->end()) {
                    army = it->second;
                    G_GetArmy(army.iArmyID, &armyData);
                    data.units[i].level = army.iArmyGrade;
                    memcpy(data.units[i].name, armyData.cArmyName, sizeof(data.units[i].name));
                    CalcArmyFinalCombatStatus(data.units[i].mcs, linkUserData, army);
                    G_GetArmyFighting(army.iArmyID, army.iArmyGrade, &armyFighting);
                    data.units[i].Skills[0].iID = armyFighting.iSkill1;
                    data.units[i].Skills[1].iID = armyFighting.iSkill2;
                    data.units[i].Skills[2].iID = armyFighting.iSkill3;
                    data.units[i].Skills[3].iID = armyFighting.iSkill4;
					if(GET_TARGET_TYPE(data.units[i].type) == 102 && bSeekFight)
					{
						data.units[i].iPetChangeID=linkUserData.pPetInfo->iChangeID;
						data.units[i].PetSkill.iID = linkUserData.pPetInfo->iSkillID[0];
					}

					CT_INT32 iNum=0;
					for(CCUserItem::DataMap::iterator it=linkUserData.pElmMap->begin();it != linkUserData.pElmMap->end();++it)
					{
						if(!IS_ELMIDX_EQUIPPED((*it).second.iIndex) || GET_FLAG_BY_INDEX((*it).second.iIndex) != data.units[i].type/10)
							continue;

						G_ItemBase ib={0};
						
						CT_INT32 iSkillID[2]={0};
						CCUserElmAttr::DataMap::iterator eait=linkUserData.pElmAttrMap->find((*it).second.key);
						if(eait == linkUserData.pElmAttrMap->end())
							continue;
						if(!G_GetItemBase((*it).second.iID,&ib))
							continue;
						if(!G_GetEquipSkill(ib.iEquipSkill,(*eait).second.iLevel,&iSkillID[0]))
							continue;
						G_GetEquipSkill(ib.iBuyPrice,(*eait).second.iLevel,&iSkillID[1]);

						if(bSeekFight)
						{
							for(WallAdditionMap::iterator wait=linkUserData.pWallAdditionMap->begin();wait != linkUserData.pWallAdditionMap->end();++wait)
							{
								G_SeekWallAddition swa={0};
								if(G_GetSeekWallAddition((*wait).second.iID,(*wait).second.iGrade,&swa))
								{
									G_AddFightAttribute(data.units[i].mcs,swa.iAddType,0,swa.fAddValue);
								}
							}
						}

						for(CT_INT32 j=0;j<2;++j)
						{
							if(iSkillID[j] == 0)continue;
							data.units[i].SkillEnuip.iSkillEquip[iNum]=iSkillID[j];

							if(iNum < SKILL_ENUIP_COUNT-1)
								iNum++;
						}
					}
                }
            }
        }
    }
}