#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CUserManager.h"
#include "CArmyEvent.h"

CCArmyEvent::CCArmyEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_ARMY_UP_GRADE);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_ARMY_ADVANCED);
}

CCArmyEvent::~CCArmyEvent(void)
{
}

//处理用户消息
CT_VOID CCArmyEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_MASTER_EVENT_CITY:
		{
			DisposeNetMessage(subID,pBuf,len);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCArmyEvent::OnTime(CT_DWORD timeID)
{
	return;
}

//网络消息
CT_VOID CCArmyEvent::DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case SUB_G_ARMY_UP_GRADE:
		{
			DisposeArmyGrade(pBuf,len);				//军队升级
			break;
		}
	case SUB_G_ARMY_ADVANCED:
		{
			DisposeArmyAdvanced(pBuf,len);			//军队进阶
			break;
		}
	}
	return;
}

//军队升级消息
CT_VOID CCArmyEvent::DisposeArmyGrade(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_ArmyUpGrade))
	{
		CCLogMeg::getSingleton().WriteError("DisposeArmyGrade : len != sizeof(CMD_G_ArmyUpGrade)");
		return;
	}
	CMD_G_ArmyUpGrade *pArmyUpGrade=(CMD_G_ArmyUpGrade*)pBuf;

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(pArmyUpGrade->dwUserID,&pUser))
	{
		//建筑验证
		if(!pUser->m_userBuilding.BuildValidate(CITY_TYPE_SOLDIER))
		{
			CCLogMeg::getSingleton().WriteError("CITY_TYPE_SOLDIER 建筑未开启");
			pUser->CloseNet();
			return;
		}

		//升级验证
		if(!ArmyGradeValidate(pUser,pArmyUpGrade->iArmyTypeID))
		{
			return;
		}

		//更新数据
		if(!pUser->m_userArmy.UpDataArmyData(pArmyUpGrade->iArmyTypeID))
		{
			CMD_G_ArmyReturnInfo ReturnInfo={0};
			ReturnInfo.dwUserID=pUser->GetUserID();
			ReturnInfo.iArmyInfo=ARMY_ERROR_DATA;
			pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
			return;
		}

		MasterData MasterInfo={0};
		pUser->m_userBase.GetMasterData(MasterInfo);

		//更新客户端
		CMD_G_UpdataArmyInfo UpDataArmy={0};
		UpDataArmy.dwUserID=pArmyUpGrade->dwUserID;
		UpDataArmy.iOldArmyID=pArmyUpGrade->iArmyTypeID;
		UpDataArmy.iFood=MasterInfo.mvd.stRes.iFood;
		UpDataArmy.iGold=MasterInfo.mvd.stRes.iGold;
		pUser->m_userArmy.GetArmyInfo(pArmyUpGrade->iArmyTypeID,UpDataArmy.Armyinfo);

		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_UPDATA_ARMY_INFO,(CT_CHAR*)&UpDataArmy,sizeof(UpDataArmy));

	}
	return;
}

//军队进阶消息
CT_VOID CCArmyEvent::DisposeArmyAdvanced(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_ArmyAdvanced))
	{
		CCLogMeg::getSingleton().WriteError("DisposeArmyGrade : len != sizeof(CMD_G_ArmyAdvanced)");
		return;
	}
	CMD_G_ArmyAdvanced *pArmyAdvanced=(CMD_G_ArmyAdvanced*)pBuf;

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(pArmyAdvanced->dwUserID,&pUser))
	{
		//建筑验证
		if(!pUser->m_userBuilding.BuildValidate(CITY_TYPE_SOLDIER))
		{
			CCLogMeg::getSingleton().WriteError("CITY_TYPE_SOLDIER 建筑未开启");
			pUser->CloseNet();
			return;
		}

		//进阶验证
		if(!ArmyAdvancedValidate(pUser,pArmyAdvanced->iArmyTypeID))
		{
			return;
		}

		//更新数据
		G_Army Army={0};
		if(!G_GetArmy(pArmyAdvanced->iArmyTypeID,&Army))
		{
			CMD_G_ArmyReturnInfo ReturnInfo={0};
			ReturnInfo.dwUserID=pUser->GetUserID();
			ReturnInfo.iArmyInfo=ARMY_ERROR_DATA;
			pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));


			CCLogMeg::getSingleton().WriteError("CCArmyEvent::DisposeArmyAdvanced G_GetArmy");
			return;
		}
		pUser->m_userArmy.UpDataArmyAdvanced(pArmyAdvanced->iArmyTypeID,Army.iAdcancedSoldier);

		MasterData MasterInfo={0};
		pUser->m_userBase.GetMasterData(MasterInfo);

		//更新客户端
		CMD_G_UpdataArmyInfo UpDataArmy={0};
		UpDataArmy.dwUserID=pArmyAdvanced->dwUserID;
		UpDataArmy.iOldArmyID=pArmyAdvanced->iArmyTypeID;
		pArmyAdvanced->iArmyTypeID=Army.iAdcancedSoldier;
		UpDataArmy.iFood=MasterInfo.mvd.stRes.iFood;
		UpDataArmy.iGold=MasterInfo.mvd.stRes.iGold;
		pUser->m_userArmy.GetArmyInfo(pArmyAdvanced->iArmyTypeID,UpDataArmy.Armyinfo);

		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_UPDATA_ARMY_INFO,(CT_CHAR*)&UpDataArmy,sizeof(UpDataArmy));

		//更新阵位
		CMD_G_BattleArrayInfo BattleArray={0}; 
		BattleArray.dwUserID=pUser->GetUserID();
		pUser->m_userBattleArray.GetBattleArray(BattleArray.iPosition);

		for(CT_INT32 i=0;i<POST_COUNT/2;++i)
		{
			if(BattleArray.iPosition[i] == UpDataArmy.iOldArmyID)
			{
				BattleArray.iPosition[i]=UpDataArmy.Armyinfo.iArmyID;
				break;
			}
		}

		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadid,MSC_MAIN_BATTLE_ARRAY,MSC_SUB_UPDATA_BATTLE_ARRAY,(CT_CHAR*)&BattleArray,sizeof(BattleArray));


	}
	return;
}

//军队阶级
CT_BOOL	CCArmyEvent::ArmyAdvancedValidate(CCUser *pUser,CT_INT32 iArmyID)
{
	//军队阶级验证
	ArmyInfo Armyinfo={0};
	MasterVariableData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(!pUser->m_userArmy.GetArmyInfo(iArmyID,Armyinfo))
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_DATA;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
		return CT_FALSE;
	}

	if(Armyinfo.iArmyID > iArmyID)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_BUILD;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
		return CT_FALSE;
	}

	G_Army Army={0};
	if(!G_GetArmy(iArmyID,&Army))
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_DATA;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		CCLogMeg::getSingleton().WriteError("CCArmyEvent::ArmyAdvancedValidate G_GetArmy ArmyID: %d",iArmyID);
		return CT_FALSE;
	}

	//兵营验证
	CMD_G_BuildingData BuildData={0};
	if(!pUser->m_userBuilding.GetBuildingData(CITY_TYPE_SOLDIER,BuildData))
	{
		CCLogMeg::getSingleton().WriteError("CCArmyEvent::ArmyAdvancedValidate GetBuildingData");
		return CT_FALSE;
	}

	if(BuildData.iGrade < Army.iArmyCampGrade)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_ARMY;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
		return CT_FALSE;
	}


	//军队等级
	if(Armyinfo.iArmyGrade < Army.iAdvancedGrade)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_ARMY;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
		return CT_FALSE;
	}

	//金币验证
	if(MasterInfo.stRes.iGold < -Army.iGold)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_GOLD;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		return CT_FALSE;
	}

	//道具验证
	CT_INT32 num =pUser->m_userItem.GetItemCount(Army.iItemID[0]);
	if(num < -Army.iItemNum[0])
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_GOODS;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		return CT_FALSE;
	}

	//消耗资源
	pUser->m_userItem.AutoExpendItem(SAVETAG_ARMY_CONSUME,Army.iItemID[0],-Army.iItemNum[0]);

	//道具验证
	num =pUser->m_userItem.GetItemCount(Army.iItemID[1]);
	if(num < -Army.iItemNum[1])
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_GOODS;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		return CT_FALSE;
	}

	//消耗资源
	pUser->m_userItem.AutoExpendItem(SAVETAG_ARMY_CONSUME,Army.iItemID[1],-Army.iItemNum[1]);

	MasterVariableData MasterReward={0};
	MasterReward.stRes.iGold+=Army.iGold;

	// 更新领主资源
	pUser->m_userBase.AddMastData(MasterReward, SAVETAG_ARMY_CONSUME);

	//数据记录
	MSC_LOGDB_ArmyAdvanced LOGDBArmyAdvanced={0};
	LOGDBArmyAdvanced.dwUserID=pUser->GetUserID();
	LOGDBArmyAdvanced.iOldArmyID=iArmyID;
	LOGDBArmyAdvanced.iNewArmyID=Army.iAdcancedSoldier;
	LOGDBArmyAdvanced.iGold=Army.iGold;
	memcpy(LOGDBArmyAdvanced.iGoods,Army.iItemID,sizeof(Army.iItemID));
	memcpy(LOGDBArmyAdvanced.iCount,Army.iItemNum,sizeof(Army.iItemNum));
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ARMYUPDATARANK,(CT_CHAR*)&LOGDBArmyAdvanced,sizeof(LOGDBArmyAdvanced));

	return CT_TRUE;
}
//军队等级
CT_BOOL	CCArmyEvent::ArmyGradeValidate(CCUser *pUser,CT_INT32 iArmyID)
{
	//军营验证
	if(!pUser->m_userBuilding.BuildValidate(CITY_TYPE_SOLDIER))
	{
		CCLogMeg::getSingleton().WriteError("CITY_TYPE_SOLDIER 未开启 %d",pUser->GetUserID());
		pUser->CloseNet();
		return CT_FALSE;
	}

	//军队阶级验证
	ArmyInfo Armyinfo={0};
	MasterVariableData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(!pUser->m_userArmy.GetArmyInfo(iArmyID,Armyinfo))
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_DATA;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
		return CT_FALSE;
	}

	if(Armyinfo.iArmyID > iArmyID)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_ARMY;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
		return CT_FALSE;
	}


	//军营等级
	CMD_G_BuildingData BuildInof={0};
	if(!pUser->m_userBuilding.GetBuildingData(CITY_TYPE_SOLDIER,BuildInof))
	{
		CCLogMeg::getSingleton().WriteError("CITY_TYPE_SOLDIER 获取信息失败 %d",pUser->GetUserID());
		return CT_FALSE;
	}

	if(BuildInof.iGrade <= Armyinfo.iArmyGrade)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_BUILD;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));
		return CT_FALSE;

	}

	//金币验证
	G_ArmyGrade ArmyGrade={0};
	if(!G_GetArmyGrade(iArmyID,Armyinfo.iArmyGrade+1,&ArmyGrade))
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_DATA;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		CCLogMeg::getSingleton().WriteError("CCArmyEvent::ArmyGradeValidate G_GetArmyGrade ArmyID:%d ArmyGrade:%d",iArmyID,Armyinfo.iArmyGrade);
		return CT_FALSE;
	}

	if(MasterInfo.stRes.iGold < -ArmyGrade.iGold)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_GOLD;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		return CT_FALSE;
	}

	if(MasterInfo.stRes.iFood < -ArmyGrade.iFood)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_GOLD;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		return CT_FALSE;
	}

	//道具验证
	CT_INT32 num =pUser->m_userItem.GetItemCount(ArmyGrade.iGoods);
	if(num < -ArmyGrade.iCount)
	{
		CMD_G_ArmyReturnInfo ReturnInfo={0};
		ReturnInfo.dwUserID=pUser->GetUserID();
		ReturnInfo.iArmyInfo=ARMY_ERROR_GOODS;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ARMY_RETURN_INFO,(CT_CHAR*)&ReturnInfo,sizeof(ReturnInfo));

		return CT_FALSE;
	}

	//消耗资源
	pUser->m_userItem.AutoExpendItem(SAVETAG_ARMY_CONSUME,ArmyGrade.iGoods,-ArmyGrade.iCount);

	MasterVariableData MasterReward={0};
	MasterReward.stRes.iGold+=ArmyGrade.iGold;
	MasterReward.stRes.iFood+=ArmyGrade.iFood;

	// 更新领主资源
	pUser->m_userBase.AddMastData(MasterReward, SAVETAG_ARMY_CONSUME);
	//G_SaveMastVariableData(pUser, SAVETAG_ARMY_CONSUME);
	//pUser->UpdateAndSendMasterData();

	//数据记录
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData( mvd);

	MSC_LOGDB_W_ArmyGrade LOGDBArmyGrade={0};
	LOGDBArmyGrade.dwUserID=pUser->GetUserID();
	LOGDBArmyGrade.iUserGarde = mvd.stGrade.iLevel;
	LOGDBArmyGrade.iGrade=Armyinfo.iArmyGrade;
	LOGDBArmyGrade.iArmyID=iArmyID;
	LOGDBArmyGrade.iGold=ArmyGrade.iGold;
	LOGDBArmyGrade.iFood=ArmyGrade.iFood;
	LOGDBArmyGrade.iGoods=ArmyGrade.iGoods;
	LOGDBArmyGrade.iCount=ArmyGrade.iCount;

	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ARMYUPDATAGRADE,(CT_CHAR*)&LOGDBArmyGrade,sizeof(LOGDBArmyGrade));



	return CT_TRUE;
}
