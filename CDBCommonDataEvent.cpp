#include "CDBCommonDataEvent.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CDBConnector.h"
#include "CDBLogMes.h"

CCDBCommonDataEvent::CCDBCommonDataEvent(CCGameMesManager* pMesManager, CT_DWORD id,
									      CCDBConnector& dbConnector) : CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_MASTER_DATA);
    RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_MASTER_COOLDOWN);
	RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_MASTER_DATA_EX);
	RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_W_MASTER_NAME);
	RegistMessage(MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_MASTER_FIGHT_DATA);
}

CCDBCommonDataEvent::~CCDBCommonDataEvent(void)
{
}

//处理用户消息
CT_VOID CCDBCommonDataEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
										 CT_CHAR* pBuf,		CT_DWORD len,
										 CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case  MSC_MAIN_DB_SAVE_DATA:
		{
			if (subID == MSC_SUB_DB_MASTER_DATA)
			{
				if (len != sizeof(MSC_DB_MasterSaveData))
				{
					G_DisposeErrorMes(0, subID, "CCDBCommonDataEvent::OnUserEvent: len == sizeof(MSC_DB_MasterSaveData)");
					return ;
				}
				MSC_DB_MasterSaveData* pMasterSaveData = (MSC_DB_MasterSaveData*)pBuf;
				DisposeMasterSaveData(*pMasterSaveData);
			}
			else if (subID == MSC_SUB_DB_MASTER_DATA_EX)
			{
				if (len != sizeof(MSC_DB_MasterSaveDataEx))
				{
					G_DisposeErrorMes(0, subID, "CCDBCommonDataEvent::OnUserEvent: len == sizeof(MSC_DB_MasterSaveDataEx)");
					return ;
				}
				MSC_DB_MasterSaveDataEx* pMasterSaveDataEx = (MSC_DB_MasterSaveDataEx*)pBuf;
				DisposeMasterSaveDataEx(*pMasterSaveDataEx);
			}
            else if (subID == MSC_SUB_DB_MASTER_COOLDOWN)
            {
                if (len != sizeof(MSC_DB_W_CooldownData))
                {
                    G_DisposeErrorMes(0, subID, "CCDBCommonDataEvent::OnUserEvent: len == sizeof(MSC_DB_W_CooldownData)");
                    return ;
                }
                DisposeSaveData(*(MSC_DB_W_CooldownData*)pBuf);
            }
			else if(subID == MSC_SUB_DB_W_MASTER_NAME)
			{
				if (len != sizeof(MSC_DB_MasterName))
				{
					G_DisposeErrorMes(0, subID, "CCDBCommonDataEvent::OnUserEvent: len == sizeof(MSC_DB_MasterName)");
					return ;
				}
				DisposeChangeMasterName(*(MSC_DB_MasterName*)pBuf);
			}
			else if (subID == MSC_SUB_DB_MASTER_FIGHT_DATA)
			{
				if (len != sizeof(MSC_DB_MasterFightSaveData))
				{
					G_DisposeErrorMes(0, subID, "CCDBCommonDataEvent::OnUserEvent: len == sizeof(MSC_DB_MasterFightSaveData)");
					return ;
				}
				MSC_DB_MasterFightSaveData* pMfs = (MSC_DB_MasterFightSaveData*)pBuf;
				DisposeMasterFightSaveData(*pMfs);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBCityUserBuildEvent::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
			break;
		}
	}
}


//保存领主数据
CT_VOID CCDBCommonDataEvent::DisposeMasterSaveData(const MSC_DB_MasterSaveData& masterSaveData)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SaveMasterData");

	m_dbConnector.AddParm("_uid",       masterSaveData.dwUserID);
	m_dbConnector.AddParm("_gold",      masterSaveData.mvd.stRes.iGold);
	m_dbConnector.AddParm("_diamod",    masterSaveData.mvd.stRes.iDiamod );

	m_dbConnector.AddParm("_food",      masterSaveData.mvd.stRes.iFood );
	m_dbConnector.AddParm("_civiNum",   masterSaveData.mvd.stCivili.iCurrentExp );
	m_dbConnector.AddParm("_civiGrade", masterSaveData.mvd.stCivili.iLevel );
	m_dbConnector.AddParm("_grade",     masterSaveData.mvd.stGrade.iLevel);
	m_dbConnector.AddParm("_gradeXp",   masterSaveData.mvd.stGrade.iCurrentExp );

	m_dbConnector.AddParm("_rank",		masterSaveData.mvd.stRank.iLevel ); 
	m_dbConnector.AddParm("_rankExp",   masterSaveData.mvd.stRank.iCurrentExp);


	m_dbConnector.AddParm("_talentNum", masterSaveData.mvd.stRes.iTalentNum);
	m_dbConnector.AddParm("_stamina ",  masterSaveData.mvd.stRes.iStamina);
    m_dbConnector.AddParm("_skillsouls", masterSaveData.mvd.stRes.iSkillSoul);
	//m_dbConnector.AddParm("_exitTime",  (CT_INT64)masterSaveData.llExitTime);
	m_dbConnector.AddParm("_pvpVicotryTimes ", masterSaveData.mvd.pvpData.iVictoryTimes);
	m_dbConnector.AddParm("_pvpFightTimes ",   masterSaveData.mvd.pvpData.iFightTimes);
	m_dbConnector.AddParm("_stiminaBuyTimies", masterSaveData.mvd.stRes.iStaminaBuyTimes);
    m_dbConnector.AddParm("_useSkillNums", masterSaveData.mvd.stRes.iSkillSlots);
	m_dbConnector.AddParm("_bagSlot",     masterSaveData.mvd.stRes.iBagSlots);
	m_dbConnector.AddParm("_Savetag",  masterSaveData.iSaveTag);
	m_dbConnector.AddParm("_fightingpower",  masterSaveData.mvd.stRes.iFightingPower);
	m_dbConnector.AddParm("_teamfighttimes",  masterSaveData.mvd.stRes.iTeamDuplicateFightingTimes);
	m_dbConnector.AddParm("_contwin",  masterSaveData.mvd.pvpData.iCurrentContinuousWin);
    m_dbConnector.AddParm("_elmslot",  masterSaveData.mvd.stRes.iElmSlots);
	m_dbConnector.AddParm("_fightingflagpower",  masterSaveData.mvd.stRes.iFightingFlagPower);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(masterSaveData.dwUserID,"SP_SaveMasterData","_uid:%d _gold:%d _diamod:%d  _food:%d _civiNum:%d _civiGrade:%d _grade:%d _gradeXp:%d _rank:%d _rankExp:%d \
                                                                  _talentNum:%d _stamina:%d  _skillsouls:%d _exitTime:%d _pvpVicotryTimes:%d _pvpFightTimes:%d _stiminaBuyTimies:%d _useSkillNums:%d _Savetag:%d",\
																  masterSaveData.dwUserID,masterSaveData.mvd.stRes.iGold,masterSaveData.mvd.stRes.iDiamod,\
																  masterSaveData.mvd.stRes.iFood,masterSaveData.mvd.stCivili.iCurrentExp,masterSaveData.mvd.stCivili.iLevel,masterSaveData.mvd.stGrade.iLevel,masterSaveData.mvd.stGrade.iCurrentExp,\
																  masterSaveData.mvd.stRank.iLevel,masterSaveData.mvd.stRank.iCurrentExp,masterSaveData.mvd.stRes.iTalentNum,\
																  masterSaveData.mvd.stRes.iStamina,masterSaveData.mvd.stRes.iSkillSoul,masterSaveData.llExitTime,masterSaveData.mvd.pvpData.iVictoryTimes,\
																  masterSaveData.mvd.pvpData.iFightTimes,masterSaveData.mvd.stRes.iStaminaBuyTimes,masterSaveData.mvd.stRes.iSkillSlots,masterSaveData.iSaveTag);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
};

CT_VOID CCDBCommonDataEvent::DisposeMasterSaveDataEx(const MSC_DB_MasterSaveDataEx& masterSaveDataEx)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SaveMasterDataEx");	

	m_dbConnector.AddParm("_uid",			masterSaveDataEx.dwUserID);
	m_dbConnector.AddParm("_savetag",		masterSaveDataEx.iSaveTagEx);
	m_dbConnector.AddParm("_bigintvalue",	masterSaveDataEx.iInt64Value);
	m_dbConnector.AddParm("_intvalue",		masterSaveDataEx.iInt32Value);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(masterSaveDataEx.dwUserID,
			"SP_SaveMasterDataEx","_uid:%d _savetag:%d _bigintvalue:%lld  _intvalue:%d ",
			masterSaveDataEx.dwUserID, 	masterSaveDataEx.iSaveTagEx,masterSaveDataEx.iInt64Value,masterSaveDataEx.iInt32Value);
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

// 保存冷却信息
CT_VOID CCDBCommonDataEvent::DisposeSaveData(const MSC_DB_W_CooldownData &data)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_PropUpdateCool");

    m_dbConnector.AddParm("_uid",       data.dwUserID);
    m_dbConnector.AddParm("_cooltype",  data.iType);
    m_dbConnector.AddParm("_lasting",   data.llLeftTime);

    //*
    CT_CHAR strLog[1024] = {0};
    sprintf(strLog, "[WCDDATA]: USER:%u TYPE:%d LEFT:%lld", data.dwUserID, data.iType, data.llLeftTime);
    CCLogMeg::getSingleton().WriteInfo("%s", strLog);
    //*/

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        //pResult->GetResult();
        pResult->Release();
    }else
    {
		CCDBLogMes::getSingleton().WriteInfo(data.dwUserID,"SP_PropUpdateCool","_uid:%d _cooltype:%d _lasting:%d",data.dwUserID,data.iType,data.llLeftTime);
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }
}

//修改领主名称
CT_VOID CCDBCommonDataEvent::DisposeChangeMasterName(MSC_DB_MasterName &mn)
{
	MSC_USR_ChangeMasterName cmn={0};
	cmn.dwUserID=mn.dwUserID;
	cmn.iPropID=mn.iPropID;
	memcpy(cmn.cName,mn.cName,NAME_SIZE);

	m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_ChangeMasterName");

    m_dbConnector.AddParm("_uid",mn.dwUserID);
    m_dbConnector.AddParm("_Name",mn.cName);

	CT_INT32 iStatus=0;
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, cmn.iStatus);
			}
		}
        pResult->Release();
    }else
    {
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }

	CT_INT32 threadID =  GAME_THREAD_CITY + mn.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_USR_ITEM_UPDATE,MSC_SUB_DB_W_MASTER_NAME , (CT_CHAR*)&cmn, sizeof(cmn));

	return;
}

//保存领主加成数据
CT_VOID CCDBCommonDataEvent::DisposeMasterFightSaveData(const MSC_DB_MasterFightSaveData& mfsd)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MasterFightAdd");

	m_dbConnector.AddParm("_uid",			mfsd.dwUserID);
	m_dbConnector.AddParm("_type",			mfsd.iType);
	m_dbConnector.AddParm("_MaxHP",			mfsd.mcs.iMaxHP);			
	m_dbConnector.AddParm("_MaxMP",			mfsd.mcs.iMaxMP);			
	m_dbConnector.AddParm("_Strength",		mfsd.mcs.iStrength);		
	m_dbConnector.AddParm("_Agility",		mfsd.mcs.iAgility);		
	m_dbConnector.AddParm("_Intellect",     mfsd.mcs.iIntellect);		
	m_dbConnector.AddParm("_Spirit",		mfsd.mcs.iSpirit);		
	m_dbConnector.AddParm("_HitRate",		mfsd.mcs.fHitRate);		
	m_dbConnector.AddParm("_DodgeRate",     mfsd.mcs.fDodgeRate);		
	m_dbConnector.AddParm("_CritRate",		mfsd.mcs.fCritRate);		
	m_dbConnector.AddParm("_Resilient",		mfsd.mcs.fResilient);		
	m_dbConnector.AddParm("_DmgIncPercent",	mfsd.mcs.fDmgIncPercent);	
	m_dbConnector.AddParm("_DmgDecPercent", mfsd.mcs.fDmgDecPercent);	
	m_dbConnector.AddParm("_Attack",		mfsd.mcs.iAttack);		
	m_dbConnector.AddParm("_Defence",		mfsd.mcs.iDefence);		
	m_dbConnector.AddParm("_Speed",			mfsd.mcs.fSpeed);

	if(m_dbConnector.Exec())						
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}