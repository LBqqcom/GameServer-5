#include <ctime>
#include "CUserBase.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CThreadMes.h"
#include "CMasterScriptSetting.h"
#include "CBuildingScriptSetting.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "CBroadcastScriptSetting.h"
#include "CActive.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"

CCUserBase::CCUserBase() 
{
	memset(&m_masterData, 0, sizeof(MasterData));
	m_llLastSysTime = time(0);
}

CCUserBase::~CCUserBase(void)
{
	memset(&m_masterData, 0, sizeof(MasterData));
}

//规范数值范围防止数值出界
CT_VOID CCUserBase::CheckValueRand(CT_INT32 tagValue, CT_INT32& value)
{
	G_ValueRange gvr = {0};
	if(!G_GetValueRange(tagValue, &gvr))
	{
		CCLogMeg::getSingleton().WriteError("CheckValueRand: fail to find tag in G_GetValueRange(), id=%i", tagValue);
		return;
	}

	if (value < gvr.fMinVal)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBase::CheckValueRand tagValue:%d fMinVal:%d",tagValue,value);
		value = (CT_INT32)gvr.fMinVal;
	}

	if (value > gvr.fMaxVal)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBase::CheckValueRand tagValue:%d fManVal:%d",tagValue,value);
		value = (CT_INT32)gvr.fMaxVal;
	}
}

//规范数值范围防止数值出界
CT_VOID CCUserBase::CheckValueRand(CT_INT32 tagValue, CT_FLOAT& value)
{
    G_ValueRange gvr = {0};
    if(!G_GetValueRange(tagValue, &gvr))
    {
        CCLogMeg::getSingleton().WriteError("CheckValueRand: fail to find tag in G_GetValueRange(), id=%i", tagValue);
        return;
    }

    if (value < gvr.fMinVal)
    {
		CCLogMeg::getSingleton().WriteError("CCUserBase::CheckValueRand tagValue:%d fMinVal:%f",tagValue,value);
        value = gvr.fMinVal;
    }

    if (value > gvr.fMaxVal)
    {
		CCLogMeg::getSingleton().WriteError("CCUserBase::CheckValueRand tagValue:%d fManVal:%f",tagValue,value);
        value = gvr.fMaxVal;
    }

}


// 同步到数据库 领主升级，军衔升级，文明升级，钻石，潜能
void CCUserBase::UpdateToDatabase(CT_INT32 saveTag)
{

#ifdef _DEBUG   
    MSC_DB_MasterSaveData msd = {0};
    GetMasterSaveData(msd);
    msd.iSaveTag = saveTag;

    CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
    G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));

	CT_INT32 logThreadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));

#else
	

	if(m_masterData.mvd.stRank.iLevel != m_masterData.primeMvd.stRank.iLevel  ||
	   m_masterData.mvd.stCivili.iLevel != m_masterData.primeMvd.stCivili.iLevel || 
	   m_masterData.mvd.stGrade.iLevel != m_masterData.primeMvd.stGrade.iLevel  ||
	
	   m_masterData.mvd.stRes.iDiamod != m_masterData.primeMvd.stRes.iDiamod  ||
	   m_masterData.mvd.stRes.iTalentNum != m_masterData.primeMvd.stRes.iTalentNum  ||
	   m_masterData.mvd.stRes.iBagSlots  != m_masterData.primeMvd.stRes.iBagSlots ||
	   m_masterData.mvd.pvpData.iFightTimes != m_masterData.primeMvd.pvpData.iFightTimes)
	{
		MSC_DB_MasterSaveData msd = {0};
		GetMasterSaveData(msd);
		msd.iSaveTag = saveTag;
		CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));

		CT_INT32 logThreadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));
	}

#endif
}

CT_VOID CCUserBase::SetMastData(const MasterData& md)
{
	_AUTOMUTEX(m_pMutex);   
	memcpy(&m_masterData, &md, sizeof(md));
	//m_pUser->UpdateFactorData();
}

//设置战斗加成数据
CT_VOID CCUserBase::SetMasterFightAdd(const MasterFightAdd &mfa)
{
	_AUTOMUTEX(m_pMutex);   
	m_MasterFightAddMap.insert(MasterFightAddMap::value_type(mfa.iType,mfa));
	return;
}

//设置可变数据数据 
CT_VOID CCUserBase::SetVariableData(MasterVariableData& desMastData, const MasterVariableData& addValue)
{
	desMastData.stRes.iCurrentHP   += addValue.stRes.iCurrentHP;
	if (desMastData.stRes.iCurrentHP > m_masterData.mcs.iMaxHP)
	{
		desMastData.stRes.iCurrentHP = m_masterData.mcs.iMaxHP;
	}

	desMastData.stRes.iCurrentMP   += addValue.stRes.iCurrentMP;
	if (desMastData.stRes.iCurrentMP > m_masterData.mcs.iMaxMP)
	{
		desMastData.stRes.iCurrentMP = m_masterData.mcs.iMaxMP;
	}

	desMastData.stRes.iDiamod      += addValue.stRes.iDiamod;
	CheckValueRand(VALUE_TYPE_DIAMOND, desMastData.stRes.iDiamod);
	//判断充值和消耗
	if (addValue.stRes.iDiamod<0)
	{
		SetMastDataEx(addValue.stRes.iDiamod*-1, SAVETAGEX_COSTDIAMOD);
		CCActive::getSingleton().ActiveProcess(ACTIVE_CONSUME_ADDUP,-addValue.stRes.iDiamod,m_pUser);
		CCActive::getSingleton().ActiveProcess(ACTIVE_CONSUME_ONCE,-addValue.stRes.iDiamod,m_pUser);
	}

	desMastData.stRes.iFood        += addValue.stRes.iFood;
	CheckValueRand(VALUE_TYPE_FOOD, desMastData.stRes.iFood);
	//判断是否有累计食物活动
	CCActive::getSingleton().ActiveProcess(ACTIVE_FOOD_ADDUP,desMastData.stRes.iFood,m_pUser);

	desMastData.stRes.iGold        += addValue.stRes.iGold;
	CheckValueRand(VALUE_TYPE_GOLD, desMastData.stRes.iGold);
	//判断是否有累计金币活动
	CCActive::getSingleton().ActiveProcess(ACTIVE_GOLD_ADDUP,desMastData.stRes.iGold,m_pUser);

	desMastData.stRes.iMaxWorker   += addValue.stRes.iMaxWorker;
	CheckValueRand(VALUE_TYPE_WORKER, desMastData.stRes.iMaxWorker);


	desMastData.stRes.iStamina     += addValue.stRes.iStamina;
	CheckValueRand(VALUE_TYPE_STAMINA, desMastData.stRes.iStamina);

	desMastData.stRes.iWorker      += addValue.stRes.iWorker;
	if (desMastData.stRes.iWorker > desMastData.stRes.iMaxWorker)
	{
		desMastData.stRes.iWorker = desMastData.stRes.iMaxWorker;
	}

	desMastData.stRes.iTalentNum   += addValue.stRes.iTalentNum;
	CheckValueRand(VALUE_TYPE_TALENTNUM, desMastData.stRes.iTalentNum);

    desMastData.stRes.iSkillSoul   += addValue.stRes.iSkillSoul;
    desMastData.stRes.iSkillSlots  += addValue.stRes.iSkillSlots;

	desMastData.stGrade.iCurrentExp += addValue.stGrade.iCurrentExp;

	desMastData.stGrade.iLevel      += (addValue.stGrade.iLevel >= 1 && addValue.stGrade.iCurrentExp == 0?0:addValue.stGrade.iLevel);
	CheckValueRand(VALUE_TYPE_MASTERLEVEL, desMastData.stGrade.iLevel);

	desMastData.stRank.iCurrentExp  += addValue.stRank.iCurrentExp;
	desMastData.stRank.iLevel       += (addValue.stRank.iLevel >= 1 && addValue.stRank.iCurrentExp == 0?0:addValue.stRank.iLevel);
	CheckValueRand(VALUE_TYPE_RANKLEVEL, desMastData.stRank.iLevel);

	desMastData.stCivili.iLevel      += (addValue.stCivili.iLevel >= 1 && addValue.stCivili.iCurrentExp?0:addValue.stCivili.iLevel);	
	CheckValueRand(VALUE_TYPE_CIVILILEVEL, desMastData.stCivili.iLevel);

	desMastData.stCivili.iCurrentExp += addValue.stCivili.iCurrentExp;

	desMastData.pvpData.iFightTimes += addValue.pvpData.iFightTimes;

	desMastData.pvpData.iVictoryTimes += addValue.pvpData.iVictoryTimes;

	desMastData.pvpData.iVictoryRate = desMastData.pvpData.iVictoryTimes/(desMastData.pvpData.iFightTimes == 0?1:desMastData.pvpData.iFightTimes);

	desMastData.pvpData.iCurrentContinuousWin += addValue.pvpData.iCurrentContinuousWin;

	desMastData.stRes.iStaminaBuyTimes+=addValue.stRes.iStaminaBuyTimes;

	desMastData.stRes.iBagSlots        += addValue.stRes.iBagSlots;

	desMastData.stRes.iTeamDuplicateFightingTimes        += addValue.stRes.iTeamDuplicateFightingTimes;

    desMastData.stRes.iElmSlots     += addValue.stRes.iElmSlots;

}

//同步领主信息
CT_VOID CCUserBase::AddMastData(MasterVariableData& mvd, CT_INT32 saveTag) 
{ 
	_AUTOMUTEX(m_pMutex);
    
    // 满级不在获得经验值(等级/文明等级/军衔)
    MasterLevelInfo cu = {0};
    if(! CCMasterScriptSetting::getSingleton().GetMasterUpgradeData (m_masterData.mvd.stGrade.iLevel + 1, cu) || cu.iCurrentExp >= 0)
    {
        mvd.stGrade.iCurrentExp = 0;
    }
    if(! CCBuildingScriptSetting::getSingleton().GetCivilUpgradeData(m_masterData.mvd.stCivili.iLevel + 1, &cu) || cu.iCurrentExp >= 0)
    {
        mvd.stCivili.iCurrentExp = 0;
    }
    if(! CCMasterScriptSetting::getSingleton().GetRankUpgradeData(m_masterData.mvd.stRank.iLevel + 1, cu) || cu.iCurrentExp >= 0)
    {
        mvd.stRank.iCurrentExp = 0;
    }


    // 同步到内存
	SetVariableData(m_masterData.mvd, mvd);
    
    // 同步到数据库
    UpdateToDatabase(saveTag);

    if (mvd.stGrade.iCurrentExp != 0)
    {
        // 检查领主能否升级，升级后数据同步到数据库和客户端
        MasterUpgrage(false);
    }
    
    if (mvd.stCivili.iCurrentExp != 0)
    {
        // 检查领主文明能否升级，升级后数据同步到数据库和客户端
        CivilUpgrage(false);
    }
    
#if 0 // 军衔为手动升级
    if (mvd.stRank.iCurrentExp != 0)
    {
        // 检查军衔升级，升级后数据同步到数据库和客户端
        RankUpgrage(false);
    }
#endif
}

//增加战斗属性
CT_VOID CCUserBase::AddMastFightData(const CT_INT32 iType,MasterCombatStatus& mcs, CT_INT32 saveTag)
{
	_AUTOMUTEX(m_pMutex);
	MasterFightAddMap::iterator it=m_MasterFightAddMap.find(iType);
	if(it != m_MasterFightAddMap.end())
	{
		(*it).second.mcs.iMaxHP+=mcs.iMaxHP;			    // 生命值上限
		(*it).second.mcs.iMaxMP+=mcs.iMaxMP;			    // 法力值上限
		(*it).second.mcs.iStrength+=mcs.iStrength;             // 力量
		(*it).second.mcs.iAgility+=mcs.iAgility;              // 敏捷
		(*it).second.mcs.iIntellect+=mcs.iIntellect;            // 智力
		(*it).second.mcs.iSpirit+=mcs.iSpirit;               // 精神
		(*it).second.mcs.fHitRate+=mcs.fHitRate;              // 命中率
		(*it).second.mcs.fDodgeRate+=mcs.fDodgeRate;            // 闪避率
		(*it).second.mcs.fCritRate+=mcs.fCritRate;             // 暴击率
		(*it).second.mcs.fResilient+=mcs.fResilient;            // 任性
		(*it).second.mcs.fDmgIncPercent+=mcs.fDmgIncPercent;        // 造成伤害增加的百分比
		(*it).second.mcs.fDmgDecPercent+=mcs.fDmgDecPercent;        // 受到伤害减少的百分比
		(*it).second.mcs.iAttack+=mcs.iAttack;               // 攻击力
		(*it).second.mcs.iDefence+=mcs.iDefence;              // 防御力
		(*it).second.mcs.fSpeed+=mcs.fSpeed;				//速度
	}
	else
	{
		MasterFightAdd mfa={0};
		mfa.dwUserID=m_pUser->GetUserID();
		mfa.iType=iType;
		memcpy(&mfa.mcs,&mcs,sizeof(mcs));
		m_MasterFightAddMap.insert(MasterFightAddMap::value_type(iType,mfa));
	}

	//写入数据库
	MSC_DB_MasterFightSaveData mfsd={0};
	mfsd.dwUserID=m_pUser->GetUserID();
	mfsd.iType=iType;
	memcpy(&mfsd.mcs,&mcs,sizeof(mcs));
	G_SendDBThreadInfo(m_pUser->GetUserID(),MSC_MAIN_DB_SAVE_DATA,MSC_SUB_DB_MASTER_FIGHT_DATA,(CT_CHAR*)&mfsd,sizeof(mfsd));
	return;
}

CT_VOID CCUserBase::SetMastDataEx(CT_INT64 iValue, CT_INT32 saveTagEx, CT_BOOL bDBSave)
{
	_AUTOMUTEX(m_pMutex);
	//switch (saveTagEx)
	//{
	//default:
	//	{
	//		CCLogMeg::getSingleton().WriteError("CCUserBase::AddMastDataEx saveTagEx: %i", saveTagEx);
	//		break;
	//	}
	//}
	if (bDBSave)
	{
		MSC_DB_MasterSaveDataEx msde = {0};
		msde.dwUserID = m_pUser->GetUserID();
		msde.iSaveTagEx = saveTagEx;
		msde.iInt64Value = iValue;
		CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_DATA_EX, (CT_CHAR*)&msde, sizeof(msde));
	}
}
CT_VOID CCUserBase::SetMastDataEx(CT_INT32 iValue, CT_INT32 saveTagEx, CT_BOOL bDBSave)
{
	_AUTOMUTEX(m_pMutex);
	switch (saveTagEx)
	{
	case SAVETAGEX_TOWER_HELP:
		{
			m_masterData.mex.iTowerHelpTime = iValue;
			break;
		}
	case SAVETAGEX_CHARGEDIAMOD:
		{
			m_masterData.mex.iChargeDiamod += iValue;
			break;
		}
	case SAVETAGEX_COSTDIAMOD:
		{
			m_masterData.mex.iCostDiamod += iValue;
			break;
		}
	case SAVETAGEX_TOWER_TIME:
		{
			m_masterData.mex.fpd.iFPTowerTime=iValue;
			break;
		}
	case SAVETAGEEX_WORSHIP_TIME0:
	case SAVETAGEEX_WORSHIP_TIME1:
	case SAVETAGEEX_WORSHIP_TIME2:
	case SAVETAGEEX_WORSHIP_TIME3:
	case SAVETAGEEX_WORSHIP_TIME4:
	case SAVETAGEEX_WORSHIP_TIME5:
		{
			CT_INT32 iTage=saveTagEx-SAVETAGEEX_WORSHIP_TIME0;
			if(iTage > 0 && iTage < WORSHIP_COUNT)
			{
				m_masterData.mex.fpd.iFPWorshipTime[iTage]=iValue;
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCUserBase::SetMastDataEx iFPWorshipTime iTage:%d",saveTagEx);
			}
			break;
		}
	case SAVETAGEEX_OPEN_CHEST:
		{
			m_masterData.mex.fpd.iFPChestTime=iValue;
			break;
		}
	case SAVETAGEEX_FINISH_CAVE:
		{
			m_masterData.mex.fpd.iCaveTime=iValue;
			break;
		}
	case SAVETAGEEX_PVP_CHEST:
		{
			m_masterData.mex.fpd.iPVPChest=iValue;
			break;
		}
	case SAVETAGEEX_TURNTABLE0:
	case SAVETAGEEX_TURNTABLE1:
	case SAVETAGEEX_TURNTABLE2:
		{
			CT_INT32 iTage=saveTagEx-SAVETAGEEX_TURNTABLE0;
			if(iTage >= 0 && iTage < TURNTABLE_COUNT)
				m_masterData.mex.fpd.iTurntable[iTage]=iValue;
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCUserBase::AddMastDataEx saveTagEx: %i", saveTagEx);
			return;
		}
	}
	if (bDBSave)
	{
		MSC_DB_MasterSaveDataEx msde = {0};
		msde.dwUserID = m_pUser->GetUserID();
		msde.iSaveTagEx = saveTagEx;
		msde.iInt32Value = iValue;
		CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_DATA_EX, (CT_CHAR*)&msde, sizeof(msde));
	}
}

//得到领主数据
CT_BOOL CCUserBase::GetMasterData(CT_CHAR* pBuf, CT_INT32& bufLen)
{
	_AUTOMUTEX(m_pMutex);  
	if (sizeof(CMD_G_MasterData) > bufLen) //缓冲区不足
	{
		return CT_FALSE;
	}

	CMD_G_MasterData  mi;
	mi.dwUserID = m_masterData.dwUserID; 
	memcpy(mi.cNickName, m_masterData.cNickName, NAME_SIZE);
	mi.iCiviliGrade =  m_masterData.mvd.stCivili.iLevel; 
	mi.iCiviliNum   =  m_masterData.mvd.stCivili.iCurrentExp;
	mi.iGrade  =  m_masterData.mvd.stGrade.iLevel;
	mi.iGradeXp=  m_masterData.mvd.stGrade.iCurrentExp;
	mi.iGold   =  m_masterData.mvd.stRes.iGold;
	mi.iDiamond =  m_masterData.mvd.stRes.iDiamod;
	mi.iMaxWorker = m_masterData.mvd.stRes.iMaxWorker;
	mi.iWorker =  m_masterData.mvd.stRes.iWorker;
	mi.iFood   =  m_masterData.mvd.stRes.iFood;

	mi.iCurrentHP = m_masterData.mvd.stRes.iCurrentHP;
	mi.iCurrentMP = m_masterData.mvd.stRes.iCurrentMP;
	mi.iRank      = m_masterData.mvd.stRank.iLevel;
	mi.iRankXp    = m_masterData.mvd.stRank.iCurrentExp;
	mi.iStamina   = m_masterData.mvd.stRes.iStamina;
	mi.iTalentNum = m_masterData.mvd.stRes.iTalentNum;
	mi.iSkillSoul = m_masterData.mvd.stRes.iSkillSoul;
	mi.iOccupation= m_masterData.iOccupation;
    mi.iBagSlot   = m_masterData.mvd.stRes.iBagSlots;
    mi.iSkillSlot = m_masterData.mvd.stRes.iSkillSlots;
    mi.iElmSlot   = m_masterData.mvd.stRes.iElmSlots;

	mi.iVictoryRate  =m_masterData.mvd.pvpData.iVictoryRate;
	mi.iFightTimes	 =m_masterData.mvd.pvpData.iFightTimes;
	mi.iRanking		 =m_masterData.mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingCur;
	mi.iVictoryTimes =m_masterData.mvd.pvpData.iVictoryTimes;
	mi.iCurrentContinuousWin=m_masterData.mvd.pvpData.iCurrentContinuousWin;

	mi.iFightingPower = m_masterData.mvd.stRes.iFightingPower;
	mi.iTeamDuplicateFightingTimes=m_masterData.mvd.stRes.iTeamDuplicateFightingTimes; 

	//mi.bTalk=m_pUser->GetTalk();
	mi.bTalk=CT_FALSE;

	mi.iMaxContinuousWin=m_masterData.mvd.topData.iMaxContinuousWin;	
	mi.iLeaveTeamTime = m_masterData.mex.iLeaveTeamTime;
	mi.iTowerHelpTime = m_masterData.mex.iTowerHelpTime;
	mi.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;

	memcpy(pBuf, &mi, sizeof(mi));
	bufLen = sizeof(mi);
	CCLogMeg::getSingleton().WriteError("CCUserBase::GetMasterData bufLen: %d", bufLen);
	return CT_TRUE;
}

//得到领主战斗属性
CT_BOOL CCUserBase::GetMasterFightAdd(CT_CHAR* pBuf, CT_INT32& bufLen)
{
	_AUTOMUTEX(m_pMutex);
	if(bufLen <= m_MasterFightAddMap.size()*sizeof(MasterFightAdd))
	{
		CCLogMeg::getSingleton().WriteWaring("CCUserBase::GetMasterFightAdd bufLen <= m_MasterFightAddMap.size()*sizeof(MasterFightAdd)");
		return CT_FALSE;
	}

	CT_INT32 iLen=0;
	for(MasterFightAddMap::iterator it=m_MasterFightAddMap.begin();it != m_MasterFightAddMap.end();++it)
	{
		memcpy(pBuf+iLen,&((*it).second),sizeof(MasterFightAdd));
		iLen+=sizeof(MasterFightAdd);
	}
	bufLen=iLen;
	return CT_TRUE;
}

//得到领主战斗属性
CT_BOOL CCUserBase::GetMasterFightAdd(CT_INT32 iType,MasterFightAdd &mfa)
{
	_AUTOMUTEX(m_pMutex);
	MasterFightAddMap::iterator it=m_MasterFightAddMap.find(iType);
	if(it != m_MasterFightAddMap.end())
	{
		memcpy(&mfa,&((*it).second),sizeof(mfa));
		return CT_TRUE;
	}

	return CT_FALSE;
}
//得到用户需要保存的可变数据
CT_VOID CCUserBase::GetMasterSaveData(MSC_DB_MasterSaveData& mvd) 
{ 
	_AUTOMUTEX(m_pMutex);
	mvd.dwUserID     = m_pUser->m_dwID;
	mvd.mvd.stCivili.iLevel      = m_masterData.mvd.stCivili.iLevel - m_masterData.primeMvd.stCivili.iLevel; 
	mvd.mvd.stCivili.iCurrentExp = m_masterData.mvd.stCivili.iCurrentExp - m_masterData.primeMvd.stCivili.iCurrentExp; 
	mvd.mvd.stGrade.iLevel       = m_masterData.mvd.stGrade.iLevel - m_masterData.primeMvd.stGrade.iLevel; 
	mvd.mvd.stGrade.iCurrentExp  = m_masterData.mvd.stGrade.iCurrentExp - m_masterData.primeMvd.stGrade.iCurrentExp; 

	mvd.mvd.stRank.iLevel        = m_masterData.mvd.stRank.iLevel - m_masterData.primeMvd.stRank.iLevel;
	mvd.mvd.stRank.iCurrentExp   = m_masterData.mvd.stRank.iCurrentExp - m_masterData.primeMvd.stRank.iCurrentExp;

	mvd.mvd.stRes.iCurrentHP     = m_masterData.mvd.stRes.iCurrentHP - m_masterData.primeMvd.stRes.iCurrentHP;
	mvd.mvd.stRes.iCurrentMP     = m_masterData.mvd.stRes.iCurrentMP - m_masterData.primeMvd.stRes.iCurrentMP;
	mvd.mvd.stRes.iDiamod        = m_masterData.mvd.stRes.iDiamod - m_masterData.primeMvd.stRes.iDiamod;

	mvd.mvd.stRes.iFood          = m_masterData.mvd.stRes.iFood - m_masterData.primeMvd.stRes.iFood;
	mvd.mvd.stRes.iGold          = m_masterData.mvd.stRes.iGold - m_masterData.primeMvd.stRes.iGold;
	mvd.mvd.stRes.iStamina       = m_masterData.mvd.stRes.iStamina - m_masterData.primeMvd.stRes.iStamina;
	mvd.mvd.stRes.iTalentNum     = m_masterData.mvd.stRes.iTalentNum - m_masterData.primeMvd.stRes.iTalentNum;
	mvd.mvd.stRes.iWorker        = m_masterData.mvd.stRes.iWorker - m_masterData.primeMvd.stRes.iWorker;
	mvd.mvd.stRes.iSkillSoul     = m_masterData.mvd.stRes.iSkillSoul - m_masterData.primeMvd.stRes.iSkillSoul;

    mvd.mvd.stRes.iBagSlots      = m_masterData.mvd.stRes.iBagSlots - m_masterData.primeMvd.stRes.iBagSlots;    
	mvd.mvd.pvpData.iFightTimes  = m_masterData.mvd.pvpData.iFightTimes -m_masterData.primeMvd.pvpData.iFightTimes;
	mvd.mvd.pvpData.iVictoryTimes= m_masterData.mvd.pvpData.iVictoryTimes - m_masterData.primeMvd.pvpData.iVictoryTimes;
	mvd.mvd.stRes.iStaminaBuyTimes = m_masterData.mvd.stRes.iStaminaBuyTimes - m_masterData.primeMvd.stRes.iStaminaBuyTimes;
	mvd.mvd.stRes.iSkillSlots      = m_masterData.mvd.stRes.iSkillSlots     - m_masterData.primeMvd.stRes.iSkillSlots;
    mvd.mvd.stRes.iElmSlots      = m_masterData.mvd.stRes.iElmSlots     - m_masterData.primeMvd.stRes.iElmSlots;
	
	if (m_masterData.mvd.pvpData.iFightTimes != 0)
	{
		m_masterData.mvd.pvpData.iVictoryRate = (CT_INT32)(m_masterData.mvd.pvpData.iVictoryTimes / (float)m_masterData.primeMvd.pvpData.iFightTimes)-
			m_masterData.primeMvd.pvpData.iVictoryRate;
	}

	mvd.llExitTime               = time(0);
	mvd.mvd.stRes.iFightingPower = m_masterData.mvd.stRes.iFightingPower;
	mvd.mvd.stRes.iFightingFlagPower = m_masterData.mvd.stRes.iFightingFlagPower;
	mvd.mvd.stRes.iTeamDuplicateFightingTimes = m_masterData.mvd.stRes.iTeamDuplicateFightingTimes - m_masterData.primeMvd.stRes.iTeamDuplicateFightingTimes;
	mvd.mvd.pvpData.iCurrentContinuousWin      = m_masterData.mvd.pvpData.iCurrentContinuousWin     - m_masterData.primeMvd.pvpData.iCurrentContinuousWin;
	m_masterData.primeMvd = m_masterData.mvd;

}
CT_VOID CCUserBase::InsertCooldownData(CooldownData &data) 
{ 
    _AUTOMUTEX(m_pMutex);
    CD_IT it = m_CooldownDataList.find(data.iType);
    if (it != m_CooldownDataList.end()) {
        it->second = data;
    } else {
        m_CooldownDataList.insert(CooldownDataList::value_type(data.iType, data));
    }  
}

// 读取能力冷却事件
CT_BOOL CCUserBase::GetCooldownData(CT_CHAR* pBuf, CT_INT32& bufLen)
{
    _AUTOMUTEX(m_pMutex);
    CT_INT32 len = m_CooldownDataList.size() * sizeof(CMD_G_CooldownData);
    if (len > bufLen)
    {
        return CT_FALSE;
    }

    time_t currTime = time(0);
    CD_IT it = m_CooldownDataList.begin();
    for (int i = 0; it != m_CooldownDataList.end(); ++it, ++i)
    {
        CMD_G_CooldownData cdData = {0};
        cdData.iType = it->second.iType;
        cdData.leftTime = it->second.leftTime - (currTime - it->second.toggleTime);
        memcpy(pBuf + sizeof(CMD_G_CooldownData) * i, &cdData, sizeof(cdData));
        //*
        CT_CHAR strLog[1024] = {0};
        sprintf(strLog, "GETCD>> USER:%d TYPE:%d, LEFT:%lld", m_pUser->GetUserID(), cdData.iType, cdData.leftTime);
        CCLogMeg::getSingleton().WriteInfo("%s", strLog);
        //*/
    }
    bufLen = len;

    return CT_TRUE;
}

//得到领主排行数据
CT_BOOL CCUserBase::GetMasterTopInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);

	if(iLen < sizeof(RankingListStatus)*TOP_TYPE_MAX)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBase::GetMasterTopInfo iLen < sizeof(RankingListStatus)*TOP_TYPE_MAX");
		return CT_FALSE;
	}

	iLen=0;
	for(CT_INT32 i=0;i<TOP_TYPE_MAX;++i)
	{
		if(i != TOP_TYPE_TEAM)
		{
			memcpy(pBuf+iLen,&m_masterData.mvd.topData.TopInfo[i],sizeof(RankingListStatus));
			iLen+=sizeof(RankingListStatus);
		}
	}
	return CT_TRUE;
}

// 更新领主能力冷却
CT_VOID CCUserBase::UpdateCooldown(CT_INT32 iType, CT_INT64 addTime)
{
    CT_INT64 currTime = time(0);
    CD_IT it = m_CooldownDataList.find(iType);    
    if (it != m_CooldownDataList.end())
    {
        CT_INT64 leftTime = it->second.leftTime - (currTime - it->second.toggleTime);
        if (leftTime < 0)
        {
            it->second.toggleTime = currTime;
            it->second.leftTime = 0;
        }
        it->second.leftTime += addTime;        
        //*
        CT_CHAR strLog[1024] = {0};
        sprintf(strLog, "ADDCD>> USER:%d TYPE:%d LEFT:%lld ADD:%lld", m_pUser->GetUserID(), it->second.iType, it->second.leftTime, addTime);
        CCLogMeg::getSingleton().WriteInfo("%s", strLog);
        //*/
    }
    else
    {
        CooldownData cdData = {0};
        cdData.iType = iType;
        cdData.leftTime = addTime;
        cdData.toggleTime = currTime;
        InsertCooldownData(cdData);
        /* 
        CT_CHAR strLog[1024] = {0};
        sprintf(strLog, "INCCD>> USER:%d TYPE:%d, LEFT:%lld", m_pUser->GetUserID(), cdData.iType, cdData.leftTime);
        CCLogMeg::getSingleton().WriteInfo("%s", strLog);*/
        //*/
    }
    // 更新冷却数据到客户端
    it = m_CooldownDataList.find(iType);
    if (it != m_CooldownDataList.end()) {
        CMD_G_CooldownData cd = {0};
        cd.iType = it->second.iType;
        cd.leftTime = it->second.leftTime - (currTime - it->second.toggleTime);
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_CD_DATA, (CT_CHAR *)&cd, sizeof(cd), false, true);
    }
}


// 刷新领主能力冷却
CT_VOID CCUserBase::RefreshCooldown(void)
{
    _AUTOMUTEX(m_pMutex);

    time_t currTime = time(0);    
    
    for (CD_IT it = m_CooldownDataList.begin(); it != m_CooldownDataList.end();)
    {
        CD_IT itTmp = it;
        itTmp++;
        G_ItemBuff itBuff = {0};
        if (it->second.toggleTime + (*it).second.leftTime <= currTime)
        {
            m_CooldownDataList.erase(it);
        }
        it = itTmp;
    }    
}
// 同步领主冷却信息
CT_VOID CCUserBase::SyncCooldown(time_t currTime)
{
    _AUTOMUTEX(m_pMutex);
    //CCLogMeg::getSingleton().WriteInfo("=== SYNC COOLDOWN ===");
    CD_IT itTmp;
    for (CD_IT it = m_CooldownDataList.begin(); it != m_CooldownDataList.end();)
    {
        itTmp = it;
        ++itTmp;
        
        /*
        CT_CHAR strLog[1024] = {0};
        sprintf(strLog, "SYNCCD>> USER:%d TYPE:%d, LEFT:%lld", m_pUser->GetUserID(), it->second.iType, it->second.leftTime);
        CCLogMeg::getSingleton().WriteInfo("%s", strLog);*/
        //*/

        if (it->second.iType != BLACKREFRESH_COOL 
            && it->second.iType != MARKETREFRESH_COOL) {
            // 更新冷却数据
            MSC_DB_W_CooldownData stDb = {0};
            stDb.dwUserID = m_pUser->GetUserID();
            stDb.iType = it->second.iType;
            stDb.llLeftTime = it->second.leftTime - (currTime - it->second.toggleTime);
            if (stDb.llLeftTime < 0) stDb.llLeftTime = 0;
            CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
            G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_COOLDOWN, (CT_CHAR*)&stDb, sizeof(stDb));
        }
        it = itTmp;
    }
    CCLogMeg::getSingleton().WriteInfo("==============");
}

// 发放军衔俸禄
CT_VOID CCUserBase::PushRankSalary()
{
    G_RankStatus st = {0};
    ENUM_RankSalaryFail enFail = ERR_DATA_ERROR;
    if (CCMasterScriptSetting::getSingleton().GetRankStatus(m_masterData.mvd.stRank.iLevel, st))
    {
        DailyQuestData stQuest = {0};
        if (!m_pUser->m_userDaily.SearchDailyQuest(DAILY_REQ_RANK_SALARY, stQuest) || stQuest.iCurrent < 1)
        {
            MasterVariableData mvd = {0};
            mvd.stRes.iGold = G_GetRankingGold(st.iSalaryGold, m_masterData.mvd.stGrade.iLevel);
            mvd.stRes.iFood = G_GetRankingFood((CT_INT32)st.fFoodCoefficient, m_masterData.mvd.stGrade.iLevel);
            //mvd.stRes.iTalentNum = st.iSalaryTalentNum;
            //mvd.stRank.iCurrentExp = st.iSalaryRankXp;
            AddMastData(mvd, SAVETAG_RANK_SALARY);

            // 通知客户端
            CMD_G_RankSalary salary = {0};
            salary.dwUserID = m_pUser->GetUserID();
            salary.iGold = m_masterData.mvd.stRes.iGold;
            salary.iFood = m_masterData.mvd.stRes.iFood;
            //salary.iTalentNum = m_masterData.mvd.stRes.iTalentNum;
            //salary.iRankXp = m_masterData.mvd.stRank.iCurrentExp;
            m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_RANK_SALARY_PUSH, (CT_CHAR*)&salary, sizeof(salary), false, true);

			//记录日志
			MSC_LOGDB_Salary LogSalary={0};
			LogSalary.dwUserID=m_pUser->GetUserID();
			LogSalary.iFood=mvd.stRes.iFood;
			LogSalary.iGold=mvd.stRes.iGold;
			CT_DWORD threadid = GAME_THREAD_LOGDATABASE + LogSalary.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SALARY,(CT_CHAR*)&LogSalary,sizeof(LogSalary));					


            // 记录每日领取军衔
            m_pUser->m_userDaily.UpdateQuestData(DAILY_REQ_RANK_SALARY, 1);

            enFail = ERR_SUCCESS;
        }
        else
        {
            enFail = ERR_HAS_REQ;
        }
        
    }
    if (enFail != ERR_SUCCESS)
    {
        CMD_G_RankSalaryFail stFail = {0};
        stFail.dwUserID = m_pUser->GetUserID();
        stFail.enFail = enFail;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_RANK_REQ_FAIL, (CT_CHAR*)&stFail, sizeof(stFail), false, true);
    }
}

// 恢复体力值
CT_VOID CCUserBase::RecoverStamina()
{
    _AUTOMUTEX(m_pMutex);

    G_ValueRange vr = {0};
    CT_INT32 nAddSta = 0;
    CT_INT32 nMaxSta = 0;
    if (CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_STAMINARESTOR, vr))
    {
        nAddSta = (CT_INT32)vr.fMinVal;
    }
    if (CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_MAXSTAMINA, vr))
    {
        nMaxSta = (CT_INT32)vr.fMaxVal;
    }
    CT_INT32 nMaxAdd = nMaxSta - m_masterData.mvd.stRes.iStamina;
    if (nMaxAdd < 0) nMaxAdd = 0;
    if (nAddSta > nMaxAdd) nAddSta = nMaxAdd;
    if (nAddSta > 0)
    {
        MasterVariableData mvd = {0};
        mvd.stRes.iStamina = nAddSta;
        AddMastData(mvd, SAVETAG_STAMINA_RECOVER);

        // 通知客户端
        CMD_G_MasterStamina stamina = {0};
        stamina.dwUserID = m_pUser->GetUserID();
        stamina.iStamina = m_masterData.mvd.stRes.iStamina; 
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_STAMINA_RECOVER, (CT_CHAR*)&stamina, sizeof(stamina), false, true);
    }
}

// 技能栏增加
CT_VOID CCUserBase::IncSkillSlot(CT_INT32 nSlot)
{
    _AUTOMUTEX(m_pMutex);
    MasterVariableData mvd = {0};
    mvd.stRes.iSkillSlots = nSlot;
    AddMastData(mvd, SAVETAG_SKILLSLOT_INC);
    
    // 通知客户端
    CMD_G_MasterSkillSlot st = {0};
    st.dwUserID = m_pUser->GetUserID();
    st.iSkillSlot = m_masterData.mvd.stRes.iSkillSlots; 
    m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_SKILLSLOT_INC, (CT_CHAR*)&st, sizeof(st), false, true);
}
//////////////////////////////////////////////////////////////////////////
// 领主升级
CT_VOID CCUserBase::MasterUpgrage(CT_BOOL bSendFailMsg)
{
    ENUM_LevelUpgradeFail enFail = (ENUM_LevelUpgradeFail)-1;
    MasterLevelInfo stGrade = m_masterData.mvd.stGrade;
    
    // 连续升级
    do
    {
        MasterLevelInfo cu = {0};
        if(! CCMasterScriptSetting::getSingleton().GetMasterUpgradeData (stGrade.iLevel + 1, cu) || cu.iCurrentExp >= 0)
        {
            enFail    = LUF_NO_NEXT_UPGRADE;
            break;
        }

        if (stGrade.iCurrentExp + cu.iCurrentExp < 0 || stGrade.iLevel == 0)
        {
            //文明度不足
            enFail    = LUF_NO_XP;
            break;
        }

        stGrade.iLevel += 1;
        stGrade.iCurrentExp += cu.iCurrentExp;
    } while (!bSendFailMsg);

    if (stGrade.iLevel == m_masterData.mvd.stGrade.iLevel || stGrade.iCurrentExp == m_masterData.mvd.stGrade.iCurrentExp)
    {
        goto FAIL;
    }

    {
        UpdateMasterLevel(stGrade);
        UpdateToDatabase(SAVETAG_MASTER_UPGRADE_FINISH);

        CMD_G_LevelUpgradeSuc ucs;
        memset(&ucs, 0, sizeof(ucs));
        ucs.dwUser = m_pUser->GetUserID();
        ucs.iLevel = stGrade.iLevel;
        ucs.iXp    = stGrade.iCurrentExp;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_UPGRADE_SUC,(CT_CHAR*)&ucs, sizeof(ucs), false, true);

        m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
		//add by caihan
		//领主升级，判断领主升级任务
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_UPGRADE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_UPGRADE_MASTER;
		taskEventInfo.iSubType = 0;
		taskEventInfo.iLevel = ucs.iLevel;
		m_pUser->m_userTask.DisposeTask(taskEventInfo);
		//领主升级，推送新的任务
		m_pUser->m_userTask.GetTaskAutomatic(ucs.iLevel);
		//end
		//add by caihan
		//领主升级，判断领主活动
		CCActive::getSingleton().ActiveProcess(ACTIVE_MASTER_LEVEL,ucs.iLevel,m_pUser);
		//end
		//领主升级，判断商城，黑市功能是否开启
		G_ValueRange vrMall = {0};
		if (G_GetValueRange(VALUE_TYPE_MALL_MASTER_LEVEL,&vrMall))
		{
			if (vrMall.fMaxVal<=ucs.iLevel&&!m_pUser->m_userBlackMarket.ViewMallMarket())
			{
				m_pUser->m_userBlackMarket.OpenMallMarketFunction();
			}
		}
		G_ValueRange vr1={0},vr2={0};
		if (G_GetValueRange(VALUE_TYPE_BLACK_MASTER_LEVEL,&vr1)&&G_GetValueRange(VALUE_TYPE_BLACK_TASK,&vr2))
		{
			//领主等级到达，并且任务完成,黑店功能没有开启
			if ( (CT_INT32)vr1.fMaxVal<=ucs.iLevel&&m_pUser->m_userTask.ViewCompleteTask((CT_INT32)vr2.fMaxVal)&&!m_pUser->m_userBlackMarket.ViewBlackMarket())
			{
				m_pUser->m_userBlackMarket.OpenBlackMarketFucntion();
			}
		}
		//领主升级，判断祭祀类型开启
		if (G_GetValueRange(VALUE_TYPE_WORSHIP_TASKID,&vr1))
		{
			if (m_pUser->m_userTask.ViewCompleteTask((CT_INT32)vr1.fMaxVal))
			{
				m_pUser->m_userWorship.GetWorshipByMasterLevel(ucs.iLevel);
			}	
		}

        // 检查附魔能否自动激活
        // 不自动激活了，呵呵
        //m_pUser->m_userEquip.CheckEnchatActive(ucs.iLevel);

		//更新等级战斗力到战队系统

		MSC_DB_FTMemeberRefresh ftmMemRefresh = {0};
		ftmMemRefresh.dwUser = m_pUser->GetUserID();
		ftmMemRefresh.iGrade = ucs.iLevel;
		ftmMemRefresh.iFightingPower = m_masterData.mvd.stRes.iFightingPower;

		CT_DWORD dwThreadID = GAME_THREAD_CITY +  ftmMemRefresh.dwUser % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
							
		G_SendThreadInfo(dwThreadID,   MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_DB_FIGHTTEAM_MEM_REFRESH, (CT_CHAR*)&ftmMemRefresh, sizeof(ftmMemRefresh));
		//CCFightTeamManager::getSingleton().FightTeamMemberRef(m_pUser->GetUserID(), ucs.iLevel, m_masterData.mvd.stRes.iFightingPower);
        
		//领主升级，判断龙岛是否开启
		G_Building gbuiding = {0};
		if (G_GetBuildingData(CITY_TYPE_DRAGONISLAND, &gbuiding))
		{
			if (ucs.iLevel >= gbuiding.iMasterGrade && !m_pUser->m_userIsland.ViewIsland())
			{
				CT_DWORD dwUserID = m_pUser->GetUserID();
				dwThreadID = GAME_THREAD_CITY +  m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
				G_SendThreadInfo(dwThreadID,   MSC_MAIN_ISLAND_EVENT, MSC_SUB_OPEN_ISLAND, (CT_CHAR*)&dwUserID, sizeof(dwUserID));
			}
		}

		return;
    }

FAIL:
    if (bSendFailMsg) 
    {
        CMD_G_LevelUpgradeFail stFail = {0};
        stFail.dwUser = m_pUser->GetUserID();
        stFail.ucf = enFail;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_UPGRADE_FAIL,(CT_CHAR*)&stFail, sizeof(stFail), false, true);
    }
}
// 文明升级
CT_VOID CCUserBase::CivilUpgrage(CT_BOOL bSendFailMsg)
{
    ENUM_LevelUpgradeFail enFail = (ENUM_LevelUpgradeFail)-1;    
    MasterLevelInfo stCivil = m_masterData.mvd.stCivili;

    do
    {
        MasterLevelInfo cu = {0};
        if(! CCBuildingScriptSetting::getSingleton().GetCivilUpgradeData(stCivil.iLevel + 1, &cu) || cu.iCurrentExp >= 0)
        {
            enFail    = LUF_NO_NEXT_UPGRADE;
            break;
        }

        if (stCivil.iCurrentExp + cu.iCurrentExp < 0 || stCivil.iLevel == 0)
        {
            //文明度不足
            enFail    = LUF_NO_XP;
            break;
        }

        stCivil.iLevel += 1;
        stCivil.iCurrentExp += cu.iCurrentExp;
    } while (!bSendFailMsg);
    
    if (stCivil.iLevel == m_masterData.mvd.stCivili.iLevel || stCivil.iCurrentExp == m_masterData.mvd.stCivili.iCurrentExp)
    {
        goto FAIL;
    }

    {
        UpdateMasterCivili(stCivil);
        UpdateToDatabase(SAVETAG_CIVIL_UPGRADE_FINISH);

        CMD_G_LevelUpgradeSuc ucs;
        memset(&ucs, 0, sizeof(ucs));
        ucs.dwUser = m_pUser->GetUserID();
        ucs.iLevel = stCivil.iLevel;
        ucs.iXp    = stCivil.iCurrentExp;
        m_pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_CIVILUPGRADE_SUC,(CT_CHAR*)&ucs, sizeof(ucs), false, true);

        // 更新系数数据
        m_pUser->UpdateFactorData();

		//文明升级，判断文明升级任务
		//add by caihan  11-16
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_UPGRADE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_UPGRADE_CIVIL;
		taskEventInfo.iSubType = 0;
		taskEventInfo.iLevel = ucs.iLevel;
		m_pUser->m_userTask.DisposeTask(taskEventInfo);
		//end

		//判断文明活动
		CCActive::getSingleton().ActiveProcess(ACTIVE_CIVIL_ADDUP,ucs.iLevel,m_pUser);
		
		              
		//增加文明广播消息             彭文修改
		G_CivilUpgrade cu={0};
		if (!G_GetCivilUpgrade(ucs.iLevel,&cu))
		{
			return;
		}
		CMD_G_BroadCastMes bcm = {0};
		bcm.dwUserID = m_pUser->GetUserID();
		bcm.iSubType = BROADCAST_SUB_TYPE_CIVIL_UPGRADE;
		bcm.iQuality = ucs.iLevel;
		memcpy(bcm.cResultName,cu.cCivilName,CIVIL_NAME_LENEX);


		//G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_SYSTEM, (CT_CHAR*)&bcm, sizeof(bcm));
		//end
		
        return;
    }

FAIL:
    if (bSendFailMsg) 
    {
        CMD_G_LevelUpgradeFail stFail = {0};
        stFail.dwUser = m_pUser->GetUserID();
        stFail.ucf = enFail;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_CIVILUPGRADE_FAIL,(CT_CHAR*)&stFail, sizeof(stFail), false, true);
    }
}
// 军衔升级
CT_VOID CCUserBase::RankUpgrage(CT_BOOL bSendFailMsg)
{
    ENUM_LevelUpgradeFail enFail = (ENUM_LevelUpgradeFail)-1;
    MasterLevelInfo stGrade = m_masterData.mvd.stRank;

    // 连续升级
    do
    {
        MasterLevelInfo cu = {0};
        if(! CCMasterScriptSetting::getSingleton().GetRankUpgradeData(stGrade.iLevel + 1, cu) || cu.iCurrentExp >= 0)
        {
            enFail    = LUF_NO_NEXT_UPGRADE;
            break;
        }

        if (stGrade.iCurrentExp + cu.iCurrentExp < 0 || stGrade.iLevel == 0)
        {
            // 声望不足
            enFail    = LUF_NO_XP;
            break;
        }

        stGrade.iLevel += 1;
        stGrade.iCurrentExp += cu.iCurrentExp;
    } while (!bSendFailMsg);

    if (stGrade.iLevel == m_masterData.mvd.stRank.iLevel || stGrade.iCurrentExp == m_masterData.mvd.stRank.iCurrentExp)
    {
        goto FAIL;
    }

    {
		//日志记录
		MSC_LOGDB_RankingUpGrade LogUpGrade={0};
		LogUpGrade.dwUserID=m_pUser->GetUserID();
		LogUpGrade.iGrade=stGrade.iLevel;
		LogUpGrade.iGradeUp=stGrade.iLevel;
		LogUpGrade.iRankxp=m_masterData.mvd.stRank.iCurrentExp-stGrade.iCurrentExp;
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_RANKINGUPGRADE,(CT_CHAR*)&LogUpGrade,sizeof(LogUpGrade));					

        UpdateMasterRank(stGrade);
         UpdateToDatabase(SAVETAG_RANK_UPGRADE_FINISH);

        CMD_G_LevelUpgradeSuc ucs;
        memset(&ucs, 0, sizeof(ucs));
        ucs.dwUser = m_pUser->GetUserID();
        ucs.iLevel = stGrade.iLevel;
        ucs.iXp    = stGrade.iCurrentExp;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_RANK_UPGRADE_SUC,(CT_CHAR*)&ucs, sizeof(ucs), false, true);

        m_pUser->UpdateAndSendMasterData(EMI_BATLLE);

        // 升级了，重置领取俸禄, 仅当升到2级以上时有效
        if (stGrade.iLevel > 2) {
            m_pUser->m_userDaily.UpdateQuestData(DAILY_REQ_RANK_SALARY, -1);
        }

		//军衔升级，判断军衔升级任务
		//add by caihan  11-16
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_UPGRADE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_UPGRADE_RANK;
		taskEventInfo.iSubType = 0;
		taskEventInfo.iLevel = ucs.iLevel;
		m_pUser->m_userTask.DisposeTask(taskEventInfo);
		//end
		//判断是否有军衔升级活动
		CCActive::getSingleton().ActiveProcess(ACTIVE_RANK_LEVEL,ucs.iLevel,m_pUser);

		RollBroadcast rb[2]={0};
		rb[0].SetValue(m_pUser->GetUserID(),m_masterData.cNickName);
		rb[1].SetValue(ucs.iLevel,"");
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{1}<9:%d>", m_pUser->GetUserID(),m_masterData.cNickName,ucs.iLevel);
		G_SendRollBroadecast(1,5,1,(CT_CHAR*)rb,sizeof(rb));
        return;
    }

FAIL:
    if (bSendFailMsg) 
    {
        CMD_G_LevelUpgradeFail stFail = {0};
        stFail.dwUser = m_pUser->GetUserID();
        stFail.ucf = enFail;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_RANK_UPGRADE_FAIL,(CT_CHAR*)&stFail, sizeof(stFail), false, true);
    }
}

//更新数据到数据库
CT_VOID CCUserBase::UpdataData(CT_INT64 lTime)
{
	_AUTOMUTEX(m_pMutex);

	if(lTime-m_llLastSysTime < g_iDBUpdataValue)
		return;

	MSC_DB_MasterSaveData msd = {0};
	GetMasterSaveData(msd);
	msd.iSaveTag   = SAVETAG_TIMING_UPDATA;

	CT_INT32 threadID =  GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_MASTER_DATA , (CT_CHAR*)&msd, sizeof(msd));

	CT_INT32 logThreadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));

	m_llLastSysTime = lTime;
	return;
}

CT_VOID CCUserBase::SyncStamina( void )
{
	_AUTOMUTEX(m_pMutex);
	// 同步体力值
	G_ValueRange vr = {0};
	CT_INT32 nInterval = 0;
	CT_INT32 nRecover = 0;
	CT_INT32 nMaxStamina = 0;
	CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_MAXSTAMINA, vr);
	nMaxStamina = (CT_INT32)vr.fMaxVal;
	if (nMaxStamina > m_masterData.mvd.stRes.iStamina)
	{
		CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_STAMINAINTERVAL, vr);
		nInterval = (CT_INT32)vr.fMinVal;
		CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_STAMINARESTOR, vr);
		nRecover = (CT_INT32)vr.fMinVal;
        CT_INT64 llServerLaunchTime = g_lOpenServerTime; // 服务器启动时间
        CT_INT64 llLastRecoverTime = llServerLaunchTime + (m_masterData.llLastExitTime - llServerLaunchTime) / nInterval * nInterval;
		CT_INT64 passTime = time(0) - llLastRecoverTime;
		m_masterData.mvd.stRes.iStamina +=  (CT_INT32)(passTime / nInterval * nRecover);
		if (m_masterData.mvd.stRes.iStamina > nMaxStamina) m_masterData.mvd.stRes.iStamina = nMaxStamina;
	}
}

//更新组队副本战斗次数
CT_VOID CCUserBase::UpdataTeamFightingTimes()
{
	_AUTOMUTEX(m_pMutex);
	m_masterData.mvd.stRes.iTeamDuplicateFightingTimes=0;
	m_masterData.primeMvd.stRes.iTeamDuplicateFightingTimes=0;
	return;
}

//更新体力购买次数
CT_VOID CCUserBase::UpdataBuyStaminaTimes()
{
	_AUTOMUTEX(m_pMutex);
	m_masterData.mvd.stRes.iStaminaBuyTimes=0;
	m_masterData.primeMvd.stRes.iStaminaBuyTimes=0;
	return;
}

// 消耗某种资源
CT_BOOL CCUserBase::CheckUserRes(CT_INT32 iResID, CT_INT32 iResCnt)
{
    
    switch (iResID)
    {
    case VALUE_TYPE_GOLD:
        return m_masterData.mvd.stRes.iGold + iResCnt >= 0;
    case VALUE_TYPE_FOOD:
        return m_masterData.mvd.stRes.iFood + iResCnt >= 0;
    case VALUE_TYPE_TALENTNUM:
        return m_masterData.mvd.stRes.iTalentNum + iResCnt >= 0;
    case VALUE_TYPE_DIAMOND:
        return m_masterData.mvd.stRes.iDiamod + iResCnt >= 0;
    case VALUE_TYPE_STAMINA:
        return m_masterData.mvd.stRes.iStamina + iResCnt >= 0;
    default:
        return CT_FALSE;
    }
}

//修改领主名称
CT_VOID CCUserBase::ChangeMasterName(CT_CHAR *pName)
{
	_AUTOMUTEX(m_pMutex);
	memcpy(m_masterData.cNickName,pName,NAME_SIZE-1);
	return;
}

CT_VOID CCUserBase::UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iClass, const CT_INT32 iLevel, const CT_INT32 iRank)
{
    // 职业属性
    G_ClassStatus ClassStatus = {0};
    if (G_GetClassStatus(iClass, &ClassStatus))
    {
        mcs.iMaxMP         += ClassStatus.iMana;
        mcs.iStrength      += ClassStatus.iStrength;
        mcs.iAgility       += ClassStatus.iAgility;
        mcs.iIntellect     += ClassStatus.iIntellect;
        mcs.iSpirit        += ClassStatus.iSpirit;
        mcs.fHitRate       += ClassStatus.fHitRate;
        mcs.fDodgeRate     += ClassStatus.fDodgeRate;
        mcs.fCritRate      += ClassStatus.fCritRate;
        mcs.fResilient     += ClassStatus.fResilient;
    }

    // 等级属性
    G_GradeStatus GradeStatus = {0};
    if (G_GetGradeStatus(iClass, iLevel, &GradeStatus))
    {
        mcs.iMaxHP         += GradeStatus.iHealth;
        mcs.iAttack        += GradeStatus.iAttack;
        mcs.iDefence       += GradeStatus.iDefence;
        mcs.iStrength      += GradeStatus.iStrength;
        mcs.iAgility       += GradeStatus.iAgility;
        mcs.iIntellect     += GradeStatus.iIntellect;
        mcs.iSpirit        += GradeStatus.iSpirit;
		mcs.fSpeed		   += GradeStatus.fSpeed;
    }

    // 军衔属性
    G_RankStatus RankStatus = {0};
    if (G_GetRankStatus(iRank, &RankStatus))
    {
        mcs.iMaxHP         += RankStatus.iHealth;
        mcs.iMaxMP         += RankStatus.iMana;
        mcs.iStrength      += RankStatus.iStrength;
        mcs.iAgility       += RankStatus.iAgility;
        mcs.iIntellect     += RankStatus.iIntellect;
        mcs.iSpirit        += RankStatus.iSpirit;
        mcs.fHitRate       += RankStatus.fHitRate;
        mcs.fDodgeRate     += RankStatus.fDodgeRate;
        mcs.fCritRate      += RankStatus.fCritRate;
        mcs.fResilient     += RankStatus.fResilient;
        mcs.fDmgIncPercent += RankStatus.fDmgIncPercent;
        mcs.fDmgDecPercent += RankStatus.fDmgDecPercent;
        mcs.fHPPersent     += RankStatus.fHealthPercent;
    }
}

CT_VOID CCUserBase::UpdateCooldownData( CT_DWORD dwUserID, CooldownData &data )
{
    // 更新冷却数据
    MSC_DB_W_CooldownData stDb = {0};
    stDb.dwUserID = dwUserID;
    stDb.iType = data.iType;
    stDb.llLeftTime = data.leftTime - (time(0) - data.toggleTime);
    if (stDb.llLeftTime < 0) stDb.llLeftTime = 0;
    CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
    G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_COOLDOWN, (CT_CHAR*)&stDb, sizeof(stDb));
}

//更新排行榜奖励领取状态
CT_VOID CCUserBase::UpdateTopRewardStatus(CT_INT32 iType, CT_INT32 iStatus)
{
	_AUTOMUTEX(m_pMutex);

	m_masterData.mvd.topData.TopInfo[iType].iRewardStatus = iStatus;
}