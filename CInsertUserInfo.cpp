#include "CInsertUserInfo.h"
#include "CLogMeg.h"
#include "CThreadMes.h"
#include "GlobeScriptInterface.h"
#include "CItemScriptSetting.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CTop.h"
#include "CUserManager.h"
#include "CBuildingScriptSetting.h"
#include "CFightingScriptSetting.h"
#include "CGameConfig.h"
#include "CResourceGrabMgr.h"

CCInsertUserInfo::CCInsertUserInfo(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MASTERDATA);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_UPGRADEBUILD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TALENTINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ITEMINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_EQUIPINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BUFFINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_COOLDOWNINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ARMYINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PRODUCTING);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_WORLDSCENE);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SCENEINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_REGIONINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BATTLEARRAY);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TASKFINISH);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TASKPORGRESS);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_DAILYQUEST);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_DAILYACTIVE);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BUILDINGINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MARKETFUNCTION);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BLACKMARKETINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PETINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_VIPINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PVPINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOP);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PVPRECORD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SKILLDATA);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SIGNINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ENCHANTDATA);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BUYFIGHTINGTIME);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_RAIDSINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_RAIDSREWARD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_FRIENDINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TEAMDUPLICATE);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PRESENT);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MARKETPERSONAL);
    RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BATTLEFLAG);
    RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ELMATTR);
    RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ELMPOWER);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWER);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERSLOTREWARD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MSG);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_FIGHTTEAM_SCIENCE);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERSLOTFIGHTING);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWEREVENT);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERHELP);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ENCHANT_REWARD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BOSS_RECORD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TITLE);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PETCHANGE);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLANDDATA);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLAND_GIVEUID);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLAND_SCOREREWARD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLAND_PRESENTNAME);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WALLADDITION);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WALLARMY);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_RANDPLAYER);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_HAD_SEEKEDPLAYER);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_TEAMFLAG);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_SAINTESSKIDNAP);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WORKERKIDNAP);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WILLLIST);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_SPITELIST);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_DATA);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_VISITORINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_FIGHTRECORD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MASTGER_FIGHTADD);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_OTHERSINFO);
	RegistMessage(MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_PETKIDNAP);
}		

CCInsertUserInfo::~CCInsertUserInfo(void)
{
}

//处理用户消息  最后的2个参数为备用
CT_VOID CCInsertUserInfo::OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
							CT_CHAR* pBuf,   CT_DWORD len, 
							CT_LONGLONG reserve)
{
	if(MSC_MAIN_INSERT_USERDATA == mainID)
	{
		switch(subID)
		{
		case MSC_SUB_INSERT_MASTERDATA:
			{
				DisposeMasterInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_UPGRADEBUILD:
			{
				DisposeUpgradeBuildingInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_TALENTINFO:
			{
				DisposeTalentInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SKILLDATA:
			{
				DisposeSkillData(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ITEMINFO:
			{
				DisposeItemInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_EQUIPINFO:
			{
				DisposeEquipInfo(pBuf,len);
				break;
			}
        case MSC_SUB_INSERT_ELMPOWER:
            {
                DisposeElmPowerInfo(pBuf, len);
                break;
            }
        case MSC_SUB_INSERT_ELMATTR:
            {
                DisposeElmAttrInfo(pBuf, len);
                break;
            } 
		case MSC_SUB_INSERT_BUFFINFO:
			{
				DisposeBuffInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_COOLDOWNINFO:
			{
				DisposeCoolDownInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ARMYINFO:
			{
				DisposeArmyInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_PRODUCTING:
			{
				DisposeProducting(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_WORLDSCENE:
			{
				DisposeWorldScene(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SCENEINFO:
			{
				DisposeSceneInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_REGIONINFO:
			{
				DisposeRrgionInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_BATTLEARRAY:
			{
				DisposeBattleArray(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_TASKFINISH:
			{
				DisposeTaskFinish(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_TASKPORGRESS:
			{
				DisposeTaskPorgress(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_DAILYQUEST:
			{
				DisposeDailyQuest(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_DAILYACTIVE:
			{
				DisposeDailyActive(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_BUILDINGINFO:
			{
				DisposeBuildingInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_MARKETFUNCTION:
			{
				DisposeMarketFunction(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_BLACKMARKETINFO:
			{
				DisposeMarketInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_PETINFO:
			{
				DisposePetInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_VIPINFO:
			{
				DisposeVipInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_PVPINFO:
			{
				DisposePVPInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_TOP:
			{
				DisposeTop(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_PVPRECORD:
			{
				DisposePVPRecord(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SIGNINFO:
			{
				DisposeSignInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ENCHANTDATA:
			{
				DisposeEnchantdata(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_BUYFIGHTINGTIME:
			{
				DisposeBuyFightingTime(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_RAIDSINFO:
			{
				DisposeRaidsInfo(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_RAIDSREWARD:
			{
				DisposeRaidsReward(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_FRIENDINFO:
			{
				DisposeFriendInfo(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_TEAMDUPLICATE:
			{
				DisposeTeamDuplicate(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_PRESENT:
			{
				DisposePresent(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_MARKETPERSONAL:
			{
				DisposeMarketPersonal(pBuf,len);
				break;
			}
        case MSC_SUB_INSERT_BATTLEFLAG:
            {
                DisposeBattleFlagInfo(pBuf, len);
                break;
            }
		case MSC_SUB_INSERT_TOWER:
			{
				DisposeTower(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_TOWERSLOTREWARD:
			{
				DisposeTowerSlotReward(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_TOWEREVENT:
			{
				DisposeTowerEvent(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_TOWERSLOTFIGHTING:
			{
				DisposeTowerSlotFighting(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_TOWERHELP:
			{
				DisposeTowerHelp(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_MSG:
			{
				DisposeTowerMSG(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_FIGHTTEAM_SCIENCE:
			{
				DisposeFightTeamScience(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ENCHANT_REWARD:
			{
				DisposeEnchantReward(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_BOSS_RECORD:
			{
				DisposeBossRecord(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_PETCHANGE:
			{
				DisposePetChange(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_TITLE:
			{
				DisposeTitleData(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ISLANDDATA:
			{
				DisposeIslandData(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ISLAND_GIVEUID:
			{
				DisposeIslandGiveUid(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ISLAND_SCOREREWARD:
			{
				DisposeIslandScoreReward(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_ISLAND_PRESENTNAME:
			{
				DipsoseIslandPresentName(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_WALLADDITION:
			{
				DisposeSeekWallAddition(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_WALLARMY:
			{
				DisposeWallArmy(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_RANDPLAYER:
			{
				DisposeSeekRandPlayer(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_HAD_SEEKEDPLAYER:
			{
				DisposeHadSeekedPlayer(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_TEAMFLAG:
			{
				DisposeTeamFlagInsert(pBuf,len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_SAINTESSKIDNAP:
			{
				DisposeSaintessKidnap(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_WORKERKIDNAP:
			{
				DisposeWorkerKidnap(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_WILLLIST:
			{
				DisposeWillSeekerList(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_SPITELIST:
			{
				DisposeSpiteSeekerList(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_DATA:
			{
				DisposeSeekDataInsert(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_VISITORINFO:
			{
				DisposeSeekVisitorInfo(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_FIGHTRECORD:
			{
				DisposeSeekFightRecord(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_MASTGER_FIGHTADD:
			{
				DisposeMasterFightAdd(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_OTHERSINFO:
			{
				DisposeSeekOthersInfo(pBuf, len);
				break;
			}
		case MSC_SUB_INSERT_SEEK_PETKIDNAP:
			{
				DisposePetKidnap(pBuf, len);
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::OnUserEvent mainID:%d subID:%d",mainID,subID);
				return;
			}
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::OnUserEvent mainID:%d",mainID);
	}
	return;
}

//领主信息
CT_VOID CCInsertUserInfo::DisposeMasterInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertMasterInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMasterInfo sizeof(MSC_DB_InsertMasterInfo) != iLen");
		return;
	}

	MSC_DB_InsertMasterInfo *pInsertMaster=(MSC_DB_InsertMasterInfo*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertMaster->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMasterInfo No Find UserID:%d",pInsertMaster->dwUserID);
		return;
	}

	pUser->m_userBase.SetMastData(pInsertMaster->MasterInfo);

	return;
}

//得到正在升级建筑的信息
CT_VOID CCInsertUserInfo::DisposeUpgradeBuildingInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertUpGradeBuild) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeUpgradeBuildingInfo sizeof(MSC_DB_InsertUpGradeBuild) != iLen");
		return;
	}

	MSC_DB_InsertUpGradeBuild *pInsertUpdataBuild=(MSC_DB_InsertUpGradeBuild*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertUpdataBuild->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeUpgradeBuildingInfo No Find UserID:%d",pInsertUpdataBuild->dwUserID);
		return;
	}

	//得到升级数据
	G_BuildingUpgrade bu ={0};
	if (! CCBuildingScriptSetting::getSingleton().GetBuildingUpgradeDataWithID( pInsertUpdataBuild->iBid, pInsertUpdataBuild->iGrade, &bu))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeUpgradeBuildingInfo 没有发现%i的建筑等级为%i的升级数据", pInsertUpdataBuild->iBid, pInsertUpdataBuild->iGrade);
		return ;
	}

	G_Building build = {0};
	if(!CCBuildingScriptSetting::getSingleton().GetBuildingData(pInsertUpdataBuild->iBid, &build))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeUpgradeBuildingInfo 不能插到建筑类型 bid=%i",  pInsertUpdataBuild->iBid);
		return;
	}
	
	UpgradingBuildData ubd = {0};
	ubd.iID = pInsertUpdataBuild->iBid;
	ubd.llBeginTime = pInsertUpdataBuild->llBeginTime;
	//if (pInsertUpdataBuild->iDiamod != 0)
	//{
	//	ubd.bSpeedUpTime = 1;
	//}
	ubd.iNeedTime = pInsertUpdataBuild->iLasting;
	ubd.buildingUpgrade.iBT = build.iBT;
	ubd.buildingUpgrade.iCiliviNum = bu.iCiliviNum;
	//ubd.buildingUpgrade.iDiamond   = pInsertUpdataBuild->iDiamod;
	ubd.buildingUpgrade.iFood      = bu.iFood;
	ubd.buildingUpgrade.iGold      = bu.iGold;
	ubd.buildingUpgrade.iGrade     = bu.iGrade;
	ubd.buildingUpgrade.iNeedTime  = bu.iNeedTime;
	ubd.buildingUpgrade.iProp1     = bu.iProp1;
	ubd.buildingUpgrade.iProp2     = bu.iProp2;
	ubd.buildingUpgrade.iProp3     = bu.iProp3;
	ubd.buildingUpgrade.iProp4     = bu.iProp4;

	ubd.buildingUpgrade.iPropNum1  = bu.iPropNum1;
	ubd.buildingUpgrade.iPropNum2  = bu.iPropNum2;
	ubd.buildingUpgrade.iPropNum3  = bu.iPropNum3;
	ubd.buildingUpgrade.iPropNum4  = bu.iPropNum4;
	
	ubd.buildingUpgrade.iWorker    = bu.iWorker;
	ubd.buildingUpgrade.iXp        = bu.iXp;
	
	pUser->m_userBuilding.InsertUpgradeBuildData(ubd );
	return;
}

//得到潜能数据
CT_VOID CCInsertUserInfo::DisposeTalentInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertTalentInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTalentInfo sizeof(MSC_DB_InsertTalentInfo) != iLen");
		return;
	}

	MSC_DB_InsertTalentInfo *pInsertTalent=(MSC_DB_InsertTalentInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertTalent->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTalentInfo No Find UserID:%d",pInsertTalent->dwUserID);
		return;
	}

	pUser->m_userTalent.InsertData(pInsertTalent->talentData.iID, pInsertTalent->talentData);
	return;
}

//得到技能数据
CT_VOID CCInsertUserInfo::DisposeSkillData(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertSkillData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSkillData sizeof(MSC_DB_InsertSkillData) != iLen");

		return;
	}

	MSC_DB_InsertSkillData *pInsertSkill=(MSC_DB_InsertSkillData*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertSkill->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSkillData No Find UserID:%d",pInsertSkill->dwUserID);
		return;
	}

	pUser->m_userSkill.InsertData(pInsertSkill->skillData.iID, pInsertSkill->skillData);

	return;
}

//得到道具数据
CT_VOID CCInsertUserInfo::DisposeItemInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertItemInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeItemInfo sizeof(MSC_DB_InsertItemInfo) != iLen");

		return;
	}

	MSC_DB_InsertItemInfo *pInsertItem=(MSC_DB_InsertItemInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertItem->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeItemInfo No Find UserID:%d",pInsertItem->dwUserID);
		return;
	}
	pUser->m_userItem.InsertData(pInsertItem->itemData.iIndex, pInsertItem->itemData);
	return;
}

//得到装备信息
CT_VOID CCInsertUserInfo::DisposeEquipInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertEquipInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeEquipInfo sizeof(MSC_DB_InsertEquipInfo) != iLen");
		return;
	}

	MSC_DB_InsertEquipInfo *pInsertEquip=(MSC_DB_InsertEquipInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertEquip->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeEquipInfo No Find UserID:%d",pInsertEquip->dwUserID);
		return;
	}
	pUser->m_userEquip.InsertData(pInsertEquip->equipData.key, pInsertEquip->equipData);

	return;
}

//得到增益数据
CT_VOID CCInsertUserInfo::DisposeBuffInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertBuffInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBuffInfo sizeof(MSC_DB_InsertBuffInfo) != iLen");
		return;
	}

	MSC_DB_InsertBuffInfo *pInsertBuff=(MSC_DB_InsertBuffInfo*)pBuf;

	G_ItemBuff stBuff = {0};
	CCItemScriptSetting::getSingleton().GetItemBuff(pInsertBuff->buffData.iID, stBuff);
	pInsertBuff->buffData.iGroup = stBuff.iGroup;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertBuff->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBuffInfo No Find UserID:%d",pInsertBuff->dwUserID);
		return;
	}

    pUser->m_userBuff.InsertData(pInsertBuff->buffData.iGroup, pInsertBuff->buffData);

	return;
}

//得到冷却信息
CT_VOID CCInsertUserInfo::DisposeCoolDownInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertCooldownInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeCoolDownInfo sizeof(MSC_DB_InsertCooldownInfo) != iLen");
		return;
	}

	MSC_DB_InsertCooldownInfo *pInsertCoolDown=(MSC_DB_InsertCooldownInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertCoolDown->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeCoolDownInfo No Find UserID:%d",pInsertCoolDown->dwUserID);
		return;
	}

    MasterData md = {0};
    pUser->m_userBase.GetMasterData(md);
	time_t currTime = time(0);
    time_t pastTime = currTime - md.llLastExitTime;
    time_t leftTime = pInsertCoolDown->cdData.leftTime - pastTime;
    //*
    CT_CHAR strLog[1024] = {0};
    sprintf(strLog, "CDDATA>> USER:%d TYPE:%d, LEFT:%lld", 
        pInsertCoolDown->dwUserID, pInsertCoolDown->cdData.iType, leftTime);
    CCLogMeg::getSingleton().WriteInfo("%s", strLog);
    //*/

	if (leftTime < 0) leftTime = 0;
    pInsertCoolDown->cdData.leftTime = leftTime;
    pInsertCoolDown->cdData.toggleTime = currTime;
    pUser->m_userBase. InsertCooldownData(pInsertCoolDown->cdData);
	return;
}

//得到军队信息
CT_VOID CCInsertUserInfo::DisposeArmyInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertArmyInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeArmyInfo sizeof(MSC_DB_InsertArmyInfo) != iLen");
		return;
	}

	MSC_DB_InsertArmyInfo *pInsertArmy=(MSC_DB_InsertArmyInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertArmy->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeArmyInfo No Find UserID:%d",pInsertArmy->dwUserID);
		return;
	}

	pUser->m_userArmy.InsertBufferData(pInsertArmy->Armydata.iArmyID,pInsertArmy->Armydata.iArmyGrade);

	return;
}

//得到正在生产的建筑
CT_VOID CCInsertUserInfo::DisposeProducting(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertProductingBuilding) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeProducting sizeof(MSC_DB_InsertProductingBuilding) != iLen");
		return;
	}

	MSC_DB_InsertProductingBuilding *pInsertProducting=(MSC_DB_InsertProductingBuilding*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertProducting->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeProducting No Find UserID:%d",pInsertProducting->dwUserID);
		return;
	}

	if(pInsertProducting->Producting.iBType == CITY_TYPE_RESIDENTIAL)
	{
		pUser->m_userBuilding.InsertAutoBuilding(pInsertProducting->Producting.iBid, pInsertProducting->Producting);
	}else
	{
		pUser->m_userBuilding.InsertBuildingProductMap (pInsertProducting->Producting);
	}


	return;
}

//得到世界场景数据
CT_VOID CCInsertUserInfo::DisposeWorldScene(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertWorldScene) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWorldScene sizeof(MSC_DB_InsertWorldScene) != iLen");
		return;
	}

	MSC_DB_InsertWorldScene *pInsertWorldScene=(MSC_DB_InsertWorldScene*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertWorldScene->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWorldScene No Find UserID:%d",pInsertWorldScene->dwUserID);
		return;
	}

	pUser->m_userDuplicate.InsterReward(pInsertWorldScene->data.iSceneID,pInsertWorldScene->data.iReward);

	return;
}

//得到场景数据
CT_VOID CCInsertUserInfo::DisposeSceneInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertSceneInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSceneInfo sizeof(MSC_DB_InsertSceneInfo) != iLen");
		return;
	}

	MSC_DB_InsertSceneInfo *pInsertScene=(MSC_DB_InsertSceneInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertScene->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSceneInfo No Find UserID:%d",pInsertScene->dwUserID);
		return;
	}

	pUser->m_userDuplicate.InsterScene(pInsertScene->data);
	return;
}

//得到用户解锁区域
CT_VOID CCInsertUserInfo::DisposeRrgionInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertRegionInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeRrgionInfo sizeof(MSC_DB_InsertRegionInfo) != iLen");
		return;
	}

	MSC_DB_InsertRegionInfo *pInsertRegion=(MSC_DB_InsertRegionInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertRegion->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeRrgionInfo No Find UserID:%d",pInsertRegion->dwUserID);
		return;
	}

	pUser->m_userRegion.InsertRegionUnlock(pInsertRegion->unlockRegion.iRegionID);

	return;
}

//得到阵法信息
CT_VOID CCInsertUserInfo::DisposeBattleArray(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertBattleArray) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBattleArray sizeof(MSC_DB_InsertBattleArray) != iLen");
		return;
	}

	MSC_DB_InsertBattleArray *pInsertBattle=(MSC_DB_InsertBattleArray*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertBattle->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBattleArray No Find UserID:%d",pInsertBattle->dwUserID);
		return;
	}
	pUser->m_userBattleArray.InsertBufferData(pInsertBattle->data.iPosition);

	return;
}

//得到用户已完成任务
CT_VOID CCInsertUserInfo::DisposeTaskFinish(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertTaskFinish) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTaskFinish sizeof(MSC_DB_InsertTaskFinish) != iLen");
		return;
	}

	MSC_DB_InsertTaskFinish *pInsertTaskFinish=(MSC_DB_InsertTaskFinish*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertTaskFinish->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTaskFinish No Find UserID:%d",pInsertTaskFinish->dwUserID);
		return;
	}

	pUser->m_userTask.InsertFinishTask(pInsertTaskFinish->taskFinishData.iTaskID);


	return;
}

//得到用户在进行任务
CT_VOID CCInsertUserInfo::DisposeTaskPorgress(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertProgressData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTaskPorgress sizeof(MSC_DB_InsertProgressData) != iLen");
		return;
	}

	MSC_DB_InsertProgressData *pInsertProgress=(MSC_DB_InsertProgressData*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertProgress->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTaskPorgress No Find UserID:%d",pInsertProgress->dwUserID);
		return;
	}

	pUser->m_userTask.InsertProgressTask(pInsertProgress->taskPData);

	return;
}

//得到日常任务进度信息
CT_VOID CCInsertUserInfo::DisposeDailyQuest(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertDailyQuestInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeDailyQuest sizeof(MSC_DB_InsertDailyQuestInfo) != iLen");
		return;
	}

	MSC_DB_InsertDailyQuestInfo *pInsertDailyQuest=(MSC_DB_InsertDailyQuestInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertDailyQuest->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeDailyQuest No Find UserID:%d",pInsertDailyQuest->dwUserID);
		return;
	}

	pUser->m_userDaily.InsertQuestData(pInsertDailyQuest->stInfo);
	return;
}

//得到活跃度信息
CT_VOID CCInsertUserInfo::DisposeDailyActive(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertDailyActive) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeDailyActive sizeof(MSC_DB_InsertDailyActive) != iLen");
		return;
	}

	MSC_DB_InsertDailyActive *pInsertDailyActive=(MSC_DB_InsertDailyActive*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertDailyActive->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeDailyActive No Find UserID:%d",pInsertDailyActive->dwUserID);
		return;
	}

	pUser->m_userDaily.SetActiveData(pInsertDailyActive->stInfo);
	return;
}

//得到建筑信息
CT_VOID CCInsertUserInfo::DisposeBuildingInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertBuildingInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBuildingInfo sizeof(MSC_DB_InsertBuildingInfo) != iLen");
		return;
	}
	MSC_DB_InsertBuildingInfo *pInsertBuild=(MSC_DB_InsertBuildingInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertBuild->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBuildingInfo No Find UserID:%d",pInsertBuild->dwUserID);
		return;
	}

	pUser->m_userBuilding. InsertBuildData(pInsertBuild->buildData);

	return;
}

//得到商店状态
CT_VOID CCInsertUserInfo::DisposeMarketFunction(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertMarketFunction) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMarketFunction sizeof(MSC_DB_InsertMarketFunction) != iLen");
		return;
	}

	MSC_DB_InsertMarketFunction *pInsertMarket=(MSC_DB_InsertMarketFunction*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertMarket->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMarketFunction No Find UserID:%d",pInsertMarket->dwUserID);
		return;
	}
	pUser->m_userBlackMarket.GetMarketStatus(pInsertMarket->marketFun);
/*
	//商城
	G_ValueRange vr={0};
	if (G_GetValueRange(VALUE_TYPE_MARKET_NORMALREFRESHTIME,&vr))
	{
		CooldownData data={0};
		data.iType=MARKETREFRESH_COOL;
		data.leftTime= (CT_INT32)(g_lMarketRefreshTime+vr.fMaxVal-time(0));
		pUser->m_userBase.InsertCooldownData(data);
	}

	//黑店
	memset(&vr,0,sizeof(vr));
	if (G_GetValueRange(VALUE_TYPE_MARKET_BLACKREFRESHTIME_1,&vr))
	{
		CooldownData data={0};
		data.iType=BLACKREFRESH_COOL;
		data.leftTime= (CT_INT32)(g_lBlackRefreshTime+vr.fMaxVal-time(0));
		pUser->m_userBase.InsertCooldownData(data);
	}
*/
	return;
}

//获取黑店信息
CT_VOID CCInsertUserInfo::DisposeMarketInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertBlackMarketInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMarketInfo sizeof(MSC_DB_InsertBlackMarketInfo) != iLen");
		return;
	}

	MSC_DB_InsertBlackMarketInfo *pInsertBlackMarket=(MSC_DB_InsertBlackMarketInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertBlackMarket->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMarketInfo No Find UserID:%d",pInsertBlackMarket->dwUserID);
		return;
	}
	pUser->m_userBlackMarket.GetBlackMarketInfo(pInsertBlackMarket->blackMarket);

	return;
}

//得到宠物信息
CT_VOID CCInsertUserInfo::DisposePetInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertPetInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePetInfo sizeof(MSC_DB_InsertPetInfo) != iLen");
		return;
	}

	MSC_DB_InsertPetInfo *pInsertPet=(MSC_DB_InsertPetInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertPet->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePetInfo No Find UserID:%d",pInsertPet->dwUserID);
		return;
	}
	pUser->m_userPet.InsertPetBase(pInsertPet->pi);

	return;
}

//获取vip信息
CT_VOID CCInsertUserInfo::DisposeVipInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertVipInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeVipInfo sizeof(MSC_DB_InsertVipInfo) != iLen");
		return;
	}

	MSC_DB_InsertVipInfo *pInsertVip=(MSC_DB_InsertVipInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertVip->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeVipInfo No Find UserID:%d",pInsertVip->dwUserID);
		return;
	}

	pUser->m_userVip.VipGetDBInfo(pInsertVip->iGrade,pInsertVip->iTime,pInsertVip->iGift);


	return;
}

//竞技场信息
CT_VOID CCInsertUserInfo::DisposePVPInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertPVPInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePVPInfo sizeof(MSC_DB_InsertPVPInfo) != iLen");
		return;
	}

	MSC_DB_InsertPVPInfo *pInsertPVPInfo=(MSC_DB_InsertPVPInfo*)pBuf;
	MasterData MasterInfo={0};
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertPVPInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePVPInfo No Find UserID:%d",pInsertPVPInfo->dwUserID);
		return;
	}


	CT_INT64 lTime=(CT_INT64)time(0);
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(pInsertPVPInfo->PVPdata.lRefreshTime <= lTime-MasterInfo.llLastExitTime)
	{
		pInsertPVPInfo->PVPdata.lRefreshTime=0;
		pInsertPVPInfo->PVPdata.lBeginTime=0; 
		pUser->m_userPVPInfo.InsertBufferData(pInsertPVPInfo->PVPdata);
	}
	else
	{
		pInsertPVPInfo->PVPdata.lRefreshTime=pInsertPVPInfo->PVPdata.lRefreshTime;
		pInsertPVPInfo->PVPdata.lBeginTime=lTime;
		pUser->m_userPVPInfo.InsertBufferData(pInsertPVPInfo->PVPdata);
	}

	//竞技度计算
	tm tmOpenTime={0};
	tm tmExitTime={0};
	G_SecondsToTime((CT_CHAR*)&tmOpenTime,g_lOpenServerTime);
	G_SecondsToTime((CT_CHAR*)&tmExitTime,MasterInfo.llLastExitTime);

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPDEGREEUP,&vr))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetPVPInfo G_GetValueRange VALUE_TYPE_PVPDEGREEUP");
	}

	CT_INT32 iHour=(CT_INT32)(time(0)-MasterInfo.llLastExitTime)/3600;
	if(iHour > 0)
	{
		pUser->m_userPVPInfo.SetPVPDegree((CT_INT32)(vr.fMinVal*iHour));
		if(tmOpenTime.tm_min < tmExitTime.tm_min)
		{
			pUser->m_userPVPInfo.SetPVPDegree((CT_INT32)vr.fMinVal);
		}
		else if(tmOpenTime.tm_min == tmExitTime.tm_min)
		{
			if(tmOpenTime.tm_sec < tmExitTime.tm_sec)
			{
				pUser->m_userPVPInfo.SetPVPDegree((CT_INT32)vr.fMinVal);
			}
		}
	}

	//对手信息
	pUser->m_userPVPInfo.InsertChallengUser(pInsertPVPInfo->PVPdata);
	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT;++i)
	{
		pUser->m_userPVPInfo.SetChallengInfo(i,pInsertPVPInfo->UserFightinginfo[i],pInsertPVPInfo->LookUserinfo[i]);
	}

	//登录完成
	MSC_DB_R_BaseInfo ui={0};
	ui.dwUserID = pInsertPVPInfo->dwUserID;
	ui.pNetInterface = pInsertPVPInfo->pNetInterface;
	G_SendThreadInfo(pInsertPVPInfo->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_ENTER_FINISH, 
		(CT_CHAR*)&ui, sizeof(ui));
	return;
}

//排行榜
CT_VOID CCInsertUserInfo::DisposeTop(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertTop) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePVPTop sizeof(MSC_DB_InsertTop) != iLen");
		return;
	}

	MSC_DB_InsertTop *pInsertTop=(MSC_DB_InsertTop*)pBuf;

	CCTop::getSingleton().InsertTopUserInfo(pInsertTop->LookUserinfo,TOP_COUNT,(ENUM_TOP)pInsertTop->iType);

	return;
}

//战报记录
CT_VOID CCInsertUserInfo::DisposePVPRecord(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertPVPRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePVPRecord sizeof(MSC_DB_InsertPVPRecord) != iLen");
		return;
	}

	MSC_DB_InsertPVPRecord *pInsertPVPRecord=(MSC_DB_InsertPVPRecord*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertPVPRecord->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePVPRecord No Find UserID:%d",pInsertPVPRecord->dwUserID);
		return;
	}


	for(CT_INT32 i=0;i<PVP_FIGHTING_COUNT;++i)
	{
		pUser->m_userPVPInfo.InsertRecordFighting(pInsertPVPRecord->FightingInfo[i]);
		pUser->m_userPVPInfo.InsertRecordInfo(i,pInsertPVPRecord->FightingRecord[i].PVPFightinginfo);
	}

	return;
}

//得到签到信息
CT_VOID CCInsertUserInfo::DisposeSignInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertDailySignInfo) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSignInfo sizeof(MSC_DB_InsertDailySignInfo) != iLen");
		return;
	}

	MSC_DB_InsertDailySignInfo *pInsertDailySignInfo=(MSC_DB_InsertDailySignInfo*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertDailySignInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSignInfo No Find UserID:%d",pInsertDailySignInfo->dwUserID);
		return;
	}

	pUser->m_userDaily.SetSignData(pInsertDailySignInfo->stInfo);

	return;
}

//获取附魔信息
CT_VOID CCInsertUserInfo::DisposeEnchantdata(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertEnchantData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeEnchantdata sizeof(MSC_DB_InsertEnchantData) != iLen");
		return;
	}

	MSC_DB_InsertEnchantData *pInsertEnchantData=(MSC_DB_InsertEnchantData*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertEnchantData->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeEnchantdata No Find UserID:%d",pInsertEnchantData->dwUserID);
		return;
	}

	EnchantData data={0};
	data.iBID=pInsertEnchantData->iBID;
	data.iGrade=pInsertEnchantData->iGrade;
	data.iIDX=pInsertEnchantData->iIDX;
	data.iFailCount = pInsertEnchantData->iFailCount;
	pUser->m_userEnchant.InsertData(G_Combine64(data.iBID, data.iIDX) ,data);

	return;
}

//获取附魔奖励信息
CT_VOID CCInsertUserInfo::DisposeEnchantReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if (sizeof(MSC_DB_InsertEnchantReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeEnchantReward sizeof(MSC_DB_InsertEnchantReward) != iLen");
		return;
	}

	MSC_DB_InsertEnchantReward *pInsertEnchantReward = (MSC_DB_InsertEnchantReward *)pBuf;
	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(pInsertEnchantReward->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertuserInfo::DisposeEnchantReward No Find UserID:%d",pInsertEnchantReward->dwUserID);
		return;
	}

	pUser->m_userEnchant.m_EnchantRewardVector.push_back(pInsertEnchantReward->iIndex);
	return;
}

//得到购买战斗次数
CT_VOID CCInsertUserInfo::DisposeBuyFightingTime(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertBuyFightingTime) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeEnchantdata sizeof(MSC_DB_InsertEnchantData) != iLen");
		return;
	}

	MSC_DB_InsertBuyFightingTime *pInsertBuyFightingTime=(MSC_DB_InsertBuyFightingTime*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertBuyFightingTime->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBuyFightingTime No Find UserID:%d",pInsertBuyFightingTime->dwUserID);
		return;
	}

	pUser->m_userDuplicate.InsertBuyFighting(pInsertBuyFightingTime->iBuyFightingTime);
	return;
}

//扫荡信息
CT_VOID CCInsertUserInfo::DisposeRaidsInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_InsertRaidsInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeRaidsInfo len != sizeof(MSC_DB_InsertRaidsInfo)");
		return;
	}

	MSC_DB_InsertRaidsInfo *pInsertRaidsInfo=(MSC_DB_InsertRaidsInfo*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertRaidsInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeRaidsInfo No Find UserID:%d",pInsertRaidsInfo->dwUserID);
		return;
	}

	RaidsInfo Raidsinfo={0};
	Raidsinfo.iAllTime=pInsertRaidsInfo->iAllTime;
	Raidsinfo.iCurrentTime=pInsertRaidsInfo->iCurrentTime;
	Raidsinfo.iDifficulty=pInsertRaidsInfo->iDifficulty;
	Raidsinfo.iSceneID=pInsertRaidsInfo->iSceneID;
	Raidsinfo.iWorldID=pInsertRaidsInfo->iWorldID;
	Raidsinfo.lBeginTime=pInsertRaidsInfo->lBeginTime;
	Raidsinfo.iLastStamina=pInsertRaidsInfo->iLastStamina;

	pUser->m_userDuplicate.InsterRaidsInfo(Raidsinfo);

	return;
}

//扫荡奖励
CT_VOID CCInsertUserInfo::DisposeRaidsReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_InsertRaidsReward))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeRaidsReward len != sizeof(MSC_DB_InsertRaidsInfo)");
		return;
	}

	MSC_DB_InsertRaidsReward *pInsertRaidsReward=(MSC_DB_InsertRaidsReward*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertRaidsReward->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeRaidsReward No Find UserID:%d",pInsertRaidsReward->dwUserID);
		return;
	}

	CMD_G_RaidsReward RaidReward={0};
	RaidReward.dwUserID=pInsertRaidsReward->dwUserID;
	RaidReward.iDiamond=pInsertRaidsReward->iDiamond;
	RaidReward.iExperience=pInsertRaidsReward->iExperience;
	RaidReward.iGold=pInsertRaidsReward->iGold;
	RaidReward.iPotential=pInsertRaidsReward->iPotential;
	RaidReward.iSkillPoint=pInsertRaidsReward->iSkillPoint;
	RaidReward.iCurrentTime=pInsertRaidsReward->iCurrentTime;
	memcpy(RaidReward.iGoods,pInsertRaidsReward->iGoods,sizeof(RaidReward.iGoods));
	memcpy(RaidReward.iGoodsCount,pInsertRaidsReward->iGoodsCount,sizeof(RaidReward.iGoodsCount));

	pUser->m_userDuplicate.InsertRaidsReward(RaidReward);
	return;
}

CT_VOID CCInsertUserInfo::DisposeFriendInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_FRIEND))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeFriendInfo len != sizeof(MSC_DB_FRIEND)");
		return;
	}

	MSC_DB_FRIEND *pInsertFriend=(MSC_DB_FRIEND*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertFriend->dwU_ID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeFriendInfo No Find UserID:%d",pInsertFriend->dwU_ID);
		return;
	}

	if (pInsertFriend->iF_Type == FRIEND_BLACKREV)
	{
		pUser->m_userFriend.InsertBlackRev(pInsertFriend->dwF_UID);
		return;
	}

	CMD_G_FriendData FriendData={0};
	FriendData.dwUserID			=pInsertFriend->dwU_ID;
	FriendData.dwF_UID			=pInsertFriend->dwF_UID;
	FriendData.iF_Occupation	=pInsertFriend->iF_Occupation;
	FriendData.iF_Grade			=pInsertFriend->iF_Grade;
	memcpy(FriendData.cF_NickName,pInsertFriend->cF_NickName,sizeof(FriendData.cF_NickName));
	FriendData.iF_FightingPower = pInsertFriend->iF_FightingPower;
	FriendData.iLastExitTime	= pInsertFriend->iLastExitTime;
	FriendData.iTitleID			= pInsertFriend->iTitleID;

	CCUser *pF_User=0;
	if(CCUserManager::getSingleton().Find(pInsertFriend->dwF_UID,&pF_User))	//如果对方在线, 则加上等级,战斗力及在线状态
	{
		MasterData masterData = {0};
		pF_User->m_userBase.GetMasterData(masterData);
		FriendData.iF_Grade			= masterData.mvd.stGrade.iLevel;
		FriendData.iF_Status		= 1;
		FriendData.iF_FightingPower = masterData.mvd.stRes.iFightingPower;
	}

	switch(pInsertFriend->iF_Type)
	{
	case FRIEND_REQ:
		pUser->m_userFriend.InsertFriendReq(FriendData);
		break;
	case FRIEND_DATA:
		pUser->m_userFriend.InsertFriendData(FriendData);
		break;
	case FRIEND_BLACKNAME:
		pUser->m_userFriend.InsertBlackName(FriendData);
		break;
	}

	return;
}

CT_VOID CCInsertUserInfo::DisposePresent(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_Present))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePresent len != sizeof(MSC_DB_Present)");
		return;
	}

	MSC_DB_Present *pInsertPresent=(MSC_DB_Present*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsertPresent->dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePresent No Find UserID:%d",pInsertPresent->dwUID);
		return;
	}

	pUser->m_userActive.InsertPresent(*pInsertPresent);
	/*
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	if (0 == pUser->m_userItem.AutoAddItemData(threadid,pInsertPresent->iP_ID,pInsertPresent->iP_Count))
	{
		//更新DB
		CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_LOGIN,MSC_SUB_DB_PRESENT,(CT_CHAR*)pInsertPresent,sizeof(MSC_DB_Present));
	}*/
}

//个人限购
CT_VOID CCInsertUserInfo::DisposeMarketPersonal(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_InsertMarketPersonal))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMarketPersonal len != sizeof(MSC_DB_InsertMarketPersonal)");
		return;
	}

	MSC_DB_InsertMarketPersonal *pMarketPersonal=(MSC_DB_InsertMarketPersonal*)pBuf;

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(pMarketPersonal->dwUserID,&pUser))
	{
		pUser->m_userBlackMarket.InsertMarketPersonal(pMarketPersonal->iGoods,pMarketPersonal->iNum);
	}
	return;
}

//组队副本
CT_VOID CCInsertUserInfo::DisposeTeamDuplicate(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_InsertTeamDuplicateScene))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTeamDuplicate len != sizeof(MSC_DB_InsertTeamDuplicateScene)");
		return;
	}

	MSC_DB_InsertTeamDuplicateScene *pSceneInfo=(MSC_DB_InsertTeamDuplicateScene*)pBuf;

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(pSceneInfo->dwUserID,&pUser))
	{
		pUser->m_userTeamDuplicate.InsertSceneData(pSceneInfo->iSceneID,pSceneInfo->iDifficulty,pSceneInfo->iWin);
	}
	return;
}

// 用户元素之力数据
CT_VOID CCInsertUserInfo::DisposeElmPowerInfo( CT_CHAR *pBuf,CT_INT32 iLen )
{
    if(sizeof(MSC_DB_InsertItemInfo) != iLen)
    {
        CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeElmPowerInfo sizeof(MSC_DB_InsertItemInfo) != iLen");

        return;
    }

    MSC_DB_InsertItemInfo *pInsertItem=(MSC_DB_InsertItemInfo*)pBuf;
    CCUser *pUser=0;
    if(!CCUserManager::getSingleton().Find(pInsertItem->dwUserID,&pUser))
    {
        CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeElmPowerInfo No Find UserID:%d",pInsertItem->dwUserID);
        return;
    }
    pUser->m_userElmPower.InsertData(pInsertItem->itemData.iIndex, pInsertItem->itemData);
    return;
}
// 战旗
CT_VOID CCInsertUserInfo::DisposeBattleFlagInfo( CT_CHAR *pBuf,CT_INT32 iLen )
{
    if(sizeof(MSC_DB_InsertBattleFlagInfo) != iLen)
    {
        CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBattleFlagInfo sizeof(DisposeBattleFlagInfo) != iLen");

        return;
    }
    MSC_DB_InsertBattleFlagInfo *pInsert=(MSC_DB_InsertBattleFlagInfo*)pBuf;
    CCUser *pUser=0;
    if(!CCUserManager::getSingleton().Find(pInsert->dwUserID,&pUser))
    {
        CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeElmPowerInfo No Find UserID:%d",pInsert->dwUserID);
        return;
    }
    pUser->m_userBattleFlag.InsertData(pInsert->flagData.iType, pInsert->flagData);
    return;
}

// 元素属性
CT_VOID CCInsertUserInfo::DisposeElmAttrInfo( CT_CHAR *pBuf,CT_INT32 iLen )
{
    if(sizeof(MSC_DB_InsertElmAttrInfo) != iLen)
    {
        CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::MSC_DB_InsertElmAttrInfo sizeof(MSC_DB_InsertElmAttrInfo) != iLen");

        return;
    }
    MSC_DB_InsertElmAttrInfo *pInsert=(MSC_DB_InsertElmAttrInfo*)pBuf;
    CCUser *pUser=0;
    if(!CCUserManager::getSingleton().Find(pInsert->dwUserID,&pUser))
    {
        CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::MSC_DB_InsertElmAttrInfo No Find UserID:%d",pInsert->dwUserID);
        return;
    }

    // 修正元素属性
    // 经验值

    // 属性

    pUser->m_userElmAttr.InsertData(pInsert->elmData.key, pInsert->elmData);
    return;
}

CT_VOID CCInsertUserInfo::DisposeTower(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_TOWER) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTower sizeof(MSC_DB_TOWER) != iLen");

		return;
	}
	MSC_DB_TOWER *pInsert=(MSC_DB_TOWER*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsert->dwU_ID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTower No Find UserID:%d",pInsert->dwU_ID);
		return;
	}
	TowerData towerData = {0};
	towerData.iCPID = pInsert->iCPID;
	towerData.iTreasure = pInsert->iTreasure;
	towerData.iTime = pInsert->iTime;
	pUser->m_userTower.InsertTowerData(towerData);
}

CT_VOID CCInsertUserInfo::DisposeTowerSlotReward(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_TowerSlotReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerSlotReward sizeof(MSC_DB_TowerSlotReward) != iLen");

		return;
	}
	MSC_DB_TowerSlotReward *pInsert=(MSC_DB_TowerSlotReward*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsert->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerSlotReward No Find UserID:%d",pInsert->dwUserID);
		return;
	}
	TowerSlotAward towerSlotAward = {0};
	towerSlotAward.dwUserID = pInsert->dwUserID;
	towerSlotAward.iFloor = pInsert->iFloor;
	towerSlotAward.iSlot = pInsert->iSlot;
	towerSlotAward.gReward = pInsert->mnReward;
	pUser->m_userTower.InsertTowerSlotAward(towerSlotAward, 1);
}

CT_VOID CCInsertUserInfo::DisposeTowerEvent(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(TowerEvent) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerEvent sizeof(TowerEvent) != iLen");

		return;
	}
	TowerEvent *pInsert=(TowerEvent*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsert->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerEvent No Find UserID:%d",pInsert->dwUserID);
		return;
	}
	pUser->m_userTower.InitEvent(*pInsert);
}

CT_VOID CCInsertUserInfo::DisposeTowerSlotFighting(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_TowerPVP_ResRobbery) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerSlotFighting sizeof(MSC_TowerPVP_ResRobbery) != iLen");

		return;
	}
	MSC_TowerPVP_ResRobbery *pInsert=(MSC_TowerPVP_ResRobbery*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsert->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerSlotFighting No Find UserID:%d",pInsert->dwUserID);
		return;
	}
	pUser->m_userTower.InitPVPInfo(*pInsert);
}

CT_VOID CCInsertUserInfo::DisposeTowerMSG(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_MSG) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerMSG sizeof(MSC_DB_MSG) != iLen");

		return;
	}
	MSC_DB_MSG *pInsert=(MSC_DB_MSG*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsert->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerMSG No Find UserID:%d",pInsert->dwUserID);
		return;
	}
	TowerMSG towerMSG = {0};
	towerMSG.dwUserID = pInsert->dwUserID;
	towerMSG.iType = pInsert->iMSGType;
	towerMSG.iTime = pInsert->iTime;
	memcpy(towerMSG.cMSG, pInsert->cMSG, sizeof(towerMSG.cMSG));
	pUser->m_userTower.InsertTowerMSG(towerMSG);
}

CT_VOID CCInsertUserInfo::DisposeTowerHelp(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_TowerHelp) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerHelp sizeof(MSC_DB_TowerHelp) != iLen");

		return;
	}
	MSC_DB_TowerHelp *pInsert=(MSC_DB_TowerHelp*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsert->dwU_ID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTowerHelp No Find UserID:%d",pInsert->dwU_ID);
		return;
	}
	pUser->m_userTower.InitHelp(pInsert->dwF_ID);
}

//战队科技
CT_VOID CCInsertUserInfo::DisposeFightTeamScience(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsterFightTeamScience) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeFightTeamScience sizeof(MSC_DB_InsterFightTeamScience) != iLen");
		return;
	}

	MSC_DB_InsterFightTeamScience *pFts=(MSC_DB_InsterFightTeamScience*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFts->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeFightTeamScience No Find UserID:%d",pFts->dwUserID);
		return;
	}

	NemberScience ns = {0};
	ns.dwUserID = pFts->dwUserID;
	ns.iScienceID = pFts->iScienceID;
	ns.iScienceGrade=pFts->iGrade;
	pUser->m_userFightTeamNember.InsertMemberScience(ns);
	return;
}

//宠物幻化
CT_VOID CCInsertUserInfo::DisposePetChange(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_InsertPetChange) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePetChange sizeof(MSC_DB_InsertPetChange) != iLen");
		return;
	}

	MSC_DB_InsertPetChange *pIpc=(MSC_DB_InsertPetChange*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pIpc->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePetChange No Find ID:%d",pIpc->dwUserID);
		return;
	}

	PetChange pc={0};
	pc.iChangeID=pIpc->iChangeID;
	pc.iSkillID=pIpc->iSkillID;
	pUser->m_userPet.InsertPetChange(pc);
	return;
}

CT_VOID CCInsertUserInfo::DisposeBossRecord(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_DB_WorldBossRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBossRecord sizeof(MSC_DB_WorldBossRecord) != iLen");

		return;
	}
	MSC_DB_WorldBossRecord *pInsert=(MSC_DB_WorldBossRecord*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInsert->dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeBossRecord No Find UserID:%d",pInsert->dwUID);
		return;
	}
	pUser->m_userBoss.InsertRecord(pInsert->iAEID);
}

CT_VOID CCInsertUserInfo::DisposeTitleData(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_DB_TitleData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTitleData sizeof(MSC_DB_InsertTitle) != iLen");
		return;
	}

	MSC_DB_TitleData* pIT =(MSC_DB_TitleData*)pBuf;
	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(pIT->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTitleData No Find UserID:%d", pIT->dwUserID);
		return;
	}

	TitleData td = {0};
	td.iID = pIT->iTitleID;
	td.iActiveTime = pIT->iActiveTime;
	td.iEffectiveTime = pIT->iEffectTime;
	pIT->iStatus == TITLE_USE? td.bIsUse = CT_TRUE:td.bIsUse = CT_FALSE;
	pUser->m_userTitle.InsertData(pIT->iTitleID, td);
}

CT_VOID CCInsertUserInfo::DisposeIslandData(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_DB_IslandData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandData sizeof(MSC_DB_IslandData) != iLen");
		return;
	}

	MSC_DB_IslandData* pIslandData = (MSC_DB_IslandData*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pIslandData->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandData connot find user.uid:%d", pIslandData->dwUserID);
		return;
	}

	Point point = {0};
	DragonPlayer DPlayer = {0};
	G_Point gPoint = {0};
	G_PointEvent gPointEvent = {0};
	G_ValueRange gvr = {0};
	if (!G_GetPoint(pIslandData->iPoint, &gPoint)&& pIslandData->iPoint != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandData G_GetPoint error.point:%d", pIslandData->iPoint);
		return;
	}
	if (!G_GetPointEvent(pIslandData->iEventID, &gPointEvent) && pIslandData->iEventID != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandData G_GetPointEvent error.eventid:%d", pIslandData->iEventID);
		return;
	}
	if (!G_GetValueRange(VALUE_TYPE_ISLAND_THROWTIMES, &gvr))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandData G_GetValueRange error.id:%d", VALUE_TYPE_ISLAND_THROWTIMES);
		return;
	}
	point.iPointID = pIslandData->iPoint;
	point.iEventID = pIslandData->iEventID;
	point.iStatus = pIslandData->iEventStatus;
	point.iBeginChest = pIslandData->iStartChest;
	point.iType = gPoint.iEventType;
	point.iStep = pIslandData->iStep;
	if (point.iStatus == ISLAND_EVENT_COMPLETE)
	{
		point.iEventChest = gPointEvent.iReward;
	}
	pUser->m_userIsland.SetPoint(point);

	
	DPlayer.iBeginTimes = pIslandData->iStartTimes;
	DPlayer.iFightTeamID = pIslandData->iFT_ID;
	DPlayer.iGetTimes = pIslandData->iGetTimes;
	DPlayer.iScore = pIslandData->iScore;
	DPlayer.iThrowTimes = pIslandData->iThrowTimes;
	DPlayer.iTotalTimes = gvr.fMinVal + pIslandData->iGetTimes;
	DPlayer.iIsOpen = pIslandData->iIsOpen;
	DPlayer.iRewardTimes = pIslandData->iRewardTimes;
	pUser->m_userIsland.SetPlayerInfo(DPlayer);
}

CT_VOID CCInsertUserInfo::DisposeIslandGiveUid(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_DB_IslandGiveUid) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandGiveUid sizeof(MSC_DB_IslandGiveUid) != iLen");
		return;
	}

	MSC_DB_IslandGiveUid* pIslandGiveUid = (MSC_DB_IslandGiveUid*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pIslandGiveUid->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandGiveUid connot find user.uid:%d", pIslandGiveUid->dwUserID);
		return;
	}

	pUser->m_userIsland.InsertGiveUserID(pIslandGiveUid->aGiveUid);
}

CT_VOID CCInsertUserInfo::DisposeIslandScoreReward(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_DB_IslandScoreReward) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandScoreReward sizeof(MSC_DB_IslandScoreReward) != iLen");
		return;
	}

	MSC_DB_IslandScoreReward* pIslandSR = (MSC_DB_IslandScoreReward*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pIslandSR->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeIslandScoreReward connot find user.uid:%d", pIslandSR->dwUserID);
		return;
	}

	pUser->m_userIsland.InsertTeamChest(pIslandSR->aScoreReward);
}

CT_VOID CCInsertUserInfo::DipsoseIslandPresentName(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_DB_IslandPresentUserName) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DipsoseIslandPresentName sizeof(MSC_DB_IslandPresentUserName) != iLen");
		return;
	}

	MSC_DB_IslandPresentUserName* pName = (MSC_DB_IslandPresentUserName*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pName->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DipsoseIslandPresentName connot find user.uid:%d", pName->dwUserID);
		return;
	}

	std::string str=pName->cUserName;
	pUser->m_userIsland.InsertPresentName(str);
}

//获取城墙加成
CT_VOID CCInsertUserInfo::DisposeSeekWallAddition(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_DB_InsertWallAddition) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekWallAddition sizeof(MSC_DB_InsertWallAddition) != iLen");
		return;
	}

	MSC_DB_InsertWallAddition* pIwa = (MSC_DB_InsertWallAddition*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pIwa->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekWallAddition connot find user.uid:%d", pIwa->dwUserID);
		return;
	}

	WallAddition wa={0};
	wa.iID=pIwa->iID;
	wa.iGrade=pIwa->iGrade;
	pUser->m_userSeek.InsertWallAddition(wa);
	return;
}

//插入城墙军队
CT_VOID CCInsertUserInfo::DisposeWallArmy(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (sizeof(MSC_DB_InsertWallArmy) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWallArmy sizeof(MSC_DB_InsertWallArmy) != iLen");
		return;
	}

	MSC_DB_InsertWallArmy* pIwa = (MSC_DB_InsertWallArmy*)pBuf;
	CCUser* pUser;
	if (!CCUserManager::getSingleton().Find(pIwa->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWallArmy connot find user.uid:%d", pIwa->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertWallArmy(pIwa->iPos);
	return;
}

//插入可探访随机列表
CT_VOID CCInsertUserInfo::DisposeSeekRandPlayer(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_Seek_FriendGet))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekRandPlayer iLen != sizeof(MSC_Seek_FriendGet)");
		return;
	}
	
	MSC_Seek_FriendGet* pFg = (MSC_Seek_FriendGet*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pFg->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekRandPlayer connot find user,uid:%d", pFg->dwUserID);
		return;
	}

	UserInfo userInfo  = {0};
	userInfo.dwUserID = pFg->dwFUserID;
	strncpy(userInfo.cUserName, pFg->cUserName, sizeof(userInfo.cUserName));
	userInfo.iLevel = pFg->iLevel;
	userInfo.iOccupation = pFg->iOccupation;
	pUser->m_userSeek.InsertSeekRandPlayer(userInfo);
	return;
}

//插入已探访的列表
CT_VOID CCInsertUserInfo::DisposeHadSeekedPlayer(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_Had_SeekedPlayer))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeHadSeekedPlayer iLen != sizeof(MSC_DB_Had_SeekedPlayer)");
		return;
	}

	MSC_DB_Had_SeekedPlayer* player = (MSC_DB_Had_SeekedPlayer*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(player->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeHadSeekedPlayer connot find user.uid:%d", player->dwUserID);
		return;
	}

	if (player->iType == SEEK_GOODWILL)
	{
		pUser->m_userSeek.InsertSeekedGoodwill(player->dwFUserID);
	}
	else if (player->iType == SEEK_SPITE)
	{
		pUser->m_userSeek.InsertSeekedSpite(player->dwFUserID);
	}
	return;
}

//插入战队旗帜
CT_VOID CCInsertUserInfo::DisposeTeamFlagInsert(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekTeamFlag))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTeamFlagInsert iLen != sizeof(TeamSeek)");
		return;
	}

	MSC_DB_SeekTeamFlag* pTeam = (MSC_DB_SeekTeamFlag*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pTeam->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeTeamFlagInsert connot find user,uid:%d", pTeam->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertFlags(pTeam->teamFlag);
}

//插入圣女人质信息
CT_VOID CCInsertUserInfo::DisposeSaintessKidnap(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekKidnap))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSaintessKidnap iLen != sizeof(MSC_DB_SeekKidnap)");
		return;
	}

	MSC_DB_SeekKidnap *pSaintess = (MSC_DB_SeekKidnap *)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pSaintess->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSaintessKidnap connot find user,uid:%d", pSaintess->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertHostages(KIDNAP_WOMAN, pSaintess->kidnap);
}

//插入工人人质信息
CT_VOID CCInsertUserInfo::DisposeWorkerKidnap(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekKidnap))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWorkerKidnap iLen != sizeof(MSC_DB_SeekKidnap)");
		return;
	}

	MSC_DB_SeekKidnap *pWorker = (MSC_DB_SeekKidnap *)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pWorker->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWorkerKidnap connot find user,uid:%d", pWorker->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertHostages(KIDNAP_FARMER, pWorker->kidnap);
}

//插入初始善访人员列表
CT_VOID CCInsertUserInfo::DisposeWillSeekerList(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekListPlayer))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWillSeekerList iLen != sizeof(MSC_DB_SeekListPlayer)");
		return;
	}

	MSC_DB_SeekListPlayer* pPlayer = (MSC_DB_SeekListPlayer*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pPlayer->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeWillSeekerList connot find user,uid:%d", pPlayer->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertFriendList(pPlayer->playerInfo);
}

//插入初始恶访人员列表
CT_VOID CCInsertUserInfo::DisposeSpiteSeekerList(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekListPlayer))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSpiteSeekerList iLen != sizeof(MSC_DB_SeekListPlayer)");
		return;
	}

	MSC_DB_SeekListPlayer* pPlayer = (MSC_DB_SeekListPlayer*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pPlayer->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSpiteSeekerList connot find user,uid:%d", pPlayer->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertRandList(pPlayer->playerInfo);
}

//插入探访基本数据
CT_VOID CCInsertUserInfo::DisposeSeekDataInsert(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekData))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekDataInsert iLen != sizeof(MSC_DB_SeekData)");
		return;
	}

	MSC_DB_SeekData* pData = (MSC_DB_SeekData*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pData->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekDataInsert connot find user,uid:%d", pData->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertSeekData(pData->seekData);
}

//插入探访记录
CT_VOID CCInsertUserInfo::DisposeSeekVisitorInfo(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekVistorInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekVisitorInfo iLen != sizeof(MSC_DB_SeekVistorInfo)");
		return;
	}

	MSC_DB_SeekVistorInfo* pInfo = (MSC_DB_SeekVistorInfo*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pInfo->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekVisitorInfo connot find uid:%d", pInfo->dwUserID);
		return;
	}

	pUser->m_userSeek.AddVistorInfo(pInfo->vistorInfo);
}

//插入探访战报
CT_VOID CCInsertUserInfo::DisposeSeekFightRecord(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_SeekFightRecord))
	{
		CCLogMeg::getSingleton().WriteWaring("CCInsertUserInfo::DisposeSeekFightRecord iLen != sizeof(MSC_DB_SeekFightRecord)");
		return;
	}

	MSC_DB_SeekFightRecord *p=(MSC_DB_SeekFightRecord*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(p->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekFightRecord connot find uid:%d", p->dwUserID);
		return;
	}

	pUser->m_userSeek.InsertFightingData(*p);
	return;
}

//插入领主战斗信息
CT_VOID CCInsertUserInfo::DisposeMasterFightAdd(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(iLen != sizeof(MSC_DB_MasterFightSaveData))
	{
		CCLogMeg::getSingleton().WriteWaring("CCInsertUserInfo::DisposeMasterFightAdd iLen != sizeof(MSC_DB_MasterFightSaveData)");
		return;
	}

	MSC_DB_MasterFightSaveData *p=(MSC_DB_MasterFightSaveData*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(p->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeMasterFightAdd connot find uid:%d", p->dwUserID);
		return;
	}

	pUser->m_userBase.SetMasterFightAdd(*p);
	return;
}

//插入探访别人的信息
CT_VOID CCInsertUserInfo::DisposeSeekOthersInfo(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekVistorInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekOthersInfo iLen != sizeof(MSC_DB_SeekVistorInfo)");
		return;
	}

	MSC_DB_SeekVistorInfo* pInfo = (MSC_DB_SeekVistorInfo*)pBuf;
	CCUser* pUser = 0;
	if (CCUserManager::getSingleton().Find(pInfo->dwUserID, &pUser))
	{
		pUser->m_userSeek.AddSeekInfo(pInfo->vistorInfo);
	}
}

//插入宠物肉票
CT_VOID CCInsertUserInfo::DisposePetKidnap(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_SeekKidnap))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposePetKidnap iLen != sizeof(MSC_DB_SeekKidnap)");
		return;
	}

	MSC_DB_SeekKidnap* pPet = (MSC_DB_SeekKidnap*)pBuf;
	CCUser* pUser = 0;
	if (CCUserManager::getSingleton().Find(pPet->dwUserID, &pUser))
	{
		pUser->m_userSeek.InsertHostages(KIDNAP_PET,pPet->kidnap);
	}
}