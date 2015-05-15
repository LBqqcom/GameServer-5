#include <ctime>
#include "CBuildFunEvent.h"
#include "CMD_Game.h"
#include "CGameConfig.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CBuildingScriptSetting.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CLogMeg.h"
#include "BuildFuntion.h"
#include "CFormula.h"
#include "CFightingData.h"

CCBuildFunEvent::CCBuildFunEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	//消息注册
	RegistMessage(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_CIVILUPGRADE);
	RegistMessage(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_BUILDING_PRODUCT);
	RegistMessage(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_BUILDING_PRODUCT_APPLY_FINISH);
	RegistMessage(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_BUILDING_PRODUCT_SPEED_UP);
	RegistMessage(MAIN_G_BUILDING_EVENT_CITY,	SUB_G_BUILDING_PRODUCT_SPEED_UP_REQUEST);



	//建筑生产检测定时器
	G_ValueRange vr = {0};
	if(G_GetValueRange(VALUE_TYPE_BUILDINGPRODUCTTIME,  &vr))
	{
		if (vr.fMaxVal < 0)
		{
			CCLogMeg::getSingleton().WriteError("没有发现在全局配置文件中发现 VALUE_TYPE_BUILDINGPRODUCTTIME =%i", vr.fMaxVal);
			abort();
		}
		RegistTimer(IDI_BUILDING_PRODUCT_TIMER);
		SetTimer(IDI_BUILDING_PRODUCT_TIMER, (CT_INT32)(vr.fMaxVal * 1000));
	}
	
	if (G_GetValueRange(VALUE_TYPE_PRODUCTING_SYS_DATABASE_CHECK_TIME, &vr ) )
	{
		if (vr.fMaxVal < 0)
		{
			CCLogMeg::getSingleton().WriteError("没有发现在全局配置文件中发现 VALUE_TYPE_PRODUCTING_SYS_DATABASE_CHECK_TIME =%i", vr.fMaxVal);
			abort();
		}
		RegistTimer(IDI_PRODUCTING_SAVE_DATABASE_TIMER);
		SetTimer(IDI_PRODUCTING_SAVE_DATABASE_TIMER, (CT_INT32)(vr.fMaxVal*1000));
	}
}

CCBuildFunEvent::~CCBuildFunEvent(void)
{
}

//处理用户消息
CT_VOID CCBuildFunEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
									 CT_CHAR* pBuf,		CT_DWORD len,
									 CT_LONGLONG reserve)
{
	switch (mainID) 
	{
	case MAIN_G_BUILDING_EVENT_CITY :
		{
			DisposeBuildingEvent(subID, pBuf, len);
			break;
		}

	default:
		break;
	}
}

//用户定时器处理
CT_VOID CCBuildFunEvent::OnTime(CT_DWORD timeID)
{
	if(timeID == IDI_BUILDING_PRODUCT_TIMER)
	{
		CCUserManager::getSingleton().UpdateProductingBuilding(m_dwThread - GAME_THREAD_CITY, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum );
		return;
	}

	if (timeID == IDI_PRODUCTING_SAVE_DATABASE_TIMER)
	{
		CCUserManager::getSingleton().UpdateSaveProductingData(m_dwThread - GAME_THREAD_CITY, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum, m_dwThread);
		return;
	}
}

//处理建筑事件
CT_VOID CCBuildFunEvent::DisposeBuildingEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch (subID)
	{
	case SUB_G_CIVILUPGRADE:
		{
			if ( len != sizeof(CMD_G_LevelUpgrade))
			{
				G_DisposeErrorMes(0, subID, "CCBuildFunEvent::DisposeBuildingEvent: len != sizeof(CMD_G_LevelUpgrade)");
				return ;
			}
			CMD_G_LevelUpgrade* pUpgradCivil = (CMD_G_LevelUpgrade*) pBuf;
			DisposeCivilUpgrade(pUpgradCivil);
			break;
		}
	case SUB_G_BUILDING_PRODUCT:
		{
			if (len != sizeof(CMD_G_Product))
			{
				G_DisposeErrorMes(0, subID, "CCBuildFunEvent::DisposeBuildingEvent: len != sizeof(CMD_G_Product)");
				return  ;
			}
			CMD_G_Product* pProduct = (CMD_G_Product*)pBuf;
			DisposeProduct(*pProduct);
			break;
		}
	case  SUB_G_BUILDING_PRODUCT_APPLY_FINISH:
		{
			if (len != sizeof(CMD_G_RequestHavest))
			{
				G_DisposeErrorMes(0, subID, "CCBuildFunEvent::DisposeBuildingEvent: len != sizeof(CMD_G_RequestHavest)");
				return  ;
			}
			CMD_G_RequestHavest* pProduct = (CMD_G_RequestHavest*)pBuf;
			DisposeHarvest(*pProduct);
			break;
		}
	case  SUB_G_BUILDING_PRODUCT_SPEED_UP:
		{
			if (len != sizeof(CMD_G_ProductBuildSpeedUp))
			{
				G_DisposeErrorMes(0, subID, "CCBuildFunEvent::DisposeBuildingEvent: len != sizeof(CMD_G_ProductHarvest)");
				return  ;
			}
			CMD_G_ProductBuildSpeedUp* pProduct = (CMD_G_ProductBuildSpeedUp*)pBuf;
			DisposeProductBuildSpeeUp(*pProduct);	
			break;
		}
	case  SUB_G_BUILDING_PRODUCT_SPEED_UP_REQUEST:
		{
			if (len != sizeof(CMD_G_ProductBuildSpeedUp))
			{
				G_DisposeErrorMes(0, subID, "CCBuildFunEvent::DisposeBuildingEvent: len != sizeof(CMD_G_ProductHarvest)");
				return  ;
			}
			CMD_G_ProductBuildSpeedUp* pProduct = (CMD_G_ProductBuildSpeedUp*)pBuf;
			GetProductBuildSpeedRequest(*pProduct);
			break;
		}
	default:
		break;
	}
}

//处理文明升级事件
CT_VOID CCBuildFunEvent::DisposeCivilUpgrade(const CMD_G_LevelUpgrade* pUpgradCivil)
{
	CCUser* pUser = 0;
	if(CCUserManager::getSingleton().Find(pUpgradCivil->dwUserID, &pUser))
	{
#if 0
		MasterLevelInfo civil;
		pUser->m_userBase.GetMasterCivili(civil);

		MasterLevelInfo cu;
		memset(&cu, 0, sizeof(cu));
		if(! CCBuildingScriptSetting::getSingleton().GetCivilUpgradeData(civil.iLevel + 1, &cu))
		{
			CMD_G_LevelUpgradeFail ucf;
			ucf.dwUser = pUser->GetUserID();
			ucf.ucf    = LUF_NO_NEXT_UPGRADE;
			pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_CIVILUPGRADE_FAIL, (CT_CHAR*)&ucf, sizeof(ucf),false, true);
			return;
		}

		if (civil.iCurrentExp < cu.iCurrentExp)
		{
			//文明度不足
			CMD_G_LevelUpgradeFail ucf;
			ucf.dwUser = pUser->GetUserID();
			ucf.ucf    = LUF_NO_XP;
			pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_CIVILUPGRADE_FAIL, (CT_CHAR*)&ucf, sizeof(ucf), false, true);
		    return;
		}

		civil.iLevel += 1;
		civil.iCurrentExp += cu.iCurrentExp;
		pUser->m_userBase.UpdateMasterLevel(civil);

		G_SaveMastVariableData(pUser, SAVETAG_CIVIL_UPGRADE_FINISH);

		CMD_G_LevelUpgradeSuc ucs;
		memset(&ucs, 0, sizeof(ucs));
		ucs.dwUser = pUser->GetUserID();
		ucs.iLevel = civil.iLevel;
		ucs.iXp    = civil.iCurrentExp;
		pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_CIVILUPGRADE_SUC,(CT_CHAR*)&ucs,  sizeof(ucs), false, true);
#else
        pUser->m_userBase.CivilUpgrage();
#endif

	}
	return ;
}


//处理生产
CT_VOID CCBuildFunEvent::DisposeProduct(const CMD_G_Product& product)
{

	G_BuildingType bt;
	memset(&bt, 0, sizeof(bt));
	if(!CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(product.iBID, &bt))
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct : fail to find buildtype : %i", product.iBID);
		return ;
	}


	CCUser* pUser = 0;
	if(CCUserManager::getSingleton().Find(product.dwUserID, &pUser))
	{
		//vip add 如果建筑是vip特权建筑，判断vip特权
		int iPriID=0;
		if ((iPriID=G_GetVipPriIDByBuildID(product.iBID))>0)
		{
			if (!pUser->m_userVip.VipHasPrivilege(iPriID))
			{
				CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct: build id: %d need vip pri %d \n", product.iBID,iPriID );
				pUser->CloseNet();
				return;
			}
		}
		G_Building Build={0};
		if(!G_GetBuildingData(product.iBID,&Build))
		{
			CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeDBBuildingFinish G_GetBuildingData %d",bt.iID);
		}

		//洞穴生产
		if(CITY_TYPE_PATCAVE == Build.iBT)
		{
			DisposePetCave(pUser, product, bt.iID);
		}
		else
		{
			DisposeProduct(pUser, product, bt.iID);
		}        
	}
}

//生产
CT_VOID CCBuildFunEvent::DisposeProduct(CCUser* pUser,  const CMD_G_Product& product, int buildType)
{
 	G_BuildingProduct gbp = {0};
	if (!CCBuildingScriptSetting::getSingleton().GetProductType(product.iType, &gbp ) )
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct : fail to find producttype in GetProductType: %i", product.iType );
		return;
	}

	BuildingData bd = {0};
	if (! pUser->m_userBuilding.FindBuildData(product.iBID, bd) )
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct : no build, buildId : %i", product.iBID );
		return;
	}

	if (  gbp.iBuildGrade > bd.iGrade)
	{
		ProductFail(pUser, product, PF_ERR_PRODUCT_TYPE);	
		return ;
	}

	if ( pUser->m_userEvent.FindBuildingEventByID(product.iBID) ) //目前建筑有事件
	{
		ProductFail(pUser, product, PF_IN_EVENT);	
		return ;
	}

	if (pUser->m_userBuilding.FindBuildingProduct(product.iBID)) //目前建筑正在生产
	{
		ProductFail(pUser, product, PF_IN_PRODUCT);
		return ;	
	}

	if (pUser->m_userBuilding.FindUpgradeBuildData(product.iBID)) //目前建筑正在升级
	{
		ProductFail(pUser, product, PF_BUILDING_UPGRADE);
		return ;
	}

	if(pUser->m_userEvent.FindBuildingEventByID(product.iBID)) //建筑处于事件中
	{
		ProductFail(pUser, product, PF_BUINGING_EVENT);
		return ;
	}

	G_BuildingProductInfo bpi={0};
	if (!CCBuildingScriptSetting::getSingleton().GetProductInfo(product.iType, &bpi))
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct : fail to find producttype in FindProduct: %i", product.iType );
		return ;
	}

	MasterData md = {0};
	pUser->m_userBase.GetMasterData(md);
	if (md.mvd.stRes.iWorker + bpi.iWorkerCount < 0)
	{
		ProductFail(pUser, product, PF_NO_WORKER);
		return ;
	}

	if (md.mvd.stRes.iStamina + bpi.iStamina < 0)
	{
		ProductFail(pUser, product, PF_NO_STAMINA);
		return ;
	}

	MasterVariableData addValue={0};

	//if (buildType != CITY_TYPE_RESIDENTIAL)
	//{
	//	addValue.stRes.iWorker += bpi.iWorkerCount;//消耗工人
	//	addValue.stRes.iStamina += bpi.iStamina;   //消耗体力
	//}
	//增加森林，铁矿以及石矿,增加驯龙生产
	if (buildType != CITY_TYPE_FARMLAND && buildType != CITY_TYPE_PANNING && buildType != CITY_TYPE_FOREST && 
		buildType != CITY_TYPE_STONEORE && buildType != CITY_TYPE_IRONORE && buildType!=CITY_TYPE_TAMEDRAGON && buildType!=CITY_TYPE_RESFIELD)
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct : this building cannot product , %i", buildType);
		return;
	}
	addValue.stRes.iWorker += bpi.iWorkerCount;//消耗工人
	addValue.stRes.iStamina += bpi.iStamina;   //消耗体力
	//消耗金币和食物
	addValue.stRes.iFood += G_GetBuildProductGoldFood(bpi.iFood, bd.iGrade,bpi.fFoodCF,md.factor.fProductFactor)<0?G_GetBuildProductGoldFood(bpi.iFood, bd.iGrade,bpi.fFoodCF,md.factor.fProductFactor):0;
	addValue.stRes.iGold += G_GetBuildProductGoldFood(bpi.iGold, bd.iGrade,bpi.fGoldCF,md.factor.fProductFactor)<0?G_GetBuildProductGoldFood(bpi.iGold, bd.iGrade,bpi.fGoldCF,md.factor.fProductFactor):0;

	pUser->m_userBase.AddMastData(addValue, SAVETAG_PRODUCT_START);


	//重新读取数据
	pUser->m_userBase.GetMasterData(md);

	ProductingBuilding bp = {0};
	bp.iBid    = product.iBID;
	bp.iType  = product.iType;
	bp.llTime = time(0);
	bp.bSendCanHavest = CT_FALSE;
	bp.iWorker      = bpi.iWorkerCount;
	//民居不在这里处理
	//if (buildType == CITY_TYPE_RESIDENTIAL)
	//{
	//	bp.iLastingTime = 0 ;
	//}else
	//{
	//	bp.iLastingTime = bpi.iTime;
	//}
	bp.iLastingTime = bpi.iTime;
	bp.iBType       = buildType;
	bp.iBGrade      = bd.iGrade;
	bp.fSeekAddition=1.0f;
	pUser->m_userBuilding.InsertBuildingProductMap(bp);

    CMD_G_ProductSuc suc = {0};
	suc.dwUserID   = product.dwUserID;
	suc.iWorker    = md.mvd.stRes.iWorker;
	suc.iStamina   = md.mvd.stRes.iStamina;
	suc.iBID       = product.iBID;
	suc.iGold      = md.mvd.stRes.iGold;
	suc.iFood      = md.mvd.stRes.iFood;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_SUC, (CT_CHAR*)&suc, sizeof(suc));
	

	//MSC_DB_SaveProductingData spd = {0};
	//spd.dwUserID = product.dwUserID;
	//spd.productingBuilding = bp;
	//spd.iStamina           = addValue.stRes.iStamina;
	//CT_INT32 threadID =  GAME_THREAD_DATABASE + product.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	//G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_PRODUCTING_DATA , (CT_CHAR*)&spd, sizeof(spd));
	

	//请求生产成功，添加生产任务判断
	//add by caihan 11-14
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_PRODUCT;
	taskEventInfo.iSubType = gbp.iBuildType;
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end

	//指定类型生产任务判断
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_PRODUCT;
	taskSysInfo.arrayParam[0] = bp.iBType;          //参数0：建筑类型
	taskSysInfo.arrayParam[1] = bp.iBid;            //参数1：建筑id
	taskSysInfo.arrayParam[2] = bp.iType;           //参数2：生产类型
	taskSysInfo.iProgress = 1;
	pUser->m_userTask.DisposeTask(taskSysInfo);

    pUser->m_userDaily.UpdateQuestData(DAILY_BUILDIND_PRODUCT, 1);
}

//处理洞穴生产
CT_VOID CCBuildFunEvent::DisposePetCave(CCUser* pUser, const CMD_G_Product& product, int buildType)
{
	//探索类型
	MasterData md = {0};
	pUser->m_userBase.GetMasterData(md);
	G_PetCave gbp = {0};
	if (!CCBuildingScriptSetting::getSingleton().GetPetCave(product.iType,md.mvd.stGrade.iLevel, &gbp ) )
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposePetCave : fail to find producttype in GetPetCave: %i", product.iType );
		return;
	}

	//验证等级
	if(md.mvd.stGrade.iLevel < gbp.iGrade)
	{
		ProductFail(pUser, product, PE_PRODUCT_GRADE);
		return;
	}

	//检测是否拥有这个建筑
	BuildingData bd = {0};
	if (! pUser->m_userBuilding.FindBuildData(product.iBID, bd) )
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct : no build, buildId : %i", product.iBID );
		return;
	}

	if (pUser->m_userBuilding.FindBuildingProduct(product.iBID)) //目前建筑正在生产
	{
		ProductFail(pUser, product, PF_IN_PRODUCT);
		return ;	
	}


	if ( pUser->m_userEvent.FindBuildingEventByID(product.iBID) ) //目前建筑有事件
	{
		ProductFail(pUser, product, PF_IN_EVENT);	
		return ;
	}

	if(pUser->m_userEvent.FindBuildingEventByID(product.iBID)) //建筑处于事件中
	{
		ProductFail(pUser, product, PF_BUINGING_EVENT);
		return ;
	}

	//消耗验证
	if (md.mvd.stRes.iWorker + gbp.iCostWorker < 0)
	{
		ProductFail(pUser, product, PF_NO_WORKER);
		return ;
	}

	if (md.mvd.stRes.iDiamod + gbp.iCostDiamond < 0)
	{
		ProductFail(pUser, product, PE_NO_DIAMOD);
		return ;
	}

	if (md.mvd.stRes.iGold + gbp.iCostGold < 0)
	{
		ProductFail(pUser, product, PE_NO_GOLD);
		return ;
	}

	if (md.mvd.stRes.iStamina + gbp.iCostPower < 0)
	{
		ProductFail(pUser, product, PF_NO_STAMINA);
		return ;
	}

	MasterVariableData addValue={0};

	//建筑类型
	if (buildType != CITY_TYPE_PATCAVE)
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposePetCave : this building cannot product , %i", buildType);
		return;
	}
	addValue.stRes.iWorker += gbp.iCostWorker;//消耗工人
	addValue.stRes.iStamina += gbp.iCostPower;   //消耗体力
	addValue.stRes.iGold += gbp.iCostGold;		//消耗金币
	addValue.stRes.iDiamod += gbp.iCostDiamond;	//消耗钻石


	pUser->m_userBase.AddMastData(addValue, SAVETAG_PRODUCT_START);


	//重新读取数据
	pUser->m_userBase.GetMasterData(md);

	ProductingBuilding bp = {0};
	bp.iBid    = product.iBID;
	bp.iType  = product.iType;
	bp.llTime = time(0);
	bp.bSendCanHavest = CT_FALSE;
	bp.iWorker      = gbp.iCostWorker;
	bp.iLastingTime = gbp.iCostTime;
	bp.iBType       = buildType;
	bp.iBGrade      = bd.iGrade;
	pUser->m_userBuilding.InsertBuildingProductMap(bp);

	CMD_G_CaveProductSuc suc = {0};
	suc.dwUserID   = product.dwUserID;
	suc.iWorker    = md.mvd.stRes.iWorker;
	suc.iStamina   = md.mvd.stRes.iStamina;
	suc.iBID       = product.iBID;
	suc.iDiamod    = md.mvd.stRes.iDiamod;
	suc.iGold	   = md.mvd.stRes.iGold;
	suc.iTime	   = gbp.iCostTime;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_CAVE_PRODUCT_SUC, (CT_CHAR*)&suc, sizeof(suc));


	/*MSC_DB_SaveProductingData spd = {0};
	spd.dwUserID = product.dwUserID;
	spd.productingBuilding = bp;
	spd.iStamina           = addValue.stRes.iStamina;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + product.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_PRODUCTING_DATA , (CT_CHAR*)&spd, sizeof(spd));*/

	//请求生产成功，添加生产任务判断
	//add by caihan 11-14
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_PRODUCT;
	taskEventInfo.iSubType = buildType;
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end

	//指定类型生产任务判断
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_PRODUCT;
	taskSysInfo.arrayParam[0] = bp.iBType;          //参数0：建筑类型
	taskSysInfo.arrayParam[1] = bp.iBid;            //参数1：建筑id
	taskSysInfo.arrayParam[2] = bp.iType;           //参数2：生产类型
	taskSysInfo.iProgress = 1;
	pUser->m_userTask.DisposeTask(taskSysInfo);

	return;
}

//处理请求收获
CT_VOID CCBuildFunEvent::DisposeHarvest(const CMD_G_RequestHavest& harvest)
{
	CCUser* pUser = 0;
	if(CCUserManager::getSingleton().Find(harvest.dwUserID, &pUser))
	{
		ProductingBuilding pb = {0};
		if(!pUser->m_userBuilding.FindBuildingProduct(harvest.iBuildID, pb) && !pUser->m_userBuilding.FindAutoBuilding(harvest.iBuildID, pb))
		{
			CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeHarvest : fail to find producting building id = %i", pb.iBid);
			return;
		}

		//vip add 如果建筑是vip特权建筑，判断vip特权
		/*	VIP到期之前的生产, 允许收获
		int iPriID=0;
		if ((iPriID=G_GetVipPriIDByBuildID(harvest.iBuildID))>0)
		{
			if (!pUser->m_userVip.VipHasPrivilege(iPriID))
			{
				CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeHarvest: build id: %d need vip pri %d \n", harvest.iBuildID,iPriID );
				pUser->CloseNet();
				return;
			}
		} */

		if(!pb.bSendCanHavest)
		{
			CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeHarvest : didn't send harvest message,  building id = %i", pb.iBid);
			return;	
		}

		G_BuildingType bt;
		memset(&bt, 0, sizeof(bt));
		if(!CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(harvest.iBuildID, &bt))
		{
			CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeHarvest : fail to find buildtype : %i", harvest.iBuildID);
			return ;
		}

		G_Building Build={0};
		if(!G_GetBuildingData(harvest.iBuildID, &Build))
		{
			CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeHarvest : fail to find buildtype : %d", harvest.iBuildID);
			return ;
		}

		G_BuildingProductInfo bpi={0};
		//洞穴收获
 		if(Build.iBT == CITY_TYPE_PATCAVE)
		{
			G_PetCave gbp = {0};
			MasterVariableData mvd={0};
			pUser->m_userBase.GetMasterData(mvd);
			if (!CCBuildingScriptSetting::getSingleton().GetPetCave(pb.iType,mvd.stGrade.iLevel, &gbp ) )
			{
				CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeHarvest : fail to find producttype in GetPetCave: %i", pb.iType );
				return;
			}

			//发放奖励
			G_CityFinishCave(pUser, m_dwThread, bt,gbp, pb);
		}
		else
		{
			
			if (!CCBuildingScriptSetting::getSingleton().GetProductInfo(pb.iType , &bpi))
			{
				return ;
			}

			//探访圣女被绑架
			G_ValueRange vr={0};
			Seekers seekersInfo={0};
			if(!G_GetValueRange(VALUE_TYPE_PLUNDER_WOMAN_BUFFER,&vr))
			{
				CCLogMeg::getSingleton().WriteWaring("CCBuildFunEvent::DisposeHarvest G_GetValueRange VALUE_TYPE_PLUNDER_WOMAN_BUFFER");
			}
			CCFightingData::getSingleton().GetSeekedInfo(pUser->GetUserID(),seekersInfo);
			pb.fSeekAddition=(seekersInfo.iKidnapSaintess !=0?1.0+vr.fMaxVal:1.0f);

			//发放奖励
			G_CityFinishProduct(pUser, m_dwThread, bpi, bt, pb);
		}



		if (Build.iBT == CITY_TYPE_RESIDENTIAL)
		{
			pb.iLastingTime   = bpi.iTime;
			pb.bSendCanHavest = CT_FALSE;
			pb.llTime         = time(0);
			pUser->m_userBuilding.UpdateAutoBuilding(pb.iBid, pb);
		}else
		{
			pUser->m_userBuilding.DeleteBuildingProductMap(harvest.iBuildID);
		}

		//zhoujun	重新推送之前建筑忙时的延迟任务
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = MAIN_G_BUILDING_EVENT_CITY;
		taskEventInfo.iTaskSubType = SUB_G_BUILDING_PRODUCT_APPLY_FINISH;
		taskEventInfo.iSubType = Build.iBT;
		taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
		pUser->m_userTask.DisposeTask(taskEventInfo);
	}
}

//处理生产错误
CT_VOID CCBuildFunEvent::ProductFail(CCUser* pUser, const CMD_G_Product& product,  int pf)
{
	CMD_G_ProductFail gpf = {0};
	gpf.dwUserID = product.dwUserID;
	gpf.iBID     = product.iBID;
	gpf.pf       = (ENUM_ProductFail)pf;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_FAIL, (CT_CHAR*)&gpf, sizeof(gpf));
}

//处理生产加速
CT_VOID CCBuildFunEvent::DisposeProductBuildSpeeUp(const CMD_G_ProductBuildSpeedUp& bsu)
{
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(bsu.dwUserID, &pUser) )
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProductBuildSpeeUp: fail to find user , id=%i", bsu.dwUserID);
		return;
	}

	CMD_G_ProductBuildSpeedUpFail pbsf = {0};
	pbsf.dwUserID = bsu.dwUserID;
	pbsf.iBuildID = bsu.iBuildID;
	
	ProductingBuilding pb = {0};
	if (!pUser->m_userBuilding.FindBuildingProduct(bsu.iBuildID, pb))
	{
		CCLogMeg::getSingleton().WriteError(" CCBuildFunEvent::DisposeProductBuildSpeeUp: fail to find producting build ,buildID = %i  ",bsu.iBuildID);
		pbsf.pbsf = PBSUF_NO_PRODUCTING_BUILD;
		pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_BUILDING_PRODUCT_SPEED_UP_FAIL, (CT_CHAR*)&pbsf, sizeof(pbsf) );
		return ;
	}

	if(CITY_TYPE_FARMLAND != pb.iBType && CITY_TYPE_PANNING != pb.iBType && CITY_TYPE_FOREST != pb.iBType && CITY_TYPE_STONEORE != pb.iBType && 
		CITY_TYPE_IRONORE != pb.iBType && CITY_TYPE_PATCAVE != pb.iBType && CITY_TYPE_TAMEDRAGON!=pb.iBType && CITY_TYPE_RESFIELD!=pb.iBType)
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProductBuildSpeeUp: this build can't speed up bid=%i  cid=%i", pb.iBid, pb.iBType);
		pbsf.pbsf = PBSUF_CAN_NOT_PRODUCT_BUILD;
        pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_BUILDING_PRODUCT_SPEED_UP_FAIL, (CT_CHAR*)&pbsf, sizeof(pbsf) );
		return ;
	}

	G_ValueRange vr = {0};
	if (CITY_TYPE_FARMLAND == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_FARMSPEEDUP, &vr);
	}else if (CITY_TYPE_PANNING == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_PANNINGSPEEDUP, &vr);
	}
	else if (CITY_TYPE_FOREST == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_FORESTSPEEDUP, &vr);
	}
	else if (CITY_TYPE_STONEORE == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_STONESPEEDUP, &vr);
	}
	else if (CITY_TYPE_IRONORE == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_IRONSPPEDUP, &vr);
	}
	else if (CITY_TYPE_PATCAVE == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_CAVESPPEDUP, &vr);
	}
	else if (CITY_TYPE_TAMEDRAGON == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_TAMEDRAGONSPEEDUP, &vr);
	}
	else if(CITY_TYPE_RESFIELD == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_RESFIELDSPEEDUP, &vr);
	}

	CT_INT32 leftTime   =   pb.iLastingTime - (CT_INT32)(time(0) - pb.llTime);
	leftTime=(leftTime < 0?0:leftTime);
	CT_INT32 needDiamod =  G_GetProductSpeedDiamod(leftTime , vr.fMaxVal) - 1;//钻石消耗+1，避免0钻石加速情况

	if (leftTime < 0)
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::DisposeProductBuildSpeeUp: speed up need %i diamond", needDiamod);
		return ;
	}
    
	//钻石消耗为负数
	MasterData md = {0};
	pUser->m_userBase.GetMasterData(md);
	if (md.mvd.stRes.iDiamod + needDiamod < 0)
	{
		pbsf.pbsf     = PBSUF_NO_DIAMOD;
		pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_SPEED_UP_FAIL, (CT_CHAR*)&pbsf, sizeof(pbsf));
		return ;
	}


    MasterVariableData mvd = {0};
    mvd.stRes.iDiamod += needDiamod;
    pUser->m_userBase.AddMastData(mvd, SAVETAG_BUILDING_PRODUCT_SPEE_UP_FINISH);

    pUser->m_userBase.GetMasterData(md);
	CMD_G_ProductBuildSpeedUpSuc pbss = {0};
	pbss.dwUserID = bsu.dwUserID;
	pbss.iBuildID = bsu.iBuildID;
	pbss.iDiamod  = md.mvd.stRes.iDiamod;

	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_SPEED_UP_SUC, (CT_CHAR*)&pbss, sizeof(pbss));
	pb.iLastingTime = 0;
	pb.iDiamod = needDiamod;
	//生产加速直接通知可收获，不在由定时器推送
	pb.bSendCanHavest = CT_TRUE;
	pUser->m_userBuilding.UpdateBuildingProduct(bsu.iBuildID,  pb);
	//到达收获条件，释放工人
	pUser->CountFreeWorker();
	CMD_G_ProductHarvest ch = {0};
	ch.dwUserID = pUser->GetUserID();
	ch.iBuildID = pb.iBid;
	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);
	ch.iWorker = mvd.stRes.iWorker;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_CAN_HARVEST,(CT_CHAR*)&ch, sizeof(ch));
}

CT_VOID CCBuildFunEvent::GetProductBuildSpeedRequest(const CMD_G_ProductBuildSpeedUp& bsu)
{
	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(bsu.dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::GetProductBuildSpeedRequest: fail to find user , id=%i", bsu.dwUserID);
		return;
	}
	CMD_G_ProductBuildSpeedUpFail pbsf = {0};
	pbsf.dwUserID = bsu.dwUserID;
	pbsf.iBuildID = bsu.iBuildID;

	ProductingBuilding pb = {0};
	if (!pUser->m_userBuilding.FindBuildingProduct(bsu.iBuildID, pb))
	{
		CCLogMeg::getSingleton().WriteError(" CCBuildFunEvent::DisposeProductBuildSpeeUp: fail to find producting build ,buildID = %i  ",bsu.iBuildID);
		pbsf.pbsf = PBSUF_NO_PRODUCTING_BUILD;
		pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_BUILDING_PRODUCT_SPEED_UP_FAIL, (CT_CHAR*)&pbsf, sizeof(pbsf) );
		return ;
	}

	if(CITY_TYPE_FARMLAND != pb.iBType && CITY_TYPE_PANNING != pb.iBType && CITY_TYPE_FOREST != pb.iBType && CITY_TYPE_STONEORE != pb.iBType && 
		CITY_TYPE_IRONORE != pb.iBType&& CITY_TYPE_PATCAVE != pb.iBType && CITY_TYPE_TAMEDRAGON!=pb.iBType && CITY_TYPE_RESFIELD!=pb.iBType)
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProductBuildSpeeUp: this build can't speed up bid=%i  cid=%i", pb.iBid, pb.iBType);
		pbsf.pbsf = PBSUF_CAN_NOT_PRODUCT_BUILD;
		pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY,  SUB_G_BUILDING_PRODUCT_SPEED_UP_FAIL, (CT_CHAR*)&pbsf, sizeof(pbsf) );
		return ;
	}

	G_ValueRange vr = {0};
	if (CITY_TYPE_FARMLAND == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_FARMSPEEDUP, &vr);
	}else if (CITY_TYPE_PANNING == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_PANNINGSPEEDUP, &vr);
	}
	else if (CITY_TYPE_FOREST == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_FORESTSPEEDUP, &vr);
	}
	else if (CITY_TYPE_STONEORE == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_STONESPEEDUP, &vr);
	}
	else if (CITY_TYPE_IRONORE == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_IRONSPPEDUP, &vr);
	}
	else if (CITY_TYPE_PATCAVE == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_CAVESPPEDUP, &vr);
	}
	else if (CITY_TYPE_TAMEDRAGON == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_TAMEDRAGONSPEEDUP, &vr);
	}
	else if(CITY_TYPE_RESFIELD == pb.iBType)
	{
		G_GetValueRange(VALUE_TYPE_RESFIELDSPEEDUP, &vr);
	}
	
	CT_INT32 leftTime   =   pb.iLastingTime - (CT_INT32)(time(0) - pb.llTime);
	leftTime=(leftTime < 0?0:leftTime);
	CT_INT32 needDiamod =  (CT_INT32)(leftTime * vr.fMaxVal) - 1;   //钻石消耗+1，避免0钻石加速情况

	if (leftTime < 0)
	{
		CCLogMeg::getSingleton().WriteError("CCCityUserBuildEvent::GetProductBuildSpeedRequest: speed up need %i diamond", needDiamod);
		return ;
	}

	CMD_G_SpeedUpNeedResource nd = {0};
	nd.dwUserID = bsu.dwUserID;
	nd.iBuildID = bsu.iBuildID;
	nd.iDiamod = needDiamod;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY,SUB_G_BUILDING_PRODUCT_SPEED_UP_REQUEST,(CT_CHAR*)&nd,sizeof(nd));
}
   

