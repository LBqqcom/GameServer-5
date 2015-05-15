#include "CSeekEvent.h"
#include "MessageGame.h"
#include "ProtocolGame.h"
#include "CLogMeg.h"
#include "CUserManager.h"
#include "CThreadMes.h"
#include "CFightingData.h"
//#include "CFightTeam.h"
#include "CFightTeamManager.h"
#include"CMD5.h"

CCSeekEvent::CCSeekEvent(CCGameMesManager* pMesManager, CT_DWORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_SEEK, SUB_G_SEEK_PLAYER_GET);
	RegistMessage(MAIN_G_SEEK, SUB_G_SEEK_GOODWILL);
	RegistMessage(MAIN_G_SEEK, SUB_G_SEEK_SPITE);
	RegistMessage(MAIN_G_SEEK, SUB_G_SEEK_GIVE_PRESENT);
	RegistMessage(MAIN_G_SEEK, SUB_G_SEEK_SPEEDUP_PRODUCT);
	RegistMessage(MAIN_G_SEEK, SUB_G_SEEK_ENTER_TOWN);
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_APPLY);    //掠夺请求
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_FLAG_APPLY); //拔掉旗帜
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_KIDNAP_APPLY);   //绑票请求
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_RESCUE_APPLY);   //营救请求
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_WALLADDITION_UPGRADE_APPLY);   //城墙技能升级
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_ARMY_APPLY);   //城墙军队布阵
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_ADD_PRODUCT);   //增产
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_APPLY_PRESENT);	//领取礼物
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_BACK);				//回营
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_LOOK_FIGHTING);				//查看战斗
	RegistMessage(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_REQUEST);	//界面掠夺请求

	RegistMessage(MSC_MAIN_SEEK, MSC_SUB_DB_GET_OTHERSTOWN);			//得到他人城镇信息
	RegistMessage(MSC_MAIN_SEEK, MSC_SUB_GET_SEEKED_RETURN);			//得到被访者信息

	RegistMessage(MSC_MAIN_SEEK,MSC_SUB_PLUNDER_FIGHT);
	RegistMessage(MSC_MAIN_SEEK,MSC_SUB_KIDNAP_FIGHT);
	RegistMessage(MSC_MAIN_SEEK,MSC_SUB_RESCUE_FIGHT);

	m_fFarmland=0.125f;
	m_fPanning=0.15f;
	m_Totem.fGold=0.5f;
	m_Totem.fFood=0.55f;

	RegistTimer(IDI_SEEK_PRESENT_TIME);
	SetTimer(IDI_SEEK_PRESENT_TIME, 10 * 60 * 1000);
}

CCSeekEvent::~CCSeekEvent()
{

}

CT_VOID CCSeekEvent::OnTime(CT_DWORD timeID)
{
	switch (timeID)
	{
	case IDI_SEEK_PRESENT_TIME:
		{
			CCUserManager::getSingleton().UpdateSeekPresent();
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCSeekEvent::OnTime timeID error,tid:%d", timeID);
		}
	}
}

CT_VOID CCSeekEvent::OnClock(CT_DWORD timeID)
{

}

CT_VOID CCSeekEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	if (mainID == MAIN_G_SEEK)
	{
		switch (subID)
		{
		case SUB_G_SEEK_PLAYER_GET:
			{
				DisposeSeekedPlayerUpdate(pBuf, len);
				break;
			}
		case SUB_G_SEEK_GOODWILL:
			{
				DisposeSeek(pBuf, len, SEEK_GOODWILL);
				break;
			}
		case SUB_G_SEEK_SPITE:
			{
				DisposeSeek(pBuf, len, SEEK_SPITE);
				break;
			}
		case SUB_G_SEEK_GIVE_PRESENT:
			{
				DisposePresent(pBuf, len);
				break;
			}
		case SUB_G_SEEK_SPEEDUP_PRODUCT:
			{
				DisposeBuildingUpSpeed(pBuf, len);
				break;
			}
		case SUB_G_SEEK_ENTER_TOWN:
			{
				DisposeEnterOthersTown(pBuf, len);
				break;
			}
		case SUB_G_SEEK_PLUNDER_APPLY:
			{
				DisposePlunder(pBuf, len);
				break;
			}
		case SUB_G_SEEK_PLUNDER_FLAG_APPLY:
			{
				DisposePunderFlag(pBuf, len);
				break;
			}
		case SUB_G_SEEK_KIDNAP_APPLY:
			{
				DisposeKidnap(pBuf, len);
				break;
			}
		case SUB_G_SEEK_RESCUE_APPLY:
			{
				DisposeRescue(pBuf, len);
				break;
			}
		case SUB_G_SEEK_WALLADDITION_UPGRADE_APPLY:
			{
				DisposeWallAddition(pBuf, len);
				break;
			}
		case SUB_G_SEEK_ARMY_APPLY:
			{
				DisposeArmyApply(pBuf, len);
				break;
			}
		case SUB_G_SEEK_ADD_PRODUCT:
			{
				DisoposeAddProduct(pBuf, len);
				break;
			}
		case SUB_G_SEEK_APPLY_PRESENT:
			{
				DisposeGetPresent(pBuf, len);
				break;
			}
		case SUB_G_SEEK_BACK:
			{
				DisposeSeekBack(pBuf, len);
				break;
			}
		case SUB_G_SEEK_LOOK_FIGHTING:
			{
				DisposeLookFighting(pBuf,len);
				break;
			}
		case SUB_G_SEEK_PLUNDER_REQUEST:
			{
				DisposePlunderRequest(pBuf, len);
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("CCSeekEvent::OnUserEvent subID error.mainID:%d,subID:%d",mainID, subID);
			}
		}
	}
	else if(mainID == MSC_MAIN_SEEK)
	{
		switch(subID)
		{
		case MSC_SUB_PLUNDER_FIGHT:
			{
				DisposePlunderFight(pBuf, len);
				break;
			}
		case MSC_SUB_KIDNAP_FIGHT:
			{
				DisposeKidnapFight(pBuf, len);
				break;
			}
		case MSC_SUB_RESCUE_FIGHT:
			{
				DisposeRescueFight(pBuf, len);;
				break;
			}
		case MSC_SUB_DB_GET_OTHERSTOWN:
			{
				DisposeGetOthersTownInfo(pBuf, len);
				break;
			}
		case MSC_SUB_GET_SEEKED_RETURN:
			{
				DisposeSeekedGet(pBuf, len);
				break;
			}
		default:
			{
				CCLogMeg::getSingleton().WriteError("CCSeekEvent::OnUserEvent main:%d,subid:%d",mainID,subID);
			}
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::OnUserEvent mainID error.mainID:%d,subID:%d",mainID, subID);
	}
}

CT_VOID CCSeekEvent::DisposeSeekedPlayerUpdate(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_SeekedPlayerGet))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeSeekedPlayerUpdate iLen != sizeof(CMD_G_SeekedPlayerGet)");
		return;
	}
	CMD_G_SeekedPlayerGet* pSeekPlayer = (CMD_G_SeekedPlayerGet*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pSeekPlayer->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeSeekedPlayerUpdate connot find user,uid:%d", pSeekPlayer->dwUserID);
		return;
	}
	pUser->m_userSeek.UpdateSeekedPerson(pSeekPlayer->iType);
}

// 探访请求
CT_VOID CCSeekEvent::DisposeSeek(CT_CHAR* pBuf, CT_INT32 iLen, CT_INT32 iType)
{
	if (iLen != sizeof(CMD_G_Seek))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeGoodWillSeek iLen != sizeof(CMD_G_Seek)");
		return;
	}

	CMD_G_Seek* pSeek = (CMD_G_Seek*)pBuf;
	CCUser* pUser = 0;
	CT_INT32 enFail = 0;
	G_ChestByLevel chest = {0};
	MasterData md = {0};
	SeekData sd = {0};
	if (!CCUserManager::getSingleton().Find(pSeek->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeGoodWillSeek connot find user.uid:%d", pSeek->dwUserID);
		return;
	}

	// 判断是否在探访界面
	if (!pUser->m_userSeek.IsInSeekedList(pSeek->dwFID))
	{
		enFail = ESG_USERID_ERROR;
	}

	// 判断探访次数
	if (!pUser->m_userSeek.CheckSeekTimes(iType))
	{
		enFail = ESG_NO_SEEK_TIMES;
	}
	// 是否已探访过
	if ((iType == SEEK_GOODWILL && pUser->m_userSeek.IsSeekGoodwill(pSeek->dwFID))
		|| (iType == SEEK_SPITE && pUser->m_userSeek.IsSeekSpite(pSeek->dwFID)))
	{
		enFail = ESG_HAVING_SEEKED;
	}
	// 得到善访时的礼品
	pUser->m_userBase.GetMasterData(md);
	if (!G_GetChestByLevel(iType, md.mvd.stGrade.iLevel, &chest))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeGoodWillSeek G_GetChestByLevel error");
		enFail = ESG_DATA_ERROR;
	}

	// 条件不足，发送失败
	if (enFail != 0)
	{
		CMD_G_SeekFail seekFail;
		memset(&seekFail,0,sizeof(seekFail));
		seekFail.enFail = (ENUM_SeekFail)enFail;
		if (SEEK_GOODWILL == iType)
		{
			pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_GOODWILL_FAIL,(CT_CHAR*)&seekFail,sizeof(seekFail));
		}
		else
		{
			pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_SPITE_FAIL,(CT_CHAR*)&seekFail,sizeof(seekFail));
		}
		
		return;
	}

	// 设置探访类型和被访人ID
	pUser->m_userSeek.GetSeekData(sd);
	sd.iType = iType;
	sd.dwSeekID = pSeek->dwFID;
	pUser->m_userSeek.UpdateSeekData(sd);

	// 被探访者在线
	CCUser* pFUser = 0;
	Seekers seeks = {0};
	if (CCUserManager::getSingleton().Find(pSeek->dwFID, &pFUser))
	{
		// 检查被探访次数是否足够
		if (!pFUser->m_userSeek.CheckSeekedTimes(iType))
		{
			enFail = ESG_NO_SEEKED_TIMES;
			CMD_G_SeekFail seekFail;
			memset(&seekFail,0,sizeof(seekFail));
			seekFail.enFail = (ENUM_SeekFail)enFail;
			if (SEEK_GOODWILL == iType)
			{
				pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_GOODWILL_FAIL,(CT_CHAR*)&seekFail,sizeof(seekFail));
			}
			else
			{
				pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_SPITE_FAIL,(CT_CHAR*)&seekFail,sizeof(seekFail));
			}
			return;
		}
		
		if (SEEK_GOODWILL == iType)
		{
			pUser->m_userSeek.SeekOneTimes(iType, pSeek->dwFID);
			pFUser->m_userSeek.SeekedOneTimes(iType);
		}

		// 进城镇
		CMD_G_EnterTown_Others townInfo = {0};
		townInfo.dwUserID = pSeek->dwUserID;
		townInfo.dwFUserID = pSeek->dwFID;
		DisposeEnterOthersTown((CT_CHAR*)&townInfo, sizeof(CMD_G_EnterTown_Others));
	}
	else if (CCFightingData::getSingleton().GetSeekedInfo(pSeek->dwFID, seeks))
	{
		G_ValueRange vr = {0};
		// 检查被探访次数是否足够
		if (iType == SEEK_GOODWILL)
		{
			G_GetValueRange(VALUE_TYPE_GOODSEEKED_TIMES, &vr);

			if (seeks.iSeekedWillTimes >= vr.fMaxVal)
			{
				enFail = ESG_NO_SEEKED_TIMES;
			}
			else
			{
				++seeks.iSeekedWillTimes;
			}
		}
		else if(iType == SEEK_SPITE)
		{
			G_GetValueRange(VALUE_TYPE_BADSEEKED_TIMES, &vr);

			if (seeks.iSeekedSpiteTimes >= vr.fMaxVal)
			{
				enFail = ESG_NO_SEEKED_TIMES;
			}
			/*else
			{
				++seeks.iSeekedSpiteTimes;
			}*/
		}
		if (enFail != 0)
		{
			CMD_G_SeekFail seekFail;
			memset(&seekFail,0,sizeof(seekFail));
			seekFail.enFail = (ENUM_SeekFail)enFail;
			if (SEEK_GOODWILL == iType)
			{
				pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_GOODWILL_FAIL,(CT_CHAR*)&seekFail,sizeof(seekFail));
			}
			else
			{
				pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_SPITE_FAIL,(CT_CHAR*)&seekFail,sizeof(seekFail));
			}
			return;
		}

		// 探访成功
		if (SEEK_GOODWILL == iType)
		{
			pUser->m_userSeek.SeekOneTimes(iType, pSeek->dwFID);
			CCFightingData::getSingleton().UpdateSeekedTimes(iType,seeks);
		}

		// 进城镇
		CMD_G_EnterTown_Others townInfo = {0};
		townInfo.dwUserID = pSeek->dwUserID;
		townInfo.dwFUserID = pSeek->dwFID;
		DisposeEnterOthersTown((CT_CHAR*)&townInfo, sizeof(CMD_G_EnterTown_Others));
	}
	else
	{
		MSC_DB_SeekedGet seeked = {0};
		seeked.dwUserID = pSeek->dwUserID;
		seeked.dwFUserID = pSeek->dwFID;
		seeked.iType = iType;
		CT_DWORD threadID = GAME_THREAD_DATABASE + pSeek->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_LOGIN,MSC_SUB_DB_GET_SEEKED,(CT_CHAR*)&seeked,sizeof(seeked));
		return;
	}

	if (SEEK_SPITE == iType)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_SPITE_SUC);
	}
}

// 送礼
CT_VOID CCSeekEvent::DisposePresent(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_SeekGivePresent))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePresent iLen != sizeof(CMD_G_SeekGivePresent)");
		return ;
	}

	CT_BOOL bOnline = CT_FALSE;
	CT_INT32 enfail = ESP_PRESENT_SUC;
	CT_INT32 ip = SEEK_PRESENT_ITEM_NUM;
	CMD_G_NormalReward addReward;
	CMD_G_SeekGivePresentFail presentFail;
	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	FakeProBase fb = {0};
	RewardCost rc = {0};
	G_ChestByLevel gChest = {0};
	G_ChestByLevel gTmpChest = {0};
	CT_INT32 iIndex = 0;
	CT_INT32 i = 0;
	SeekData sd = {0};
	VistorInfo vistorInfo = {0};
	CMD_G_SeekGivePresent *pPresent = (CMD_G_SeekGivePresent *)pBuf;
	CMD_G_SeekGivePresent present = {0};
	MasterVariableData mvd = {0};

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pPresent->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePresent connot find user.uid", pPresent->dwUserID);
		return;
	}

	pUser->m_userSeek.GetSeekData(sd);
	pUser->m_userBase.GetMasterData(mvd);

	// 读取对应等级的礼品信息
	if (!pUser->m_userSeek.GetSeekPresent(gChest))
	{
		enfail = ESP_PRESENT_ERROR;
	}

	// 检查礼物是否已赠送过
	if (!pUser->m_userSeek.CheckPresentStatus(pPresent->aPresent))
	{
		enfail = ESP_PRESENT_ERROR;
	}

	if (enfail != ESP_PRESENT_SUC)
	{
		// 发送客户端
		presentFail.enFail = (ENUM_SeekGivePresentFail)enfail;
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_GIVE_PRESENT_FAIL,(CT_CHAR*)&presentFail,sizeof(presentFail));
		return;
	}

	// 判断消耗
	for (CT_INT32 iPresent = 0;iPresent < SEEK_PRESENT_ITEM_NUM;++iPresent)
	{
		if (pPresent->aPresent[iPresent] == 0)
		{
			break;
		}
		for (i = 0;i < SEEK_PRESENT_ITEM_NUM;++i)
		{
			if (pPresent->aPresent[iPresent] == gChest.chest[i].iChestID)
			{
				gTmpChest.chest[iIndex++] = gChest.chest[i];

				enfail = G_PerformCostReward(gChest.chest[i].iCost, pUser, ENUM_PCRT_TEST, addReward, iGoods, iCount, fb);
				if (ENUM_CREC_ELMBAG_NOAREA == enfail || ENUM_CREC_ITEM_FAIL == enfail)
				{
					G_SendErrorInfo(pUser, ERE_SYSTEM, EEI_BUG_FULL);
					return;
				}
				else if (ENUM_CREC_RES_SUC == enfail)
				{
					enfail = ESP_PRESENT_SUC;
				}
				else
				{
					enfail = ESP_RES_LESS;
				}
				break;
			}
		}
		// 礼品的数据读不到
		if (i == SEEK_PRESENT_ITEM_NUM)
		{
			enfail = ESP_PRESENT_ERROR;
		}
	}

	CCUser* pFUser = 0;
	CT_INT32 iNum = 0;
	// 好友在线
	if (CCUserManager::getSingleton().Find(sd.dwSeekID, &pFUser))
	{
		bOnline = CT_TRUE;
		iNum = pFUser->m_userSeek.GetPresentNum();
		if (iNum + i > SEEKED_PRESENT_NUM)
		{
			presentFail.iNum = SEEKED_PRESENT_NUM - iNum;
			enfail = ESG_SEEKED_PRESENT_FULL;
		}
		//else
		//{	// 消耗并发放礼物
		//	for (CT_INT32 iCost = 0;iCost < SEEK_PRESENT_ITEM_NUM;++iCost)
		//	{
		//		if (gTmpChest.chest[iCost].iChestID == 0)
		//		{
		//			break;
		//		}

		//		pUser->m_userSeek.SendPresent(gTmpChest.chest[iCost].iChestID);
		//		vistorInfo.present[iCost] = gTmpChest.chest[iCost].iChestID;
		//		G_PerformCostReward(gTmpChest.chest[iCost].iCost, pUser, ENUM_PCRT_PERFORM, addReward, iGoods, iCount, fb);
		//		// 处理消耗
		//		MasterVariableData mvd = {0};
		//		pUser->m_userBase.GetMasterData(mvd);
		//		memset(&mvd, 0, sizeof(mvd));
		//		G_ChangeResType(mvd, addReward.iResType0, addReward.iResNum0);
		//		G_ChangeResType(mvd, addReward.iResType1, addReward.iResNum1);
		//		G_ChangeResType(mvd, addReward.iResType2, addReward.iResNum2);
		//		G_ChangeResType(mvd, addReward.iResType3, addReward.iResNum3);
		//		pUser->m_userBase.AddMastData(mvd, SAVATAG_SEND_PRESENT);
		//		pFUser->m_userSeek.GetPresent(gTmpChest.chest[iCost].iChestID);
		//	}

		//	CMD_G_SeekGetPresent sgp = {0};
		//	for (CT_INT32 iPresent = 0;iPresent < SEEK_PRESENT_ITEM_NUM;++iPresent)
		//	{
		//		sgp.aPresent[iPresent] = pPresent->aPresent[iPresent];
		//	}
		//	pFUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_GET_PRESENT, (CT_CHAR*)&sgp, sizeof(sgp));
		//}
	}
	else 
	{	// 好友不在线，读内存数据
		iNum = CCFightingData::getSingleton().GetPresentNum(sd.dwSeekID);
		if (iNum + i > SEEKED_PRESENT_NUM)
		{
			presentFail.iNum = SEEKED_PRESENT_NUM - iNum;
			enfail = ESG_SEEKED_PRESENT_FULL;
		}
		//else
		//{	// 消耗并发放礼物
		//	for (CT_INT32 iCost = 0;iCost < SEEK_PRESENT_ITEM_NUM;++iCost)
		//	{
		//		if (gTmpChest.chest[iCost].iChestID == 0)
		//		{
		//			break;
		//		}

		//		pUser->m_userSeek.SendPresent(gTmpChest.chest[iCost].iChestID);
		//		vistorInfo.present[iCost] = gTmpChest.chest[iCost].iChestID;
		//		G_PerformCostReward(gTmpChest.chest[iCost].iCost, pUser, ENUM_PCRT_PERFORM, addReward, iGoods, iCount, fb);
		//		// 处理消耗
		//		MasterVariableData mvd = {0};
		//		pUser->m_userBase.GetMasterData(mvd);
		//		memset(&mvd, 0, sizeof(mvd));
		//		G_ChangeResType(mvd, addReward.iResType0, addReward.iResNum0);
		//		G_ChangeResType(mvd, addReward.iResType1, addReward.iResNum1);
		//		G_ChangeResType(mvd, addReward.iResType2, addReward.iResNum2);
		//		G_ChangeResType(mvd, addReward.iResType3, addReward.iResNum3);
		//		pUser->m_userBase.AddMastData(mvd, SAVATAG_SEND_PRESENT);
		//		CCFightingData::getSingleton().GetPresent(sd.dwSeekID, gTmpChest.chest[iCost].iChestID);
		//	}
		//}
	}

	if (enfail != ESP_PRESENT_SUC)
	{
		// 发送客户端
		presentFail.enFail = (ENUM_SeekGivePresentFail)enfail;
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_GIVE_PRESENT_FAIL,(CT_CHAR*)&presentFail,sizeof(presentFail));
		return;
	}

	// 消耗并发放礼物
	for (CT_INT32 iCost = 0;iCost < SEEK_PRESENT_ITEM_NUM;++iCost)
	{
		if (gTmpChest.chest[iCost].iChestID == 0)
		{
			break;
		}

		pUser->m_userSeek.SendPresent(gTmpChest.chest[iCost].iChestID);
		vistorInfo.present[iCost] = gTmpChest.chest[iCost].iChestID;


//		G_PerformCostReward(gTmpChest.chest[iCost].iCost, pUser, ENUM_PCRT_PERFORM, addReward, iGoods, iCount, fb);
		// 处理消耗
		memset(&fb, 0, sizeof(fb));
		memset(&rc, 0, sizeof(rc));
		G_GetCostReward(gTmpChest.chest[iCost].iCost,pUser,ITEM_OP_MODE_EXEC,0,rc,fb);

		for (CT_INT32 k = 0;k < TEAM_RES_CNT;++k)
		{
			if (rc.iUseResType[k] == 0)
			{
				break;
			}
			for (CT_INT32 j = 0;j < TEAM_RES_CNT;++j)
			{
				if (rc.iUseResType[k] == present.iResType[j])
				{
					present.iResCount[j] += rc.iUseResCount[k];
					break;
				}
				if (present.iResType[j] == 0)
				{
					present.iResType[j] = rc.iUseResType[k];
					present.iResCount[j] = rc.iUseResCount[k];
					break;
				}
			}
		}

		/*MasterVariableData mvd = {0};
		pUser->m_userBase.GetMasterData(mvd);
		memset(&mvd, 0, sizeof(mvd));
		G_ChangeResType(mvd, addReward.iResType0, addReward.iResNum0);
		G_ChangeResType(mvd, addReward.iResType1, addReward.iResNum1);
		G_ChangeResType(mvd, addReward.iResType2, addReward.iResNum2);
		G_ChangeResType(mvd, addReward.iResType3, addReward.iResNum3);
		pUser->m_userBase.AddMastData(mvd, SAVATAG_SEND_PRESENT);*/
	}

	if (bOnline)
	{
		for (CT_INT32 iCost = 0;iCost < SEEK_PRESENT_ITEM_NUM;++iCost)
		{
			if (0 == gTmpChest.chest[iCost].iChestID)
			{
				break;
			}
			pFUser->m_userSeek.GetPresent(gTmpChest.chest[iCost].iChestID);
		}
		
		CMD_G_SeekGetPresent sgp = {0};
		for (CT_INT32 iPresent = 0;iPresent < SEEK_PRESENT_ITEM_NUM;++iPresent)
		{
			G_GetCostRewardGroup(pPresent->aPresent[iPresent], &(sgp.aPresent[iPresent]));
		}
		pFUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_GET_PRESENT, (CT_CHAR*)&sgp, sizeof(sgp));
	}
	else
	{
		for (CT_INT32 iCost = 0;iCost < SEEK_PRESENT_ITEM_NUM;++iCost)
		{
			if (0 == gTmpChest.chest[iCost].iChestID)
			{
				break;
			}
			CCFightingData::getSingleton().GetPresent(sd.dwSeekID, gTmpChest.chest[iCost].iChestID);
		}
	}

	// 添加访客记录
	if (ESP_PRESENT_SUC == enfail)
	{
		vistorInfo.seekInfo.iType = SEEK_TYPE_PRESENT;
		for (CT_INT32 iVistor = 0;iVistor < SEEK_PRESENT_ITEM_NUM;++iVistor)
		{
			vistorInfo.present[iVistor] = pPresent->aPresent[iVistor];
		}
		pUser->m_userSeek.SetVistorInfo(vistorInfo);
	}

	// 发送客户端
	
	for (CT_INT32 j = 0;j < TEAM_RES_CNT;++j)
	{
		G_ChangeResType(mvd, present.iResType[j], present.iResCount[j]);
	}
	present.dwUserID = pPresent->dwUserID;
	for (CT_INT32 ipr = 0;ipr < SEEK_PRESENT_ITEM_NUM;++ipr)
	{
		present.aPresent[ipr] = pPresent->aPresent[ipr];
	}
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_GIVE_PRESENT_SUC,(CT_CHAR*)&present,sizeof(CMD_G_SeekGivePresent));
	return;
}

// 生产、升级加速
CT_VOID CCSeekEvent::DisposeBuildingUpSpeed(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_BuildingSpeedUp))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeBuildingUpSpeed iLen != sizeof(CMD_G_BuildingSpeedUp)");
		return;
	}

	CMD_G_BuildingSpeedUp *pSpeedUp = (CMD_G_BuildingSpeedUp *)pBuf;

	CCUser* pUser = 0;
	CT_DWORD dwFUserID = 0;
	CCUser* pFUser = 0;
	UpgradingBuildData upgBuilding = {0};
	CT_INT32 enfail = EBU_SPEEDUP_SUC;
	CT_INT32 iNow = time(0);
	CMD_G_GetSpeedUp speedUpSuc = {0};
	speedUpSuc.iBID = pSpeedUp->iBuildID;
	G_ValueRange vr = {0};
	VistorInfo vistorInfo = {0};
	if (!G_GetValueRange(VALUE_TYPE_SPEEDUP_PRESENT, &vr))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeBuildingUpSpeed G_GetValueRange error.value:%d",VALUE_TYPE_SPEEDUP_PRESENT);
	}

	if (!CCUserManager::getSingleton().Find(pSpeedUp->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeBuildingUpSpeed connot find user,id:%d", pSpeedUp->dwUserID);
		return;
	}

	// 得到当前被访者信息
	dwFUserID = pUser->m_userSeek.GetSeeked();

	// 建筑是否已加速过
	if (!pUser->m_userSeek.CheckBuildingSpeedUp(pSpeedUp->iBuildID))
	{
		enfail = EBU_BUILDING_ERROR;
	}

	// 被访者在线
	if (CCUserManager::getSingleton().Find(dwFUserID, &pFUser))
	{
		// 建筑不在升级状态
		if (!pFUser->m_userBuilding.GetUpgardingBuildingData(pSpeedUp->iBuildID, upgBuilding))
		{
			enfail = EBU_BUILDING_ERROR;
		}
		else
		{
			upgBuilding.llBeginTime -= (upgBuilding.iNeedTime * vr.fMaxVal);
			speedUpSuc.iLastTime = upgBuilding.iNeedTime - (iNow - upgBuilding.llBeginTime);
			if (speedUpSuc.iLastTime < 0)
			{
				speedUpSuc.iLastTime = 0;
			}
			pFUser->m_userBuilding.UpdateUpgradingBuilding(pSpeedUp->iBuildID, upgBuilding);
			pFUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_GET_SPEEDUP, (CT_CHAR*)&speedUpSuc, sizeof(speedUpSuc));
		}
	}
	else
	{
		// 被访者不在线
		if (!CCFightingData::getSingleton().GetUpgradeBuilding(dwFUserID, pSpeedUp->iBuildID, upgBuilding))
		{
			enfail = EBU_BUILDING_ERROR;
		}
		upgBuilding.llBeginTime -= (upgBuilding.iNeedTime * vr.fMaxVal);
		speedUpSuc.iLastTime = upgBuilding.iNeedTime - (iNow - upgBuilding.llBeginTime);
		if (speedUpSuc.iLastTime < 0)
		{
			speedUpSuc.iLastTime = 0;
		}
		CCFightingData::getSingleton().UpdateUpgradeBuilding(dwFUserID, pSpeedUp->iBuildID, upgBuilding);
	}
	
	// 加速失败
	if (enfail != EBU_SPEEDUP_SUC)
	{
		CMD_G_BuildingSpeedUpFail suf;
		suf.enFail = EBU_BUILDING_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_SPEEDUP_PRODUCT_FAIL, (CT_CHAR*)&suf, sizeof(suf));
		return;
	}

	pUser->m_userSeek.SpeedUpBuilding(pSpeedUp->iBuildID);

	// 添加探访记录
	vistorInfo.seekInfo.iType = SEEK_TYPE_UPSPEED;
	vistorInfo.seekInfo.iID[0] = upgBuilding.buildingUpgrade.iBT;
	vistorInfo.seekInfo.iNum[0] = 1;
	pUser->m_userSeek.SetVistorInfo(vistorInfo);

	// 发送客户端
	pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_SPEEDUP_PRODUCT_SUC, (CT_CHAR*)&speedUpSuc, sizeof(speedUpSuc));
	
	return;
}

//城墙加成
CT_VOID CCSeekEvent::DisposeWallAddition(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(CMD_G_SeekWallAdditionUpGradeApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePlunder sizeof(CMD_G_SeekWallAdditionUpGradeApply) != iLen");
		return;
	}

	CMD_G_SeekWallAdditionUpGradeApply *pSwaug=(CMD_G_SeekWallAdditionUpGradeApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pSwaug->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeWallAddition No Find UserID:%d",pSwaug->dwUserID);
		return;
	}

	
	RewardCost rc={0};
	CT_INT32 iFail=WallAdditionValidate(pUser,pSwaug->iID,rc);
	if(iFail != 0)
	{
		CMD_G_SeekWallAdditionUpGradeFail swugf={0};
		swugf.iFail=iFail;
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_WALLADDITION_UPGRADE_FAIL,(CT_CHAR*)&swugf,sizeof(swugf));
		return;
	}

	WallAddition wa={0};
	CMD_G_SeekWallAdditionUpGradeReturn swugr={0};
	
	pUser->m_userSeek.GetWallAddition(pSwaug->iID,wa);
	swugr.iID=pSwaug->iID;
	swugr.iGrade=wa.iGrade;
	memcpy(swugr.iResType,rc.iUseResType,sizeof(swugr.iResType));
	memcpy(swugr.iResCount,rc.iCurrentResCount,sizeof(swugr.iResCount));
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_WALLADDITION_UPGRADE_RETURN,(CT_CHAR*)&swugr,sizeof(swugr));

	return;
}

//掠夺
CT_VOID CCSeekEvent::DisposePlunder(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(CMD_G_SeekPlunderApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePlunder sizeof(CMD_G_SeekPlunderApply) != iLen");
		return;
	}

	CMD_G_SeekPlunderApply *pSpa=(CMD_G_SeekPlunderApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pSpa->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePlunder No Find UserID:%d",pSpa->dwUserID);
		return;
	}

	CMD_G_SeekPlunderFail spf={0};
	spf.iFail=PlunderValidete(pUser,pSpa);
	if(spf.iFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_FAIL,(CT_CHAR*)&spf,sizeof(spf));
		return;
	}

	G_Building buildingInfo = {0};
	MSC_PLUNDER_FIGHT pf={0};
	pf.dwUserID=pUser->GetUserID();
	pf.dwDstUserID=pSpa->dwDstUserID;
	pf.iBulidID=pSpa->iBuildID;
	pf.iSeed=rand();
	G_GetFightingData(pUser,pf.LeftFightingData[0],CT_TRUE);
	G_GetBuildingData(pSpa->iBuildID, &buildingInfo);
	CCFightingData::getSingleton().GetFightingData(pSpa->dwDstUserID,pf.RightFightingData[0],CT_TRUE);

	if(buildingInfo.iBT == CITY_TYPE_FARMLAND || buildingInfo.iBT == CITY_TYPE_PANNING)
	{
		pf.iWin=WINER_LIFT;
		DisposePlunderFight((CT_CHAR*)&pf,sizeof(pf));
	}
	else if(buildingInfo.iBT == CITY_TYPE_TOTEM)
	{
		pUser->m_userArmy.DeleteBoss(CT_TRUE);
		G_SendFightThreadInfo(pUser->GetUserID(),MSC_MAIN_FIGHTING_EVENT,MSC_SUB_PLUNDER_FIGHT,(CT_CHAR*)&pf,sizeof(pf));
	}

	return;
}

//拔掉旗帜
CT_VOID CCSeekEvent::DisposePunderFlag(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(CMD_G_SeekPlunderFlagApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePunderFlag sizeof(CMD_G_SeekPlunderFlagApply) != iLen");
		return;
	}

	CMD_G_SeekPlunderFlagApply *pSpfa=(CMD_G_SeekPlunderFlagApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pSpfa->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePunderFlag No Find UserID:%d",pSpfa->dwUserID);
		return;
	}

	CMD_G_SeekPlunderFlagReturn spfr={0};
	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_PLUNDER_FLAG_REWARD,&vr))
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePunderFlag G_GetValueRange VALUE_TYPE_PLUNDER_FLAG_REWARD");
	}

	MasterVariableData mvd={0};
	mvd.stRank.iCurrentExp=vr.fMaxVal*pUser->m_userSeek.GetFlagCount();
	pUser->m_userBase.AddMastData(mvd,SAVATAG_SEEK_FALGS);

	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);
	spfr.iResType[0]=VALUE_TYPE_RANKXP;
	spfr.iResCount[0]=mvd.stRank.iCurrentExp;
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_FLAG_RETURN,(CT_CHAR*)&spfr,sizeof(spfr));

	pUser->m_userSeek.PullTeamFlag(0,SEEK_FLAG_TOTEM);
	return;
}

//绑票请求
CT_VOID CCSeekEvent::DisposeKidnap(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(CMD_G_SeekKidnapApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeKidnap sizeof(CMD_G_SeekKidnapApply) != iLen");
		return;
	}

	CMD_G_SeekKidnapApply *pSka=(CMD_G_SeekKidnapApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pSka->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeKidnap No Find UserID:%d",pSka->dwUserID);
		return;
	}

	CMD_G_KidnapFail kf={0};
	kf.iFail=KidnapValidate(pUser,pSka);
	if(kf.iFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_KIDNAP_FAIL,(CT_CHAR*)&kf,sizeof(kf));
		return;
	}

	MSC_KIDNAP_FIGHT kfight={0};
	kfight.dwUserID=pUser->GetUserID();
	kfight.iType=pSka->iType;
	kfight.iSeed=rand();
	kfight.iPetChange=pSka->iPetChange;
	G_GetFightingData(pUser,kfight.LeftFightingData[0],CT_TRUE);
	CCFightingData::getSingleton().GetFightingData(pSka->dwDstUserID,kfight.RightFightingData[0],CT_TRUE);
	if(pSka->iType == KIDNAP_WOMAN || pSka->iType == KIDNAP_PET)
	{
		pUser->m_userArmy.DeleteBoss(CT_TRUE);
		G_SendFightThreadInfo(pUser->GetUserID(),MSC_MAIN_FIGHTING_EVENT,MSC_SUB_KIDNAP_FIGHT,(CT_CHAR*)&kfight,sizeof(kfight));
		return;
	}
	else if(pSka->iType == KIDNAP_FARMER)
	{
		kfight.iWin=WINER_LIFT;
		DisposeKidnapFight((CT_CHAR*)&kfight,sizeof(kfight));
		return;
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeKidnap No Find Type:%d",pSka->iType);
		return;
	}

	return;
}

//营救请求
CT_VOID CCSeekEvent::DisposeRescue(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(CMD_G_SeekRescurApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeRescue sizeof(CMD_G_SeekRescurApply) != iLen");
		return;
	}

	CMD_G_SeekRescurApply *pSra=(CMD_G_SeekRescurApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pSra->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeRescue No Find UserID:%d",pSra->dwUserID);
		return;
	}

	CMD_G_RescueFail rf={0};
	rf.iFail=RescueValidate(pUser,pSra);
	if(rf.iFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_RESCUE_FAIL,(CT_CHAR*)&rf,sizeof(rf));
		return;
	}

	MSC_RESCUE_FIGHT rescuefight={0};
	FightingData fd={0};
	rescuefight.dwUserID=pSra->dwUserID;
	rescuefight.iSeed=rand();
	G_GetFightingData(pUser,rescuefight.LeftFightingData[0],CT_TRUE);
	CCFightingData::getSingleton().GetFightingData(pSra->dwDstUserID,rescuefight.RightFightingData[0],CT_TRUE);
	
	if(pSra->iType == RESCUE_MONEY)
	{
		rescuefight.iWin=WINER_LIFT;
		DisposeRescueFight((CT_CHAR*)&rescuefight,sizeof(rescuefight));
	}
	else if(pSra->iType == RESCUE_FIGHT)
	{
		pUser->m_userArmy.DeleteBoss(CT_TRUE);
		G_SendFightThreadInfo(pUser->GetUserID(),MSC_MAIN_FIGHTING_EVENT,MSC_SUB_RESCUE_FIGHT,(CT_CHAR*)&rescuefight,sizeof(rescuefight));
	}
	else
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposeRescue pSra->iType:%d",pSra->iType);
	}
	return;
}


//城墙军队
CT_VOID CCSeekEvent::DisposeArmyApply(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(CMD_G_SeekArmyApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeArmyApply sizeof(CMD_G_SeekArmyApply) != iLen");
		return;
	}

	CMD_G_SeekArmyApply *pSra=(CMD_G_SeekArmyApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pSra->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeArmyApply No Find UserID:%d",pSra->dwUserID);
		return;
	}

	CMD_G_SeekArmyFail saf={0};
	saf.iFail=WallArmyValidate(pUser,pSra->iPos);
	if(saf.iFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_ARMY_FAIL,(CT_CHAR*)&saf,sizeof(saf));
		return;
	}

	CMD_G_SeekArmyInfo sai={0};
	memcpy(sai.iPos,pSra->iPos,sizeof(sai.iPos));
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_ARMY_INFO,(CT_CHAR*)&sai,sizeof(sai));

	// 判断城墙任务
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_WALL_ARMY;
	taskSysInfo.iProgress = 1;
	pUser->m_userTask.DisposeTask(taskSysInfo);
	return;
}

//绑票战果
CT_VOID CCSeekEvent::DisposeKidnapFight(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(MSC_KIDNAP_FIGHT) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeKidnapFight sizeof(MSC_KIDNAP_FIGHT) != iLen");
		return; 
	}

	MSC_KIDNAP_FIGHT *pKf=(MSC_KIDNAP_FIGHT*)pBuf;
	CMD_G_KidnapReturn kr={0};
	VistorInfo visterInfo = {0};
	visterInfo.bIsKidnap = CT_FALSE;
	kr.iPetChange=pKf->iPetChange;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pKf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposeKidnapFight No Find UserID:%d",pKf->dwUserID);
		return;
	}
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pUser->GetUserID());
	if(CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		pFT->GetFightTeamName(kr.cTeamName);
	}
	kr.iIsWin=pKf->iWin;
	kr.iType=pKf->iType;
	kr.dwDstUserID=pKf->RightFightingData[0].dwUserID;

	if(pKf->iType == KIDNAP_FARMER)
	{
		G_ValueRange vr={0};
		if(G_GetValueRange(VALUE_TYPE_PLUNDER_FARMER,&vr))
		{
			CT_INT32 iRand=rand()%100;
			if(vr.fMaxVal*100 >= iRand)
			{
				kr.iIsWin=WINER_LIFT;
			}
			else
			{
				kr.iIsWin=WINER_RIGHT;
				pKf->iWin=WINER_RIGHT;
			}
		}
		else
		{
			kr.iIsWin=WINER_RIGHT;
			pKf->iWin=WINER_RIGHT;
			CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposeKidnapFight G_GetValueRange(VALUE_TYPE_PLUNDER_FARMER,&vr)");
		}
	}
	
	if(pKf->iWin == WINER_LIFT)
	{
		KidnapInfo ki={0};
		ki.Num=1;
		ki.iPetChange=pKf->iPetChange;
		ki.owerUserId=pKf->RightFightingData[0].dwUserID;
		pUser->m_userSeek.InsertHostages(pKf->iType,ki);

		//广播被掠夺用户
		CCUser *pDstUser=0;
		if(CCUserManager::getSingleton().Find(pKf->RightFightingData[0].dwUserID,&pDstUser))
		{
			CMD_G_KidnapData kd={0};
			MasterData md={0};
			pUser->m_userBase.GetMasterData(md);
			kd.iType=pKf->iType;
			kd.iNum=1;
			kd.iPetChange=pKf->iPetChange;
			memcpy(kd.cUserName,md.cNickName,NAME_SIZE);
			memcpy(kd.cTeamName,kr.cTeamName,NAME_SIZE);
			pDstUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_KINNAP_DATA,(CT_CHAR*)&kd,sizeof(kd));

			//设置绑票和被绑票信息
			Seekers si={0};
			SeekData sd={0};
			pDstUser->m_userSeek.GetSeekData(sd);
			if(pKf->iType == KIDNAP_WOMAN)
			{
				sd.iWomanNum+=1;
				visterInfo.seekInfo.iNum[0] = sd.iWomanNum;
			}
			else if(pKf->iType == KIDNAP_PET)
			{
				for(CT_INT32 i=0;i<SEEK_KIDNAP_PET;++i)
				{
					if(sd.iPet[i] == 0)
					{
						sd.iPet[i]=pKf->iPetChange;
						break;
					}
				}
				visterInfo.seekInfo.iNum[0] = pKf->iPetChange;
			}
			else
			{
				sd.iFarmarNum+=1;
				visterInfo.seekInfo.iNum[0] = sd.iFarmarNum;
			}

			pDstUser->m_userSeek.UpdateSeekData(sd);
		}
		else
		{
			Seekers si={0};
			if(CCFightingData::getSingleton().GetSeekedInfo(pKf->RightFightingData[0].dwUserID,si))
			{
				if(pKf->iType == KIDNAP_WOMAN)
				{
					si.iKidnapSaintess+=1;
					visterInfo.seekInfo.iNum[0] = si.iKidnapSaintess;
				}
				else if(pKf->iType == KIDNAP_PET)
				{
					for(CT_INT32 i=0;i<SEEK_KIDNAP_PET;++i)
					{
						if(si.iPetKidnap[i] == 0)
						{
							si.iPetKidnap[i]=pKf->iPetChange;
							break;
						}
					}
					visterInfo.seekInfo.iNum[0]=pKf->iPetChange;
				}
				else
				{
					si.iKidnapWorkerNum+=1;
					visterInfo.seekInfo.iNum[0] = si.iKidnapWorkerNum;
				}
				CCFightingData::getSingleton().UpdateSeekersBaseInfo(pKf->RightFightingData[0].dwUserID,si);
			}
		}

		// 更新领主人质
		pUser->m_userSeek.UpdateKidnap(pKf->iType, ki);

		visterInfo.bIsKidnap = CT_TRUE;

		// 读取掠夺人质的积分
		G_ValueRange vr = {0};
		if (!G_GetValueRange(VALUE_TYPE_PLUNDER_SCORE_SAINTESS + pKf->iType - 1, &vr))
		{
			CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeKidnapFight G_GetValueRange  plunderscore error.");
		}
		
		//更新掠夺资源
		MSC_DB_PlunderRes mdPlunder = {0};
		mdPlunder.dwUserID = pUser->GetUserID();
		mdPlunder.iPlunderScore = vr.fMinVal;
		G_SendDBThreadInfo(pUser->GetUserID(), MSC_MAIN_DB_SEEK, MSC_SUB_DB_PLUNDER_RES_UPDATE, (CT_CHAR*)&mdPlunder, sizeof(mdPlunder));
	}
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_KINNAP_RETURN,(CT_CHAR*)&kr,sizeof(kr));

	// 添加探访记录
	visterInfo.seekInfo.iType = SEEK_TYPE_KIDNAP;
	visterInfo.seekInfo.iID[0] = pKf->iType;
	pUser->m_userSeek.SetVistorInfo(visterInfo);

	//添加战报
	if(pKf->iType == KIDNAP_WOMAN)
	{
		CCUser *pDstUser=0;
		SeekFightingData sfd={0};
		TeamSeek ts={0};
		time_t t = time(0);
		tm* pTm = localtime(&t);
		ts.dwUserID=pKf->dwUserID;
		ts.iFightResult=pKf->iWin;
		ts.iType=SEEK_FLAG_SAINTESS;
		memcpy(ts.cTName,kr.cTeamName,NAME_SIZE);
				
		sfd.dwUserID=pKf->RightFightingData[0].dwUserID;
		sfd.iSeed=pKf->iSeed;
		sfd.iType=SEEK_TYPE_KIDNAP + pTm->tm_wday * 100;
		memcpy(&sfd.LeftFightingData,&pKf->LeftFightingData[0],sizeof(sfd.LeftFightingData));
		memcpy(&sfd.RightFightingData,&pKf->RightFightingData[0],sizeof(sfd.RightFightingData));
		MD5FightIndex(sfd.ucIndex,pKf->LeftFightingData[0].dwUserID,pKf->RightFightingData[0].dwUserID,sfd.iType);
		if(CCUserManager::getSingleton().Find(pKf->RightFightingData[0].dwUserID,&pDstUser))
		{
			pDstUser->m_userSeek.InsertFightingData(sfd,CT_TRUE);
			pDstUser->m_userSeek.InsertTeamFlag(ts);
		}
		else
		{
			CCFightingData::getSingleton().InsertTeamFlag(pKf->RightFightingData[0].dwUserID,ts);
			//数据库保存
			G_SendDBThreadInfo(sfd.dwUserID,MSC_MAIN_DB_SEEK,MSC_SUB_DB_SEEK_FIGHTING_RECORD,(CT_CHAR*)&sfd,sizeof(sfd));
		}
	}

	return;
}

//营救战果
CT_VOID CCSeekEvent::DisposeRescueFight(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(MSC_RESCUE_FIGHT) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeRescueFight sizeof(MSC_RESCUE_FIGHT) != iLen");
		return; 
	}

	MSC_RESCUE_FIGHT *pKf=(MSC_RESCUE_FIGHT*)pBuf;
	CMD_G_RescueReturn rr={0};
	CMD_G_UpdateVisitorInfo uvi = {0};
	VistorInfo oppositeVi = {0};
	CCUser* pUserOpposite = 0;
	CCUser *pUser=0;
	MasterData md = {0};
	time_t t = time(0);
	tm* pTm = localtime(&t);
	if(!CCUserManager::getSingleton().Find(pKf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposeRescueFight No Find UserID:%d",pKf->dwUserID);
		return;
	}
	rr.dwDstUserID=pKf->RightFightingData[0].dwUserID;
	rr.iIsSuc=pKf->iWin;

	if(pKf->iWin == WINER_LIFT)
	{
		VistorInfo vi={0};
		pUser->m_userSeek.GetVistorInfo(SEEK_TYPE_KIDNAP,pKf->RightFightingData[0].dwUserID,vi);
		SeekData sd={0};
		MasterVariableData mvd={0};
		pUser->m_userBase.GetMasterData(mvd);
		pUser->m_userSeek.GetSeekData(sd);
		rr.iWomanNum=abs(sd.iWomanNum);
		rr.iFarmarNum=abs(sd.iFarmarNum);
		rr.iGold=mvd.stRes.iGold;
		rr.iFreeFarmer=mvd.stRes.iWorker;

		//清楚人质
		for(CT_INT32 i=0;i<TEAM_RES_CNT;++i)
		{
			if(vi.seekInfo.iID[i] == KIDNAP_FARMER)
				sd.iFarmarNum-=vi.seekInfo.iNum[i];
			else if(vi.seekInfo.iID[i] == KIDNAP_PET)
			{
				for(CT_INT32 j=0;j<SEEK_KIDNAP_PET;++j)
				{
					if(vi.seekInfo.iNum[i] == sd.iPet[j])
					{
						rr.iPetChange=sd.iPet[j];
						sd.iPet[i]=0;
						break;
					}
				}
			}
			else
				sd.iWomanNum-=vi.seekInfo.iNum[i];

			// 读取掠夺人质的积分
			G_ValueRange vr = {0};
			if (!G_GetValueRange(VALUE_TYPE_PLUNDER_SCORE_SAINTESS + vi.seekInfo.iID[i] - 1, &vr))
			{
				CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeKidnapFight G_GetValueRange  plunderscore error.");
			}

			//更新掠夺资源
			MSC_DB_PlunderRes mdPlunder = {0};
			mdPlunder.dwUserID = pUser->GetUserID();
			mdPlunder.iPlunderScore = vr.fMinVal;
			G_SendDBThreadInfo(pUser->GetUserID(), MSC_MAIN_DB_SEEK, MSC_SUB_DB_PLUNDER_RES_UPDATE, (CT_CHAR*)&mdPlunder, sizeof(mdPlunder));
		}

		vi.bIsKidnap=CT_FALSE;
		pUser->m_userSeek.UpdateSeekData(sd);
		pUser->m_userSeek.UpdateSeekVisitorInfo(SEEK_TYPE_KIDNAP,vi);
		pUser->m_userSeek.PullTeamFlag(pKf->RightFightingData[0].dwUserID,SEEK_FLAG_SAINTESS);

		//通知掠夺玩家
		CMD_G_RescueSuc rs={0};
		CCUser *pDstUser=0;
		rs.dwDstUserID=pKf->dwUserID;
		rs.iPetChange=rr.iPetChange;
		if(CCUserManager::getSingleton().Find(pKf->RightFightingData[0].dwUserID,&pDstUser))
		{
			MasterVariableData DstMvd={0};
			pDstUser->m_userBase.GetMasterData(DstMvd);
			rs.iGold=DstMvd.stRes.iGold;
			pDstUser->m_userSeek.ReleaseHostages(pUser->GetUserID());
			pDstUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_RESCUE_SUC,(CT_CHAR*)&rs,sizeof(rs));
		}
		else
		{
			// 更新到数据库
			MSC_DB_SeekKidnap sk = {0};
			sk.dwUserID = pKf->RightFightingData[0].dwUserID;
			sk.kidnap.owerUserId = pUser->GetUserID();
			G_SendDBThreadInfo(pUser->GetUserID(),  MSC_MAIN_DB_SEEK, MSC_SUB_KIDNAP_DELETE, (CT_CHAR*) &sk, sizeof(sk));
		}
	}

	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_RESCUE_RETURN,(CT_CHAR*)&rr,sizeof(rr));

	// 更新绑架者的营救信息
	pUser->m_userBase.GetMasterData(md);
	oppositeVi.dwUserID = pKf->dwUserID;
	oppositeVi.seekInfo.iType = SEEK_TYPE_RESCUE;
	oppositeVi.seekInfo.iNum[0] = pKf->iWin;
	strncpy(oppositeVi.cUserName, md.cNickName, sizeof(oppositeVi.cUserName));
	if (CCUserManager::getSingleton().Find(pKf->RightFightingData[0].dwUserID, &pUserOpposite))
	{
		pUserOpposite->m_userSeek.AddVistorInfo(oppositeVi);

		// 发送客户端
		uvi.vi = oppositeVi;
		pUserOpposite->m_userSeek.GetVisitorDes(uvi.vi, uvi.cInfo, sizeof(uvi.cInfo));
		if (strlen(uvi.cInfo) != 0)
		{
			pUserOpposite->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_FRIEND_RECORD_UPDATE, (CT_CHAR*)&uvi, sizeof(uvi));
		}
	}

	MSC_DB_SeekVistorInfo svi = {0};
	svi.dwUserID = pKf->RightFightingData[0].dwUserID;
	svi.vistorInfo = oppositeVi;
	svi.vistorInfo.seekInfo.iType += pTm->tm_wday * 100;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + pKf->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID, MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_VISTORINFO, (CT_CHAR*)&svi, sizeof(svi));

	CT_INT32 threadid =  GAME_THREAD_LOGDATABASE + pKf->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,  MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SEEKDATA, (CT_CHAR*)&svi, sizeof(svi));

	return;
}

//掠夺结果
CT_VOID CCSeekEvent::DisposePlunderFight(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if(sizeof(MSC_PLUNDER_FIGHT) != iLen)
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunderFight sizeof(MSC_PLUNDER_FIGHT) != iLen");
		return;
	}

	MSC_PLUNDER_FIGHT *pPf=(MSC_PLUNDER_FIGHT*)pBuf;
	CCUser *pUser=0;
	CCUser *pDstUser=0;
	Seekers seekers={0};
	G_Building    buildingInfo = {0};
	if(!CCUserManager::getSingleton().Find(pPf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunder No Find UserID:%d",pPf->dwUserID);
		return;
	}
	if(!CCUserManager::getSingleton().Find(pPf->RightFightingData[0].dwUserID,&pDstUser))
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunderFight No Find dwDstUserID:%d",pPf->RightFightingData[0].dwUserID);
	}
	if (!G_GetBuildingData(pPf->iBulidID, &buildingInfo))
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunderFight G_GetBuildingData iBuildID:%d",pPf->iBulidID);
	}
	if(!CCFightingData::getSingleton().GetSeekedInfo(pPf->RightFightingData[0].dwUserID,seekers))
	{
		CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunderFight GetSeekedInfo dwDstUserID:%d",pPf->RightFightingData[0].dwUserID);
	}

	VistorInfo vistorInfo = {0};
	CMD_G_SeekPlunderReturn spr={0};
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pUser->GetUserID());
	if(CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		pFT->GetFightTeamName(spr.cTeamName);
	}

	spr.iIsWin=pPf->iWin;
	spr.iBuildId=pPf->iBulidID;
	spr.dwUserID=pUser->GetUserID();

	if(pPf->iWin == WINER_LIFT)
	{
		G_ValueRange vrpr={0};
		if(!G_GetValueRange(VALUE_TYPE_PLUNDER_REWARD_WIN,&vrpr))
		{
			CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunderFight G_GetValueRange VALUE_TYPE_PLUNDER_REWARD_WIN");
		}
		G_ValueRange vrpp={0};
		if(!G_GetValueRange(VALUE_TYPE_PLUNDER_PROPORTION,&vrpp))
		{
			CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunderFight G_GetValueRange VALUE_TYPE_PLUNDER_PROPORTION");
		}


		//掠夺资源
		MSC_LOGDB_Plunder logdbP={0};
		MasterVariableData mvd={0};
		if(pDstUser != NULL)
		{
			pDstUser->m_userBase.GetMasterData(mvd);

			seekers.iFood=mvd.stRes.iFood;
			seekers.iMoney=mvd.stRes.iGold;
		}

		memset(&mvd,0,sizeof(mvd));
		if(buildingInfo.iBT == CITY_TYPE_FARMLAND)
		{
			mvd.stRes.iFood=seekers.iFood*vrpp.fMaxVal*m_fFarmland;
			mvd.stRes.iFood=SeekValueCorrection(VALUE_TYPE_SEEK_FARMLAND,mvd.stRes.iFood);
		}
		else if(buildingInfo.iBT == CITY_TYPE_PANNING)
		{
			mvd.stRes.iGold=seekers.iMoney*vrpp.fMaxVal*m_fPanning;
			mvd.stRes.iGold=SeekValueCorrection(VALUE_TYPE_SEEK_PANNING,mvd.stRes.iGold);
		}
		else
		{
			mvd.stRes.iFood=seekers.iFood*vrpp.fMaxVal*m_Totem.fFood;
			mvd.stRes.iGold=seekers.iMoney*vrpp.fMaxVal*m_Totem.fGold;

			mvd.stRes.iFood=SeekValueCorrection(VALUE_TYPE_SEEK_TAMEDRAGON_FOOD,mvd.stRes.iFood);
			mvd.stRes.iGold=SeekValueCorrection(VALUE_TYPE_SEEK_TAMEDRAGON_GOLD,mvd.stRes.iGold);
		}
		logdbP.iFood=mvd.stRes.iFood;
		logdbP.iGold=mvd.stRes.iGold;

		pUser->m_userBase.AddMastData(mvd,SAVATAG_SEEK_PLUNDER);

		if(pDstUser != NULL)
		{
			mvd.stRes.iFood=-mvd.stRes.iFood;
			mvd.stRes.iGold=-mvd.stRes.iGold;
			pDstUser->m_userBase.AddMastData(mvd,SAVATAG_SEEK_PLUNDER);

			MasterVariableData Dstmvd={0};
			pDstUser->m_userBase.GetMasterData(Dstmvd);
			spr.iResType[0]=VALUE_TYPE_GOLD;
			spr.iResType[1]=VALUE_TYPE_FOOD;
			spr.iResCount[0]=Dstmvd.stRes.iGold;
			spr.iResCount[1]=Dstmvd.stRes.iFood;
			spr.iResType[2]=0;
			spr.iResCount[2]=0;

			pDstUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_SUC,(CT_CHAR*)&spr,sizeof(spr));
		}
		else
		{
			seekers.iFood+=-mvd.stRes.iFood;
			seekers.iMoney+=-mvd.stRes.iGold;
			CCFightingData::getSingleton().UpdateSeekersBaseInfo(pPf->dwDstUserID,seekers);
		}

		//掠夺记录
		vistorInfo.seekInfo.iID[0] = VALUE_TYPE_GOLD;
		vistorInfo.seekInfo.iNum[0] = abs(mvd.stRes.iGold);
		vistorInfo.seekInfo.iID[1] = VALUE_TYPE_FOOD;
		vistorInfo.seekInfo.iNum[1] = abs(mvd.stRes.iFood);

		//声望奖励
		RewardCost rc={0};
		FakeProBase fb={0};
		//memset(&mvd,0,sizeof(mvd));
		//mvd.stRank.iCurrentExp=vrpr.fMaxVal;
		//pUser->m_userBase.AddMastData(mvd,SAVATAG_SEEK_PLUNDER);
		if(buildingInfo.iBT == CITY_TYPE_TOTEM)
			G_GetCostReward(vrpr.fMaxVal,pUser,ITEM_OP_MODE_CKEXE,SAVATAG_SEEK_PLUNDER,rc,fb);
		pUser->UpdateAndSendMasterData();

		//vistorInfo.seekInfo.iID[2] = VALUE_TYPE_RANKXP + 1; // 客户端解析公告时12是声望
		//vistorInfo.seekInfo.iNum[2] = vrpr.fMaxVal;

		memset(&mvd,0,sizeof(mvd));
		pUser->m_userBase.GetMasterData(mvd);
		spr.iResType[0]=VALUE_TYPE_GOLD;
		spr.iResType[1]=VALUE_TYPE_FOOD;
		spr.iResType[2]=VALUE_TYPE_RANKXP;
		spr.iResCount[0]=mvd.stRes.iGold;
		spr.iResCount[1]=mvd.stRes.iFood;
		spr.iResCount[2]=mvd.stRank.iCurrentExp;

		//记录掠夺次数
		BuildData bd={0};
		bd.iPlunderNum+=1;
		bd.bd.iID=pPf->iBulidID;
		pUser->m_userSeek.SetBuildData(pPf->iBulidID,bd);

		//更新掠夺资源
		MSC_DB_PlunderRes mdPlunder = {0};
		mdPlunder.dwUserID = pUser->GetUserID();
		mdPlunder.iPlunderScore = vistorInfo.seekInfo.iNum[0] + vistorInfo.seekInfo.iNum[1];
		G_SendDBThreadInfo(pUser->GetUserID(), MSC_MAIN_DB_SEEK, MSC_SUB_DB_PLUNDER_RES_UPDATE, (CT_CHAR*)&mdPlunder, sizeof(mdPlunder));

		//日志记录
		logdbP.dwUserID=pUser->GetUserID();
		logdbP.dwDstUserID=pPf->RightFightingData[0].dwUserID;
		G_SendLogDBThreadInfo(pUser->GetUserID(),MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PLUNDER,(CT_CHAR*)&logdbP,sizeof(logdbP));
	}
#if 0 //作废
	else
	{
		G_ValueRange vrpr={0};
		if(!G_GetValueRange(VALUE_TYPE_PLUNDER_REWARD_LOST,&vrpr))
		{
			CCLogMeg::getSingleton().WriteDebug("CCSeekEvent::DisposePlunderFight G_GetValueRange VALUE_TYPE_PLUNDER_REWARD_LOST");
		}

		RewardCost rc={0};
		FakeProBase fb={0};
		if(vrpr.fMaxVal != 0)
			G_GetCostReward(vrpr.fMaxVal,pUser,ITEM_OP_MODE_CKEXE,SAVATAG_SEEK_PLUNDER,rc,fb);
	}
#endif
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_RETURN,(CT_CHAR*)&spr,sizeof(spr));

	// 增加探访记录
	vistorInfo.seekInfo.iType = SEEK_TYPE_PLUNDER;
	vistorInfo.iPlunderBID[0] = pPf->iBulidID;
	pUser->m_userSeek.SetVistorInfo(vistorInfo);

	//添加战报
	SeekFightingData sfd={0};
	time_t t = time(0);
	tm* pTm = localtime(&t);
	sfd.dwUserID=pPf->LeftFightingData[0].dwUserID;
	sfd.iSeed=pPf->dwUserID;
	sfd.iType=SEEK_TYPE_PLUNDER + pTm->tm_wday * 100;
	memcpy(&sfd.LeftFightingData,&pPf->LeftFightingData[0],sizeof(sfd.LeftFightingData));
	memcpy(&sfd.RightFightingData,&pPf->RightFightingData[0],sizeof(sfd.RightFightingData));

	TeamSeek ts={0};
	ts.dwUserID=pUser->GetUserID();
	ts.iFightResult=pPf->iWin;
	ts.iType=SEEK_FLAG_TOTEM;
	memcpy(ts.cTName,spr.cTeamName,NAME_SIZE);
	MD5FightIndex(sfd.ucIndex,sfd.LeftFightingData.dwUserID,sfd.RightFightingData.dwUserID,sfd.iType);
	if(pDstUser != NULL)
	{
		if(buildingInfo.iBT == CITY_TYPE_TOTEM)
			pDstUser->m_userSeek.InsertTeamFlag(ts);
		pDstUser->m_userSeek.InsertFightingData(sfd,CT_TRUE);
	}
	else
	{
		if(buildingInfo.iBT == CITY_TYPE_TOTEM)
			CCFightingData::getSingleton().InsertTeamFlag(pPf->dwDstUserID,ts);

		//数据库保存
		G_SendDBThreadInfo(pPf->dwUserID,MSC_MAIN_DB_SEEK,MSC_SUB_DB_SEEK_FIGHTING_RECORD,(CT_CHAR*)&sfd,sizeof(sfd));
	}
	return;
}

//城墙技能升级验证
CT_INT32 CCSeekEvent::WallAdditionValidate(CCUser *pUser,const CT_INT32 iID,RewardCost &rc)
{
	WallAddition wa={0};
	G_SeekWallAddition swa={0};
	if(!pUser->m_userSeek.GetWallAddition(iID,wa))
	{
		return WALLADDITION_DATA;
	}

	if(!G_GetSeekWallAddition(wa.iID,wa.iGrade+1,&swa))
	{
		return WALLADDITION_DATA;
	}

	
	FakeProBase fb={0};
	if(G_GetCostReward(swa.iCostGroup,pUser,ITEM_OP_MODE_CHECK,SAVETAG_SEEK_WALLADDITION,rc,fb) != 0)
	{
		return WALLADDITION_COST;
	}

	G_GetCostReward(swa.iCostGroup,pUser,ITEM_OP_MODE_EXEC,SAVETAG_SEEK_WALLADDITION,rc,fb);
	pUser->m_userSeek.AddWallAddition(iID);

	//日志记录
	MSC_LOGDB_WallAddition logwa={0};
	logwa.dwUserID=pUser->GetUserID();
	memcpy(logwa.iResID,rc.iUseResType,sizeof(logwa.iResID));
	memcpy(logwa.iResCount,rc.iUseResCount,sizeof(logwa.iResCount));
	G_SendLogDBThreadInfo(pUser->GetUserID(),MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_WALLADDITION,(CT_CHAR*)&logwa,sizeof(logwa));
	return 0;
}

// 进入他人城镇
CT_VOID CCSeekEvent::DisposeEnterOthersTown(CT_CHAR* pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_EnterTown_Others))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeEnterOthersTown iLen != sizeof(CMD_G_EnterTown_Others)");
		return;
	}

	CMD_G_EnterTown_Others* pOthers = (CMD_G_EnterTown_Others*)pBuf;
	
	CCUser* pUser = NULL;
	CCUser* pFUser = NULL;
	if (!CCUserManager::getSingleton().Find(pOthers->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeEnterOthersTown connot find user,uid:%d", pOthers->dwUserID);
		return;
	}

	if (pOthers->dwUserID == pOthers->dwFUserID)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_ENTER_TOWN_FAIL);
		return;
	}

	if (CCUserManager::getSingleton().Find(pOthers->dwFUserID, &pFUser))
	{
		CMD_G_EnterTown_Others_Suc otherTown = {0};
		MasterData md = {0};
		SeekData sd = {0};
		CT_INT32 iBufLen = 0;
		CT_INT32 iPetChangeID[SEEK_KIDNAP_PET] = {0};
		CT_INT32 iSize = sizeof(iPetChangeID);
		pFUser->m_userBase.GetMasterData(md);
		pFUser->m_userSeek.GetSeekData(sd);

		// 基础信息
		otherTown.dwUserID = pOthers->dwFUserID;
		strncpy(otherTown.cUserName, md.cNickName, sizeof(otherTown.cUserName));
		otherTown.iOccupation = md.iOccupation;
		otherTown.iLevel = md.mvd.stGrade.iLevel;
		otherTown.iDiamond = md.mvd.stRes.iDiamod;
		otherTown.iMoney = md.mvd.stRes.iGold;
		otherTown.iFood = md.mvd.stRes.iFood;
		otherTown.iWorker = md.mvd.stRes.iWorker;
		otherTown.iMaxWorker = md.mvd.stRes.iMaxWorker;
		otherTown.iStamina = md.mvd.stRes.iStamina;
		otherTown.iVip = pUser->m_userVip.VipGradeGet();
		otherTown.iFightingPower = md.mvd.stRes.iFightingPower;
		otherTown.iSaintessNum = sd.iWomanNum;

		// 建筑信息
		iBufLen = sizeof(otherTown.building);
		pFUser->m_userBuilding.GetBuildingData((CT_CHAR*)&otherTown.building,iBufLen);

		iBufLen = sizeof(otherTown.upgradeBuilding);
		pFUser->m_userBuilding.GetUpgardingBuildingData((CT_CHAR*)&otherTown.upgradeBuilding,iBufLen);

		iBufLen = sizeof(otherTown.productBuilding);
		pFUser->m_userBuilding.GetProductingData((CT_CHAR*)&otherTown.productBuilding,iBufLen);

		// 战旗
		TeamSeek pFlagBuf[1024] = {0};
		iBufLen = sizeof(otherTown.aTeam);
		pFUser->m_userSeek.GetTeamFlag((CT_CHAR*)pFlagBuf, iBufLen);
		for (CT_INT32 iFlag = 0;iFlag < iBufLen / sizeof(TeamSeek);++iFlag)
		{
			otherTown.aTeam[iFlag].dwFUserID = pFlagBuf[iFlag].dwUserID;
			otherTown.aTeam[iFlag].iType = pFlagBuf[iFlag].iType;
			otherTown.aTeam[iFlag].iFightResult = pFlagBuf[iFlag].iFightResult;
			strncpy(otherTown.aTeam[iFlag].cTName, pFlagBuf[iFlag].cTName, NAME_SIZE);
		}

		// 绑票圣女
		iBufLen = sizeof(otherTown.aSaintessInfo);
		pFUser->m_userSeek.GetKidnapGirls((CT_CHAR*)otherTown.aSaintessInfo, iBufLen);

		// 绑票工人
		iBufLen = sizeof(otherTown.aFarmerInfo);
		pFUser->m_userSeek.GetKidnapWorkers((CT_CHAR*)otherTown.aFarmerInfo, iBufLen);

		// 绑票宠物
		iBufLen = sizeof(otherTown.aPetInfo);
		pFUser->m_userSeek.GetKidnapPets((CT_CHAR*)otherTown.aPetInfo, iBufLen);

		// 开启功能
		CMD_G_FUNCTIONOPEN aFunction[100] = {0};
		CT_INT32 iIndex = 0;
		CT_INT32 iFunctionLen = sizeof(aFunction);
		CT_BOOL bBusinessman = false;
		CT_BOOL bSaintess = false;
		CT_BOOL bIronSmith = false;
		pFUser->GetFunctionOpen((CT_CHAR*)&aFunction, iFunctionLen);
		for (CT_INT32 i = 0;i < iLen / sizeof(CMD_G_FUNCTIONOPEN);++i)
		{
			if (aFunction[i].iType == FUNCTION_TYPE_BUSINESSMAN)
			{
				bBusinessman = true;
			}
			else if (aFunction[i].iType == FUNCTION_TYPE_SAINTESS)
			{
				bSaintess = true;
			}
			else if (aFunction[i].iType == FUNCTION_TYPE_IRONSMITH)
			{
				bIronSmith = true;
			}
		}
		bBusinessman?1:otherTown.iOpenFunction[iIndex++] = FUNCTION_TYPE_BUSINESSMAN;
		bSaintess?1:otherTown.iOpenFunction[iIndex++] = FUNCTION_TYPE_SAINTESS;
		bIronSmith?1:otherTown.iOpenFunction[iIndex++] = FUNCTION_TYPE_IRONSMITH;

		// 剩余宠物
		if (pFUser->m_userPet.GetAllChangID(iPetChangeID, iSize))
		{
			CT_INT32 iPetIndex = 0;
			for (CT_INT32 i = 0; i < iSize / sizeof(CT_INT32);++i)
			{
				if (!pFUser->m_userSeek.IsPetKidnap(iPetChangeID[i]))
				{
					otherTown.iPetID[iPetIndex++] = iPetChangeID[i];
				}
			}
		}

		pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_ENTER_TOWN_SUC, (CT_CHAR*)&otherTown, sizeof(otherTown));
	}
	else
	{
		MSC_DB_EnterOtherTown otherTown = {0};
		otherTown.dwUserID = pOthers->dwUserID;
		otherTown.dwFUserID = pOthers->dwFUserID;
		CT_DWORD threadID = GAME_THREAD_DATABASE + pOthers->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_LOGIN,MSC_SUB_DB_GET_OTHERSTOWN,(CT_CHAR*)&otherTown,sizeof(otherTown));
	}
}

//增产
CT_VOID CCSeekEvent::DisoposeAddProduct(CT_CHAR* pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_BuildingAddition))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisoposeAddProduct iLen != sizeof(CMD_G_BuildingAddition)");
		return;
	}

	CMD_G_BuildingAddition *pBa=(CMD_G_BuildingAddition*)pBuf;
	VistorInfo vistorInfo = {0};

	CCUser* pUser = NULL;
	if (!CCUserManager::getSingleton().Find(pBa->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeEnterOthersTown connot find user,uid:%d", pBa->dwUserID);
		return;
	}

	CMD_G_BuildingAdditionFail baf;
	baf.enFail=BuildProValidate(pUser,pBa->iBuildID);
	if(baf.enFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_ADD_PRODUCT_FAIL,(CT_CHAR*)&baf,sizeof(baf));
		return;
	}

	CMD_G_BuildingAdditionReturn bar={0};
	bar.iBuildID=pBa->iBuildID;
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_ADD_PRODUCT_SUC,(CT_CHAR*)&bar,sizeof(bar));

	// 添加探访记录
	vistorInfo.seekInfo.iType = SEEK_TYPE_ADDPRODUCT;
	vistorInfo.seekInfo.iID[0] = pBa->iBuildID;
	pUser->m_userSeek.SetVistorInfo(vistorInfo);
	return;
}

CT_VOID CCSeekEvent::DisposeGetOthersTownInfo(CT_CHAR* pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_EnterOtherTown))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeGetOthersTownInfo iLen != sizeof(MSC_DB_EnterOtherTown)");
		return;
	}

	MSC_DB_EnterOtherTown* pOthersTown = (MSC_DB_EnterOtherTown*)pBuf;
	CCUser* pUser = NULL;
	if (!CCUserManager::getSingleton().Find(pOthersTown->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeGetOthersTownInfo connot find user, uid:%d", pOthersTown->dwUserID);
		return;
	}

	// 发送客户端
	CT_INT32 iTime = time(0);
	CT_INT32 iTmp = 0;
	CT_INT32 i = 0;
	CMD_G_EnterTown_Others_Suc enterTown = {0};
	enterTown.dwUserID = pOthersTown->dwFUserID;
	strncpy(enterTown.cUserName, pOthersTown->cUserName, sizeof(enterTown.cUserName));
	enterTown.iOccupation = pOthersTown->iOccupation;
	enterTown.iLevel = pOthersTown->iLevel;
	enterTown.iDiamond = pOthersTown->iDiamond;
	enterTown.iMoney = pOthersTown->iMoney;
	enterTown.iFood = pOthersTown->iFood;
	enterTown.iWorker = pOthersTown->iWorker;
	enterTown.iMaxWorker = pOthersTown->iMaxWorker;
	enterTown.iStamina = pOthersTown->iStamina;
	enterTown.iVip = pOthersTown->iVip;
	enterTown.iFightingPower = pOthersTown->iFightingPower;
	enterTown.iSaintessNum = pOthersTown->iSaintess;

	for (CT_INT32 iFlag = 0;iFlag < sizeof(pOthersTown->aTeam) / sizeof(TeamSeek);++iFlag)
	{
		enterTown.aTeam[iFlag].dwFUserID = pOthersTown->aTeam[iFlag].dwUserID;
		enterTown.aTeam[iFlag].iType = pOthersTown->aTeam[iFlag].iType;
		enterTown.aTeam[iFlag].iFightResult = pOthersTown->aTeam[iFlag].iFightResult;
		strncpy(enterTown.aTeam[iFlag].cTName, pOthersTown->aTeam[iFlag].cTName, NAME_SIZE);
	}

	memcpy(enterTown.aSaintessInfo, pOthersTown->aSaintessInfo, sizeof(enterTown.aSaintessInfo));
	memcpy(enterTown.aFarmerInfo, pOthersTown->aFarmerInfo, sizeof(enterTown.aFarmerInfo));
	memcpy(enterTown.aPetInfo, pOthersTown->aPetInfo, sizeof(enterTown.aPetInfo));
	memcpy(enterTown.iPetID, pOthersTown->iPetID, sizeof(enterTown.iPetID));
	memcpy(enterTown.building, pOthersTown->building, sizeof(enterTown.building));
	for (i = 0;i < SEEK_BUILD_NUM;++i)
	{
		enterTown.building[i].iID = pOthersTown->building[i].bd.iID;
		enterTown.building[i].iGrade = pOthersTown->building[i].bd.iGrade;
	}

	for (i = 0;i < SEEK_BUILD_NUM;++i)
	{
		iTmp = pOthersTown->upgradeBuilding[i].iNeedTime - (iTime - pOthersTown->upgradeBuilding[i].llBeginTime);

		if (iTmp > 0)
		{
			enterTown.upgradeBuilding[i].iID = pOthersTown->upgradeBuilding[i].iID;
			enterTown.upgradeBuilding[i].iNeedLastingTime = iTmp;
		}
	}

	for (i = 0;i < SEEK_BUILD_NUM;++i)
	{
		iTmp = pOthersTown->productBuilding[i].iLastingTime - (iTime - pOthersTown->productBuilding[i].llTime);

		if (iTmp > 0)
		{
			enterTown.productBuilding[i].iBID = pOthersTown->productBuilding[i].iBid;
			enterTown.productBuilding[i].iNeedTime = iTmp;
			enterTown.productBuilding[i].iProductType = pOthersTown->productBuilding[i].iType;
		}
	}

	memcpy(enterTown.iOpenFunction, pOthersTown->iOpenFunction, sizeof(enterTown.iOpenFunction));

	pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_ENTER_TOWN_SUC, (CT_CHAR*)&enterTown, sizeof(enterTown));
}

//城墙军队验证
CT_INT32 CCSeekEvent::WallArmyValidate(CCUser *pUser,CT_INT32 iPos[POST_COUNT/2])
{
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPos[i] == 0)continue;

		ArmyInfo ai={0};
		if(!pUser->m_userArmy.GetArmyInfo(iPos[i],ai))
			return WALLARMY_ID;
	}

	for(CT_INT32 i=0;i<POST_COUNT/2-1;++i)
	{
		if(iPos[i] == 0)continue;
		for(CT_INT32 j=i+1;j<POST_COUNT/2;++j)
		{
			if(iPos[j] == 0)continue;
			if(iPos[i] == iPos[j])
				return WALLARMY_SAME;
		}
	}

	CT_INT32 iNum=0;
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPos[i] != 0)
		{
			iNum++;
		}
	}
	if(iNum >= POST_COUNT/2)
		return WALLARMY_COUNT;

	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPos[i] != 0 && iPos[i] <= CLASS_TYPE_MAX)
			return WALLARMY_MASTER;
	}

	pUser->m_userSeek.SetWallArmy(iPos);
	return 0;
}

//绑票验证
CT_INT32 CCSeekEvent::KidnapValidate(CCUser *pUser,CMD_G_SeekKidnapApply *pSka)
{
	if(pSka->iType != KIDNAP_WOMAN && pSka->iType != KIDNAP_FARMER && pSka->iType != KIDNAP_PET)
	{
		return KIDNAP_TYPE;
	}

	Seekers skeeers={0};
	if(pUser->m_userSeek.GetSeeked() != pSka->dwDstUserID)
	{
		return KIDNAP_USER;
	}

	//if(pUser->m_userSeek.IsSeekSpite(pSka->dwDstUserID))
	//{
	//	return KIDNAP_STATUS;
	//}

	Seekers seekers={0};
	if(!CCFightingData::getSingleton().GetSeekedInfo(pSka->dwDstUserID,seekers))
	{
		return KIDNAP_DATA;
	}

	if((pSka->iType == KIDNAP_WOMAN && seekers.iKidnapSaintess != 0) || 
		(pSka->iType == KIDNAP_FARMER && seekers.iKidnapWorkerNum >= SEEK_KIDNAP_FARMAR_MAX))
		return KIDNAP_NUM;

	CT_BOOL bPet=CT_TRUE;
	for(CT_INT32 i=0;i<SEEK_KIDNAP_PET;++i)
	{
		if(seekers.iPetKidnap[i] == pSka->iPetChange && pSka->iPetChange != 0)
		{
			bPet=CT_FALSE;
			break;
		}
	}
	if(!bPet)
		return KIDNAP_NO_PET;

	if(pUser->m_userSeek.GetKidnapNum(pSka->dwDstUserID,pSka->iType) != 0)
		return KIDNAP_ENOUGH;

	return 0;
}


//营救验证
CT_INT32 CCSeekEvent::RescueValidate(CCUser *pUser,CMD_G_SeekRescurApply *pSra)
{
	VistorInfo vi={0};
	if(!pUser->m_userSeek.GetVistorInfo(SEEK_TYPE_KIDNAP,pSra->dwDstUserID,vi))
	{
		return RESCUE_USER;
	}

	if(vi.bIsKidnap == 0)
		return RESCUE_HOSTAGE;

	//资源验证
	if(pSra->iType == RESCUE_MONEY)
	{
		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_RANSOM,&vr))
			return RESCUE_DATA;

		MasterVariableData mvd={0};
		pUser->m_userBase.GetMasterData(mvd);
		if(mvd.stRes.iGold + vr.fMaxVal < 0)
		{
			return RESCUE_RES;
		}

		//扣除资源
		memset(&mvd,0,sizeof(mvd));
		mvd.stRes.iGold=vr.fMaxVal;
		pUser->m_userBase.AddMastData(mvd,SAVATAG_SEEK_RANSOM);

		CCUser *pDstUser=0;
		if(CCUserManager::getSingleton().Find(pSra->dwDstUserID,&pDstUser))
		{
			MasterVariableData DstMvd={0};
			DstMvd.stRes.iGold=abs(vr.fMaxVal);
			pDstUser->m_userBase.AddMastData(DstMvd,SAVATAG_SEEK_RANSOM);
		}
		else
		{
			MSC_DB_MasterSaveData msd = {0};
			msd.dwUserID = pSra->dwDstUserID;
			msd.iSaveTag = SAVATAG_SEEK_RANSOM;
			msd.mvd.stRes.iGold=abs(vr.fMaxVal);
			G_SaveMastvariableData(msd);
		}
	}



	return 0;
}

//掠夺验证
CT_INT32 CCSeekEvent::PlunderValidete(CCUser *pUser,CMD_G_SeekPlunderApply *pSpa)
{
	BuildData bd={0};
	if(!CCFightingData::getSingleton().GetBuildData(pSpa->dwDstUserID,pSpa->iBuildID,bd))
	{
		return PLUNDER_BUILD_ID;
	}

	G_Building    buildingInfo = {0};
	if (!G_GetBuildingData(pSpa->iBuildID, &buildingInfo))
	{
		return PLUNDER_DATA;
	}

	if(buildingInfo.iBT != CITY_TYPE_TOTEM && buildingInfo.iBT != CITY_TYPE_FARMLAND && buildingInfo.iBT != CITY_TYPE_PANNING)
		return PLUNDER_BUILD_TYPE;

	memset(&bd,0,sizeof(bd));
	pUser->m_userSeek.GetBuildData(pSpa->iBuildID,bd);
	if(bd.iPlunderNum > 0)
	{
		return PLUNDER_NUM;
	}

	return 0;
}

//增产验证
ENUM_BuildingAdditionFail CCSeekEvent::BuildProValidate(CCUser *pUser,const CT_INT32 iBuildID)
{
	ProductingBuilding bp={0};
	Seekers seekersInfo={0};
	CCFightingData::getSingleton().GetSeekedInfo(pUser->m_userSeek.GetSeeked(),seekersInfo);
	CT_INT32 iBuildNum=-1;
	for(CT_INT32 i=0;i<SEEK_BUILD_NUM;++i)
	{
		if(iBuildID == seekersInfo.productBuilding[i].iBid)
		{
			iBuildNum=i;
			break;
		}
	}
	if(iBuildNum == -1)
		return EBA_NO_PRODUCTION;

	if(seekersInfo.productBuilding[iBuildNum].iBType != CITY_TYPE_FARMLAND && seekersInfo.productBuilding[iBuildNum].iBType != CITY_TYPE_PANNING)
	{
		return EBA_NO_BUILD;
	}

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_BUILD_ADDITION,&vr))
	{
		return EBA_DATA;
	}

	CCUser *pDstUser=0;
	seekersInfo.productBuilding[iBuildNum].fSeekAddition+=vr.fMaxVal;
	if(CCUserManager::getSingleton().Find(pUser->m_userSeek.GetSeeked(),&pDstUser))
	{
		pDstUser->m_userBuilding.UpdateBuildingProduct(iBuildID,seekersInfo.productBuilding[iBuildNum]);
		pDstUser->m_userBuilding.UpdateSaveProductingData(CT_FALSE, CT_TRUE);
	}
	else
	{
		CCFightingData::getSingleton().UpdateSeekersBaseInfo(pUser->m_userSeek.GetSeeked(),seekersInfo);

		//建筑信息写如数据库
		MSC_DB_SaveProductingData spd = {0};
		spd.dwUserID = pUser->m_userSeek.GetSeeked();
		spd.productingBuilding = seekersInfo.productBuilding[iBuildNum];
		G_SendDBThreadInfo(spd.dwUserID,  MSC_MAIN_DB_SAVE_DATA,     MSC_SUB_DB_PRODUCTING_DATA , (CT_CHAR*)&spd, sizeof(spd));
	} 
	return EBA_BUILADDITION_SUC;
}

//数值纠正
CT_INT32 CCSeekEvent::SeekValueCorrection(CT_INT32 iType,CT_INT32 iValue)
{
	G_ValueRange vr={0};
	if(!G_GetValueRange(iType,&vr))
	{
		CCLogMeg::getSingleton().WriteWaring("CCSeekEvent::SeekValueCorrection G_GetValueRange iType:%d",iType);
		return 0;
	}

	if(iValue > vr.fMaxVal)
	{
		iValue=vr.fMaxVal;
	}
	return iValue;
}

//战报索引
CT_VOID CCSeekEvent::MD5FightIndex(CT_UCHAR *pDst,CT_DWORD dwDstID,CT_DWORD dwMyID,CT_INT32 iType)
{
	CT_UCHAR ucStrBuf[SEEK_FIGHT_RECORD_INDEX*3]={0};
	CT_INT32 iStrLen=SEEK_FIGHT_RECORD_INDEX*3;
	CT_INT32 iDstLen=SEEK_FIGHT_RECORD_INDEX;
	sprintf((CT_CHAR*)ucStrBuf,"%d%d%d",dwMyID,dwDstID,iType);
	CCMD5::getSingleton().Encrypt(pDst,iDstLen,ucStrBuf,iStrLen);
	return;
}

//得到被访者信息
CT_VOID CCSeekEvent::DisposeSeekedGet(CT_CHAR* pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_OtherSeekedInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeSeekedGet iLen != sizeof(MSC_DB_SeekedInfo)");
		return;
	}

	MSC_DB_OtherSeekedInfo* pSeeked = (MSC_DB_OtherSeekedInfo*)pBuf;
	CCUser* pUser;
	G_ValueRange vr = {0};
	G_ChestByLevel chest = {0};
	MasterData md = {0};
	CT_INT32 enFail = 0;
	CT_BOOL flag = false;
	Seekers seeks = {0};
	CCFightingData::getSingleton().GetSeekedInfo(pSeeked->seekInfo.dwFUserID, seeks);
	if (!CCUserManager::getSingleton().Find(pSeeked->seekInfo.dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeSeekedGet connot find user.uid:%d", pSeeked->seekInfo.dwUserID);
		return;
	}

	pUser->m_userBase.GetMasterData(md);
	if (SEEK_GOODWILL == pSeeked->seekInfo.iType)
	{
		G_GetValueRange(VALUE_TYPE_GOODSEEKED_TIMES, &vr);
		G_GetChestByLevel(SEEK_GOODWILL, md.mvd.stGrade.iLevel, &chest);

		if (pSeeked->seekInfo.iSeekedWillTimes < vr.fMaxVal)
		{
			// 善访成功
			flag = true;
			++seeks.iSeekedWillTimes;
			pUser->m_userSeek.SeekOneTimes(SEEK_GOODWILL, pSeeked->seekInfo.dwFUserID);
			CCFightingData::getSingleton().UpdateSeekedTimes(pSeeked->seekInfo.iType,seeks);
		}
	}
	else
	{
		G_GetValueRange(VALUE_TYPE_BADSEEKED_TIMES, &vr);
		if (pSeeked->seekInfo.iSeekedSpiteTimes < vr.fMaxVal)
		{
			// 恶访成功
			flag = true;
	//		++seeks.iSeekedSpiteTimes;
	//		pUser->m_userSeek.SeekOneTimes(SEEK_SPITE, pSeeked->seekInfo.dwFUserID);
			pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_SPITE_SUC);
		}
	}

	if (CT_TRUE == flag)
	{
		DisposeGetOthersTownInfo((CT_CHAR*)&(pSeeked->townInfo), sizeof(MSC_DB_EnterOtherTown));
	}
	return;
}

//领取礼物
CT_VOID CCSeekEvent::DisposeGetPresent(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_GetSeekedPresent))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeGetPresent iLen != sizeof(CMD_G_GetSeekedPresent)");
		return;
	}

	CMD_G_GetSeekedPresent* pPresent = (CMD_G_GetSeekedPresent*)pBuf;
	CCUser* pUser = 0;

	if (!CCUserManager::getSingleton().Find(pPresent->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeGetPresent connot find uid:%d", pPresent->dwUserID);
		return;
	}

	pUser->m_userSeek.UsePresent();
	return;
}

//回营
CT_VOID CCSeekEvent::DisposeSeekBack(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_SeekBack))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeSeekBack iLen != sizeof(CMD_G_SeekBack)");
		return;
	}

	CMD_G_SeekBack* pBack = (CMD_G_SeekBack*)pBuf;
	CCUser* pUser = 0;
	SeekData sd = {0};
	VistorInfo vi[SEEK_DOTYPE_NUM] = {0};
	CT_INT32 iSize= sizeof(vi);
	if (!CCUserManager::getSingleton().Find(pBack->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeSeekBack connot find uid:%d", pBack->dwUserID);
		return;
	}

	pUser->m_userSeek.GetSeekData(sd);

	pUser->m_userSeek.GetCurrentVistorInfo(vi, iSize);

	CCUser* pFUser = 0;
	if (CCUserManager::getSingleton().Find(sd.dwSeekID, &pFUser))
	{
		for (CT_INT32 i = 0;i < iSize / sizeof(VistorInfo);++i)
		{
			pFUser->m_userSeek.AddVistorInfo(vi[i]);

			// 发送客户端
			CT_INT32 iVisitorLen = VISTOR_INFO_LEN;
			CMD_G_UpdateVisitorInfo uvi = {0};
			uvi.vi = vi[i];
			uvi.vi.seekInfo.iType %= 100;
			pFUser->m_userSeek.GetVisitorDes(vi[i], uvi.cInfo, iVisitorLen);
			if (strlen(uvi.cInfo) != 0)
			{
				pFUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_FRIEND_RECORD_UPDATE, (CT_CHAR*)&uvi, sizeof(uvi));
			}
			
			// 统计信息
			CMD_G_VisitorTotalInfo vti = {0};
			pFUser->m_userSeek.GetTotalInfo(vti.tDayTotal, sizeof(vti.tDayTotal), vti.iDayNum, CT_TRUE);
			pFUser->m_userSeek.GetTotalInfo(vti.wDayTotal, sizeof(vti.wDayTotal), vti.iWeekNum, CT_FALSE);
			pFUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_UPDATE_STATISTIC, (CT_CHAR*)&vti, sizeof(vti));
		}
	}

	for (CT_INT32 j = 0;j < iSize / sizeof(VistorInfo);++j)
	{
		pUser->m_userSeek.AddSeekInfo(vi[j]);

		// 更新统计信息
		CMD_G_VisitorTotalInfo vti = {0};
		pUser->m_userSeek.GetTotalInfo(vti.tDayTotal, sizeof(vti.tDayTotal), vti.iDayNum, CT_TRUE);
		pUser->m_userSeek.GetTotalInfo(vti.wDayTotal, sizeof(vti.wDayTotal), vti.iWeekNum, CT_FALSE);
		pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_UPDATE_STATISTIC, (CT_CHAR*)&vti, sizeof(vti));
	}
	pUser->m_userSeek.SeekVistor();
}

//查看战斗
CT_VOID CCSeekEvent::DisposeLookFighting(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_SeekLookFighting))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeLookFighting iLen != sizeof(CMD_G_SeekLookFighting)");
		return;
	}

	CMD_G_SeekLookFighting* pSlf = (CMD_G_SeekLookFighting*)pBuf;
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pSlf->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposeLookFighting connot find uid:%d", pSlf->dwUserID);
		return;
	}

	CMD_G_SeekLookFightingFail slf={0};
	VistorInfo vi={0};
	SeekFightingData sfd={0};
	time_t t = time(0);
	tm* pTm = localtime(&t);
	if(!pUser->m_userSeek.GetVistorInfo(pSlf->iType,pSlf->dwDstUserID,vi))
	{
		slf.iFail=SEEK_FIGHTING_USER;
	}
	else if(pSlf->iType == SEEK_TYPE_KIDNAP)
	{
		slf.iFail=SEEK_FIGHTING_USER;
		for(CT_INT32 i=0;i<TEAM_RES_CNT;++i)
		{
			if(vi.seekInfo.iID[i] == KIDNAP_WOMAN)
			{
				slf.iFail=0;
				break;
			}
		}
	}
	else if(pSlf->iType == SEEK_TYPE_PLUNDER)
	{
		slf.iFail=SEEK_FIGHTING_USER;
		for(CT_INT32 i=0;i<SEEK_PLUNDER_BUILDTYPE;++i)
		{
			if(vi.iPlunderBID[i] == CITY_TYPE_TOTEM)
			{
				slf.iFail=0;
				break;
			}
		}
	}

	pSlf->iType = pSlf->iType + pTm->tm_wday * 100;
	MD5FightIndex(sfd.ucIndex,pSlf->dwDstUserID,pSlf->dwUserID,pSlf->iType);
	if(!pUser->m_userSeek.GetFightingData(sfd.ucIndex,sfd))
	{
		slf.iFail=SEEK_FIGHTING_USER;
	}

	if(slf.iFail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_LOOK_FIGHTING_FAIL,(CT_CHAR*)&slf,sizeof(slf));
		return;
	}

	MSC_RESCUE_FIGHT rf={0};
	rf.dwUserID=pUser->GetUserID();
	rf.iSeed=sfd.iSeed;
	memcpy(&rf.LeftFightingData[0],&sfd.LeftFightingData,sizeof(rf.LeftFightingData[0]));
	memcpy(&rf.RightFightingData[0],&sfd.RightFightingData,sizeof(rf.RightFightingData[0]));
	G_SendFightThreadInfo(pUser->GetUserID(),MSC_MAIN_FIGHTING_EVENT,MSC_SUB_SEEK_LOOK_FIGHTING,(CT_CHAR*)&rf,sizeof(rf));
	return;
}

//界面掠夺请求
CT_VOID CCSeekEvent::DisposePlunderRequest(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_PlunderRequest))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePlunderRequest iLen != sizeof(CMD_G_PlunderRequest)");
		return;
	}

	CMD_G_PlunderRequest* pRequest = (CMD_G_PlunderRequest*)pBuf;
	CCUser* pUser = 0;
	CCUser* pFUser = 0;
	SeekData sd = {0};
	Seekers seeker = {0};
	CT_INT32 enfail = 0;
	G_ValueRange vr = {0};

	if (!CCUserManager::getSingleton().Find(pRequest->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePlunderRequest connot find uid:%d", pRequest->dwUserID);
		return;
	}

	pUser->m_userSeek.GetSeekData(sd);
	if (sd.iType != SEEK_SPITE	|| sd.dwSeekID == 0 
		|| !CCFightingData::getSingleton().GetSeekedInfo(sd.dwSeekID, seeker))
	{
		enfail = ERF_SEEKERID_ERROR;
	}

	G_GetValueRange(VALUE_TYPE_BADSEEKED_TIMES, &vr);
	if (seeker.iSeekedSpiteTimes >= vr.fMaxVal)
	{
		enfail = ERF_SEEKERID_ERROR;
	}

	if (enfail != 0)
	{
		// 请求失败
		CMD_G_PlunderRequestFial prf;
		prf.enFail = (ENUM_PlunderRequestFail)enfail;
		pUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_PLUNDER_REQUEST_FAIL, (CT_CHAR*)&prf, sizeof(prf));
		return;
	}

	pUser->m_userSeek.SeekOneTimes(SEEK_SPITE, sd.dwSeekID);
	if (CCUserManager::getSingleton().Find(sd.dwSeekID, &pFUser))
	{
		pFUser->m_userSeek.SeekedOneTimes(SEEK_SPITE);
	}
	else
	{
		++seeker.iSeekedSpiteTimes;
		CCFightingData::getSingleton().UpdateSeekedTimes(SEEK_SPITE,seeker);
	}
	
	CMD_G_SeekSpiteSuc sss = {0};
	sss.iTimes = sd.iSeekSpiteTimes + 1;
	sss.dwRUserID = sd.dwSeekID;
	pUser->GetNetInterface()->Send(MAIN_G_SEEK,SUB_G_SEEK_PLUNDER_REQUEST_SUC, (CT_CHAR*)&sss, sizeof(sss));

	// 掠夺成功发放道具
	FakeProBase fb = {0};
	RewardCost rc = {0};
	memset(&vr,0,sizeof(vr));
	if (!G_GetValueRange(VALUE_TYPE_PLUNDER_REWARD_LOST, &vr))
	{
		CCLogMeg::getSingleton().WriteError("CCSeekEvent::DisposePlunderRequest G_GetValueRange  VALUE_TYPE_PLUNDER_REWARD_LOST error.");
		return;
	}

	G_GetCostReward(vr.fMinVal,pUser,ITEM_OP_MODE_CKEXE,SAVATAG_PLUNDER_REWARD,rc,fb);
}