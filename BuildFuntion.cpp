#include <ctime>
#include "BuildFuntion.h"
#include "CUser.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"
#include "CNetInterface.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CDatabaseMes.h"
#include "CGameConfig.h"
#include "CFightingScriptSetting.h"
#include "CLogMeg.h"
#include "CFormula.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CActive.h"
#include "CBroadcastScriptSetting.h"

#define ACTIVE_FARMLAND_ID			4202
#define ACTIVE_PANNING_ID			4201
#define ACTIVE_TAMEDRAGON_ID		4203
CT_VOID G_CityHarvestGainFail(CCUser *pUser ,CT_INT32 iBID, int pf)
{
	CMD_G_ProductFail gpf = {0};
	gpf.dwUserID = pUser->GetUserID();
	gpf.iBID     = iBID;
	gpf.pf       = (ENUM_ProductFail)pf;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY,SUB_G_BUILDING_PRODUCT_GAIN_FAIL,(CT_CHAR*)&gpf,sizeof(gpf));
}

CT_VOID G_CityFinishProduct(CCUser* pUser, const CT_DWORD dwThread, const G_BuildingProductInfo& bpi, const G_BuildingType& bt,  const ProductingBuilding& pb )
{
	CMD_GProductFinish gf = {0};
	gf.dwUserID = pUser->GetUserID();
  	gf.iBuildID = pb.iBid;
	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);
	CT_DWORD dwTeamID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pUser->GetUserID());
	MasterVariableData mvd = {0};

	CT_FLOAT fProductFactor=0.0f;
	CT_INT32 AcviteList[100] = {0};
	CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_PRODUCTION,AcviteList);
	for(CT_INT32 i=0;i<n && i<100;++i)
	{
		G_ActiveEventData aed={0};
		if(G_GetActiveEvent(AcviteList[i],&aed))
		{
			fProductFactor=aed.iProperty1;
			fProductFactor=fProductFactor/100.0f;
		}
	}
	fProductFactor+=masterData.factor.fProductFactor;

	CT_INT32 iItemProduct[3]={0};
	CT_INT32 iNum=0;
	memset(AcviteList,0,sizeof(AcviteList));
	n=CCActive::getSingleton().GetActiveByType(ACTIVE_BUILDPRODUCT,AcviteList);
	for(CT_INT32 i=0;i<n && i<100;++i)
	{
		if(iNum < 3 && (AcviteList[i] == ACTIVE_FARMLAND_ID || AcviteList[i] == ACTIVE_PANNING_ID || AcviteList[i] == ACTIVE_TAMEDRAGON_ID))
		{
			iItemProduct[iNum++]=AcviteList[i];
		}
	}

	if (bt.iID == CITY_TYPE_RESIDENTIAL)
	{
		G_ValueRange gVr={0};
		G_GetValueRange(VALUE_TYPE_RESIDENTIALPRODUCT, &gVr);
		G_ValueRange gVMaxTime = {0};
		G_GetValueRange(VALUE_TYPE_HOUSESTORE, &gVMaxTime);
		//民居生产最小时间
		G_ValueRange gMinTime = {0};
		G_GetValueRange(VALUE_TYPE_HOUSEINTERVAL,&gMinTime);
		G_ValueRange gXpBase={0};
		if(!G_GetValueRange(VALUE_TYPE_RESIDENTIALBASE,&gXpBase))
		{
			CCLogMeg::getSingleton().WriteError("G_CityFinishProduct G_GetValueRange VALUE_TYPE_RESIDENTIALBASE");
		}
		CT_INT32 maxTime = (CT_INT32)(time(0) - pb.llTime);
        maxTime          = (CT_INT32)(maxTime > gVMaxTime.fMaxVal ?gVMaxTime.fMaxVal : maxTime);
		maxTime = (CT_INT32)(maxTime<gMinTime.fMaxVal?gMinTime.fMaxVal:maxTime);
		//民居收获金币增加内政系数
		mvd.stGrade.iCurrentExp  +=  G_M_GetHouseStore((CT_INT32)gXpBase.fMaxVal,gVr.fMaxVal,maxTime,pb.iBGrade,masterData.factor.fProductFactor);
		mvd.stGrade.iCurrentExp  +=  pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stGrade.iCurrentExp,bt.iID);
		//mvd.stGrade.iCurrentExp  +=  (CT_INT32)((gXpBase.fMaxVal + (CT_INT32)(gVr.fMaxVal* maxTime  * pb.iBGrade))*masterData.factor.fProductFactor);
		mvd.stCivili.iCurrentExp += bpi.iCivil + (CT_INT32)(gVr.fMaxVal* maxTime  * pb.iBGrade);
		mvd.stGrade.iCurrentExp  +=  pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
	}

	if (bt.iID == CITY_TYPE_FARMLAND)
	{
		mvd.stRes.iFood += G_GetBuildProductGoldFood(bpi.iFood,pb.iBGrade,bpi.fFoodCF,fProductFactor);
		mvd.stRes.iFood += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stRes.iFood,bt.iID);
		mvd.stRes.iFood *=pb.fSeekAddition;
		mvd.stCivili.iCurrentExp += bpi.iCivil * pb.iBGrade;
		mvd.stCivili.iCurrentExp += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
//		mvd.stRes.iWorker -= bpi.iWorkerCount; //恢复工人数目
	}

	if (bt.iID == CITY_TYPE_PANNING || bt.iID == CITY_TYPE_TAMEDRAGON)
	{
		mvd.stRes.iGold   += G_GetBuildProductGoldFood(bpi.iGold,pb.iBGrade,bpi.fGoldCF,fProductFactor);
		mvd.stRes.iGold	  += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stRes.iGold,bt.iID);
		if(bt.iID == CITY_TYPE_PANNING)
			mvd.stRes.iGold *=pb.fSeekAddition;
		mvd.stCivili.iCurrentExp += bpi.iCivil * pb.iBGrade;
		mvd.stCivili.iCurrentExp += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
	}
	
	//木材，铁矿，石头属于道具，增加到背包
	if (bt.iID == CITY_TYPE_FOREST)
	{
		CT_INT32 count = bpi.iItemNum * pb.iBGrade;
		count+=pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,count,bt.iID);
		if((pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,count))>0)
		{
			G_CityHarvestGainFail(pUser,pb.iBid,PF_BAG_NO_SPACE);
			return;
		}
		mvd.stCivili.iCurrentExp += bpi.iCivil * pb.iBGrade;
		mvd.stCivili.iCurrentExp += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
	}
	else if((bt.iID == CITY_TYPE_FARMLAND || bt.iID == CITY_TYPE_PANNING || bt.iID == CITY_TYPE_TAMEDRAGON))
	{
		for(CT_INT32 i=0;i<3;++i)
		{
			if((iItemProduct[i] == ACTIVE_FARMLAND_ID && bt.iID == CITY_TYPE_FARMLAND) || 
				(iItemProduct[i] == ACTIVE_PANNING_ID && bt.iID == CITY_TYPE_PANNING))
			{
				CT_INT32 iCount=bpi.iItemNum;
				if((pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,iCount,ITEM_OP_MODE_CHECK))>0)
				{
					G_CityHarvestGainFail(pUser,pb.iBid,PF_BAG_NO_SPACE);
					return;
				}
				pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,iCount,ITEM_OP_MODE_EXEC);
				mvd.stCivili.iCurrentExp += bpi.iCivil * pb.iBGrade;
				mvd.stCivili.iCurrentExp += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
			}
		}
		if(bt.iID == CITY_TYPE_TAMEDRAGON)
		{
			CT_INT32 iCount=bpi.iItemNum;
			if((pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,iCount,ITEM_OP_MODE_CHECK))>0)
			{
				G_CityHarvestGainFail(pUser,pb.iBid,PF_BAG_NO_SPACE);
				return;
			}
			pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,iCount,ITEM_OP_MODE_EXEC);
		}
	}

	if (bt.iID == CITY_TYPE_STONEORE)
	{
		CT_INT32 count = bpi.iItemNum * pb.iBGrade;
		count+=pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,count,bt.iID);
		if((pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,count))>0)
		{
			G_CityHarvestGainFail(pUser,pb.iBid,PF_BAG_NO_SPACE);
			return;
		}
		mvd.stCivili.iCurrentExp += bpi.iCivil * pb.iBGrade;
		mvd.stCivili.iCurrentExp += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
	}

	if (bt.iID == CITY_TYPE_IRONORE)
	{
		CT_INT32 count = bpi.iItemNum * pb.iBGrade;
		count+=pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,count,bt.iID);
		if((pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,count))>0)
		{
			G_CityHarvestGainFail(pUser,pb.iBid,PF_BAG_NO_SPACE);
			return;
		}
		mvd.stCivili.iCurrentExp += bpi.iCivil * pb.iBGrade;
		mvd.stCivili.iCurrentExp += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
	}

	if(bt.iID == CITY_TYPE_RESFIELD)
	{
		CT_INT32 count = bpi.iItemNum * pb.iBGrade;
		count+=pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,count,bt.iID);
		if((pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,bpi.iItemID,count))>0)
		{
			G_CityHarvestGainFail(pUser,pb.iBid,PF_BAG_NO_SPACE);
			return;
		}
		mvd.stCivili.iCurrentExp += bpi.iCivil * pb.iBGrade;
		mvd.stCivili.iCurrentExp += pUser->m_userFightTeamNember.ScienceBulidAdd(dwTeamID,mvd.stCivili.iCurrentExp,bt.iID);
	}

	
	pUser->m_userBase.AddMastData(mvd, SAVETAG_PRODUCT_FINISH);

	MasterData md = {0};
	pUser->m_userBase.GetMasterData(md);
	gf.iFood   = md.mvd.stRes.iFood;
	gf.iGold   = md.mvd.stRes.iGold; 
	gf.iWorker = md.mvd.stRes.iWorker;
	gf.iCivil  = md.mvd.stCivili.iCurrentExp;
	gf.iExperience=md.mvd.stGrade.iCurrentExp;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_FINISH,  (CT_CHAR*)&gf, sizeof(gf) );

	CT_INT32 threadID =  GAME_THREAD_DATABASE + gf.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	
	//G_SaveMastVariableData(pUser, SAVETAG_PRODUCT_FINISH);  //保存数据

	ProductedBuildingResultKey key={0};
	key.iBid = pb.iBid;
	key.iProp1 = bpi.iItemID;
	key.iProp2 = 0;

	if (pb.iDiamod != 0)
	{
		pUser->m_userBuilding.UpdateSaveProductingData(CT_FALSE, CT_TRUE);  //强制将这个用户的生产数据刷新到数据库，防止钻石加速和普通数据写到了一起
	}
	ProductedBuildingResult    result={0};
	result.iCivil = mvd.stCivili.iCurrentExp;
	result.iDiamod = pb.iDiamod;
	result.iFood   = mvd.stRes.iFood;
	result.iGold   = mvd.stRes.iGold;
	result.iProductingTimes = 1;
	result.iProp1Num = bpi.iItemNum;
	result.iProp2Num = 0;
	result.iStamina = bpi.iStamina;
	result.iWorker  = bpi.iWorkerCount;
	pUser->m_userBuilding.AddProductedBuildingResult(key, result);

	if (pb.iDiamod != 0)
	{
		pUser->m_userBuilding.UpdateSaveProductingData(CT_FALSE, CT_TRUE);
	}

	//收获完成，增加收获任务判断
	//add by caihan 11-14
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_HAVEST;
	taskEventInfo.iSubType = bt.iID;
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
}

//处理洞穴收获
CT_VOID G_CityFinishCave(CCUser* pUser, const CT_DWORD dwThread,const G_BuildingType& bt,const G_PetCave &pc,const ProductingBuilding& pb)
{
	G_BuildingType Build={0};
	if(!G_GetBuildingTypeData(CITY_TYPE_PATCAVE,&Build))
	{
		CCLogMeg::getSingleton().WriteError("CCBuildFunEvent::DisposeProduct G_GetBuildingTypeData");
		return;
	}

	if (bt.iID != Build.iID)
	{
		return;
	}

	MasterVariableData mvd = {0};
	CMD_GCaveProductFinish cpf={0};

	//随机奖励
	CT_INT32 iWight=0;
	iWight+=pc.iGetFoodWight1;
	iWight+=pc.iGetFoodWight2;
	iWight+=pc.iGetGoldWight1;
	iWight+=pc.iGetGoldWight1;
	iWight+=pc.iPropWight;

	if(iWight != 0)
	{
		CT_INT32 iRand=rand()%iWight;
		if(iRand < pc.iGetFoodWight1)
		{
			mvd.stRes.iFood+=pc.iGetFood1;
		}
		else if(iRand < pc.iGetFoodWight1+pc.iGetFoodWight2)
		{
			mvd.stRes.iFood+=pc.iGetFood2;
		}
		else if(iRand < pc.iGetFoodWight1+pc.iGetFoodWight2+pc.iGetGoldWight1)
		{
			mvd.stRes.iGold+=pc.iGetGold1;
		}
		else if(iRand < pc.iGetFoodWight1+pc.iGetFoodWight2+pc.iGetGoldWight1+pc.iGetGoldWight2)
		{
			mvd.stRes.iGold+=pc.iGetGold2;
		}
		else
		{
			//道具奖励
			CT_INT32 iGoods[GOODS_COUNT]={0};
			CT_INT32 iCount[GOODS_COUNT]={0};
			FakeProBase fb={0};
			G_GetFakeProDta(pUser,SAVETAGEEX_FINISH_CAVE,fb);
			G_GetGoodsGroupInfo(pc.iPropGroupeID,iGoods,iCount,fb);
			G_ClearFakeProDat(pUser,SAVETAGEEX_FINISH_CAVE,fb);
			CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			pUser->m_userItem.AutoAddItemData(SAVETAG_PRODUCT_FINISH,iGoods,iCount,2);

			for(CT_INT32 i=0;i<GOODS_COUNT;++i)
			{
				if (G_BroadcastFilter(3, iGoods[i]))
				{
					RollBroadcast rb[2]={0};
					MasterData masterData = {0};
					pUser->m_userBase.GetMasterData(masterData);
					rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
					rb[1].SetValue(iGoods[i], "");
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{6}<2:%d>!", pUser->GetUserID(), masterData.cNickName,iGoods[i]);
					G_SendRollBroadecast(121,3,1,(CT_CHAR*)rb,sizeof(rb));
				}
			}

		//	CT_INT32 iGoods[GOODS_COUNT]={0};
		//	G_GoodsCount GoodsCount={0};
		//	G_GoodsGroup GoodsGroup={0};
		//	G_GoodsWeights GoodsWeights={0};
		//	CCFightingScriptSetting::getSingleton().GetGoodsCount(pc.iPropGroupeID,GoodsCount);
		//	CCFightingScriptSetting::getSingleton().GetGoodsGroup(pc.iPropGroupeID,GoodsGroup);
		//	CCFightingScriptSetting::getSingleton().GetGoodsWeights(pc.iPropGroupeID,GoodsWeights);

		//	CT_INT32 iWeights=0;
		//	for(CT_INT32 i=0;i<GOODS_COUNT;++i)
		//	{
		//		iWeights+=GoodsWeights.iWeights[i];
		//	}

		//	CT_INT32 iGoodsCount=0;
		//	for(CT_INT32 i=0;i<GoodsWeights.iDropCount;++i)
		//	{
		//		CT_INT32 iRand=rand()%iWeights;

		//		CT_INT32 iWeightsRand=0;
		//		for(CT_INT32 j=0;j<GOODS_COUNT;++j)
		//		{
		//			iWeightsRand+=GoodsWeights.iWeights[j];
		//			if(GoodsGroup.iGoodsID[j] != 0 && GoodsCount.iCount[j] != 0 && iWeightsRand >= iRand)
		//			{
		//				cpf.iGoods=GoodsGroup.iGoodsID[j];
		//				cpf.iCount=GoodsCount.iCount[j];
		//				iGoodsCount++;


		//				CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		//				pUser->m_userItem.AutoAddItemData(threadid,GoodsGroup.iGoodsID[j],GoodsCount.iCount[j]);

		//				break;
		//			}
		//		}
		//	}
		}
	}
	pUser->m_userBase.AddMastData(mvd, SAVETAG_PRODUCT_FINISH);

	MasterData md = {0};
	pUser->m_userBase.GetMasterData(md);
	cpf.dwUserID=pUser->GetUserID();
	cpf.iBuildID=pb.iBid;
	cpf.iFood   = md.mvd.stRes.iFood;
	cpf.iGold   = md.mvd.stRes.iGold; 
	cpf.iWorker = md.mvd.stRes.iWorker;
	pUser->GetNetInterface()->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_CAVE_PRODUCT_FINISH,  (CT_CHAR*)&cpf, sizeof(cpf) );

	//G_SaveMastVariableData(pUser, SAVETAG_PRODUCT_FINISH);  //保存数据

	if (pb.iDiamod != 0)
	{
		pUser->m_userBuilding.UpdateSaveProductingData(CT_FALSE, CT_TRUE);
	}
	ProductedBuildingResultKey key={0};
	key.iBid = pb.iBid;
	key.iProp1 = cpf.iGoods;
	key.iProp2 = 0;
	ProductedBuildingResult    result={0};
	result.iCivil = mvd.stCivili.iCurrentExp;
	result.iDiamod = pb.iDiamod;
	result.iFood   = mvd.stRes.iFood;
	result.iGold   = mvd.stRes.iGold;
	result.iProductingTimes = 1;
	result.iProp1Num = cpf.iCount;
	result.iProp2Num = 0;
	result.iStamina = pc.iCostPower;
	result.iWorker  = pc.iCostWorker;
	pUser->m_userBuilding.AddProductedBuildingResult(key, result);
	if (pb.iDiamod != 0)
	{
		pUser->m_userBuilding.UpdateSaveProductingData(CT_FALSE, CT_TRUE);
	}


	//收获完成，增加收获任务判断
	//add by caihan 11-14
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_HAVEST;
	taskEventInfo.iSubType = bt.iID;
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
	return;
}