#include <time.h>
#include "CCityUserBuildEvent.h"
#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "GlobeScriptInterface.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CUserManager.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "CBuildingScriptSetting.h"
#include "CRegionScriptSetting.h"
#include "CMarket.h"
#include "CFormula.h"

CCCityUserBuildEvent::CCCityUserBuildEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	//消息注册
	RegistMessage(MAIN_G_APPLY_UPGRADE_CITY,       SUB_G_APPLY_UPGRADE_BUILD);
	RegistMessage(MAIN_G_APPLY_UPGRADE_CITY,       SUB_G_UPGRADE_SPEED_UP );
	RegistMessage(MAIN_G_APPLY_UPGRADE_CITY,		SUB_G_UPGRADE_SPEED_UP_REQUEST);
	RegistMessage(MSC_MAIN_DB_UPGRAE_FINISH,       MSC_SUB_DB_R_BUILDING_UPGRADING_FINISH );


	//设置全局定时器
	//CT_INT32 threadID =  GAME_THREAD_CITY /*+ rand() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum*/;
	//if (m_dwThread == threadID)
	//{
		//定时器注册
		G_ValueRange vr = {0};
		if(G_GetValueRange(VALUE_TYPE_BUILDINGUPGRADETIME,  &vr))
		{
			if (vr.fMaxVal < 1)
			{
				CCLogMeg::getSingleton().WriteError("没有发现在全局配置文件中发现 VALUE_TYPE_BUILDINGUPGRADETIME = %i",0);
				abort();
			}
			RegistTimer(IDI_UPGRADE_BUILDING_TIMER);
			SetTimer(IDI_UPGRADE_BUILDING_TIMER, (CT_INT32)(vr.fMaxVal*1000));
		}

		if (G_GetValueRange(VALUE_TYPE_BUILDING_SYS_DATABASE_CHECK_TIME, &vr ) )
		{
			if (vr.fMaxVal < 1)
			{
				CCLogMeg::getSingleton().WriteError("没有发现在全局配置文件中发现 VALUE_TYPE_BUILDING_SYS_DATABASE_CHECK_TIME = %i",0);
				abort();
			}
			RegistTimer(IDI_BUILDING_SAVE_DATABASE_TIMER);
			SetTimer(IDI_BUILDING_SAVE_DATABASE_TIMER, (CT_INT32)(vr.fMaxVal*1000));
		}
}

CCCityUserBuildEvent::~CCCityUserBuildEvent(void)
{
}

//用户定时器处理
CT_VOID CCCityUserBuildEvent::OnTime(CT_DWORD timeID)
{
	if(timeID == IDI_UPGRADE_BUILDING_TIMER)
	{
		CCUserManager::getSingleton().UpdateUpgradingBuilding(m_dwThread - GAME_THREAD_CITY, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum, m_dwThread);
	}

	if (timeID == IDI_BUILDING_SAVE_DATABASE_TIMER)
	{
		CCUserManager::getSingleton().UpdateSaveBuildingData(m_dwThread - GAME_THREAD_CITY, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum, m_dwThread);
	}
}

//处理用户消息
CT_VOID CCCityUserBuildEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
										  CT_CHAR* pBuf,		CT_DWORD len,
							              CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_APPLY_UPGRADE_CITY:
		{
			DisposeUpgradeBuilding(subID, pBuf, len);
			break;
		}

	case  MSC_MAIN_DB_UPGRAE_FINISH:
		{
			DisposeDBBuildingFinish(subID, pBuf, len);
		}
	default:
		{
			break;
		}
	}
}

//处理建造
CT_VOID CCCityUserBuildEvent::DisposeUpgradeBuilding(CT_DWORD subID,   CT_CHAR* pBuf,		CT_DWORD len)
{
	switch(subID)
	{
	case  SUB_G_UPGRADE_SPEED_UP:
		{
			if(len != sizeof(CMD_G_UpgradeBuildSpeedUp ))
			{
				G_DisposeErrorMes(0, subID, "CCCityUserBuildEvent::OnUserEvent:len != sizeof(CMD_G_UpgradeBuildSpeedUp");
				return ;
			}

			CMD_G_UpgradeBuildSpeedUp* pGB = (CMD_G_UpgradeBuildSpeedUp*)pBuf;
			DisposeUpgradeBuildSpeedUp(* pGB);
			break;
		}
	case  SUB_G_APPLY_UPGRADE_BUILD:
		{
			if(len != sizeof(CMD_G_UpgradeBuild ))
			{
				G_DisposeErrorMes(0, subID, "CCCityUserBuildEvent::OnUserEvent:len != sizeof(CMD_G_UpgradeBuild");
				return ;
			}

			CMD_G_UpgradeBuild* pGB = (CMD_G_UpgradeBuild*)pBuf;
			DisposeUpgradeBuilding(*pGB);
			break;
		}
	case SUB_G_UPGRADE_SPEED_UP_REQUEST:
		{
			if(len != sizeof(CMD_G_UpgradeBuildSpeedUp ))
			{
				G_DisposeErrorMes(0, subID, "CCCityUserBuildEvent::OnUserEvent:len != sizeof(CMD_G_UpgradeBuildSpeedUpRequest");
				return ;
			}

			CMD_G_UpgradeBuildSpeedUp* pGB = (CMD_G_UpgradeBuildSpeedUp*)pBuf;
			GetBuildSpeedUpRequest(* pGB);
			break;
		}
	default:
		{
			break;
		}
	}		  

}

//处理建造加速
CT_VOID CCCityUserBuildEvent::DisposeUpgradeBuildSpeedUp(const CMD_G_UpgradeBuildSpeedUp& ubsu)
{
	CCUser* pUser = 0;
	if(!CCUserManager::getSingleton().Find(ubsu.dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuildSpeedUp: Can't find user id: %i", ubsu.dwUserID);
		return;
	}
	
	CMD_G_UpgradeBuildSpeedUpFail bsf = {0};
	bsf.dwUserID = ubsu.dwUserID;
	bsf.iBuildID = ubsu.iBuildID;

	UpgradingBuildData ubd = {0};
	if (!pUser->m_userBuilding.GetUpgardingBuildingData(ubsu.iBuildID, ubd )/*, !pUser->m_userBuilding.FindAutoBuilding(ubsu.iBuildID)*/)
	{
		bsf.ubsf     = UBSF_NO_UPGRADING_BUILD;
		pUser->GetNetInterface()->Send(MAIN_G_APPLY_UPGRADE_CITY, SUB_G_UPGRADE_SPEED_UP_FAIL,(CT_CHAR*)&bsf, sizeof(bsf));
		return;
	}

	//vip add 如果建筑是vip特权建筑，判断vip特权
	int iPriID=0;
	if ((iPriID=G_GetVipPriIDByBuildID(ubsu.iBuildID))>0)
	{
		if (!pUser->m_userVip.VipHasPrivilege(iPriID))
		{
			CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuildSpeedUp: build id: %d need vip pri %d \n", ubsu.iBuildID,iPriID );
			pUser->CloseNet();
			return;
		}
	}

	G_ValueRange gvr = {0};
    G_GetValueRange(VALUE_TYPE_BUILDSPEEDUP, &gvr);

	CT_INT32 leftTime   =   ubd.buildingUpgrade.iNeedTime - (CT_INT32)(time(0) - ubd.llBeginTime);
	leftTime=(leftTime < 0?0:leftTime);
	CT_INT32 needDiamod =  (CT_INT32)(leftTime * gvr.fMaxVal) - 1; //钻石消耗+1，避免0钻石加速情况
	if (leftTime < 0)
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuildSpeedUp: speed up need %i diamond", needDiamod);
		return ;
	}

	MasterData md = {0};
	pUser->m_userBase. GetMasterData(md);
	if (md.mvd.stRes.iDiamod + needDiamod < 0)
	{
		bsf.ubsf     = UBSF_NO_DIAMOD;
		pUser->GetNetInterface()->Send(MAIN_G_APPLY_UPGRADE_CITY, SUB_G_UPGRADE_SPEED_UP_FAIL, (CT_CHAR*)&bsf, sizeof(bsf));
		return ;
	}

	
    MasterVariableData mvd = {0};
    mvd.stRes.iDiamod = needDiamod;
    pUser->m_userBase.AddMastData(mvd, SAVETAG_CIVIL_UPGRADE_SPEE_UP_FINISH);

    pUser->m_userBase.GetMasterData(md);
	CMD_G_UpgradeBuildSpeedUpSuc sus = {0};
	sus.dwUserID = ubsu.dwUserID;
	sus.iBuildID = ubsu.iBuildID;
	sus.iDiamod  = md.mvd.stRes.iDiamod;  
	pUser->GetNetInterface()->Send(MAIN_G_APPLY_UPGRADE_CITY, SUB_G_UPGRADE_SPEED_UP_SUC, (CT_CHAR*)&sus, sizeof(sus));
 
	ubd.iNeedTime     = 0;
	ubd.bSpeedUpTime  = CT_TRUE;

	pUser->m_userBuilding.UpdateSaveBuildingData(CT_FALSE, CT_FALSE, CT_TRUE);  //强制同步一次建筑数据到数据库，防止钻石加速数据被合并，这样的实现比较简单，但是效率低
	//升级加速完成做修改，直接删除升级数据，写入钻石消耗，不在由定时器触发完成
	UpgradedBuildingResult result = {0};
	result.dwBid    =  ubd.iID;
	result.iCiviNum = ubd.buildingUpgrade.iCiliviNum;
	result.iDiamod  =  needDiamod; //ubd.buildingUpgrade.iDiamond;
	result.iFood    = ubd.buildingUpgrade.iFood;
	result.iGold    = ubd.buildingUpgrade.iGold;
	result.iProp1   = ubd.buildingUpgrade.iProp1;
	result.iProp1Num = ubd.buildingUpgrade.iPropNum1;
	result.iProp2    = ubd.buildingUpgrade.iProp2;
	result.iProp2Num = ubd.buildingUpgrade.iPropNum2;
	result.iProp3    = ubd.buildingUpgrade.iProp3;
	result.iProp3Num = ubd.buildingUpgrade.iPropNum3;
	result.iProp4    = ubd.buildingUpgrade.iProp4;
	result.iProp4Num = ubd.buildingUpgrade.iPropNum4;
	result.iWorker   = ubd.buildingUpgrade.iWorker;
	//result.iSpeedTimes = (CT_INT32)ubd.bSpeedUpTime;
	result.iXp         = ubd.buildingUpgrade.iXp;
	++result.iUpgradeTimes;
	pUser->m_userBuilding.AddUpgradedBuildingResult(result);



	MSC_DB_R_BuildUpgradeFinish upgradeFinish = {0}; 
	upgradeFinish.dwUserID =   pUser->GetUserID();
	upgradeFinish.iBID     =   ubd.iID;
	upgradeFinish.iGrade   =   ubd.buildingUpgrade.iGrade;
	DisposeDBBuildingFinish(upgradeFinish);

	//自动生产的建筑是否在生产
	ProductingBuilding pb = {0};
	if (pUser->m_userBuilding.FindAutoBuilding(ubsu.iBuildID, pb))
	{
		pb.llTime = time(0);
		pUser->m_userBuilding.UpdateAutoBuilding(ubsu.iBuildID, pb);
	}
};


//处理建造					  			
CT_VOID CCCityUserBuildEvent::DisposeUpgradeBuilding(const CMD_G_UpgradeBuild& ubData)
{
	//基本建筑数据关联检测（判断发来的数据是否合法）
	G_Building build = {0};
	G_BuildingType bt = {0};
	if(!CCBuildingScriptSetting::getSingleton().GetBuildingData(ubData.iBuildID, &build))
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: CCBuildingScriptSetting::getSingleton().GetBuildingData Can't find user id: %i : buildID=%i", ubData.dwUserID, ubData.iBuildID);
		return;
	}

	if(!CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(ubData.iBuildID, &bt))
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID Can't find user id: %i : buildID=%i", ubData.dwUserID, ubData.iBuildID);
		return;
	}

	//查找是否正在到达升级限制和是否正在升级
	CCUser* pUser = 0;
	if(!CCUserManager::getSingleton().Find(ubData.dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: Can't find user id: %i", ubData.dwUserID);
		return;
	}

	//vip add 如果建筑是vip特权建筑，判断vip特权
	int iPriID=0;
	if ((iPriID=G_GetVipPriIDByBuildID(ubData.iBuildID))>0)
	{
		if (!pUser->m_userVip.VipHasPrivilege(iPriID))
		{
			CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: build id: %d need vip pri %d \n", ubData.iBuildID,iPriID );
			pUser->CloseNet();
			return;
		}
	}

	//判断建筑所在区域是否解锁
	CT_INT32 iRegionID=0;
	CCRegionScriptSetting::getSingleton().GetRegionIDByBuildID(ubData.iBuildID,&iRegionID);
	if(!pUser->m_userRegion.GetRegionStatus(iRegionID))
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: build id: %d the region %d is unlock\n", ubData.iBuildID, iRegionID);
		pUser->CloseNet();
		return;
	}

	//判断任务是否完成
	if(!pUser->m_userTask.ViewCompleteTask(build.iTaskID) && build.iTaskID != 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_TASK_FAIL);
		return;
	}

	//判断建筑需要的领主和文明等级
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	if (mvd.stCivili.iLevel < build.iCivilGrade)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_CIVILI_GRADE_LOW_FAIL);		
		return ;
	}

	
	if (mvd.stGrade.iLevel< build.iMasterGrade )
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_MASTER_GRADE_LOW_FAIL);
		return ;
	}

	//得到用户建筑数据
	BuildingData buildData={0};
	if(pUser->m_userBuilding. FindBuildData(ubData.iBuildID, buildData))
	{
		if (buildData.iGrade >= bt.iMaxGrade)
		{
			SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_TOP_GRADE_FAIL);
			return ;
		}
		if (pUser->m_userBuilding. FindUpgradeBuildData(ubData.iBuildID))
		{
			SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_UPGRADING_FAIL);
			return;
		}
	}

	//图腾等级不能超过领主等级，建筑等级不能超过图腾等级
	if (buildData.iID == BUILIDING_ID_TOTEM) 
	{
		MasterLevelInfo mle = {0};
	    pUser->m_userBase.GetMasterGrade(mle);
		if (buildData.iGrade >= mle.iLevel)
		{
			SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_MASTER_FAIL);
			return;
		}
	}else{

		BuildingData totem = {0};
		if(!pUser->m_userBuilding.FindBuildData(BUILIDING_ID_TOTEM, totem))
		{
			CT_ASSERT(false || "CCCityUserBuildEvent::DisposeUpgradeBuilding 致命的错误没有发现图腾建筑");
			CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding 致命的错误没有发现图腾建筑 usid =%i", ubData.dwUserID);
			return;
		}

		if (buildData.iGrade >= totem.iGrade)
		{
			SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_TOTEM_FAIL);
			return;
		}
	}

	//手动生产建筑是否在生产
	if (pUser->m_userBuilding. FindBuildingProduct(ubData.iBuildID))
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_PRODUCTING_FAIL);
		return ;
	}

	if (pUser->m_userEvent.FindBuildingEventByID(ubData.iBuildID))
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_EVENTING_FAIL);
		return ;
	}
	
	//得到升级数据
	G_BuildingUpgrade bu ={0};
	if (! CCBuildingScriptSetting::getSingleton().GetBuildingUpgradeDataWithID( ubData.iBuildID, buildData.iGrade+1, &bu))
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_DATA_FAIL);
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: CCBuildingScriptSetting::getSingleton().GetBuildingUpgradeDataWithID Can't find building type id: %i", bt.iID);
		return ;
	}

	//得到建筑信息
	G_Building     buildingInfo = {0};
	if (!CCBuildingScriptSetting::getSingleton().GetBuildingData(ubData.iBuildID, &buildingInfo))
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_DATA_FAIL);
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: G_GetBuildingData Can't find building id: %i", ubData.iBuildID );
		return ;
	}

	//检测资源是否满足
	if (!CheckResForUpgrade(pUser, ubData, bu))
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeUpgradeBuilding: fail to CanBuildingUpgrade()" );
		return ;
	}


	//自动生产的建筑是否在生产
	if (pUser->m_userBuilding.FindAutoProductingBuilding(ubData.iBuildID))
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_PRODUCTING_FAIL);
		return ;
	}

	ProductingBuilding pb = {0};
	if (pUser->m_userBuilding.FindAutoBuilding(ubData.iBuildID, pb))
	{
		pb.llTime = time(0) + bu.iNeedTime;
		pUser->m_userBuilding.UpdateAutoBuilding(ubData.iBuildID, pb);
	}

	UpgradingBuildData  ubd = {0};
	ubd.iID           = build.iID;
	ubd.llBeginTime   = time(0);
	ubd.bSpeedUpTime  = 0;
	ubd.iNeedTime     = bu.iNeedTime;
	memcpy(&(ubd.buildingUpgrade), &bu, sizeof(ubd.buildingUpgrade));
	pUser->m_userBuilding.InsertUpgradeBuildData(ubd);

	DisposeDBUpgadeSuc(pUser, ubd);
}	

//检测资源是否满足
CT_BOOL CCCityUserBuildEvent::CheckResForUpgrade(  CCUser* pUser, const CMD_G_UpgradeBuild& ubData, const G_BuildingUpgrade& bu)
{
	MasterData md;
	memset(&md, 0, sizeof(md));
	pUser->m_userBase. GetMasterData(md);

	//if (md.mvd.iCiviliGrade < bu.iCivilGrade)
	//if (md.mvd.stCivili.iLevel < bu.iCivilGrade)
	//{
	//	SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_CIVILI_GRADE_LOW_FAIL);		
	//	return CT_FALSE;
	//}

	//if (md.mvd.stGrade.iLevel < bu.iMasterGrade)
	////if (md.mvd.stGrade.iLevel + bu.iMasterGrade < 0)
	//{
	//	SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_MASTER_GRADE_LOW_FAIL);
	//	return CT_FALSE;
	//}

	//if (md.mvd.iWorker <  bu.iWorker)
	if (md.mvd.stRes.iWorker +  bu.iWorker < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_WORKER_LACK_FAIL);
		return CT_FALSE;
	}

	//if (md.mvd.iDiamod < bu.iDiamond)
	if (md.mvd.stRes.iDiamod + bu.iDiamond < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_DIAMOD_LACK_FAIL);
		return CT_FALSE;
	}

	//if (md.mvd.iFood < bu.iFood)
	if (md.mvd.stRes.iFood + bu.iFood < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_FOOD_LACK_FAIL);
		return CT_FALSE;
	}

	//if (md.mvd.iGold < bu.iGold)
	if (md.mvd.stRes.iGold + bu.iGold < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_GOLD_LACK_FAIL);
		return CT_FALSE;
	}
	//升级完成，获得文明度
	/*if (md.mvd.stCivili.iCurrentExp + bu.iCiliviNum < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_GOLD_LACK_FAIL);
		return CT_FALSE;
	}*/
	//判断道具
	if ( pUser->m_userItem.GetItemCount(bu.iProp1) + bu.iPropNum1 < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_PROP_LACK_FAIL);
		return CT_FALSE;
	}

	if ( pUser->m_userItem.GetItemCount(bu.iProp2) + bu.iPropNum2 < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_PROP_LACK_FAIL);
		return CT_FALSE;
	}

	if ( pUser->m_userItem.GetItemCount(bu.iProp3) + bu.iPropNum3 < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_PROP_LACK_FAIL);
		return CT_FALSE;
	}

	if ( pUser->m_userItem.GetItemCount(bu.iProp4) + bu.iPropNum4 < 0)
	{
		SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_PROP_LACK_FAIL);
		return CT_FALSE;
	}

	return CT_TRUE;
}



//发送建造错误
CT_VOID CCCityUserBuildEvent::SendBuildError(CCUser* pUser, CT_DWORD rMain, CT_DWORD rSub,  ENUM_UpgradeBuildingFail en)
{
	CMD_G_UpgradeBuildingFail cbf;
	cbf.cbfCode = en;
	pUser->GetNetInterface()->Send(rMain, rSub, (CT_CHAR*)&cbf, sizeof(cbf) );
}



//处理升级成功
CT_VOID CCCityUserBuildEvent::DisposeDBUpgadeSuc(CCUser* pUser,   const UpgradingBuildData& ubd)
{
		MasterVariableData  md;
		memset(&md, 0, sizeof(md));
		// [9/11/2012 John]
		md.stRes.iDiamod += ubd.buildingUpgrade.iDiamond;
		md.stRes.iFood   += ubd.buildingUpgrade.iFood;
		md.stRes.iGold   += ubd.buildingUpgrade.iGold;
		md.stRes.iWorker += ubd.buildingUpgrade.iWorker;
		//md.stCivili.iCurrentExp += ubd.buildingUpgrade.iCiliviNum;
		//md.stGrade.iCurrentExp += ubd.buildingUpgrade.iXp;
		pUser->m_userBase.AddMastData(md, SAVETAG_BUILD_UPGRADE_FINISH);

		CT_INT32 iCount=0;
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		if(ubd.buildingUpgrade.iProp1 != 0 && ubd.buildingUpgrade.iPropNum1 != 0)
		{
			iCount=ubd.buildingUpgrade.iPropNum1;
			pUser->m_userItem.AutoExpendItem(SAVETAG_BUILD_UPGRADE_FINISH,ubd.buildingUpgrade.iProp1,-iCount,ITEM_OP_MODE_EXEC);
		}
		if(ubd.buildingUpgrade.iProp2 != 0 && ubd.buildingUpgrade.iPropNum2 != 0)
		{
			iCount=ubd.buildingUpgrade.iPropNum2;
			pUser->m_userItem.AutoExpendItem(SAVETAG_BUILD_UPGRADE_FINISH,ubd.buildingUpgrade.iProp2,-iCount,ITEM_OP_MODE_EXEC);
		}
		if(ubd.buildingUpgrade.iProp3 != 0 && ubd.buildingUpgrade.iPropNum3 != 0)
		{
			iCount=ubd.buildingUpgrade.iPropNum3;
			pUser->m_userItem.AutoExpendItem(SAVETAG_BUILD_UPGRADE_FINISH,ubd.buildingUpgrade.iProp3,-iCount,ITEM_OP_MODE_EXEC);
		}
		if(ubd.buildingUpgrade.iProp4 != 0 && ubd.buildingUpgrade.iPropNum4 != 0)
		{
			iCount=ubd.buildingUpgrade.iPropNum4;
			pUser->m_userItem.AutoExpendItem(SAVETAG_BUILD_UPGRADE_FINISH,ubd.buildingUpgrade.iProp4,-iCount,ITEM_OP_MODE_EXEC);
		}

		//G_SaveMastVariableData(pUser, SAVETAG_BUILD_UPGRADE_FINISH);

		pUser->m_userBase.GetMasterData(md);
		CMD_G_UpgradeBuildingSuc bs;
		memset(&bs, 0, sizeof(bs));
		bs.dwUserID = pUser->GetUserID();
		bs.iBID =     ubd.iID;
		bs.iLastingTime = ubd.buildingUpgrade.iNeedTime;
		bs.iGold        = md.stRes.iGold;
		bs.iDiamod      = md.stRes.iDiamod;
		bs.iMaxWorker   = md.stRes.iMaxWorker;
		bs.iWorker      = md.stRes.iWorker;
		bs.iFood        = md.stRes.iFood;
		bs.iCiviNum     = md.stCivili.iCurrentExp;
		
		//道具没有处理
		pUser->GetNetInterface()->Send(MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_SUC, (CT_CHAR*)&bs,  sizeof(bs));

		MSC_USR_AddItem ua = {0};
		ua.dwUserID = pUser->GetUserID();
		ua.iItemID  = ubd.buildingUpgrade.iProp1;
		ua.iItemID  = ubd.buildingUpgrade.iPropNum1;
		G_SendThreadInfo(m_dwThread, MSC_MAIN_USR_ITEM_UPDATE, MSC_SUB_USR_ADD_ITEM, (CT_CHAR*)&ua, sizeof(ua));
		ua.iItemID  = ubd.buildingUpgrade.iProp2;
		ua.iItemID  = ubd.buildingUpgrade.iPropNum2;
		G_SendThreadInfo(m_dwThread, MSC_MAIN_USR_ITEM_UPDATE, MSC_SUB_USR_ADD_ITEM, (CT_CHAR*)&ua, sizeof(ua));
		ua.iItemID  = ubd.buildingUpgrade.iProp3;
		ua.iItemID  = ubd.buildingUpgrade.iPropNum3;
		G_SendThreadInfo(m_dwThread, MSC_MAIN_USR_ITEM_UPDATE, MSC_SUB_USR_ADD_ITEM, (CT_CHAR*)&ua, sizeof(ua));
		ua.iItemID  = ubd.buildingUpgrade.iProp4;
		ua.iItemID  = ubd.buildingUpgrade.iPropNum4;
		G_SendThreadInfo(m_dwThread, MSC_MAIN_USR_ITEM_UPDATE, MSC_SUB_USR_ADD_ITEM, (CT_CHAR*)&ua, sizeof(ua));
		//升级请求成功，增加升级任务判断
		//add by caihan  11-14
		CMD_G_TaskEventInfo taskEventInfo = {0};
		/*G_BuildingType buildingType = {0};
		CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(bs.iBID,&buildingType);*/
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_UPGRADE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_UPGRADE_BUILD;
		taskEventInfo.iSubType = bs.iBID;
		taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
		pUser->m_userTask.DisposeTask(taskEventInfo);
		//end

};


//处理建造完成
CT_VOID CCCityUserBuildEvent::DisposeDBBuildingFinish(CT_DWORD subID,   CT_CHAR* pBuf, CT_DWORD len)
{
	switch(subID)
	{
	case  MSC_SUB_DB_R_NET_ERROR:
		{
			break;
		}
	case MSC_SUB_DB_R_BUILDING_UPGRADING_FINISH:
		{
			MSC_DB_R_BuildUpgradeFinish* pBuildUpgradeFinish = (MSC_DB_R_BuildUpgradeFinish*)pBuf;
			if (len != sizeof(MSC_DB_R_BuildUpgradeFinish))
			{
				G_DisposeErrorMes(0, subID, "CCCityUserBuildEvent::DisposeDBBuildingFinish : len != sizeof(MSC_DB_R_BuildUpgradeFinish)");
				return;
			}
			
			DisposeDBBuildingFinish(*pBuildUpgradeFinish);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeDBBuildingFinish : Can't find subID = %i", subID);
			break;
		}
	}
}

//处理建造完成
CT_VOID CCCityUserBuildEvent::DisposeDBBuildingFinish(const MSC_DB_R_BuildUpgradeFinish& buildUpgradeFinish)
{
	CCUser* pUser = 0;
	if(CCUserManager::getSingleton().Find(buildUpgradeFinish.dwUserID, &pUser))
	{
		BuildingData buildData = {0};
		buildData.iID = buildUpgradeFinish.iBID;
		buildData.iGrade = buildUpgradeFinish.iGrade;
		buildData.bChange= CT_TRUE;

		pUser->m_userBuilding.UpateBuildData(buildData);
		pUser->m_userBuilding.DeleteUpagradeBuildData(buildUpgradeFinish.iBID);

		pUser->CalMaxWorker();  //重新计算最大工人数目
		pUser->CountFreeWorker(); 

		MasterVariableData  md = {0};
		pUser->m_userBase.GetMasterData(md);	//获取领主信息

		CMD_G_UpgradeBuildingFinish uf;
		memset(&uf, 0, sizeof(uf));
		uf.dwUserID = buildUpgradeFinish.dwUserID;
		uf.iBID     = buildUpgradeFinish.iBID;
		uf.iGrade   = buildUpgradeFinish.iGrade;
		uf.iMaxWorker = md.stRes.iMaxWorker;
		uf.iWorker = md.stRes.iWorker;

		G_BuildingType buildingType = {0};
		CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(uf.iBID,&buildingType);

		//升级完成，获得文明度和领主经验
		G_BuildingUpgrade buildUpgrade = {0};
		if (!G_GetBuildingUpgradeData(buildingType.iID,buildUpgradeFinish.iGrade, &buildUpgrade))
		{
			CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeDBBuildingFinish G_GetBuildingUpgradeData %d",buildingType.iID);
			SendBuildError(pUser, MAIN_G_APPLY_UPGRADE_CITY, SUB_G_APPLY_UPGRADE_BUILD_FAIL, CBF_DATA_FAIL);
			return ;
		}
		MasterVariableData  mvd = {0};
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		mvd.stGrade.iCurrentExp =  G_GetBuildUpgradeMasterXp(buildUpgrade.iXp,masterData.factor.fProductFactor);
		mvd.stCivili.iCurrentExp = buildUpgrade.iCiliviNum;
		pUser->m_userBase.AddMastData(mvd,SAVETAG_BUILD_UPGRADE_FINISH);
		//end

		memset(&mvd,0,sizeof(mvd));
		pUser->m_userBase.GetMasterData(mvd);

		uf.iCivil = mvd.stCivili.iCurrentExp;
		uf.iXP = mvd.stGrade.iCurrentExp;

		pUser->GetNetInterface()->Send(MAIN_G_APPLY_UPGRADE_CITY,SUB_G_UPGRADE_BUILD_FINISH, (CT_CHAR*)&uf, sizeof(uf));
		
		pUser->m_userBuilding.UpdateSaveBuildingData(CT_TRUE, CT_FALSE, CT_TRUE);  //强制同步一次建筑数据到数据库，防止钻石加速数据被合并，这样的实现比较简单，但是效率低

		//升级成功(升级完成)，增加升级任务判断
		//add by caihan  11-14
		CMD_G_TaskEventInfo taskEventInfo = {0};
		
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_UPGRADE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_UPGRADE_BUILD;
		taskEventInfo.iSubType = buildUpgradeFinish.iBID;
		taskEventInfo.iLevel = uf.iGrade;
		pUser->m_userTask.DisposeTask(taskEventInfo);

		//增加指定类型建筑升级任务判断
		CMD_G_TaskSystemInfo taskSysInfo = {0};
		taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
		taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_BUILD_UPGRADE;
		taskSysInfo.arrayParam[0] = buildingType.iID;       //参数0：建筑类型
		taskSysInfo.arrayParam[1] = uf.iBID;                //参数1：建筑id
		taskSysInfo.iProgress = uf.iGrade;
		pUser->m_userTask.DisposeTask(taskSysInfo);

		//升级完成，增加城市任务判断
		memset(&taskEventInfo,0,sizeof(taskEventInfo));
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_BUILD;
		taskEventInfo.iSubType = buildingType.iID;
		pUser->m_userTask.DisposeTask(taskEventInfo);
		//end
        pUser->m_userDaily.UpdateQuestData(DAILY_BUILDIND_UPGRADE, 1);

		//建筑完成，如果是集市，推送商店消息
		if(uf.iGrade == 1)
		{
			G_Building Build={0};
			if(!G_GetBuildingData(uf.iBID,&Build))
			{
				CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeDBBuildingFinish G_GetBuildingData %d",uf.iBID);
			}
			if (Build.iID == CITY_TYPE_MARKET_BUILDID)
			{
				CCMarket::getSingleton().SendNormalMarketInfo(pUser);
			}
			else if(Build.iBT == CITY_TYPE_DUPLICATE)
			{
				MSC_Duplicate_Finish DuplicateFinish={0};
				DuplicateFinish.dwUserID=pUser->GetUserID();
				CT_INT32 threadID =  GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadID,  MSC_MAIN_DUPLICATE_EVENT,     MSC_SUB_DUPLICATE_FINISH , (CT_CHAR*)&DuplicateFinish, sizeof(DuplicateFinish));
			}
			else if(Build.iBT == CITY_TYPE_ARENA)
			{
				MSC_DB_Refresh Refresh={0};
				Refresh.dwUserID=pUser->GetUserID();

				CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_W_REFRESH,(CT_CHAR*)&Refresh,sizeof(Refresh));
			}
			else if(Build.iBT == CITY_TYPE_SOLDIER)
			{
				pUser->m_userArmy.UpDataOpenArmy(uf.iGrade);
			}
		}
		else if(uf.iGrade > 1)
		{
			G_Building Build={0};
			if(!G_GetBuildingData(uf.iBID,&Build))
			{
				CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeDBBuildingFinish G_GetBuildingData %d",uf.iBID);
			}
			if(Build.iBT == CITY_TYPE_SOLDIER)
			{
				pUser->m_userArmy.UpDataOpenArmy(uf.iGrade);
			}

			//生产型建筑，升级完成后直接完成最低级生产
			G_BuildingType bt = {0};
			if(CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(buildUpgradeFinish.iBID, &bt))
			{
				if (bt.iID == CITY_TYPE_RESIDENTIAL || CITY_TYPE_FARMLAND == bt.iID || CITY_TYPE_PANNING == bt.iID || CITY_TYPE_FOREST == bt.iID || CITY_TYPE_STONEORE == bt.iID || CITY_TYPE_IRONORE == bt.iID || CITY_TYPE_PATCAVE == bt.iID || CITY_TYPE_TAMEDRAGON==bt.iID)
				{
					ProductingBuilding bp = {0};
					bp.iBid    = buildUpgradeFinish.iBID;
					bp.iType  = G_GetLeastProductTypeByBuildType(bt.iID);
					bp.llTime = time(0);
					bp.bSendCanHavest = CT_TRUE;
					bp.iWorker      = 0;
					//民居不在这里处理
					//if (buildType == CITY_TYPE_RESIDENTIAL)
					//{
					//	bp.iLastingTime = 0 ;
					//}else
					//{
					//	bp.iLastingTime = bpi.iTime;
					//}
					G_BuildingProductInfo bpi={0};
					if (!CCBuildingScriptSetting::getSingleton().GetProductInfo(bp.iType, &bpi))
					{
						CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeDBBuildingFinish : fail to find producttype in FindProduct: %d",bp.iType );
						return ;
					}
					bp.iLastingTime = 0;
					bp.iBType       = bt.iID;
					bp.iBGrade      = buildUpgradeFinish.iGrade;
					if (bt.iID == CITY_TYPE_RESIDENTIAL)
					{
						pUser->m_userBuilding.UpdateAutoBuilding(bp.iBid, bp);
					}
					else
					{
						pUser->m_userBuilding.InsertBuildingProductMap(bp);
					}

					CMD_G_ProductHarvest ch = {0};
					MasterVariableData mvd = {0};
					ch.dwUserID = pUser->GetUserID();
					ch.iBuildID = buildUpgradeFinish.iBID;
					memset(&mvd,0,sizeof(mvd));
					pUser->m_userBase.GetMasterData(mvd);
					ch.iWorker = mvd.stRes.iWorker;
					pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_CAN_HARVEST,(CT_CHAR*)&ch, sizeof(ch));

				}
			}
		}

	//	DisposeAutoBuilding(pUser, buildUpgradeFinish);
	}
}

//处理自动生产建筑
CT_VOID CCCityUserBuildEvent::DisposeAutoBuilding(CCUser* pUser, const MSC_DB_R_BuildUpgradeFinish& buildUpgradeFinish)
{
	G_BuildingType bt = {0};
	if(CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(buildUpgradeFinish.iBID, &bt) && bt.iID == CITY_TYPE_RESIDENTIAL)
	{
		G_BuildingProduct  gBp = {0};
		if (CCBuildingScriptSetting::getSingleton().GetProductType(bt.iID, buildUpgradeFinish.iGrade, &gBp))
		{
			G_BuildingProductInfo gbpi = {0};
			if (CCBuildingScriptSetting::getSingleton().GetProductInfo(gBp.iProductType,  &gbpi))
			{
				//如果是民居类型的自动生产建筑则直接插入生产队列
				ProductingBuilding pb = {0};
				pb.bSendCanHavest = CT_FALSE;
				pb.iBGrade        = buildUpgradeFinish.iGrade;
				pb.iBid           = buildUpgradeFinish.iBID;
				pb.iBType         = bt.iID;
				pb.iLastingTime   = gbpi.iTime;
				pb.iType          = gBp.iProductType;
				pb.iWorker        = 0;
				pb.llTime         = time(0);
				pUser->m_userBuilding.UpdateAutoBuilding(pb.iBid, pb);
			}
		}
	}
}

CT_VOID CCCityUserBuildEvent::GetBuildSpeedUpRequest(const CMD_G_UpgradeBuildSpeedUp& ubsu)
{
	CCUser* pUser = 0;
	if(!CCUserManager::getSingleton().Find(ubsu.dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::GetBuildSpeedUpRequest: Can't find user id: %i", ubsu.dwUserID);
		return;
	}

	CMD_G_UpgradeBuildSpeedUpFail bsf = {0};
	bsf.dwUserID = ubsu.dwUserID;
	bsf.iBuildID = ubsu.iBuildID;

	UpgradingBuildData ubd = {0};
	if (!pUser->m_userBuilding.GetUpgardingBuildingData(ubsu.iBuildID, ubd ))
	{
		bsf.ubsf     = UBSF_NO_UPGRADING_BUILD;
		pUser->GetNetInterface()->Send(MAIN_G_APPLY_UPGRADE_CITY, SUB_G_UPGRADE_SPEED_UP_FAIL,(CT_CHAR*)&bsf, sizeof(bsf));
		return;
	}

	G_ValueRange gvr = {0};
	G_GetValueRange(VALUE_TYPE_BUILDSPEEDUP, &gvr);

	CT_INT32 leftTime   =   ubd.iNeedTime - (CT_INT32)(time(0) - ubd.llBeginTime);
	leftTime=(leftTime < 0?0:leftTime);
	CT_INT32 needDiamod =  (CT_INT32)(leftTime * gvr.fMaxVal) - 1; //钻石消耗+1，避免0钻石加速情况
	if (leftTime < 0)
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::GetBuildSpeedUpRequest: speed up need %i diamond", needDiamod);
		return ;
	}

	CMD_G_SpeedUpNeedResource nd = {0};
	nd.dwUserID = ubsu.dwUserID;
	nd.iBuildID = ubsu.iBuildID;
	nd.iDiamod = needDiamod;
	pUser->GetNetInterface()->Send(MAIN_G_APPLY_UPGRADE_CITY,SUB_G_UPGRADE_SPEED_UP_REQUEST,(CT_CHAR*)&nd,sizeof(nd));
}
