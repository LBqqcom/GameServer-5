#include "CLogDBEvent.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CDBConnector.h"
#include "CGlobeFunc.h"

CCLogDBEvent::CCLogDBEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector): 
							CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ARMYUPDATAGRADE);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ARMYUPDATARANK);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_BAGSLOTUPDATA);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_MASTERAREA);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPADDSTRENGTHEN);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPFINISH);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPINHERIT);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PROPUPDATATRAIN);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SAVESKILL);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SAVEEVENTFINISH);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SAVETALENT);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TASKFINISH);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_USEREXIT);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_UPGRADEFINISH);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_VIPAPPLY);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ACTIVEREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ONLINEUSER);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_CHATTIME);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PRODUCTFINISH);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_CHARGE);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_CHARGEUPDATE);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SIGNREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PETUPSKILL);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PETRENAME);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ITEMSELL);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SHOPPING);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_STATMINA);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PVPREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATE);
    RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ENCHANT);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATEREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATEENTER);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PVPFIGHTINGCOST);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ACTIVEEVENTREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_BUYFIGHTINGTIE);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PVPREFRESH);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_CLEARCD);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FEEDBACK);
	RegistMessage(MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_RAIDSSPEED);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_WORSHIP_REWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CHEST);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TEAMDUPLICATEREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TEAMDUPLICATECOST);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SALARY);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_FIGHTTEAM);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_DBLOG_RES_PRODUCT_SAVE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_DBLOG_RES_OCCUPY_SAVE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_RANKINGUPGRADE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_USESITEM);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_EVENTREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_FIGHTTEAM_UPGRADE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_FIGHTTEAM_SUMMONCOST);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDRES);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDITEM);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_FIGHTTEAM_SCIENCE_UPGRADE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TOWER_PASS);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TOWER_OCCUPY);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TOWER_HELP);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_ADDSITEM);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_BATTLEFLAG);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_EATELM);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TREASUREUSE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_EMBEDEQUIP);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CSREWARD);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_INSPORE_COST);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SUPPORT);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_PETUPGRADE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TITLE);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_ISLAND);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_WALLADDITION);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_PLUNDER);
	RegistMessage(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SEEKDATA);
	
}

CCLogDBEvent::~CCLogDBEvent(void)
{
}

//处理用户消息
CT_VOID CCLogDBEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	if(MSC_MAIN_LOGDB_DATA == mainID)
	{
		switch(subID)
		{
		case MSC_SUB_LOGDB_ARMYUPDATAGRADE://
			{
				DisposeArmyUpdataGrade(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_ARMYUPDATARANK://暂时无法进阶
			{
				DisposeArmyUpdataRank(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_BAGSLOTUPDATA://
			{
				DisposeBagSlotUpdata(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_MASTERAREA://
			{
				DisposeMasterArea(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PROPADDSTRENGTHEN://
			{
				DisposePropAddStrengthen(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PROPFINISH://
			{
				DisposePropFinish(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PROPINHERIT://
			{
				DisposePropInherit(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PROPUPDATATRAIN://
			{
				DisposePropUpdataTrain(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_SAVESKILL://未加
			{
				DisposeSaveSkill(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_SAVEEVENTFINISH://
			{
				DisposeSaveEventFinish(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_SAVETALENT://
			{
				DisposeSaveTalent(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_TASKFINISH://
			{
				DisposeTaskFinish(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_USEREXIT://
			{
				DispseUserExit(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_UPGRADEFINISH://
			{
				DisposeUpGradeFinish(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_VIPAPPLY:
			{
				DisposeVIPApply(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_ACTIVEREWARD:
			{
				DisposeActiveReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_ONLINEUSER:
			{
				DisposeOnlineUser(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_CHATTIME:
			{
				DisposeChatTime(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PRODUCTFINISH:
			{
				DisposeProductFinish(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_CHARGE:
			{
				DisposeCharge(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_CHARGEUPDATE:
			{
				DisposeChargeUpdate(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_SIGNREWARD:
			{
				DisposeSignReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PETUPSKILL:
			{
				DisposePetUpSkill(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PETRENAME:
			{
				DisposePetRename(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_ITEMSELL:
			{
				DisposeItemSell(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_SHOPPING:
			{
				DisposeShopping(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_STATMINA:
			{
				DisposeStatminaBuy(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PVPREWARD:
			{
				DisposePVPReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_DUPLICATE:
			{
				DisposeDuplicate(pBuf,len);
				return;
			}
        case MSC_SUB_LOGDB_ENCHANT:
            {
                DisposeEnchant(pBuf,len);
                return;
            }
		case MSC_SUB_LOGDB_DUPLICATEREWARD:
			{
				DisposeDuplicateReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_DUPLICATEENTER:
			{
				DisposeDuplicateEnter(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PVPFIGHTINGCOST:
			{
				DisposePVPFightingCost(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_ACTIVEEVENTREWARD:
			{
				DisposeActiveEventReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_BUYFIGHTINGTIE:
			{
				DisposeBuyFightingTime(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PVPREFRESH:
			{
				DisposePVPRefresh(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_CLEARCD:
			{
				DisposeClearCD(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_FEEDBACK:
			{
				DisposeFeedBack(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_RAIDSSPEED:
			{
				DisposeRaidsSpeed(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_WORSHIP_REWARD:
			{
				DisposeWorshipReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_CHEST:
			{
				DisposeOpenChest(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_TEAMDUPLICATEREWARD:
			{
				DiposeTeamDuplicateReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_TEAMDUPLICATECOST:
			{
				DisposeTeamDuplicateCost(pBuf,len);
				return;
			}
		case  MSC_SUB_DBLOG_RES_PRODUCT_SAVE:
			{
				DisposeResProductSave(pBuf, len);
				return;
			}
		
		case MSC_SUB_LOGDB_FIGHTTEAM:
			{
				DisposeFightTeam(pBuf,len);
				return;
			}
		case  MSC_SUB_DBLOG_RES_OCCUPY_SAVE:
			{
				DisposeResOccpySave(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_SALARY:
			{
				DisposeSalary(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_RANKINGUPGRADE:
			{
				DisposeRankingUpGrade(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_USESITEM:
			{
				DisposeUserItem(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_EVENTREWARD:
			{
				DisposeEventReward(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_PETUPGRADE:
			{
				DisposePetUpGrade(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_MASTER_DATA://未测
			{
				DisposeMasterSaveData(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_FIGHTTEAM_UPGRADE:
			{
				DisposeFightTeamUpGrade(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_FIGHTTEAM_SUMMONCOST:
			{
				DisposeFightTeamSummonCost(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDRES:
			{
				DisposeFightTeamSummonRewardRes(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDITEM:
			{
				DisposeFightTeamSummonRewardItem(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_FIGHTTEAM_SCIENCE_UPGRADE:
			{
				DisposeFightTeamScienceUpGrade(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_TOWER_PASS:
			{
				DisposeTowerPass(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_TOWER_OCCUPY:
			{
				DisposeTowerOccupy(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_TOWER_HELP:
			{
				DisposeTowerHelp(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_ADDSITEM:
			{
				DisposeAddItem(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_BATTLEFLAG:
			{
				DisposeBattleFlag(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_EATELM:
			{
				DisposeEatElm(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_TREASUREUSE:
			{
				DisposeTreasureUse(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_EMBEDEQUIP:
			{
				DisposeEmbedEquip(pBuf, len);
				return;
			}
		case MSC_SUB_LOGDB_CSREWARD:
			{
				DisposeCSReward(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_INSPORE_COST:
			{
				DisposeInsporeCost(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_SUPPORT:
			{
				DiposeSupport(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_TITLE:
			{
				DisposeTitle(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_ISLAND:
			{
				DisposeIsland(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_WALLADDITION:
			{
				DisposeWallAddition(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_PLUNDER:
			{
				DisposePlunder(pBuf,len);
				return;
			}
		case MSC_SUB_LOGDB_SEEKDATA:
			{
				DisposeSeekData(pBuf,len);
				return;
			}
		default:
			{
				
				CCLogMeg::getSingleton().WriteError("没有发现数据库日志的消息mid=%i, sid=%i", mainID, subID);
				return;
			}
		}
	}

	CCLogMeg::getSingleton().WriteError("CCLogDBEvent::OnUserEvent mainID:%d\tSubID:%d",mainID,subID);
	return;
}

//用户定时器处理
CT_VOID CCLogDBEvent::OnTime(CT_DWORD timeID)
{
	return;
}

//军队升级记录
CT_VOID CCLogDBEvent::DisposeArmyUpdataGrade(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_W_ArmyGrade) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeArmyUpdataGrade sizeof(MSC_DB_R_ArmyGrade) != iLen");
		return;
	}

	MSC_LOGDB_W_ArmyGrade *pArmyGrade=(MSC_LOGDB_W_ArmyGrade*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyUpdateGradeRecordAdd");

	m_dbConnector.AddParm("_uid", pArmyGrade->dwUserID);
	m_dbConnector.AddParm("_aid", pArmyGrade->iArmyID);
	m_dbConnector.AddParm("_grade", pArmyGrade->iGrade);
	m_dbConnector.AddParm("_money ", pArmyGrade->iGold);
	m_dbConnector.AddParm("_food ", pArmyGrade->iFood);
	m_dbConnector.AddParm("_propID ", pArmyGrade->iGoods);
	m_dbConnector.AddParm("_propNum ", pArmyGrade->iCount);
	m_dbConnector.AddParm("_ugrade",   pArmyGrade->iUserGarde);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//军队进阶
CT_VOID CCLogDBEvent::DisposeArmyUpdataRank(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_ArmyAdvanced) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeArmyUpdataRank sizeof(MSC_DB_R_ArmyAdvanced) != iLen");
		return;
	}

	MSC_LOGDB_ArmyAdvanced *pArmyAdvanced=(MSC_LOGDB_ArmyAdvanced*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyUpdateRankRecordAdd");

	m_dbConnector.AddParm("_uid", pArmyAdvanced->dwUserID);
	m_dbConnector.AddParm("_aidold", pArmyAdvanced->iOldArmyID);
	m_dbConnector.AddParm("_aidnew", pArmyAdvanced->iNewArmyID);
	m_dbConnector.AddParm("_money ", pArmyAdvanced->iGold);
	m_dbConnector.AddParm("_food ", 0);
	m_dbConnector.AddParm("_propID1 ", pArmyAdvanced->iGoods[0]);
	m_dbConnector.AddParm("_propNum1 ", pArmyAdvanced->iCount[0]);
	m_dbConnector.AddParm("_propID2 ", pArmyAdvanced->iGoods[1]);
	m_dbConnector.AddParm("_propNum2 ", pArmyAdvanced->iCount[1]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}


//购买背包格子记录
CT_VOID CCLogDBEvent::DisposeBagSlotUpdata(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_BagSlot) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeBagSlotUpdata sizeof(MSC_DB_W_BagSlot) != iLen");
		return;
	}

	MSC_LOGDB_BagSlot *pBagSlot=(MSC_LOGDB_BagSlot*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BagSlotUpdateRecordAdd");

	m_dbConnector.AddParm("_uid", pBagSlot->dwUserID);
	m_dbConnector.AddParm("_slots", pBagSlot->nSlot);
	m_dbConnector.AddParm("_diamond", pBagSlot->iDiamond);
	m_dbConnector.AddParm("_ugrade",  pBagSlot->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}


//增加城市记录
CT_VOID CCLogDBEvent::DisposeMasterArea(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_LOGDB_RegionUnlockData))
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeMasterArea sizeof(MSC_DB_W_RegionUnlockData) != iLen");
		return;
	}
	MSC_LOGDB_RegionUnlockData *pRegionUnlockData=(MSC_LOGDB_RegionUnlockData*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MasterAreaRecordAdd");

	m_dbConnector.AddParm("_uid",       pRegionUnlockData->dwUserID);
	m_dbConnector.AddParm("_aid",       pRegionUnlockData->iRegionID);
	m_dbConnector.AddParm("_money",     pRegionUnlockData->iGold);
	m_dbConnector.AddParm("_diamod",	pRegionUnlockData->iDiamod);
	m_dbConnector.AddParm("_ugrade",    pRegionUnlockData->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}


//道具增强
CT_VOID CCLogDBEvent::DisposePropAddStrengthen(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_SmithingAndSlot) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePropAddStrengthen sizeof(MSC_DB_W_SmithingAndSlot) != iLen");
		return;
	}

	MSC_LOGDB_SmithingAndSlot *pSmithingAndSlot=(MSC_LOGDB_SmithingAndSlot*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropAddStrengthenRecord");

	m_dbConnector.AddParm("_uid", pSmithingAndSlot->dwUserID);
	m_dbConnector.AddParm("_index", pSmithingAndSlot->key);
	m_dbConnector.AddParm("_pid", pSmithingAndSlot->iEquipID);
	m_dbConnector.AddParm("_grade", pSmithingAndSlot->iLevel);
	m_dbConnector.AddParm("_holes", pSmithingAndSlot->nSlot);
	m_dbConnector.AddParm("_diamods", pSmithingAndSlot->iDiamond);
	m_dbConnector.AddParm("_money", pSmithingAndSlot->iGold);
	m_dbConnector.AddParm("_propID", pSmithingAndSlot->iItemID);
	m_dbConnector.AddParm("_propNum", pSmithingAndSlot->iItemNum);
	m_dbConnector.AddParm("_ugrade",  pSmithingAndSlot->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//道具使用完记录
CT_VOID CCLogDBEvent::DisposePropFinish(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_LOGDB_PropFinish) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePropFinish sizeof(CMD_LOGDB_PropFinish) != iLen");
		return;
	}

	CMD_LOGDB_PropFinish *pPropFinish=(CMD_LOGDB_PropFinish*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropFinishAddRecord");

	m_dbConnector.AddParm("_uid",       pPropFinish->dwUserID);
	m_dbConnector.AddParm("_pid",       pPropFinish->iID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//道具继承记录
CT_VOID CCLogDBEvent::DisposePropInherit(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_InheritData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePropInherit sizeof(MSC_DB_W_InheritData) != iLen");
		return;
	}

	MSC_LOGDB_InheritData *pInheritData=(MSC_LOGDB_InheritData*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropInheritAddRecord");

	m_dbConnector.AddParm("_uid", pInheritData->dwUserID);
	m_dbConnector.AddParm("_index1", pInheritData->keySrc);
	m_dbConnector.AddParm("_pid1", pInheritData->iSrcID);
	m_dbConnector.AddParm("_grade1", pInheritData->iLvSrc);
	m_dbConnector.AddParm("_index2", pInheritData->keyDst);
	m_dbConnector.AddParm("_pid2", pInheritData->iDstID);
	m_dbConnector.AddParm("_money", pInheritData->iGold);
	m_dbConnector.AddParm("_diamod", pInheritData->iDiamond);
	m_dbConnector.AddParm("_propID", pInheritData->iItemID);
	m_dbConnector.AddParm("_propNum", pInheritData->iItemNum);
	m_dbConnector.AddParm("_ugrade", pInheritData->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//道具增强
CT_VOID CCLogDBEvent::DisposePropUpdataTrain(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_PropAddStrengthen) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePropAddStrengthen sizeof(MSC_DB_W_SmithingAndSlot) != iLen");
		return;
	}

	MSC_LOGDB_PropAddStrengthen *pSmithingAndSlot=(MSC_LOGDB_PropAddStrengthen*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropUpdateTrainAddRecord");

	m_dbConnector.AddParm("_uid", pSmithingAndSlot->dwUserID);
	m_dbConnector.AddParm("_index", pSmithingAndSlot->iIndex);
	m_dbConnector.AddParm("_pid", pSmithingAndSlot->iEquipID);
	m_dbConnector.AddParm("_money", pSmithingAndSlot->iGold);
	m_dbConnector.AddParm("_diamod", pSmithingAndSlot->iDiamond);
	m_dbConnector.AddParm("_propID", pSmithingAndSlot->iItemID);
	m_dbConnector.AddParm("_propNum", pSmithingAndSlot->iItemNum);
	m_dbConnector.AddParm("_ugrade", pSmithingAndSlot->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//保存技能记录
CT_VOID CCLogDBEvent::DisposeSaveSkill(CT_CHAR *pBuf,CT_INT32 iLen)
{
#if 0
	if(sizeof(MSC_DB_W_SmithingAndSlot) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePropAddStrengthen sizeof(MSC_DB_W_SmithingAndSlot) != iLen");
		return;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SaveSkillRecord");

	//m_dbConnector.AddParm("_uid",       skillSaveData.dwUserID);
	//m_dbConnector.AddParm("_sid",       skillSaveData.iSkillID);
	//m_dbConnector.AddParm("_name",      (CT_CHAR *)skillSaveData.szName);
	//m_dbConnector.AddParm("_gradeInc",  skillSaveData.iGradeInc);
	//m_dbConnector.AddParm("_xp",        skillSaveData.iExp);
	//m_dbConnector.AddParm("_use",       skillSaveData.Used);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
#endif
	return;
}


//事件完成记录
CT_VOID CCLogDBEvent::DisposeSaveEventFinish(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_EventAward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeSaveEventFinish sizeof(MSC_DB_EventAward) != iLen");
		return;
	}
	MSC_LOGDB_EventAward *pEventAward=(MSC_LOGDB_EventAward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SaveEventFinishRecord");

	m_dbConnector.AddParm("_uid",   pEventAward->dwUserID);
	m_dbConnector.AddParm("_food",  pEventAward->iFood);
	m_dbConnector.AddParm("_gold",  pEventAward->iGold);
	m_dbConnector.AddParm("_civiliXp", pEventAward->iCiviliNum);

	m_dbConnector.AddParm("prop1Name", (CT_CHAR*)pEventAward->cProp1Name );
	m_dbConnector.AddParm("prop1",	   pEventAward->iProp1);
	m_dbConnector.AddParm("prop1num",  pEventAward->iPropNum1);
	m_dbConnector.AddParm("prop2Name", (CT_CHAR*)pEventAward->cProp2Name);
	m_dbConnector.AddParm("prop2",     pEventAward->iProp2);
	m_dbConnector.AddParm("prop2num",  pEventAward->iPropNum2);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//保存天赋记录
CT_VOID CCLogDBEvent::DisposeSaveTalent(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_TalentData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeSaveTalent sizeof(MSC_DB_W_TalentData) != iLen");
		return;
	}
	MSC_LOGDB_TalentData *pTalentData=(MSC_LOGDB_TalentData*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SaveTalentRecord");

	m_dbConnector.AddParm("_uid",   pTalentData->dwUserID);
	m_dbConnector.AddParm("_tid",   pTalentData->iTalentID);
	m_dbConnector.AddParm("_xp",    pTalentData->iExp);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//保存任务完成记录
CT_VOID CCLogDBEvent::DisposeTaskFinish(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_TaskReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeTaskFinish sizeof(TaskRewardUserID) != iLen");
		return;
	}
	MSC_LOGDB_TaskReward *pTaskReward=(MSC_LOGDB_TaskReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TaskFinishRecordAdd");

	m_dbConnector.AddParm("_uid",pTaskReward->dwUserID);
	m_dbConnector.AddParm("_mid",pTaskReward->iTaskID);
	m_dbConnector.AddParm("_money",pTaskReward->iGold);
	m_dbConnector.AddParm("_diamod",pTaskReward->iDiamod);
	m_dbConnector.AddParm("_xp",pTaskReward->iXp);
	m_dbConnector.AddParm("_food",pTaskReward->iFood);
	m_dbConnector.AddParm("_talent",pTaskReward->iPotential);
	m_dbConnector.AddParm("_skillsoul",pTaskReward->iSoul);
	m_dbConnector.AddParm("_rankxp",pTaskReward->iPrestige);
	m_dbConnector.AddParm("_civilxp",pTaskReward->iCiviliNum);
	m_dbConnector.AddParm("_stamina",pTaskReward->iStamina);
	m_dbConnector.AddParm("_propID0",pTaskReward->iItemID1);
	m_dbConnector.AddParm("_prop0num",pTaskReward->iNum1);
	m_dbConnector.AddParm("_propID1",pTaskReward->iItemID2);
	m_dbConnector.AddParm("_prop1num",pTaskReward->iNum2);
	m_dbConnector.AddParm("_propID2",pTaskReward->iItemID3);
	m_dbConnector.AddParm("_prop2num",pTaskReward->iNum3);
	m_dbConnector.AddParm("_propID3",pTaskReward->iItemID4);
	m_dbConnector.AddParm("_prop3num",pTaskReward->iNum4);
	m_dbConnector.AddParm("_ugrade", pTaskReward->iUserGrade);
	m_dbConnector.AddParm("_diamodcost", pTaskReward->iDiamodCost);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//用户退出记录
CT_VOID CCLogDBEvent::DispseUserExit(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_UserExit) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DispseUserExit sizeof(MSC_DB_UserExit) != iLen");
		return;
	}

	MSC_LOGDB_UserExit *pUserExit=(MSC_LOGDB_UserExit*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UserExitRecordAdd");

	m_dbConnector.AddParm("_uid",     pUserExit->dwUserID );

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//用户生产记录
CT_VOID CCLogDBEvent::DisposeUpGradeFinish(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_AddUpgradeRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeUpGradeFinish sizeof(MSC_LOGDB_AddUpgradeRecord) != iLen");
		return;
	}

	MSC_LOGDB_AddUpgradeRecord *p=(MSC_LOGDB_AddUpgradeRecord*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UpgradeFinishRecordAdd");

	m_dbConnector.AddParm("_uid",			p->dwUserID);
	m_dbConnector.AddParm("_cid ",           p->iBid);
	m_dbConnector.AddParm("_diamod",        p->iDiamod);
	m_dbConnector.AddParm("_gold",			p->iGold);
	m_dbConnector.AddParm("_worker",		p->iWorker);
	m_dbConnector.AddParm("_food",          p->iFood);
	m_dbConnector.AddParm("_prop1",         p->iProp1);
	m_dbConnector.AddParm("_prop1Num",      p->iProp1Num);
	m_dbConnector.AddParm("_prop2",         p->iProp2);
	m_dbConnector.AddParm("_prop2Num",      p->iProp2Num);
	m_dbConnector.AddParm("_prop3",         p->iProp3);
	m_dbConnector.AddParm("_prop3Num",      p->iProp2Num);
	m_dbConnector.AddParm("_prop4",         p->iProp4);
	m_dbConnector.AddParm("_prop4Num",      p->iProp4Num);
	m_dbConnector.AddParm("_civiNum",       p->iCivil);
	//m_dbConnector.AddParm("_speedTimes",    p->iSpeedTimes);
	m_dbConnector.AddParm("_xp",           p->iXp);
	m_dbConnector.AddParm("_upgradeTimes", p->iUpgradeTimes);
	m_dbConnector.AddParm("_ugrade",       p->iUserGrade);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


//vip申请记录
CT_VOID CCLogDBEvent::DisposeVIPApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_VipApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeVIPApply sizeof(MSC_DB_VipApply) != iLen");
		return;
	}
	MSC_LOGDB_VipApply *pVipApply=(MSC_LOGDB_VipApply*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_VipApplyRecordAdd");

	m_dbConnector.AddParm("_uid",pVipApply->dwUserID);
	m_dbConnector.AddParm("_propID",pVipApply->iPropID);
	m_dbConnector.AddParm("_propNum ",pVipApply->iPropNum);
	m_dbConnector.AddParm("_diamod ",pVipApply->iDiamod);
	m_dbConnector.AddParm("_days",pVipApply->iDays);
	m_dbConnector.AddParm("_gradeNew",pVipApply->iGradeOpen);
	m_dbConnector.AddParm("_ugrade", pVipApply->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeActiveReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_ActiveReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeActiveReward sizeof(MSC_LOGDB_ActiveReward) != iLen");
		return;
	}
	MSC_LOGDB_ActiveReward *p=(MSC_LOGDB_ActiveReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EveryDayAddVitalityRecord");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_vitalityGrade", p->iLevel);
	m_dbConnector.AddParm("_money", p->iGold);
	m_dbConnector.AddParm("_food", p->iFood);
	m_dbConnector.AddParm("_stamina", p->iStamina);
	m_dbConnector.AddParm("_potential", p->iTalentNum);
	m_dbConnector.AddParm("_rankXp", p->iRankXp);
	m_dbConnector.AddParm("_prop0", p->ItemReward[0].iID);
	m_dbConnector.AddParm("_prop0num", p->ItemReward[0].iNum);
	m_dbConnector.AddParm("_prop1", p->ItemReward[1].iID);
	m_dbConnector.AddParm("_prop1num", p->ItemReward[1].iNum);
	m_dbConnector.AddParm("_prop2", p->ItemReward[2].iID);
	m_dbConnector.AddParm("_prop2num", p->ItemReward[2].iNum);
	m_dbConnector.AddParm("_prop3", p->ItemReward[3].iID);
	m_dbConnector.AddParm("_prop3num", p->ItemReward[3].iNum);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//打开宝箱
CT_VOID CCLogDBEvent::DisposeOpenChest(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_LOGDB_Chest))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPInfo :len != sizeof(MSC_LOGDB_Chest)");
		return;
	}


	MSC_LOGDB_Chest *pChest=(MSC_LOGDB_Chest*)pBuf;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPSaveTreasureRecord");

	m_dbConnector.AddParm("_uid", pChest->dwUserID);
	m_dbConnector.AddParm("_money", pChest->iGold);
	m_dbConnector.AddParm("_talent", pChest->iTalentNum);
	m_dbConnector.AddParm("_rankxp", pChest->iPrestige);
	m_dbConnector.AddParm("_prop0", pChest->iGoodsID1);
	m_dbConnector.AddParm("_prop0Num", pChest->iGoodsCount1);
	m_dbConnector.AddParm("_prop1", pChest->iGoodsID2);
	m_dbConnector.AddParm("_prop1Num", pChest->iGoodsCount2);
	m_dbConnector.AddParm("_prop2", pChest->iGoodsID3);
	m_dbConnector.AddParm("_prop2Num", pChest->iGoodsCount3);
	m_dbConnector.AddParm("_prop3", 0);
	m_dbConnector.AddParm("_prop3Num", 0);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeOnlineUser(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CT_INT32) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeOnlineUser sizeof(int) != iLen");
		return;
	}
	CT_INT32 *p = (CT_INT32*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_OnlineRecordAdd");

	m_dbConnector.AddParm("_onlineusernum", *p);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeChatTime(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_ChatTime) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeChatTime sizeof(MSC_LOGDB_ChatTime) != iLen");
		return;
	}
	MSC_LOGDB_ChatTime *p = (MSC_LOGDB_ChatTime*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ChatRecordAdd");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_chattimes", p->iChatTime);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeProductFinish(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_AddProductingRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeProductFinish sizeof(MSC_LOGDB_AddProductingRecord) != iLen");
		return;
	}

	MSC_LOGDB_AddProductingRecord  *p = (MSC_LOGDB_AddProductingRecord*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ProductingBuildingAddRecord");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_bid ", p->iBid);
	m_dbConnector.AddParm("_gold ", p->iGold);
	m_dbConnector.AddParm("_food", p->iFood);
	m_dbConnector.AddParm("_worker", p->iWorker);
	m_dbConnector.AddParm("_stamina", p->iStamina);
	m_dbConnector.AddParm("_civiNum", p->iCivil);
	m_dbConnector.AddParm("_diamod", p->iDiamod);
	m_dbConnector.AddParm("_prop1", p->iProp1);
	m_dbConnector.AddParm("_prop1Num", p->iProp1Num);
	m_dbConnector.AddParm("_prop2", p->iProp2);
	m_dbConnector.AddParm("_prop2Num", p->iProp2Num);
	//m_dbConnector.AddParm("_speeduptimes", p->iSpeedupTimes);
	m_dbConnector.AddParm("_productedTimes", p->iProductingTimes);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeCharge(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_Charge) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeCharge sizeof(MSC_LOGDB_Charge) != iLen");
		return;
	}

	MSC_LOGDB_Charge  *p = (MSC_LOGDB_Charge*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ChargeRecordAdd");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_money", p->iPrice);
	m_dbConnector.AddParm("_diamod", p->iDiamond);
	m_dbConnector.AddParm("_state", p->iState);
	m_dbConnector.AddParm("_sign", (CT_CHAR*)p->szSign);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeChargeUpdate(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_UpdateCharge) > iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeChargeUpdate sizeof(MSC_LOGDB_UpdateCharge) != iLen");
		return;
	}

	MSC_LOGDB_UpdateCharge  *p = (MSC_LOGDB_UpdateCharge*)pBuf;
	CT_INT32 iAmount = 0;
	CT_INT32 iDiamond = 0;

	if (iLen == sizeof(MSC_LOGDB_UpdateCharge_EX))
	{
		MSC_LOGDB_UpdateCharge_EX *pex = (MSC_LOGDB_UpdateCharge_EX *)pBuf;
		iAmount = pex->iMoney;
		iDiamond = pex->iDiamond;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ChargeRecordUpdate");

	m_dbConnector.AddParm("_sign", (CT_CHAR *)p->szSign);
	m_dbConnector.AddParm("_cIndex", p->iIndex);
	m_dbConnector.AddParm("_state", p->iState);
	m_dbConnector.AddParm("_money", iAmount);
	m_dbConnector.AddParm("_diamond", iDiamond);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeSignReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_SignReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeSignReward sizeof(MSC_LOGDB_SignReward) != iLen");
		return;
	}

	MSC_LOGDB_SignReward  *p = (MSC_LOGDB_SignReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EveryDayAddLoginInRecord");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_loginInTimes", p->nSigned);
	m_dbConnector.AddParm("_money", p->iGold);
	m_dbConnector.AddParm("_food", p->iFood);
	m_dbConnector.AddParm("_stamina", p->iStamina);
	m_dbConnector.AddParm("_potential", p->iTalentNum);
	m_dbConnector.AddParm("_rankXp", p->iRankXp);
	m_dbConnector.AddParm("_prop0", p->ItemReward[0].iID);
	m_dbConnector.AddParm("_prop0num", p->ItemReward[0].iNum);
	m_dbConnector.AddParm("_prop1", p->ItemReward[1].iID);
	m_dbConnector.AddParm("_prop1num", p->ItemReward[1].iNum);
	m_dbConnector.AddParm("_prop2", p->ItemReward[2].iID);
	m_dbConnector.AddParm("_prop2num", p->ItemReward[2].iNum);
	m_dbConnector.AddParm("_prop3", p->ItemReward[3].iID);
	m_dbConnector.AddParm("_prop3num", p->ItemReward[3].iNum);
	m_dbConnector.AddParm("_diamond", p->iCostDiamond);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposePetUpSkill(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_PETUPSKILL) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePetUpSkill sizeof(MSC_LOGDB_PETUPSKILL) != iLen");
		return;
	}

	MSC_LOGDB_PETUPSKILL  *p = (MSC_LOGDB_PETUPSKILL*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetUpSkillRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_skill",p->iSkillID);
	m_dbConnector.AddParm("_pid",p->iItemID);
	m_dbConnector.AddParm("_inum ",p->iNum);
	m_dbConnector.AddParm("_glod ",p->iGold);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposePetRename(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_PetRename) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePetRename sizeof(MSC_LOGDB_PetRename) != iLen");
		return;
	}

	MSC_LOGDB_PetRename  *p = (MSC_LOGDB_PetRename*)pBuf;
	
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetChangeNameRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_pid",p->iPetID);
	m_dbConnector.AddParm("_index",p->iIndex);
	m_dbConnector.AddParm("_propid",p->iPropID);
	m_dbConnector.AddParm("_propnum",p->iPropNum);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeItemSell(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_ItemSell) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeItemSell sizeof(MSC_LOGDB_ItemSell) != iLen");
		return;
	}

	MSC_LOGDB_ItemSell  *p = (MSC_LOGDB_ItemSell*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropSellRecordAdd");
	
	m_dbConnector.AddParm("_pid",p->iPropID);
	m_dbConnector.AddParm("_pnum",p->iPropNum);
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_gold ",p->iGlod);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeShopping(CT_CHAR *pBuf,CT_INT32 iLen)
{	
	if(sizeof(MSC_LOGDB_ShoppingRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeShopping sizeof(MSC_LOGDB_ShoppingRecord) != iLen");
		return;
	}

	MSC_LOGDB_ShoppingRecord  *p = (MSC_LOGDB_ShoppingRecord*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ShopAddBuyRecord");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_propid",p->iGoodsID);
	m_dbConnector.AddParm("_propnum",p->iGoodsNum);
	m_dbConnector.AddParm("_diamod",p->iDiamod);
	m_dbConnector.AddParm("_money",p->iGold);
	m_dbConnector.AddParm("_propused",p->iItemID);
	m_dbConnector.AddParm("_propusednum",p->iItemNum);
	m_dbConnector.AddParm("shoptype)",p->iMarketType);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeStatminaBuy(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_BuyStatmina) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeStatminaBuy sizeof(MSC_LOGDB_BuyStatmina) != iLen");
		return;
	}

	MSC_LOGDB_BuyStatmina  *p = (MSC_LOGDB_BuyStatmina*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ShopBuyStaminaRecordAdd");
	
	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_stamina", p->iStatNum);
	m_dbConnector.AddParm("_diamod ", p->iDiamod);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposePVPReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_PVPReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePVPReward sizeof(MSC_LOGDB_PVPReward) != iLen");
		return;
	}

	MSC_LOGDB_PVPReward  *p = (MSC_LOGDB_PVPReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPSaveFightAwardRecord");

	m_dbConnector.AddParm("_index", p->cIndex);
	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_money",p->iGold);
	m_dbConnector.AddParm("_rankxp", p->iPrestige);
	m_dbConnector.AddParm("_prop0", p->iGoods[0]);
	m_dbConnector.AddParm("_prop0Num", p->iCount[0]);
	m_dbConnector.AddParm("_prop1", p->iGoods[1]);
	m_dbConnector.AddParm("_prop1Num", p->iCount[1]);
	m_dbConnector.AddParm("_prop2", p->iGoods[2]);
	m_dbConnector.AddParm("_prop2Num", p->iCount[2]);
	m_dbConnector.AddParm("_prop3", p->iGoods[3]);
	m_dbConnector.AddParm("_prop3Num", p->iCount[3]);
	m_dbConnector.AddParm("_bVictory", p->iResult);
	m_dbConnector.AddParm("_aimUID ", p->dwAimID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeDuplicate(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_FightRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeDuplicate sizeof(MSC_LOGDB_FightRecord) != iLen");
		return;
	}

	MSC_LOGDB_FightRecord  *p = (MSC_LOGDB_FightRecord*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightAddRecord");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_aid", p->iAid);
	m_dbConnector.AddParm("_fid", p->iFid);
	m_dbConnector.AddParm("_diffcult", p->iDifficulty);
	m_dbConnector.AddParm("_xp", p->iXp);
	m_dbConnector.AddParm("_money",p->iGold);
	m_dbConnector.AddParm("_talent", p->iTalent);
	m_dbConnector.AddParm("_diamod", p->iDiamod);
	m_dbConnector.AddParm("_skillnum", p->iSkillNum);
	m_dbConnector.AddParm("_propid1", p->iPropID[0]);
	m_dbConnector.AddParm("_prop1num", p->iPropNum[0]);
	m_dbConnector.AddParm("_propid2", p->iPropID[1]);
	m_dbConnector.AddParm("_prop2num", p->iPropNum[1]);
	m_dbConnector.AddParm("_propid3", p->iPropID[2]);
	m_dbConnector.AddParm("_prop3num", p->iPropNum[2]);
	m_dbConnector.AddParm("_propid4", p->iPropID[3]);
	m_dbConnector.AddParm("_prop4num", p->iPropNum[3]);
	m_dbConnector.AddParm("_win ", p->iWin);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

// 附魔记录
CT_VOID CCLogDBEvent::DisposeEnchant(CT_CHAR *pBuf,CT_INT32 iLen)
{
    if(sizeof(MSC_LOGDB_Enchant) != iLen)
    {
        CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeEnchant sizeof(MSC_LOGDB_Enchant) != iLen");
        return;
    }

    MSC_LOGDB_Enchant  *p = (MSC_LOGDB_Enchant*)pBuf;

    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EnchantRecordAdd");

    m_dbConnector.AddParm("_uid", p->dwUserID);
    m_dbConnector.AddParm("_Diamond", p->nDiamondCost);
    m_dbConnector.AddParm("_Talent", p->nTalentNumCost);
    m_dbConnector.AddParm("_gold", p->nGoldCost);
    m_dbConnector.AddParm("_Pos", p->iIndex);
    m_dbConnector.AddParm("_Grade",p->iGrade);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

    if(m_dbConnector.Exec())
    {
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
    }
    else
    {
        G_DBError(m_dbConnector, m_dwThread);
        return;
    }
}

//副本奖励领取
CT_VOID CCLogDBEvent::DisposeDuplicateReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_DuplicateReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeDuplicateReward sizeof(MSC_LOGDB_DuplicateReward) != iLen");
		return;
	}

	MSC_LOGDB_DuplicateReward  *p = (MSC_LOGDB_DuplicateReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorldScensRewardRecordAdd");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_worldsceneID", p->iWorldSceneID);
	m_dbConnector.AddParm("_exp", p->iExperience);
	m_dbConnector.AddParm("_gold", p->iGold);
	m_dbConnector.AddParm("_score",p->iScore);
	m_dbConnector.AddParm("_propid1",p->iGoodsID1);
	m_dbConnector.AddParm("_propid1num",p->iCount1);
	m_dbConnector.AddParm("_propid2",p->iGoodsID2);
	m_dbConnector.AddParm("_propid2num",p->iCount2);
	m_dbConnector.AddParm("_talent",p->iPotential);
	m_dbConnector.AddParm("_diamod",p->iDiamond);
	m_dbConnector.AddParm("_skillpoint",p->iSkillPrint);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//副本进入消耗
CT_VOID CCLogDBEvent::DisposeDuplicateEnter(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_DuplicateEnter) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeDuplicateEnter sizeof(MSC_LOGDB_DuplicateEnter) != iLen");
		return;
	}

	MSC_LOGDB_DuplicateEnter  *p = (MSC_LOGDB_DuplicateEnter*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EnterFightScensSpendRecordAdd");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_food", p->iFood);
	m_dbConnector.AddParm("_phsical", p->iPphysical);
	m_dbConnector.AddParm("_propid", p->iGoods);
	m_dbConnector.AddParm("_propidnum",p->iCount);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//竞技场战斗消耗
CT_VOID CCLogDBEvent::DisposePVPFightingCost(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_PVPFighringCost) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePVPFightingCost sizeof(MSC_LOGDB_PVPFighringCost) != iLen");
		return;
	}

	MSC_LOGDB_PVPFighringCost  *p = (MSC_LOGDB_PVPFighringCost*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPEnterSpendRecordAdd");

	m_dbConnector.AddParm("_uid", p->dwUserID);
	m_dbConnector.AddParm("_diamod", p->iDiamod);
	m_dbConnector.AddParm("_stamina", p->iStamina);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeActiveEventReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_ActiveEventReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeActiveEventReward sizeof(MSC_LOGDB_ActiveEventReward) != iLen");
		return;
	}
	MSC_LOGDB_ActiveEventReward *p=(MSC_LOGDB_ActiveEventReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveEventRewardRecord");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_aid",p->activeReward.iAEID);
	m_dbConnector.AddParm("_gold",p->activeReward.iGold);
	m_dbConnector.AddParm("_xp",p->activeReward.iXP);
	m_dbConnector.AddParm("_food",p->activeReward.iFood);
	m_dbConnector.AddParm("_talent",p->activeReward.iTalent);
	m_dbConnector.AddParm("_prestige",p->activeReward.iPrestige);
//	m_dbConnector.AddParm("_civil",p->activeReward.iCivil);
	m_dbConnector.AddParm("_civil",p->activeReward.iStamina);
	m_dbConnector.AddParm("_diamod",p->activeReward.iDiamod);
	m_dbConnector.AddParm("_propid1",p->activeReward.iPropID1);
	m_dbConnector.AddParm("_propnum1",p->activeReward.iPropNum1);
	m_dbConnector.AddParm("_propid2",p->activeReward.iPropID2);
	m_dbConnector.AddParm("_propnum2",p->activeReward.iPropNum2);
	m_dbConnector.AddParm("_propid3",p->activeReward.iPropID3);
	m_dbConnector.AddParm("_propnum3",p->activeReward.iPropNum3);
	m_dbConnector.AddParm("_propid4",p->activeReward.iPropID4);
	m_dbConnector.AddParm("_propnum4",p->activeReward.iPropNum4);
	m_dbConnector.AddParm("_grade",   p->iGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//战斗购买次数
CT_VOID CCLogDBEvent::DisposeBuyFightingTime(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_BuyFightingTime) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeBuyFightingTime sizeof(MSC_LOGDB_BuyFightingTime) != iLen");
		return;
	}
	MSC_LOGDB_BuyFightingTime *p=(MSC_LOGDB_BuyFightingTime*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorldFightTimesBuyAdd");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);
	m_dbConnector.AddParm("_buyfighttimes",p->iBuyFightingTime);
	m_dbConnector.AddParm("_diamod",p->iDiamod);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//强制刷新
CT_VOID CCLogDBEvent::DisposePVPRefresh(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_Refresh) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePVPRefresh sizeof(MSC_LOGDB_Refresh) != iLen");
		return;
	}
	MSC_LOGDB_Refresh *p=(MSC_LOGDB_Refresh*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPAddRefreshRecord");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_diamod",p->iDiamod);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//钻石减少强化冷却时间
CT_VOID CCLogDBEvent::DisposeClearCD(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_ClearCD) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeClearCD sizeof(MSC_LOGDB_ClearCD) != iLen");
		return;
	}
	MSC_LOGDB_ClearCD *p=(MSC_LOGDB_ClearCD*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SmithingCooldownRecord");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_diamod",p->iDiamod);
	m_dbConnector.AddParm("_ugrade", p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//信息反馈
CT_VOID CCLogDBEvent::DisposeFeedBack(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_FeedBack) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeFeedBack sizeof(MSC_DB_FeedBack) != iLen");
		return;
	}
	MSC_DB_FeedBack *p=(MSC_DB_FeedBack*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BugInfoAdd");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_phone",p->cMobile);
	m_dbConnector.AddParm("_buginfo",p->cEmailContent);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//扫荡加速
CT_VOID CCLogDBEvent::DisposeRaidsSpeed(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_RaidsSpeed) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeRaidsSpeed sizeof(MSC_LOGDB_RaidsSpeed) != iLen");
		return;
	}
	MSC_LOGDB_RaidsSpeed *p=(MSC_LOGDB_RaidsSpeed*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_RaidsSpeedRecordAdd");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_grade",p->iGrade);
	m_dbConnector.AddParm("_diamod",p->iDiamod);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}
CT_VOID CCLogDBEvent::DisposeWorshipReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_WorshipReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeWorshipReward sizeof(MSC_LOGDB_WorshipReward) != iLen");
		return;
	}
	MSC_LOGDB_WorshipReward *p=(MSC_LOGDB_WorshipReward*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorshipRewardRecord");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_goldcost",p->iGoldCost);
	m_dbConnector.AddParm("_gold",p->iGold);
	m_dbConnector.AddParm("_food",p->iFood);
	m_dbConnector.AddParm("_talent",p->iTalent);
	m_dbConnector.AddParm("_diamod",p->iDiamod);
	m_dbConnector.AddParm("_xp",p->iXp);
	m_dbConnector.AddParm("_itemid1",p->iItemID1);
	m_dbConnector.AddParm("_itemnum1",p->iItemNum1);
	m_dbConnector.AddParm("_itemid2",p->iItemID2);
	m_dbConnector.AddParm("_itemnum2",p->iItemNum2);
	m_dbConnector.AddParm("_ugrade",p->iGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//组队副本奖励
CT_VOID CCLogDBEvent::DiposeTeamDuplicateReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_TeamDuplicateRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DiposeTeamDuplicateReward sizeof(MSC_LOGDB_TeamDuplicateRecord) != iLen");
		return;
	}
	MSC_LOGDB_TeamDuplicateRecord *p=(MSC_LOGDB_TeamDuplicateRecord*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamFightAwardRecord");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm(" _fid",p->iFid);
	m_dbConnector.AddParm("_diffcult",p->iDifficulty);
	m_dbConnector.AddParm("_xp",p->iXp);
	m_dbConnector.AddParm("_money",p->iGold);
	m_dbConnector.AddParm("_talent",p->iTalent);
	m_dbConnector.AddParm("_diamod",p->iDiamod);
	m_dbConnector.AddParm("_skillnum",p->iSkillNum);
	m_dbConnector.AddParm("_propid1",p->iPropID[0]);
	m_dbConnector.AddParm("_prop1num",p->iPropNum[0]);
	m_dbConnector.AddParm("_propid2",p->iPropID[1]);
	m_dbConnector.AddParm("_prop2num",p->iPropNum[1]);
	m_dbConnector.AddParm("_propid3",p->iPropID[2]);
	m_dbConnector.AddParm("_prop3num",p->iPropNum[2]);
	m_dbConnector.AddParm("_propid4",p->iPropID[3]);
	m_dbConnector.AddParm("_prop4num",p->iPropNum[3]);
	m_dbConnector.AddParm("_win",p->iWin);
	m_dbConnector.AddParm("_ugrade",p->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//组队副本消耗
CT_VOID CCLogDBEvent::DisposeTeamDuplicateCost(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_TeamDuplicateCost) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeTeamDuplicateCost sizeof(MSC_LOGDB_TeamDuplicateCost) != iLen");
		return;
	}
	MSC_LOGDB_TeamDuplicateCost *p=(MSC_LOGDB_TeamDuplicateCost*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamFightSecneRecord");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm(" _fid",p->iSceneID);
	m_dbConnector.AddParm("_diffcult",p->iDifficulty);
	m_dbConnector.AddParm("_phsica",p->iStaminaCost);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//资源生产记录
CT_VOID CCLogDBEvent::DisposeResProductSave(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_ResourceProductRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeResProductSave sizeof(MSC_DB_ResourceProductRecord) != iLen");
		return;
	}

	MSC_DB_ResourceProductRecord *p=(MSC_DB_ResourceProductRecord*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ResProductSaveRecord");
	m_dbConnector.AddParm("_fid",p->dwFID);
	m_dbConnector.AddParm(" _uid",p->dwUID);
	m_dbConnector.AddParm("_scID",p->iScenceID);
	m_dbConnector.AddParm("_spot",p->iSpotID);
	m_dbConnector.AddParm("_iRes",p->iRes);
	m_dbConnector.AddParm("_iRes",p->iResType);
	m_dbConnector.AddParm("_reason",p->iReason);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}



CT_VOID CCLogDBEvent::DisposeFightTeam(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_FightTeam) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeFightTeam sizeof(MSC_LOGDB_FightTeam) != iLen");
		return;
	}
	MSC_LOGDB_FightTeam *p=(MSC_LOGDB_FightTeam*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamRecord");

	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_goldcost",p->iGoldCost);
	m_dbConnector.AddParm("_foodcost",p->iFoodCost);
	m_dbConnector.AddParm("_diamodcost",p->iDiamodCost);
	m_dbConnector.AddParm("_gold",p->iGold);
	m_dbConnector.AddParm("_grade",p->iGrade);
	m_dbConnector.AddParm("_Item",p->iItemID);
	m_dbConnector.AddParm("_ItemCount",p->iItemCount);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//资源占领记录
CT_VOID CCLogDBEvent::DisposeResOccpySave(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_ResourceOccupyRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeResOccpySave sizeof(MSC_DB_ResourceOccupyRecord) != iLen");
		return;
	}

	MSC_DB_ResourceOccupyRecord *p=(MSC_DB_ResourceOccupyRecord*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ResOccupyRecord");
	m_dbConnector.AddParm("_fid",p->dwFID);
	m_dbConnector.AddParm(" _scid",p->iScenceID);
	m_dbConnector.AddParm("_spotid",p->iSpotID);
	m_dbConnector.AddParm("_res",p->iFTeamRes);
	m_dbConnector.AddParm("_gold",p->iOccupyGold);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//军衔俸禄记录
CT_VOID CCLogDBEvent::DisposeSalary(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_Salary) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeSalary sizeof(MSC_LOGDB_Salary) != iLen");
		return;
	}

	MSC_LOGDB_Salary *p=(MSC_LOGDB_Salary*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SalaryRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_money",p->iGold);
	m_dbConnector.AddParm("_food",p->iFood);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//军衔升级
CT_VOID CCLogDBEvent::DisposeRankingUpGrade(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_RankingUpGrade) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeRankingUpGrade sizeof(MSC_LOGDB_RankingUpGrade) != iLen");
		return;
	}

	MSC_LOGDB_RankingUpGrade *p=(MSC_LOGDB_RankingUpGrade*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_RankRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_rankgrade",p->iGrade);
	m_dbConnector.AddParm("_rankgradeinc",p->iGradeUp);
	m_dbConnector.AddParm("_rankxp",p->iRankxp);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//物品使用记录
CT_VOID CCLogDBEvent::DisposeUserItem(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_UserItem) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeUserItem sizeof(MSC_LOGDB_UserItem) != iLen");
		return;
	}

	MSC_LOGDB_UserItem *p=(MSC_LOGDB_UserItem*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropUsedRecordAdd");
	m_dbConnector.AddParm("_pid",p->iItemID);
	m_dbConnector.AddParm("_pnum", p->iItemNum);
	m_dbConnector.AddParm("_uid",p->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//物品获得记录
CT_VOID CCLogDBEvent::DisposeAddItem(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_AddItem) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeAddItem sizeof(MSC_LOGDB_AddItem) != iLen");
		return;
	}

	MSC_LOGDB_AddItem *p=(MSC_LOGDB_AddItem*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropAddRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_pid",p->iItemID);
	m_dbConnector.AddParm("_pnum", p->iItemNum);
	m_dbConnector.AddParm("_savrtag", p->saveTag);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeBattleFlag(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_BattleFlag) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeBattleFlag sizeof(MSC_LOGDB_BattleFlag) != iLen");
		return;
	}

	MSC_LOGDB_BattleFlag *p=(MSC_LOGDB_BattleFlag*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BattleFlagRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_flag",p->iFlag);
	m_dbConnector.AddParm("_grade", p->iGrade);
	m_dbConnector.AddParm("_ugrade", p->iUGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeEatElm(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_EatElm) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeEatElm sizeof(MSC_LOGDB_EatElm) != iLen");
		return;
	}

	MSC_LOGDB_EatElm *p=(MSC_LOGDB_EatElm*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EatElmRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_eatid",p->iEatID);
	m_dbConnector.AddParm("_eatenid", p->iEatenID);
	m_dbConnector.AddParm("_count",p->iCount);
	m_dbConnector.AddParm("_ugrade", p->iUGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}


CT_VOID CCLogDBEvent::DisposeEmbedEquip(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_EmbedEquip) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::MSC_LOGDB_EmbedEquip sizeof(MSC_LOGDB_EatElm) != iLen");
		return;
	}

	MSC_LOGDB_EmbedEquip *p=(MSC_LOGDB_EmbedEquip*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EmbedEquipRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_key",p->key);
	m_dbConnector.AddParm("_gemid", p->iGemID);
	m_dbConnector.AddParm("_slot",p->iSlot);
	m_dbConnector.AddParm("_ugrade", p->iGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//跨服奖励
CT_VOID CCLogDBEvent::DisposeCSReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_CS_REWARD) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeCSReward sizeof(MSC_LOGDB_CS_REWARD) != iLen");
		return;
	}

	MSC_LOGDB_CS_REWARD *p=(MSC_LOGDB_CS_REWARD*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UserArwardRecordInsert");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_type",p->iResType);
	m_dbConnector.AddParm("_resid", p->iResID);
	m_dbConnector.AddParm("_num",p->iNum);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//威能消耗
CT_VOID CCLogDBEvent::DisposeInsporeCost(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_CS_INSPORECOST) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::MSC_LOGDB_EmbedEquip sizeof(MSC_LOGDB_CS_INSPORECOST) != iLen");
		return;
	}

	MSC_LOGDB_CS_INSPORECOST *p=(MSC_LOGDB_CS_INSPORECOST*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PowerConsumeRecordInsert");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_powerid",p->iID);
	m_dbConnector.AddParm("_grade", p->iGrade);
	m_dbConnector.AddParm("_exp", p->iGrade);
	m_dbConnector.AddParm("_resid1",p->iResID[0]);
	m_dbConnector.AddParm("_resid2", p->iResID[1]);
	m_dbConnector.AddParm("_resid3", p->iResID[2]);
	m_dbConnector.AddParm("_resid4", p->iResID[3]);
	m_dbConnector.AddParm("_resnum1", p->iResNum[0]);
	m_dbConnector.AddParm("_resnum2", p->iResNum[1]);
	m_dbConnector.AddParm("_resnum3", p->iResNum[2]);
	m_dbConnector.AddParm("_resnum4", p->iResNum[3]);
	m_dbConnector.AddParm("_prop1", p->iItemID[0]);
	m_dbConnector.AddParm("_prop2", p->iItemID[1]);
	m_dbConnector.AddParm("_propnum1", p->iItemNum[0]);
	m_dbConnector.AddParm("_propnum2", p->iItemNum[1]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}

//支持
CT_VOID CCLogDBEvent::DiposeSupport(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_CS_SUPPORT) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DiposeSupport sizeof(MSC_LOGDB_CS_SUPPORT) != iLen");
		return;
	}

	MSC_LOGDB_CS_SUPPORT *p=(MSC_LOGDB_CS_SUPPORT*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UserSupportRecordInsert");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_ftid",p->dwFTID);
	m_dbConnector.AddParm("_resid", p->iResID);
	m_dbConnector.AddParm("_num",p->iNum);


	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
	return;
}


//事件奖励记录
CT_VOID CCLogDBEvent::DisposeEventReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_EventReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeEventReward sizeof(MSC_LOGDB_EventReward) != iLen");
		return;
	}

	MSC_LOGDB_EventReward *p=(MSC_LOGDB_EventReward*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EventRecordAdd");
	m_dbConnector.AddParm("_uid",p->dwUserID);
	m_dbConnector.AddParm("_food",p->iGold);
	m_dbConnector.AddParm("_gold",p->iFood);
	m_dbConnector.AddParm("_xp",p->iXp);
	m_dbConnector.AddParm("_civilixp",p->iCiviliNum);
	m_dbConnector.AddParm("_prop1",p->iProp1);
	m_dbConnector.AddParm("_prop1num",p->iNum1);
	m_dbConnector.AddParm("_prop2",p->iProp2);
	m_dbConnector.AddParm("_prop2num",p->iNum2);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//宠物训练
CT_VOID CCLogDBEvent::DisposePetUpGrade(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_PetUpGrade) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposePetTrain sizeof(MSC_LOGDB_PetUpGrade) != iLen");
		return;
	}

	MSC_LOGDB_PetUpGrade *pPug=(MSC_LOGDB_PetUpGrade*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetUpGradeRecord");
	m_dbConnector.AddParm("_uid",pPug->dwUserID);
	m_dbConnector.AddParm("_grade",pPug->iPetGrade);
	m_dbConnector.AddParm("_exp",pPug->iExp);

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


CT_VOID CCLogDBEvent::DisposeMasterSaveData(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_MasterSaveData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeMasterSaveData sizeof(MSC_DB_MasterSaveData) != iLen");
		return;
	}

	MSC_DB_MasterSaveData *pMasterSaveData=(MSC_DB_MasterSaveData*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MasterDataRecord");

	m_dbConnector.AddParm("_uid",       pMasterSaveData->dwUserID);
	m_dbConnector.AddParm("_gold",      pMasterSaveData->mvd.stRes.iGold);
	m_dbConnector.AddParm("_diamod",    pMasterSaveData->mvd.stRes.iDiamod );

	m_dbConnector.AddParm("_food",      pMasterSaveData->mvd.stRes.iFood );
	m_dbConnector.AddParm("_civiNum",   pMasterSaveData->mvd.stCivili.iCurrentExp );
	m_dbConnector.AddParm("_civiGrade", pMasterSaveData->mvd.stCivili.iLevel );
	m_dbConnector.AddParm("_grade",     pMasterSaveData->mvd.stGrade.iLevel);
	m_dbConnector.AddParm("_gradeXp",   pMasterSaveData->mvd.stGrade.iCurrentExp );

	m_dbConnector.AddParm("_rank",		pMasterSaveData->mvd.stRank.iLevel ); 
	m_dbConnector.AddParm("_rankExp",   pMasterSaveData->mvd.stRank.iCurrentExp);


	m_dbConnector.AddParm("_talentNum", pMasterSaveData->mvd.stRes.iTalentNum);
	m_dbConnector.AddParm("_stamina ",  pMasterSaveData->mvd.stRes.iStamina);
	m_dbConnector.AddParm("_skillsouls", pMasterSaveData->mvd.stRes.iSkillSoul);
	//m_dbConnector.AddParm("_exitTime",  (CT_INT64)masterSaveData.llExitTime);
	m_dbConnector.AddParm("_pvpVicotryTimes ", pMasterSaveData->mvd.pvpData.iVictoryTimes);
	m_dbConnector.AddParm("_pvpFightTimes ",   pMasterSaveData->mvd.pvpData.iFightTimes);
	m_dbConnector.AddParm("_stiminaBuyTimies", pMasterSaveData->mvd.stRes.iStaminaBuyTimes);
	m_dbConnector.AddParm("_useSkillNums", pMasterSaveData->mvd.stRes.iSkillSlots);
	m_dbConnector.AddParm("_bagSlot",     pMasterSaveData->mvd.stRes.iBagSlots);
	m_dbConnector.AddParm("_Savetag",  pMasterSaveData->iSaveTag);
	m_dbConnector.AddParm("_fightingpower",  pMasterSaveData->mvd.stRes.iFightingPower);
	m_dbConnector.AddParm("_teamfighttimes",  pMasterSaveData->mvd.stRes.iTeamDuplicateFightingTimes);
    m_dbConnector.AddParm("_elmSlot",     pMasterSaveData->mvd.stRes.iElmSlots);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//战队升级
CT_VOID CCLogDBEvent::DisposeFightTeamUpGrade(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_FightTeamUpGrade) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeFightTeamUpGrade sizeof(MSC_LOGDB_FightTeamUpGrade) != iLen");
		return;
	}

	MSC_LOGDB_FightTeamUpGrade *pFTUpGrade=(MSC_LOGDB_FightTeamUpGrade*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamUpGrade");

	m_dbConnector.AddParm("_FTid",      pFTUpGrade->dwFTID);
	m_dbConnector.AddParm("_uid",       pFTUpGrade->dwUserID);
	m_dbConnector.AddParm("_Res",      pFTUpGrade->iRes);
	m_dbConnector.AddParm("_ItemRes",    pFTUpGrade->iItemRes);
	m_dbConnector.AddParm("_FTGrade",    pFTUpGrade->iGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//战队召唤消耗
CT_VOID CCLogDBEvent::DisposeFightTeamSummonCost(CT_CHAR *pBuf,CT_INT32 iLen)
{
	return;//非个人消耗,暂时不记
	if(sizeof(MSC_LOGDB_FightTeamSummonCost) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeFightTeamSummonCost sizeof(MSC_LOGDB_FightTeamSummonCost) != iLen");
		return;
	}

	MSC_LOGDB_FightTeamSummonCost *pFTSummonCost=(MSC_LOGDB_FightTeamSummonCost*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamUpGrade");

	m_dbConnector.AddParm("_FTid",      pFTSummonCost->dwFTID);
	m_dbConnector.AddParm("_uid",       pFTSummonCost->dwUserID);
	m_dbConnector.AddParm("_Res",      pFTSummonCost->iRes);
	m_dbConnector.AddParm("_ItemRes",    pFTSummonCost->iItemRes);
	m_dbConnector.AddParm("_BossIndex",    pFTSummonCost->iBossIndex);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//战队召唤奖励
CT_VOID CCLogDBEvent::DisposeFightTeamSummonRewardRes(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_FightTeamSummonRewardRes) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeFightTeamSummonReward sizeof(MSC_LOGDB_FightTeamSummonRewardRes) != iLen");
		return;
	}

	MSC_LOGDB_FightTeamSummonRewardRes *pFTSummonReward=(MSC_LOGDB_FightTeamSummonRewardRes*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamSummonRecord");
	m_dbConnector.AddParm("_uid",       pFTSummonReward->dwUserID);
	m_dbConnector.AddParm("_FTid",      pFTSummonReward->dwFTID);
	m_dbConnector.AddParm("_Type0",      pFTSummonReward->Reward.iResType0);
	m_dbConnector.AddParm("_Num0",      pFTSummonReward->Reward.iResNum0);
	m_dbConnector.AddParm("_Type1",      pFTSummonReward->Reward.iResType1);
	m_dbConnector.AddParm("_Num1",      pFTSummonReward->Reward.iResNum1);
	m_dbConnector.AddParm("_Type2",      pFTSummonReward->Reward.iResType2);
	m_dbConnector.AddParm("_Num2",      pFTSummonReward->Reward.iResNum2);
	m_dbConnector.AddParm("_Type3",      pFTSummonReward->Reward.iResType3);
	m_dbConnector.AddParm("_Num3",      pFTSummonReward->Reward.iResNum3);
	m_dbConnector.AddParm("_Grade",      pFTSummonReward->iGarde);

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

//战队召唤奖励道具
CT_VOID CCLogDBEvent::DisposeFightTeamSummonRewardItem(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_FightTeamSummonRewardItem) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeFightTeamSummonReward sizeof(MSC_LOGDB_FightTeamSummonRewardItem) != iLen");
		return;
	}

	MSC_LOGDB_FightTeamSummonRewardItem *pFTSummonReward=(MSC_LOGDB_FightTeamSummonRewardItem*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamSummonItemRecord");

	m_dbConnector.AddParm("_uid",       pFTSummonReward->dwUserID);
	m_dbConnector.AddParm("_FTid",      pFTSummonReward->dwFTID);
	m_dbConnector.AddParm("_BossIndex",      pFTSummonReward->iBossIndex);
	m_dbConnector.AddParm("_pid1",      pFTSummonReward->iItemID[0]);
	m_dbConnector.AddParm("_pnum1",      pFTSummonReward->iItemCount[0]);
	m_dbConnector.AddParm("_pid2",      pFTSummonReward->iItemID[1]);
	m_dbConnector.AddParm("_pnum2",      pFTSummonReward->iItemCount[1]);
	m_dbConnector.AddParm("_pid3",      pFTSummonReward->iItemID[2]);
	m_dbConnector.AddParm("_pnum3",      pFTSummonReward->iItemCount[2]);
	m_dbConnector.AddParm("_pid4",      pFTSummonReward->iItemID[3]);
	m_dbConnector.AddParm("_pnum4",      pFTSummonReward->iItemCount[3]);
	m_dbConnector.AddParm("_Grade",      pFTSummonReward->iGarde);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

//战队科技升级
CT_VOID CCLogDBEvent::DisposeFightTeamScienceUpGrade(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_FightTeamScienceUpGrade) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeFightTeamScienceUpGrade sizeof(MSC_LOGDB_FightTeamScienceUpGrade) != iLen");
		return;
	}

	MSC_LOGDB_FightTeamScienceUpGrade *pFTSUG=(MSC_LOGDB_FightTeamScienceUpGrade*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FightTeamTechnRecord");

	//m_dbConnector.AddParm("_FTid",      pFTSUG->dwFTID);
	m_dbConnector.AddParm("_uid",       pFTSUG->dwUserID);
	m_dbConnector.AddParm("_technid",      pFTSUG->iScienceID);
	m_dbConnector.AddParm("_tgrade",      pFTSUG->iScienceGrade);
	m_dbConnector.AddParm("_costtype",     pFTSUG->iCostType);
	m_dbConnector.AddParm("_costnum",      pFTSUG->iCostNum);
	m_dbConnector.AddParm("_ugrade",      pFTSUG->iUserGrade);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return;
	}
}

CT_VOID CCLogDBEvent::DisposeTowerPass(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_Tower) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeTowerPass sizeof(MSC_LOGDB_Tower) != iLen");
		return;
	}

	MSC_LOGDB_Tower *pTower=(MSC_LOGDB_Tower*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerPassRecord");	
	m_dbConnector.AddParm("_uid",		pTower->dwUserID);
	m_dbConnector.AddParm("_begincpid",	pTower->iBeginCPID);
	m_dbConnector.AddParm("_endcpid",	pTower->iEndCPID);
	m_dbConnector.AddParm("_restype1",	pTower->iResType[0]);
	m_dbConnector.AddParm("_restype2",	pTower->iResType[1]);
	m_dbConnector.AddParm("_restype3",	pTower->iResType[2]);
	m_dbConnector.AddParm("_restype4",	pTower->iResType[3]);
	m_dbConnector.AddParm("_resnum1",	pTower->iResNum[0]);
	m_dbConnector.AddParm("_resnum2",	pTower->iResNum[1]);
	m_dbConnector.AddParm("_resnum3",	pTower->iResNum[2]);
	m_dbConnector.AddParm("_resnum4",	pTower->iResNum[3]);
	m_dbConnector.AddParm("_ugrade",	pTower->iUGrade);

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

CT_VOID CCLogDBEvent::DisposeTowerOccupy(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_TowerSlot) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeTowerOccupy sizeof(MSC_LOGDB_TowerSlot) != iLen");
		return;
	}

	MSC_LOGDB_TowerSlot *pTowerSlot=(MSC_LOGDB_TowerSlot*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerOccupyRecord");	
	m_dbConnector.AddParm("_uid",		pTowerSlot->dwUserID);
	m_dbConnector.AddParm("_cpid",		pTowerSlot->iCPID);
	m_dbConnector.AddParm("_slotid",	pTowerSlot->iSlotID);
	m_dbConnector.AddParm("_restype1",	pTowerSlot->iResType[0]);
	m_dbConnector.AddParm("_restype2",	pTowerSlot->iResType[1]);
	m_dbConnector.AddParm("_restype3",	pTowerSlot->iResType[2]);
	m_dbConnector.AddParm("_restype4",	pTowerSlot->iResType[3]);
	m_dbConnector.AddParm("_resnum1",	pTowerSlot->iResNum[0]);
	m_dbConnector.AddParm("_resnum2",	pTowerSlot->iResNum[1]);
	m_dbConnector.AddParm("_resnum3",	pTowerSlot->iResNum[2]);
	m_dbConnector.AddParm("_resnum4",	pTowerSlot->iResNum[3]);
	m_dbConnector.AddParm("_ugrade",	pTowerSlot->iUGrade);

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

CT_VOID CCLogDBEvent::DisposeTowerHelp(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_TowerHelp) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeTowerHelp sizeof(MSC_LOGDB_TowerHelp) != iLen");
		return;
	}

	MSC_LOGDB_TowerHelp *pTowerHelp=(MSC_LOGDB_TowerHelp*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerHelpRecord");	
	m_dbConnector.AddParm("_uid",		pTowerHelp->dwUserID);
	m_dbConnector.AddParm("_fuid",		pTowerHelp->dwFUID);
	m_dbConnector.AddParm("_cpid",		pTowerHelp->iCPID);
	m_dbConnector.AddParm("_slotid",	pTowerHelp->iSlotID);
	m_dbConnector.AddParm("_restype1",	pTowerHelp->iResType[0]);
	m_dbConnector.AddParm("_restype2",	pTowerHelp->iResType[1]);
	m_dbConnector.AddParm("_restype3",	pTowerHelp->iResType[2]);
	m_dbConnector.AddParm("_restype4",	pTowerHelp->iResType[3]);
	m_dbConnector.AddParm("_resnum1",	pTowerHelp->iResNum[0]);
	m_dbConnector.AddParm("_resnum2",	pTowerHelp->iResNum[1]);
	m_dbConnector.AddParm("_resnum3",	pTowerHelp->iResNum[2]);
	m_dbConnector.AddParm("_resnum4",	pTowerHelp->iResNum[3]);
	m_dbConnector.AddParm("_ugrade",	pTowerHelp->iUGrade);

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

CT_VOID CCLogDBEvent::DisposeTreasureUse(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_TreasureUse) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeTreasureUse sizeof(MSC_LOGDB_TreasureUse) != iLen");
		return;
	}

	MSC_LOGDB_TreasureUse *pTreasureUse=(MSC_LOGDB_TreasureUse*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TreasureUseRecordAdd");	
	m_dbConnector.AddParm("_uid",		pTreasureUse->dwUserID);
	m_dbConnector.AddParm("_cpid",		pTreasureUse->iCPID);
	m_dbConnector.AddParm("_treasureid",pTreasureUse->iTreasureID);
	m_dbConnector.AddParm("_usetype",	pTreasureUse->iUseType);
	m_dbConnector.AddParm("_restype1",	pTreasureUse->iResType[0]);
	m_dbConnector.AddParm("_restype2",	pTreasureUse->iResType[1]);
	m_dbConnector.AddParm("_restype3",	pTreasureUse->iResType[2]);
	m_dbConnector.AddParm("_restype4",	pTreasureUse->iResType[3]);
	m_dbConnector.AddParm("_resnum1",	pTreasureUse->iResNum[0]);
	m_dbConnector.AddParm("_resnum2",	pTreasureUse->iResNum[1]);
	m_dbConnector.AddParm("_resnum3",	pTreasureUse->iResNum[2]);
	m_dbConnector.AddParm("_resnum4",	pTreasureUse->iResNum[3]);
	m_dbConnector.AddParm("_ugrade",	pTreasureUse->iUGrade);

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

CT_VOID CCLogDBEvent::DisposeTitle(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if (sizeof(MSC_LOGDB_Title) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeTitle sizeof(MSC_LOGDB_Title) != iLen");
		return;
	}

	MSC_LOGDB_Title* plt = (MSC_LOGDB_Title*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TitleRecordAdd");
	m_dbConnector.AddParm("_uid", plt->dwUserID);
	m_dbConnector.AddParm("_tid", plt->iTitleID);
	m_dbConnector.AddParm("_status", plt->iStatus);
	m_dbConnector.AddParm("_lastTime", plt->iLastTime);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector,m_dwThread);
	}
}

CT_VOID CCLogDBEvent::DisposeIsland(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_LOGDB_IslandData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeIsland sizeof(MSC_LOGDB_IslandData) != iLen");
		return;
	}

	MSC_LOGDB_IslandData* pIsland = (MSC_LOGDB_IslandData*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_IslandRecordAdd");
	m_dbConnector.AddParm("_uid",	pIsland->dwUserID);
	m_dbConnector.AddParm("_point",	pIsland->iPoint);
	m_dbConnector.AddParm("_eventid",	pIsland->iEventID);
	m_dbConnector.AddParm("_eventstatus",	pIsland->iEventStatus);
	m_dbConnector.AddParm("_throwtimes",	pIsland->iThrowTimes);
	m_dbConnector.AddParm("_starttimes",	pIsland->iStartTimes);
	m_dbConnector.AddParm("_gettimes",		pIsland->iGetTimes);
	m_dbConnector.AddParm("_ftid",			pIsland->iFT_ID);
	m_dbConnector.AddParm("_score",			pIsland->iScore);
	m_dbConnector.AddParm("_rewardtimes",	pIsland->iRewardTimes);
	
	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}

//城墙加成
CT_VOID CCLogDBEvent::DisposeWallAddition(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if (sizeof(MSC_LOGDB_WallAddition) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeWallAddition sizeof(MSC_LOGDB_WallAddition) != iLen");
		return;
	}

	MSC_LOGDB_WallAddition* pWa = (MSC_LOGDB_WallAddition*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WallAdditionRecordAdd");
	m_dbConnector.AddParm("_uid",	pWa->dwUserID);
	m_dbConnector.AddParm("_resid1",	pWa->iResID[0]);
	m_dbConnector.AddParm("_rescount1",	pWa->iResCount[0]);
	m_dbConnector.AddParm("_resid2",	pWa->iResID[1]);
	m_dbConnector.AddParm("_rescount2",	pWa->iResCount[1]);
	m_dbConnector.AddParm("_resid3",	pWa->iResID[2]);
	m_dbConnector.AddParm("_rescount3",	pWa->iResCount[2]);
	m_dbConnector.AddParm("_resid4",	pWa->iResID[3]);
	m_dbConnector.AddParm("_rescount4",	pWa->iResCount[3]);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//掠夺
CT_VOID CCLogDBEvent::DisposePlunder(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_LOGDB_Plunder) != iLen)
	{
		CCLogMeg::getSingleton().WriteWaring("CCLogDBEvent::DisposePlunder sizeof(MSC_LOGDB_Plunder) != iLen");
		return;
	}

	MSC_LOGDB_Plunder *p=(MSC_LOGDB_Plunder*)pBuf;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PlunderRecordAdd");
	m_dbConnector.AddParm("_uid",	p->dwUserID);
	m_dbConnector.AddParm("_food",	p->iFood);
	m_dbConnector.AddParm("_gold",	p->iGold);
	m_dbConnector.AddParm("_dstuid",	p->dwDstUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}

//探访记录
CT_VOID CCLogDBEvent::DisposeSeekData(CT_CHAR* pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekVistorInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCLogDBEvent::DisposeSeekData iLen != sizeof(MSC_DB_SeekVistorInfo)");
		return;
	}

	MSC_DB_SeekVistorInfo* pInfo = (MSC_DB_SeekVistorInfo*)pBuf;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekRecordUpdate");
	m_dbConnector.AddParm("_uid", pInfo->dwUserID);
	m_dbConnector.AddParm("_fuid", pInfo->vistorInfo.dwUserID);
	m_dbConnector.AddParm("_type", pInfo->vistorInfo.seekInfo.iType);
	m_dbConnector.AddParm("_id0", pInfo->vistorInfo.seekInfo.iID[0]);
	m_dbConnector.AddParm("_num0", pInfo->vistorInfo.seekInfo.iNum[0]);
	m_dbConnector.AddParm("_id1", pInfo->vistorInfo.seekInfo.iID[1]);
	m_dbConnector.AddParm("_num1", pInfo->vistorInfo.seekInfo.iNum[1]);
	m_dbConnector.AddParm("_id2", pInfo->vistorInfo.seekInfo.iID[2]);
	m_dbConnector.AddParm("_num2", pInfo->vistorInfo.seekInfo.iNum[2]);
	m_dbConnector.AddParm("_id3", pInfo->vistorInfo.seekInfo.iID[3]);
	m_dbConnector.AddParm("_num3", pInfo->vistorInfo.seekInfo.iNum[3]);
	m_dbConnector.AddParm("_present0", pInfo->vistorInfo.present[0]);
	m_dbConnector.AddParm("_present1", pInfo->vistorInfo.present[1]);
	m_dbConnector.AddParm("_present2", pInfo->vistorInfo.present[2]);
	m_dbConnector.AddParm("_iskidnap", pInfo->vistorInfo.bIsKidnap);
	m_dbConnector.AddParm("_plunderbid0", pInfo->vistorInfo.iPlunderBID[0]);
	m_dbConnector.AddParm("_plunderbid1", pInfo->vistorInfo.iPlunderBID[1]);
	m_dbConnector.AddParm("_plunderbid2", pInfo->vistorInfo.iPlunderBID[2]);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
	return;
}