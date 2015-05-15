#include <ctime>
#include "CUserBuilding.h"
#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CUser.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "CBuildingScriptSetting.h"
#include "CNetInterface.h"
#include "BuildFuntion.h"

CCUserBuilding::CCUserBuilding() 
{
	m_upgradeBuildDataMap.clear();
	m_buildingDataMap.clear();
	m_productingAtuoBuildingMap.clear();
	m_productingManualBuildingMap.clear();
	m_upgradedBuildingResultMap.clear();
	m_productedBuildingResultMap.clear();
	m_llSaveBuildingDataLastTime = time(0);
	m_llSaveProductingDataLastTime=time(0);
}

CCUserBuilding::~CCUserBuilding(void)
{
	m_upgradeBuildDataMap.clear();
	m_buildingDataMap.clear();
	m_productingAtuoBuildingMap.clear();
	m_productingManualBuildingMap.clear();
	m_upgradedBuildingResultMap.clear();
	m_productedBuildingResultMap.clear();
	m_llSaveBuildingDataLastTime = 0;
	m_llSaveProductingDataLastTime=0;
}

////更新城市建筑升级(由定时器催动的更新)
CT_VOID CCUserBuilding::UpdateUpgradingBuilding( CT_DWORD dwThreadID)
{
	_AUTOMUTEX(m_pMutex);  
	CT_INT64 timeNow = time(0);
	UBD_IT it = m_upgradeBuildDataMap.begin();
	UBD_IT itEnd = m_upgradeBuildDataMap.end();
	for (; it != itEnd; ++it)
	{
		if(m_pUser->GetInit() &&  timeNow -(*it).second.llBeginTime > (*it).second.iNeedTime)
		{
			UpgradedBuildingResult result = {0};
			result.dwBid = (*it).second.iID;
			result.iCiviNum = (*it).second.buildingUpgrade.iCiliviNum;
			result.iDiamod  = (*it).second.buildingUpgrade.iDiamond;
			result.iFood    = (*it).second.buildingUpgrade.iFood;
			result.iGold    = (*it).second.buildingUpgrade.iGold;
			result.iProp1   = (*it).second.buildingUpgrade.iProp1;
			result.iProp1Num = (*it).second.buildingUpgrade.iPropNum1;
			result.iProp2   = (*it).second.buildingUpgrade.iProp2;
			result.iProp2Num = (*it).second.buildingUpgrade.iPropNum2;
			result.iProp3   = (*it).second.buildingUpgrade.iProp3;
			result.iProp3Num = (*it).second.buildingUpgrade.iPropNum3;
			result.iProp4   = (*it).second.buildingUpgrade.iProp4;
			result.iProp4Num = (*it).second.buildingUpgrade.iPropNum4;
			result.iWorker   = (*it).second.buildingUpgrade.iWorker;
			//result.iSpeedTimes = (CT_INT32)(*it).second.bSpeedUpTime;
			result.iXp         = (*it).second.buildingUpgrade.iXp;
			++result.iUpgradeTimes;
			AddUpgradedBuildingResult(result);
		

			MSC_DB_R_BuildUpgradeFinish upgradeFinish = {0}; 
            upgradeFinish.dwUserID = m_pUser->GetUserID();
			upgradeFinish.iBID     = result.dwBid;
			upgradeFinish.iGrade   = (*it).second.buildingUpgrade.iGrade;
			G_SendThreadInfo(dwThreadID,  MSC_MAIN_DB_UPGRAE_FINISH
				          ,MSC_SUB_DB_R_BUILDING_UPGRADING_FINISH, (CT_CHAR*) &upgradeFinish, sizeof(upgradeFinish));
		}
	}
}

//得到建筑数据
CT_BOOL CCUserBuilding::GetBuildingData( CT_CHAR* pBuf, CT_INT32& bufLen)
{
	_AUTOMUTEX(m_pMutex);  
	CT_INT32 len = m_buildingDataMap.size() * sizeof(CMD_G_BuildingData);
	if (len > bufLen) //缓冲区不足
	{
		return CT_FALSE;
	}

	CMD_G_BuildingData ci;
	BD_IT it = m_buildingDataMap.begin();
    BD_IT itEnd = m_buildingDataMap.end();
	for (CT_INT32 i=0; it!= itEnd; ++it, ++i)
	{
		memset(&ci, 0, sizeof(ci));
		ci.iID = (*it).second.iID;
		ci.iGrade = (*it).second.iGrade;
		memcpy(pBuf + i*(sizeof(CMD_G_BuildingData)), &ci, sizeof(ci));
	}

	bufLen = len;
	return CT_TRUE;
}

//得到升级建筑数据
CT_BOOL CCUserBuilding::GetUpgardingBuildingData( CT_CHAR* pBuf, CT_INT32& bufLen)
{
	_AUTOMUTEX(m_pMutex);  
	CT_INT32 len = m_upgradeBuildDataMap.size() * sizeof(CMD_G_UpgradeBuildingData);
	if (len > bufLen) //缓冲区不足
	{
		return CT_FALSE;
	}

	CMD_G_UpgradeBuildingData cd;
	UBD_IT it = m_upgradeBuildDataMap.begin();
    UBD_IT itEnd = m_upgradeBuildDataMap.end();
	for (CT_INT32 i=0; it!=itEnd; ++it, ++i)
	{
		memset(&cd, 0, sizeof(cd));
		cd.iID = (*it).second.iID;
		CT_INT32 needLastingTime = (*it).second.iNeedTime - (CT_INT32)(time(0) - (*it).second.llBeginTime);
		cd.iNeedLastingTime = needLastingTime > 0 ? needLastingTime : 0;
		memcpy(pBuf + i*sizeof(cd), &cd, sizeof(cd));
	}
	bufLen = len;
	return CT_TRUE;
}


//得到升级建筑数据
CT_BOOL CCUserBuilding::GetUpgardingBuildingData(int bid, UpgradingBuildData& ubd)
{
	_AUTOMUTEX(m_pMutex);  
	UBD_IT it = m_upgradeBuildDataMap.find(bid);
	if (it != m_upgradeBuildDataMap.end())
	{
		memcpy(&ubd, &((*it).second), sizeof(UpgradingBuildData));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//更新城市建筑升级
CT_BOOL CCUserBuilding::UpdateUpgradingBuilding(int bid, UpgradingBuildData& ubd)
{
	_AUTOMUTEX(m_pMutex);  
	UBD_IT it = m_upgradeBuildDataMap.find(bid);
	if (it != m_upgradeBuildDataMap.end())
	{
		memcpy(&((*it).second), &ubd, sizeof(UpgradingBuildData));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获得建筑信息
CT_BOOL CCUserBuilding::GetBuildingData(const CT_INT32 iType,CMD_G_BuildingData &BuildData)
{
	_AUTOMUTEX(m_pMutex);
	G_Building Build={0};
	if(!G_GetBuildingDataEx(iType,&Build))
	{
		CCLogMeg::getSingleton().WriteError("CCUserBuilding::IsBuild G_GetBuildingDataEx");
		return CT_FALSE; 
	}

	BD_IT bdit=m_buildingDataMap.find(Build.iID);
	if(m_buildingDataMap.end() != bdit)
	{
		BuildData.iGrade=(*bdit).second.iGrade;
		BuildData.iID=(*bdit).second.iID;
		return CT_TRUE;
	}
	else
		return CT_FALSE;
}

//获得建筑信息
CT_BOOL CCUserBuilding::GetBuildingDataEx(const CT_INT32 iBID,CMD_G_BuildingData &BuildData)
{
	_AUTOMUTEX(m_pMutex);
	BD_IT bdit=m_buildingDataMap.find(iBID);
	if(m_buildingDataMap.end() != bdit)
	{
		BuildData.iGrade=(*bdit).second.iGrade;
		BuildData.iID=(*bdit).second.iID;
		return CT_TRUE;
	}
	else
		return CT_FALSE;
}

//检测建筑正在升级
CT_BOOL CCUserBuilding::FindUpgradeBuildData(CT_INT32 bid)
{
	_AUTOMUTEX(m_pMutex);  
	UBD_IT it = m_upgradeBuildDataMap.find(bid);
	if (it  != m_upgradeBuildDataMap.end() )
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//检测是否拥有这个建筑
CT_BOOL CCUserBuilding::FindBuildData(CT_INT32 bid, BuildingData& db)
{
	_AUTOMUTEX(m_pMutex);  
	BD_IT it = m_buildingDataMap.find(bid);
	if (it != m_buildingDataMap.end())
	{
		db = (*it).second;
		return CT_TRUE;
	}
	return CT_FALSE;
}

//检测是否拥有这个建筑
CT_BOOL CCUserBuilding::FindBuildData(CT_INT32 bid)
{
	_AUTOMUTEX(m_pMutex);  
	BD_IT it = m_buildingDataMap.find(bid);
	if (it != m_buildingDataMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//更新建筑数据
CT_VOID CCUserBuilding::UpateBuildData(const BuildingData& ud)
{
	_AUTOMUTEX(m_pMutex);  
	BD_IT it = m_buildingDataMap.find(ud.iID);
	if (it != m_buildingDataMap.end())
	{
		memcpy(&((*it).second), &ud, sizeof(ud));  
	}else
	{
		m_buildingDataMap.insert( BuildingDataMap::value_type(ud.iID, ud));
	}
}

//删除更新数据
CT_VOID CCUserBuilding::DeleteUpagradeBuildData(CT_DWORD bid)
{
	_AUTOMUTEX(m_pMutex);  
	UBD_IT it = m_upgradeBuildDataMap.find(bid);
	if (it != m_upgradeBuildDataMap.end())
	{
		m_upgradeBuildDataMap.erase(it);
	}
}



//插入建筑生产
CT_BOOL CCUserBuilding::InsertBuildingProductMap(const ProductingBuilding& bp)
{
	_AUTOMUTEX(m_pMutex);
	if (FindBuildingProduct(bp.iBid))
	{
		return CT_FALSE;
	}

	m_productingManualBuildingMap.insert(ProductingtManualBuildingMap::value_type(bp.iBid, bp));
	return CT_TRUE;
}

//清除生产中建筑
CT_VOID CCUserBuilding::DeleteBuildingProductMap(CT_INT32 bid)
{
	_AUTOMUTEX(m_pMutex);  
	ProductingtManualBuildingMap::iterator it = m_productingManualBuildingMap.find(bid);
	if (it != m_productingManualBuildingMap.end())
	{
		m_productingManualBuildingMap.erase(it);
	}
}

//查找建筑生产
CT_BOOL   CCUserBuilding::FindBuildingProduct(CT_INT32 bid)
{
	_AUTOMUTEX(m_pMutex);
	ProductingtManualBuildingMap::iterator it = m_productingManualBuildingMap.find(bid);
	if (it != m_productingManualBuildingMap.end())
	{
		return  CT_TRUE;// CheckAutoBuildingProducting(bid, (*it).second);
	}
	return CT_FALSE;
}

//找到正在生产建筑数据
CT_BOOL CCUserBuilding::FindBuildingProduct(CT_INT32 bid, ProductingBuilding& bp)
{
	_AUTOMUTEX(m_pMutex);  
	ProductingtManualBuildingMap::iterator it = m_productingManualBuildingMap.find(bid);
	if (it != m_productingManualBuildingMap.end())
	{
		//if (CheckAutoBuildingProducting(bid, (*it).second))
		//{
			memcpy(&bp, &(*it).second, sizeof(ProductingBuilding));
			return CT_TRUE;
		//}
	}
	return CT_FALSE;
}


//得到正在生产建筑数据
CT_BOOL CCUserBuilding::GetProductingData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);  
	CT_INT32 len = m_productingManualBuildingMap.size() * sizeof(CMD_G_ProductingBuildingData);
	if (len > bufLen) //缓冲区不足
	{
		return CT_FALSE;
	}

	CT_INT64 nowTime = time(0);
	CMD_G_ProductingBuildingData cd;
	ProductingtManualBuildingMap::iterator it    = m_productingManualBuildingMap.begin();
	ProductingtManualBuildingMap::iterator itEnd = m_productingManualBuildingMap.end();
	for (CT_INT32 i=0; it!=itEnd; ++it, ++i)
	{
		memset(&cd, 0, sizeof(cd));
		cd.iBID      = (*it).second.iBid;
		CT_INT32 needTime = (CT_INT32) ((*it).second.iLastingTime - (nowTime - (*it).second.llTime));
		if (needTime < 0)
		{
			needTime = 0;
		}
		cd.iNeedTime = needTime;
		cd.iProductType = (*it).second.iType;
		memcpy(pBuf + i*sizeof(cd), &cd, sizeof(cd));
	}
	bufLen = len;
	return CT_TRUE;
}

//得到正在生产建筑数据
CT_BOOL CCUserBuilding::GetProductingData(ProductingBuilding *pBp,CT_INT32 ilen)
{
	_AUTOMUTEX(m_pMutex);
	if(ilen <= sizeof(ProductingBuilding)*m_productingManualBuildingMap.size())
	{
		CCLogMeg::getSingleton().WriteWaring("CCUserBuilding::GetProductingData ilen <= sizeof(ProductingBuilding)*m_productingManualBuildingMap.size()");
		return CT_FALSE;
	}

	CT_INT32 iSize=0;
	ProductingtManualBuildingMap::iterator it    = m_productingManualBuildingMap.begin();
	ProductingtManualBuildingMap::iterator itEnd = m_productingManualBuildingMap.end();
	for (iSize=0;it!=itEnd; ++it,++iSize)
	{
		memcpy(&pBp[iSize], &((*it).second), sizeof(ProductingBuilding));
		iSize++;
	}
	ilen=iSize*sizeof(ProductingBuilding);
	return CT_TRUE;
}

//得到正在生产建筑数据
CT_VOID CCUserBuilding::UpdateBuildingProduct(CT_INT32 bid, ProductingBuilding& bp)
{
	_AUTOMUTEX(m_pMutex);
	ProductingtManualBuildingMap::iterator it = m_productingManualBuildingMap.find(bid);
	if (it != m_productingManualBuildingMap.end())
	{
		memcpy(&((*it).second), &bp, sizeof(ProductingBuilding));
	}else
	{
		m_productingManualBuildingMap.insert(ProductingtManualBuildingMap::value_type(bid, bp));
	}
}


//计算自动生产的建筑事件
CT_VOID CCUserBuilding::SysProductingBuilding()
{
	_AUTOMUTEX(m_pMutex);  
	ProductingtManualBuildingMap::iterator it	   = m_productingManualBuildingMap.begin();
	ProductingtManualBuildingMap::iterator itEnd =  m_productingManualBuildingMap.end();
	for (; it != itEnd; ++it)
	{
		(*it).second.bSendCanHavest = false; //重置发送状态，以便通知用户
	}

	ProductingtAutoBuildingMap::iterator ita     = m_productingAtuoBuildingMap.begin();
	ProductingtAutoBuildingMap::iterator itaEnd  = m_productingAtuoBuildingMap.end();
	for (; ita != itaEnd; ++ita)
	{
		(*ita).second.bSendCanHavest = false;  //重置发送状态，以便通知用户
	}

	//同步自动生产防止数据不同步
	BuildingDataMap::iterator itt = m_buildingDataMap.begin();
	BuildingDataMap::iterator ittEnd = m_buildingDataMap.end();
	for (; itt != ittEnd; ++itt)
	{
		G_BuildingType  gBT = {0};
		if(!CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID((*itt).second.iID, &gBT ))
		{
			CCLogMeg::getSingleton().WriteError("CCUserBuilding::SysProductingBuilding() : fail to find build type, bid = %i", (*itt).second.iID);
			continue;
		}
		if (gBT.iID == CITY_TYPE_RESIDENTIAL)
		{
			ProductingtAutoBuildingMap::iterator it =  m_productingAtuoBuildingMap.find((*itt).second.iID );
			if (it == m_productingAtuoBuildingMap.end())
			{
				G_BuildingProduct gBP = {0};
				if(CCBuildingScriptSetting::getSingleton().GetProductType(gBT.iID, (*itt).second.iGrade, &gBP))
				{
					G_BuildingProductInfo gbpi = {0};
					if (CCBuildingScriptSetting::getSingleton().GetProductInfo(gBP.iProductType,  &gbpi))
					{
						ProductingBuilding pb = {0};
						pb.bSendCanHavest = false;
						pb.iBGrade        = (*itt).second.iGrade;
						pb.iBid           = (*itt).second.iID;
						pb.llTime         = time(0);
						pb.iLastingTime   = gbpi.iTime;
						pb.iWorker        = 0;
						pb.iType          = gBP.iProductType;
						pb.iBType         = gBT.iID;
						m_productingAtuoBuildingMap.insert(ProductingtAutoBuildingMap::value_type(pb.iBid, pb));
					}
				}
			}
		}
	}
	
}



//更新生产的建筑
CT_VOID CCUserBuilding::UpdateProductingBuilding()
{
	_AUTOMUTEX(m_pMutex);  
	CT_INT64 timeNow = time(0);
	ProductingtManualBuildingMap::iterator it	   = m_productingManualBuildingMap.begin();
	ProductingtManualBuildingMap::iterator itEnd =  m_productingManualBuildingMap.end();
	//ProductingtManualBuildingMap::iterator cpIT = it;
	for (; it != itEnd; ++it)
	{
		CheckHarvest(timeNow, (*it).second);
	}

	ProductingtAutoBuildingMap::iterator itt = m_productingAtuoBuildingMap.begin();
	ProductingtAutoBuildingMap::iterator ittEnd = m_productingAtuoBuildingMap.end();
	for (; itt != ittEnd; ++itt)
	{
		CheckHarvest(timeNow, (*itt).second);
	}

}

//检测建筑是否可以收获
CT_VOID CCUserBuilding::CheckHarvest(CT_INT64 timeNow,  ProductingBuilding& pb)
{
	_AUTOMUTEX(m_pMutex);
	if (m_pUser->m_userEvent.FindBuildingEventByID(pb.iBid) || m_pUser->m_userBuilding.FindUpgradeBuildData(pb.iBid))  //建筑在事件中或者正在升级
	{
		pb.llTime = timeNow;
		return ;
	}
	CT_INT64   beginTime    =  pb.llTime;
	CT_INT64   lastingTime  =  pb.iLastingTime;
	CT_BOOL    bSendHarvest =  pb.bSendCanHavest;
	if(timeNow - beginTime > lastingTime && !bSendHarvest  && !(m_pUser->m_userEvent.FindBuildingEventByID(pb.iBid )/*防止任务引发的事件*/))
	{
		pb.bSendCanHavest = CT_TRUE;
		//到达收获条件，释放工人
		m_pUser->CountFreeWorker();
		CMD_G_ProductHarvest ch = {0};
		ch.dwUserID = m_pUser->m_dwID;
		ch.iBuildID = pb.iBid;
		ch.iWorker = m_pUser->m_userBase.m_masterData.mvd.stRes.iWorker;
		m_pUser->m_pNetInterface->Send(MAIN_G_BUILDING_EVENT_CITY, SUB_G_BUILDING_PRODUCT_CAN_HARVEST,(CT_CHAR*)&ch, sizeof(ch));
	}
}


//查找自动生产建筑
CT_BOOL CCUserBuilding::FindAutoBuilding(CT_INT32 bid)
{
	_AUTOMUTEX(m_pMutex);
	ProductingtAutoBuildingMap::iterator it = m_productingAtuoBuildingMap.find(bid);
	if (it != m_productingAtuoBuildingMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//查找自动生产建筑
CT_BOOL CCUserBuilding::FindAutoBuilding(CT_INT32 bid, ProductingBuilding& pb)
{
	_AUTOMUTEX(m_pMutex);
	ProductingtAutoBuildingMap::iterator it = m_productingAtuoBuildingMap.find(bid);
	if (it != m_productingAtuoBuildingMap.end())
	{
		memcpy(&pb, &((*it).second), sizeof(ProductingBuilding));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//查找已经生产的自动建筑
CT_BOOL CCUserBuilding::FindAutoProductingBuilding(CT_INT32 bid)
{
	_AUTOMUTEX(m_pMutex);
	ProductingtAutoBuildingMap::iterator it = m_productingAtuoBuildingMap.find(bid);
	if (it != m_productingAtuoBuildingMap.end())
	{
		if ((*it).second.bSendCanHavest)
		{
			return CT_TRUE;
		}	
	}
	return CT_FALSE;
}

//更新生产建筑
CT_VOID CCUserBuilding::UpdateAutoBuilding(CT_INT32 bid, ProductingBuilding& pb)
{
	ProductingtAutoBuildingMap::iterator it = m_productingAtuoBuildingMap.find(bid);
	if (it != m_productingAtuoBuildingMap.end())
	{
		memcpy(&(*it).second, &pb, sizeof(ProductingBuilding));
	}else
	{
		m_productingAtuoBuildingMap.insert(ProductingtAutoBuildingMap::value_type(bid, pb));
	}
}

//插入自动生产建筑
CT_VOID CCUserBuilding::InsertAutoBuilding(CT_INT32 bid, ProductingBuilding& pb)
{
	_AUTOMUTEX(m_pMutex);
	m_productingAtuoBuildingMap.insert(ProductingtAutoBuildingMap::value_type(bid, pb));
}

CT_INT32 CCUserBuilding::GetUserBuildByType(CT_INT32 iBuildType,CT_INT32 iLevel)
{
	_AUTOMUTEX(m_pMutex);
	BD_IT it = m_buildingDataMap.begin();
	CT_INT32 iOwnNum=0;
	while(it!=m_buildingDataMap.end())
	{
		/*G_Building build = {0};
		if (CCBuildingScriptSetting::getSingleton().GetBuildingData(it->first,&build))
		{*/
			if (it->first==iBuildType)
			{
				//如果级别大于0,是城市建筑升级任务（目前策划文档是要求一个）
				if (iLevel>0)
				{
					if (it->second.iGrade>=iLevel)
					{
						iOwnNum++;
						return iOwnNum;
					}
				}
				else
				{
					iOwnNum++;
				}
			}
		
		it++;
	}
	return iOwnNum;
}

//建筑存在
CT_BOOL CCUserBuilding::BuildValidate(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	G_Building Build={0};
	if(!G_GetBuildingDataEx(iType,&Build))
	{
		CCLogMeg::getSingleton().WriteError("CCUserBuilding::IsBuild G_GetBuildingDataEx");
		return CT_FALSE; 
	}

	BD_IT bdit=m_buildingDataMap.find(Build.iID);
	if(m_buildingDataMap.end() != bdit)
	{
		if((*bdit).second.iGrade >= 1)
			return CT_TRUE;
		else
			return CT_FALSE;
	}
	else
		return CT_FALSE;
}

CT_VOID CCUserBuilding::DisposeHarvestAutoOnce()
{
	_AUTOMUTEX(m_pMutex);
	CT_DWORD threadid = GAME_THREAD_CITY + m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	ProductingtManualBuildingMap::iterator it = m_productingManualBuildingMap.begin();
	std::vector<int>    buildList;   //记录收获的手动生产建筑，循环完成后删除
	while(it!=m_productingManualBuildingMap.end())
	{
		if (!it->second.bSendCanHavest)
		{
			continue;
			it++;
		}
		G_BuildingType bt;
		memset(&bt, 0, sizeof(bt));
		if(!CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(it->second.iBid, &bt))
		{
			CCLogMeg::getSingleton().WriteError("CCUserBuilding::DisposeHarvestAutoOnce : fail to find buildtype : %d", it->second.iBid);
			return ;
		}

		G_Building Build={0};
		if(!G_GetBuildingData(it->second.iBid, &Build))
		{
			CCLogMeg::getSingleton().WriteError("CCUserBuilding::DisposeHarvestAutoOnce : fail to find buildtype : %d", it->second.iBid);
			return ;
		}
		
		
		//洞穴收获
		if(Build.iBT == CITY_TYPE_PATCAVE)
		{
			G_PetCave gbp = {0};
			MasterVariableData mvd={0};
			m_pUser->m_userBase.GetMasterData(mvd);
			if (!CCBuildingScriptSetting::getSingleton().GetPetCave(it->second.iType,mvd.stGrade.iLevel, &gbp ) )
			{
				CCLogMeg::getSingleton().WriteError("CCUserBuilding::DisposeHarvestAutoOnce : fail to find producttype in GetPetCave: %d", it->second.iType );
				return;
			}

			//发放奖励
			G_CityFinishCave(m_pUser, threadid, bt,gbp, it->second);
		}
		else
		{
			G_BuildingProductInfo bpi={0};
			if (!CCBuildingScriptSetting::getSingleton().GetProductInfo(it->second.iType , &bpi))
			{
				return ;
			}

			//发放奖励
			G_CityFinishProduct(m_pUser, threadid, bpi, bt, it->second);
		}

		buildList.push_back(it->second.iBid);
		it++;
	}
	for (CT_DWORD i=0;i<buildList.size();i++)
	{
		DeleteBuildingProductMap(buildList[i]);
	}
	ProductingtAutoBuildingMap::iterator it2 = m_productingAtuoBuildingMap.begin();
	while (it2!=m_productingAtuoBuildingMap.end())
	{
		if (!it2->second.bSendCanHavest)
		{
			continue;
			it2++;
		}
		G_BuildingType bt;
		memset(&bt, 0, sizeof(bt));
		if(!CCBuildingScriptSetting::getSingleton().GetBuildTypeWithID(it2->second.iBid, &bt))
		{
			CCLogMeg::getSingleton().WriteError("CCUserBuilding::DisposeHarvestAutoOnce2 : fail to find buildtype : %d", it2->second.iBid);
			return ;
		}

		G_Building Build={0};
		if(!G_GetBuildingData(it2->second.iBid, &Build))
		{
			CCLogMeg::getSingleton().WriteError("CCUserBuilding::DisposeHarvestAutoOnce2 : fail to find buildtype : %d", it2->second.iBid);
			return ;
		}


		G_BuildingProductInfo bpi={0};
		if (!CCBuildingScriptSetting::getSingleton().GetProductInfo(it2->second.iType , &bpi))
		{
			return ;
		}

		//发放奖励
		G_CityFinishProduct(m_pUser, threadid, bpi, bt, it2->second);
		it2->second.bSendCanHavest = CT_FALSE;
		it2->second.llTime         = time(0);
		m_pUser->m_userBuilding.UpdateAutoBuilding(it2->second.iBid, it2->second);
		it2++;
	}
	
}

//增加升级消耗
CT_VOID CCUserBuilding::AddUpgradedBuildingResult(const UpgradedBuildingResult& result)
{
	_AUTOMUTEX(m_pMutex);
	UpgradedBuildingResultMap::iterator itt = m_upgradedBuildingResultMap.find(result.dwBid);
	if (itt != m_upgradedBuildingResultMap.end())
	{
		(*itt).second.iCiviNum += result.iCiviNum;
		(*itt).second.iDiamod  += result.iDiamod;
		(*itt).second.iFood    += result.iFood;
		(*itt).second.iGold    += result.iGold;
		(*itt).second.iProp1   += result.iProp1;
		(*itt).second.iProp2   += result.iProp2;
		(*itt).second.iProp3   += result.iProp3;
		(*itt).second.iProp4   += result.iProp4;
		(*itt).second.iWorker  += result.iWorker;
		//(*itt).second.iSpeedTimes += result.iSpeedTimes;
		(*itt).second.iXp      += result.iXp;
	}else
	{
		m_upgradedBuildingResultMap.insert(UpgradedBuildingResultMap::value_type(result.dwBid, result ) );
	}

}

//同步建筑数据到数据库
CT_VOID CCUserBuilding::UpdateSaveBuildingData(CT_BOOL bForceBuildData, CT_BOOL bForceUpgradeBuildData,  CT_BOOL bForceResult)
{
	_AUTOMUTEX(m_pMutex);
	G_ValueRange vr = {0};
	G_GetValueRange(VALUE_TYPE_BUILDING_SYS_DATABASE_MAX_TIME,  &vr);
	if (vr.fMaxVal < 1)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBuilding::UpdateSaveBuildingData: 错误的VALUE_TYPE_BUILDING_SYS_DATABASE_MAX_TIME=%f", vr.fMaxVal);
	}
	
	   CT_BOOL bTimeout = time(0) - m_llSaveBuildingDataLastTime >= vr.fMaxVal;
	
		m_llSaveBuildingDataLastTime = time(0);
		CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		CT_DWORD userID = m_pUser->GetUserID();
				
		if (bTimeout ||  bForceBuildData)
		{
			{
				MSC_DB_BuildingInfo bi  = {0};
				BD_IT it = m_buildingDataMap.begin();
				BD_IT itEnd = m_buildingDataMap.end();
				for (; it!= itEnd; ++it)
				{
					if ((*it).second.bChange)
					{
						memset(&bi, 0, sizeof(bi));
						bi.dwUserID = userID;
						bi.iBid     = (*it).second.iID;
						bi.iGrade   = (*it).second.iGrade;
						G_SendThreadInfo(threadid,  MSC_MAIN_DB_SAVE_BUILDING, MSC_SUB_DB_SAVE_BUILDING, (CT_CHAR*) &bi, sizeof(bi));
						(*it).second.bChange = CT_FALSE;
					}
				}
			}
		}

		if (bTimeout || bForceUpgradeBuildData)
		{
			MSC_DB_SysBuildingUpgrading sysBuildingUpgrading = {0};
			sysBuildingUpgrading.dwUserID = userID;
			G_SendThreadInfo(threadid,  MSC_MAIN_DB_SAVE_BUILDING
				,MSC_SUB_DB_SYS_BUILDING_UPGRADINT, (CT_CHAR*) &sysBuildingUpgrading, sizeof(sysBuildingUpgrading));
			
			{
				MSC_DB_BuildingUpgradingInfo bui = {0};
				UBD_IT it = m_upgradeBuildDataMap.begin();
				UBD_IT itEnd = m_upgradeBuildDataMap.end();
				for (; it!= itEnd; ++it)
				{
					memset(&bui, 0, sizeof(bui));
					bui.buildingUpgrade = (*it).second.buildingUpgrade;
					bui.dwUserID        = userID;
					bui.iID             = (*it).second.iID;
					bui.iNeedTime       = (*it).second.iNeedTime;
					bui.llBeginTime     = (*it).second.llBeginTime;

					G_SendThreadInfo(threadid,  MSC_MAIN_DB_SAVE_BUILDING, MSC_SUB_DB_SAVE_UPGRADING_BUILDING, (CT_CHAR*) &bui, sizeof(bui));
				}
			}
		}

		if (bTimeout || bForceResult)
		{
			{
				UpgradedBuildingResultMap::iterator it = m_upgradedBuildingResultMap.begin();
				UpgradedBuildingResultMap::iterator itEnd = m_upgradedBuildingResultMap.end();
				for (; it!=itEnd; ++it)
				{
					MSC_LOGDB_AddUpgradeRecord addur = {0};
					addur.dwUserID = userID;
					addur.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
					addur.iBid =   (*it).second.dwBid;
					addur.iWorker = (*it).second.iWorker;
					addur.iCivil =  (*it).second.iCiviNum;
					addur.iDiamod = (*it).second.iDiamod;
					addur.iFood =   (*it).second.iFood;
					addur.iGold =   (*it).second.iGold;
					addur.iProp1 =  (*it).second.iProp1;
					addur.iProp1Num = (*it).second.iProp1Num;
					addur.iProp2 =    (*it).second.iProp2;
					addur.iProp2Num = (*it).second.iProp2Num;
					addur.iProp3    = (*it).second.iProp3;
					addur.iProp3Num = (*it).second.iProp3Num;
					addur.iProp4 =    (*it).second.iProp4;
					addur.iProp4Num = (*it).second.iProp4Num;
					addur.iXp       = (*it).second.iXp;
					addur.iUpgradeTimes=(*it).second.iUpgradeTimes;

					CT_DWORD threadid = GAME_THREAD_LOGDATABASE + userID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
					G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_UPGRADEFINISH,(CT_CHAR*)&addur,sizeof(addur));
				}
				m_upgradedBuildingResultMap.clear();
			}
		}
}

//增加升级消耗
CT_VOID CCUserBuilding::AddProductedBuildingResult(const ProductedBuildingResultKey& key, const ProductedBuildingResult& result)
{
	_AUTOMUTEX(m_pMutex);
	ProductedBuildingResultMap::iterator itt = m_productedBuildingResultMap.find(key);
	if (itt != m_productedBuildingResultMap.end())
	{
		(*itt).second.iCivil   += result.iCivil;
		(*itt).second.iDiamod  += result.iDiamod;
		(*itt).second.iFood    += result.iFood;
		(*itt).second.iGold    += result.iGold;
		(*itt).second.iProductingTimes += result.iProductingTimes;
		(*itt).second.iProp1Num        += result.iProp1Num;
		(*itt).second.iProp2Num        += result.iProp2Num;
		(*itt).second.iStamina         += result.iStamina;
		(*itt).second.iWorker          += result.iWorker;
	}else
	{
		m_productedBuildingResultMap.insert(ProductedBuildingResultMap::value_type(key, result ) );
	}
}


//同步建筑生产数据到数据库
CT_VOID CCUserBuilding::UpdateSaveProductingData(CT_BOOL bForceBuildProducting  ,CT_BOOL bForceResult )
{
	_AUTOMUTEX(m_pMutex);
	G_ValueRange vr = {0};
	G_GetValueRange(VALUE_TYPE_PRODUCTING_SYS_DATABASE_MAX_TIME,  &vr);
	if (vr.fMaxVal < 1)
	{
		CCLogMeg::getSingleton().WriteError("CCUserBuilding::UpdateSaveBuildingData: 错误的VALUE_TYPE_BUILDING_SYS_DATABASE_MAX_TIME=%f", vr.fMaxVal);
	}

	CT_BOOL bTimeout = time(0) - m_llSaveProductingDataLastTime >= vr.fMaxVal;

		m_llSaveProductingDataLastTime = time(0);
		CT_DWORD dwUserID = m_pUser->GetUserID();
			
		if (bTimeout || bForceBuildProducting)
		{
			CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			MSC_DB_SysProductingData  sysData = {0};
			sysData.dwUserID = dwUserID;
			G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_SYS_PROPDUCTING , (CT_CHAR*)&sysData, sizeof(sysData));

			{
				ProductingtAutoBuildingMap::iterator it = m_productingAtuoBuildingMap.begin();
				ProductingtAutoBuildingMap::iterator itend=m_productingAtuoBuildingMap.end();

				for (; it != itend; ++it)
				{
					MSC_DB_SaveProductingData spd = {0};
					spd.dwUserID = dwUserID;
					spd.productingBuilding = (*it).second;
					G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_PRODUCTING_DATA , (CT_CHAR*)&spd, sizeof(spd));
				}
			}	

			{
				ProductingtManualBuildingMap::iterator it = m_productingManualBuildingMap.begin();
				ProductingtManualBuildingMap::iterator itend=m_productingManualBuildingMap.end();

				for (; it != itend; ++it)
				{
					MSC_DB_SaveProductingData spd = {0};
					spd.dwUserID = dwUserID;
					spd.productingBuilding = (*it).second;
					G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_PRODUCTING_DATA , (CT_CHAR*)&spd, sizeof(spd));
				}
			}	

		}

		if (bTimeout || bForceResult)
		{
			//数据记录
			ProductedBuildingResultMap::iterator it = m_productedBuildingResultMap.begin();
			ProductedBuildingResultMap::iterator itend = m_productedBuildingResultMap.end();

			for (; it!= itend; ++it)
			{
				MSC_LOGDB_AddProductingRecord AddProducting={0};
				AddProducting.dwUserID = dwUserID;
				AddProducting.iUserGrade = m_pUser->m_userBase.m_masterData.mvd.stGrade.iLevel;
				AddProducting.iBid =  (*it).first.iBid;
				AddProducting.iCivil = (*it).second.iCivil;
				AddProducting.iFood =  (*it).second.iFood;
				AddProducting.iGold =  (*it).second.iGold;
				AddProducting.iProp1 = (*it).first.iProp1;
				AddProducting.iProp1Num = (*it).second.iProp1Num;
				AddProducting.iStamina = (*it).second.iStamina;
				AddProducting.iWorker =  (*it).second.iWorker;
				AddProducting.iDiamod =  (*it).second.iDiamod;
				AddProducting.iProductingTimes = (*it).second.iProductingTimes;
				//AddProducting.iSpeedupTimes    = (*it).second.iSpeedupTimes;

				//	memcpy(AddProducting.cName,bt.strName,sizeof(AddProducting.cName));
				CT_DWORD threadid = GAME_THREAD_LOGDATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PRODUCTFINISH,(CT_CHAR*)&AddProducting,sizeof(AddProducting));
			}
			m_productedBuildingResultMap.clear();
		}
	
}

//得到正在升级的建筑
CT_BOOL CCUserBuilding::GetUpgradingBuildingData(UpgradingBuildData* pBd, CT_INT32 iLen)
{
	_AUTOMUTEX(m_pMutex);  
	CT_INT32 len = m_upgradeBuildDataMap.size() * sizeof(UpgradingBuildData);
	if (len > iLen) //缓冲区不足
	{
		return CT_FALSE;
	}

	UBD_IT it = m_upgradeBuildDataMap.begin();
	UBD_IT itEnd = m_upgradeBuildDataMap.end();
	for (CT_INT32 i=0; it!=itEnd; ++it, ++i)
	{
		pBd[i] = it->second;
	}
	iLen = len;
	return CT_TRUE;
}