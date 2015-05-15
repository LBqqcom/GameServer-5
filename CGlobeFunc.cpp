#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CMesBlock.h"
#include "CThreadManager.h"
#include "CDBConnector.h"
#include "CTimerThread.h"
#include "CGlobeData.h"
#include "CDatabaseMes.h"
#include "CUser.h"
#include "CGameConfig.h"
#include "CNetInterface.h"
#include "CFightTeamManager.h"
#include "CBroadcastScriptSetting.h"
#include "CUserSeek.h"

ReadCountMap g_ReadCountMap;

//消息处理错误记录
CT_VOID G_DisposeErrorMes(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pErrorScr)
{
	CCLogMeg::getSingleton().WriteError("G_DisposeErrorMes : mainID=%i, subID=%i : %s , %s , %i", mainID, subID, pErrorScr, __FILE__, __LINE__);
}

//发送线程消息
CT_VOID G_SendThreadInfo(CT_DWORD threadID,  CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len)
{
	CCGameMesEvent::SendThreadInfo(threadID, mainID, subID, pBuf, len);
#ifdef  READCOUNT
	if(threadID >= GAME_THREAD_DATABASE)
	{
		CT_INT64 lID=G_Combine64(mainID,subID);
		rc_it it=g_ReadCountMap.find(lID);
		if(it != g_ReadCountMap.end())
		{
			(*it).second.iCount++;
		}
		else
		{
			ReadCount rc={0};
			rc.lId=lID;
			rc.iMainID=mainID;
			rc.iSubID=subID;
			g_ReadCountMap.insert(ReadCountMap::value_type(rc.lId,rc));
		}
	}
#endif
}


//处理db错误
CT_VOID G_DBError(CCDBConnector& dbConnector, CT_DWORD dwThread)
{
	if(dbConnector.GetErrno() == DB_NET_ERROR)
	{
		CCLogMeg::getSingleton().WriteError("G_DBError(): DB happen fatal error threadID=%i", dwThread);
		CCThread* pThread = CCThreadManager::getSingleton().GetTaskThread(dwThread);
		CT_ASSERT(pThread != 0);

		G_ValueRange vr = {0};
		if(G_GetValueRange(VALUE_TYPE_DATABASERECONNECTTIME,  &vr))
		{
			CCTimerThread::getSingleton().SetTimer(IDI_DB_CONNECT_TIMER, pThread, (CT_INT32)vr.fMaxVal * 1000);
			
		/*	CCDataBaseThread* pDBThread = dynamic_cast<CCDataBaseThread*>(pThread);
			if (pDBThread != 0)
			{
				pDBThread->SetConnectorDB(CT_FALSE);
			}*/

		}
	}else
	{
		//一般用户级错误
		CCLogMeg::getSingleton().WriteError("G_DBError(): please check DB PROCEDURE threadID=%i", dwThread);
	}
}


//发送db错误消息
CT_VOID G_SendThreadDbStandardInfo  (CT_DWORD threadID, CT_DWORD mainID, CT_DWORD subID,  CT_DWORD dwUser, const CCNetInterface* pNetInter)
{
	MSC_DB_R_BaseInfo mde;
	memset(&mde, 0, sizeof(mde));
	mde.dwUserID = dwUser;
	mde.pNetInterface = (CCNetInterface*)pNetInter;
	G_SendThreadInfo(threadID, mainID, subID, (CT_CHAR*)&mde, sizeof(mde));
}

//发送建筑信息
CT_VOID G_SendThreadDBBuildingInfo  (CT_DWORD threadID, CT_DWORD mainID, CT_DWORD subID,
									 CT_DWORD dwUser, CT_INT32 buildID, const CCNetInterface* pNetInter)
{
	MSC_DB_R_BuildingBaseInfo mde;
	memset(&mde, 0, sizeof(mde));
	mde.dwUserID = dwUser;
	mde.iBuildID = buildID;
	mde.pNetInterface = (CCNetInterface*)pNetInter;
	G_SendThreadInfo(threadID, mainID, subID, (CT_CHAR*)&mde, sizeof(mde));
}

//保存领主货币资源数据
CT_VOID G_SaveMastVariableData(CCUser* pUser, ENUM_BaseDataSaveTag saveTag)
{
#ifdef _DEBUG
	MSC_DB_MasterSaveData msd = {0};
	pUser->m_userBase. GetMasterSaveData(msd);
	msd.iSaveTag   = saveTag;

	CT_INT32 threadID =  GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_DATA , (CT_CHAR*)&msd, sizeof(msd));

	CT_INT32 logThreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));
#else
	if(SAVETAG_EVENT_EXIT == saveTag)
	{
		MSC_DB_MasterSaveData msd = {0};
		pUser->m_userBase. GetMasterSaveData(msd);
		msd.iSaveTag   = saveTag;

		CT_INT32 threadID =  GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_MASTER_DATA , (CT_CHAR*)&msd, sizeof(msd));

		CT_INT32 logThreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));
	}
#endif
}

//离线用户数据保存
CT_VOID G_SaveMastvariableData(MSC_DB_MasterSaveData &msd)
{
	CT_INT32 threadID =  GAME_THREAD_DATABASE + msd.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));

	CT_INT32 logThreadid = GAME_THREAD_LOGDATABASE + msd.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));
	return;
}

//获取军队
CT_INT32 G_GetArmyID(CT_INT32 iArmyID,CT_INT32 iGrade)
{
	return (iArmyID<<16)|iGrade;
}

//获取体力购买消费
CT_INT32 G_GetStatminaCost(CCUser *pUser)
{
	G_ValueRange StatminaValue={0};
	if(!G_GetValueRange(VALUE_TYPE_STATMINA_COST,&StatminaValue))
	{
		CCLogMeg::getSingleton().WriteError("G_GetStatminaCost:VALUE_TYPE_STATMINA_COST%s%d",__FILE__,__LINE__);
		return 0;
	}

	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);

	return (CT_INT32)(MasterInfo.mvd.stRes.iStaminaBuyTimes*StatminaValue.fMinVal+StatminaValue.fMinVal);
}

//体力消费记录
CT_BOOL G_SaveStatminaCost(CCUser *pUser)
{
	CT_INT32 iDiamod=G_GetStatminaCost(pUser);

	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);

	//增加体力购买次数上限
	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_STATMINA_TIME,&vr))
	{
		CCLogMeg::getSingleton().WriteError("G_SaveStatminaCost: G_GetValueRange VALUE_TYPE_STATMINA_TIME%s%d",__FILE__,__LINE__);
		return CT_FALSE;
	}

	if (MasterInfo.mvd.stRes.iStaminaBuyTimes >= vr.fMinVal + pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_STATMINA))
	{
		CCLogMeg::getSingleton().WriteError("G_SaveStatminaCost:VALUE_TYPE_STATMINA_TIME%s%d",__FILE__,__LINE__);
		return CT_FALSE;
	}

	//CT_INT32 threadID =  GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
//	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_BUY_STATMINA, (CT_CHAR*)&BuyStatmina, sizeof(BuyStatmina));


	G_ValueRange StatminaValue={0};
	if(!G_GetValueRange(VALUE_TYPE_STATMINA,&StatminaValue))
	{
		CCLogMeg::getSingleton().WriteError("G_GetStatminaCost:VALUE_TYPE_STATMINA_COST%s%d",__FILE__,__LINE__);
		return CT_FALSE;
	}

	MasterVariableData mvd={0};
	mvd.stRes.iStamina+=(CT_INT32)StatminaValue.fMinVal;
	mvd.stRes.iDiamod+=iDiamod;
	mvd.stRes.iStaminaBuyTimes++;
	pUser->m_userBase.AddMastData(mvd,SAVETAG_STAMINA_COST);

	MSC_LOGDB_BuyStatmina BuyStatmina={0};

	BuyStatmina.dwUserID=pUser->GetUserID();
	BuyStatmina.iUserGrade = MasterInfo.mvd.stGrade.iLevel;
	BuyStatmina.iDiamod=iDiamod;
	BuyStatmina.iStatNum=(CT_INT32)StatminaValue.fMaxVal;

	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_STATMINA,(CT_CHAR*)&BuyStatmina,sizeof(BuyStatmina));

	return CT_TRUE;
}

// 字符时间格式转换为秒数
CT_INT64 G_StrTimeToSeconds(CT_CHAR *date)
{
    struct tm LoginTime = {0};
    LoginTime.tm_year = atoi(date) - 1900;
    LoginTime.tm_mon = atoi(date + 5) - 1;
    LoginTime.tm_mday = atoi(date + 8);
    LoginTime.tm_hour = atoi(date + 11);
    LoginTime.tm_min = atoi(date + 14);
    LoginTime.tm_sec = atoi(date + 17);
    return mktime(&LoginTime); 
}

//time转时间格式
CT_VOID G_SecondsToTime(CT_CHAR *strTime,CT_INT64 lTime)
{
	tm tmTime={0};
	tmTime=*localtime((time_t*)&lTime);
	memcpy(strTime,&tmTime,sizeof(tmTime));
	return;
}

//
CT_VOID G_ReadCount()
{
	rc_it itEnd=g_ReadCountMap.end();
	for(rc_it i=g_ReadCountMap.begin();i != itEnd;++i)
	{
		CCLogMeg::getSingleton().WriteError("mainID:%d\t SubID:%d\t Count:%d",(*i).second.iMainID,(*i).second.iSubID,(*i).second.iCount);
	}
}



//副本关卡KEY
CT_INT32 G_GetKeyValue(int value0, int value1, int value2)
{
	return (value0 * 10000 + value1 * 100 + value2);
}

//为概率计算
CT_INT32 G_GetFakeProCount(CT_INT32 iID,FakeProBase &fb)
{
	G_FakeProBase bese={0};
	G_FakeProLimit FakeProLimit={0};

	if(!G_GetFakeProBase(iID,&bese))
	{
		CCLogMeg::getSingleton().WriteError("G_GetFakeProCount iID:%d",iID);
		return 0;
	}

	CT_BOOL bFinish=CT_TRUE;
	for(CT_INT32 i=0;i<FAKE_PRO_LIMIT_COUNT;++i)
	{
		if(bese.iLimits[i] == 0 || !G_GetFakeProLimit(bese.iLimits[i],&FakeProLimit))continue;
		if(FakeProLimit.iLimitType == fb.iSrcType && 0 != fb.iDataCount)
		{
			if(FakeProLimit.iDataType == FP_MASTER_GRADE && FakeProLimit.iDataCount > fb.iMasterGrade ||
				FakeProLimit.iDataType == FP_FINISH_TIME && FakeProLimit.iDataCount > fb.iDataCount ||
				FakeProLimit.iDataType == FP_CHARGE_DIAMOD && FakeProLimit.iDataCount > fb.iChargeDiamod ||
				FakeProLimit.iDataType == FP_COST_DIAMOD && FakeProLimit.iDataCount > fb.iCostDiamod)
			{
				bFinish=CT_FALSE;
				break;
			}
		}
		else
		{
			if(FakeProLimit.iLimitType != fb.iSrcType)
				CCLogMeg::getSingleton().WriteError("G_GetFakeProCount FakeProLimit.iLimitType %d != fb.iSrcType %d",FakeProLimit.iLimitType,fb.iSrcType);
			bFinish=CT_FALSE;
			break;
		}
	}

	if(bFinish)
	{
		//fb.bFakePro=CT_TRUE;

		if(bese.iNextProID != 0)
		{
			CT_INT32 iWeight=G_GetFakeProCount(bese.iNextProID,fb);
			return (iWeight != 0?iWeight:bese.fWeight);
		}
		else
			return (CT_INT32)bese.fWeight;
	}
	else
	{
		return 0;
	}
}

//道具组抽取
CT_INT32 G_GetGoodsGroupInfo(const CT_INT32 iGroupID,CT_INT32 iGoods[GOODS_COUNT],CT_INT32 iCount[GOODS_COUNT],FakeProBase &fb)
{
	//道具奖励
	G_GoodsCount GoodsCount={0};
	G_GoodsGroup GoodsGroup={0};
	G_GoodsWeights GoodsWeights={0};
	G_GoodsCharge GoodsCharge={0};
	G_GoodsFakePros GoodsFakePros={0};
	if(!G_GetGoodsCount(iGroupID,&GoodsCount))
	{
		CCLogMeg::getSingleton().WriteError("G_GetGoodsGroup G_GetGoodsCount %d",iGroupID);
		return 0;
	}
	if(!G_GetGoodsGroup(iGroupID,&GoodsGroup))
	{
		CCLogMeg::getSingleton().WriteError("G_GetGoodsGroup G_GetGoodsGroup %d",iGroupID);
		return 0;
	}
	if(!G_GetGoodsWeights(iGroupID,&GoodsWeights))
	{
		CCLogMeg::getSingleton().WriteError("G_GetGoodsGroup G_GetGoodsWeights %d",iGroupID);
		return 0;
	}
	if(GoodsWeights.iFakePro != 0)
		G_GetGoodsFakePro(iGroupID,&GoodsFakePros);

	//道具组抽取
	CT_INT32 iWeights=0;
	CT_INT32 iFakeWeight[GOODS_COUNT]={0};
	for(CT_INT32 i=0;i<GOODS_COUNT;++i)
	{
		if(g_iGlobeData[GLOBEDATA_TYPE_REDMI] > 0 && GoodsGroup.iGoodsID[i] == PHONE_RED_MI || 
			g_iGlobeData[GLOBEDATA_TYPE_MITHREE] > 0 && GoodsGroup.iGoodsID[i] == PHONE_MI_THREE)continue;

		if(GoodsWeights.iFakePro == 0)
		{
			iWeights+=GoodsWeights.iWeights[i];
		}
		else
		{
			if(GoodsFakePros.iProIDs[i] != 0)
			{
				iFakeWeight[i]=G_GetFakeProCount(GoodsFakePros.iProIDs[i],fb);
				iWeights+=(iFakeWeight[i]!=0?iFakeWeight[i]:GoodsWeights.iWeights[i]);
				iFakeWeight[i]=(iFakeWeight[i]!=0?iFakeWeight[i]:GoodsWeights.iWeights[i]);
			}
			else
				iWeights+=GoodsWeights.iWeights[i];
		}
	}

	CT_INT32 iWeightsNum=0;
	CT_INT32 iWeightsGroup[GOODS_COUNT]={0};
	std::map <CT_INT32,CT_INT32> WeightMap;
	for(CT_INT32 i=0;i<GOODS_COUNT;++i)
	{
		if(GoodsWeights.iWeights[i] != 0 || iFakeWeight[i] != 0)
		{
			WeightMap[iWeightsNum]=i;
			iWeightsNum++;
		}
	}

	CT_INT32 iNum=0;
	for(CT_INT32 i=0;i<GoodsWeights.iDropCount;++i)
	{
		if(iWeights <= 0)break;
		CT_INT32 iRand=rand()%iWeights;

		CT_INT32 iWeightsRand=0;
		for(CT_INT32 j=0;j<iWeightsNum;++j)
		{
			std::map <CT_INT32,CT_INT32>::iterator it=WeightMap.find(j);
			if(it == WeightMap.end())continue;
			CT_INT32 iTemp=(*it).second;
			CT_INT32 iAddWeights=(GoodsFakePros.iProIDs[j] != 0?iFakeWeight[j]:GoodsWeights.iWeights[iTemp]);
			if(GoodsGroup.iGoodsID[iTemp] != 0 && GoodsCount.iCount[iTemp] != 0 && iWeightsRand <= iRand && iRand < iWeightsRand+iAddWeights)
			{
				iGoods[iNum]=GoodsGroup.iGoodsID[iTemp];
				iCount[iNum]=GoodsCount.iCount[iTemp];
				iWeights-=iAddWeights;
				iNum++;

				G_FakeGoods fg={0};
				if(G_GetFakeProGoods(GoodsGroup.iGoodsID[iTemp],fb.iSrcType,&fg))
				{
					fb.bFakePro=CT_TRUE;
				}

				MSC_DB_GlobeData gd={0};
				if(GoodsGroup.iGoodsID[iTemp] == PHONE_RED_MI)
				{
					g_iGlobeData[GLOBEDATA_TYPE_REDMI]++;
					gd.iType=GLOBEDATA_TYPE_REDMI;
					gd.iValue=g_iGlobeData[GLOBEDATA_TYPE_REDMI];
					G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
				}
				else if(GoodsGroup.iGoodsID[iTemp] == PHONE_MI_THREE)
				{
					g_iGlobeData[GLOBEDATA_TYPE_MITHREE]++;
					gd.iType=GLOBEDATA_TYPE_MITHREE;
					gd.iValue=g_iGlobeData[GLOBEDATA_TYPE_MITHREE];
					G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA,(CT_CHAR*)&gd,sizeof(gd));
				}
#ifdef _WIN32
				//CCLogMeg::getSingleton().WriteError("goods:%d type:%d",GoodsGroup.iGoodsID[iTemp],fb.iSrcType);
				if(GoodsGroup.iGoodsID[iTemp] == 18107)
					CCLogMeg::getSingleton().WriteError("goods:%d type:%d",GoodsGroup.iGoodsID[iTemp],fb.iSrcType);
#endif

				WeightMap.erase(it);
			}
			iWeightsRand+=iAddWeights;
		}
	}

	return iNum; 
}

//获取玩家战斗属性
CT_VOID G_GetFightingData(CCUser *pUser,UserFightingStatus &UserData,CT_BOOL bSeekFight/*=CT_FALSE*/)
{
	PTRLinkUserData data={0};
	pUser->GetPTRLinkUserData(data);
	CCUser::GetFightingStatusData(UserData,data,bSeekFight);

	return;
}

//获取领主战斗属性
CT_VOID G_GetMasterFightingData(const UserFightingStatus &data,CombatUnitData &masterdata)
{
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(data.units[i].type < UNIT_ALL_ARMY && data.units[i].type != 0)
		{
			masterdata=data.units[i];
			break;
		}
	}
	return;
}

//获取领主信息
CT_BOOL G_GetLookUserInfo(PTRLinkUserData &linkUserData,UserFightingStatus &Fightinginfo,LookUserInfo &Lookinfo,CT_BOOL bSeekFight/*=CT_FALSE*/)
{
	//信息是否完整
	if(linkUserData.pMd->dwUserID == 0)
	{
		CCLogMeg::getSingleton().WriteError("G_GetLookUserInfo No Find User:%d",linkUserData.pMd->dwUserID);
		return CT_FALSE;
	}

	//提取战斗信息	
	MasterCombatStatus mcs={0};
	FightingPower Power={0};
	CCUser::GetFightingStatusData(Fightinginfo,linkUserData,bSeekFight);
	CCUser::CalcUserFinalCombatStatus(mcs, linkUserData,Power);

	//提取查看信息
	Lookinfo.dwUserID=linkUserData.pMd->dwUserID;
	Lookinfo.iGrade=linkUserData.pMd->mvd.stGrade.iLevel;
	Lookinfo.iOccupation=linkUserData.pMd->iOccupation;
	Lookinfo.iRank=linkUserData.pMd->mvd.stRank.iLevel;
	Lookinfo.iContinuousWin=linkUserData.pMd->mvd.pvpData.iCurrentContinuousWin;
	//Lookinfo.iFightingPower=linkUserData.pMd->mvd.stRes.iFightingPower;
	Lookinfo.iFightingPower=Power.iMasterFightingPower;   // 刷新领主战斗属性
	Lookinfo.iAllFightingPower=Power.iAllFightingPower;   // 刷新领主战斗属性
	memcpy(Lookinfo.cUserName,linkUserData.pMd->cNickName,NAME_SIZE);

	CombatUnitData masterdata={0};
	G_GetMasterFightingData(Fightinginfo,masterdata);
	Lookinfo.iAttack=masterdata.mcs.iAttack;
	Lookinfo.iDefence=masterdata.mcs.iDefence;
	Lookinfo.iMaxHP=masterdata.mcs.iMaxHP;
	Lookinfo.iMaxHP=masterdata.mcs.iMaxHP*masterdata.mcs.fHPPersent;
	Lookinfo.iMaxMP=masterdata.mcs.iMaxMP;


	//装备信息
	CMD_G_ItemData EquipedItemData[EQUIPED_ITEM_COUNT]={0};
	EquipmentData EquipmentInfo[EQUIPED_ITEM_COUNT]={0};
	CT_INT32 iCount=0;

	for(CCUserItem::DataMap_IT it =linkUserData.pItemMap->begin();it != linkUserData.pItemMap->end();++it)
	{
		if (it->second.iIndex > ITEM_SLOT_EQUIPMENT && it->second.iIndex <= ITEM_SLOT_BAG) // 是装备槽
		{
			Lookinfo.EquipedItemData[iCount].iID=it->second.iID;
			Lookinfo.EquipedItemData[iCount].iCount=it->second.iCount;
			Lookinfo.EquipedItemData[iCount].iIndex=it->second.iIndex;
			Lookinfo.EquipedItemData[iCount].key=it->second.key;
			iCount++;
		}
	}

	for(CT_INT32 i=0;i<EQUIPED_ITEM_COUNT;++i)
	{
		CCUserEquipment::DataMap_IT it=linkUserData.pEquipMap->find(Lookinfo.EquipedItemData[i].key);
		if(it != linkUserData.pEquipMap->end())
		{
			Lookinfo.EquipmentInfo[i].key=(*it).second.key;
			Lookinfo.EquipmentInfo[i].iLevel=(*it).second.iLevel;
			Lookinfo.EquipmentInfo[i].nSlot=(*it).second.nSlot;
			memcpy(&Lookinfo.EquipmentInfo[i].Forge,&((*it).second.Forge),sizeof(Lookinfo.EquipmentInfo[i].Forge));
			memcpy(&Lookinfo.EquipmentInfo[i].Gem,&((*it).second.Gem),sizeof(Lookinfo.EquipmentInfo[i].Gem));
		}
	}

	//纹章
	iCount=0;
	for(CCUserItem::DataMap_IT it =linkUserData.pElmMap->begin();it != linkUserData.pElmMap->end();++it)
	{
		if (GET_FLAG_BY_INDEX(it->second.iIndex) == Lookinfo.iOccupation) // 是领主的战旗槽
		{
			Lookinfo.EquipedElmData[iCount].iID=it->second.iID;
			Lookinfo.EquipedElmData[iCount].iCount=it->second.iCount;
			Lookinfo.EquipedElmData[iCount].iIndex=it->second.iIndex;
			Lookinfo.EquipedElmData[iCount].key=it->second.key;
			iCount++;
		}
	}

	// 纹章属性
	iCount = 0;
	for (CT_INT32 i = 0; i < FLAG_SLOT_COUNT; ++i)
	{
		if (Lookinfo.EquipedElmData[i].key != 0) 
		{
			CCUserElmAttr::DataMap_IT it = linkUserData.pElmAttrMap->find(Lookinfo.EquipedElmData[i].key);
			if(it != linkUserData.pElmAttrMap->end())
			{
				Lookinfo.ElmAttrInfo[iCount].iExp = (*it).second.iExp;
				Lookinfo.ElmAttrInfo[iCount].iLevel = (*it).second.iLevel;
				Lookinfo.ElmAttrInfo[iCount].key = (*it).second.key;
				memcpy(Lookinfo.ElmAttrInfo[iCount].attrs ,(*it).second.attrs,sizeof(Lookinfo.ElmAttrInfo[iCount].attrs[0]));
				iCount++;
			}
		}
	}	

	//纹章等级
	iCount=0;
	for(CCUserBattleFlag::DataMap_IT it =linkUserData.pFlagMap->begin();it != linkUserData.pFlagMap->end();++it)
	{
		if ((*it).second.iType == Lookinfo.iOccupation) 
		{
			memcpy(&Lookinfo.BattleFlag[iCount],&((*it).second),sizeof(CMD_G_BattleFlag));
			iCount++;
		}
	}	

	// VIP
	Lookinfo.iVIPLevel = (*linkUserData.pVipGrade);
	// 附魔数据
	CT_INT32 i = 0;
	for (CCUserEnchant::DataMap_IT it = linkUserData.pEnchantMap->begin(); it != linkUserData.pEnchantMap->end(); ++it) 
	{
		Lookinfo.enchantData[i].iBID = it->second.iBID;
		Lookinfo.enchantData[i].iGrade = it->second.iGrade;
		Lookinfo.enchantData[i].iIDX = it->second.iIDX;
		i++;
		if (i == sizeof(Lookinfo.enchantData) / sizeof(*Lookinfo.enchantData)) 
		{
			break;
		}

	}

	CT_DWORD dwFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(linkUserData.pMd->dwUserID);
	if (dwFTID != 0) {
		CCFightTeam *ft = NULL;
		if (CCFightTeamManager::getSingleton().Find(dwFTID, &ft)) {
			ft->GetFightTeamName(Lookinfo.cFightTeam);
		}
	}

	// 头衔
	for (CCUserTitle::DataMap_IT it = linkUserData.pTitleMap->begin();it != linkUserData.pTitleMap->end();++it)
	{
		if (it->second.bIsUse)
		{
			Lookinfo.iTitle = it->first;
			break;
		}
	}

	//宠物信息ITEM_TYPE_PET_EQUIPMENT
	memcpy(&Lookinfo.pi,linkUserData.pPetInfo,sizeof(Lookinfo.pi));
	for(CCUserItem::DataMap_IT it =linkUserData.pItemMap->begin();it != linkUserData.pItemMap->end();++it)
	{
		if (it->second.iIndex >= ITEM_SLOT_NECKLACE && it->second.iIndex <= ITEM_SLOT_BAG) // 是装备槽
		{
			Lookinfo.PetEquipedItemData.iID=it->second.iID;
			Lookinfo.PetEquipedItemData.iCount=it->second.iCount;
			Lookinfo.PetEquipedItemData.iIndex=it->second.iIndex;
			Lookinfo.PetEquipedItemData.key=it->second.key;
			iCount++;
		}
	}

	CCUserEquipment::DataMap_IT it=linkUserData.pEquipMap->find(Lookinfo.PetEquipedItemData.key);
	if(it != linkUserData.pEquipMap->end())
	{
		Lookinfo.PetEquipmentInfo.key=(*it).second.key;
		Lookinfo.PetEquipmentInfo.iLevel=(*it).second.iLevel;
		Lookinfo.PetEquipmentInfo.nSlot=(*it).second.nSlot;
		memcpy(&Lookinfo.PetEquipmentInfo.Forge,&((*it).second.Forge),sizeof(Lookinfo.PetEquipmentInfo.Forge));
		memcpy(&Lookinfo.PetEquipmentInfo.Gem,&((*it).second.Gem),sizeof(Lookinfo.PetEquipmentInfo.Gem));
	}


	return CT_TRUE;
}

//资源类型转换
CT_BOOL G_ChangeResType(MasterVariableData &mvd, const CT_INT32 iResType, CT_INT32& iRes)
{
	switch(iResType)
	{
	case VALUE_TYPE_GOLD:
		{
			mvd.stRes.iGold+=iRes;
			iRes = mvd.stRes.iGold;
			return CT_TRUE;
		}
	case VALUE_TYPE_DIAMOND:
		{
			mvd.stRes.iDiamod+=iRes;
			iRes = mvd.stRes.iDiamod;
			return CT_TRUE;
		}
	case VALUE_TYPE_FOOD:
		{
			mvd.stRes.iFood+=iRes;
			iRes = mvd.stRes.iFood;
			return CT_TRUE;
		}
	case VALUE_TYPE_STAMINA:
		{
			mvd.stRes.iStamina+=iRes;
			iRes = mvd.stRes.iStamina;
			return CT_TRUE;
		}
	case VALUE_TYPE_TALENTNUM:
		{
			mvd.stRes.iTalentNum+=iRes;
			iRes = mvd.stRes.iTalentNum;
			return CT_TRUE;
		}
	case VALUE_TYPE_MASTERXP:
		{
			mvd.stGrade.iCurrentExp+=iRes;
			iRes = mvd.stGrade.iCurrentExp;
			return CT_TRUE;
		}
	case VALUE_TYPE_RANKXP:
		{
			mvd.stRank.iCurrentExp+=iRes;
			iRes = mvd.stRank.iCurrentExp;
			return CT_TRUE;
		}
	case VALUE_TYPE_VIP:
		{
			return CT_TRUE;
		}
	case 0:
		{
			return CT_FALSE;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("G_ChangeResType No Find Res %d", iResType);
			return CT_FALSE;
		}
	}
	return CT_FALSE;
}


//合并奖励的道具
CT_VOID MergeItem(CT_INT32 iItemGroup, CT_INT32* iGoods, CT_INT32* iCount, CT_INT32& iIndex,FakeProBase &fb)
{
	CT_INT32 iGoodsGroup[GOODS_COUNT] = {0};
	CT_INT32 iCountGroup[GOODS_COUNT] = {0};

	CT_INT32 iGoodCount = G_GetGoodsGroupInfo(iItemGroup, iGoodsGroup, iCountGroup,fb);
	for (CT_INT32 j=0; j<iGoodCount; ++j)
	{
		CT_INT32 k=0;
		for ( ; k<iIndex; ++k)
		{
			if (iGoods[k] == iGoodsGroup[j])
			{
				iCount[k] += iCountGroup[j];
				break;
			}
		}

		if (k >= iIndex)
		{
			iGoods[iIndex]   = iGoodsGroup[j];
			iCount[iIndex]   = iCountGroup[j];
			++iIndex;
		}
	}
}

//添加奖励或者消耗
ENUM_CostRewardErrorCode G_PerformCostReward(CT_INT32 iRewardID, CCUser* pUser, ENUM_PerformCostRewardType type, CMD_G_NormalReward& addReward, CT_INT32* pGoods, CT_INT32* pCount,FakeProBase &fb)
{
	G_CostRewardGroup rewardGroup = {0};
	if(!G_GetCostRewardGroup(iRewardID, &rewardGroup))
	{
		//CT_ASSERT(CT_FALSE);
        CCLogMeg::getSingleton().WriteError(" G_PerformCostReward 没有找到奖励消耗组 GrpID=%i",iRewardID);
		return ENUM_CREC_NO_ID_FAIL;
	}
	
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	Resoure  resAdd[TEAM_RES_CNT] = {0};
	Resoure  res[TEAM_RES_CNT] = {0};
	for (CT_INT32 i=0; i<TEAM_RES_CNT; ++i)
	{
		CT_INT32 iRes = rewardGroup.res[i].iNum;
		// 但奖励资源为Vip类型时，领主直接升级，若领主已Vip已最高级则不作处理
		if (rewardGroup.res[i].iID == VALUE_TYPE_VIP && (type == ENUM_PCRT_PERFORM	|| type == ENUM_PCRT_TEST_PERFORM))
		{
			CT_INT32 iGrade = pUser->m_userVip.VipGradeGet();
			G_ValueRange vr = {0};
			if (!G_GetValueRange(VALUE_TYPE_VIP, &vr))
			{
				CCLogMeg::getSingleton().WriteError("G_GetValueRange VALUE_TYPE_VIP error. ");
				return ENUM_CREC_NO_ID_FAIL;
			}
			if (iGrade >= vr.fMaxVal || iGrade == 3) // vip已达3级时不自动升为4级
			{
				continue;
			}
			else
			{
				G_VipType vipType ={0};
				if (!G_GetVipTypeInfo(iGrade + 1, &vipType))
				{
					CCLogMeg::getSingleton().WriteError("G_GetVipTypeInfo  error. ");
					return ENUM_CREC_NO_ID_FAIL;
				}
				pUser->m_userVip.VipRenewal(vipType.iGrade, vipType.iTime, 0, 0);
			}
			res[i].iID  =  0;	// vip升级后相应的资源以及数量不用再更新
			res[i].iNum =  0;
			continue;
		}
		if(rewardGroup.res[i].iID != 0 && ( !G_ChangeResType(mvd, rewardGroup.res[i].iID, iRes) || iRes <0) )
		{
			return ENUM_CREC_RES_FAIL;
			
		}
		
		res[i].iID  =  rewardGroup.res[i].iID;
		res[i].iNum =  rewardGroup.res[i].iNum;
	}

	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCounts[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	
	CT_INT32 i=0, iCount = 0;
	for (; i<TEAM_ITEM_CNT; ++i)
	{
		if ( rewardGroup.item[i].iID != 0 && rewardGroup.item[i].iNum != 0)
		{
			iGoods[iCount] = rewardGroup.item[i].iID;
			iCounts[iCount] = rewardGroup.item[i].iNum;
			++iCount;
		}
	}
	
	if (rewardGroup.iItemGroup1 != 0)
	{
		MergeItem(rewardGroup.iItemGroup1, iGoods, iCounts, iCount,fb);	
	}

	if (rewardGroup.iItemGroup2 != 0)
	{
		MergeItem(rewardGroup.iItemGroup2, iGoods, iCounts, iCount,fb);
	}

	ENUM_LootItem bResult = pUser->LootItems(0, iGoods, iCounts, iCount, ITEM_OP_MODE_CHECK);
	if (LOOT_SUC != bResult  && type == ENUM_PCRT_TEST )
	{
		if (LOOT_ELM_FULL == bResult)
		{
			return ENUM_CREC_ELMBAG_NOAREA;     // 元素背包不足
		}
		return ENUM_CREC_ITEM_FAIL;             // 物品背包不足
	}
	
	if (LOOT_SUC != bResult  &&  type == ENUM_PCRT_PERFORM)
	{
		if (LOOT_ELM_FULL == bResult)
		{
			return ENUM_CREC_ELMBAG_NOAREA;    
		}
		return ENUM_CREC_ITEM_FAIL;
	}

	addReward.dwUserID = pUser->GetUserID();
	addReward.iResType0 = res[0].iID;
	addReward.iResNum0 = res[0].iNum;
	addReward.iResType1 = res[1].iID;
	addReward.iResNum1 = res[1].iNum;
	addReward.iResType2 = res[2].iID;
	addReward.iResNum2 = res[2].iNum;
	addReward.iResType3 = res[3].iID;
	addReward.iResNum3 = res[3].iNum;
	memcpy(pGoods, iGoods, sizeof(iGoods));
	memcpy(pCount, iCounts, sizeof(iCounts));

	if (type == ENUM_PCRT_PERFORM || type == ENUM_PCRT_TEST_PERFORM )
	{
		pUser->LootItems(0, iGoods, iCounts, iCount, ITEM_OP_MODE_EXEC);
	}
	
	return ENUM_CREC_RES_SUC;
}


//添加奖励或者消耗
CT_VOID G_GetReward(CT_INT32 iRewardID, Resoure* pRes,  Item* pItem, CT_INT32* pGoods, CT_INT32* pCount,FakeProBase &fb)
{
	G_CostRewardGroup rewardGroup = {0};
	if(!G_GetCostRewardGroup(iRewardID, &rewardGroup))
	{
		//CT_ASSERT(CT_FALSE);
        CCLogMeg::getSingleton().WriteError(" G_PerformCostReward 没有找到奖励消耗组 GrpID=%i",iRewardID);
		return ;
	}

	memcpy(pRes, rewardGroup.res, sizeof(rewardGroup.res));
	memcpy(pItem, rewardGroup.item, sizeof(rewardGroup.item));

	CT_INT32 i=0, iCount = 0;
	//for (; i<TEAM_ITEM_CNT; ++i)
	//{
	//	if ( rewardGroup.item[i].iID != 0 && rewardGroup.item[i].iNum != 0)
	//	{
	//		pGoods[iCount] = rewardGroup.item[i].iID;
	//		pCount[iCount] = rewardGroup.item[i].iNum;
	//		++iCount;
	//	}
	//}

	if (rewardGroup.iItemGroup1 != 0)
	{
		MergeItem(rewardGroup.iItemGroup1, pGoods, pCount, iCount,fb);	
	}

	if (rewardGroup.iItemGroup2 != 0)
	{
		MergeItem(rewardGroup.iItemGroup2, pGoods, pCount, iCount,fb);
	}
}

//添加奖励消耗
CT_INT32 G_GetCostReward(CT_INT32 iID,CCUser *pUser,const CT_DWORD dwtype,const CT_DWORD saveTag,RewardCost &rc,FakeProBase &fb)
{
	G_CostRewardGroup rewardGroup = {0};
	if(!G_GetCostRewardGroup(iID, &rewardGroup))
	{
		//CT_ASSERT(CT_FALSE);
		CCLogMeg::getSingleton().WriteError(" G_GetCostReward No Find RewardGroup=%d",iID);
		return ENUM_CREC_NO_ID_FAIL;
	}

	CT_INT32 iGoodsNum=0;
	for(CT_INT32 i=0;i<sizeof(rewardGroup.res)/sizeof(*rewardGroup.res);++i)
	{
		rc.iUseResType[i]=rewardGroup.res[i].iID;
		rc.iUseResCount[i]=rewardGroup.res[i].iNum;
	}
	for(CT_INT32 i=0;i<sizeof(rewardGroup.item)/sizeof(*rewardGroup.item);++i)
	{
		if(rewardGroup.item[i].iID == 0)continue;
		rc.iUseItem[iGoodsNum]=rewardGroup.item[i].iID;
		rc.iUseItemCount[iGoodsNum]=rewardGroup.item[i].iNum;
		if(iGoodsNum < GOODS_COUNT*2)iGoodsNum++;
	}

	CT_INT32 iGoods[GOODS_COUNT*2]={0};
	CT_INT32 iCount[GOODS_COUNT*2]={0};
	if (rewardGroup.iItemGroup1 != 0)
	{
		MergeItem(rewardGroup.iItemGroup1, rc.iUseItem, rc.iUseItemCount, iGoodsNum,fb);
	}

	if (rewardGroup.iItemGroup2 != 0)
	{
		MergeItem(rewardGroup.iItemGroup2, rc.iUseItem, rc.iUseItemCount, iGoodsNum,fb);
	}

	if(dwtype == ITEM_OP_MODE_CHECK)
	{
		MasterVariableData mvd={0};
		for(CT_INT32 i=0;i<sizeof(rewardGroup.res)/sizeof(*rewardGroup.res);++i)
		{
			rc.iCurrentResCount[i]=rc.iUseResCount[i];
			pUser->m_userBase.GetMasterData(mvd);
			G_ChangeResType(mvd,rc.iUseResType[i],rc.iCurrentResCount[i]);
			if(rc.iCurrentResCount[i] < 0)return ENUM_CREC_RES_FAIL;
		}
		
		return pUser->LootItems(saveTag,rc.iUseItem,rc.iUseItemCount,iGoodsNum,(ITEM_OP_MODE)dwtype);
	}
	else
	{
		CT_INT32 enfail = 0;
		MasterVariableData mvd={0};
		MasterVariableData Tempmvd={0};
		pUser->m_userBase.GetMasterData(Tempmvd);
		for(CT_INT32 i=0;i<sizeof(rewardGroup.res)/sizeof(*rewardGroup.res);++i)
		{
			CT_INT32 iResCount=rc.iUseResCount[i];
			G_ChangeResType(mvd,rc.iUseResType[i],iResCount);

			rc.iCurrentResCount[i]=rc.iUseResCount[i];
			G_ChangeResType(Tempmvd,rc.iUseResType[i],rc.iCurrentResCount[i]);
		}
		pUser->m_userBase.AddMastData(mvd,saveTag);

		//return pUser->LootItems(saveTag,rc.iUseItem,rc.iUseItemCount,iGoodsNum,(ITEM_OP_MODE)dwtype);
		enfail = pUser->LootItems(saveTag,rc.iUseItem,rc.iUseItemCount,iGoodsNum,(ITEM_OP_MODE)dwtype);
		if (enfail == LOOT_BAG_FULL || enfail == LOOT_ELM_FULL)
		{
			for (CT_INT32 iPresent = 0;iPresent < iGoodsNum;++iPresent)
			{
				MSC_DB_RESREWARD rr={0};
				rr.dwUID=pUser->GetUserID();
				rr.iPID=rc.iUseItem[iPresent];
				rr.iPNum=rc.iUseItemCount[iPresent];
				G_SendDBThreadInfo(pUser->GetUserID(),MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_RES_PRODUCT_REWARD,(CT_CHAR*)&rr,sizeof(rr));
			}

			pUser->UpdateAndSendMasterData();
		}
		else
		{
			return enfail;
		}
	}

	return 0;
}

// 产生用户随机
CT_INT32 G_Random(CT_DWORD dwUserID, CT_INT32 iParam1, CT_INT32 iParam2)
{
    srand(dwUserID * ((CT_INT32)(time(0) << iParam1)) * iParam2);
    CT_INT32 ret = rand();
    if (ret < 0) 
    {
        CCLogMeg::getSingleton().WriteError("G_Random: ret = %i", ret);
    }
    return abs(ret);
}

CT_VOID G_SendClientData( CCUser &user, CT_DWORD mainID, const CT_DWORD subID, CT_CHAR *buf, CT_INT32 bufLen )
{
    user.GetNetInterface()->Send(mainID, subID, buf, bufLen);
}

//保存战报
CT_VOID G_SaveFightingRecord(CCDBConnector& m_dbConnector,UserFightingStatus &FightingData,CT_INT32 iPos,CT_INT32 iSeed)
{
	m_dbConnector.AddParm("_uid ", FightingData.dwUserID);
	m_dbConnector.AddParm("_NickName", FightingData.units[iPos].name);
	m_dbConnector.AddParm("_hpIncRate", FightingData.units[iPos].mcs.fHPPersent);
	m_dbConnector.AddParm("_dmgIncRate", FightingData.units[iPos].mcs.fDmgIncPercent);
	m_dbConnector.AddParm("_dmgDesRate", FightingData.units[iPos].mcs.fDmgDecPercent);
	m_dbConnector.AddParm("_Rand ", iSeed);
	m_dbConnector.AddParm("_Grade ", FightingData.units[iPos].level);
	m_dbConnector.AddParm("_Score ", 0);
	m_dbConnector.AddParm("_Occupation ", FightingData.units[iPos].type);
	m_dbConnector.AddParm("_Hp ", FightingData.units[iPos].mcs.iMaxHP);
	m_dbConnector.AddParm("_Mp ", FightingData.units[iPos].mcs.iMaxMP);
	m_dbConnector.AddParm("_Strength ", FightingData.units[iPos].mcs.iStrength);
	m_dbConnector.AddParm("_Agility ", FightingData.units[iPos].mcs.iAgility);
	m_dbConnector.AddParm("_Interllect  ", FightingData.units[iPos].mcs.iIntellect);
	m_dbConnector.AddParm("_Spirit ", FightingData.units[iPos].mcs.iSpirit);
	m_dbConnector.AddParm("_HitRate ", FightingData.units[iPos].mcs.fHitRate);
	m_dbConnector.AddParm("_DodgeRate ", FightingData.units[iPos].mcs.fDodgeRate);
	m_dbConnector.AddParm("_CritRate ", FightingData.units[iPos].mcs.fCritRate);
	m_dbConnector.AddParm("_ResilientRate ", FightingData.units[iPos].mcs.fResilient);
	m_dbConnector.AddParm("_AttackAddRate ", FightingData.units[iPos].mcs.fDmgIncPercent);
	m_dbConnector.AddParm("_DamageReduceRate ", FightingData.units[iPos].mcs.fDmgDecPercent);
	m_dbConnector.AddParm("_Attack ", FightingData.units[iPos].mcs.iAttack);
	m_dbConnector.AddParm("_Defense ", FightingData.units[iPos].mcs.iDefence);
	m_dbConnector.AddParm("_Pos ", iPos);
	m_dbConnector.AddParm("_Skill0 ", FightingData.units[iPos].Skills[0].iID);
	m_dbConnector.AddParm("_Skill1 ", FightingData.units[iPos].Skills[1].iID);
	m_dbConnector.AddParm("_Skill2  ", FightingData.units[iPos].Skills[2].iID);
	m_dbConnector.AddParm("_Skill3 ", FightingData.units[iPos].Skills[3].iID);
	m_dbConnector.AddParm("_hprecover  ", FightingData.units[iPos].mcs.iHPRecover);
	m_dbConnector.AddParm("_mprecover ", FightingData.units[iPos].mcs.iMPRecover);
	m_dbConnector.AddParm("_petchangeid", FightingData.units[iPos].iPetChangeID);
	m_dbConnector.AddParm("Speed", FightingData.units[iPos].mcs.fSpeed);
	m_dbConnector.AddParm("PetSkill", FightingData.units[iPos].PetSkill.iID);
	return;
}

//读取战报
CT_VOID G_GetFightingRecord(CCDBResult* pResult,CombatUnitData &data,CT_INT32 &iSceneStar,CT_INT32 &iSeed,CT_INT32 &iIndex,CT_DWORD &dwUserID)
{
	pResult->GetValue(iIndex++,	dwUserID);
	pResult->GetValue(iIndex++, data.name,NAME_SIZE );
	pResult->GetValue(iIndex++, data.mcs.fHPPersent);
	pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
	pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
	pResult->GetValue(iIndex++, iSeed );
	pResult->GetValue(iIndex++, data.level );
	pResult->GetValue(iIndex++, data.type );
	pResult->GetValue(iIndex++, iSceneStar);
	pResult->GetValue(iIndex++, data.mcs.iMaxHP );
	pResult->GetValue(iIndex++, data.mcs.iMaxMP );
	pResult->GetValue(iIndex++, data.mcs.iStrength );
	pResult->GetValue(iIndex++, data.mcs.iAgility );
	pResult->GetValue(iIndex++, data.mcs.iIntellect);
	pResult->GetValue(iIndex++, data.mcs.iSpirit );
	pResult->GetValue(iIndex++, data.mcs.fHitRate );
	pResult->GetValue(iIndex++, data.mcs.fDodgeRate );
	pResult->GetValue(iIndex++, data.mcs.fCritRate );
	pResult->GetValue(iIndex++, data.mcs.fResilient );
	pResult->GetValue(iIndex++, data.mcs.fDmgIncPercent );
	pResult->GetValue(iIndex++, data.mcs.fDmgDecPercent );
	pResult->GetValue(iIndex++, data.mcs.iAttack );
	pResult->GetValue(iIndex++, data.mcs.iDefence );
	pResult->GetValue(iIndex++, data.position);
	pResult->GetValue(iIndex++, data.Skills[0].iID );
	pResult->GetValue(iIndex++, data.Skills[1].iID );
	pResult->GetValue(iIndex++, data.Skills[2].iID );
	pResult->GetValue(iIndex++, data.Skills[3].iID );
	pResult->GetValue(iIndex++, data.mcs.iHPRecover );
	pResult->GetValue(iIndex++, data.mcs.iMPRecover );
	pResult->GetValue(iIndex++, data.iPetChangeID );
	pResult->GetValue(iIndex++, data.mcs.fSpeed );
	pResult->GetValue(iIndex++, data.PetSkill.iID);
	return;
}

//获取为概率数据
CT_VOID G_GetFakeProDta(CCUser *pUser,CT_INT32 iTage,FakeProBase &fb)
{
	if(pUser == NULL)return;

	MasterData md={0};
	pUser->m_userBase.GetMasterData(md);
	switch(iTage)
	{
	case SAVETAGEX_TOWER_TIME:
		{
			fb.iSrcType=FAKEPRO_TYPE_TOWER;
			fb.iDataCount=md.mex.fpd.iFPTowerTime;
			fb.iMasterGrade=md.mvd.stGrade.iLevel;
			fb.iChargeDiamod=md.mex.iChargeDiamod;
			fb.iCostDiamod=md.mex.iCostDiamod;
			pUser->m_userBase.SetMastDataEx(fb.iDataCount+1,SAVETAGEX_TOWER_TIME);
			break;
		}
	case SAVETAGEEX_WORSHIP_TIME0:
	case SAVETAGEEX_WORSHIP_TIME1:
	case SAVETAGEEX_WORSHIP_TIME2:
	case SAVETAGEEX_WORSHIP_TIME3:
	case SAVETAGEEX_WORSHIP_TIME4:
	case SAVETAGEEX_WORSHIP_TIME5:
		{
			CT_INT32 iType=iTage - SAVETAGEEX_WORSHIP_TIME0;
			if(iType >= 0 && iType < WORSHIP_COUNT)
			{
				fb.iSrcType=FAKEPRO_TYPE_WORSHIP;
				fb.iDataCount=md.mex.fpd.iFPWorshipTime[iType];
				fb.iMasterGrade=md.mvd.stGrade.iLevel;
				fb.iChargeDiamod=md.mex.iChargeDiamod;
				fb.iCostDiamod=md.mex.iCostDiamod;
				pUser->m_userBase.SetMastDataEx(fb.iDataCount+1,iTage);
			}
			break;
		}
	case SAVETAGEEX_OPEN_CHEST:
		{
			fb.iSrcType=FAKEPRO_TYPE_CHEST;
			fb.iDataCount=md.mex.fpd.iFPChestTime;
			fb.iMasterGrade=md.mvd.stGrade.iLevel;
			fb.iChargeDiamod=md.mex.iChargeDiamod;
			fb.iCostDiamod=md.mex.iCostDiamod;
			pUser->m_userBase.SetMastDataEx(fb.iDataCount+1,iTage);
			break;
		}
	case SAVETAGEEX_FINISH_CAVE:
		{
			fb.iSrcType=FAKEPRO_TYPE_CAVE;
			fb.iDataCount=md.mex.fpd.iCaveTime;
			fb.iMasterGrade=md.mvd.stGrade.iLevel;
			fb.iChargeDiamod=md.mex.iChargeDiamod;
			fb.iCostDiamod=md.mex.iCostDiamod;
			pUser->m_userBase.SetMastDataEx(fb.iDataCount+1,iTage);
			break;
		}
	case SAVETAGEEX_PVP_CHEST:
		{
			fb.iSrcType=FAKEPRO_TYPE_PVP_CHEST;
			fb.iDataCount=md.mex.fpd.iPVPChest;
			fb.iMasterGrade=md.mvd.stGrade.iLevel;
			fb.iChargeDiamod=md.mex.iChargeDiamod;
			fb.iCostDiamod=md.mex.iCostDiamod;
			pUser->m_userBase.SetMastDataEx(fb.iDataCount+1,iTage);
			break;
		}
	case SAVETAGEEX_FT_CHEST:
		{
			fb.iSrcType=FAKEPRO_TYPE_FT_CHEST;
			fb.iDataCount=md.mex.fpd.iFTChest;
			fb.iMasterGrade=md.mvd.stGrade.iLevel;
			fb.iChargeDiamod=md.mex.iChargeDiamod;
			fb.iCostDiamod=md.mex.iCostDiamod;
			pUser->m_userBase.SetMastDataEx(fb.iDataCount+1,iTage);
			break;
		}
	case SAVETAGEEX_TURNTABLE0:
	case SAVETAGEEX_TURNTABLE1:
	case SAVETAGEEX_TURNTABLE2:
		{
			CT_INT32 iType=iTage - SAVETAGEEX_TURNTABLE0;
			if(iType >= 0 && iType < TURNTABLE_COUNT)
			{
				fb.iSrcType=FAKEPRO_TYPE_TURNTABLE;
				fb.iDataCount=md.mex.fpd.iTurntable[iType];
				fb.iMasterGrade=md.mvd.stGrade.iLevel;
				fb.iChargeDiamod=md.mex.iChargeDiamod;
				fb.iCostDiamod=md.mex.iCostDiamod;
				pUser->m_userBase.SetMastDataEx(fb.iDataCount+1,iTage);
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("GetFakeProDta");
			break;
		}
	}
}

//清理为概率数据
CT_VOID G_ClearFakeProDat(CCUser *pUser,CT_INT32 iTage,FakeProBase &fb)
{
	if(pUser == NULL || !fb.bFakePro)return;

	pUser->m_userBase.SetMastDataEx(0,iTage);
	return;
}

//滚动公告
CT_BOOL G_GetRollBroadecastData(CT_INT32 iID,CT_CHAR *pOutBuf,CT_INT32 iOutLen,CT_CHAR *pInBuf,CT_INT32 iInBufLen)
{
	if(iInBufLen%sizeof(RollBroadcast) != 0)
	{
		CCLogMeg::getSingleton().WriteError("G_GetRollBroadecastInfo iInBufLen%sizeof(RollBroadcast) != 0");
		return CT_FALSE;
	}

	
	std::map<CT_INT32,G_RollBroadcastInfo> rbimap;
	for(CT_INT32 i=0;i<iInBufLen/sizeof(RollBroadcast);++i)
	{
		RollBroadcast rb={0};
		G_RollBroadcastInfo rbi={0};
		CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
		memcpy(&rb,pInBuf+i*sizeof(RollBroadcast),sizeof(RollBroadcast));
		if(G_GetRollBroadcastMes(iID,i+1,&rbi))
		{
			if(rb.iBroadInfo != 0 && strlen(rb.cBroadInfo) == 0)
				sprintf(cBuf,rbi.cMes,rb.iBroadInfo);
			else if(rb.iBroadInfo == 0 && strlen(rb.cBroadInfo) != 0)
				sprintf(cBuf,rbi.cMes,rb.cBroadInfo);
			else
				sprintf(cBuf,rbi.cMes,rb.iBroadInfo,rb.cBroadInfo);

			memcpy(rbi.cMes,cBuf,sizeof(rbi.cMes));
			rbimap.insert(std::map<CT_INT32,G_RollBroadcastInfo>::value_type(rbi.iOtherOrder,rbi));
		}
	}

	CT_INT32 iSize=0;
	for(std::map<CT_INT32,G_RollBroadcastInfo>::iterator it=rbimap.begin();it != rbimap.end();++it)
	{
		if(strlen(pOutBuf)+strlen((*it).second.cMes) < iOutLen-1)
			strcat(pOutBuf,(*it).second.cMes);
		else
		{
			printf("pOutBuf:%d cMes:%d\n",strlen(pOutBuf),strlen((*it).second.cMes));
			CCLogMeg::getSingleton().WriteError("G_GetRollBroadecastData strlen(pOutBuf)+strlen((*it).second.cMes) < iOutLen-1 ");
			return CT_FALSE;
		}
	}

	return CT_TRUE;
}

//发送公告
CT_VOID G_SendRollBroadecast(CT_INT32 iID,CT_INT32 iBroadcastType,CT_INT32 iBroadcastTime,CT_CHAR *pInBuf,CT_INT32 iInBufLen)
{
	//播放公告
	CT_CHAR cBroadcastMes[CHAT_LEN*2] = {0};
	CT_INT32 iMesLen = 0;
	CT_INT32 iBroadcastLen=CHAT_LEN*2-(1+sizeof(iBroadcastType)+sizeof(iBroadcastTime));
	memcpy(cBroadcastMes, &iBroadcastType, sizeof(iBroadcastType));
	memcpy(cBroadcastMes + sizeof(iBroadcastType), &iBroadcastTime, sizeof(iBroadcastTime));
	G_GetRollBroadecastData(iID,cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime),iBroadcastLen,pInBuf,iInBufLen);
	iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
	G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);

	return;
}

//发送城市线程消息
CT_VOID G_SendCityThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len)
{
	CT_DWORD dwThreadID=GAME_THREAD_CITY + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(dwThreadID,mainID, subID,pBuf,len);
}
//发送战斗线程消息
CT_VOID G_SendFightThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len)
{
	CT_DWORD dwThreadID=GAME_THREAD_FIGHT + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(dwThreadID,mainID, subID,pBuf,len);
}
//发送DB线程消息
CT_VOID G_SendDBThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len)
{
	CT_DWORD dwThreadID=GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(dwThreadID,mainID, subID,pBuf,len);
}
//发送LOGDB线程消息
CT_VOID G_SendLogDBThreadInfo(CT_DWORD dwUserID,CT_DWORD mainID, CT_DWORD subID, const CT_CHAR* pBuf, CT_INT32 len)
{
	CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(dwThreadID,mainID, subID,pBuf,len);
}

//战斗属性加成
CT_VOID G_AddFightAttribute(MasterCombatStatus &mcs,const CT_INT32 iType,const CT_FLOAT fValue,const CT_FLOAT fValuePercent/*=1.0f*/)
{
	switch (iType) 
	{
	case VALUE_TYPE_HEALTH:         mcs.iMaxHP=mcs.iMaxHP*fValuePercent+fValue; break;
	case VALUE_TYPE_HPPENCENT:      mcs.fHPPersent=mcs.fHPPersent*fValuePercent+fValue; break;
	case VALUE_TYPE_MANA:           mcs.iMaxMP=mcs.iMaxMP*fValuePercent+fValue; break;
	case VALUE_TYPE_ATTACK:         mcs.iAttack=mcs.iAttack*fValuePercent+fValue; break;
	case VALUE_TYPE_DEFENCE:        mcs.iDefence =mcs.iDefence*fValuePercent+fValue; break;
	case VALUE_TYPE_STRENTH:        mcs.iStrength =mcs.iStrength*fValuePercent+fValue; break;
	case VALUE_TYPE_AGILITY:        mcs.iAgility =mcs.iAgility*fValuePercent+fValue; break;
	case VALUE_TYPE_INTELLENT:      mcs.iIntellect =mcs.iIntellect*fValuePercent+fValue; break;
	case VALUE_TYPE_SPIRIT:         mcs.iSpirit =mcs.iSpirit*fValuePercent+fValue; break;
	case VALUE_TYPE_HITRATE:        mcs.fHitRate =mcs.fHitRate*fValuePercent+fValue; break;
	case VALUE_TYPE_DODGERATE:      mcs.fDodgeRate =mcs.fDodgeRate*fValuePercent+fValue; break;
	case VALUE_TYPE_CRITRATE:       mcs.fCritRate =mcs.fCritRate*fValuePercent+fValue; break;
	case VALUE_TYPE_RESILIENT:      mcs.fResilient =mcs.fResilient*fValuePercent+fValue; break;
	case VALUE_TYPE_DMGINCPERCENT:  mcs.fDmgIncPercent =mcs.fDmgIncPercent*fValuePercent+fValue; break;
	case VALUE_TYPE_DMGDECPERCENT:  mcs.fDmgDecPercent =mcs.fDmgDecPercent*fValuePercent+fValue; break;
	case VALUE_TYPE_EQUIPPERCENT:   mcs.fEquipPercent =mcs.fEquipPercent*fValuePercent+fValue;break;
	case VALUE_TYPE_SPEED:			mcs.fSpeed =mcs.fSpeed*fValuePercent+fValue;break;
	}
	return;
}

//获取战斗属性值
CT_INT32 G_GetMasterCombatStatusValue(CT_INT32 iType,MasterCombatStatus &mcs)
{
	switch (iType) 
	{
	case VALUE_TYPE_HEALTH:         return mcs.iMaxHP;
	case VALUE_TYPE_HPPENCENT:      return mcs.fHPPersent;
	case VALUE_TYPE_MANA:           return mcs.iMaxMP;
	case VALUE_TYPE_ATTACK:         return mcs.iAttack;
	case VALUE_TYPE_DEFENCE:        return mcs.iDefence;
	case VALUE_TYPE_STRENTH:        return mcs.iStrength;
	case VALUE_TYPE_AGILITY:        return mcs.iAgility;
	case VALUE_TYPE_INTELLENT:      return mcs.iIntellect;
	case VALUE_TYPE_SPIRIT:         return mcs.iSpirit;
	case VALUE_TYPE_HITRATE:        return mcs.fHitRate;
	case VALUE_TYPE_DODGERATE:      return mcs.fDodgeRate;
	case VALUE_TYPE_CRITRATE:       return mcs.fCritRate;
	case VALUE_TYPE_RESILIENT:      return mcs.fResilient;
	case VALUE_TYPE_DMGINCPERCENT:  return mcs.fDmgIncPercent;
	case VALUE_TYPE_DMGDECPERCENT:  return mcs.fDmgDecPercent;
	case VALUE_TYPE_EQUIPPERCENT:   return mcs.fEquipPercent;
	case VALUE_TYPE_SPEED:			return mcs.fSpeed;
	}
	return 0;
}
//获取探访信息
CT_BOOL G_GetSeekerData(CCUser* pUser,MSC_DB_Seeked_Info& seeker)
{
	CT_INT32 iLen = 0;
	MasterData md = {0};
	SeekData sd = {0};
	CMD_G_BuildingData bd[SEEK_BUILD_NUM] = {0};
	pUser->m_userBase.GetMasterData(md);
	pUser->m_userSeek.GetSeekData(sd);
	seeker.dwUserID = pUser->GetUserID();
	seeker.iDiamond = md.mvd.stRes.iDiamod;
	seeker.iMoney = md.mvd.stRes.iGold;
	seeker.iFood = md.mvd.stRes.iFood;
	seeker.iWorker = md.mvd.stRes.iWorker;
	seeker.iMaxWorker = md.mvd.stRes.iMaxWorker;
	seeker.iKidnapSaintess = sd.iWomanNum;
	seeker.iKidnapWorkerNum = sd.iFarmarNum;
	seeker.iSeekedWillTimes = sd.iSeekedWillTimes;
	seeker.iSeekedSpiteTimes = sd.iSeekedSpiteTimes;
	memcpy(seeker.iPetKidnap, sd.iPet, sizeof(seeker.iPetKidnap));
	
	iLen = sizeof(seeker.teamFlag);
	pUser->m_userSeek.GetTeamFlag((CT_CHAR*)seeker.teamFlag, iLen);

	iLen = sizeof(bd);
	pUser->m_userBuilding.GetBuildingData((CT_CHAR*)&bd, iLen);
	for (CT_INT32 i = 0;i < iLen / sizeof(CMD_G_BuildingData);++i)
	{
		seeker.buildingData[i].bd = bd[i];
	}

	iLen = sizeof(seeker.productBuilding);
	pUser->m_userBuilding.GetProductingData(seeker.productBuilding, iLen);

	iLen = sizeof(seeker.upgradeBuilding);
	pUser->m_userBuilding.GetUpgradingBuildingData(seeker.upgradeBuilding, iLen);

	iLen = sizeof(seeker.present);
	pUser->m_userSeek.GetAllPresent((CT_CHAR*)seeker.present, iLen);

	return CT_TRUE;
}

//发送错误消息
CT_VOID G_SendErrorInfo(CCUser *pUser,CT_INT32 iType, CT_INT32 iID)
{
	G_ErrorInfo ei={0};
	if(!G_GetErrorInfo(iType,iID,&ei))
	{
		sprintf(ei.cInfo,"%s","数据错误！");
		CCLogMeg::getSingleton().WriteWaring("G_GetErrorInfo Type:%d ID:%d",iType,iID);
	}

	CMD_G_ErrorInfo ErroeInfo={0};
	ErroeInfo.iType=iType;
	ErroeInfo.iID=iID;
	memcpy(ErroeInfo.cErrorInfo,ei.cInfo,CHAT_LEN);
	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_ERROR_INFO,(CT_CHAR*)&ErroeInfo,sizeof(ErroeInfo));
	return;
}