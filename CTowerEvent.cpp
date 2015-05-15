#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CUserManager.h"
#include "CTowerEvent.h"
#include "CTowerResManager.h"
#include "CUser.h"
#include "CUserManager.h"
#include "CFightTeam.h"
#include "CFightTeamManager.h"
#include "CFightingData.h"
#include "CBroadcastScriptSetting.h"

CCTowerEvent::CCTowerEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_SCENCE_APPLY);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_APPLY);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FLOOR_GIVEUP_APPLY);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_EXIT_APPLY);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_HELP_APPLY);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_LOOK_FIGHTING);

	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_USE_TREASURE);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_MSG_READ);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_LOOK_FIGHT);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_SLOTAWARD_GET);
	RegistMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_TREASURE_ABANDON);

	RegistMessage(MSC_MAIN_TOWER_EVENT, MSC_SUB_DB_TOWER_OFFLINE_DATA);
	RegistMessage(MSC_MAIN_TOWER_EVENT, MSC_SUB_TOWER_RES_PVP);
	RegistMessage(MSC_MAIN_TOWER_EVENT, MSC_SUB_TOWER_CP_PASS);



	if (id == GAME_THREAD_CITY)
	{
		RegistTimer(IDI_TOWER_RES_PRODUCT_CHECK);
		SetTimer(IDI_TOWER_RES_PRODUCT_CHECK, TIME_TOWER_RES_PRODUCT_CHECK);
	}
}

CCTowerEvent::~CCTowerEvent(void)
{

}


//处理用户消息
CT_VOID CCTowerEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_ELEMENT_TOWER:
		{
			DisposeElementTower(subID,pBuf,len);
			break;
		}
	case MSC_MAIN_TOWER_EVENT:
		{
			if (subID == MSC_SUB_TOWER_CP_PASS)
			{
				DisposeFightingEnd(pBuf,len);
			}

	       if(subID == MSC_SUB_DB_TOWER_OFFLINE_DATA)
			{
				DisposeTowerOfflineData(pBuf, len);

			}
		   if(subID ==  MSC_SUB_TOWER_RES_PVP)
		    {
				DisposTowerPVPFromFight(pBuf, len);
		    }
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCTowerEvent::OnUserEvent mainid = %i, subid = %i", mainID, subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCTowerEvent::OnTime(CT_DWORD timeID)
{
	if (IDI_TOWER_RES_PRODUCT_CHECK == timeID)
	{
		CCTowerResManager::getSingleton().UpdateSlotProduct();
	}
}

//用户时钟处理
CT_VOID CCTowerEvent::OnClock(CT_DWORD timeID)
{

}

//处理元素之塔的网络消息
CT_VOID CCTowerEvent::DisposeElementTower(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch (subID)
	{
	case  SUB_G_ELEMENT_TOWER_SCENCE_APPLY:
		{
			DisposeElementTowerInfoApply(pBuf,len);
			break;
		}
	case SUB_G_ELEMENT_TOWER_OCCUPY_APPLY:
		{ 
			DisposeElementTowerOccupy(pBuf,len);
			break;
		}
	case SUB_G_ELEMENT_TOWER_FLOOR_GIVEUP_APPLY:
		{
			DisposeElementTowerGiveupApply(pBuf, len);
			break;
		}
	case SUB_G_ELEMENT_TOWER_EXIT_APPLY:
		{
			DisposeTowerUserExit(pBuf, len);
			break;
		}
	case SUB_G_ELEMENT_TOWER_HELP_APPLY:
		{
			DisposeTowerFriendHelpApply(pBuf, len);
			break;
		}
	case SUB_G_ELEMENT_TOWER_LOOK_FIGHTING:
		{
			DisposeTowerLookFighting(pBuf, len);
			break;
		}
	case SUB_G_TOWER_PASS_APPLY:
		{
			DisposeTowerPassApply(pBuf,len);
			break;
		}
	case SUB_G_TOWER_RAIDS:
		{
			DisposeTowerRaids(pBuf,len);
			break;
		}
	case SUB_G_TOWER_USE_TREASURE:
		{
			DisposeTowerUseTreasure(pBuf,len);
			break;
		}
	case SUB_G_TOWER_MSG_READ:
		{
			DisposeTowerMSGRead(pBuf,len);
			break;
		}
	case SUB_G_TOWER_LOOK_FIGHT:
		{
			DisposeTowerLookFight(pBuf,len);
			break;
		}
	case SUB_G_TOWER_SLOTAWARD_GET:
		{
			DisposeTowerSlotAwardGet(pBuf,len);
			break;
		}
	case SUB_G_TOWER_TREASURE_ABANDON:
		{
			DisposeTowerTreasureAbandon(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTower subid = %i", subID);
			break;
		}
	}
}

//处理塔信息请求
CT_VOID CCTowerEvent::DisposeElementTowerInfoApply(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_ElementTowerInfoApply))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerInfoApply len != sizeof(CMD_G_ElementTowerInfoApply)");
		return ;
	}

	CMD_G_ElementTowerInfoApply* pApply = (CMD_G_ElementTowerInfoApply*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerInfoApply 没有发现uid=%i", pApply->dwUserID);
		return;
	}


	CT_CHAR  buf[MAX_MES_MAX_BUFF_LEN] = {0};
	CT_INT32 bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCTowerResManager::getSingleton().GetTowerResData(pApply->iTowerID, pApply->iFloorLow, pApply->iFloorHigh,  buf, bufLen))
	{
		CCTowerResManager::getSingleton().InsertUserEnterMap(pApply->iTowerID, pApply->dwUserID);
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_SCENCE_INFO, buf, bufLen);
		
	}
}

//处理元素之塔的网络消息
CT_VOID CCTowerEvent::DisposeElementTowerOccupy(CT_CHAR* pBuf,CT_DWORD len)
{
	if (sizeof(CMD_G_ElementFloorOccupyApply) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerOccupy len != sizeof(CMD_G_ElementFloorOccupyApply)");
		return;
	}

	CMD_G_ElementFloorOccupyApply* pOccupyApply = (CMD_G_ElementFloorOccupyApply*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pOccupyApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerOccupy 没有发现用户 %i", pOccupyApply->dwUserID);
		return;
	}
	//判断占领次数是否用完

	if (pOccupyApply->iSlotID < 0 || pOccupyApply->iSlotID >= TOWER_SLOT_NUM)
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerOccupy error islot= %i", pOccupyApply->iSlotID);
		return;
	}

	CMD_G_ElementFloorOccupyFail floorOccpuyFail = {0};
	floorOccpuyFail.dwUserID = pOccupyApply->dwUserID;
	floorOccpuyFail.iTower   = pOccupyApply->iTower;
	floorOccpuyFail.iFoor    = pOccupyApply->iFoor;

	G_TowerCheckPoint towerCheckPoint = {0};
	if(!G_GetTowerCheckPoint(pOccupyApply->iFoor, &towerCheckPoint))
	{   
		floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_NO_FOOR_FAIL;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
		return;
	}

	if (towerCheckPoint.iAreaID != pOccupyApply->iTower)
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerOccupy  towerCheckPoint.iAreaID（%i） != pOccupyApply->iTower (%i)", towerCheckPoint.iAreaID , pOccupyApply->iTower);
		return;
	}

	G_TowerArea towerArea ={0};
	if(!G_GetTowerArea(pOccupyApply->iTower, &towerArea ))
	{
		floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_NO_TOWER_FAIL;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
		return;
	}

	/*
	if (!pUser->m_userTower.CheckPointPass(pOccupyApply->iFoor))
	{
		floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_SLOT_FAIL;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
		return;
	}
	*/
	
	 CT_INT32 iTowerID = 0, iFloor = 0,  iSlot = 0;
	 if( CCTowerResManager::getSingleton().FindUserInTower(pOccupyApply->dwUserID ,  iTowerID,  iFloor,  iSlot))
	 {
		 floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_ALEADY_FAIL;
		 pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
		 return;
	 }

	 //查看目前资源点的状况
	 CCTowerResManager::ElementTowerSlotInfo slotInfo = {0};
	 if(!CCTowerResManager::getSingleton().GetSlotInTower(pOccupyApply->iTower, pOccupyApply->iFoor, pOccupyApply->iSlotID, slotInfo))
	 {
		 floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_SLOT_FAIL;
		 pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
		 return;
	 }

     //用户资源是否足够
	 G_TowerProductReward towerProductReward = {0};
	 if(!G_GetTowerProductReward(towerCheckPoint.iProductID, pOccupyApply->iSlotID + 1, &towerProductReward) )
	 {
		 CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerOccupy productID=%i, slotID=%i", towerCheckPoint.iProductID, pOccupyApply->iSlotID);
		 return;
	 }

	 CMD_G_NormalReward  addNormalCost = {0};
	 FakeProBase fb={0};
	 CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	 CT_INT32 iCounts[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	 if(G_PerformCostReward(towerProductReward.iCostGroupID, pUser,ENUM_PCRT_TEST_PERFORM,  addNormalCost, iGoods, iCounts,fb) != ENUM_CREC_RES_SUC) //测试资源是否满足
	 {
		 floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_NO_RES;
		 pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
		 return;
	 }

	
	 if(slotInfo.userInfo.dwUser != 0)
	 {
		 G_ValueRange vrlt={0};
		 if(G_GetValueRange(VALUE_TYPE_TOWER_PROTECT_TIME,&vrlt))
		 {
			 if (time(0) - slotInfo.llBeginTime < (CT_LONGLONG)vrlt.fMaxVal)	//强攻
			 {
				 if(G_GetValueRange(VALUE_TYPE_TOWER_FOCUSOCCUPY_COST,&vrlt))
				 {
					 CT_INT32 iDiamod = 0;
					 switch (pOccupyApply->iResType)
					 {
					 case VALUE_TYPE_DIAMOND:
						 iDiamod = (CT_INT32)vrlt.fMaxVal;
						 break;
					 default:
						 floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_RESTYPE_ERR;
						 pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
						 return;
					 }

					 MasterVariableData mvData = {0};
					 pUser->m_userBase.GetMasterData(mvData);
					 if (mvData.stRes.iDiamod + iDiamod < 0)
					 {
						 floorOccpuyFail.iErrorCode = EEFOF_OCCUPY_NO_RES;
						 pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE, (CT_CHAR*)&floorOccpuyFail, sizeof(floorOccpuyFail));
						 return;
					 }
					 memset(&mvData,0,sizeof(mvData));
					 mvData.stRes.iDiamod = iDiamod;
					 pUser->m_userBase.AddMastData(mvData, SAVETAG_TOWER_OCCUPY_COST);
					 memset(&mvData,0,sizeof(mvData));
					 pUser->m_userBase.GetMasterData(mvData);
					 pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FOCUSOCCUPY_COST, (CT_CHAR*)&mvData.stRes.iDiamod, sizeof(CT_INT32));
				 }
			 }
		 }
	 }

	 MasterVariableData mvd ={0};
	 G_ChangeResType(mvd,addNormalCost.iResType0,addNormalCost.iResNum0);
	 G_ChangeResType(mvd,addNormalCost.iResType1,addNormalCost.iResNum1);
	 G_ChangeResType(mvd,addNormalCost.iResType2,addNormalCost.iResNum2);
	 G_ChangeResType(mvd,addNormalCost.iResType3,addNormalCost.iResNum3);
	 pUser->m_userBase.AddMastData(mvd, SAVETAG_TOWER_OCCUPY_COST);

	 CMD_G_NormalReward normalCost = {0};
	 MasterVariableData mvd2 ={0};
	 pUser->m_userBase.GetMasterData(mvd2);
	 normalCost.iResType0 = addNormalCost.iResType0;
	 normalCost.iResType1 = addNormalCost.iResType1;
	 normalCost.iResType2 = addNormalCost.iResType2;
	 normalCost.iResType3 = addNormalCost.iResType3;
	 G_ChangeResType(mvd2,addNormalCost.iResType0,normalCost.iResNum0);
	 G_ChangeResType(mvd2,addNormalCost.iResType1,normalCost.iResNum1);
	 G_ChangeResType(mvd2,addNormalCost.iResType2,normalCost.iResNum2);
	 G_ChangeResType(mvd2,addNormalCost.iResType3,normalCost.iResNum3);

	 MSC_LOGDB_TowerSlot logdbTowerSlot = {0};
	 logdbTowerSlot.dwUserID = pOccupyApply->dwUserID;
	 logdbTowerSlot.iCPID = pOccupyApply->iFoor;
	 logdbTowerSlot.iSlotID = pOccupyApply->iSlotID;
	 logdbTowerSlot.iResType[0] = addNormalCost.iResType0;
	 logdbTowerSlot.iResType[1] = addNormalCost.iResType1;
	 logdbTowerSlot.iResType[2] = addNormalCost.iResType2;
	 logdbTowerSlot.iResType[3] = addNormalCost.iResType3;
	 logdbTowerSlot.iResNum[0] = addNormalCost.iResNum0;
	 logdbTowerSlot.iResNum[1] = addNormalCost.iResNum1;
	 logdbTowerSlot.iResNum[2] = addNormalCost.iResNum2;
	 logdbTowerSlot.iResNum[3] = addNormalCost.iResNum3;
	 logdbTowerSlot.iUGrade = mvd2.stGrade.iLevel;
	 CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbTowerSlot.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	 G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TOWER_OCCUPY,(CT_CHAR*)&logdbTowerSlot,sizeof(MSC_LOGDB_TowerSlot));

	if(slotInfo.userInfo.dwUser == 0) //无人占领，直接处理
	{
		ElemTowerUserInfo userInfo = {0};
		MasterData md = {0};
		pUser->m_userBase.GetMasterData(md);
		userInfo.dwUser = pOccupyApply->dwUserID;
		memcpy(userInfo.cNickName, md.cNickName, NAME_SIZE);
		userInfo.iGrade = md.mvd.stGrade.iLevel;
		userInfo.iFightPower = md.mvd.stRes.iFightingPower;
		userInfo.iOccupation = md.iOccupation;

		DisposeOccupySuc(userInfo, pOccupyApply->iTower, pOccupyApply->iFoor, pOccupyApply->iSlotID, normalCost, towerProductReward.iOccupyTime);
		TowerEvent towerEvent = {0};
		towerEvent.dwUserID = pOccupyApply->dwUserID;
		towerEvent.iEventType = 5;		//直接占领
		towerEvent.iFoor = pOccupyApply->iFoor;
		towerEvent.iFightingID = time(0);
		pUser->m_userTower.InsertEvent(towerEvent);
		return;
	}else
	{
		MSC_TowerPVP_ResRobbery resRobbery = {0};
		resRobbery.dwUserID		=   pOccupyApply->dwUserID; // slotInfo.userInfo.dwUser;
		resRobbery.iSeed		= rand();
		resRobbery.iTowerID     = pOccupyApply->iTower;
		resRobbery.iFloorID     = pOccupyApply->iFoor;
		resRobbery.iSlotID      = pOccupyApply->iSlotID;
		resRobbery.iOccupyTime  = towerProductReward.iOccupyTime;
		resRobbery.bLookLeft	= CT_TRUE;
		resRobbery.towerProductCost = normalCost;


		G_GetFightingData(pUser, resRobbery.LeftFightingData);
		pUser->m_userArmy.DeleteBoss();

		CCUser*  pRight = 0;
		if (CCUserManager::getSingleton().Find(slotInfo.userInfo.dwUser, &pRight))
		{
			G_GetFightingData(pRight, resRobbery.RightFightingData);
			//CCGameMesEvent::SendThreadInfo(pOccupyApply->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum, MSC_MAIN_TOWER_EVENT,  MSC_SUB_TOWER_RES_PVP, (CT_CHAR*)&resRobbery, sizeof(resRobbery) );
			CT_DWORD threadid = GAME_THREAD_FIGHT + pOccupyApply->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
			G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_TOWER_RES_PVP, (CT_CHAR*)&resRobbery, sizeof(resRobbery));
			return;
		}else 
		{
			MSC_TowerOfflineUserFightData offlineUserFightData = {0};
			offlineUserFightData.dwUserID           = pOccupyApply->dwUserID;
			offlineUserFightData.towerPVPResRobbery = resRobbery;              //战斗数据保存，还会从数据库线程中返回使用
			offlineUserFightData.dwOfflineUserID    = slotInfo.userInfo.dwUser;  

			FightingData fd={0};
			if(CCFightingData::getSingleton().GetFightingData(offlineUserFightData.dwOfflineUserID,fd))
			{
				memcpy(&offlineUserFightData.offlineFightingData,&fd.FData,sizeof(fd.FData));
				DisposeTowerOfflineData((CT_CHAR*)&offlineUserFightData,sizeof(offlineUserFightData));
			}
			else
			{
				//CCGameMesEvent::SendThreadInfo(pOccupyApply->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum , MSC_MAIN_DB_LOGIN,  MSC_SUB_DB_TOWER_OFFLINE_DATA, (CT_CHAR*)&offlineUserFightData, sizeof(offlineUserFightData) );
				CT_DWORD threadid = GAME_THREAD_DATABASE + pOccupyApply->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_TOWER_OFFLINE_DATA, (CT_CHAR*)&offlineUserFightData, sizeof(offlineUserFightData));
			}
			return;
		}
	}
}

CT_VOID CCTowerEvent::DisposeTowerOfflineData(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(MSC_TowerOfflineUserFightData))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerOfflineData len != sizeof(MSC_TowerOfflineUserFightData)");
		return;
	}
	MSC_TowerOfflineUserFightData* pOfflineUserFightData = (MSC_TowerOfflineUserFightData*)pBuf;

	MSC_TowerPVP_ResRobbery  resRobbey = pOfflineUserFightData->towerPVPResRobbery;
	resRobbey.RightFightingData        = pOfflineUserFightData->offlineFightingData;

	//CCGameMesEvent::SendThreadInfo(pOfflineUserFightData->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum, MSC_MAIN_FIGHTING_EVENT,  MSC_SUB_TOWER_RES_PVP, (CT_CHAR*)&resRobbey, sizeof(resRobbey) );
	CT_DWORD threadid = GAME_THREAD_FIGHT + pOfflineUserFightData->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT,  MSC_SUB_TOWER_RES_PVP, (CT_CHAR*)&resRobbey, sizeof(resRobbey) );
	return;
}

CT_VOID CCTowerEvent::DisposeOccupySuc(const ElemTowerUserInfo& userInfo, CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlotID, const CMD_G_NormalReward&  costRes, CT_INT32 iOccupyTime)
{
	//结算
	G_TowerCheckPoint gtcp = {0};
	if(!G_GetTowerCheckPoint(iFloor, &gtcp))
	{
		//CT_ASSERT(CT_FALSE);
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeOccupySuc 没有找到关卡数据 cpid=%i",  iFloor);
		return;
	}

	CCTowerResManager::ElementTowerSlotInfo primaryInfo = {0};
	CCTowerResManager::getSingleton().GetSlotInTower(iTower, iFloor, iSlotID, primaryInfo);
	if (primaryInfo.userInfo.dwUser)
	{
		CCTowerResManager::getSingleton().EaseUserInTower( primaryInfo.userInfo.dwUser, iTower, iFloor, iSlotID);  
	}

	
	MSC_DB_TowerSlotReward  towerSlotReword = {0};
	if(primaryInfo.userInfo.dwUser != 0 && CCTowerResManager::getSingleton().CalHarvastRes(  iTower, iSlotID, primaryInfo, gtcp.iProductID,  time(0),  CT_TRUE, towerSlotReword))
	{
		G_ValueRange vr = {0};
		if (!G_GetValueRange(VALUE_TYPE_TOWER_OCCUPY_LOSE_RATE_NUM, &vr))
		{
			CCLogMeg::getSingleton().WriteError(" G_GetValueRange VALUE_TYPE_TOWER_OCCUPY_LOSE_RATE_NUM = %i", VALUE_TYPE_TOWER_OCCUPY_LOSE_RATE_NUM);
			return;
		}
		CCTowerResManager::getSingleton().Harvast( iFloor, iSlotID, primaryInfo.userInfo.dwUser,towerSlotReword, 1 - vr.fMaxVal );
		CCTowerResManager::getSingleton().Harvast( iFloor, iSlotID, userInfo.dwUser,             towerSlotReword, vr.fMaxVal );
		
	}

	//资源易主
	CCTowerResManager::ElementTowerSlotInfo slotInfo = {0};
	slotInfo.llBeginTime = time(0);
	slotInfo.userInfo =  userInfo;
	
	if(!CCTowerResManager::getSingleton().SetUserInTower(slotInfo.userInfo, iTower, iFloor, iSlotID))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerOccupy error itower =%i, ifloor=%i, islot=%i",  iTower, iFloor, iSlotID);
		return ;
	}

	CMD_G_ElementFloorOccupySuc   elementFloorSuc = {0};
	elementFloorSuc.dwUserID = userInfo.dwUser;
	elementFloorSuc.iTower = iTower;
	elementFloorSuc.iFoor  = iFloor;
	elementFloorSuc.iSlot =  iSlotID;
	elementFloorSuc.slotInfo.userInfo = slotInfo.userInfo;
	elementFloorSuc.costRes = costRes;


	memcpy(elementFloorSuc.slotInfo.iHelpFriend, slotInfo.iHelpFriend, sizeof(CT_DWORD)*ELEMENT_TOWER_MAX_FRIED_HELP_NUM );
	elementFloorSuc.slotInfo.iLeftTime = iOccupyTime;

	CMD_G_ElementFloor elementFloor = {0};
	elementFloor.iTower = iTower;
	elementFloor.iFoor  = iFloor;
	elementFloor.iSlotID = iSlotID;
	elementFloor.slotInfo.userInfo  = slotInfo.userInfo;
	elementFloor.slotInfo.iLeftTime = iOccupyTime;
	memcpy(elementFloor.slotInfo.iHelpFriend, slotInfo.iHelpFriend, sizeof(CT_DWORD) * ELEMENT_TOWER_MAX_FRIED_HELP_NUM);
	CCTowerResManager::getSingleton().BroadCastInTower(iTower, MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FLOOR_CHANGE, (CT_CHAR*)&elementFloor, sizeof(elementFloor));


	CCUser *pUser  = 0;
	if (CCUserManager::getSingleton().Find(userInfo.dwUser, &pUser))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_SUC, (CT_CHAR*)&elementFloorSuc, sizeof(elementFloorSuc));
	}
	CCUser *pPriUser  = 0;
	if (CCUserManager::getSingleton().Find(primaryInfo.userInfo.dwUser, &pPriUser))
	{
		pPriUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FLOOR_CHANGE, (CT_CHAR*)&elementFloor, sizeof(elementFloor));
	}
	else
	{
		if (primaryInfo.userInfo.dwUser != 0)
		{
			MSC_DB_MSG dbMSG = {0};
			dbMSG.dwUserID = primaryInfo.userInfo.dwUser;
			dbMSG.iMSGType = 1;
			dbMSG.iTime = time(0);
			sprintf(dbMSG.cMSG, "<1:%d:%s>{51}<14:%d:%d>{52}",userInfo.dwUser, userInfo.cNickName, iTower, iFloor );
			CT_DWORD threadid = GAME_THREAD_DATABASE + dbMSG.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_MSG_ADD,(CT_CHAR*)&dbMSG,sizeof(dbMSG));
		}
	}

	if (gtcp.iType == 1)
	{
		CT_CHAR cNickName[NAME_SIZE]={0};
		memcpy(cNickName,userInfo.cNickName,NAME_SIZE);
		if (primaryInfo.userInfo.dwUser == 0)
		{
			RollBroadcast rb[3]={0};
			rb[0].SetValue(userInfo.dwUser, cNickName);
			rb[1].SetValue(iTower, "");
			rb[2].SetValue(iFloor, "");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{54}<14:%d:%d>{71}", userInfo.dwUser, userInfo.cNickName, iTower, iFloor);
			G_SendRollBroadecast(116,4,1,(CT_CHAR*)rb,sizeof(rb));
		}
		else
		{
			RollBroadcast rb[4]={0};
			rb[0].SetValue(userInfo.dwUser, cNickName);
			rb[1].SetValue(primaryInfo.userInfo.dwUser, primaryInfo.userInfo.cNickName);
			rb[2].SetValue(iTower, "");
			rb[3].SetValue(iFloor, "");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{53}<1:%d:%s>{54}<14:%d:%d>{71}", 
				//userInfo.dwUser, userInfo.cNickName, primaryInfo.userInfo.dwUser, primaryInfo.userInfo.cNickName, iTower, iFloor);
			G_SendRollBroadecast(117,4,60,(CT_CHAR*)rb,sizeof(rb));
		}
	}
/* 运营说取消好友协助推送
	CMD_G_TowerFriendOccupySlot tfos = {0};
	tfos.dwFriendID = userInfo.dwUser;
	memcpy(tfos.cNickName, userInfo.cNickName, NAME_SIZE);
	tfos.iCPID = iFloor;
	tfos.iSlotID = iSlotID;
	pUser->m_userFriend.Broadcast(MAIN_G_ELEMENT_TOWER,SUB_G_TOWER_FRIEND_OCCUPY_SLOT, (CT_CHAR*)&tfos, sizeof(tfos));
*/
	//占领成功，判断攻击指定生产点任务
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_FIGHT;
	taskSysInfo.arrayParam[0] = iFloor;        //参数0：关卡id
	taskSysInfo.iProgress = 1;
	pUser->m_userTask.DisposeTask(taskSysInfo);

}

CT_VOID CCTowerEvent::DisposeOccupyLose(CT_DWORD dwUserID, CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlotID, const CMD_G_NormalReward&  costRes)
{
	CMD_G_ElementFloorOccupyFail occupyFail = {0};
	occupyFail.dwUserID = dwUserID;
	occupyFail.costRes  = costRes;
	occupyFail.iErrorCode  = EEFOF_OCCUPY_FIGHT_FAIL;
	occupyFail.iTower      = iTower;
	occupyFail.iFoor       = iFloor;
	occupyFail.iSlot       = iSlotID;

	CCUser *pUser  = 0;
	MasterData masterData = {0};
	if (CCUserManager::getSingleton().Find(dwUserID, &pUser))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_OCCUPY_LOSE  , (CT_CHAR*)&occupyFail, sizeof(occupyFail));
		pUser->m_userBase.GetMasterData(masterData);
	}

	CCTowerResManager::ElementTowerSlotInfo primaryInfo = {0};
	CCTowerResManager::getSingleton().GetSlotInTower(iTower, iFloor, iSlotID, primaryInfo);
	if (primaryInfo.userInfo.dwUser != 0)
	{
		CCUser *pPriUser  = 0;
		if (!CCUserManager::getSingleton().Find(primaryInfo.userInfo.dwUser, &pPriUser))
		{
			MSC_DB_MSG dbMSG = {0};
			dbMSG.dwUserID = primaryInfo.userInfo.dwUser;
			dbMSG.iMSGType = 1;
			dbMSG.iTime = time(0);
			sprintf(dbMSG.cMSG, "<1:%d:%s>{50}<14:%d:%d>{52}",dwUserID, masterData.cNickName, iTower, iFloor );
			CT_DWORD threadid = GAME_THREAD_DATABASE + dbMSG.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_MSG_ADD,(CT_CHAR*)&dbMSG,sizeof(dbMSG));
		}
	}
}

CT_VOID CCTowerEvent::DisposTowerPVPFromFight(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(MSC_TowerPVP_ResRobbery))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposTowerPVPFromFight len != sizeof(MSC_TowerPVP_ResRobbery)");
		return;
	}
	
	MSC_TowerPVP_ResRobbery* pResRobbery = (MSC_TowerPVP_ResRobbery*)pBuf;

	CT_DWORD threadid = 0;
	TowerEvent towerEvent = {0};
	CombatUnitData combatUnitData = {0};
	CT_INT64 iTime = time(0);
	pResRobbery->iFID = iTime;
	if(pResRobbery->iWin == WINER_LIFT) //挑战方赢
	{
		CCUser* pUser = 0;
		if (CCUserManager::getSingleton().Find(pResRobbery->dwUserID, &pUser))
		{
			ElemTowerUserInfo userInfo = {0};
			MasterData md = {0};
			pUser->m_userBase.GetMasterData(md);
			userInfo.dwUser = pResRobbery->dwUserID;
			memcpy(userInfo.cNickName, md.cNickName, NAME_SIZE);
			userInfo.iGrade = md.mvd.stGrade.iLevel;
			userInfo.iFightPower = md.mvd.stRes.iFightingPower;            //战力
			userInfo.iOccupation = md.iOccupation;            //职业
			DisposeOccupySuc(userInfo, pResRobbery->iTowerID , pResRobbery->iFloorID, pResRobbery->iSlotID, pResRobbery->towerProductCost, pResRobbery->iOccupyTime);
		
			towerEvent.dwUserID = pResRobbery->dwUserID;
			towerEvent.iEventType = 1;		//我挑战别人成功
			towerEvent.iFoor = pResRobbery->iFloorID;
			towerEvent.dwOID = pResRobbery->RightFightingData.dwUserID;
			towerEvent.iFightingID = iTime;
			G_GetMasterFightingData(pResRobbery->RightFightingData, combatUnitData);
			memcpy(towerEvent.cOName, combatUnitData.name, NAME_SIZE);
			pUser->m_userTower.InsertPVPInfo(*pResRobbery);
			pUser->m_userTower.InsertEvent(towerEvent, CT_TRUE);

			threadid = GAME_THREAD_DATABASE + pResRobbery->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWERSLOT_FIGHT,(CT_CHAR*)pResRobbery,sizeof(MSC_TowerPVP_ResRobbery));

			G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_EVENT,(CT_CHAR*)&towerEvent,sizeof(TowerEvent));
		}
		
		towerEvent.dwUserID = pResRobbery->RightFightingData.dwUserID;
		towerEvent.iEventType = 3;		//别人挑战我成功
		towerEvent.iFoor = pResRobbery->iFloorID;
		towerEvent.dwOID = pResRobbery->LeftFightingData.dwUserID;
		towerEvent.iFightingID = iTime;
		G_GetMasterFightingData(pResRobbery->LeftFightingData, combatUnitData);
		memcpy(towerEvent.cOName, combatUnitData.name, NAME_SIZE);
		CCUser *pOUser = 0;
		if (CCUserManager::getSingleton().Find(pResRobbery->RightFightingData.dwUserID, &pOUser))
		{
			pOUser->m_userTower.InsertPVPInfo(*pResRobbery);
			pOUser->m_userTower.InsertEvent(towerEvent, CT_TRUE);
		}
		pResRobbery->dwUserID = pResRobbery->RightFightingData.dwUserID;
		threadid = GAME_THREAD_DATABASE + pResRobbery->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWERSLOT_FIGHT,(CT_CHAR*)pResRobbery,sizeof(MSC_TowerPVP_ResRobbery));
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_EVENT,(CT_CHAR*)&towerEvent,sizeof(TowerEvent));
	}else                 //被挑战方赢
	{
		DisposeOccupyLose(pResRobbery->dwUserID, pResRobbery->iTowerID, pResRobbery->iFloorID, pResRobbery->iSlotID, pResRobbery->towerProductCost);

		CCUser *pUser = 0;
		if (CCUserManager::getSingleton().Find(pResRobbery->dwUserID, &pUser))
		{
			towerEvent.dwUserID = pResRobbery->LeftFightingData.dwUserID;
			towerEvent.iEventType = 2;			//我挑战别人失败
			towerEvent.iFoor = pResRobbery->iFloorID;
			towerEvent.dwOID = pResRobbery->RightFightingData.dwUserID;
			towerEvent.iFightingID = time(0);
			G_GetMasterFightingData(pResRobbery->RightFightingData, combatUnitData);
			memcpy(towerEvent.cOName, combatUnitData.name, NAME_SIZE);
			pUser->m_userTower.InsertPVPInfo(*pResRobbery);
			pUser->m_userTower.InsertEvent(towerEvent, CT_TRUE);
		}
		threadid = GAME_THREAD_DATABASE + pResRobbery->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWERSLOT_FIGHT,(CT_CHAR*)pResRobbery,sizeof(MSC_TowerPVP_ResRobbery));
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_EVENT,(CT_CHAR*)&towerEvent,sizeof(TowerEvent));

		towerEvent.dwUserID = pResRobbery->RightFightingData.dwUserID;
		towerEvent.iEventType = 4;		//别人挑战我失败
		towerEvent.iFoor = pResRobbery->iFloorID;
		towerEvent.dwOID = pResRobbery->LeftFightingData.dwUserID;
		towerEvent.iFightingID = time(0);
		G_GetMasterFightingData(pResRobbery->LeftFightingData, combatUnitData);
		memcpy(towerEvent.cOName, combatUnitData.name, NAME_SIZE);
		CCUser *pOUser = 0;
		if (CCUserManager::getSingleton().Find(pResRobbery->RightFightingData.dwUserID, &pOUser))
		{
			 pOUser->m_userTower.InsertPVPInfo(*pResRobbery);
			pOUser->m_userTower.InsertEvent(towerEvent, CT_TRUE);
		}
		pResRobbery->dwUserID = pResRobbery->RightFightingData.dwUserID;
		threadid = GAME_THREAD_DATABASE + pResRobbery->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWERSLOT_FIGHT,(CT_CHAR*)pResRobbery,sizeof(MSC_TowerPVP_ResRobbery));
		G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_EVENT,(CT_CHAR*)&towerEvent,sizeof(TowerEvent));
	}
}

CT_VOID CCTowerEvent::DisposeTowerLookFighting(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_ElementLookFighting))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerLookFighting len != sizeof(CMD_G_ElementLookFighting)");
		return;
	}

	CMD_G_ElementLookFighting  * pLookFighting = (CMD_G_ElementLookFighting *) pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pLookFighting->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerLookFighting 没有发现用户 id=%i",pLookFighting->dwUserID );
		return;
	}

	MSC_TowerPVP_ResRobbery  resRobbey = {0};
	if (!pUser->m_userTower.GetPVPInfo(pLookFighting->iFightingID, &resRobbey))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER,SUB_G_ELEMENT_TOWER_LOOK_FAIL);
		return;
	}
	resRobbey.dwUserID = pLookFighting->dwUserID;
	resRobbey.bLookLeft = pLookFighting->dwUserID==resRobbey.LeftFightingData.dwUserID?CT_TRUE:CT_FALSE;

	//CCGameMesEvent::SendThreadInfo(pOfflineUserFightData->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum, MSC_MAIN_FIGHTING_EVENT,  MSC_SUB_TOWER_RES_PVP, (CT_CHAR*)&resRobbey, sizeof(resRobbey) );
	CT_DWORD threadid = GAME_THREAD_FIGHT + pLookFighting->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT,  MSC_SUB_TOWER_PVP_LOOK, (CT_CHAR*)&resRobbey, sizeof(resRobbey) );
	

}


//助理朋友协助
CT_VOID CCTowerEvent::DisposeTowerFriendHelpApply(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_ElementFloorHelpApply ))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerFriendHelpApply len != sizeof(CMD_G_ElementFloorHelpApply)");
		return; 
	}

	CMD_G_ElementFloorHelpApply  * pHelpApply = (CMD_G_ElementFloorHelpApply *) pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pHelpApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerFriendHelpApply 没有发现用户 id=%i",pHelpApply->dwUserID );
		return;
	}

	CMD_G_ElementFloorHelpFail helpFail = {0};
	helpFail.dwUserID = pHelpApply->dwUserID;
	helpFail.dwFriendID = pHelpApply->dwFriendID;
	helpFail.iFoor      = pHelpApply->iFoor;
	helpFail.iTower     = pHelpApply->iTower;

	if (!pUser->m_userTower.CheckHelpArea(pHelpApply->iFoor))
	{
		helpFail.iErrorCode  = EEFHF_HELP_NO_PASS;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER,	SUB_G_ELEMENT_TOWER_HELP_FAIL, (CT_CHAR*)&helpFail, sizeof(helpFail));
		return;
	}


	//查看目前资源点的状况
	CCTowerResManager::ElementTowerSlotInfo slotInfo = {0};
	if(!CCTowerResManager::getSingleton().GetSlotInTower(pHelpApply->iTower, pHelpApply->iFoor, pHelpApply->iSlot, slotInfo) || slotInfo.userInfo.dwUser != pHelpApply->dwFriendID)
	{
		helpFail.iErrorCode  = EEFHF_FRIEND_NO_SLOT_FAIL;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER,	SUB_G_ELEMENT_TOWER_HELP_FAIL, (CT_CHAR*)&helpFail, sizeof(helpFail));
		return;
	}

	if (!pUser->m_userFriend.CheckIsFriend(pHelpApply->dwFriendID))
	{
		helpFail.iErrorCode  = EEFHF_FRIEND_NO_SLOT_FAIL;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER,	SUB_G_ELEMENT_TOWER_HELP_FAIL, (CT_CHAR*)&helpFail, sizeof(helpFail));
		return;
	}

	if (!pUser->m_userTower.CheckHelp(pHelpApply->dwFriendID))
	{
		helpFail.iErrorCode  = EEFHF_ONCE_A_DAY;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER,	SUB_G_ELEMENT_TOWER_HELP_FAIL, (CT_CHAR*)&helpFail, sizeof(helpFail));
		return;
	}
	
	for (CT_INT32 i=0; i<ELEMENT_TOWER_MAX_FRIED_HELP_NUM; ++i )
	{
		if (slotInfo.iHelpFriend[i] == pHelpApply->dwFriendID )
		{
			helpFail.iErrorCode  = EEFHF_HELP_ALEADY_FAIL;
			pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER,	SUB_G_ELEMENT_TOWER_HELP_FAIL, (CT_CHAR*)&helpFail, sizeof(helpFail));
			return;
		}
	}

	//个人协助次数检查
	G_ValueRange gvr = {0};
	if (!G_GetValueRange(VALUE_TYPE_TOWER_MAX_HELP_NUM, &gvr))
	{
		CCLogMeg::getSingleton().WriteError(" G_GetValueRange VALUE_TYPE_TOWER_MAX_HELP_NUM = %i", VALUE_TYPE_TOWER_MAX_HELP_NUM);
		return;
	}
	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);	
	if (masterData.mex.iTowerHelpTime >= gvr.fMaxVal)
	{
		helpFail.iErrorCode  = EEFHF_HELP_MAX_FAIL;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_HELP_FAIL, (CT_CHAR*)&helpFail, sizeof(helpFail));
		return;
	}

	//占领点协助次数检查 
	CT_INT32 iCount = 0;
	for (CT_INT32 i=0; i<ELEMENT_TOWER_MAX_FRIED_HELP_NUM; ++i )
	{
		if (slotInfo.iHelpFriend[i] != 0)
		{
			++iCount;
		}
	}
	G_ValueRange vr = {0};
	if (!G_GetValueRange(VALUE_TYPE_TOWER_MAX_BE_HELPED_NUM, &vr))
	{
		CCLogMeg::getSingleton().WriteError(" G_GetValueRange VALUE_TYPE_TOWER_MAX_BE_HELPED_NUM = %i", VALUE_TYPE_TOWER_MAX_BE_HELPED_NUM);
		return;
	}
	if ( iCount > vr.fMaxVal)
	{
		helpFail.iErrorCode  = EEFHF_HELP_FULL_FAIL;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER,	SUB_G_ELEMENT_TOWER_HELP_FAIL, (CT_CHAR*)&helpFail, sizeof(helpFail));
		return;
	}

	G_TowerCheckPoint tcp = {0};
	G_TowerHelpReward helpReward = {0};
	
	if(!G_GetTowerCheckPoint(pHelpApply->iFoor, &tcp) || !G_GetTowerHelpReward(tcp.iHelpRewardID, pHelpApply->iSlot+1, &helpReward)  )
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerFriendHelpApply iFloor =%i", pHelpApply->iFoor);
		return;
	}

	G_TowerProductReward tpr = {0};
	if (!G_GetTowerProductReward(tcp.iProductID, pHelpApply->iSlot+1, &tpr))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerFriendHelpApply iProductID =%i", tcp.iProductID);
		return;
	}
	

	for (CT_INT32 i=0; i<ELEMENT_TOWER_MAX_FRIED_HELP_NUM; ++i)
	{
		if (slotInfo.iHelpFriend[i] == 0)
		{
			CCTowerResManager::getSingleton().SetUserHelpInTower(pHelpApply->iTower, pHelpApply->iFoor, pHelpApply->iSlot, pHelpApply->dwUserID);

			FakeProBase fb={0};
			CMD_G_NormalReward  normalReward = {0};
			CMD_G_NormalReward  addNormalCost = {0};
			CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
			CT_INT32 iCounts[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
			G_PerformCostReward(helpReward.iGetGroupID, pUser, ENUM_PCRT_PERFORM, addNormalCost, iGoods, iCounts,fb);
			MasterVariableData mvd ={0};
			G_ChangeResType(mvd,addNormalCost.iResType0,addNormalCost.iResNum0);
			G_ChangeResType(mvd,addNormalCost.iResType1,addNormalCost.iResNum1);
			G_ChangeResType(mvd,addNormalCost.iResType2,addNormalCost.iResNum2);
			G_ChangeResType(mvd,addNormalCost.iResType3,addNormalCost.iResNum3);
			pUser->m_userBase.AddMastData(mvd, SAVETAG_TOWRE_HELP_REWARD);
			MasterVariableData mvd2 ={0};
			pUser->m_userBase.GetMasterData(mvd2);
			normalReward.iResType0 = addNormalCost.iResType0;
			normalReward.iResType1 = addNormalCost.iResType1;
			normalReward.iResType2 = addNormalCost.iResType2;
			normalReward.iResType3 = addNormalCost.iResType3;
			G_ChangeResType(mvd2,addNormalCost.iResType0,normalReward.iResNum0);
			G_ChangeResType(mvd2,addNormalCost.iResType1,normalReward.iResNum1);
			G_ChangeResType(mvd2,addNormalCost.iResType2,normalReward.iResNum2);
			G_ChangeResType(mvd2,addNormalCost.iResType3,normalReward.iResNum3);

			CMD_G_HelpReward cmdHelpReward = {0};
			cmdHelpReward.normalReward = normalReward;
			cmdHelpReward.dwF_ID = pHelpApply->dwFriendID;
			pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_HELP_SUC, (CT_CHAR*)&cmdHelpReward, sizeof(CMD_G_HelpReward));

			pUser->m_userTower.InsertHelp(pHelpApply->dwFriendID);
			pUser->m_userBase.SetMastDataEx(masterData.mex.iTowerHelpTime+1,SAVETAGEX_TOWER_HELP);

			MSC_LOGDB_TowerHelp logdbTowerHelp = {0};
			logdbTowerHelp.dwUserID = pHelpApply->dwUserID;
			logdbTowerHelp.dwFUID = pHelpApply->dwFriendID;
			logdbTowerHelp.iCPID = pHelpApply->iFoor;
			logdbTowerHelp.iSlotID = pHelpApply->iSlot;
			logdbTowerHelp.iResType[0] = addNormalCost.iResType0;
			logdbTowerHelp.iResType[1] = addNormalCost.iResType1;
			logdbTowerHelp.iResType[2] = addNormalCost.iResType2;
			logdbTowerHelp.iResType[3] = addNormalCost.iResType3;
			logdbTowerHelp.iResNum[0] = addNormalCost.iResNum0;
			logdbTowerHelp.iResNum[1] = addNormalCost.iResNum1;
			logdbTowerHelp.iResNum[2] = addNormalCost.iResNum2;
			logdbTowerHelp.iResNum[3] = addNormalCost.iResNum3;
			logdbTowerHelp.iUGrade = mvd2.stGrade.iLevel;
			CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbTowerHelp.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TOWER_HELP,(CT_CHAR*)&logdbTowerHelp,sizeof(MSC_LOGDB_TowerHelp));
			
			//协助成功，判断祝福任务
			CMD_G_TaskSystemInfo stTaskSys = {0};
			stTaskSys.iTaskMainType = TASK_MAIN_TYPE_SYS;
			stTaskSys.iTaskSubType = TASK_SUB_TYPE_SYSTEM_BLESS;
			stTaskSys.iProgress = 1;
			pUser->m_userTask.DisposeTask(stTaskSys);

			CMD_G_ElementFloor floor = {0};
			floor.iTower = pHelpApply->iTower;
			floor.iFoor  = pHelpApply->iFoor;
			floor.iSlotID = pHelpApply->iSlot;
			CCTowerResManager::ElementTowerSlotInfo slotInfo = {0};
			CCTowerResManager::getSingleton().GetSlotInTower(pHelpApply->iTower, pHelpApply->iFoor, pHelpApply->iSlot,slotInfo);
			
			floor.slotInfo.userInfo  = slotInfo.userInfo;
			floor.slotInfo.iLeftTime = tpr.iOccupyTime - ((CT_INT32)(time(0) - slotInfo.llBeginTime));
			memcpy(floor.slotInfo.iHelpFriend,  slotInfo.iHelpFriend, sizeof(floor.slotInfo.iHelpFriend)) ;

			CCUser* pFriend = 0;
			if(CCUserManager::getSingleton().Find(pHelpApply->dwFriendID, &pFriend))
			{
				pFriend->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FRIEND_HELP, (CT_CHAR*)&floor, sizeof(floor)); 
			}
			
			CCTowerResManager::getSingleton().BroadCastInTower(pHelpApply->iTower, MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FLOOR_CHANGE, (CT_CHAR*)&floor, sizeof(floor)); 		
			return;
		}
	}
}

CT_VOID CCTowerEvent::DisposeTowerUserExit(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_ElementTowerExitApply))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerUserExit len != sizeof(CMD_G_ElementTowerExitApply)");
		return;
	}
	
	CMD_G_ElementTowerExitApply* pExitAplly = (CMD_G_ElementTowerExitApply*)pBuf;
	
	CCUser* pUser =0;
	if (!CCUserManager::getSingleton().Find(pExitAplly->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerUserExit 没有这个在线用户uid=%i",pExitAplly->dwUserID );
		return;
	}

	CCTowerResManager::getSingleton().DeleteUserEnterMap(pExitAplly->dwUserID);
	pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_EXIT_SUC);
}


CT_VOID CCTowerEvent::DisposeElementTowerGiveupApply(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_ElementFloorGiveupApply))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerGiveupApply len != sizeof(CMD_G_ElementFloorGiveupApply)");
		return;
	}

	CMD_G_ElementFloorGiveupApply * giveupApply = (CMD_G_ElementFloorGiveupApply *)pBuf;
	
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(giveupApply->dwUID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeElementTowerGiveupApply 没有发现这个用户 %i", giveupApply->dwUID);
		return ;
	}


	CCTowerResManager::getSingleton().GiveUpSlot(giveupApply->iTower, giveupApply->iFoor, giveupApply->iSlot);
}


CT_VOID CCTowerEvent::DisposeTowerPassApply(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_TowerPassApply))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerPassApply len != sizeof(CMD_G_TowerPassApply)");
		return ;
	}

	CMD_G_TowerPassApply* pPassApply = (CMD_G_TowerPassApply*)pBuf;
	CT_INT32 iFailCode = 0;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pPassApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerPassApply 用户不在线 %d", pPassApply->dwUserID);
		return;
	}

	// 先检查上一关是否已通过
	G_TowerCheckPoint tcp = {0};
	if (!G_GetTowerCheckPoint(pPassApply->iCPID, &tcp))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerPassApply 关卡不存在 %d", pPassApply->iCPID);
		iFailCode = ETPF_INI_DATA_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	G_TowerArea ta = {0};
	if (!G_GetTowerArea(tcp.iAreaID, &ta))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerPassApply 未找到关卡%d的区域数据", pPassApply->iCPID);
		iFailCode = ETPF_INI_DATA_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);
	if (ta.iGrade > masterData.mvd.stGrade.iLevel)
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerPassApply %d 等级不够", pPassApply->iCPID);
		iFailCode = ETPF_NO_GRADE;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	// 先检查上一关是否已通过
	if (ta.iStartCPID == pPassApply->iCPID)	//起始关卡
	{
		if (!pUser->m_userTower.CheckPointPass(ta.iCPID, 0))
		{
			iFailCode = ETPF_PRE_NO_PASS;
			pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
			return;
		}
	}
	else
	{										// 否则检查上一关是否已通过
		if (!pUser->m_userTower.CheckPointPass(pPassApply->iCPID - 1, 0))
		{
			iFailCode = ETPF_PRE_NO_PASS;
			pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
			return;
		}
	}

	if (pUser->m_userTower.HaveTreasure())
	{
		iFailCode = ETPF_HAVE_TREASURE;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	// 然后获取用户战斗数据和怪物组(不要忘记已通关永久属性加成和战队属性加成)
	MSC_TowerCheckPoint_Fighting FightingInfo={0};
	FightingInfo.dwUserID = pPassApply->dwUserID;
	FightingInfo.iCPID = pPassApply->iCPID;
	FightingInfo.iSeed=rand()%RAND_MAX_EX;

	G_GetFightingData(pUser, FightingInfo.FightingData);
	pUser->m_userArmy.DeleteBoss();
	G_TowerTeamReward ttr = {0};
	CT_DWORD dwFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pPassApply->dwUserID);
	CCFightTeam *pFightTeam = 0;
	if (CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
	{
		CT_INT32 iNum = pFightTeam->GetTowerPassCount(pPassApply->iCPID);
		if (iNum > 0)
		{
			if (G_GetTowerTeamReward(tcp.iTeamRewardID, iNum, &ttr))
			{
				G_AttributesReward ar = {0};
				if (G_GetAttributesReward(ttr.iRewardID, &ar))
				{
					for(CT_INT32 i=0;i<POST_COUNT/2;++i)
					{
						CT_INT32 iType=FightingInfo.FightingData.units[i].type;
						if(iType == UNIT_ALL_ARMY)
						{
							if (ar.iResType1 > 0)
							{
								pUser->m_userBase.UpdateBattleStatus(FightingInfo.FightingData.units[i].mcs, ar.iResType1, ar.fResNum1);
							}
							if (ar.iResType2 > 0)
							{
								pUser->m_userBase.UpdateBattleStatus(FightingInfo.FightingData.units[i].mcs, ar.iResType2, ar.fResNum2);
							}
							if (ar.iResType3 > 0)
							{
								pUser->m_userBase.UpdateBattleStatus(FightingInfo.FightingData.units[i].mcs, ar.iResType3, ar.fResNum3);
							}
							if (ar.iResType4 > 0)
							{
								pUser->m_userBase.UpdateBattleStatus(FightingInfo.FightingData.units[i].mcs, ar.iResType4, ar.fResNum4);
							}
							break;
						}
					}
				}
			}
		}
	}

	CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_TOWER_CP_PASS, (CT_CHAR*)&FightingInfo, sizeof(FightingInfo));
//	pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_SUC, (CT_CHAR*)pPassApply->iCPID, sizeof(pPassApply->iCPID));

}

CT_VOID CCTowerEvent::DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_TowerCheckPoint_Fighting))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeFightingEnd len != sizeof(MSC_TowerCheckPoint_Fighting)");
		return;
	}

	MSC_TowerCheckPoint_Fighting *pFightingEnd=(MSC_TowerCheckPoint_Fighting*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingEnd->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeFightingEnd %d No Find UserID");
		return;
	}

	//失败不更新
	if(pFightingEnd->iScore == 0)
	{
		CT_INT32 iFailCode = ETPF_PASS_LOSE;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_PASS_APPLY_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	pUser->m_userTower.UpdateCheckPoint(pFightingEnd->iCPID, pFightingEnd->iScore); //更新通关记录和发放奖励

	//通关成功，判断指定关卡的通关任务
	CMD_G_TaskSystemInfo stTaskSys = {0};
	stTaskSys.iTaskMainType = TASK_MAIN_TYPE_SYS;
	stTaskSys.iTaskSubType = TASK_SUB_TYPE_SYSTEM_POINTOR;
	stTaskSys.arrayParam[0] = pFightingEnd->iCPID;        // 参数0：关卡id
	pUser->m_userTask.DisposeTask(stTaskSys);

	//判断N次通关任务
	stTaskSys.iTaskSubType = TASK_SUB_TYPE_SYSTEM_CHALLENGE;
	stTaskSys.iProgress = 1;
	pUser->m_userTask.DisposeTask(stTaskSys);

	CT_INT32 iFID = CCTowerResManager::getSingleton().InsertTowerFightExample(*pFightingEnd);
	if (iFID > 0)
	{
		//推送全服
		CombatUnitData  data={0};
		CMD_G_TowerFightExample towerFightExample = {0};
		towerFightExample.iCPID = pFightingEnd->iCPID;
		towerFightExample.iTime = iFID;
		towerFightExample.dwUserID = pFightingEnd->dwUserID;
		G_GetMasterFightingData(pFightingEnd->FightingData,data);
		memcpy(towerFightExample.cNickName, data.name, NAME_SIZE);
		CCUserManager::getSingleton().BroadCastMessage(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_FIGHT_ADD, (CT_CHAR*)&towerFightExample, sizeof(towerFightExample));
		G_TowerCheckPoint tcp = {0};
		if (!G_GetTowerCheckPoint(pFightingEnd->iCPID, &tcp))
		{
			if (tcp.iType == 1)
			{

				MasterData masterData = {0};
				RollBroadcast rb[3]={0};
				pUser->m_userBase.GetMasterData(masterData);
				rb[0].SetValue(pFightingEnd->dwUserID, masterData.cNickName);
				rb[1].SetValue(tcp.iAreaID,"");
				rb[2].SetValue(tcp.iCPID, "");
				if (1 == iFID)
				{
					G_SendRollBroadecast(63,3,60,(CT_CHAR*)rb,sizeof(rb));
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{63}<1:%d:%s>{64}<14:%d:%d>{67}", pFightingEnd->dwUserID, masterData.cNickName, tcp.iAreaID, tcp.iCPID);
				}
				else if (2 == iFID)
				{
					G_SendRollBroadecast(64,3,60,(CT_CHAR*)rb,sizeof(rb));
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{63}<1:%d:%s>{65}<14:%d:%d>{67}", pFightingEnd->dwUserID, masterData.cNickName, tcp.iAreaID, tcp.iCPID);
				}
				else if (3 == iFID)
				{
					G_SendRollBroadecast(65,3,60,(CT_CHAR*)rb,sizeof(rb));
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{63}<1:%d:%s>{66}<14:%d:%d>{67}", pFightingEnd->dwUserID, masterData.cNickName, tcp.iAreaID, tcp.iCPID);
				}
			}
		}
	}
}

CT_VOID CCTowerEvent::DisposeTowerRaids(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_TowerRaidsApply))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerRaids len != sizeof(CMD_G_TowerRaidsApply)");
		return ;
	}

	CMD_G_TowerRaidsApply* pRaidsApply = (CMD_G_TowerRaidsApply*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pRaidsApply->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerRaids 用户不在线 %i", pRaidsApply->dwUserID);
		return;
	}
	CT_INT32 iFailCode = 0;
	if (pRaidsApply->iBeginCPID > pRaidsApply->iEndCPID)
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerRaids iBeginCPID > iEndCPID");
		return;
	}

	G_TowerCheckPoint tcp = {0};
	if (!G_GetTowerCheckPoint(pRaidsApply->iBeginCPID, &tcp))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerRaids 关卡不存在 %d", pRaidsApply->iBeginCPID);
		iFailCode = ETPF_INI_DATA_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	G_TowerArea ta = {0};
	if (!G_GetTowerArea(tcp.iAreaID, &ta))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerRaids 未找到关卡%d的区域数据", pRaidsApply->iBeginCPID);
		iFailCode = ETPF_INI_DATA_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	// 先检查上一关是否已通过
	if (ta.iStartCPID == pRaidsApply->iBeginCPID)	//起始关卡
	{
		if (!pUser->m_userTower.CheckPointPass(ta.iCPID, 0))
		{
			iFailCode = ETPF_PRE_NO_PASS;
			pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
			return;
		}
	}
	else
	{										// 否则检查上一关是否已通过
		if (!pUser->m_userTower.CheckPointPass(pRaidsApply->iBeginCPID - 1, 0))
		{
			iFailCode = ETPF_PRE_NO_PASS;
			pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
			return;
		}
	}
	//然后检查当前关当天是否已通关
	if (pUser->m_userTower.CheckPointPass(pRaidsApply->iBeginCPID))
	{
		iFailCode = ETPF_BEGIN_HAS_PASS;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	if (!G_GetTowerCheckPoint(pRaidsApply->iEndCPID, &tcp))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerRaids 关卡EndCPID不存在 %d", pRaidsApply->iEndCPID);
		iFailCode = ETPF_INI_DATA_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	if (!pUser->m_userTower.CheckPointRaids(pRaidsApply->iEndCPID))
	{
		iFailCode = ETPF_PRE_NO_PASS;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	if (pUser->m_userTower.HaveTreasure())
	{
		iFailCode = ETPF_HAVE_TREASURE;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_RAIDS_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}

	pUser->m_userTower.UpdateRaids(pRaidsApply->iBeginCPID, pRaidsApply->iEndCPID);

}

CT_VOID CCTowerEvent::DisposeTowerUseTreasure(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_TowerUseTreasure))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerUseTreasure len != sizeof(CMD_G_TowerUseTreasure)");
		return ;
	}

	CMD_G_TowerUseTreasure* pUseTreasure = (CMD_G_TowerUseTreasure*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pUseTreasure->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerUseTreasure 用户不在线 %i", pUseTreasure->dwUserID);
		return;
	}

	pUseTreasure->iUseType = 1;	//策划临时要求只用钥匙打开宝箱
	pUser->m_userTower.TreasureUse(pUseTreasure->iCPID, pUseTreasure->iTreasureID, pUseTreasure->iUseType);
}

CT_VOID CCTowerEvent::DisposeTowerTreasureAbandon(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_TowerTreasureAbandon))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerTreasureAbandon len != sizeof(CMD_G_TowerTreasureAbandon)");
		return ;
	}

	CMD_G_TowerTreasureAbandon* pTreasure = (CMD_G_TowerTreasureAbandon*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pTreasure->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerTreasureAbandon 用户不在线 %i", pTreasure->dwUserID);
		return;
	}
	pUser->m_userTower.TreasureAbandon(pTreasure->iCPID, pTreasure->iTreasureID);
}

CT_VOID CCTowerEvent::DisposeTowerMSGRead(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len == 0 || 0 != len % sizeof(CMD_G_TowerMSGRead))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerMSGRead len != sizeof(CMD_G_TowerMSGRead)*n");
		return ;
	}

	CMD_G_TowerMSGRead* pMSGRead = (CMD_G_TowerMSGRead*)pBuf;

	CCUser* pUser = 0;
	CT_INT32 iCount = len / sizeof(CMD_G_TowerMSGRead);
	if (!CCUserManager::getSingleton().Find(pMSGRead->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerLookFight 用户不在线 %i", pMSGRead->dwUserID);
		return;
	}
	for (CT_INT32 i=0;i<iCount;i++)
	{
		pUser->m_userTower.DeleteTowerMSG(pMSGRead->iType, pMSGRead->iTime);
		pMSGRead++;
	}
	
	pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_MSG_READ, pBuf, len);
}


CT_VOID CCTowerEvent::DisposeTowerLookFight(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_TowerLookFight))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerLookFight len != sizeof(CMD_G_TowerLookFight)");
		return ;
	}

	CMD_G_TowerLookFight* pLookFight = (CMD_G_TowerLookFight*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pLookFight->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerLookFight 用户不在线 %i", pLookFight->dwUserID);
		return;
	}

	MSC_TowerCheckPoint_Fighting fighting = {0};
	if (CCTowerResManager::getSingleton().GetTowerFightExample(pLookFight->iCPID, pLookFight->iTime, fighting))
	{
		fighting.dwUserID = pLookFight->dwUserID;
		CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_TOWER_CPPASS_LOOK, (CT_CHAR*)&fighting, sizeof(MSC_TowerCheckPoint_Fighting));
	}
	else
	{
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_LOOK_FIGHT_NO);
	}
}

CT_VOID CCTowerEvent::DisposeTowerSlotAwardGet(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len != sizeof(CMD_G_TowerSlotAwardGet))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerSlotAwardGet len != sizeof(CMD_G_TowerSlotAwardGet)");
		return ;
	}

	CMD_G_TowerSlotAwardGet* pAwardGet = (CMD_G_TowerSlotAwardGet*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pAwardGet->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTowerEvent::DisposeTowerSlotAwardGet 用户不在线 %i", pAwardGet->dwUserID);
		return;
	}
	TowerSlotAwardKey towerSlotAwardKey;
	memset(&towerSlotAwardKey, 0, sizeof(TowerSlotAwardKey));
	towerSlotAwardKey.dwUserID = pAwardGet->dwUserID;
	towerSlotAwardKey.iCPID = pAwardGet->iFloor;
	towerSlotAwardKey.iSlot = pAwardGet->iSlot;
	CT_INT32 iFailCode = 0;
	if (!pUser->m_userTower.CheckTowerSlotAward(towerSlotAwardKey))
	{
		iFailCode = ETPF_NO_SLOTAWARD;
		pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_TOWER_SLOTAWARD_FAIL, (CT_CHAR*)&iFailCode, sizeof(CT_INT32));
		return;
	}
	pUser->m_userTower.RecvTowerSlotAward(towerSlotAwardKey);		
}

