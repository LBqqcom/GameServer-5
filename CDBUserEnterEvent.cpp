#include <list>
#include <map>
#include <ctime>
#include "CUser.h"
#include "CDBUserEnterEvent.h"
#include "CDatabaseMes.h"
#include "CThreadManager.h"
#include "CLogMeg.h"
#include "CTimerThread.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CDBConnector.h"
#include "CItemScriptSetting.h"
#include "CUserManager.h"
#include "CDBLogMes.h"
#include "CTop.h"
#include "CGameConfig.h"
#include "CBattleArray.h"
#include "CFightTeamManager.h"
#include "CFightTeam.h"
#include "CFightingData.h"
#include "CBuildingScriptSetting.h"
#include"CSeekEvent.h"

CCDBUserEnterEvent::CCDBUserEnterEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector) :
                   CCGameMesEvent(pMesManager, id), m_dbConnector(dbConnector)
{

	//登录消息
	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_DB_USRE_ENTER);
	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_DB_TOP_INFO);
	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_DB_CHALLENG);
	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_DB_ANOTHER);
	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_TEAMDUPLICATE_AUTOMATCH);
	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_DB_FTFightingData);
	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_DB_PRESENT);

	RegistMessage(MSC_MAIN_DB_LOGIN           , MSC_SUB_DB_TOWER_OFFLINE_DATA);
	
	RegistMessage(MSC_MAIN_DB_LOGIN			  ,	MSC_SUB_DB_GET_SEEKED);
	RegistMessage(MSC_MAIN_DB_LOGIN, MSC_SUB_DB_GET_OTHERSTOWN);

}

CCDBUserEnterEvent::~CCDBUserEnterEvent(void)
{
}

//处理用户消息
CT_VOID CCDBUserEnterEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
							            CT_CHAR* pBuf, CT_DWORD len,
							            CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case  MSC_MAIN_DB_LOGIN:
		{
			DisposeLoginSubEvent(subID, pBuf, len);
			break;
		}

	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::OnUserEvent find wrong mainID: %i", mainID);
			break;
		}
	}
}



CT_VOID CCDBUserEnterEvent::DisposeLoginSubEvent( CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch(subID)
	{
	case MSC_SUB_DB_USRE_ENTER:
		{
			
			if (len != sizeof(MSC_DB_UserEnter))
			{
				CCLogMeg::getSingleton().WriteError("CCDataBaseThread::DisposeLoginSubEvent: Len is wrong: len == sizeof(MSC_DB_UserEnter)");
				return ;
			}

			MSC_DB_UserEnter*   pTUE = (MSC_DB_UserEnter*) pBuf;
			if(
				!GetTaskFinishedData(pTUE)||  //提前处理任务pengwen
				!GetVipInfo(pTUE)||
				!GetMasterInfo(pTUE,subID)||
				!GetUpgradeBuildingInfo(pTUE)||
				!GetTalentInfo(pTUE)||
				!GetSkillInfo(pTUE)||
				!GetItemInfo(pTUE)||
				!GetEquipInfo(pTUE)||
                !GetBattleFlagInfo(pTUE)||
                !GetElmPowerInfo(pTUE)||
				!GetBuffInfo(pTUE)||          
				!GetCooldownInfo(pTUE)|| 
				!GetArmyInfo(pTUE)||
				!GetWorldSceneInfo(pTUE)||
				!GetSceneInfo(pTUE)||
				!GetRegionInfo(pTUE)||
				!GetBattleArray(pTUE)||
				!GetProductingBuilding(pTUE)||
				!GetDailyQuestInfo(pTUE)||
				!GetDailySignInfo(pTUE)||
				!GetDailyActiveInfo(pTUE)||
				!GetBuildingInfo(pTUE)||
				!GetMarketFunctionStatus(pTUE)||
				!GetBlackMarketInfo(pTUE)||
				!GetPetInfo(pTUE)||
				!GetEmailInfo(pTUE)||
				!GetEnchantRewardInfo(pTUE)||
				!GetEnchantInfo(pTUE)||
				!GetPVPRecord(pTUE)||
				!GetUserActiveEventInfo(pTUE)||
				!GetUserWorshipInfo(pTUE)||
				!GetRaidsInfo(pTUE)||
				!GetTeamDuplicateSceneInfo(pTUE)||
				!GetTitleInfo(pTUE)		||
				!GetFriendInfo(pTUE)||
				!GetPresent(pTUE)||
				!GetMarketPersonal(pTUE)||
				!GetTower(pTUE) ||
				!GetTowerHelp(pTUE) ||
				!GetTowerSlotReward(pTUE) ||
				!GetTowerEvent(pTUE) ||
				!GetTowerSlotFightRecord(pTUE) ||
				!GetMSG(pTUE) ||
				!GetBossRecord(pTUE) ||
				!GetFightTeamScience(pTUE)||
				!GetTaskProgressData(pTUE)||
				!GetDragonIslandData(pTUE) ||
				!GetWallAddition(pTUE)		||
				!GetWallArmy(pTUE)		||
				!GetSeekRandList(pTUE)	||
				!GetHadSeekedList(pTUE)	||
				!GetSeekInfo(pTUE)		||
				!GetMasterFightAdd(pTUE)		||
				!GetPVPInfo(pTUE)		//这条查询必须放在最后
				)
			{				
				CMD_G_LeaveGame LeaveGame={0};
				LeaveGame.dwUserID= pTUE->dwUserID;
				CT_DWORD dwThreadID=GAME_THREAD_CITY+(LeaveGame.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum);
				G_SendThreadInfo(dwThreadID,MSC_MAIN_USER_EXIT,0,(CT_CHAR*)&LeaveGame,sizeof(LeaveGame));

				G_SendThreadDbStandardInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
				pTUE->pNetInterface->Close();
			}
			
			
			break;
		}
	case MSC_SUB_DB_TOP_INFO:			//查找排行榜信息
		{
			GetTopInfo(pBuf,len);
			break;
		}
	case MSC_SUB_DB_CHALLENG:			//刷新对手
		{
			GetChalleng(pBuf,len);
			break;
		}
	case MSC_SUB_DB_ANOTHER:			//查看他人信息
		{
			GetAnother(pBuf,len);
			break;
		}
	case MSC_SUB_TEAMDUPLICATE_AUTOMATCH:
		{
			GetTeamDuplicateAutoMatch(pBuf,len);
			break;
		}
	case MSC_SUB_DB_FTFightingData:
		{
			GetFTFightingData(pBuf,len);
			break;
		}
	case MSC_SUB_DB_PRESENT:
		{
			UpdatePresent(pBuf,len);
			break;
		}
	case  MSC_SUB_DB_TOWER_OFFLINE_DATA:
		{
			GetTowerOffLineUserFightData(pBuf, len);
			break;
		}
	case MSC_SUB_DB_GET_SEEKED:
		{
			GetSeekerInfo(pBuf, len);
			break;
		}
	case MSC_SUB_DB_GET_OTHERSTOWN:
		{
			DisposeGetOtherTown(pBuf, len);
			break;
		}
	default:
		{
			break;
		}
	}
}

//非在线用户信息
CT_VOID CCDBUserEnterEvent::GetNoLinkUserInfo(CT_DWORD *dwUserID,UserFightingStatus *Userinfo,LookUserInfo *LookUserinfo,CT_INT32 iUserCount)
{
	
	for(CT_INT32 i=0;i<iUserCount;++i)
	{
		if(dwUserID[i] != 0)
		{
			FightingData fd={0};
			OnLinkUserData UserData ={0};

			ReadDBData(dwUserID[i],UserData.masterInfo);
			ReadDBData(dwUserID[i],UserData.talentMap);
			ReadDBData(dwUserID[i],UserData.skillMap);
			ReadDBData(dwUserID[i],UserData.itemMap);
			ReadDBData(dwUserID[i],UserData.equipMap);
			ReadDBData(dwUserID[i],UserData.buffMap);
			ReadDBData(dwUserID[i],UserData.armyMap);
			ReadDBData(dwUserID[i],UserData.BattleArraydata);
			ReadDBData(dwUserID[i],UserData.pi);
			ReadDBData(dwUserID[i],UserData.enchantMap);
			ReadDBData(dwUserID[i],UserData.flagMap);
			ReadDBData(dwUserID[i],UserData.elmMap, UserData.elmAttrMap);
			ReadDBData(dwUserID[i],UserData.towerMap);
			ReadDBData(dwUserID[i],UserData.scienceMap);
			ReadDBData(dwUserID[i],UserData.titleMap);
			ReadDBData(dwUserID[i],UserData.enchantRewardVector);
			CT_INT32 iTime, iGrade;
			ReadDBData(dwUserID[i],UserData.VipGrade, iTime, iGrade);
			ReadDBData(dwUserID[i],UserData.SeekArray);
			ReadDBData(dwUserID[i],UserData.wamap);
			ReadDBData(dwUserID[i],UserData.mfamap);
			ReadDBData(dwUserID[i],UserData.pcmap);

			//提取用户信息
			PTRLinkUserData linkUserData={0};
			GetPTRLinkUserData(UserData,linkUserData);
			G_GetLookUserInfo(linkUserData,fd.SeekFData,LookUserinfo[i],CT_TRUE);
			G_GetLookUserInfo(linkUserData,Userinfo[i],LookUserinfo[i]);

			
			memcpy(&fd.FData,&Userinfo[i],sizeof(Userinfo[i]));
			memcpy(&fd.LookData,&LookUserinfo[i],sizeof(LookUserinfo[i]));
			CT_DWORD threadid = GAME_THREAD_CITY;
			G_SendThreadInfo(threadid,MAIN_G_USER_DATA,MSC_SUB_USER_DATA_INSERT,(CT_CHAR*)&fd,sizeof(fd));
		}
	}
	return;
}
//得到领主信息
CT_BOOL CCDBUserEnterEvent::GetMasterInfo(const MSC_DB_UserEnter* pTUE,CT_DWORD subID)
{
    MasterData  masterData = {0};    
    
    // 读取领主数据
    CT_INT32 iRet = 0;
    
    if (ReadDBData(pTUE->dwUserID, masterData))
    {
        // 得到上次登陆时间
        m_dbConnector.Reset();
        m_dbConnector.SetPcName("SP_EveryDayLastLoginIn");
        m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
        if(m_dbConnector.Exec())
        {
            CCDBResult* pResult = m_dbConnector.GetResult();
            if (pResult->GetResult())
            { 
                while (pResult->MoveNextRow())
                {
                    pResult->GetValue(0, iRet);
                    if (iRet > 0)
                    {
                        CT_CHAR date[32];
                        pResult->GetValue(1, date, sizeof(date)); // yyyy-mm-dd hh:mm:ss
                        struct tm LoginTime = {0};
                        LoginTime.tm_year = atoi(date) - 1900;
                        LoginTime.tm_mon = atoi(date + 5) - 1;
                        LoginTime.tm_mday = atoi(date + 8);
                        LoginTime.tm_hour = atoi(date + 11);
                        LoginTime.tm_min = atoi(date + 14);
                        LoginTime.tm_sec = atoi(date + 17);

                        masterData.llLastLoginTime = mktime(&LoginTime);                   
                    }
                }
            }
            pResult->Release();
        }else
        {
		    CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_EveryDayLastLoginIn","_uid:%d",pTUE->dwUserID);
            G_DBError(m_dbConnector, m_dwThread);
            return CT_FALSE;
        }
    }

	if (iRet > 0)
	{
		MSC_DB_InsertMasterInfo InsertMaster={0};
		InsertMaster.dwUserID=pTUE->dwUserID;
		memcpy(&InsertMaster.MasterInfo,&masterData,sizeof(masterData));
		CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MASTERDATA,(CT_CHAR*)&InsertMaster,sizeof(InsertMaster));

		if (subID==MSC_SUB_DB_USRE_ENTER)
		{
			CCUser *pUser=0;
			if(CCUserManager::getSingleton().Find(pTUE->dwUserID,&pUser))
			{
				CCUserManager::getSingleton().InsertNickNameMap( masterData.cNickName, pUser);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetMasterInfo No Find UserID:%d",pTUE->dwUserID);
				return CT_FALSE;
			}
		}
			
	}else
	{	
		//异常情况，数据库数据无法匹配用户的id，说明用户没有在这组服务器上生成角色
		CCLogMeg::getSingleton().WriteError("CCDataBaseThread::GetMasterInfo(const CT_CHAR* pBuf) : Can't find %i masterData", pTUE->dwUserID);
		G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NO_USER, pTUE->dwUserID, pTUE->pNetInterface);
		return CT_FALSE;
	}

	return CT_TRUE;
}


//得到正在升级建筑的信息
CT_BOOL CCDBUserEnterEvent::GetUpgradeBuildingInfo(const MSC_DB_UserEnter*   pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetUpgradingBuildingData");

	m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
					MSC_DB_InsertUpGradeBuild InsertUpGradeBuild={0};
					pResult->GetValue(0, InsertUpGradeBuild.iBid );
					pResult->GetValue(1, InsertUpGradeBuild.llBeginTime);
					pResult->GetValue(2, InsertUpGradeBuild.iLasting);
					pResult->GetValue(3, InsertUpGradeBuild.iGrade);

					InsertUpGradeBuild.dwUserID= pTUE->dwUserID;
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_UPGRADEBUILD,(CT_CHAR*)&InsertUpGradeBuild,sizeof(InsertUpGradeBuild));
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetUpgradingBuildingData","_uid:%d",pTUE->dwUserID);
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//得到建筑信息
CT_BOOL CCDBUserEnterEvent::GetBuildingInfo(const MSC_DB_UserEnter*   pTUE)
{
	BuildingData  buildData;
	memset(&buildData, 0, sizeof(buildData));

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetBuildingtData");
	m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, buildData.iID );
				pResult->GetValue(1,  buildData.iGrade);

				MSC_DB_InsertBuildingInfo InsertBuildingInfo={0};
				InsertBuildingInfo.dwUserID= pTUE->dwUserID;
				memcpy(&InsertBuildingInfo.buildData,&buildData,sizeof(buildData));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BUILDINGINFO,(CT_CHAR*)&InsertBuildingInfo,sizeof(InsertBuildingInfo));
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetBuildingtData","_uid:%d",pTUE->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);;
		return CT_FALSE;
	}

	return CT_TRUE;
}

//得到正在生产的建筑
CT_BOOL CCDBUserEnterEvent::GetProductingBuilding(const MSC_DB_UserEnter*   pTUE)
{
	ProductingBuilding  data = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ProductingBuildingGet");

	m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				//pResult->GetValue(0, data.dwUserID );
				pResult->GetValue(1, data.iBType);
				pResult->GetValue(2, data.iBid);
				pResult->GetValue(3, data.llTime);
				pResult->GetValue(4, data.iType);

				pResult->GetValue(5, data.bSendCanHavest);
				pResult->GetValue(6, data.iLastingTime);
				pResult->GetValue(7, data.iWorker);
				pResult->GetValue(8, data.iBGrade);
				pResult->GetValue(9, data.fSeekAddition);
				//G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_BUILDING_PRODUCTING, (CT_CHAR*)&data, sizeof(data));
				
				MSC_DB_InsertProductingBuilding InsertProducting={0};
				InsertProducting.dwUserID=pTUE->dwUserID;
				memcpy(&InsertProducting.Producting,&data,sizeof(data));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PRODUCTING,(CT_CHAR*)&InsertProducting,sizeof(InsertProducting));
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_ProductingBuildingGet","_uid:%d",pTUE->dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到潜能数据
CT_BOOL CCDBUserEnterEvent::GetTalentInfo(const MSC_DB_UserEnter*   pTUE)
{
    CCUserTalent::DataMap dataMap;
    if (ReadDBData(pTUE->dwUserID, dataMap))
    {
        for (CCUserTalent::DataMap_IT it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            MSC_DB_InsertTalentInfo insertData={0};
            insertData.dwUserID=pTUE->dwUserID;
            insertData.talentData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TALENTINFO,(CT_CHAR*)&insertData,sizeof(insertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_GetTalent");
    
    m_dbConnector.AddParm("_uid", pTUE->dwUserID );
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();        
        if (pResult)
        {
            if (pResult->GetResult())
            {
				while (pResult->MoveNextRow())
				{
					TalentData talentData = {0};                
					pResult->GetValue(0, talentData.iID);
					pResult->GetValue(1, talentData.szName, sizeof(talentData.szName));
					talentData.iStatus = TALENT_ACTIVATED; // 标识已学会

					MSC_DB_InsertTalentInfo InsertTalent={0};
					InsertTalent.dwUserID=pTUE->dwUserID;
					memcpy(&InsertTalent.talentData,&talentData,sizeof(talentData));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TALENTINFO,(CT_CHAR*)&InsertTalent,sizeof(InsertTalent));
				}
            }

            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetTalent","_uid:%d",pTUE->dwUserID);
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

	return CT_TRUE;
#endif
}

// 得到技能数据
CT_BOOL CCDBUserEnterEvent::GetSkillInfo(const MSC_DB_UserEnter*   pTUE)
{
    CCUserSkill::DataMap dataMap;
    if (ReadDBData(pTUE->dwUserID, dataMap))
    {
        for (CCUserSkill::DataMap_IT it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            MSC_DB_InsertSkillData insertData = {0};
            insertData.dwUserID=pTUE->dwUserID;
            insertData.skillData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SKILLDATA,(CT_CHAR*)&insertData,sizeof(insertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_GetSkill");

    m_dbConnector.AddParm("_uid",pTUE->dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    SkillData skillData = {0};
                    pResult->GetValue(0, skillData.iID );
                    pResult->GetValue(1, skillData.szName, sizeof(skillData.szName));
                    pResult->GetValue(2, skillData.iSkillSoul);
                    pResult->GetValue(3, skillData.Used);
                   // G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_SKILL_INFO, (CT_CHAR*)&skillData, sizeof(skillData));
					
					MSC_DB_InsertSkillData InsertSkill={0};
					InsertSkill.dwUserID=pTUE->dwUserID;
					memcpy(&InsertSkill.skillData,&skillData,sizeof(skillData));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SKILLDATA,(CT_CHAR*)&InsertSkill,sizeof(InsertSkill));

				}
            }

            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetSkill","_uid:%d",pTUE->dwUserID);
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

	return CT_TRUE;
#endif
}

// 得到道具数据
CT_BOOL CCDBUserEnterEvent::GetItemInfo(const MSC_DB_UserEnter* pTUE)
{
    CCUserItem::DataMap dataMap;
    if (ReadDBData(pTUE->dwUserID, dataMap))
    {
        for (CCUserItem::DataMap_IT it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            MSC_DB_InsertItemInfo insertData = {0};
            insertData.dwUserID=pTUE->dwUserID;
            insertData.itemData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ITEMINFO,(CT_CHAR*)&insertData,sizeof(insertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_PropGet");

    m_dbConnector.AddParm("_uid",pTUE->dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    ItemData itemData = {0};
                    pResult->GetValue(0, itemData.key);
                    pResult->GetValue(1, itemData.iID);
                    pResult->GetValue(2, itemData.szName, sizeof(itemData.szName));
                    pResult->GetValue(3, itemData.iCount);
                    pResult->GetValue(4, itemData.iIndex);

                    //G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_ITEM_INFO, (CT_CHAR*)&itemData, sizeof(itemData));

					MSC_DB_InsertItemInfo InsertItemInfo={0};
					InsertItemInfo.dwUserID=pTUE->dwUserID;
					memcpy(&InsertItemInfo.itemData,&itemData,sizeof(itemData));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ITEMINFO,(CT_CHAR*)&InsertItemInfo,sizeof(InsertItemInfo));
				}
            }

            pResult->Release();
        }
    }
    else
    {
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_PropGet","_uid:%d",pTUE->dwUserID);
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

	return CT_TRUE;
#endif
}
// 得到装备信息
CT_BOOL CCDBUserEnterEvent::GetEquipInfo(const MSC_DB_UserEnter* pTUE)
{
    CCUserEquipment::DataMap dataMap;
    if (ReadDBData(pTUE->dwUserID, dataMap))
    {
        for (CCUserEquipment::DataMap_IT it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            MSC_DB_InsertEquipInfo insertData = {0};
            insertData.dwUserID=pTUE->dwUserID;
            insertData.equipData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_EQUIPINFO,(CT_CHAR*)&insertData,sizeof(insertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
    // 强化与插孔
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_PropGetStrengthen");
    
    m_dbConnector.AddParm("_uid", pTUE->dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    CT_INT32 iID = 0;
                    EquipmentData equipData = {0};
                    pResult->GetValue(1, equipData.key);                    
                    pResult->GetValue(2, iID);
                    pResult->GetValue(3, equipData.iLevel);
                    pResult->GetValue(4, equipData.nSlot);

                    //G_SendThreadInfo(pTUE->dwThreadID, mainDB, MSC_SUB_DB_R_SMITHING_AND_SLOT, (CT_CHAR*)&equipData, sizeof(equipData));
					
					MSC_DB_InsertEquipInfo InsertEquip={0};
					InsertEquip.dwUserID=pTUE->dwUserID;
					memcpy(&InsertEquip.equipData,&equipData,sizeof(equipData));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_EQUIPINFO,(CT_CHAR*)&InsertEquip,sizeof(InsertEquip));					
				}
            }

            pResult->Release();
        }
    }
    else
    {
       // G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

    // 洗炼信息
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_PropGetTrain");

    m_dbConnector.AddParm("_uid",pTUE->dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                CT_INT32 attrs[] = FORGE_ATTR_ARRAY;
                while (pResult->MoveNextRow())
                {
                    CT_INT32 n = 0;
                    CT_INT32 iID = 0;
                    EquipmentData equipData = {0};
                    n++;
                    pResult->GetValue(n++, equipData.key);
                    for (int i = 0; i < sizeof(attrs) / sizeof(*attrs); ++i)
                    {
                        CT_FLOAT fValue = 0.0f;
                        pResult->GetValue(n++, fValue); 
                        if (fValue > 0.000001)  // not zero
                        {
                            for (int j = 0; j < FORGE_ATTRS; ++j)
                            {
                                if (equipData.Forge[j].iAID == 0)
                                {
                                    equipData.Forge[j].iAID = attrs[i];
                                    equipData.Forge[j].fValue = fValue;
                                    break;
                                }
                            }
                        }
                    }
                   // G_SendThreadInfo(pTUE->dwThreadID, mainDB, MSC_SUB_DB_R_FORGE_INFO, (CT_CHAR*)&equipData, sizeof(equipData));
					
					MSC_DB_InsertEquipInfo InsertEquip={0};
					InsertEquip.dwUserID=pTUE->dwUserID;
					memcpy(&InsertEquip.equipData,&equipData,sizeof(equipData));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_EQUIPINFO,(CT_CHAR*)&InsertEquip,sizeof(InsertEquip));					
                }
            }

            pResult->Release();
        }
    }
    else
    {
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

    // 镶嵌信息
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_PropGetInlay");

    m_dbConnector.AddParm("_uid",pTUE->dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    CT_INT32 n = 0;
                    CT_INT32 iSlot = 0;
                    CT_INT32 iGemID = 0;
                    EquipmentData equipData = {0};
                    n++;
                    pResult->GetValue(n++, equipData.key);
                    pResult->GetValue(n++, iGemID);
                    pResult->GetValue(n++, iSlot);
                    equipData.Gem[iSlot] = iGemID;
                    					
					MSC_DB_InsertEquipInfo InsertEquip={0};
					InsertEquip.dwUserID=pTUE->dwUserID;
					memcpy(&InsertEquip.equipData,&equipData,sizeof(equipData));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_EQUIPINFO,(CT_CHAR*)&InsertEquip,sizeof(InsertEquip));					
				}
            }
            pResult->Release();
        }
    }
    else
    {
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }
#endif
	return CT_TRUE;
}

// 得到增益数据
CT_BOOL CCDBUserEnterEvent::GetBuffInfo(const MSC_DB_UserEnter* pTUE)
{
    CCUserBuff::DataMap dataMap;
    if (ReadDBData(pTUE->dwUserID, dataMap))
    {
        for (CCUserBuff::DataMap_IT it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            MSC_DB_InsertBuffInfo insertData = {0};
            insertData.dwUserID=pTUE->dwUserID;
            insertData.buffData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BUFFINFO,(CT_CHAR*)&insertData,sizeof(insertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_PropGetUse");

    m_dbConnector.AddParm("_uid", pTUE->dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    BuffData buffData = {0};
                    pResult->GetValue(0, buffData.iID);
                    pResult->GetValue(1, buffData.toggleTime);
                    pResult->GetValue(2, buffData.iDuration);
                    
                    time_t currTime = time(0);
                    if (currTime < buffData.toggleTime + buffData.iDuration)
                    {
						MSC_DB_InsertBuffInfo InsertBuffer={0};
						InsertBuffer.dwUserID=pTUE->dwUserID;
                        InsertBuffer.buffData = buffData;
						CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
						G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BUFFINFO,(CT_CHAR*)&InsertBuffer,sizeof(InsertBuffer));					
					}
                }
            }
            pResult->Release();
        }
    }
    else
    {
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }
	return CT_TRUE;
#endif
}

// 得到冷却信息
CT_BOOL CCDBUserEnterEvent::GetCooldownInfo(const MSC_DB_UserEnter *pTUE)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_PropGetCool");

    m_dbConnector.AddParm("_uid", pTUE->dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    CooldownData cdData = {0};
                   // pResult->GetValue(0, cdData.dwUserID);
                    pResult->GetValue(1, cdData.iType);                    
                    pResult->GetValue(2, cdData.leftTime);
                    
					MSC_DB_InsertCooldownInfo InsertCoolDown={0};
					InsertCoolDown.dwUserID=pTUE->dwUserID;
                    InsertCoolDown.cdData = cdData;
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_COOLDOWNINFO,(CT_CHAR*)&InsertCoolDown,sizeof(InsertCoolDown));					
              }
            }

            pResult->Release();
        }
    }
    else
    {
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

	return CT_TRUE;
}

// 得到世界场景数据
CT_BOOL CCDBUserEnterEvent::GetWorldSceneInfo(const MSC_DB_UserEnter*   pTUE)
{
	MSC_DB_WorldSceneInfo  data = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorldGetArea");

	m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, data.dwUserID );
				pResult->GetValue(1, data.iSceneID);
				pResult->GetValue(2, data.iReward);
				//G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_DUPLICATE_WORLD_SCENE, (CT_CHAR*)&data, sizeof(data));
				
				MSC_DB_InsertWorldScene InsertWorldScene={0};
				InsertWorldScene.dwUserID=pTUE->dwUserID;
				memcpy(&InsertWorldScene.data,&data,sizeof(data));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_WORLDSCENE,(CT_CHAR*)&InsertWorldScene,sizeof(InsertWorldScene));					
			}
		}

		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到场景数据
CT_BOOL CCDBUserEnterEvent::GetSceneInfo(const MSC_DB_UserEnter*   pTUE)
{
    //得到购买战斗次数
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_WorldFightTimeGet");

    m_dbConnector.AddParm("_uid", pTUE->dwUserID );
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult->GetResult())
        {  
            while (pResult->MoveNextRow())
            {
                MSC_DB_InsertBuyFightingTime data={0};
                pResult->GetValue(0, data.dwUserID );
                pResult->GetValue(1, data.iBuyFightingTime);

                CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
                G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BUYFIGHTINGTIME,(CT_CHAR*)&data,sizeof(data));					
            }
        }

        pResult->Release();
    }else
    {
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

	SceneInfo  data = {0};
	for(CT_INT32 i=0;i<WORLD_SCENE_COUNT;++i)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_WorldGetFight");

		m_dbConnector.AddParm("_uid", pTUE->dwUserID );
		m_dbConnector.AddParm("_aid",  i);
		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					//pResult->GetValue(0, data.dwUserID );
					pResult->GetValue(1, data.iWorldSceneID);
					pResult->GetValue(2, data.iSceneID);
					pResult->GetValue(3, data.iFightingNum);
					pResult->GetValue(4, data.iDifficulty);
					pResult->GetValue(5, data.iSceneStar);

					MSC_DB_InsertSceneInfo InsertScnen={0};
					InsertScnen.dwUserID=pTUE->dwUserID;
					memcpy(&InsertScnen.data,&data,sizeof(data));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SCENEINFO,(CT_CHAR*)&InsertScnen,sizeof(InsertScnen));					
				}
			}

			pResult->Release();
		}else
		{
			//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
			G_DBError(m_dbConnector, m_dwThread);
			return CT_FALSE;
		}
	}

	return CT_TRUE;
}

//得到军队信息
CT_BOOL	CCDBUserEnterEvent::GetArmyInfo(const MSC_DB_UserEnter*   pTUE)
{
    CCUserArmy::DataMap dataMap;
    if (ReadDBData(pTUE->dwUserID, dataMap))
    {
        for (CCUserArmy::DataMap_IT it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            MSC_DB_InsertArmyInfo insertData = {0};
            insertData.dwUserID=pTUE->dwUserID;
            insertData.Armydata = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ARMYINFO,(CT_CHAR*)&insertData,sizeof(insertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
	MSC_DB_ArmyInfo  data = {0};
	for(CT_INT32 i=0;i<WORLD_SCENE_COUNT;++i)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_ArmyGet");

		m_dbConnector.AddParm("_uid", pTUE->dwUserID );
		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				CT_DWORD dwValue=0;
				while (pResult->MoveNextRow())
				{
					pResult->GetValue(0, dwValue );
					if(dwValue != 0)
					{
						pResult->GetValue(0, data.dwUserID );
						pResult->GetValue(1, data.iArmyID);
						pResult->GetValue(2, data.iArmyGrade);
						//G_SendThreadInfo(pTUE->dwThreadID, mainDB, MSC_SUB_DB_R_ARMY_INFO, (CT_CHAR*)&data, sizeof(data));

						MSC_DB_InsertArmyInfo InsertArmy={0};
						InsertArmy.dwUserID=pTUE->dwUserID;
						memcpy(&InsertArmy.Armydata,&data,sizeof(data));
						CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
						G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ARMYINFO,(CT_CHAR*)&InsertArmy,sizeof(InsertArmy));					
					}

				}
			}

			pResult->Release();
		}else
		{
			//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
			G_DBError(m_dbConnector, m_dwThread);
			return CT_FALSE;
		}
	}
	return CT_TRUE;
#endif
}

//得到阵法信息
CT_BOOL	CCDBUserEnterEvent::GetBattleArray(const MSC_DB_UserEnter*   pTUE)
{
	MSC_DB_BattleArray  data = {0};
    if (ReadDBData(pTUE->dwUserID, data))
    {
        MSC_DB_InsertBattleArray InsertBattleArray={0};
        InsertBattleArray.dwUserID=pTUE->dwUserID;
        memcpy(&InsertBattleArray.data,&data,sizeof(data));
        CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
        G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BATTLEARRAY,(CT_CHAR*)&InsertBattleArray,sizeof(InsertBattleArray));
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyGetPos");

	m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_DWORD dwValue=0;
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dwValue );
				if(dwValue != 0)
				{
					pResult->GetValue(0, data.dwUserID );
					pResult->GetValue(1, data.iPosition[0]);
					pResult->GetValue(2, data.iPosition[1]);
					pResult->GetValue(3, data.iPosition[2]);
					pResult->GetValue(4, data.iPosition[3]);
					pResult->GetValue(5, data.iPosition[4]);
					//G_SendThreadInfo(pTUE->dwThreadID, mainDB, MSC_SUB_DB_BATTLE_ARRAY_INFO, (CT_CHAR*)&data, sizeof(data));
					
					MSC_DB_InsertBattleArray InsertBattleArray={0};
					InsertBattleArray.dwUserID=pTUE->dwUserID;
					memcpy(&InsertBattleArray.data,&data,sizeof(data));
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BATTLEARRAY,(CT_CHAR*)&InsertBattleArray,sizeof(InsertBattleArray));					
				}

			}
		}

		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
#endif
}

//竞技场信息
CT_BOOL CCDBUserEnterEvent::GetPVPInfo(const  MSC_DB_UserEnter* pTUE)
{
	MSC_DB_InsertPVPInfo InsertPVPInfo={0};
	InsertPVPInfo.dwUserID=pTUE->dwUserID;
	InsertPVPInfo.pNetInterface=pTUE->pNetInterface;
	InsertPVPInfo.dwThreadID=pTUE->dwThreadID;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPGetChallenge");
	m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertPVPInfo.PVPdata.dwUserID);
				pResult->GetValue(1, InsertPVPInfo.PVPdata.lRefreshTime);
				pResult->GetValue(2, InsertPVPInfo.PVPdata.cbChestStatus);
				pResult->GetValue(3, InsertPVPInfo.PVPdata.iPVPDegree);
				pResult->GetValue(4, InsertPVPInfo.PVPdata.iFightingCount);
				pResult->GetValue(5, InsertPVPInfo.PVPdata.iPVPChestNum);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	//对手信息
	iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPGetChallengeData");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iCount=0;
			while (pResult->MoveNextRow())
			{
				CT_INT32 iHeight=0;
				pResult->GetValue(0, iCount);

				//记录为空
				if(iCount == 0)
				{
					MSC_DB_R_BaseInfo ui={0};
					ui.dwUserID = pTUE->dwUserID;
					ui.pNetInterface = pTUE->pNetInterface;

					G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_ENTER_FINISH, 
						(CT_CHAR*)&ui, sizeof(ui));

					pResult->Release();
					return CT_TRUE; 
				}

				pResult->GetValue(1, InsertPVPInfo.PVPdata.dwUserID);
				pResult->GetValue(10, iHeight);
				if(iHeight == LOW)
				{
					pResult->GetValue(2, InsertPVPInfo.PVPdata.dwChallengUserID[0]);
					pResult->GetValue(3, InsertPVPInfo.PVPdata.dwChallengUserID[1]);
					pResult->GetValue(4, InsertPVPInfo.PVPdata.dwChallengUserID[2]);
					pResult->GetValue(5, InsertPVPInfo.PVPdata.dwChallengUserID[3]);
					pResult->GetValue(6, InsertPVPInfo.PVPdata.bChallengUser[0]);
					pResult->GetValue(7, InsertPVPInfo.PVPdata.bChallengUser[1]);
					pResult->GetValue(8, InsertPVPInfo.PVPdata.bChallengUser[2]);
					pResult->GetValue(9, InsertPVPInfo.PVPdata.bChallengUser[3]);
				}
				else
				{
					pResult->GetValue(2, InsertPVPInfo.PVPdata.dwChallengUserID[4]);
					pResult->GetValue(3, InsertPVPInfo.PVPdata.dwChallengUserID[5]);
					pResult->GetValue(4, InsertPVPInfo.PVPdata.dwChallengUserID[6]);
					pResult->GetValue(5, InsertPVPInfo.PVPdata.dwChallengUserID[7]);
					pResult->GetValue(6, InsertPVPInfo.PVPdata.bChallengUser[4]);
					pResult->GetValue(7, InsertPVPInfo.PVPdata.bChallengUser[5]);
					pResult->GetValue(8, InsertPVPInfo.PVPdata.bChallengUser[6]);
					pResult->GetValue(9, InsertPVPInfo.PVPdata.bChallengUser[7]);
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	//获取用户信息
	GetNoLinkUserInfo(InsertPVPInfo.PVPdata.dwChallengUserID,InsertPVPInfo.UserFightinginfo,InsertPVPInfo.LookUserinfo,PVP_CHALLENGE_COUNT);
	CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PVPINFO,(CT_CHAR*)&InsertPVPInfo,sizeof(InsertPVPInfo));

	return CT_TRUE;
}

//竞技场战报
CT_BOOL CCDBUserEnterEvent::GetPVPRecord(const  MSC_DB_UserEnter* pTUE)
{

	//读取记录
	//const CT_INT32 iLen=PVP_FIGHTING_COUNT*sizeof(MSC_DB_FighitngRecord);
	//CT_CHAR cBuf[iLen]={0};
	//CT_DWORD dwChallengID[PVP_FIGHTING_COUNT]={0};
	CT_INT32 iCount=0;
	CT_CHAR cIndex[PVP_FIGHTING_COUNT][PVPRECORD_INDEX]={0};
	MSC_DB_InsertPVPRecord PVPRecord={0};

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PVPGetFightAward");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, cIndex[iCount],PVPRECORD_INDEX);
				pResult->GetValue(1, PVPRecord.FightingRecord[iCount].PVPFightinginfo.dwChallengUserID);
				pResult->GetValue(2, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iGold);
				pResult->GetValue(3, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iPrestige);
				pResult->GetValue(4, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iGoods[0]);
				pResult->GetValue(5, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iCount[0]);
				pResult->GetValue(6, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iGoods[1]);
				pResult->GetValue(7, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iCount[1]);
				pResult->GetValue(8, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iGoods[2]);
				pResult->GetValue(9, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iCount[2]);
				pResult->GetValue(10, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iGoods[3]);
				pResult->GetValue(11, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iCount[3]);
				pResult->GetValue(12, PVPRecord.FightingRecord[iCount].PVPFightinginfo.iWiner);
				pResult->GetValue(13, PVPRecord.FightingRecord[iCount].PVPFightinginfo.dwUserID);

				PVPRecord.FightingRecord[iCount].dwUserID=PVPRecord.FightingRecord[iCount].PVPFightinginfo.dwChallengUserID;
				if(PVPRecord.FightingRecord[iCount].PVPFightinginfo.dwUserID == pTUE->dwUserID)
				{
					PVPRecord.FightingRecord[iCount].PVPFightinginfo.dwUserID=PVPRecord.FightingRecord[iCount].dwUserID;
					PVPRecord.FightingRecord[iCount].dwUserID=pTUE->dwUserID;

					if(PVPRecord.FightingRecord[iCount].PVPFightinginfo.iWiner == WINER_LIFT)
					{
						PVPRecord.FightingRecord[iCount].PVPFightinginfo.iWiner=WINER_RIGHT;
					}
					else if(PVPRecord.FightingRecord[iCount].PVPFightinginfo.iWiner == WINER_RIGHT)
					{
						PVPRecord.FightingRecord[iCount].PVPFightinginfo.iWiner=WINER_LIFT;
					}
				}

				if(iCount < PVP_FIGHTING_COUNT-1)
					iCount++;

			}
			pResult->Release();
		}else
		{
			CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_PVPGetFightRecord","_uid:%d",pTUE->dwUserID);
			G_DBError(m_dbConnector, m_dwThread);
			return CT_FALSE;
		}
	}


	for(CT_INT32 i=0;i<iCount;++i)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_PVPGetFightExample");
		m_dbConnector.AddParm("_FightIndex", cIndex[i]);

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				CT_INT32 iRival=0;
				while (pResult->MoveNextRow())
				{
					CT_INT32 iIndex=0;
					CT_DWORD dwUserID=0;
					CT_INT32 iScore=0;
					CombatUnitData data={0};

					//pResult->GetValue(iIndex++, cIndex  );
					iIndex++;
					//pResult->GetValue(iIndex++, dwUserID  );
					//pResult->GetValue(iIndex++, data.name,NAME_SIZE);
					//pResult->GetValue(iIndex++, data.mcs.fHPPersent );
					//pResult->GetValue(iIndex++, data.mcs.fDmgIncPercent );
					//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
					//pResult->GetValue(iIndex++, PVPRecord.FightingInfo[i].iSeed );
					//pResult->GetValue(iIndex++, data.level);
					//pResult->GetValue(iIndex++, data.type);
					//iIndex++;
					////pResult->GetValue(10, PVPRecord.FightingInfo[iCount].iSceneStar );
					//pResult->GetValue(iIndex++, data.mcs.iMaxHP );
					//pResult->GetValue(iIndex++, data.mcs.iMaxMP );
					//pResult->GetValue(iIndex++, data.mcs.iStrength );
					//pResult->GetValue(iIndex++, data.mcs.iAgility );
					//pResult->GetValue(iIndex++, data.mcs.iIntellect);
					//pResult->GetValue(iIndex++, data.mcs.iSpirit );
					//pResult->GetValue(iIndex++, data.mcs.fHitRate );
					//pResult->GetValue(iIndex++, data.mcs.fDodgeRate );
					//pResult->GetValue(iIndex++, data.mcs.fCritRate );
					//pResult->GetValue(iIndex++, data.mcs.fResilient );
					//iIndex++;
					//iIndex++;
					////pResult->GetValue(21, 0 );
					////pResult->GetValue(22, 0 );
					//pResult->GetValue(iIndex++, data.mcs.iAttack );
					//pResult->GetValue(iIndex++, data.mcs.iDefence );
					//pResult->GetValue(iIndex++, data.position);
					//pResult->GetValue(iIndex++, data.Skills[0].iID );
					//pResult->GetValue(iIndex++, data.Skills[1].iID );
					//pResult->GetValue(iIndex++, data.Skills[2].iID );
					//pResult->GetValue(iIndex++, data.Skills[3].iID );
					//pResult->GetValue(iIndex++, data.mcs.iHPRecover );
					//pResult->GetValue(iIndex++, data.mcs.iMPRecover );

					G_GetFightingRecord(pResult,data,iScore,PVPRecord.FightingInfo[i].iSeed,iIndex,dwUserID);
					pResult->GetValue(iIndex++, PVPRecord.FightingInfo[i].iSceneID );

					if(PVPRecord.FightingRecord[i].PVPFightinginfo.dwChallengUserID == dwUserID)
					{
						iRival=0;
					}
					else
					{
						iRival=1;
					}
					PVPRecord.FightingInfo[i].FightingData[iRival].dwUserID=dwUserID;
					PVPRecord.FightingInfo[i].FightingData[iRival].units[data.position%(POST_COUNT/2)]=data;
				}
			}
			pResult->Release();
		}else
		{
			CCDBLogMes::getSingleton().WriteInfo(PVPRecord.FightingRecord[iCount].dwUserID,"SP_PVPGetFightAwardRecord","_FightIndex:%s",cIndex[i]);
			G_DBError(m_dbConnector, m_dwThread);
			return CT_FALSE;
		}
	}

	for(CT_INT32 i=0;i<PVP_FIGHTING_COUNT;++i)
	{
		for(CT_INT32 j=0;j<PVP_USER_COUNT;++j)
		{
			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_PVPGetFightSkill");
			m_dbConnector.AddParm("_index", cIndex[i] );
			m_dbConnector.AddParm("_uid", PVPRecord.FightingInfo[i].FightingData[j].dwUserID );

			if(m_dbConnector.Exec())
			{
				CT_INT32 iNum=0;
				CT_INT32 iPos1=0;
				CCDBResult* pResult = m_dbConnector.GetResult();
				if (pResult->GetResult())
				{  
					while (pResult->MoveNextRow())
					{
						CT_INT32 iSkillID=0;
						CT_INT32 iPos2=0;
						pResult->GetValue(0,iSkillID);
						pResult->GetValue(1,iPos2);
						pResult->GetValue(2,iPos2);
						if(iPos1 != iPos2)
						{
							iNum=0;
							iPos1=iPos2;
						}
						PVPRecord.FightingInfo[i].FightingData[j].units[iPos2%(POST_COUNT/2)].SkillEnuip.iSkillEquip[iNum%SKILL_ENUIP_COUNT]=iSkillID;
						iNum++;

					}
				}
				pResult->Release();
			}else
			{
				G_DBError(m_dbConnector, m_dwThread);
				return CT_FALSE;
			}
		}
	}

	for(CT_INT32 i=0;i<PVP_FIGHTING_COUNT;++i)
	{
		if(PVPRecord.FightingInfo[i].FightingData[0].dwUserID == pTUE->dwUserID)
		{
			CombatUnitData data={0};
			G_GetMasterFightingData(PVPRecord.FightingInfo[i].FightingData[1],data);
			memcpy(PVPRecord.FightingInfo[i].LookData.cUserName,data.name,NAME_SIZE);
		}
		else
		{
			CombatUnitData data={0};
			G_GetMasterFightingData(PVPRecord.FightingInfo[i].FightingData[0],data);
			memcpy(PVPRecord.FightingInfo[i].LookData.cUserName,data.name,NAME_SIZE);
		}
	}


	PVPRecord.dwUserID=pTUE->dwUserID;
	CT_DWORD threadid = GAME_THREAD_CITY + PVPRecord.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;;
	G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PVPRECORD,(CT_CHAR*)&PVPRecord,sizeof(PVPRecord));

	return CT_TRUE;
}
// End [10/22/2012 wangchen]
//得到用户解锁区域
CT_BOOL CCDBUserEnterEvent::GetRegionInfo(const MSC_DB_UserEnter* pTUE)
{
	MSC_DB_R_UnlockRegion  unlockRegion;
	memset(&unlockRegion, 0, sizeof(unlockRegion));
	unlockRegion.dwUserID = pTUE->dwUserID;

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MasterGetArea");
	m_dbConnector.AddParm("_uid",  pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, unlockRegion.iRegionID );
				//G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_REGION_INFO, 
				//	(CT_CHAR*)&unlockRegion, sizeof(unlockRegion));

				MSC_DB_InsertRegionInfo InsertRrgion={0};
				InsertRrgion.dwUserID=pTUE->dwUserID;
				memcpy(&InsertRrgion.unlockRegion,&unlockRegion,sizeof(unlockRegion));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_REGIONINFO,(CT_CHAR*)&InsertRrgion,sizeof(InsertRrgion));					
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//得到用户已完成任务
CT_BOOL CCDBUserEnterEvent::GetTaskFinishedData(const MSC_DB_UserEnter* pTUE)
{
	MSC_DB_TaskFinishData taskFinishData = {0};
	taskFinishData.dwUserID = pTUE->dwUserID;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TaskGetFinish");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, taskFinishData.iTaskID );
				//G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_TASK_FINISH, 
				//	(CT_CHAR*)&taskFinishData, sizeof(MSC_DB_TaskFinishData));

				MSC_DB_InsertTaskFinish InsertTaskFinish={0};
				InsertTaskFinish.dwUserID=pTUE->dwUserID;
				memcpy(&InsertTaskFinish.taskFinishData,&taskFinishData,sizeof(taskFinishData));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TASKFINISH,(CT_CHAR*)&InsertTaskFinish,sizeof(InsertTaskFinish));					

			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}
//得到用户在进行任务
CT_BOOL CCDBUserEnterEvent::GetTaskProgressData(const MSC_DB_UserEnter* pTUE)
{
	MSC_DB_TaskProgressData taskPData = {0};
	taskPData.dwUserID = pTUE->dwUserID;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TaskGetLasting");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, taskPData.iTaskID );
				pResult->GetValue(2, taskPData.iSubTaskID );
				pResult->GetValue(3, taskPData.iTime );
				pResult->GetValue(4, taskPData.iCompleted );
			//	G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_TASK_PROGRESS, 
			//		(CT_CHAR*)&taskPData, sizeof(MSC_DB_TaskProgressData));

				MSC_DB_InsertProgressData InsertTaskPData={0};
				InsertTaskPData.dwUserID=pTUE->dwUserID;
				memcpy(&InsertTaskPData.taskPData,&taskPData,sizeof(taskPData));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TASKPORGRESS,(CT_CHAR*)&InsertTaskPData,sizeof(InsertTaskPData));					
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到日常任务进度信息
CT_BOOL CCDBUserEnterEvent::GetDailyQuestInfo(const MSC_DB_UserEnter* pTUE)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayGetTask");
    m_dbConnector.AddParm("_uid", pTUE->dwUserID);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult->GetResult())
        {  
            while (pResult->MoveNextRow())
            {
                DailyQuestData stInfo = {0};
            
                pResult->GetValue(1, stInfo.iQuestID);
                pResult->GetValue(2, stInfo.iCurrent);
               // G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_DAILY_QUEST, 
                //    (CT_CHAR*)&stInfo, sizeof(stInfo));

				MSC_DB_InsertDailyQuestInfo InsertDailyQuest={0};
				InsertDailyQuest.dwUserID=pTUE->dwUserID;
				memcpy(&InsertDailyQuest.stInfo,&stInfo,sizeof(stInfo));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_DAILYQUEST,(CT_CHAR*)&InsertDailyQuest,sizeof(InsertDailyQuest));					
            }
        }
        pResult->Release();
    }
    else
    {
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

	return CT_TRUE;
}

// 得到签到信息
CT_BOOL CCDBUserEnterEvent::GetDailySignInfo(const  MSC_DB_UserEnter* pTUE)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayGetLoginIn");
    m_dbConnector.AddParm("_uid",  pTUE->dwUserID);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult->GetResult())
        {  
            while (pResult->MoveNextRow())
            {
                SignData stInfo = {0};

                pResult->GetValue(1, stInfo.nSigned);
                pResult->GetValue(2, stInfo.bReward);
                CT_CHAR date[32];
                pResult->GetValue(3, date, sizeof(date)); // yyyy-mm-dd hh:mm:ss
                stInfo.llLastSignIn = G_StrTimeToSeconds(date); 
                
                if (stInfo.nSigned > MAX_SIGN_TIMES) 
                {
                    stInfo.nSigned = MAX_SIGN_TIMES;
                }
              //  G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_SIGN_INFO, 
              //      (CT_CHAR*)&stInfo, sizeof(stInfo));

				MSC_DB_InsertDailySignInfo InsertDailySignInfo={0};
				InsertDailySignInfo.dwUserID=pTUE->dwUserID;
				memcpy(&InsertDailySignInfo.stInfo,&stInfo,sizeof(stInfo));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SIGNINFO,(CT_CHAR*)&InsertDailySignInfo,sizeof(InsertDailySignInfo));					
            }
        }
        pResult->Release();
    }
    else
    {
       // G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }
	return CT_TRUE;
}

// 得到活跃度信息
CT_BOOL CCDBUserEnterEvent::GetDailyActiveInfo(const  MSC_DB_UserEnter* pTUE)
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayGetVitality");
    m_dbConnector.AddParm("_uid",  pTUE->dwUserID );

    ActiveData stInfo = {0};

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult->GetResult())
        {  
            while (pResult->MoveNextRow())
            {               
                pResult->GetValue(1, stInfo.iLevel);
                pResult->GetValue(2, stInfo.iActive);
            }
        }
        pResult->Release();
    }
    else
    {
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_EveryDayGetVitalityAwardToday");
    m_dbConnector.AddParm("_uid",  pTUE->dwUserID);

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult->GetResult())
        {  
            while (pResult->MoveNextRow())
            {               
                CT_INT32 iGrade = 0;
                pResult->GetValue(1, iGrade);
                stInfo.bReward[iGrade - 1] = CT_TRUE;
            }
        }
        pResult->Release();
    }
    else
    {
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

   // G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_ACTIVE_INFO, 
    //    (CT_CHAR*)&stInfo, sizeof(stInfo));

	MSC_DB_InsertDailyActive InsertDailyActive={0};
	InsertDailyActive.dwUserID=pTUE->dwUserID;
	memcpy(&InsertDailyActive.stInfo,&stInfo,sizeof(stInfo));
	CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_DAILYACTIVE,(CT_CHAR*)&InsertDailyActive,sizeof(InsertDailyActive));					

	return CT_TRUE;
}

//得到商店状态
CT_BOOL	CCDBUserEnterEvent::GetMarketFunctionStatus(const MSC_DB_UserEnter* pTUE)
{
	MSC_DB_MarketFunction marketFun = {0};
	marketFun.dwUserID = pTUE->dwUserID;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ShopGetStatus");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, marketFun.iMall );
				pResult->GetValue(2, marketFun.iBlackMarket );
				//	G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_TASK_PROGRESS, 
				//		(CT_CHAR*)&taskPData, sizeof(MSC_DB_TaskProgressData));
				
				MSC_DB_InsertMarketFunction InsertMarketFunction={0};
				InsertMarketFunction.dwUserID=pTUE->dwUserID;
				memcpy(&InsertMarketFunction.marketFun,&marketFun,sizeof(marketFun));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MARKETFUNCTION,(CT_CHAR*)&InsertMarketFunction,sizeof(InsertMarketFunction));					
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}	

//获取商店个人限购
CT_BOOL CCDBUserEnterEvent::GetMarketPersonal(const  MSC_DB_UserEnter* pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MarketPersonalGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				MSC_DB_InsertMarketPersonal InsertMarketPersonal={0};
				pResult->GetValue(0, InsertMarketPersonal.dwUserID );
				pResult->GetValue(1, InsertMarketPersonal.iGoods );
				pResult->GetValue(2, InsertMarketPersonal. iNum);

				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MARKETPERSONAL,(CT_CHAR*)&InsertMarketPersonal,sizeof(InsertMarketPersonal));					
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//获取黑店信息
CT_BOOL CCDBUserEnterEvent::GetBlackMarketInfo(const MSC_DB_UserEnter* pTUE)
{

	MSC_DB_BlackMarketRefresh blackMarket = {0};
	blackMarket.dwUserID = pTUE->dwUserID;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ShopGetBlackMarket");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, blackMarket.iTime );
				pResult->GetValue(2, blackMarket.iGoodsID1 );
				pResult->GetValue(3, blackMarket.iNum1 );
				pResult->GetValue(4, blackMarket.iGoodsID2 );
				pResult->GetValue(5, blackMarket.iNum2 );
				pResult->GetValue(6, blackMarket.iGoodsID3 );
				pResult->GetValue(7, blackMarket.iNum3 );
				pResult->GetValue(8, blackMarket.iGoodsID4 );
				pResult->GetValue(9, blackMarket.iNum4 );
				pResult->GetValue(10, blackMarket.iGoodsID5 );
				pResult->GetValue(11, blackMarket.iNum5 );
				pResult->GetValue(12, blackMarket.iGoodsID6 );
				pResult->GetValue(13, blackMarket.iNum6 );
				pResult->GetValue(14, blackMarket.iLastRefTime );
				//	G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_TASK_PROGRESS, 
				//		(CT_CHAR*)&taskPData, sizeof(MSC_DB_TaskProgressData));
				
				MSC_DB_InsertBlackMarketInfo InsertBlackMarket={0};
				InsertBlackMarket.dwUserID=pTUE->dwUserID;
				memcpy(&InsertBlackMarket.blackMarket,&blackMarket,sizeof(blackMarket));
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BLACKMARKETINFO,(CT_CHAR*)&InsertBlackMarket,sizeof(InsertBlackMarket));								}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

//得到宠物信息
CT_BOOL CCDBUserEnterEvent::GetPetInfo(const MSC_DB_UserEnter* pTUE)
{
	PetInfo pi={0}; 
    if (ReadDBData(pTUE->dwUserID, pi))
    {
		MSC_DB_InsertPetInfo InsertData={0};
		memcpy(&InsertData.pi,&pi,sizeof(pi));
		InsertData.dwUserID=pTUE->dwUserID;

		CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PETINFO,(CT_CHAR*)&InsertData,sizeof(InsertData));

		CCUserPet::PetChangeMap pcMap;
		if(ReadDBData(pTUE->dwUserID,pcMap))
		{
			for(CCUserPet::PetChangeMap::iterator it=pcMap.begin();it != pcMap.end();++it)
			{
				MSC_DB_InsertPetChange ipc={0};
				ipc.dwUserID=pTUE->dwUserID;
				ipc.iChangeID=(*it).second.iChangeID;
				ipc.iSkillID=(*it).second.iSkillID;

				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PETCHANGE,(CT_CHAR*)&ipc,sizeof(ipc));
			}
		}

        return CT_TRUE;
    }
    return CT_FALSE;

}

CT_BOOL CCDBUserEnterEvent::GetEmailInfo(const MSC_DB_UserEnter* pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EmailGetGmSend");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	
	CT_INT32 iIndex = 0;

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, iIndex);
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	std::list<CMD_G_EmailGM>								GM_list;   //GM邮件
	typedef std::list<CMD_G_EmailGM>::iterator				GM_list_it;

	std::list<CMD_G_EmailUserSendBack>						User_list; //用户邮件
	typedef std::list<CMD_G_EmailUserSendBack>::iterator	User_list_it;

	//读取GM邮件
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EmailGetGmData");
	m_dbConnector.AddParm("_index", iIndex);

	CT_INT32  iGMIndex = 0,iFlag=0;

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CMD_G_EmailGM  EmailGM = {0};
				if (iFlag==0)
				{
					pResult->GetValue(0, iGMIndex);
					iFlag=1;
				}
				pResult->GetValue(1, EmailGM.cEmailTitle,EMAIL_TITLE_SIZE);
				pResult->GetValue(2, EmailGM.cEmailContent,EMAIL_CONTENT_SIZE);
				pResult->GetValue(3, EmailGM.iSendTime,TIME_SIZE ); // yyyy-mm-dd hh:mm:ss  
				GM_list.push_back(EmailGM);
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

#if 0
	//读取用户邮件
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EmailGetUserData");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
			{
				CMD_G_EmailUserSendBack  EmailUserSendBack = {0};
				pResult->GetValue(0, EmailUserSendBack.dwAimUserID);
				pResult->GetValue(1, EmailUserSendBack.dwUserID);
				pResult->GetValue(2, EmailUserSendBack.cUserName,NAME_SIZE);
				pResult->GetValue(3, EmailUserSendBack.cEmailTitle,EMAIL_TITLE_SIZE);
				pResult->GetValue(4, EmailUserSendBack.cEmailContent,EMAIL_CONTENT_SIZE);
				pResult->GetValue(5, EmailUserSendBack.iSendTime, TIME_SIZE); // yyyy-mm-dd hh:mm:ss

				User_list.push_back(EmailUserSendBack);
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	//如果两个邮件数量超过30，发送前30封邮件，按时间处理
	GM_list_it gm_it_1 = GM_list.begin();
	User_list_it user_it_1 = User_list.begin();
	if (GM_list.size()+User_list.size()>EMAIL_MAX_ONCE)
	{
		CT_INT32 iCount=0;
		while(gm_it_1!=GM_list.end()&&user_it_1!=User_list.end())
		{
			if (gm_it_1->iSendTime>=user_it_1->iSendTime)
			{
				gm_it_1++;
				iCount++;
			}
			else
			{
				user_it_1++;
				iCount++;
			}
			if (iCount==EMAIL_MAX_ONCE)
			{
				break;
			}
		}
		//某个链表遍历完了，还没有筹够30邮件
		if (iCount!=EMAIL_MAX_ONCE)
		{
			if (gm_it_1==GM_list.end())
			{
				while(iCount!=EMAIL_MAX_ONCE)
				{
					iCount++;
					user_it_1++;
				}
			}
			else
			{
				while(iCount!=EMAIL_MAX_ONCE)
				{
					iCount++;
					gm_it_1++;
				}
			}
		}
		GM_list_it gm_it_2 = GM_list.begin();
		User_list_it user_it_2 = User_list.begin();
		//发送gm邮件
		while(gm_it_2!=gm_it_1)
		{
			CT_INT32 iMesLen = strlen(gm_it_2->cEmailContent);
			pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_GM_EMAIL,(CT_CHAR*)(&(*gm_it_2)),sizeof(CMD_G_EmailGM)-(EMAIL_CONTENT_SIZE-iMesLen));
			gm_it_2++;
		}
		while(user_it_2!=user_it_1)
		{
			CT_INT32 iMesLen = strlen(user_it_2->cEmailContent);
			pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL,(CT_CHAR*)(&(*user_it_2)),sizeof(CMD_G_EmailUserSendBack)-(EMAIL_CONTENT_SIZE-iMesLen));
			user_it_2++;
		}
		//如果GM邮件有发送，要更新用户GM index
		if (gm_it_1!=GM_list.begin())
		{
			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_EmailUpdateGmSend");
			m_dbConnector.AddParm("_uid",pTUE->dwUserID);
			m_dbConnector.AddParm("_sendGmIndex",iGMIndex);

			if(m_dbConnector.Exec())
			{
				CCDBResult* pResult = m_dbConnector.GetResult();
				//pResult->GetResult();
				pResult->Release();
			}else
			{
				G_DBError(m_dbConnector, m_dwThread);
				return CT_FALSE;
			}
		}
	}
	//直接发送所有邮件
	else
	{
		while(gm_it_1!=GM_list.end())
		{
			CT_INT32 iMesLen = strlen(gm_it_1->cEmailContent);
			pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_GM_EMAIL,(CT_CHAR*)(&(*gm_it_1)),sizeof(CMD_G_EmailGM)-(EMAIL_CONTENT_SIZE-iMesLen));
			gm_it_1++;
		}
		while(user_it_1!=User_list.end())
		{
			CT_INT32 iMesLen = strlen(user_it_1->cEmailContent);
			pUser->GetNetInterface()->Send(MAIN_G_EMAIL_EVENT_CITY,SUB_G_USER_EMAIL,(CT_CHAR*)(&(*user_it_1)),sizeof(CMD_G_EmailUserSendBack)-(EMAIL_CONTENT_SIZE-iMesLen));
			user_it_1++;
		}
		if (iGMIndex>0)
		{
			m_dbConnector.Reset();
			m_dbConnector.SetPcName("SP_EmailUpdateGmSend");
			m_dbConnector.AddParm("_uid",pTUE->dwUserID);
			m_dbConnector.AddParm("_sendGmIndex",iGMIndex);

			if(m_dbConnector.Exec())
			{
				CCDBResult* pResult = m_dbConnector.GetResult();
				//pResult->GetResult();
				pResult->Release();
			}else
			{
				G_DBError(m_dbConnector, m_dwThread);
				return CT_FALSE;
			}
		}
	}
	//清空用户邮件表
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EmailClearUserData");
	m_dbConnector.AddParm("_uid",pTUE->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		//pResult->GetResult();
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
#endif

	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetVipInfo(const MSC_DB_UserEnter* pTUE)
{
    CT_INT32 iGrade=0,iTime=0,iGift=0;
    if (ReadDBData(pTUE->dwUserID, iGrade, iTime, iGift))
    {
        MSC_DB_InsertVipInfo InsertVipInfo={0};
        InsertVipInfo.dwUserID=pTUE->dwUserID;
        InsertVipInfo.iGrade=iGrade;
        InsertVipInfo.iTime=iTime;
        InsertVipInfo.iGift=iGift;
        CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
        G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_VIPINFO,(CT_CHAR*)&InsertVipInfo,sizeof(InsertVipInfo));
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_VipGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_INT32 iGrade=0,iTime=0,iGift=0;
				CT_INT32 iFlag=0;
				pResult->GetValue(0, iFlag);
				if (iFlag==0)
				{
					continue;
				}
				pResult->GetValue(2, iGrade);
				pResult->GetValue(3, iTime );
				pResult->GetValue(4, iGift );
				//	G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_TASK_PROGRESS, 
				//		(CT_CHAR*)&taskPData, sizeof(MSC_DB_TaskProgressData));
				
				MSC_DB_InsertVipInfo InsertVipInfo={0};
				InsertVipInfo.dwUserID=pTUE->dwUserID;
				InsertVipInfo.iGrade=iGrade;
				InsertVipInfo.iTime=iTime;
				InsertVipInfo.iGift=iGift;
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_VIPINFO,(CT_CHAR*)&InsertVipInfo,sizeof(InsertVipInfo));
			}
		}
		pResult->Release();
	}else
	{
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
#endif
}

// 获取附魔信息
CT_BOOL CCDBUserEnterEvent::GetEnchantInfo(const MSC_DB_UserEnter *pTUE)
{
    CCUserEnchant::DataMap dataMap;
    if (ReadDBData(pTUE->dwUserID, dataMap))
    {
        for (CCUserEnchant::DataMap_IT it = dataMap.begin(); it != dataMap.end(); ++it)
        {
            MSC_DB_InsertEnchantData insertData = {0};
            insertData.dwUserID=pTUE->dwUserID;
            insertData.iBID = it->second.iBID;
            insertData.iIDX = it->second.iIDX;
            insertData.iGrade = it->second.iGrade;
			insertData.iFailCount = it->second.iFailCount;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ENCHANTDATA,(CT_CHAR*)&insertData,sizeof(insertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
#if 0
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UserEnchantGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_INT32 parts[] = ENCHANT_INDEX;
				MSC_DB_InsertEnchantData data = {0};
				for (int i = 0; i < sizeof(parts) / sizeof(*parts); ++i) {
					for (int j = 0; j < ENCHANT_ATTR_CNT; ++j) {
						data.iBID = parts[i];
						data.iIDX = j + 1;
						pResult->GetValue(1 + ENCHANT_ATTR_CNT * i + j, data.iGrade); // 第一个数据是用户ID

						if (data.iGrade > -1) 
						{
							data.dwUserID=pTUE->dwUserID;
							CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
							G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ENCHANTDATA,(CT_CHAR*)&data,sizeof(data));
						}
					}
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
#endif
}
//获取附魔奖励信息
CT_BOOL CCDBUserEnterEvent::GetEnchantRewardInfo(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EnchantRewardGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);
	MSC_DB_InsertEnchantReward insertData = {0};
	insertData.dwUserID = pTUE->dwUserID;
	if (m_dbConnector.Exec())
	{
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{ 
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, insertData.iIndex);
				
				CT_DWORD treadID = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(treadID, MSC_MAIN_INSERT_USERDATA, MSC_SUB_INSERT_ENCHANT_REWARD, (CT_CHAR*)&insertData,sizeof(insertData));
			}
			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(insertData.dwUserID,"SP_EnchantRewardGet","_uid:%d,_index:%d",insertData.dwUserID,insertData.iIndex);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}
//获得排行榜信息
CT_VOID CCDBUserEnterEvent::GetTopInfo(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(MSC_DB_Top))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetTopInfo len != sizeof(MSC_DB_Top)");
		return;
	}

	MSC_DB_Top *pTop=(MSC_DB_Top*)pBuf;
	MSC_DB_InsertTop InsertTop={0};
	InsertTop.iType=pTop->TopType;
	GetNoLinkUserInfo(pTop->dwUserID,InsertTop.UserFightinginfo,InsertTop.LookUserinfo,TOP_COUNT);
	 
	CT_DWORD threadid = GAME_THREAD_CITY;
	G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOP,(CT_CHAR*)&InsertTop,sizeof(InsertTop));

	return;
}

//获得对手信息
CT_BOOL CCDBUserEnterEvent::GetChalleng(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(MSC_DB_Challeng))
	{
		CCLogMeg::getSingleton().WriteError("CCDataBaseThread::GetChalleng: Len is wrong: len == sizeof(MSC_DB_Challeng)");
		return CT_FALSE;
	}

	MSC_DB_Challeng*   pChalleng = (MSC_DB_Challeng*) pBuf;
	MSC_DB_ChallengData ChallengInfo={0};
	ChallengInfo.dwUserID=pChalleng->dwUserID;

	GetNoLinkUserInfo(pChalleng->dwChalleng,ChallengInfo.UserFightinginfo,ChallengInfo.LookUserinfo,PVP_CHALLENGE_COUNT);
	CT_DWORD threadid = GAME_THREAD_CITY + pChalleng->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_EVENT_PVP,MSC_SUB_PVP_CHALLENG,(CT_CHAR*)&ChallengInfo,sizeof(ChallengInfo));

	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetAnother(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_ANOTHER_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetAnother: len != sizeof(CMD_G_ANOTHER_ARG)");
		return CT_FALSE;
	}
	CMD_G_ANOTHER_ARG *pAnotherArg=(CMD_G_ANOTHER_ARG*)pBuf;

	

	UserFightingStatus fightInfo={0};
	LookUserInfo anotherInfo = {0};
	GetNoLinkUserInfo(&(pAnotherArg->dwAnotherID),&fightInfo,&anotherInfo,1);
	

	MSC_PVP_OtherInfo otherInfo = {0};
	otherInfo.dwUserID = pAnotherArg->dwUserID;
	otherInfo.otherUserInfo = anotherInfo;

	CT_DWORD threadid = GAME_THREAD_CITY + pAnotherArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_MASTER , MSC_SUB_CHECK_OTHER_FINISH, (CT_CHAR*)&otherInfo, sizeof(otherInfo) );
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetFTFightingData(CT_CHAR* pBuf, CT_DWORD len)
{
	
	if (len != sizeof(CT_DWORD))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetFTFightingData: len != sizeof(CT_DWORD)");
		return CT_FALSE;
	}
	CT_DWORD dwUserID = 0;
	memcpy(&dwUserID, pBuf, len);

	MSC_DB_FTMemberFightingData dbFTMemberFightingData={0};
	LookUserInfo	userdata = {0};
	dbFTMemberFightingData.dwUserID = dwUserID;
	GetNoLinkUserInfo(&dwUserID,&dbFTMemberFightingData.userFightingData,&userdata,1);

#if 0	//整合到用户数据
	CT_DWORD threadid = GAME_THREAD_CITY + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;;
	G_SendThreadInfo(threadid,MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_DB_FIGHTTEAMFIGHTINGDATA,(CT_CHAR*)&dbFTMemberFightingData,sizeof(MSC_DB_FTMemberFightingData));
#endif
	return CT_TRUE;
}

CT_VOID CCDBUserEnterEvent::UpdatePresent(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(MSC_DB_Present))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::UpdatePresent len != sizeof(MSC_DB_Present)");
		return;
	}
	MSC_DB_Present *pInsertPresent=(MSC_DB_Present*)pBuf;
	CT_INT32 iRet = 0;
	MasterData masterData={0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PresentUpdate");
	m_dbConnector.AddParm("_uid",  pInsertPresent->dwUID);
	m_dbConnector.AddParm("_index",  pInsertPresent->iIndex);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pInsertPresent->dwUID,"SP_PresentUpdate","_uid:%d,_index:%d",pInsertPresent->dwUID,pInsertPresent->iIndex);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

//组队副本自动匹配
CT_VOID CCDBUserEnterEvent::GetTeamDuplicateAutoMatch(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(MSC_DB_TeamDuplicateAutoMatchInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetTeamDuplicateAutoMatch len != sizeof(MSC_DB_TeamDuplicateAutoMatchInfo)");
		return;
	}

	MSC_DB_TeamDuplicateAutoMatchInfo *pAutoMatch=(MSC_DB_TeamDuplicateAutoMatchInfo*)pBuf;
	MSC_DB_TeamDuplicateUserInfo UserData={0};
	UserData.dwUserID=pAutoMatch->dwUserID;

	GetNoLinkUserInfo(pAutoMatch->dwDstUserID,UserData.UserFightinginfo,UserData.LookUserinfo,TEAM_NUM-1);
	CT_DWORD threadid = GAME_THREAD_CITY + pAutoMatch->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;;
	G_SendThreadInfo(threadid,MSC_MAIN_TEAMDUPLICATE,MSC_SUB_TEAMDUPLICATE_AUTOMATCHINFO,(CT_CHAR*)&UserData,sizeof(UserData));

	return;
}

//得到领主信息
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID,MasterData &masterData)
{
	masterData.dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(dwUserID);

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetMasterData");
	m_dbConnector.AddParm("_uid",  dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, iRet);
				if (iRet > 0)
				{
					pResult->GetValue(1, masterData.dwUserID);
					pResult->GetValue(2, masterData.cNickName, NAME_SIZE);
					pResult->GetValue(3, masterData.mvd.stCivili.iLevel );
					pResult->GetValue(4, masterData.mvd.stCivili.iCurrentExp);

					pResult->GetValue(5, masterData.mvd.stRes.iGold);
					pResult->GetValue(6, masterData.mvd.stRes.iDiamod);
					pResult->GetValue(7, masterData.mvd.stRes.iFood);

					pResult->GetValue(8, masterData.iOccupation );
					pResult->GetValue(9, masterData.mvd.stRes.iTalentNum);

					pResult->GetValue(10, masterData.mvd.stRes.iStamina);
					pResult->GetValue(11, masterData.mvd.stGrade.iLevel );
					pResult->GetValue(12, masterData.mvd.stGrade.iCurrentExp);
					pResult->GetValue(13, masterData.mvd.stRank.iLevel );
					pResult->GetValue(14, masterData.mvd.stRank.iCurrentExp);

					pResult->GetValue(15, masterData.llLastExitTime);
					pResult->GetValue(16, masterData.mvd.stRes.iBagSlots);
					pResult->GetValue(17, masterData.mvd.stRes.iSkillSoul);
					pResult->GetValue(18, masterData.mvd.pvpData.iVictoryTimes);
					pResult->GetValue(19, masterData.mvd.pvpData.iFightTimes);
					pResult->GetValue(20, masterData.mvd.pvpData.iVictoryRate);
                    // 21 排名
					pResult->GetValue(22, masterData.mvd.stRes.iStaminaBuyTimes);
					pResult->GetValue(23, masterData.mvd.stRes.iSkillSlots);
                    // 25 战斗战斗次数？
					pResult->GetValue(24, masterData.mvd.stRes.iFightingPower);
					pResult->GetValue(25, masterData.mvd.stRes.iTeamDuplicateFightingTimes);
                    pResult->GetValue(26, masterData.iChannel);
                    pResult->GetValue(27, masterData.mvd.stRes.iElmSlots);

					masterData.primeMvd = masterData.mvd;
				}
			}
		}
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetMasterData","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetTopInfoByUser");
	m_dbConnector.AddParm("_uid",  dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, iRet);
				if (iRet > 0)
				{
					CT_INT32 iRewardStatus = 0;
					pResult->GetValue(3, masterData.mvd.topData.iMaxContinuousWin);
					pResult->GetValue(4, masterData.mvd.topData.TopInfo[TOP_TYPE_WIN].iRankingCur);
					pResult->GetValue(5, masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingCur );
					pResult->GetValue(6, masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingCur);
					pResult->GetValue(7, masterData.mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingCur);
					pResult->GetValue(8, masterData.mvd.topData.TopInfo[TOP_TYPE_WIN].iRankingChange);
					pResult->GetValue(9, masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingChange);
					pResult->GetValue(10, masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingChange);
					pResult->GetValue(11, masterData.mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingChange);
					pResult->GetValue(12, masterData.mvd.pvpData.iCurrentContinuousWin);
					pResult->GetValue(13, masterData.mvd.topData.TopInfo[TOP_FLAG].iRankingCur);
					pResult->GetValue(14, masterData.mvd.topData.TopInfo[TOP_FLAG].iRankingChange);
					pResult->GetValue(15, masterData.mvd.topData.TopInfo[TOP_PLUNDER].iRankingCur);
					pResult->GetValue(16, masterData.mvd.topData.TopInfo[TOP_PLUNDER].iRankingChange);
					pResult->GetValue(17, iRewardStatus);
					pResult->GetValue(18, masterData.mvd.plunderData.iPlunderScore);

					masterData.mvd.topData.TopInfo[TOP_TYPE_WIN].iType=TOP_TYPE_WIN;
					masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iType=TOP_TYPE_POWER;
					masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iType=TOP_TYPE_GRADE;
					masterData.mvd.topData.TopInfo[TOP_TYPE_RANKING].iType=TOP_TYPE_RANKING;
					masterData.mvd.topData.TopInfo[TOP_FLAG].iType=TOP_FLAG;
					masterData.mvd.topData.TopInfo[TOP_PLUNDER].iType=TOP_TYPE_PLUNDER;

					masterData.mvd.topData.TopInfo[TOP_TYPE_WIN].iRankingChange=masterData.mvd.topData.TopInfo[TOP_TYPE_WIN].iRankingChange-masterData.mvd.topData.TopInfo[TOP_TYPE_WIN].iRankingCur;
					masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingChange=masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingChange-masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingCur;
					masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingChange=masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingChange-masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingCur;
					masterData.mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingChange=masterData.mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingChange-masterData.mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingCur;
					masterData.mvd.topData.TopInfo[TOP_FLAG].iRankingChange=masterData.mvd.topData.TopInfo[TOP_FLAG].iRankingChange-masterData.mvd.topData.TopInfo[TOP_FLAG].iRankingCur;
					

					if (masterData.mvd.plunderData.iPlunderScore == 0)
					{
						masterData.mvd.topData.TopInfo[TOP_PLUNDER].iRankingCur = 0;
						masterData.mvd.topData.TopInfo[TOP_PLUNDER].iRankingChange *= (-1);
					}
					else
					{
						masterData.mvd.topData.TopInfo[TOP_PLUNDER].iRankingChange=masterData.mvd.topData.TopInfo[TOP_PLUNDER].iRankingChange-masterData.mvd.topData.TopInfo[TOP_PLUNDER].iRankingCur;
					}

					for (CT_INT32 iToptmp = 0;iToptmp < TOP_TYPE_MAX;++iToptmp)
					{
						masterData.mvd.topData.TopInfo[iToptmp].iRewardStatus = iRewardStatus & (1 << iToptmp) ?1:0;
					}

					masterData.primeMvd.pvpData.iCurrentContinuousWin = masterData.mvd.pvpData.iCurrentContinuousWin;
				}
			}
		}
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetTopInfoByUser","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetMasterDataEx");
	m_dbConnector.AddParm("_uid",  dwUserID );
	//	select U_ID, LeaveTeamTime, TowerHelpTime,ChargeDiamod,CostDiamod,FPTowerTime,FPWorshipTime1,FPWorshipTime2,FPWorshipTime3,FPWorshipTime4,FPWorshipTime5,FPWorshipTime6,
	// FPChestTime,CaveTime,PVPChest,FTChest,Turntable1,Turntable2,Turntable3
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, masterData.mex.iLeaveTeamTime);
				pResult->GetValue(2, masterData.mex.iTowerHelpTime);
				pResult->GetValue(3, masterData.mex.iChargeDiamod);
				pResult->GetValue(4, masterData.mex.iCostDiamod);
				pResult->GetValue(5, masterData.mex.fpd.iFPTowerTime);
				pResult->GetValue(6, masterData.mex.fpd.iFPWorshipTime[0]);
				pResult->GetValue(7, masterData.mex.fpd.iFPWorshipTime[1]);
				pResult->GetValue(8, masterData.mex.fpd.iFPWorshipTime[2]);
				pResult->GetValue(9, masterData.mex.fpd.iFPWorshipTime[3]);
				pResult->GetValue(10, masterData.mex.fpd.iFPWorshipTime[4]);
				pResult->GetValue(11, masterData.mex.fpd.iFPWorshipTime[5]);
				pResult->GetValue(12, masterData.mex.fpd.iFPChestTime);
				pResult->GetValue(13, masterData.mex.fpd.iCaveTime);
				pResult->GetValue(14, masterData.mex.fpd.iPVPChest);
				pResult->GetValue(15, masterData.mex.fpd.iFTChest);
				pResult->GetValue(16, masterData.mex.fpd.iTurntable[0]);
				pResult->GetValue(17, masterData.mex.fpd.iTurntable[1]);
				pResult->GetValue(18, masterData.mex.fpd.iTurntable[2]);
			}
		}
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetMasterDataEx","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	if (iRet > 0)
	{
	}else
	{	
		//异常情况，数据库数据无法匹配用户的id，说明用户没有在这组服务器上生成角色
		CCLogMeg::getSingleton().WriteError("CCDataBaseThread::GetMasterInfo(const CT_DWORD dwUserID,MasterData &masterInfo) : Can't find %i masterData", dwUserID);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到潜能数据
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserTalent::DataMap &talentMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetTalent");

	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();        
		if (pResult)
		{
			if (pResult->GetResult())
			{
				while (pResult->MoveNextRow())
				{
					TalentData talentData = {0};                
					pResult->GetValue(0, talentData.iID);
					pResult->GetValue(1, talentData.szName, sizeof(talentData.szName));
					talentData.iStatus = TALENT_ACTIVATED; // 标识已学会

                    talentMap.insert(CCUserTalent::DataMap::value_type(talentData.iID, talentData));
				}
			}

			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetTalent","_uid:%d \n%s%d",dwUserID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到技能数据
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserSkill::DataMap &skillMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetSkill");

	m_dbConnector.AddParm("_uid",dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			if (pResult->GetResult())
			{
				while (pResult->MoveNextRow())
				{
					SkillData skillData = {0};
					pResult->GetValue(0, skillData.iID );
					pResult->GetValue(1, skillData.szName, sizeof(skillData.szName));
					pResult->GetValue(2, skillData.iSkillSoul);
					pResult->GetValue(3, skillData.Used);
					
                    skillMap.insert(CCUserSkill::DataMap::value_type(skillData.iID, skillData));
				}
			}

			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetSkill","_uid:%d\n%s%d",dwUserID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到道具信息
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserItem::DataMap &itemMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropGet");

	m_dbConnector.AddParm("_uid",dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			if (pResult->GetResult())
			{
				while (pResult->MoveNextRow())
				{
					ItemData itemData = {0};
					pResult->GetValue(0, itemData.key);
					pResult->GetValue(1, itemData.iID);
					pResult->GetValue(2, itemData.szName, sizeof(itemData.szName));
					pResult->GetValue(3, itemData.iCount);
					pResult->GetValue(4, itemData.iIndex);

                    itemMap.insert(CCUserItem::DataMap::value_type(itemData.iIndex, itemData));
				}
			}

			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_PropGet","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}
// 得到装备信息
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserEquipment::DataMap &equipMap)
{
	// 强化与插孔
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropGetStrengthen");

	m_dbConnector.AddParm("_uid", dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			if (pResult->GetResult())
			{
				while (pResult->MoveNextRow())
				{
                    EquipmentData equipData = {0};
					pResult->GetValue(1, equipData.key);                    
					//pResult->GetValue(2, iID);
					pResult->GetValue(3, equipData.iLevel);
					pResult->GetValue(4, equipData.nSlot);

					equipMap.insert(CCUserEquipment::DataMap::value_type(equipData.key,equipData));
				}
			}

			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 洗炼信息
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropGetTrain");

	m_dbConnector.AddParm("_uid",dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			if (pResult->GetResult())
			{
				CT_INT32 attrs[] = FORGE_ATTR_ARRAY;
				while (pResult->MoveNextRow())
				{
					CT_INT32 n = 0;
					CT_INT32 iID = 0;
					EquipmentData equipData = {0};
					n++;
					pResult->GetValue(n++, equipData.key);
					for (int i = 0; i < sizeof(attrs) / sizeof(*attrs); ++i)
					{
						CT_FLOAT fValue = 0.0f;
						pResult->GetValue(n++, fValue); 
						if (fValue > 0.000001)  // not zero
						{
							for (int j = 0; j < FORGE_ATTRS; ++j)
							{
								if (equipData.Forge[j].iAID == 0)
								{
									equipData.Forge[j].iAID = attrs[i];
									equipData.Forge[j].fValue = fValue;
									break;
								}
							}
						}
					}

                    CCUserEquipment::DataMap_IT it = equipMap.find(equipData.key);
					if(it != equipMap.end())
					{
						memcpy(&((*it).second.Forge),&equipData.Forge,sizeof(equipData.Forge));
					}
					else
					{
						equipMap.insert(CCUserEquipment::DataMap::value_type(equipData.key,equipData));
					}
				}
			}

			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 镶嵌信息
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropGetInlay");

	m_dbConnector.AddParm("_uid",dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			if (pResult->GetResult())
			{
				while (pResult->MoveNextRow())
				{
					CT_INT32 n = 0;
					CT_INT32 iSlot = 0;
					CT_INT32 iGemID = 0;
					EquipmentData equipData = {0};
					n++;
					pResult->GetValue(n++, equipData.key);
					pResult->GetValue(n++, iGemID);
					pResult->GetValue(n++, iSlot);

					CCUserEquipment::DataMap_IT it=equipMap.find(equipData.key);
					if(it != equipMap.end())
					{
						(*it).second.Gem[iSlot]=iGemID;
					}
					else
					{
						equipMap.insert(CCUserEquipment::DataMap::value_type(equipData.key,equipData));
					}
				}
			}
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到增益数据
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserBuff::DataMap &buffMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PropGetUse");

	m_dbConnector.AddParm("_uid", dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			if (pResult->GetResult())
			{
				while (pResult->MoveNextRow())
				{
					BuffData buffData = {0};
					pResult->GetValue(0, buffData.iID);
					pResult->GetValue(1, buffData.toggleTime);
					pResult->GetValue(2, buffData.iDuration);

					time_t currTime = time(0);
					if (currTime < buffData.toggleTime + buffData.iDuration)
					{
                        G_ItemBuff itBuff = {0};
                        G_GetItemBuff(buffData.iID, &itBuff);
                        buffData.iGroup = itBuff.iGroup;
                        buffMap.insert(CCUserBuff::DataMap::value_type(buffData.iGroup, buffData));
					} 
				}
			}
			pResult->Release();            
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//得到军队信息
CT_BOOL	CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserArmy::DataMap &armyMap)
{
	ArmyInfo  data = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyGet");

	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_DWORD dwValue=0;
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dwValue );
				if(dwValue != 0)
				{
					pResult->GetValue(1, data.iArmyID);
					pResult->GetValue(2, data.iArmyGrade);

                    armyMap.insert(CCUserArmy::AtmyInfoMap::value_type(data.iArmyID, data));
				}

			}
		}

		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

//得到阵法信息
CT_BOOL	CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID,MSC_DB_BattleArray  &BattleArraydata)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ArmyGetPos");

	m_dbConnector.AddParm("_uid",  dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_DWORD dwValue=0;
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dwValue );
				if(dwValue != 0)
				{
					pResult->GetValue(0, BattleArraydata.dwUserID );
					pResult->GetValue(1, BattleArraydata.iPosition[0]);
					pResult->GetValue(2, BattleArraydata.iPosition[1]);
					pResult->GetValue(3, BattleArraydata.iPosition[2]);
					pResult->GetValue(4, BattleArraydata.iPosition[3]);
					pResult->GetValue(5, BattleArraydata.iPosition[4]);
				}
			}
		}

		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

//获取宠物信息
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, PetInfo &pi)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetGet");
	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_INT32 iRow=0;
				pResult->GetValue(iRow++, pi.iPetID);
				pResult->GetValue(iRow++, pi.iGrade );
				pResult->GetValue(iRow++, pi.iExp );
				pResult->GetValue(iRow++, pi.iChangeID );
				pResult->GetValue(iRow++, pi.iSkillID[0] );
				pResult->GetValue(iRow++, pi.iSkillID[1] );
				pResult->GetValue(iRow++, pi.iSkillID[2] );
				pResult->GetValue(iRow++, pi.iSkillID[3] );
				pResult->GetValue(iRow++, pi.iSkillID[4] );
				pResult->GetValue(iRow++, pi.iSkillID[5] );
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

//获取vip信息
CT_BOOL	CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CT_INT32 &vipGrade, CT_INT32 &vipTime, CT_INT32 &vipGift)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_VipGet");
	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_INT32 iFlag=0;
				pResult->GetValue(0, iFlag);
				if (iFlag==0)
				{
					continue;
				}
				pResult->GetValue(2, vipGrade);
				pResult->GetValue(3, vipTime );
				pResult->GetValue(4, vipGift );
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 获取附魔信息
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserEnchant::DataMap &enchantMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_UserEnchantGet");
	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				CT_INT32 parts[] = ENCHANT_INDEX;
				EnchantData data = {0};
				for (int i = 0; i < sizeof(parts) / sizeof(*parts); ++i) {
					for (int j = 0; j < ENCHANT_ATTR_CNT; ++j) {
						data.iBID = parts[i];
						data.iIDX = j + 1;
						pResult->GetValue(1 + ENCHANT_ATTR_CNT * i + j, data.iGrade); 

						if (data.iGrade > -1) 
						{
							enchantMap.insert(CCUserEnchant::DataMap::value_type(G_Combine64(data.iBID, data.iIDX), data));
						}
					}
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EnchantFailGet");
	m_dbConnector.AddParm("_uid", dwUserID);
	if (m_dbConnector.Exec())
	{
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				CT_INT32 iIndex = 0;
				EnchantData enchantData = {0};

				pResult->GetValue(iIndex++, enchantData.iBID);
				pResult->GetValue(iIndex++, enchantData.iIDX);
				pResult->GetValue(iIndex++, enchantData.iFailCount);

				CCUserEnchant::DataMap_IT it = enchantMap.find(G_Combine64(enchantData.iBID, enchantData.iIDX));
				if (it != enchantMap.end())
				{
					(it->second).iFailCount = enchantData.iFailCount;
				}
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

// 获取附魔奖励信息
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserEnchant::EnchantRewardVector& enchantRewardVec)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EnchantRewardGet");
	m_dbConnector.AddParm("_uid", dwUserID);
	CT_INT32 iIndex = 0;
	if (m_dbConnector.Exec())
	{
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{ 
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, iIndex);
				enchantRewardVec.push_back(iIndex);
			}
			pResult->Release();
		}
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

// 战旗数据
CT_BOOL CCDBUserEnterEvent::ReadDBData( const CT_DWORD dwUserID, CCUserBattleFlag::DataMap &flagMap )
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_BattleFlagGet");

    m_dbConnector.AddParm("_uid", dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    CT_INT32 index = 0;
                    BattleFlag dbData = {0};
                    index++; // uid
                    pResult->GetValue(index++, dbData.iType);
                    pResult->GetValue(index++, dbData.iLevel);

                    flagMap.insert(CCUserBattleFlag::DataMap::value_type(dbData.iType, dbData));
                }
            }

            pResult->Release();
        }
    }
    else
    {
        CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_ElmPowerGet","_uid:%d",dwUserID);
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

    return CT_TRUE;
}

// 元素之力数据
CT_BOOL CCDBUserEnterEvent::ReadDBData( const CT_DWORD dwUserID, CCUserItem::DataMap &elmMap, CCUserElmAttr::DataMap &elmAttrMap )
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_ElmPropGet");

    m_dbConnector.AddParm("_uid", dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    ItemData itemData = {0};
                    pResult->GetValue(0, itemData.key);
                    // uid
                    pResult->GetValue(2, itemData.iID);
                    pResult->GetValue(3, itemData.szName, sizeof(itemData.szName));
                    pResult->GetValue(4, itemData.iCount);
                    pResult->GetValue(5, itemData.iIndex);
                    
                    elmMap.insert(CCUserItem::DataMap::value_type(itemData.iIndex, itemData));
                }
            }
            pResult->Release();
        }
    }
    else
    {
        CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_ElmPowerGet","_uid:%d",dwUserID);
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }
    
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_ElmAttrGet");

    m_dbConnector.AddParm("_uid", dwUserID);
    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult)
        {
            if (pResult->GetResult())
            {
                while (pResult->MoveNextRow())
                {
                    CT_INT32 index = 0;
                    ElmMarrowAttr dbData = {0};
                    index++; // uid
                    pResult->GetValue(index++, dbData.key);
                    pResult->GetValue(index++, dbData.iLevel);
                    pResult->GetValue(index++, dbData.iExp);
                    for (CT_INT32 i = 0; i < sizeof(dbData.attrs) / sizeof(*dbData.attrs); ++i)
                    {
                        index++;                        
                        pResult->GetValue(index++, dbData.attrs[i]);
                    }

                    elmAttrMap.insert(CCUserElmAttr::DataMap::value_type(dbData.key, dbData));
                }
            }
            pResult->Release();
        }
    }
    else
    {
        CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_ElmPowerGet","_uid:%d",dwUserID);
        //G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }

    return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserTower::AwardStatusMap &towerMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerGet");

	m_dbConnector.AddParm("_uid", dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult)
		{
			if (pResult->GetResult())
			{
				while (pResult->MoveNextRow())
				{
					CT_INT32 index = 0;
					CT_INT32 iCPID = 0;
					index++; // uid
					pResult->GetValue(index++, iCPID);

					G_TowerCheckPoint tcp = {0};
					if (G_GetTowerCheckPoint(iCPID, &tcp))
					{
						G_AttributesReward ar = {0};
						if (G_GetAttributesReward(tcp.iPersonRewardID, &ar))
						{
							if (ar.iResType1 > 0)
							{
								towerMap[ar.iResType1] += ar.fResNum1;
							}
							if (ar.iResType2 > 0)
							{
								towerMap[ar.iResType2] += ar.fResNum2;
							}
							if (ar.iResType3 > 0)
							{
								towerMap[ar.iResType3] += ar.fResNum3;
							}
							if (ar.iResType4 > 0)
							{
								towerMap[ar.iResType4] += ar.fResNum4;
							}
						}
					}
				}
			}

			pResult->Release();
		}
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_TowerGet","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::ReadDBData( const CT_DWORD dwUserID, CCUserFightTeamNember::DataMap &scienceMap )
{
    m_dbConnector.Reset();
    m_dbConnector.SetPcName("SP_FightTeamTechnGet");
    m_dbConnector.AddParm("_uid", dwUserID );

    if(m_dbConnector.Exec())
    {
        CCDBResult* pResult = m_dbConnector.GetResult();
        if (pResult->GetResult())
        {  
            while (pResult->MoveNextRow())
            {
                //MSC_DB_InsterFightTeamScience FTS={0};
                NemberScience nbSci = {0};
                pResult->GetValue(0, nbSci.dwUserID);
                pResult->GetValue(1, nbSci.iScienceID );
                pResult->GetValue(2, nbSci.iScienceGrade);

                scienceMap.insert(std::make_pair(nbSci.iScienceID, nbSci));
            }
        }
        pResult->Release();
    }else
    {
        G_DBError(m_dbConnector, m_dwThread);
        return CT_FALSE;
    }
    return CT_TRUE;
}

CT_VOID CCDBUserEnterEvent::GetPTRLinkUserData(OnLinkUserData &UserData,PTRLinkUserData &linkUserData)
{
	linkUserData.pMd = &UserData.masterInfo;
	linkUserData.pTalentMap = &UserData.talentMap;
	linkUserData.pItemMap = &UserData.itemMap;
	linkUserData.pEquipMap = &UserData.equipMap;
	linkUserData.pVipGrade = &UserData.VipGrade;
	linkUserData.pEnchantMap = &UserData.enchantMap;
	linkUserData.pFlagMap = &UserData.flagMap;
	linkUserData.pElmMap = &UserData.elmMap;
	linkUserData.pElmAttrMap = &UserData.elmAttrMap;
	linkUserData.pBuffMap = &UserData.buffMap;
	linkUserData.pArray = UserData.BattleArraydata.iPosition;
	linkUserData.pTowerMap = &UserData.towerMap;
	linkUserData.pArmyMap = &UserData.armyMap;
	linkUserData.pSkillMap = &UserData.skillMap;
	linkUserData.pScienceMap = &UserData.scienceMap;
	linkUserData.pEnchatRewardVector = &UserData.enchantRewardVector;
	linkUserData.pTitleMap = &UserData.titleMap;
	linkUserData.pPetInfo= &UserData.pi;
	linkUserData.pSeekArray= UserData.SeekArray;
	linkUserData.pWallAdditionMap=&UserData.wamap;
	linkUserData.pMasterFightAddMap=&UserData.mfamap;
	linkUserData.pcmap=&UserData.pcmap;
	return;
}

// 更新领主的战斗数据
CT_VOID CCDBUserEnterEvent::UpdateBattleStatus(OnLinkUserData &UserData)
{
    MasterCombatStatus mcs = {0};
	FightingPower Power={0};
    PTRLinkUserData linkUserData = {0};
    linkUserData.pMd = &UserData.masterInfo;
    linkUserData.pTalentMap = &UserData.talentMap;
    linkUserData.pItemMap = &UserData.itemMap;
    linkUserData.pEquipMap = &UserData.equipMap;
    linkUserData.pVipGrade = &UserData.VipGrade;
    linkUserData.pEnchantMap = &UserData.enchantMap;
    linkUserData.pFlagMap = &UserData.flagMap;
    linkUserData.pElmMap = &UserData.elmMap;
    linkUserData.pElmAttrMap = &UserData.elmAttrMap;
    linkUserData.pBuffMap = &UserData.buffMap;
    linkUserData.pArray = UserData.BattleArraydata.iPosition;
	linkUserData.pTowerMap = &UserData.towerMap;
    linkUserData.pScienceMap = &UserData.scienceMap;
	linkUserData.pEnchatRewardVector = &UserData.enchantRewardVector;
	linkUserData.pTitleMap = &UserData.titleMap;

	CCUser::CalcUserFinalCombatStatus(mcs, linkUserData,Power);
    UserData.masterInfo.mvd.stRes.iFightingPower = Power.iMasterFightingPower;

	// 乘法
	mcs.iMaxHP = (CT_INT32)(mcs.iMaxHP * mcs.fHPPersent);
	// 上限限制    
    CCUserBase::LimiteBattleStatus(mcs);

	// 当前血量/法力值不能高过上限 - 同步血量 // 当前血量值在战斗外已经无意义
	mcs.iCurrentHP = mcs.iMaxHP;
	mcs.iCurrentMP = mcs.iMaxMP;
	UserData.masterInfo.mcs = mcs;

	return;
}

CT_BOOL CCDBUserEnterEvent::GetUserActiveEventInfo(const MSC_DB_UserEnter* pTUE)
{
	CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ActiveStatusGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_ACTIVE_Status activeStatus = {0};
	activeStatus.dwUserID = pTUE->dwUserID;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, activeStatus.iAID );
				pResult->GetValue(2, activeStatus.iStatus );
				pResult->GetValue(3, activeStatus.iQuanity );
				G_SendThreadInfo(threadid,MSC_MAIN_USER_ENTER , MSC_SUB_ENTER_ACTIEVE_ADD, (CT_CHAR*)&activeStatus, sizeof(activeStatus) );
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TurnTableGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_TurnTable_Status status = {0};
	status.dwUserID = pTUE->dwUserID;
	CT_BOOL bEmpty = CT_TRUE;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, status.iTimes1 );
				pResult->GetValue(2, status.iTimes2 );
				pResult->GetValue(3, status.iTimes3 );
				
				G_SendThreadInfo(threadid,MSC_MAIN_USER_ENTER , MSC_SUB_ENTER_TRUNTABLE_ADD, (CT_CHAR*)&status, sizeof(status) );
				bEmpty = CT_FALSE;
			}
		}
		pResult->Release();
	}else
	{
		G_SendThreadInfo(threadid,MSC_MAIN_USER_ENTER , MSC_SUB_ENTER_TRUNTABLE_ADD, (CT_CHAR*)&status, sizeof(status) );
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	if (bEmpty)
	{
		G_SendThreadInfo(threadid,MSC_MAIN_USER_ENTER , MSC_SUB_ENTER_TRUNTABLE_ADD, (CT_CHAR*)&status, sizeof(status) );
	}
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetUserWorshipInfo(const MSC_DB_UserEnter* pTUE)
{
	CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_WorshipStatusGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_WorshipStatus status = {0};
	status.dwUserID = pTUE->dwUserID;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, status.iType );
				pResult->GetValue(2, status.iStatus );
				pResult->GetValue(3, status.iXp );
				G_SendThreadInfo(threadid,MSC_MAIN_USER_ENTER , MSC_SUB_ENTER_WORSHIP_ADD, (CT_CHAR*)&status, sizeof(status) );
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//获取扫荡信息
CT_BOOL CCDBUserEnterEvent::GetRaidsInfo(const MSC_DB_UserEnter* pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_RaidsInfoGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_InsertRaidsInfo InsertRaidsInfo={0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertRaidsInfo.dwUserID);
				pResult->GetValue(1, InsertRaidsInfo.iWorldID);
				pResult->GetValue(2, InsertRaidsInfo.iSceneID );
				pResult->GetValue(3, InsertRaidsInfo.iDifficulty);
				pResult->GetValue(4, InsertRaidsInfo.iAllTime);
				pResult->GetValue(5, InsertRaidsInfo.iCurrentTime);
				pResult->GetValue(6, InsertRaidsInfo.lBeginTime);
				pResult->GetValue(7, InsertRaidsInfo.iLastStamina);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_RAIDSINFO,(CT_CHAR*)&InsertRaidsInfo,sizeof(InsertRaidsInfo));					



	if((InsertRaidsInfo.iAllTime != InsertRaidsInfo.iCurrentTime && InsertRaidsInfo.iAllTime != 0) || InsertRaidsInfo.lBeginTime != 0)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_RaidsRewardGet");
		m_dbConnector.AddParm("_uid", pTUE->dwUserID );

		if(m_dbConnector.Exec())
		{
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					MSC_DB_InsertRaidsReward InsertRaidsReward={0};
					pResult->GetValue(0, InsertRaidsReward.dwUserID);
					pResult->GetValue(1, InsertRaidsReward.iExperience );
					pResult->GetValue(2, InsertRaidsReward.iGold);
					pResult->GetValue(3, InsertRaidsReward.iDiamond);
					pResult->GetValue(4, InsertRaidsReward.iPotential);
					pResult->GetValue(5, InsertRaidsReward.iSkillPoint);
					pResult->GetValue(6, InsertRaidsReward.iGoods[0]);
					pResult->GetValue(7, InsertRaidsReward.iGoods[1]);
					pResult->GetValue(8, InsertRaidsReward.iGoods[2]);
					pResult->GetValue(9, InsertRaidsReward.iGoods[3]);
					pResult->GetValue(10, InsertRaidsReward.iGoodsCount[0]);
					pResult->GetValue(11, InsertRaidsReward.iGoodsCount[1]);
					pResult->GetValue(12, InsertRaidsReward.iGoodsCount[2]);
					pResult->GetValue(13, InsertRaidsReward.iGoodsCount[3]);
					pResult->GetValue(14, InsertRaidsReward.iCurrentTime);

					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_RAIDSREWARD,(CT_CHAR*)&InsertRaidsReward,sizeof(InsertRaidsReward));					

				}
			}
			pResult->Release();
		}else
		{
			G_DBError(m_dbConnector, m_dwThread);
			return CT_FALSE;
		}
	}

	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetFriendInfo(const MSC_DB_UserEnter* pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FriendGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_FRIEND	InsertFriend = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertFriend.dwU_ID );
				pResult->GetValue(1, InsertFriend.dwF_UID );
				pResult->GetValue(2, InsertFriend.cF_NickName, NAME_SIZE );
				pResult->GetValue(3, InsertFriend.iF_Occupation );
				pResult->GetValue(4, InsertFriend.iF_Type );
				pResult->GetValue(5, InsertFriend.iF_Grade);
				pResult->GetValue(6, InsertFriend.iF_FightingPower);
				pResult->GetValue(7, InsertFriend.iLastExitTime);
				pResult->GetValue(8, InsertFriend.iTitleID);

				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_FRIENDINFO,(CT_CHAR*)&InsertFriend,sizeof(InsertFriend));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	//反向黑名单数据
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_FriendReverseGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );
	InsertFriend.iF_Type = FRIEND_BLACKREV;
	InsertFriend.dwU_ID = pTUE->dwUserID;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertFriend.dwF_UID );
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_FRIENDINFO,(CT_CHAR*)&InsertFriend,sizeof(InsertFriend));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;

}

CT_BOOL CCDBUserEnterEvent::GetPresent(const MSC_DB_UserEnter* pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PresentGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_Present	InsertPresent = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertPresent.dwUID );
				pResult->GetValue(1, InsertPresent.iIndex );
				pResult->GetValue(2, InsertPresent.iP_ID);
				pResult->GetValue(3, InsertPresent.iP_Count);
				pResult->GetValue(4, InsertPresent.iSLotID);
				pResult->GetValue(5, InsertPresent.cDesc, 255);

				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_PRESENT,(CT_CHAR*)&InsertPresent,sizeof(MSC_DB_Present));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//获取组队副本场景
CT_BOOL CCDBUserEnterEvent::GetTeamDuplicateSceneInfo(const MSC_DB_UserEnter* pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamAreaGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_InsertTeamDuplicateScene	SceneData = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, SceneData.dwUserID );
				pResult->GetValue(1, SceneData.iSceneID );
				pResult->GetValue(2, SceneData.iDifficulty);
				pResult->GetValue(3, SceneData.iWin);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TEAMDUPLICATE,(CT_CHAR*)&SceneData,sizeof(SceneData));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

// 获得元素之力数据 
CT_BOOL CCDBUserEnterEvent::GetElmPowerInfo( const MSC_DB_UserEnter *pTUE )
{    
    CCUserItem::DataMap elmMap;
    CCUserElmAttr::DataMap elmAttrMap;
    if (ReadDBData(pTUE->dwUserID, elmMap, elmAttrMap))
    {
        for (CCUserItem::DataMap_IT it = elmMap.begin(); it != elmMap.end(); ++it)
        {
            MSC_DB_InsertItemInfo InsertData = {0};
            InsertData.dwUserID = pTUE->dwUserID;
            InsertData.itemData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ELMPOWER,(CT_CHAR*)&InsertData,sizeof(InsertData));
        }
        for (CCUserElmAttr::DataMap_IT it = elmAttrMap.begin(); it != elmAttrMap.end(); ++it)
        {
            MSC_DB_InsertElmAttrInfo InsertData = {0};
            InsertData.dwUserID = pTUE->dwUserID;
            InsertData.elmData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ELMATTR,(CT_CHAR*)&InsertData,sizeof(InsertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
}

// 获得战旗数据
CT_BOOL CCDBUserEnterEvent::GetBattleFlagInfo( const MSC_DB_UserEnter *pTUE )
{
    CCUserBattleFlag::DataMap flagMap;
    if (ReadDBData(pTUE->dwUserID, flagMap))
    {
        for (CCUserBattleFlag::DataMap_IT it = flagMap.begin(); it != flagMap.end(); ++it)
        {
            MSC_DB_InsertBattleFlagInfo InsertData = {0};
            InsertData.dwUserID = pTUE->dwUserID;
            InsertData.flagData = it->second;
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BATTLEFLAG,(CT_CHAR*)&InsertData,sizeof(InsertData));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
}

//得到离线战斗数据
CT_VOID CCDBUserEnterEvent::GetTowerOffLineUserFightData(CT_CHAR* pBuf, CT_INT32 len)
{
	if (sizeof(MSC_TowerOfflineUserFightData ) != len)
	{
		return;
	}

	MSC_TowerOfflineUserFightData* pOfflinePVP = (MSC_TowerOfflineUserFightData*)pBuf;

	LookUserInfo  userdata = {0};
	GetNoLinkUserInfo(&(pOfflinePVP->dwOfflineUserID), &pOfflinePVP->offlineFightingData,&userdata, 1);
//	CCGameMesEvent::SendThreadInfo(pOfflinePVP->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum, MSC_MAIN_TOWER_EVENT, MSC_SUB_DB_TOWER_OFFLINE_DATA, (CT_CHAR*)pOfflinePVP, sizeof(MSC_TowerPVP_ResRobbery));

	CT_DWORD threadid = GAME_THREAD_CITY + pOfflinePVP->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid, MSC_MAIN_TOWER_EVENT, MSC_SUB_DB_TOWER_OFFLINE_DATA, (CT_CHAR*)pOfflinePVP, sizeof(MSC_TowerOfflineUserFightData));
}


CT_BOOL CCDBUserEnterEvent::GetTower(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerGet");		//U_ID, CPID, Treasure, Time
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_TOWER	InsertTower = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertTower.dwU_ID );
				pResult->GetValue(1, InsertTower.iCPID );
				pResult->GetValue(2, InsertTower.iTreasure );
				pResult->GetValue(3, InsertTower.iTime );
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWER,(CT_CHAR*)&InsertTower,sizeof(InsertTower));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetTowerHelp(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerHelpGet");	
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_TowerHelp InsertTowerHelp = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertTowerHelp.dwU_ID );
				pResult->GetValue(1, InsertTowerHelp.dwF_ID );
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERHELP,(CT_CHAR*)&InsertTowerHelp,sizeof(InsertTowerHelp));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetTowerSlotReward(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerSlotRewardGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_TowerSlotReward	InsertTowerSlotReward = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertTowerSlotReward.dwUserID );
				pResult->GetValue(1, InsertTowerSlotReward.iFloor );
				pResult->GetValue(2, InsertTowerSlotReward.iSlot );
				pResult->GetValue(3, InsertTowerSlotReward.mnReward.iGetResType1);
				pResult->GetValue(4, InsertTowerSlotReward.mnReward.iGetResNum1);
				pResult->GetValue(5, InsertTowerSlotReward.mnReward.iGetResType2);
				pResult->GetValue(6, InsertTowerSlotReward.mnReward.iGetResNum2 );
				pResult->GetValue(7, InsertTowerSlotReward.mnReward.iGetResType3);
				pResult->GetValue(8, InsertTowerSlotReward.mnReward.iGetResNum3 );
				pResult->GetValue(9, InsertTowerSlotReward.mnReward.iGetResType4);
				pResult->GetValue(10, InsertTowerSlotReward.mnReward.iGetResNum4 );
				pResult->GetValue(11, InsertTowerSlotReward.mnReward.iGetItemID1 );
				pResult->GetValue(12, InsertTowerSlotReward.mnReward.iGetItemNum1);
				pResult->GetValue(13, InsertTowerSlotReward.mnReward.iGetItemID2);
				pResult->GetValue(14, InsertTowerSlotReward.mnReward.iGetItemNum2);
				pResult->GetValue(15, InsertTowerSlotReward.mnReward.iGetItemGroupID1);
				pResult->GetValue(16, InsertTowerSlotReward.mnReward.iGetItemGroupNum1);
				pResult->GetValue(17, InsertTowerSlotReward.mnReward.iGetItemGroupID2);
				pResult->GetValue(18, InsertTowerSlotReward.mnReward.iGetItemGroupNum2);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERSLOTREWARD,(CT_CHAR*)&InsertTowerSlotReward,sizeof(InsertTowerSlotReward));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetTowerEvent(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerEventGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	TowerEvent	InsertTowerEvent = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertTowerEvent.dwUserID );
				pResult->GetValue(1, InsertTowerEvent.iEventType );
				pResult->GetValue(2, InsertTowerEvent.iFoor );
				pResult->GetValue(3, InsertTowerEvent.dwOID);
				pResult->GetValue(4, InsertTowerEvent.iFightingID);
				pResult->GetValue(5, InsertTowerEvent.cOName, NAME_SIZE);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWEREVENT,(CT_CHAR*)&InsertTowerEvent,sizeof(TowerEvent));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetTowerSlotFightRecord(const MSC_DB_UserEnter *pTUE)
{
	
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TowerSlotFightExampleGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	std::list<MSC_TowerPVP_ResRobbery> ResRobberyList;
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			MSC_TowerPVP_ResRobbery FightingInfo={0};
			CT_INT64 iFID = 0;
			CT_INT64 iFID_New = 0;
			CT_DWORD dwUID=0;
			CT_DWORD dwMID=0;
			CT_INT32 iSeed=0;
			CT_INT32 iScore=0;
			CT_INT32 iPositionMax = POST_COUNT/2;
			if (pResult->MoveNextRow())
			{
				while (CT_TRUE)
				{
					CT_INT32 iIndex=0;
					CombatUnitData data={0};
					pResult->GetValue(iIndex++, iFID_New);
					pResult->GetValue(iIndex++, dwMID);
					//pResult->GetValue(iIndex++, dwUID);
					//pResult->GetValue(iIndex++, data.name,sizeof(data.name) );
					//pResult->GetValue(iIndex++, data.mcs.fHPPersent);
					//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
					//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
					//pResult->GetValue(iIndex++, iSeed);
					//pResult->GetValue(iIndex++, data.level);
					//pResult->GetValue(iIndex++, data.type);
					//pResult->GetValue(iIndex++, iScore);
					//pResult->GetValue(iIndex++, data.mcs.iMaxHP );
					//pResult->GetValue(iIndex++, data.mcs.iMaxMP );
					//pResult->GetValue(iIndex++, data.mcs.iStrength );
					//pResult->GetValue(iIndex++, data.mcs.iAgility );
					//pResult->GetValue(iIndex++, data.mcs.iIntellect);
					//pResult->GetValue(iIndex++, data.mcs.iSpirit );
					//pResult->GetValue(iIndex++, data.mcs.fHitRate );
					//pResult->GetValue(iIndex++, data.mcs.fDodgeRate );
					//pResult->GetValue(iIndex++, data.mcs.fCritRate );
					//pResult->GetValue(iIndex++, data.mcs.fResilient );
					//pResult->GetValue(iIndex++, data.mcs.fDmgIncPercent );
					//pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
					//pResult->GetValue(iIndex++, data.mcs.iAttack );
					//pResult->GetValue(iIndex++, data.mcs.iDefence );
					//pResult->GetValue(iIndex++, data.position);
					//pResult->GetValue(iIndex++, data.Skills[0].iID);
					//pResult->GetValue(iIndex++, data.Skills[1].iID);
					//pResult->GetValue(iIndex++, data.Skills[2].iID);
					//pResult->GetValue(iIndex++, data.Skills[3].iID);
					//pResult->GetValue(iIndex++, data.mcs.iHPRecover);
					//pResult->GetValue(iIndex++, data.mcs.iMPRecover);

					G_GetFightingRecord(pResult,data,iScore,iSeed,iIndex,dwUID);

					if (iFID !=0 && iFID != iFID_New)	//新的战斗
					{
						FightingInfo.iFID = iFID;
						//CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
						//G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERSLOTFIGHTING,(CT_CHAR*)&FightingInfo,sizeof(MSC_TowerPVP_ResRobbery));
						ResRobberyList.push_back(FightingInfo);
						memset(&FightingInfo,0, sizeof(MSC_TowerPVP_ResRobbery));
					}

					if (data.type < 10)
					{
						if (data.position < iPositionMax )
						{
							FightingInfo.LeftFightingData.dwUserID = dwMID;
						}
						else
						{
							FightingInfo.RightFightingData.dwUserID = dwMID;
						}
					}

					FightingInfo.dwUserID = dwUID;
					if (data.position > POST_COUNT)
					{
						data.position = data.position % POST_COUNT;
					}
					if (data.position >= iPositionMax)
					{
						data.position = data.position - iPositionMax;
						FightingInfo.RightFightingData.units[data.position%iPositionMax] = data;
					}
					else
					{
						FightingInfo.LeftFightingData.units[data.position%iPositionMax] = data;
					}
					FightingInfo.dwUserID = dwUID;
					FightingInfo.iSeed = iSeed;
					iFID = iFID_New;

					if (!pResult->MoveNextRow())
					{
						FightingInfo.iFID = iFID;
						//CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
						//G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERSLOTFIGHTING,(CT_CHAR*)&FightingInfo,sizeof(MSC_TowerPVP_ResRobbery));

						ResRobberyList.push_back(FightingInfo);
						break;
					}
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	std::list<MSC_TowerPVP_ResRobbery>::iterator itend=ResRobberyList.end();
	for(std::list<MSC_TowerPVP_ResRobbery>::iterator it=ResRobberyList.begin();it != itend;++it)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_TowerSlotFightSkillGet");
		m_dbConnector.AddParm("_fid", (*it).iFID );
		m_dbConnector.AddParm("_uid", (*it).dwUserID );

		MSC_DB_MSG	InsertMSG = {0};
		if(m_dbConnector.Exec())
		{
			CT_INT32 iNum=0;
			CT_INT32 iPos1=0;
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					CT_INT32 iSkillID=0;
					CT_INT32 iPos2=0;
					pResult->GetValue(0,iSkillID);
					pResult->GetValue(1,iPos2);

					if(iPos1 != iPos2)
					{
						iNum=0;
						iPos1=iPos2;
					}

					if(iPos2 < POST_COUNT/2)
					{
						(*it).LeftFightingData.units[iPos2%(POST_COUNT/2)].SkillEnuip.iSkillEquip[iNum%SKILL_ENUIP_COUNT]=iSkillID;
					}
					else
					{
						(*it).RightFightingData.units[iPos2%(POST_COUNT/2)].SkillEnuip.iSkillEquip[iNum%SKILL_ENUIP_COUNT]=iSkillID;
					}

					iNum++;
				}
			}
			pResult->Release();
		}else
		{
			G_DBError(m_dbConnector, m_dwThread);
			return CT_FALSE;
		}

		MSC_TowerPVP_ResRobbery FightingInfo={0};
		memcpy(&FightingInfo,&(*it),sizeof(FightingInfo));
		CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TOWERSLOTFIGHTING,(CT_CHAR*)&FightingInfo,sizeof(MSC_TowerPVP_ResRobbery));

	}
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetMSG(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MSGGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_MSG	InsertMSG = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, InsertMSG.dwUserID );
				pResult->GetValue(1, InsertMSG.iMSGType );
				pResult->GetValue(2, InsertMSG.iTime );
				pResult->GetValue(3, InsertMSG.cMSG, 255);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MSG,(CT_CHAR*)&InsertMSG,sizeof(InsertMSG));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCDBUserEnterEvent::GetBossRecord(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_BossRecordGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID );

	MSC_DB_WorldBossRecord	dbReocrd = {0};
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, dbReocrd.dwUID);
				pResult->GetValue(1, dbReocrd.iAEID);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_BOSS_RECORD,(CT_CHAR*)&dbReocrd,sizeof(MSC_DB_WorldBossRecord));					
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;

}

//战队个人科技
CT_BOOL CCDBUserEnterEvent::GetFightTeamScience(const MSC_DB_UserEnter *pTUE)
{
    ScienceInfoMap scienceMap;
    if (ReadDBData(pTUE->dwUserID, scienceMap))
    {
        for (ScienceInfoMap::const_iterator it = scienceMap.begin(); it != scienceMap.end(); ++it)
        {
            MSC_DB_InsterFightTeamScience FTS={0};
            FTS.dwUserID = it->second.dwUserID;
            FTS.iScienceID = it->second.iScienceID;
            FTS.iGrade = it->second.iScienceGrade;            
            CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
            G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_FIGHTTEAM_SCIENCE,(CT_CHAR*)&FTS,sizeof(FTS));
        }
        return CT_TRUE;
    }
    return CT_FALSE;
}

// 获取头衔信息
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserTitle::DataMap& titleMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetTitle");
	m_dbConnector.AddParm("_uid", dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult *pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			CT_INT32 iCurrent = time(0);
			while (pResult->MoveNextRow())
			{
				TitleData titleData = {0};
				CT_INT32 iStatus = 0;

				pResult->GetValue(0, titleData.iID);
				pResult->GetValue(1, iStatus);
				pResult->GetValue(2, titleData.iActiveTime);
				pResult->GetValue(3, titleData.iEffectiveTime);

				if ((iStatus & 3))
				{
					if (iStatus == 2)
					{
						titleData.bIsUse = CT_TRUE;
					}
					titleMap.insert(CCUserTitle::DataMap::value_type(titleData.iID, titleData));
				}
			}
		}
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetTitle","_uid:%d\n%s%d",dwUserID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	
	return CT_TRUE;
}

// 得到探访军队
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CT_INT32 *pSeekArray)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetWallArmy");
	m_dbConnector.AddParm("_uid", dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				//pResult->GetValue(0, iwa.dwUserID);
				pResult->GetValue(1, pSeekArray[0]);
				pResult->GetValue(2, pSeekArray[1]);
				pResult->GetValue(3, pSeekArray[2]);
				pResult->GetValue(4, pSeekArray[3]);
				pResult->GetValue(5, pSeekArray[4]);
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

// 得到探访城墙加成
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, WallAdditionMap &wallAdditionMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetWallAddition");
	m_dbConnector.AddParm("_uid", dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				WallAddition wa={0};
				//pResult->GetValue(0, iwa.dwUserID);
				pResult->GetValue(1, wa.iID);
				pResult->GetValue(2, wa.iGrade);

				
				wallAdditionMap.insert(WallAdditionMap::value_type(wa.iID,wa));
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

// 得到战斗加成
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID,  CCUserBase::MasterFightAddMap &mfaMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_MasterFightGet");

	m_dbConnector.AddParm("_uid",  dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				MasterFightAdd mfa={0};
				pResult->GetValue(0, mfa.dwUserID);
				pResult->GetValue(1, mfa.iType);
				pResult->GetValue(2,mfa.mcs.iMaxHP);			
				pResult->GetValue(3,mfa.mcs.iMaxMP);			
				pResult->GetValue(4,mfa.mcs.iStrength);		
				pResult->GetValue(5,mfa.mcs.iAgility);		
				pResult->GetValue(6,mfa.mcs.iIntellect);		
				pResult->GetValue(7,mfa.mcs.iSpirit);		
				pResult->GetValue(8,mfa.mcs.fHitRate);		
				pResult->GetValue(9,mfa.mcs.fDodgeRate);		
				pResult->GetValue(10,mfa.mcs.fCritRate);		
				pResult->GetValue(11,mfa.mcs.fResilient);		
				pResult->GetValue(12,mfa.mcs.fDmgIncPercent);	
				pResult->GetValue(13, mfa.mcs.fDmgDecPercent);	
				pResult->GetValue(14,mfa.mcs.iAttack);		
				pResult->GetValue(15,mfa.mcs.iDefence);		
				pResult->GetValue(16,mfa.mcs.fSpeed);

				mfaMap.insert(CCUserBase::MasterFightAddMap::value_type(mfa.iType,mfa));

			}
		}

		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

// 得到宠物幻化
CT_BOOL CCDBUserEnterEvent::ReadDBData(const CT_DWORD dwUserID, CCUserPet::PetChangeMap &pcMap)
{
	//宠物幻化
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetChangeGet");
	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				PetChange pc={0};
				pResult->GetValue(0, pc.iChangeID );
				pResult->GetValue(1, pc.iSkillID );

				pcMap.insert(CCUserPet::PetChangeMap::value_type(pc.iChangeID,pc));
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	return CT_TRUE;
}

// 得到头衔信息
CT_BOOL CCDBUserEnterEvent::GetTitleInfo(const MSC_DB_UserEnter *pTUE)
{
	CCUserTitle::DataMap titleMap;
	if (ReadDBData(pTUE->dwUserID, titleMap))
	{
		for (CCUserTitle::DataMap_IT it = titleMap.begin(); it != titleMap.end(); ++it)
		{
			MSC_DB_TitleData DIT={0};
			DIT.dwUserID = pTUE->dwUserID;
			DIT.iTitleID = it->second.iID;
			DIT.iActiveTime = it->second.iActiveTime;
			DIT.iEffectTime = it->second.iEffectiveTime;
			it->second.bIsUse? DIT.iStatus = TITLE_USE:DIT.iStatus = TITLE_ACTIVE;       
			CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_TITLE,(CT_CHAR*)&DIT,sizeof(DIT));
		}

		return CT_TRUE;
	}
	return CT_FALSE;
}

// 得到龙岛信息
CT_BOOL CCDBUserEnterEvent::GetDragonIslandData(const MSC_DB_UserEnter *pTUE)
{
	//读取龙岛基本信息
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetDragonIsland");
	m_dbConnector.AddParm("_uid",	pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				MSC_DB_IslandData mid = {0};
				pResult->GetValue(1, mid.iPoint);
				pResult->GetValue(2, mid.iEventID);
				pResult->GetValue(3, mid.iEventStatus);
				pResult->GetValue(4, mid.iThrowTimes);
				pResult->GetValue(5, mid.iStartTimes);
				pResult->GetValue(6, mid.iStartChest);
				pResult->GetValue(7, mid.iGetTimes);
				pResult->GetValue(8, mid.iFT_ID);
				pResult->GetValue(9, mid.iScore);
				pResult->GetValue(10,mid.iIsOpen);
				pResult->GetValue(11,mid.iRewardTimes);
				pResult->GetValue(12,mid.iStep);

				mid.dwUserID = pTUE->dwUserID;
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLANDDATA,(CT_CHAR*)&mid,sizeof(mid));
			}
		}
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetDragonIsland","_uid:%d\n%s%d",pTUE->dwUserID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 读取已赠送次数的uid
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetIslandGiveUid");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				MSC_DB_IslandGiveUid mgu = {0};
				mgu.dwUserID = pTUE->dwUserID;
				pResult->GetValue(0, mgu.aGiveUid);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLAND_GIVEUID,(CT_CHAR*)&mgu,sizeof(mgu));
			}
		}
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetIslandGiveUid","_uid:%d\n%s%d",pTUE->dwUserID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 读取已经获得的战队积分宝箱
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetIslandScoreReward");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				MSC_DB_IslandScoreReward msr = {0};
				msr.dwUserID = pTUE->dwUserID;
				pResult->GetValue(0, msr.aScoreReward);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLAND_SCOREREWARD,(CT_CHAR*)&msr,sizeof(msr));
			}
		}
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetIslandScoreReward","_uid:%d\n%s%d",pTUE->dwUserID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 读取赠送过次数的玩家昵称
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetPresentName");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				MSC_DB_IslandPresentUserName mpun = {0};
				mpun.dwUserID = pTUE->dwUserID;
				pResult->GetValue(0, mpun.cUserName,NAME_SIZE);
				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_ISLAND_PRESENTNAME,(CT_CHAR*)&mpun,sizeof(mpun));
			}
		}
		pResult->Release();
	}
	else
	{
		CCDBLogMes::getSingleton().WriteInfo(pTUE->dwUserID,"SP_GetPresentName","_uid:%d\n%s%d",pTUE->dwUserID,__FILE__,__LINE__);
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//获取城墙技能
CT_BOOL CCDBUserEnterEvent::GetWallAddition(const MSC_DB_UserEnter *pTUE)
{
	WallAdditionMap wamap;
	if(ReadDBData(pTUE->dwUserID, wamap))
	{
		for(WallAdditionMap::iterator it=wamap.begin();it != wamap.end();++it)
		{
			MSC_DB_InsertWallAddition iwa={0};
			iwa.dwUserID=pTUE->dwUserID;
			iwa.iID=(*it).second.iID;
			iwa.iGrade=(*it).second.iGrade;
			G_SendCityThreadInfo(pTUE->dwUserID,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WALLADDITION,(CT_CHAR*)&iwa,sizeof(iwa));
		}
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获取城墙军队
CT_BOOL CCDBUserEnterEvent::GetWallArmy(const MSC_DB_UserEnter *pTUE)
{
	MSC_DB_InsertWallArmy iwa = {0};
	CT_INT32 iPos[POST_COUNT/2]={0};
	if (ReadDBData(pTUE->dwUserID, iPos))
	{
		iwa.dwUserID=pTUE->dwUserID;
		memcpy(iwa.iPos,iPos,sizeof(iwa.iPos));
		G_SendCityThreadInfo(iwa.dwUserID,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WALLARMY,(CT_CHAR*)&iwa,sizeof(iwa));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//可探访者信息读取
CT_VOID CCDBUserEnterEvent::GetSeekerInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(MSC_DB_SeekedGet))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetSeekerInfo len != sizeof(MSC_DB_SeekedGet)");
		return;
	}

	MSC_DB_SeekedGet* pGet = (MSC_DB_SeekedGet*)pBuf;
	MSC_DB_Seeked_Info seeked;
	memset(&seeked,0,sizeof(seeked));
	seeked.dwUserID = pGet->dwFUserID;

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekDateGet");
	m_dbConnector.AddParm("_uid", pGet->dwFUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(3, seeked.iSeekedWillTimes);
				pResult->GetValue(4, seeked.iSeekedSpiteTimes);	
				pResult->GetValue(6, seeked.iKidnapSaintess);
				pResult->GetValue(7, seeked.iKidnapWorkerNum);

				pGet->iSeekedWillTimes = seeked.iSeekedWillTimes;
				pGet->iSeekedSpiteTimes = seeked.iSeekedSpiteTimes;
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;
	}

	// 读取礼品信息
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekedPresentGet");
	m_dbConnector.AddParm("_uid", pGet->dwFUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			CT_INT32 iGetTime = 0;
			CT_INT32 iNow = time(0);
			G_ValueRange vr = {0};
			G_GetValueRange(VALUE_TYPE_UPDATEPRESENT_TIME, &vr);
			for (CT_INT32 i = 0;pResult->MoveNextRow() && i < SEEKED_PRESENT_NUM;)
			{
				pResult->GetValue(2, iGetTime);
				if (iNow - iGetTime < vr.fMaxVal)
				{                             
					pResult->GetValue(1, seeked.present[i].iChestID);
					seeked.present[i++].iGetTime = iGetTime;
				}
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return ;
	}

	// 读取战斗力数据
	UserFightingStatus fightingInfo = {0};
	LookUserInfo lookUserInfo = {0};
	GetNoLinkUserInfo(&(pGet->dwFUserID), &fightingInfo, &lookUserInfo,1);

	// 读取城镇信息
	MSC_DB_EnterOtherTown otherTown = {0};
	otherTown.dwUserID = pGet->dwUserID;
	otherTown.dwFUserID = pGet->dwFUserID;
	GetOtherTownInfo(&otherTown);

	seeked.iDiamond = otherTown.iDiamond;
	seeked.iMoney = otherTown.iMoney;
	seeked.iFood = otherTown.iFood;
	seeked.iWorker = otherTown.iWorker;
	seeked.iMaxWorker = otherTown.iMaxWorker;
	seeked.iKidnapSaintess = otherTown.iSaintess;
	memcpy(seeked.teamFlag, otherTown.aTeam, sizeof(seeked.teamFlag));
	memcpy(seeked.buildingData, otherTown.building, sizeof(seeked.buildingData));
	memcpy(seeked.productBuilding, otherTown.productBuilding, sizeof(seeked.productBuilding));
	memcpy(seeked.upgradeBuilding, otherTown.upgradeBuilding, sizeof(seeked.upgradeBuilding));

	CT_DWORD threadid = GAME_THREAD_CITY;
	G_SendThreadInfo(threadid,MAIN_G_USER_DATA,MSC_SUB_INSERT_SEEKED_PLAYERINFO,(CT_CHAR*)&seeked,sizeof(seeked));
	
	MSC_DB_OtherSeekedInfo seekedInfo = {0};
	seekedInfo.seekInfo = *pGet;
	seekedInfo.townInfo = otherTown;

	threadid = GAME_THREAD_CITY + pGet->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_SEEK, MSC_SUB_GET_SEEKED_RETURN,(CT_CHAR*)&seekedInfo,sizeof(MSC_DB_OtherSeekedInfo));
	return;
}

//获取可随机探访者列表
CT_BOOL CCDBUserEnterEvent::GetSeekRandList(const MSC_DB_UserEnter *pTUE)
{
	// 得到符合条件的随机玩家
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekRandGet");
	m_dbConnector.AddParm("_uid",pTUE->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				MSC_Seek_FriendGet fg = {0};
				fg.dwUserID = pTUE->dwUserID;
				pResult->GetValue(0, fg.dwFUserID );
				pResult->GetValue(1, fg.cUserName, sizeof(fg.cUserName));
				pResult->GetValue(2, fg.iLevel);
				pResult->GetValue(3, fg.iOccupation);

				CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_RANDPLAYER,(CT_CHAR*)&fg,sizeof(fg));
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;
}

//获取已探访的人的列表
CT_BOOL CCDBUserEnterEvent::GetHadSeekedList(const MSC_DB_UserEnter *pTUE)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekPlayerGet");
	m_dbConnector.AddParm("_uid",pTUE->dwUserID);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			MSC_DB_Had_SeekedPlayer player = {0};
			player.dwUserID = pTUE->dwUserID;
			CT_INT32 iStatus = 0;
			while (pResult->MoveNextRow())
			{
				 
				pResult->GetValue(0, player.dwFUserID);
				pResult->GetValue(1, player.iType);
				pResult->GetValue(2, iStatus);

				if (iStatus == KIDNAP_STATUS_YES)
				{
					CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_HAD_SEEKEDPLAYER,(CT_CHAR*)&player,sizeof(player));
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}
	return CT_TRUE;

}		

//获取探访信息
CT_BOOL CCDBUserEnterEvent::GetSeekInfo(const MSC_DB_UserEnter *pTUE)
{
	TeamSeekMap flagMap;
	KidnapMap saintessMap;
	KidnapMap workerMap;
	KidnapMap petMap;

	CT_DWORD threadid = GAME_THREAD_CITY + pTUE->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;

	// 战旗
	GetTeamFlag(pTUE->dwUserID, flagMap);
	TeamSeekMap::iterator itFlag = flagMap.begin();
	for (;itFlag != flagMap.end();++itFlag)
	{
		MSC_DB_SeekTeamFlag teamFlag = {0};
		teamFlag.dwUserID = pTUE->dwUserID;
		teamFlag.teamFlag = itFlag->second;
		G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_TEAMFLAG,(CT_CHAR*)&teamFlag,sizeof(teamFlag));
	}

	// 人质
	GetKidnapeeInfo(pTUE->dwUserID, saintessMap, workerMap, petMap);
	KidnapMap::iterator itSaintess = saintessMap.begin();
	KidnapMap::iterator itWorker = workerMap.begin();
	KidnapMap::iterator itPet = petMap.begin();

	for (;itSaintess != saintessMap.end();++itSaintess)
	{
		MSC_DB_SeekKidnap saintess = {0};
		saintess.dwUserID = pTUE->dwUserID;
		saintess.kidnap = itSaintess->second;
		G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_SAINTESSKIDNAP,(CT_CHAR*)&saintess,sizeof(saintess));
	}

	for (;itWorker != workerMap.end();++itWorker)
	{
		MSC_DB_SeekKidnap worker = {0};
		worker.dwUserID = pTUE->dwUserID;
		worker.kidnap = itWorker->second;
		G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WORKERKIDNAP,(CT_CHAR*)&worker,sizeof(worker));
	}

	for (;itPet != petMap.end();++itPet)
	{
		MSC_DB_SeekKidnap pet = {0};
		pet.dwUserID = pTUE->dwUserID;
		pet.kidnap = itPet->second;
		G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_PETKIDNAP,(CT_CHAR*)&pet,sizeof(pet));
	}

	// 探访基本信息
	MSC_DB_SeekData seekData = {0};
	seekData.dwUserID = pTUE->dwUserID;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekDateGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, seekData.seekData.iSeekWillTimes);
				pResult->GetValue(2, seekData.seekData.iSeekSpiteTimes);
				pResult->GetValue(3, seekData.seekData.iSeekedWillTimes);
				pResult->GetValue(4, seekData.seekData.iSeekedSpiteTimes);
				pResult->GetValue(5, seekData.seekData.iRescueNum);
				pResult->GetValue(6, seekData.seekData.iWomanNum);
				pResult->GetValue(7, seekData.seekData.iFarmarNum);
				pResult->GetValue(8, seekData.seekData.iAddSpiteTimes);
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 被绑架的宠物ID
	CT_INT32 iSize = sizeof(seekData.seekData.iPet);
	GetKidnapPet(seekData.seekData.iPet, iSize, pTUE->dwUserID);

	// 已得到礼品信息
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekedPresentGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			CT_INT32 iTime = 0;
			CT_INT32 iNow = time(0);
			G_ValueRange vr = {0};
			if (!G_GetValueRange(VALUE_TYPE_UPDATEPRESENT_TIME,&vr))
			{
				CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetSeekInfo G_GetValueRange error.value:%d",VALUE_TYPE_UPDATEPRESENT_TIME);
			}
			for (CT_INT32 iIndex = 0;pResult->MoveNextRow() && iIndex < SEEKED_PRESENT_NUM;++iIndex)
			{
				pResult->GetValue(2, iTime);
				if (iNow - iTime < vr.fMaxVal)
				{
					pResult->GetValue(1, seekData.seekData.present[iIndex].iChestID);
					seekData.seekData.present[iIndex].iGetTime = iTime;
				}
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_DATA,(CT_CHAR*)&seekData,sizeof(seekData));

	// 探访界面列表
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekPlayerListGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			CT_INT32 iType = 0;
			while (pResult->MoveNextRow())
			{
				MSC_DB_SeekListPlayer player = {0};
				player.dwUserID = pTUE->dwUserID;
				pResult->GetValue(0, player.playerInfo.dwUserID);
				pResult->GetValue(1, iType);
				pResult->GetValue(2, player.playerInfo.iStatus);
				pResult->GetValue(3, player.playerInfo.cUserName, NAME_SIZE);
				pResult->GetValue(4, player.playerInfo.iLevel);
				pResult->GetValue(5, player.playerInfo.iOccupation);

				if (iType == SEEK_GOODWILL)
				{
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_WILLLIST,(CT_CHAR*)&player,sizeof(player));
				}
				else
				{
					G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_SPITELIST,(CT_CHAR*)&player,sizeof(player));
				}
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 探访记录
	std::list<std::string> IndexList;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekVisitorInfoGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			CT_INT32 iType = 0;
			while (pResult->MoveNextRow())
			{
				MSC_DB_SeekVistorInfo visitor = {0};
				CT_INT32 iVisitorType = 0;
				visitor.dwUserID = pTUE->dwUserID;
				pResult->GetValue(0, visitor.vistorInfo.dwUserID);
				pResult->GetValue(1, visitor.vistorInfo.cUserName, NAME_SIZE);
				pResult->GetValue(2, visitor.vistorInfo.seekInfo.iType);
				pResult->GetValue(3, visitor.vistorInfo.seekInfo.iID[0]);
				pResult->GetValue(4, visitor.vistorInfo.seekInfo.iNum[0]);
				pResult->GetValue(5, visitor.vistorInfo.seekInfo.iID[1]);
				pResult->GetValue(6, visitor.vistorInfo.seekInfo.iNum[1]);
				pResult->GetValue(7, visitor.vistorInfo.seekInfo.iID[2]);
				pResult->GetValue(8, visitor.vistorInfo.seekInfo.iNum[2]);
				pResult->GetValue(9, visitor.vistorInfo.seekInfo.iID[3]);
				pResult->GetValue(10, visitor.vistorInfo.seekInfo.iNum[3]);
				pResult->GetValue(11, visitor.vistorInfo.present[0]);
				pResult->GetValue(12, visitor.vistorInfo.present[1]);
				pResult->GetValue(13, visitor.vistorInfo.present[2]);
				pResult->GetValue(14, visitor.vistorInfo.bIsKidnap);
				pResult->GetValue(15, visitor.vistorInfo.iPlunderBID[0]);
				pResult->GetValue(16, visitor.vistorInfo.iPlunderBID[1]);
				pResult->GetValue(17, visitor.vistorInfo.iPlunderBID[2]);
				pResult->GetValue(18, visitor.vistorInfo.iOccupation);
				pResult->GetValue(19, visitor.vistorInfo.iLevel);

				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_VISITORINFO,(CT_CHAR*)&visitor,sizeof(visitor));

				iVisitorType = visitor.vistorInfo.seekInfo.iType % 100;
				if(iVisitorType == SEEK_TYPE_KIDNAP || iVisitorType == SEEK_TYPE_PLUNDER)
				{
					CT_UCHAR ucIndex[SEEK_FIGHT_RECORD_INDEX+1]={0};
					CCSeekEvent::MD5FightIndex(ucIndex,visitor.vistorInfo.dwUserID,pTUE->dwUserID,visitor.vistorInfo.seekInfo.iType);
					IndexList.push_back(std::string((CT_CHAR*)ucIndex));
				}
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	// 探访别人的信息
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_SeekOthersInfoGet");
	m_dbConnector.AddParm("_uid", pTUE->dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{
			CT_INT32 iType = 0;
			while (pResult->MoveNextRow())
			{
				MSC_DB_SeekVistorInfo visitor = {0};
				CT_INT32 iVisitorType = 0;
				visitor.dwUserID = pTUE->dwUserID;
				pResult->GetValue(0, visitor.vistorInfo.dwUserID);
				pResult->GetValue(1, visitor.vistorInfo.cUserName, NAME_SIZE);
				pResult->GetValue(2, visitor.vistorInfo.seekInfo.iType);
				pResult->GetValue(3, visitor.vistorInfo.seekInfo.iID[0]);
				pResult->GetValue(4, visitor.vistorInfo.seekInfo.iNum[0]);
				pResult->GetValue(5, visitor.vistorInfo.seekInfo.iID[1]);
				pResult->GetValue(6, visitor.vistorInfo.seekInfo.iNum[1]);
				pResult->GetValue(7, visitor.vistorInfo.seekInfo.iID[2]);
				pResult->GetValue(8, visitor.vistorInfo.seekInfo.iNum[2]);
				pResult->GetValue(9, visitor.vistorInfo.seekInfo.iID[3]);
				pResult->GetValue(10, visitor.vistorInfo.seekInfo.iNum[3]);
				pResult->GetValue(11, visitor.vistorInfo.present[0]);
				pResult->GetValue(12, visitor.vistorInfo.present[1]);
				pResult->GetValue(13, visitor.vistorInfo.present[2]);
				pResult->GetValue(14, visitor.vistorInfo.bIsKidnap);
				pResult->GetValue(15, visitor.vistorInfo.iPlunderBID[0]);
				pResult->GetValue(16, visitor.vistorInfo.iPlunderBID[1]);
				pResult->GetValue(17, visitor.vistorInfo.iPlunderBID[2]);
				pResult->GetValue(18, visitor.vistorInfo.iOccupation);
				pResult->GetValue(19, visitor.vistorInfo.iLevel);

				G_SendThreadInfo(threadid,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_OTHERSINFO,(CT_CHAR*)&visitor,sizeof(visitor));
			}
		}
		pResult->Release();
	}
	else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}

	for(std::list<std::string>::iterator it=IndexList.begin();it != IndexList.end();++it)
	{
		GetSeekFightRecord(pTUE,(*it));
	}

	return CT_TRUE;
}

//获取探访战报
CT_BOOL CCDBUserEnterEvent::GetSeekFightRecord(const MSC_DB_UserEnter *pTUE,std::string strIndex)
{
	std::map<std::string,MSC_DB_SeekFightRecord> sfeMap;
	m_dbConnector.Reset();	
	m_dbConnector.SetPcName("SP_SeekGetFightExample");
	m_dbConnector.AddParm("_Index", strIndex);

	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			CT_INT32 iRival=0;
			while (pResult->MoveNextRow())
			{
				CT_INT32 iIndex=0;
				CT_DWORD dwUserID=0;
				CT_INT32 iScore=0;
				CombatUnitData data={0};
				MSC_DB_SeekFightRecord sfr={0};
				memcpy(sfr.ucIndex,strIndex.c_str(),SEEK_FIGHT_RECORD_INDEX);

				iIndex++;
				//pResult->GetValue(iIndex++, sfr.ucIndex,SEEK_FIGHT_RECORD_INDEX);
				G_GetFightingRecord(pResult,data,iScore,sfr.iSeed,iIndex,dwUserID);

				CT_DWORD dwType=0;
				sfr.iType=dwType;
				sfr.dwUserID=pTUE->dwUserID;
				std::map<std::string,MSC_DB_SeekFightRecord>::iterator it=sfeMap.find(strIndex);
				if(it !=sfeMap.end())
				{
					if(pTUE->dwUserID == dwUserID)
					{
						(*it).second.RightFightingData.dwUserID=dwUserID;
						(*it).second.RightFightingData.units[data.position%(POST_COUNT/2)]=data;
					}
					else
					{
						(*it).second.LeftFightingData.dwUserID=dwUserID;
						(*it).second.LeftFightingData.units[data.position%(POST_COUNT/2)]=data;
					}
				}
				else
				{
					if(pTUE->dwUserID == dwUserID)
					{
						sfr.RightFightingData.dwUserID=dwUserID;
						sfr.RightFightingData.units[data.position%(POST_COUNT/2)]=data;
					}
					else
					{
						sfr.LeftFightingData.dwUserID=dwUserID;
						sfr.LeftFightingData.units[data.position%(POST_COUNT/2)]=data;
					}
					sfeMap.insert(std::map<std::string,MSC_DB_SeekFightRecord>::value_type(strIndex,sfr));
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
		return CT_FALSE;
	}


	for(std::map<std::string,MSC_DB_SeekFightRecord>::iterator it=sfeMap.begin();it != sfeMap.end();++it)
	{
		m_dbConnector.Reset();
		m_dbConnector.SetPcName("SP_SeekGetFightSkill");
		m_dbConnector.AddParm("_index", (*it).second.ucIndex);

		if(m_dbConnector.Exec())
		{
			CT_INT32 iNum=0;
			CT_INT32 iPos1=0;
			CCDBResult* pResult = m_dbConnector.GetResult();
			if (pResult->GetResult())
			{  
				while (pResult->MoveNextRow())
				{
					CT_INT32 iSkillID=0;
					CT_INT32 iPos2=0;
					CT_DWORD dwUserID=0;

					pResult->GetValue(0,dwUserID);
					pResult->GetValue(1,iSkillID);
					pResult->GetValue(2,iPos2);
					if(iPos1 != iPos2)
					{
						iNum=0;
						iPos1=iPos2;
					}

					if(pTUE->dwUserID == dwUserID)
					{
						(*it).second.RightFightingData.units[iPos2%(POST_COUNT/2)].SkillEnuip.iSkillEquip[iNum%SKILL_ENUIP_COUNT]=iSkillID;
					}
					else
					{
						(*it).second.LeftFightingData.units[iPos2%(POST_COUNT/2)].SkillEnuip.iSkillEquip[iNum%SKILL_ENUIP_COUNT]=iSkillID;
					}
					iNum++;

				}
			}
			pResult->Release();
		}else
		{
			G_DBError(m_dbConnector, m_dwThread);
			return CT_FALSE;
		}
	}

	for(std::map<std::string,MSC_DB_SeekFightRecord>::iterator it=sfeMap.begin();it != sfeMap.end();++it)
	{
		G_SendCityThreadInfo(pTUE->dwUserID,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_SEEK_FIGHTRECORD,(CT_CHAR*)&(*it).second,sizeof(MSC_DB_SeekFightRecord));
	}
	return CT_TRUE;
}

// 得到建筑信息
CT_VOID CCDBUserEnterEvent::GetBuildingDate(CT_DWORD dwUserID, CCUserBuilding::BuildingDataMap& buildingMap)
{
	BuildingData  buildData;
	memset(&buildData, 0, sizeof(buildData));

	CT_INT32 iRet = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetBuildingtData");
	m_dbConnector.AddParm("_uid",  dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(0, buildData.iID );
				pResult->GetValue(1,  buildData.iGrade);

				buildingMap.insert(CCUserBuilding::BuildingDataMap::value_type(buildData.iID, buildData));
			}
		}
		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetBuildingtData","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);;
	}
}

// 得到建筑生产信息
CT_VOID CCDBUserEnterEvent::GetBuildingProduct(CT_DWORD dwUserID, CCUserBuilding::ProductingtManualBuildingMap& productBuildingMap)
{
	ProductingBuilding  data = {0};
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_ProductingBuildingGet");

	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				//pResult->GetValue(0, data.dwUserID );
				pResult->GetValue(1, data.iBType);
				pResult->GetValue(2, data.iBid);
				pResult->GetValue(3, data.llTime);
				pResult->GetValue(4, data.iType);

				pResult->GetValue(5, data.bSendCanHavest);
				pResult->GetValue(6, data.iLastingTime);
				pResult->GetValue(7, data.iWorker);
				pResult->GetValue(8, data.iBGrade);
				pResult->GetValue(9, data.fSeekAddition);
				//G_SendThreadInfo(pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_BUILDING_PRODUCTING, (CT_CHAR*)&data, sizeof(data));

				productBuildingMap.insert(CCUserBuilding::ProductingtManualBuildingMap::value_type(data.iBid, data));
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_ProductingBuildingGet","_uid:%d",dwUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 得到建筑的升级信息
CT_VOID CCDBUserEnterEvent::GetBuildingUpdate(CT_DWORD dwUserID, CCUserBuilding::UpgradeingBuildDataMap& upgradeBuildingMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_GetUpgradingBuildingData");

	m_dbConnector.AddParm("_uid",  dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				UpgradingBuildData upgradeBuild={0};
				pResult->GetValue(0, upgradeBuild.iID);
				pResult->GetValue(1, upgradeBuild.llBeginTime);
				pResult->GetValue(2, upgradeBuild.iNeedTime);
				pResult->GetValue(3, upgradeBuild.buildingUpgrade.iGrade);

				upgradeBuildingMap.insert(CCUserBuilding::UpgradeingBuildDataMap::value_type(upgradeBuild.iID, upgradeBuild));
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_GetUpgradingBuildingData","_uid:%d",dwUserID);
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 得到旗帜信息
CT_VOID CCDBUserEnterEvent::GetTeamFlag(CT_DWORD dwUserID, TeamSeekMap& teamFlagMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TeamFlagGet");

	m_dbConnector.AddParm("_uid",  dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				TeamSeek teamFlag = {0};
				pResult->GetValue(1, teamFlag.cTName, sizeof(teamFlag.cTName));
				pResult->GetValue(2, teamFlag.iFightResult);
				pResult->GetValue(3, teamFlag.dwUserID);
				pResult->GetValue(4, teamFlag.iType);
				teamFlagMap.insert(TeamSeekMap::value_type(Get64Int(teamFlag.dwUserID,teamFlag.iType), teamFlag));
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_TeamFlagGet","_uid:%d",dwUserID);
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 得到人质信息
CT_VOID CCDBUserEnterEvent::GetKidnapeeInfo(CT_DWORD dwUserID, KidnapMap& girlMap, KidnapMap& workerMap, KidnapMap& petMap)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_KidnapeeGet");

	m_dbConnector.AddParm("_uid",  dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				KidnapInfo kidnapInfo = {0};
				CT_INT32 iType = 0;
				pResult->GetValue(1, iType);
				pResult->GetValue(2, kidnapInfo.owerUserId);
				pResult->GetValue(3, kidnapInfo.Num);

				if (iType == SEEK_KIDNAPTYPE_SAINTESS)
				{
					girlMap.insert(KidnapMap::value_type(kidnapInfo.owerUserId, kidnapInfo));
				}
				else if (iType == SEEK_KIDNAPTYPE_WORKER)
				{
					workerMap.insert(KidnapMap::value_type(kidnapInfo.owerUserId, kidnapInfo));
				}
				else if (iType == SEEK_KIDNAPTYPE_PET)
				{
					kidnapInfo.iPetChange = kidnapInfo.Num;
					kidnapInfo.Num = 1;
					petMap.insert(KidnapMap::value_type(kidnapInfo.owerUserId, kidnapInfo));
				}
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(dwUserID,"SP_KidnapeeGet","_uid:%d",dwUserID);
		//G_SendThreadDbStandardInfo  (pTUE->dwThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR, pTUE->dwUserID, pTUE->pNetInterface);
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 得到别人的城镇信息
CT_VOID CCDBUserEnterEvent::DisposeGetOtherTown(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_EnterOtherTown))
	{
		CCLogMeg::getSingleton().WriteError("CCDBSeekEvnet::DisposeGetOtherTown iLen != sizeof(MSC_DB_EnterOtherTown)");
		return;
	}

	MSC_DB_EnterOtherTown* pOtherTown = (MSC_DB_EnterOtherTown*)pBuf;

	GetOtherTownInfo(pOtherTown);

	CT_DWORD threadid = GAME_THREAD_CITY + pOtherTown->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_SEEK, MSC_SUB_DB_GET_OTHERSTOWN,(CT_CHAR*)pOtherTown,sizeof(MSC_DB_EnterOtherTown));
}

CT_INT32 CCDBUserEnterEvent::CalMaxWorker(CCUserBuilding::BuildingDataMap buildingDataMap)
{
	CT_INT32 iMaxWorker  = 0;
	CCUserBuilding::BuildingDataMap::iterator it    = buildingDataMap.begin();
	CCUserBuilding::BuildingDataMap::iterator itEnd = buildingDataMap.end();
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
					iMaxWorker += bue.iWorkers;
				}
			}
		}
	}
	return iMaxWorker;
}

// 得到已完成任务
CT_VOID CCDBUserEnterEvent::GetTaskFinish(CT_DWORD dwUserID, CCUserTask::TaskCompleteVector& taskFinishVector)
{
	CT_INT32 iTaskID = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_TaskGetFinish");
	m_dbConnector.AddParm("_uid", dwUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, iTaskID);

				taskFinishVector.push_back(iTaskID);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}
}

// 得到已开启的功能
CT_VOID CCDBUserEnterEvent::GetOpenFunction(CT_INT32* pBuf, CT_INT32& iLen, CT_DWORD dwUserID, CT_INT32 iMaterLevel)
{
	CT_INT32 i=0;
	CCUserTask::TaskCompleteVector taskFinishVector;
	GetTaskFinish(dwUserID, taskFinishVector);

	while(CT_TRUE)
	{
		G_FunctionOpen FunctionOpen={0};
		if (i*sizeof(int) >= iLen)
		{
			return;
		}

		if(!G_GetFunctionOpen(i++,&FunctionOpen))
		{
			iLen = i*sizeof(int);
			return ;
		}

		if (iMaterLevel < FunctionOpen.iOpenGrade)
		{
			continue;
		}

		if(FunctionOpen.iOpenTask == 0)
		{
			pBuf[i++] = FunctionOpen.iOpenType;
			continue;
		}

		CT_INT32 iVector = taskFinishVector.size();
		for (CT_INT32 j =0;j<iVector;j++)
		{
			if (taskFinishVector[j]==FunctionOpen.iOpenTask)
			{
				pBuf[i++] = FunctionOpen.iOpenType;
			}
		}
	}
}

CT_VOID CCDBUserEnterEvent::GetOtherTownInfo(MSC_DB_EnterOtherTown* pOtherTown)
{
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_EnterOtherTownInfo");

	m_dbConnector.AddParm("_uid",  pOtherTown->dwFUserID);
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow())
			{
				pResult->GetValue(1, pOtherTown->cUserName, sizeof(pOtherTown->cUserName));
				pResult->GetValue(2, pOtherTown->iLevel);
				pResult->GetValue(3, pOtherTown->iDiamond);
				pResult->GetValue(4, pOtherTown->iMoney);
				pResult->GetValue(5, pOtherTown->iFood);
				pResult->GetValue(6, pOtherTown->iWorker);
				pResult->GetValue(7, pOtherTown->iStamina);
				pResult->GetValue(8, pOtherTown->iFightingPower);
				pResult->GetValue(9, pOtherTown->iVip);
				pResult->GetValue(10, pOtherTown->iSaintess);
				pResult->GetValue(11, pOtherTown->iOccupation);
			}
		}

		pResult->Release();
	}else
	{
		CCDBLogMes::getSingleton().WriteInfo(pOtherTown->dwFUserID,"SP_EnterOtherTownInfo","_uid:%d",pOtherTown->dwFUserID);
		G_DBError(m_dbConnector, m_dwThread);
	}

	CCUserBuilding::BuildingDataMap buildingMap;
	CCUserBuilding::ProductingtManualBuildingMap productBuildingMap;
	CCUserBuilding::UpgradeingBuildDataMap upgradeBuildingMap;
	CCUserTask::TaskCompleteVector taskFinishVector;
	TeamSeekMap teamFlagMap;
	KidnapMap girlMap;
	KidnapMap workerMap;
	KidnapMap petMap;
	CT_INT32 iPetChangeID[SEEK_KIDNAP_PET] = {0};
	CT_INT32 iSize = sizeof(iPetChangeID);
	CT_INT32 i = 0;
	CT_INT32 iNow = time(0);
	CT_INT32 iLastTime = 0;

	GetBuildingDate(pOtherTown->dwFUserID, buildingMap);
	GetBuildingProduct(pOtherTown->dwFUserID, productBuildingMap);
	GetBuildingUpdate(pOtherTown->dwFUserID, upgradeBuildingMap);
	GetTeamFlag(pOtherTown->dwFUserID, teamFlagMap);
	GetKidnapeeInfo(pOtherTown->dwFUserID, girlMap, workerMap, petMap);
	GetLastPetChangeID(iPetChangeID, iSize, pOtherTown->dwFUserID);

	pOtherTown->iMaxWorker = CalMaxWorker(buildingMap);

	// 建筑信息
	CCUserBuilding::BuildingDataMap::iterator itBuilding = buildingMap.begin();
	for (;itBuilding != buildingMap.end();++itBuilding,++i)
	{
		pOtherTown->building[i].bd.iID= itBuilding->second.iID;
		pOtherTown->building[i].bd.iGrade = itBuilding->second.iGrade;
	}

	// 生产建筑信息
	CCUserBuilding::ProductingtManualBuildingMap::iterator itProduct = productBuildingMap.begin();
	for (i = 0;itProduct != productBuildingMap.end();++itProduct)
	{
		iLastTime = itProduct->second.iLastingTime - (iNow - itProduct->second.llTime);
		if (iLastTime > 0 && itProduct->second.iBType != CITY_TYPE_RESIDENTIAL)
		{
			pOtherTown->productBuilding[i] = itProduct->second;
			++i;
		}
	}

	// 升级建筑信息
	CCUserBuilding::UpgradeingBuildDataMap::iterator itUpgrade = upgradeBuildingMap.begin();
	for (i = 0;itUpgrade != upgradeBuildingMap.end();++itUpgrade)
	{
		iLastTime = itUpgrade->second.iNeedTime - (iNow - itUpgrade->second.llBeginTime);
		if (iLastTime > 0)
		{
			pOtherTown->upgradeBuilding[i] = itUpgrade->second;
			++i;
		}
	}

	// 战旗
	TeamSeekMap::iterator itTeam = teamFlagMap.begin();
	for (i = 0;itTeam != teamFlagMap.end();++itTeam,++i)
	{
		memcpy(&(pOtherTown->aTeam[i]), &(itTeam->second), sizeof(pOtherTown->aTeam[0]));
	}

	// 圣女
	KidnapMap::iterator itGirl = girlMap.begin();
	for (i = 0;itGirl != girlMap.end();++itGirl,++i)
	{
		memcpy(&(pOtherTown->aSaintessInfo[i]), &(itGirl->second), sizeof(pOtherTown->aSaintessInfo[0]));
	}

	// 工人
	KidnapMap::iterator itWorker = workerMap.begin();
	for (i = 0;itWorker != workerMap.end();++itWorker,++i)
	{
		memcpy(&(pOtherTown->aFarmerInfo[i]), &(itWorker->second), sizeof(pOtherTown->aFarmerInfo[0]));
	}

	// 宠物
	KidnapMap::iterator itPet = petMap.begin();
	for (i = 0;itPet != petMap.end();++itPet,++i)
	{
		memcpy(&(pOtherTown->aPetInfo[i]), &(itPet->second), sizeof(pOtherTown->aPetInfo[0]));
	}

	// 获取已开启的功能
	CT_INT32 aOpenFunction[50] = {0};
	CT_INT32 iLen = sizeof(aOpenFunction);
	CT_INT32 iIndex = 0;
	GetOpenFunction(aOpenFunction, iLen, pOtherTown->dwFUserID, pOtherTown->iLevel);
	for (i = 0;i < iLen / sizeof(CT_INT32);++i)
	{
		if (aOpenFunction[i] == FUNCTION_TYPE_BUSINESSMAN
			|| aOpenFunction[i] == FUNCTION_TYPE_SAINTESS
			|| aOpenFunction[i] == FUNCTION_TYPE_IRONSMITH)
		{
			pOtherTown->iOpenFunction[iIndex++] = aOpenFunction[i];
		}
	}

	// 剩余的宠物ID
	for (i = 0;i < iSize / sizeof(CT_INT32);++i)
	{
		pOtherTown->iPetID[i] = iPetChangeID[i];
	}

	return;
}

//获取领主战斗加成
CT_BOOL CCDBUserEnterEvent::GetMasterFightAdd(const MSC_DB_UserEnter *pTUE)
{
	CCUserBase::MasterFightAddMap mfaMap;
	if(ReadDBData(pTUE->dwUserID,mfaMap))
	{
		for(CCUserBase::MasterFightAddMap::iterator it=mfaMap.begin();it != mfaMap.end();++it)
		{
			MSC_DB_MasterFightSaveData mfsd={0};
			memcpy(&mfsd,&((*it).second),sizeof(mfsd));
			G_SendCityThreadInfo(pTUE->dwUserID,MSC_MAIN_INSERT_USERDATA,MSC_SUB_INSERT_MASTGER_FIGHTADD,(CT_CHAR*)&mfsd,sizeof(mfsd));
		}
		return CT_TRUE;
	}
	return CT_TRUE;
}

// 得到被绑架的宠物幻化ID
CT_VOID CCDBUserEnterEvent::GetKidnapPet(CT_INT32* pBuf, CT_INT32& iLen, CT_DWORD dwUserID)
{
	CT_INT32 iIndex = 0;
	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_KidnapPetGet");
	m_dbConnector.AddParm("_uid", dwUserID);

	if (m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow() && iLen >= sizeof(CT_INT32) * (iIndex + 1))
			{
				pResult->GetValue(0, pBuf[iIndex++]);
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	iLen = iIndex * sizeof(CT_INT32);
}

// 得到剩余的宠物幻化ID
CT_VOID CCDBUserEnterEvent::GetLastPetChangeID(CT_INT32* pBuf, CT_INT32& iLen,CT_DWORD dwUserID)
{
	CT_INT32 pChangeID[SEEK_KIDNAP_PET] = {0};
	CT_INT32 iSize = sizeof(pChangeID);
	CT_INT32 iPetChangeID = 0;
	CT_INT32 iNum = 0;
	CT_INT32 iIndex = 0;

	GetKidnapPet(pChangeID, iSize, dwUserID);
	iNum = iSize / sizeof(CT_INT32);

	m_dbConnector.Reset();
	m_dbConnector.SetPcName("SP_PetChangeGet");
	m_dbConnector.AddParm("_uid", dwUserID );
	if(m_dbConnector.Exec())
	{
		CCDBResult* pResult = m_dbConnector.GetResult();
		if (pResult->GetResult())
		{  
			while (pResult->MoveNextRow() && iLen >= sizeof(CT_INT32) * (iIndex + 1))
			{
				pResult->GetValue(0, iPetChangeID);

				CT_INT32 i = 0;
				for (;i < iNum;++i)
				{
					if (iPetChangeID == pChangeID[i])
					{
						break;
					}
				}
				if (i == iNum)
				{
					pBuf[iIndex++] = iPetChangeID;
				}
			}
		}
		pResult->Release();
	}else
	{
		G_DBError(m_dbConnector, m_dwThread);
	}

	iLen = iIndex * sizeof(CT_INT32);
}

CT_INT64 CCDBUserEnterEvent::Get64Int(CT_INT32 high,CT_INT32 low)
{
	CT_INT64 lKey=high;
	lKey<<=32;
	lKey|=low;
	return lKey;
}