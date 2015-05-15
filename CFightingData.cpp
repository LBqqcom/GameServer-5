#include "CFightingData.h"
#include "CLogMeg.h"
#include "CGlobeFunc.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CTowerResManager.h"
#include "CTop.h"
#include<list>

template<> CCFightingData* CCSingleton<CCFightingData>::ms_Singleton=0;

#define DELETE_TIME		1800
CCFightingData::CCFightingData(void)
{
	m_FightingDataMap.clear();
	m_SeekerMap.clear();
}

CCFightingData::~CCFightingData(void)
{
	m_FightingDataMap.clear();
	m_SeekerMap.clear();
}

//插入数据
CT_VOID CCFightingData::InsertData(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if(iLen%sizeof(FightingData) != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCFightingData::InsertData iLen%sizeof(FightingData) != 0");
		return;
	}

	time_t t=time(0);
	for(CT_INT32 i=0;i<iLen/sizeof(FightingData);++i)
	{
		FightingData fd={0};
		fd.iTime=t;
		memcpy(&fd,pBuf+i*sizeof(FightingData),sizeof(FightingData));

		FD_IT it=m_FightingDataMap.find(fd.LookData.dwUserID);
		if(it == m_FightingDataMap.end())
		{
			m_FightingDataMap.insert(FightingMap::value_type(fd.LookData.dwUserID,fd));
		}
	}

#ifdef _WIN32
	//printf("InsertData m_FightingDataMap.size:%d\n",m_FightingDataMap.size());
#endif
	return;
}

//删除数据
CT_VOID CCFightingData::DeleteData()
{
	_AUTOMUTEX(&m_Mutex);
	//删除数据，检测每个模块是否引用
	time_t t=time(0);
	std::list<CT_DWORD> UserIDList;
	FD_IT itend=m_FightingDataMap.end();
	for(FD_IT it=m_FightingDataMap.begin();it != itend;++it)
	{
		if(t-(*it).second.iTime > DELETE_TIME)
		{
			CT_DWORD dwUserID=(*it).second.FData.dwUserID;
			if(!CCUserManager::getSingleton().FindPVPUser(dwUserID))
			{
				if(!CCTowerResManager::getSingleton().FindTowerUser(dwUserID))
				{
					if(CCFightTeamManager::getSingleton().GetFightTeamIDByUser(dwUserID) == 0)
					{
						if(!CCTop::getSingleton().FindTopUser(dwUserID)
							&& !CCUserManager::getSingleton().IsInSeekedList(dwUserID))
						{
							FD_IT it=m_FightingDataMap.find(dwUserID);
							if(it != m_FightingDataMap.end())
							{
								UserIDList.push_back(dwUserID);
								continue;
							}
						}
					}
				}
			}

			(*it).second.iTime=t;
		}
	}

#ifdef _WIN32
	printf("Befor DeleteData m_FightingDataMap:%d\n",m_FightingDataMap.size());
#endif
	for(std::list<CT_DWORD>::iterator it=UserIDList.begin();it != UserIDList.end();++it)
	{
		m_FightingDataMap.erase((*it));
	}

#ifdef _WIN32
	printf("DeleteData m_FightingDataMap.size:%d\n",m_FightingDataMap.size());
#endif

	//数据未删除则更新
	//UpdataFightingData(dwUserID);

	// 删除探访数据
	SeekersMap::iterator itSeek = m_SeekerMap.begin();
	SeekersMap::iterator itTmp;
	std::list<CT_DWORD> IDList;
	for (;itSeek != m_SeekerMap.end();++itSeek)
	{
		if (!CCUserManager::getSingleton().IsInSeekedList(itSeek->second.dwUserID))
		{
			IDList.push_back((*itSeek).first);
		}
		itSeek->second.iTime = t;
	}

	for(std::list<CT_DWORD>::iterator it=IDList.begin();it != IDList.end();++it)
	{
		m_SeekerMap.erase((*it));
	}

	return;
}

//更新数据
CT_VOID CCFightingData::UpdataFightingData(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		return;
	}

	MSC_SUB_UserdataInsert userData = {0};
	PTRLinkUserData data={0};
	pUser->GetPTRLinkUserData(data);
	G_GetLookUserInfo(data,userData.FData,userData.LookData);
	G_GetFightingData(pUser,userData.SeekFData,CT_TRUE);

	FD_IT it=m_FightingDataMap.find(dwUserID);
	if(it != m_FightingDataMap.end())
	{
		(*it).second.FData = userData.FData;
		(*it).second.LookData = userData.LookData;
		(*it).second.SeekFData = userData.SeekFData;
	}
	else
	{
		userData.iTime = time(0);
		m_FightingDataMap.insert(FightingMap::value_type(dwUserID, userData));
	}

	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	MSC_DB_Seeked_Info seekedInfo = {0};
	G_GetSeekerData(pUser, seekedInfo);

	if (itSeek != m_SeekerMap.end())
	{
		seekedInfo.iTime = itSeek->second.iTime;
		itSeek->second = seekedInfo;
	}
	else
	{
		seekedInfo.iTime = time(0);
		m_SeekerMap.insert(SeekersMap::value_type(dwUserID, seekedInfo));
	}

#ifdef _WIN32
	printf("UpdataFightingData m_FightingDataMap.size:%d\n",m_FightingDataMap.size());
#endif
	return;
}

//获取数据
CT_BOOL CCFightingData::GetFightingData(const CT_DWORD dwUserID,FightingData &fd)
{
	_AUTOMUTEX(&m_Mutex);
	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		PTRLinkUserData data={0};
		pUser->GetPTRLinkUserData(data);
		if(G_GetLookUserInfo(data,fd.FData,fd.LookData))
		{
			return CT_TRUE;
		}
	}
	else
	{
		FD_IT it=m_FightingDataMap.find(dwUserID);
		if(it != m_FightingDataMap.end())
		{
			memcpy(&fd,&((*it).second),sizeof(fd));
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_BOOL CCFightingData::GetFightingData(const CT_DWORD dwUserID,UserFightingStatus &fd,CT_BOOL bSeekFight/*=CT_FALSE*/)
{
	_AUTOMUTEX(&m_Mutex);
	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		G_GetFightingData(pUser,fd,bSeekFight);
	}
	else
	{
		FD_IT it=m_FightingDataMap.find(dwUserID);
		if(it != m_FightingDataMap.end())
		{
			if(bSeekFight)
				memcpy(&fd,&((*it).second.SeekFData),sizeof(fd));
			else
				memcpy(&fd,&((*it).second.FData),sizeof(fd));
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//删除boss
CT_VOID CCFightingData::DeleteBoss(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);
	FD_IT it=m_FightingDataMap.find(dwUserID);
	if(it != m_FightingDataMap.end())
	{
		for(CT_INT32 i=0;i<POST_COUNT/2;++i)
		{
			if(GET_TARGET_TYPE(UNIT_ALL_BOSS) <= GET_TARGET_TYPE((*it).second.FData.units[i].type) && (*it).second.FData.units[i].type > UNIT_ALL_ARMY)
			{
				memset(&(*it).second.FData.units[i],0,sizeof((*it).second.FData.units[i]));
			}
		}
	}
	return;
}

//竞技场连胜清空
CT_VOID CCFightingData::SetPVPConWin(CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);
	FD_IT it=m_FightingDataMap.find(dwUserID);
	if(it != m_FightingDataMap.end())
	{
		(*it).second.LookData.iContinuousWin=0;
	}
	return;
}

//获取领主信息
CT_BOOL CCFightingData::GetLookUserInfo(PTRLinkUserData &linkUserData,UserFightingStatus &Fightinginfo,LookUserInfo &Lookinfo)
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
	CCUser::GetFightingStatusData(Fightinginfo,linkUserData);
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

	return CT_TRUE;
}

//插入探访数居
CT_VOID CCFightingData::InsertSeekedInfo(MSC_DB_Seeked_Info* pInfo)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator it = m_SeekerMap.find(pInfo->dwUserID);

	if (it == m_SeekerMap.end())
	{
		CT_INT32 iTime = time(0);
		pInfo->iTime = iTime;
		m_SeekerMap.insert(SeekersMap::value_type(pInfo->dwUserID, *pInfo));
	}
	return;
}


//得到探访者数据
CT_BOOL CCFightingData::GetSeekedInfo(CT_DWORD dwUserID,Seekers& seekersInfo)
{
	_AUTOMUTEX(&m_Mutex);

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		MasterVariableData mvd={0};
		SeekData sd={0};
		pUser->m_userBase.GetMasterData(mvd);
		pUser->m_userSeek.GetSeekData(sd);
		pUser->m_userBuilding.GetProductingData(seekersInfo.productBuilding,sizeof(seekersInfo.productBuilding));

		seekersInfo.dwUserID=dwUserID;
		seekersInfo.iFood=mvd.stRes.iFood;
		seekersInfo.iMoney=mvd.stRes.iGold;
		seekersInfo.iKidnapSaintess=sd.iWomanNum;
		seekersInfo.iKidnapWorkerNum=sd.iFarmarNum;
		memcpy(seekersInfo.iPetKidnap, sd.iPet, sizeof(seekersInfo.iPetKidnap));
		return CT_TRUE;
	}
	else
	{
		SeekersMap::iterator it = m_SeekerMap.find(dwUserID);
		if (it != m_SeekerMap.end())
		{
			seekersInfo = it->second;
			return CT_TRUE;
		}
	}

	return CT_FALSE;
}

//更新探访者数据
CT_BOOL CCFightingData::UpdateSeekedTimes(CT_INT32 iType, const Seekers& seekersInfo)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator it = m_SeekerMap.find(seekersInfo.dwUserID);

	if (it != m_SeekerMap.end())
	{
		it->second = seekersInfo;

		// 更新到数据库
		MSC_DB_Seeked_TimesMinus seeked = {0};
		seeked.dwFUserID = seekersInfo.dwUserID;
		seeked.iType = iType;
		CT_DWORD threadID = GAME_THREAD_DATABASE + seekersInfo.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_SEEK,MSC_SUB_SEEKED_TIMES_MINUS,(CT_CHAR*)&seeked,sizeof(seeked));

		return CT_TRUE;
	}

	return CT_FALSE;
}

//验证建筑ID
CT_BOOL CCFightingData::GetBuildData(const CT_DWORD dwUserID,const CT_INT32 iID,BuildData &bd)
{
	_AUTOMUTEX(&m_Mutex);
	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		pUser->m_userSeek.GetBuildData(iID,bd);
		return CT_TRUE;
	}
	else
	{
		SeekersMap::iterator it=m_SeekerMap.find(dwUserID);
		if(m_SeekerMap.end() != it)
		{
			for(CT_INT32 i=0;i<SEEK_BUILD_NUM;++i)
			{
				if(iID == (*it).second.buildingData[i].bd.iID)
				{
					memcpy(&bd,&((*it).second.buildingData[i]),sizeof(bd));
					return CT_TRUE;
				}
			}
		}
	}
	return CT_FALSE;
}

//得到已获得的礼物个数
CT_INT32 CCFightingData::GetPresentNum(CT_DWORD dwUserID)
{
	CT_INT32 iNum = 0;
	CT_INT32 iNow = time(0);

	G_ValueRange vr = {0};
	G_GetValueRange(VALUE_TYPE_UPDATEPRESENT_TIME, &vr);

	SeekersMap::iterator it = m_SeekerMap.find(dwUserID);
	if (it == m_SeekerMap.end())
	{
		return -1;
	}

	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM && it->second.present[i].iChestID != 0;++i)
	{
		if (iNow - it->second.present->iGetTime < vr.fMaxVal)
		{
			++iNum;
		}
		else
		{
			for (CT_INT32 j = i;j < SEEKED_PRESENT_NUM - 1;++j)
			{
				it->second.present[j] = it->second.present[j + 1];
			}
		}
	}

	return iNum;
}

//得到礼物
CT_VOID CCFightingData::GetPresent(CT_DWORD dwUserID, CT_INT32 iChestID)
{
	SeekersMap::iterator it = m_SeekerMap.find(dwUserID);
	if (it == m_SeekerMap.end())
	{
		return ;
	}

	CT_INT32 iTime = time(0);

	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		if (it->second.present[i].iChestID == 0)
		{
			it->second.present[i].iChestID = iChestID;
			it->second.present[i].iGetTime = iTime;

			// 更新到数据库
			MSC_DB_SeekPresent present = {0};
			present.dwUserID = dwUserID;
			present.present = it->second.present[i];
			CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_UPDATE, (CT_CHAR*) &present, sizeof(present));
			return;
		}
	}
}

//得到正在生产的建筑信息
CT_BOOL CCFightingData::GetBuildingProduct(CT_DWORD dwUserID, CT_INT32 iBID, ProductingBuilding& proBuilding)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	if (itSeek == m_SeekerMap.end())
	{
		return CT_FALSE;
	}

	for (CT_INT32 i = 0;i < SEEK_BUILD_NUM;++i)
	{
		if (itSeek->second.productBuilding[i].iBid == iBID)
		{
			proBuilding = itSeek->second.productBuilding[i];
			return CT_TRUE;
		}
	}

	return CT_FALSE;
}

//更新正在生产的建筑信息
CT_VOID CCFightingData::UpdateBuildingProduct(CT_DWORD dwUserID, CT_INT32 iBID, ProductingBuilding& proBuilding)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	if (itSeek == m_SeekerMap.end())
	{
		return;
	}

	for (CT_INT32 i = 0;i < SEEK_BUILD_NUM;++i)
	{
		if (itSeek->second.productBuilding[i].iBid == iBID)
		{
			itSeek->second.productBuilding[i] = proBuilding;

			// 更新到数据库
			MSC_DB_SaveProductingData spd = {0};
			spd.dwUserID = dwUserID;
			spd.productingBuilding = itSeek->second.productBuilding[i];
			CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,MSC_SUB_DB_PRODUCTING_DATA , (CT_CHAR*)&spd, sizeof(spd));
		}
	}
}

//得到正在升级建筑
CT_BOOL CCFightingData::GetUpgradeBuilding(CT_DWORD dwUserID, CT_INT32 iBID, UpgradingBuildData& upgBuilding)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	if (itSeek == m_SeekerMap.end())
	{
		return CT_FALSE;
	}

	for (CT_INT32 i = 0;i < SEEK_BUILD_NUM;++i)
	{
		if (itSeek->second.upgradeBuilding[i].iID == iBID)
		{
			upgBuilding = itSeek->second.upgradeBuilding[i];
			return CT_TRUE;
		}
	}

	return CT_FALSE;
}

//更新正在升级建筑
CT_VOID CCFightingData::UpdateUpgradeBuilding(CT_DWORD dwUserID, CT_INT32 iBID, UpgradingBuildData& upgBuilding)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	if (itSeek == m_SeekerMap.end())
	{
		return ;
	}

	for (CT_INT32 i = 0;i < SEEK_BUILD_NUM;++i)
	{
		if (itSeek->second.upgradeBuilding[i].iID == iBID)
		{
			itSeek->second.upgradeBuilding[i] = upgBuilding;

			// 更新到数据库
			MSC_DB_BuildingUpgradingInfo bui = {0};
			bui.buildingUpgrade = upgBuilding.buildingUpgrade;
			bui.dwUserID        = dwUserID;
			bui.iID             = upgBuilding.iID;
			bui.iNeedTime       = upgBuilding.iNeedTime;
			bui.llBeginTime     = upgBuilding.llBeginTime;
			CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_BUILDING, MSC_SUB_DB_SAVE_UPGRADING_BUILDING, (CT_CHAR*) &bui, sizeof(bui));
		}
	}
}

//更新探访人物信息到数据库(不包括建筑)
CT_VOID CCFightingData::UpdateSeekersBaseInfo(const CT_DWORD dwUserID,Seekers &seekers)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	if (itSeek == m_SeekerMap.end())
	{
		return ;
	}

	MSC_DB_MasterSaveData msd={0};
	msd.dwUserID=dwUserID;
	msd.iSaveTag=SAVATAG_SEEK_PLUNDER;
	msd.mvd.stRes.iFood=seekers.iFood-itSeek->second.iFood;
	msd.mvd.stRes.iGold=seekers.iMoney-itSeek->second.iMoney;
	itSeek->second = seekers;

	// 更新到数据库
	MSC_DB_Seeked_Info sInfo = {0};
	sInfo = seekers;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEKED_DATA_UPDATE, (CT_CHAR*) &sInfo, sizeof(sInfo));

	
	CT_INT32 logThreadid = GAME_THREAD_LOGDATABASE + msd.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_MASTER_DATA, (CT_CHAR*)&msd, sizeof(msd));

}

//更新战旗信息
CT_VOID CCFightingData::InsertTeamFlag(CT_DWORD dwUserID, TeamSeek& ts)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	if (itSeek == m_SeekerMap.end())
	{
		return ;
	}

	for (CT_INT32 i = 0;i < SEEK_TEAM_FLAG_NUM;++i)
	{
		if (itSeek->second.teamFlag[i].dwUserID == 0)
		{
			itSeek->second.teamFlag[i] = ts;
			// 更新到数据库
			MSC_DB_SeekTeamFlag tFlag = {0};
			tFlag.dwUserID = dwUserID;
			tFlag.teamFlag = ts;
			CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_TEAM_FLAG_UPDATE, (CT_CHAR*) &tFlag, sizeof(tFlag));
			break;
		}
	}
}

//更新礼物信息
CT_VOID CCFightingData::UpdateSeekPresent(CT_DWORD dwUserID, SeekPresent& sp)
{
	_AUTOMUTEX(&m_Mutex);
	SeekersMap::iterator itSeek = m_SeekerMap.find(dwUserID);
	if (itSeek == m_SeekerMap.end())
	{
		return ;
	}

	for (CT_INT32 i = 0;i < SEEKED_PRESENT_NUM;++i)
	{
		if (itSeek->second.present[i].iChestID == 0)
		{
			itSeek->second.present[i] = sp;
			// 更新到数据库
			MSC_DB_SeekPresent present = {0};
			present.dwUserID = dwUserID;
			present.present = sp;
			CT_INT32 threadID =  GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadID,  MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_PRESENT_UPDATE, (CT_CHAR*) &present, sizeof(present));

		}
	}
}