#include "CDBTowerEvent.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "../sdk/include/CDBLogMes.h"
#include "CTowerResManager.h"

CCDBTowerEvent::CCDBTowerEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistTimer(IDI_TOWER_LOAD);
	RegistTimer(IDI_TOWER_SAVE);
	if (id == GAME_THREAD_DATABASE)
	{
		SetTimer(IDI_TOWER_LOAD, TIME_TOWER_LOAD);
		SetTimer(IDI_TOWER_SAVE, TIME_TOWER_SAVE);
	}
	RegistMessage(MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_UPDATE);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWER_SLOT_REWARD);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWER_SLOT_REWARD_DEL);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWER_MSG_ADD);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWER_MSG_DEL);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_SYS_EXIT);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWER_FIGHT);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWERSLOT_FIGHT);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWER_EVENT);
	RegistMessage(MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_TOWER_HELP);
}

CCDBTowerEvent::~CCDBTowerEvent(void)
{
}

//处理用户消息
CT_VOID CCDBTowerEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
									 CT_CHAR* pBuf,		CT_DWORD len,
									 CT_LONGLONG reserve)
{
	if(mainID != MSC_MAIN_DB_TOWER_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_DB_TOWER_UPDATE:	
		{
			DisposeTowerUpdate(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWER_SLOT_REWARD:
		{
			DisposeTowerSlotReward(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWER_SLOT_REWARD_DEL:
		{
			DisposeTowerSlotRewardDel(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWER_MSG_ADD:
		{
			DisposeTowerMSGAdd(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWER_MSG_DEL:
		{
			DisposeTowerMSGDel(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWER_FIGHT:
		{
			DisposeTowerFight(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWERSLOT_FIGHT:
		{
			DisposeTowerSlotFight(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWER_EVENT:
		{
			DisposeTowerEvent(pBuf,len);
			break;
		}
	case MSC_SUB_DB_TOWER_HELP:
		{
			DisposeTowerHelp(pBuf,len);
			break;
		}
	case MSC_SUB_DB_SYS_EXIT:
		{
			DisposeSaveTowerToDB();
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::OnUserEvent : mainID:%d	subID:%d",mainID,subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCDBTowerEvent::OnTime(CT_DWORD timeID)
{
	if (IDI_TOWER_LOAD == timeID)
	{
		KillTimer(IDI_TOWER_LOAD);
		DisposeLoadTowerFromDB();
		return;
	}

	if (IDI_TOWER_SAVE == timeID)
	{
		DisposeSaveTowerToDB();
		return;
	}

	return;
}

CT_VOID CCDBTowerEvent::DisposeSaveTowerToDB()
{
    std::string data;
	CCTowerResManager::getSingleton().GetTowerDataToDB( data);
	m_dbConnector.Reset();
	if(m_dbConnector.Exec((CT_CHAR*)data.c_str(), data.size()))
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(0, "CCDBTowerEvent::DisposeSaveTowerToDB", "mysql = %s",  data.c_str());
	}
}

CT_VOID CCDBTowerEvent::DisposeLoadTowerFromDB()
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerSlotGet");

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				CT_INT32 iFoor = 0, iSlot = 0;
				CCTowerResManager::ElementTowerSlotInfo slotInfo = {0};
				pResult->GetValue(0, slotInfo.userInfo.dwUser);
				pResult->GetValue(1, (CT_CHAR*)slotInfo.userInfo.cNickName, NAME_SIZE);
				pResult->GetValue(2, slotInfo.userInfo.iGrade);
				pResult->GetValue(3, slotInfo.userInfo.iOccupation);
				pResult->GetValue(4, slotInfo.userInfo.iFightPower);
				pResult->GetValue(5, iFoor);
				pResult->GetValue(6, iSlot);
				pResult->GetValue(7, slotInfo.llBeginTime);

				pResult->GetValue(8, slotInfo.iHelpFriend[0]);
				pResult->GetValue(9, slotInfo.iHelpFriend[1]);
				pResult->GetValue(10, slotInfo.iHelpFriend[2]);
				pResult->GetValue(11, slotInfo.iHelpFriend[3]);
				pResult->GetValue(12, slotInfo.iHelpFriend[4]);

				pResult->GetValue(13, slotInfo.iHelpFriend[5]);
				pResult->GetValue(14, slotInfo.iHelpFriend[6]);
				pResult->GetValue(15, slotInfo.iHelpFriend[7]);
				pResult->GetValue(16, slotInfo.iHelpFriend[8]);
				pResult->GetValue(17, slotInfo.iHelpFriend[9]);

				CCTowerResManager::getSingleton().RecoverTowerDataFromDB(iFoor, iSlot, slotInfo);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerUpdate(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_TOWER))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::DisposeTowerUpdate : len != sizeof(MSC_DB_TOWER)");
		return;
	}

	MSC_DB_TOWER *pTowerData=(MSC_DB_TOWER*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerUpdate");
	m_dbConnector.AddParm("_uid", pTowerData->dwU_ID);
	m_dbConnector.AddParm("_cpid", pTowerData->iCPID);
	m_dbConnector.AddParm("_treasure", pTowerData->iTreasure);
	m_dbConnector.AddParm("_time", pTowerData->iTime);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pTowerData->dwU_ID, "SP_TowerUpdate", "_uid:%d, _cpid:%d,, _treasure:%d, _time:%d", 
			pTowerData->dwU_ID, pTowerData->iCPID, pTowerData->iTreasure, pTowerData->iTime);
		return;
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerSlotReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_TowerSlotReward))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::DisposeTowerSlotReward : len != sizeof(MSC_DB_TowerSlotReward)");
		return;
	}
	//_uid, _floor, _slot, _restype1, _resnum1, _restype2, _resnum2, _restype3, _resnum3, _restype4, _resnum4, 
	//_itemid1, _itemnum1, _itemid2, _itemnum2, _itemgroupid1, _itemgroupnum1, _itemgroupid2, _itemgroupnum2 

	MSC_DB_TowerSlotReward *pTowerSlotReward=(MSC_DB_TowerSlotReward*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerSlotRewardUpdate");
	m_dbConnector.AddParm("_uid", pTowerSlotReward->dwUserID);
	m_dbConnector.AddParm("_floor", pTowerSlotReward->iFloor);
	m_dbConnector.AddParm("_slot", pTowerSlotReward->iSlot);
	m_dbConnector.AddParm("_restype1", pTowerSlotReward->mnReward.iGetResType1);
	m_dbConnector.AddParm("_resnum1", pTowerSlotReward->mnReward.iGetResNum1);
	m_dbConnector.AddParm("_restype2", pTowerSlotReward->mnReward.iGetResType2);
	m_dbConnector.AddParm("_resnum2", pTowerSlotReward->mnReward.iGetResNum2);
	m_dbConnector.AddParm("_restype3", pTowerSlotReward->mnReward.iGetResType3);
	m_dbConnector.AddParm("_resnum3", pTowerSlotReward->mnReward.iGetResNum3);
	m_dbConnector.AddParm("_restype4", pTowerSlotReward->mnReward.iGetResType4);
	m_dbConnector.AddParm("_resnum4", pTowerSlotReward->mnReward.iGetResNum4);
	m_dbConnector.AddParm("_itemid1", pTowerSlotReward->mnReward.iGetItemID1);
	m_dbConnector.AddParm("_itemnum1", pTowerSlotReward->mnReward.iGetItemNum1);
	m_dbConnector.AddParm("_itemid2", pTowerSlotReward->mnReward.iGetItemID2);
	m_dbConnector.AddParm("_itemnum2", pTowerSlotReward->mnReward.iGetItemNum2);
	m_dbConnector.AddParm("_itemgroupid1", pTowerSlotReward->mnReward.iGetItemGroupID1);
	m_dbConnector.AddParm("_itemgroupnum1", pTowerSlotReward->mnReward.iGetItemGroupNum1);
	m_dbConnector.AddParm("_itemgroupid2", pTowerSlotReward->mnReward.iGetItemGroupID2);
	m_dbConnector.AddParm("_itemgroupnum2", pTowerSlotReward->mnReward.iGetItemGroupNum2);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pTowerSlotReward->dwUserID, "SP_TowerSlotRewardUpdate", "_uid:%d, _floor:%d, _slot:%d", 
			pTowerSlotReward->dwUserID, pTowerSlotReward->iFloor, pTowerSlotReward->iSlot);
		return;
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerSlotRewardDel(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_TowerSlotRewardDel))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::DisposeTowerSlotReward : len != sizeof(MSC_DB_TowerSlotRewardDel)");
		return;
	}
	//_uid, _floor, _slot
	MSC_DB_TowerSlotRewardDel *pTowerSlotRewardDel=(MSC_DB_TowerSlotRewardDel*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerSlotRewardDelete");
	m_dbConnector.AddParm("_uid", pTowerSlotRewardDel->dwUserID);
	m_dbConnector.AddParm("_floor", pTowerSlotRewardDel->iFloor);
	m_dbConnector.AddParm("_slot", pTowerSlotRewardDel->iSlot);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pTowerSlotRewardDel->dwUserID, "SP_TowerSlotRewardDel", "_uid:%d, _floor:%d, _slot:%d", 
			pTowerSlotRewardDel->dwUserID, pTowerSlotRewardDel->iFloor, pTowerSlotRewardDel->iSlot);
		return;
	}
}


CT_VOID CCDBTowerEvent::DisposeTowerMSGAdd(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_MSG))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::DisposeTowerMSGAdd : len != sizeof(MSC_DB_MSG)");
		return;
	}
	
	MSC_DB_MSG *pMSG=(MSC_DB_MSG*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MSGAdd");
	m_dbConnector.AddParm("_uid", pMSG->dwUserID);
	m_dbConnector.AddParm("_type", pMSG->iMSGType);
	m_dbConnector.AddParm("_time", pMSG->iTime);
	m_dbConnector.AddParm("_text", pMSG->cMSG);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pMSG->dwUserID, "SP_MSGAdd", "_uid:%d", pMSG->dwUserID);
		return;
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerMSGDel(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_MSG_DELETE))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::DisposeTowerMSGDel : len != sizeof(MSC_DB_MSG_DELETE)");
		return;
	}

	MSC_DB_MSG_DELETE *pMSG=(MSC_DB_MSG_DELETE*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MSGDelete");
	m_dbConnector.AddParm("_uid", pMSG->dwUserID);
	m_dbConnector.AddParm("_type", pMSG->iMSGType);
	m_dbConnector.AddParm("_time", pMSG->iTime);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pMSG->dwUserID, "SP_MSGDelete", "_uid:%d", pMSG->dwUserID);
		return;
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerFight(CT_CHAR* pBuf,CT_DWORD len)
{

	if(len != sizeof(MSC_DB_TowerFight))
	{
		CCLogMeg::getSingleton().WriteError("DisposeTowerFight %s",__FILE__);
		return;
	}
	MSC_DB_TowerFight *pFightingInfo=(MSC_DB_TowerFight*)pBuf;

	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(pFightingInfo->FightingData.units[i].type == 0)continue;
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_TowerFightExampleAdd");

		m_dbConnector.AddParm("_cpid", pFightingInfo->iCPID);
		m_dbConnector.AddParm("_fid", pFightingInfo->iFID);
		//m_dbConnector.AddParm("_uid", pFightingInfo->FightingData.dwUserID);
		//m_dbConnector.AddParm("_NickName", pFightingInfo->FightingData.units[i].name);
		//m_dbConnector.AddParm("_hpIncRate", pFightingInfo->FightingData.units[i].mcs.fHPPersent);
		//m_dbConnector.AddParm("_dmgIncRate", pFightingInfo->FightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_dmgDesRate", pFightingInfo->FightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Rand", pFightingInfo->iSeed);
		//m_dbConnector.AddParm("_Grade", pFightingInfo->FightingData.units[i].level);
		//m_dbConnector.AddParm("_Score", pFightingInfo->iScore);
		//m_dbConnector.AddParm("_Occupation", pFightingInfo->FightingData.units[i].type);
		//m_dbConnector.AddParm("_Hp", pFightingInfo->FightingData.units[i].mcs.iMaxHP);
		//m_dbConnector.AddParm("_Mp", pFightingInfo->FightingData.units[i].mcs.iMaxMP);
		//m_dbConnector.AddParm("_Strength", pFightingInfo->FightingData.units[i].mcs.iStrength);
		//m_dbConnector.AddParm("_Agility", pFightingInfo->FightingData.units[i].mcs.iAgility);
		//m_dbConnector.AddParm("_Interllect ", pFightingInfo->FightingData.units[i].mcs.iIntellect);
		//m_dbConnector.AddParm("_Spirit", pFightingInfo->FightingData.units[i].mcs.iSpirit);
		//m_dbConnector.AddParm("_HitRate", pFightingInfo->FightingData.units[i].mcs.fHitRate);
		//m_dbConnector.AddParm("_DodgeRate", pFightingInfo->FightingData.units[i].mcs.fDodgeRate);
		//m_dbConnector.AddParm("_CritRate", pFightingInfo->FightingData.units[i].mcs.fCritRate);
		//m_dbConnector.AddParm("_ResilientRate", pFightingInfo->FightingData.units[i].mcs.fResilient);
		//m_dbConnector.AddParm("_AttackAddRate", pFightingInfo->FightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_DamageReduceRate", pFightingInfo->FightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Attack", pFightingInfo->FightingData.units[i].mcs.iAttack);
		//m_dbConnector.AddParm("_Defense", pFightingInfo->FightingData.units[i].mcs.iDefence);
		//m_dbConnector.AddParm("_Pos", pFightingInfo->FightingData.units[i].position);
		//m_dbConnector.AddParm("_Skill1", pFightingInfo->FightingData.units[i].Skills[0].iID);
		//m_dbConnector.AddParm("_Skill2 ", pFightingInfo->FightingData.units[i].Skills[1].iID);
		//m_dbConnector.AddParm("_Skill3", pFightingInfo->FightingData.units[i].Skills[2].iID);
		//m_dbConnector.AddParm("_Skill4", pFightingInfo->FightingData.units[i].Skills[3].iID);
		//m_dbConnector.AddParm("_hprecover", pFightingInfo->FightingData.units[i].mcs.iHPRecover);
		//m_dbConnector.AddParm("_mprecover", pFightingInfo->FightingData.units[i].mcs.iMPRecover);

		G_SaveFightingRecord(m_dbConnector,pFightingInfo->FightingData,i,pFightingInfo->iSeed);


		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			pResult->Release();
		}else
		{
			CCDBLogMes::getSingleton().WriteInfo(pFightingInfo->dwUserID,"SP_TowerFightExampleAdd",
				"_cpid:%d _fid:%d _uid:%d _NickName:%s _hpIncRate:%f _dmgIncRate:%f _dmgDesRate:%f _Rand:%d _Grade:%d _Score:%d _Occupation:%d \
				_Hp:%d _Mp:%d _Strength:%d _Agility:%d _Interllect:%d _Spirit:%d _HitRate:%f _DodgeRate:%f _CritRate:%f _ResilientRate:%f \
				_AttackAddRate:%f _DamageReduceRate:%f _Attack:%d _Defense:%d _Pos:%d _Skill1:%d _Skill2:%d _Skill3:%d _Skill4:%d",
				pFightingInfo->iCPID,pFightingInfo->iFID,pFightingInfo->dwUserID,\
				pFightingInfo->FightingData.units[i].name,pFightingInfo->FightingData.units[i].mcs.fHPPersent,pFightingInfo->FightingData.units[i].mcs.fDmgIncPercent,pFightingInfo->FightingData.units[i].mcs.fDmgDecPercent,\
				pFightingInfo->iSeed,pFightingInfo->FightingData.units[i].level,pFightingInfo->iScore,pFightingInfo->FightingData.units[i].type,\
				pFightingInfo->FightingData.units[i].mcs.iMaxHP,pFightingInfo->FightingData.units[i].mcs.iMaxMP,pFightingInfo->FightingData.units[i].mcs.iStrength,pFightingInfo->FightingData.units[i].mcs.iAgility, pFightingInfo->FightingData.units[i].mcs.iIntellect,\
				pFightingInfo->FightingData.units[i].mcs.iSpirit,pFightingInfo->FightingData.units[i].mcs.fHitRate,pFightingInfo->FightingData.units[i].mcs.fDodgeRate,pFightingInfo->FightingData.units[i].mcs.fCritRate,pFightingInfo->FightingData.units[i].mcs.fResilient,\
				pFightingInfo->FightingData.units[i].mcs.fDmgIncPercent,pFightingInfo->FightingData.units[i].mcs.fDmgDecPercent,pFightingInfo->FightingData.units[i].mcs.iAttack,pFightingInfo->FightingData.units[i].mcs.iDefence,pFightingInfo->FightingData.units[i].position,\
				pFightingInfo->FightingData.units[i].Skills[0].iID,pFightingInfo->FightingData.units[i].Skills[1].iID,pFightingInfo->FightingData.units[i].Skills[2].iID,pFightingInfo->FightingData.units[i].Skills[3].iID);
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}

	//装备技能
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		for(CT_INT32 j=0;j<SKILL_ENUIP_COUNT;++j)
		{
			if(pFightingInfo->FightingData.units[i].type == 0 ||
				pFightingInfo->FightingData.units[i].SkillEnuip.iSkillEquip[j] == 0)continue;

			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_TowerFightSkillAdd");

			m_dbConnector.AddParm("_cpid", pFightingInfo->iCPID);
			m_dbConnector.AddParm("_fid", pFightingInfo->iFID);
			m_dbConnector.AddParm("_skillid", pFightingInfo->FightingData.units[i].SkillEnuip.iSkillEquip[j]);
			m_dbConnector.AddParm("_pos", i);

			if(m_dbConnector.Exec())
			{
				CCDBResult* pResult = m_dbConnector.GetResult();
				pResult->Release();
			}else
			{
				G_DBError(m_dbConnector, m_dwThread);
				return;
			}
		}
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerSlotFight(CT_CHAR* pBuf,CT_DWORD len)
{

	if(len != sizeof(MSC_TowerPVP_ResRobbery))
	{
		CCLogMeg::getSingleton().WriteError("DisposeTowerSlotFight %s",__FILE__);
		return;
	}
	MSC_TowerPVP_ResRobbery *pFightingInfo=(MSC_TowerPVP_ResRobbery*)pBuf;
	
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(pFightingInfo->LeftFightingData.units[i].type == 0)continue;
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_TowerSlotFightExampleAdd");

		m_dbConnector.AddParm("_fid", pFightingInfo->iFID);
		//m_dbConnector.AddParm("_uid", pFightingInfo->dwUserID);
		m_dbConnector.AddParm("_mid", pFightingInfo->LeftFightingData.dwUserID);
		//m_dbConnector.AddParm("_NickName", pFightingInfo->LeftFightingData.units[i].name);
		//m_dbConnector.AddParm("_hpIncRate", pFightingInfo->LeftFightingData.units[i].mcs.fHPPersent);
		//m_dbConnector.AddParm("_dmgIncRate", pFightingInfo->LeftFightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_dmgDesRate", pFightingInfo->LeftFightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Rand", pFightingInfo->iSeed);
		//m_dbConnector.AddParm("_Grade", pFightingInfo->LeftFightingData.units[i].level);
		//m_dbConnector.AddParm("_Score", 0); //pFightingInfo->iScore);
		//m_dbConnector.AddParm("_Occupation", pFightingInfo->LeftFightingData.units[i].type);
		//m_dbConnector.AddParm("_Hp", pFightingInfo->LeftFightingData.units[i].mcs.iMaxHP);
		//m_dbConnector.AddParm("_Mp", pFightingInfo->LeftFightingData.units[i].mcs.iMaxMP);
		//m_dbConnector.AddParm("_Strength", pFightingInfo->LeftFightingData.units[i].mcs.iStrength);
		//m_dbConnector.AddParm("_Agility", pFightingInfo->LeftFightingData.units[i].mcs.iAgility);
		//m_dbConnector.AddParm("_Interllect ", pFightingInfo->LeftFightingData.units[i].mcs.iIntellect);
		//m_dbConnector.AddParm("_Spirit", pFightingInfo->LeftFightingData.units[i].mcs.iSpirit);
		//m_dbConnector.AddParm("_HitRate", pFightingInfo->LeftFightingData.units[i].mcs.fHitRate);
		//m_dbConnector.AddParm("_DodgeRate", pFightingInfo->LeftFightingData.units[i].mcs.fDodgeRate);
		//m_dbConnector.AddParm("_CritRate", pFightingInfo->LeftFightingData.units[i].mcs.fCritRate);
		//m_dbConnector.AddParm("_ResilientRate", pFightingInfo->LeftFightingData.units[i].mcs.fResilient);
		//m_dbConnector.AddParm("_AttackAddRate", pFightingInfo->LeftFightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_DamageReduceRate", pFightingInfo->LeftFightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Attack", pFightingInfo->LeftFightingData.units[i].mcs.iAttack);
		//m_dbConnector.AddParm("_Defense", pFightingInfo->LeftFightingData.units[i].mcs.iDefence);
		//m_dbConnector.AddParm("_Pos", pFightingInfo->LeftFightingData.units[i].position);
		//m_dbConnector.AddParm("_Skill1", pFightingInfo->LeftFightingData.units[i].Skills[0].iID);
		//m_dbConnector.AddParm("_Skill2 ", pFightingInfo->LeftFightingData.units[i].Skills[1].iID);
		//m_dbConnector.AddParm("_Skill3", pFightingInfo->LeftFightingData.units[i].Skills[2].iID);
		//m_dbConnector.AddParm("_Skill4", pFightingInfo->LeftFightingData.units[i].Skills[3].iID);
		//m_dbConnector.AddParm("_hprecover", pFightingInfo->LeftFightingData.units[i].mcs.iHPRecover);
		//m_dbConnector.AddParm("_mprecover", pFightingInfo->LeftFightingData.units[i].mcs.iMPRecover);

		G_SaveFightingRecord(m_dbConnector,pFightingInfo->LeftFightingData,i,pFightingInfo->iSeed);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			pResult->Release();
		}else
		{
			CCDBLogMes::getSingleton().WriteInfo(pFightingInfo->dwUserID,"SP_TowerSlotFightExampleAdd",
				"_uid:%d ",	pFightingInfo->dwUserID);
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}

	//装备技能
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		for(CT_INT32 j=0;j<SKILL_ENUIP_COUNT;++j)
		{
			if(pFightingInfo->LeftFightingData.units[i].type == 0 ||
				pFightingInfo->LeftFightingData.units[i].SkillEnuip.iSkillEquip[j] == 0)continue;

			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_TowerSlotFightSkillAdd");

			m_dbConnector.AddParm("_fid", pFightingInfo->iFID);
			m_dbConnector.AddParm("_uid", pFightingInfo->dwUserID);
			m_dbConnector.AddParm("_mid", pFightingInfo->LeftFightingData.dwUserID);
			m_dbConnector.AddParm("_skillid", pFightingInfo->LeftFightingData.units[i].SkillEnuip.iSkillEquip[j]);
			m_dbConnector.AddParm("_pos", pFightingInfo->LeftFightingData.units[i].position);

			if(m_dbConnector.Exec())
			{
				CCDBResult* pResult = m_dbConnector.GetResult();
				pResult->Release();
			}else
			{
				G_DBError(m_dbConnector, m_dwThread);
				return;
			}
		}
	}

	CT_INT32 iPostMax = POST_COUNT/2;
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(pFightingInfo->RightFightingData.units[i].type == 0)continue;
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_TowerSlotFightExampleAdd");

		m_dbConnector.AddParm("_fid", pFightingInfo->iFID);
		m_dbConnector.AddParm("_mid", pFightingInfo->RightFightingData.dwUserID);
		//m_dbConnector.AddParm("_uid", pFightingInfo->dwUserID);
		//m_dbConnector.AddParm("_NickName", pFightingInfo->RightFightingData.units[i].name);
		//m_dbConnector.AddParm("_hpIncRate", pFightingInfo->RightFightingData.units[i].mcs.fHPPersent);
		//m_dbConnector.AddParm("_dmgIncRate", pFightingInfo->RightFightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_dmgDesRate", pFightingInfo->RightFightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Rand", pFightingInfo->iSeed);
		//m_dbConnector.AddParm("_Grade", pFightingInfo->RightFightingData.units[i].level);
		//m_dbConnector.AddParm("_Score", 0); //pFightingInfo->iScore);
		//m_dbConnector.AddParm("_Occupation", pFightingInfo->RightFightingData.units[i].type);
		//m_dbConnector.AddParm("_Hp", pFightingInfo->RightFightingData.units[i].mcs.iMaxHP);
		//m_dbConnector.AddParm("_Mp", pFightingInfo->RightFightingData.units[i].mcs.iMaxMP);
		//m_dbConnector.AddParm("_Strength", pFightingInfo->RightFightingData.units[i].mcs.iStrength);
		//m_dbConnector.AddParm("_Agility", pFightingInfo->RightFightingData.units[i].mcs.iAgility);
		//m_dbConnector.AddParm("_Interllect ", pFightingInfo->RightFightingData.units[i].mcs.iIntellect);
		//m_dbConnector.AddParm("_Spirit", pFightingInfo->RightFightingData.units[i].mcs.iSpirit);
		//m_dbConnector.AddParm("_HitRate", pFightingInfo->RightFightingData.units[i].mcs.fHitRate);
		//m_dbConnector.AddParm("_DodgeRate", pFightingInfo->RightFightingData.units[i].mcs.fDodgeRate);
		//m_dbConnector.AddParm("_CritRate", pFightingInfo->RightFightingData.units[i].mcs.fCritRate);
		//m_dbConnector.AddParm("_ResilientRate", pFightingInfo->RightFightingData.units[i].mcs.fResilient);
		//m_dbConnector.AddParm("_AttackAddRate", pFightingInfo->RightFightingData.units[i].mcs.fDmgIncPercent);
		//m_dbConnector.AddParm("_DamageReduceRate", pFightingInfo->RightFightingData.units[i].mcs.fDmgDecPercent);
		//m_dbConnector.AddParm("_Attack", pFightingInfo->RightFightingData.units[i].mcs.iAttack);
		//m_dbConnector.AddParm("_Defense", pFightingInfo->RightFightingData.units[i].mcs.iDefence);
		//m_dbConnector.AddParm("_Pos", pFightingInfo->RightFightingData.units[i].position + iPostMax);
		//m_dbConnector.AddParm("_Skill1", pFightingInfo->RightFightingData.units[i].Skills[0].iID);
		//m_dbConnector.AddParm("_Skill2 ", pFightingInfo->RightFightingData.units[i].Skills[1].iID);
		//m_dbConnector.AddParm("_Skill3", pFightingInfo->RightFightingData.units[i].Skills[2].iID);
		//m_dbConnector.AddParm("_Skill4", pFightingInfo->RightFightingData.units[i].Skills[3].iID);
		//m_dbConnector.AddParm("_hprecover", pFightingInfo->RightFightingData.units[i].mcs.iHPRecover);
		//m_dbConnector.AddParm("_mprecover", pFightingInfo->RightFightingData.units[i].mcs.iMPRecover);

		G_SaveFightingRecord(m_dbConnector,pFightingInfo->RightFightingData,i,pFightingInfo->iSeed);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			pResult->Release();
		}else
		{
			CCDBLogMes::getSingleton().WriteInfo(pFightingInfo->dwUserID,"SP_TowerSlotFightExampleAdd",
				"_uid:%d ",	pFightingInfo->dwUserID);
			G_DBError(m_dbConnector, m_dwThread);
			return;
		}
	}

	//装备技能
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		for(CT_INT32 j=0;j<SKILL_ENUIP_COUNT;++j)
		{
			if(pFightingInfo->RightFightingData.units[i].type == 0 ||
				pFightingInfo->RightFightingData.units[i].SkillEnuip.iSkillEquip[j] == 0)continue;

			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_TowerSlotFightSkillAdd");

			m_dbConnector.AddParm("_fid", pFightingInfo->iFID);
			m_dbConnector.AddParm("_uid", pFightingInfo->dwUserID);
			m_dbConnector.AddParm("_mid", pFightingInfo->RightFightingData.dwUserID);
			m_dbConnector.AddParm("_skillid", pFightingInfo->RightFightingData.units[i].SkillEnuip.iSkillEquip[j]);
			m_dbConnector.AddParm("_pos", pFightingInfo->RightFightingData.units[i].position+POST_COUNT/2);

			if(m_dbConnector.Exec())
			{
				CCDBResult* pResult = m_dbConnector.GetResult();
				pResult->Release();
			}else
			{
				G_DBError(m_dbConnector, m_dwThread);
				return;
			}
		}
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerEvent(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(TowerEvent))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::DisposeTowerEvent : len != sizeof(TowerEvent)");
		return;
	}

	TowerEvent *pEvent=(TowerEvent*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerEventAdd");
	m_dbConnector.AddParm("_uid", pEvent->dwUserID);
	m_dbConnector.AddParm("_type", pEvent->iEventType);
	m_dbConnector.AddParm("_cpid", pEvent->iFoor);
	m_dbConnector.AddParm("_oid", pEvent->dwOID);
	m_dbConnector.AddParm("_fid", pEvent->iFightingID);
	m_dbConnector.AddParm("_name", pEvent->cOName);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pEvent->dwUserID, "SP_TowerEventAdd", "_uid:%d", pEvent->dwUserID);
		return;
	}
}

CT_VOID CCDBTowerEvent::DisposeTowerHelp(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_TowerHelp))
	{
		CCLogMeg::getSingleton().WriteError("CCDBTowerEvent::DisposeTowerHelp : len != sizeof(MSC_DB_TowerHelp)");
		return;
	}

	MSC_DB_TowerHelp *pHelp=(MSC_DB_TowerHelp*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerHelpAdd");
	m_dbConnector.AddParm("_uid", pHelp->dwU_ID);
	m_dbConnector.AddParm("_fid", pHelp->dwF_ID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		CCDBLogMes::getSingleton().WriteInfo(pHelp->dwU_ID, "SP_TowerHelpAdd", "_uid:%d", pHelp->dwU_ID);
		return;
	}
}
