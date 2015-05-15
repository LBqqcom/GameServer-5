#include "CPVPEvent.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CUserManager.h"
#include "CDBConnector.h"
#include "CFightingScriptSetting.h"
#include "GlobeScriptInterface.h"
#include "CTop.h"
#include <time.h>
#include "CCompress.h"
#include "CFightingData.h"
#include "CBroadcastScriptSetting.h"


CCPVPEvent::CCPVPEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_PVP_EVENT,SUB_G_REFRESH);
	RegistMessage(MAIN_G_PVP_EVENT,SUB_G_CHEST);
	RegistMessage(MAIN_G_PVP_EVENT,SUB_G_PVP_FIGHTING);
	RegistMessage(MAIN_G_PVP_EVENT,SUB_G_VIEW_USER);
	RegistMessage(MAIN_G_PVP_EVENT,SUB_G_PVP_LOOK_FIGHITNG);

	RegistMessage(MSC_MAIN_EVENT_PVP,MSC_SUB_PVP_FIGHTING_END);
	RegistMessage(MSC_MAIN_EVENT_PVP, MSC_SUB_PVP_CHALLENG);
	
	
	RegistMessage(MSC_MAIN_EVENT_PVP, MSC_SUB_PVP_LOW_CHALLER);
	RegistMessage(MSC_MAIN_EVENT_PVP, MSC_SUB_PVP_HIGN_CHALLER);
	RegistMessage(MSC_MAIN_EVENT_PVP, MSC_SUB_PVP_INSERT_CHALLER_FINISH);

    m_pBuffer = new CT_CHAR[COMPRESS_MAX_SIZE];
}

CCPVPEvent::~CCPVPEvent(void)
{
	delete m_pBuffer;
	m_pBuffer = 0;
}

//处理用户消息
CT_VOID CCPVPEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MAIN_G_PVP_EVENT:
		{
			DisposeNetMessage(subID,pBuf,len,reserve);
			break;
		}
	case MSC_MAIN_EVENT_PVP:
	case MSC_MAIN_DB_PVP_DATA:
		{
			DisposeThreadMessage(subID,pBuf,len,reserve);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCPVPEvent::OnUserEvent :mainID=%i", mainID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCPVPEvent::OnTime(CT_DWORD timeID)
{
	return;
}

//网络消息
CT_VOID CCPVPEvent::DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(subID)
	{
	case SUB_G_REFRESH:				//刷新对手
		{
			DisposeRefresh(pBuf,len);
			break;
		}
	case SUB_G_CHEST:				//打开宝箱
		{
			DisposeChest(pBuf,len);
			break;
		}
	case SUB_G_PVP_FIGHTING:		//竞技场战斗
		{
			DisposePVPFighting(pBuf,len);
			break;
		}
	case SUB_G_PVP_LOOK_FIGHITNG:		//查看战报
		{
			DisposePVPLookFighting(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeNetMessage :subID=%i", subID);
			break;
		}
	}
	return;
}

//线程消息
CT_VOID CCPVPEvent::DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(subID)
	{
	case MSC_SUB_PVP_FIGHTING_END:				//战斗记录
		{
			DisposeMSCFightingEnd(pBuf,len);
			break;
		}
	case MSC_SUB_PVP_CHALLENG:				//非在线用户信息
		{
			DisposeMSCUserInfo(pBuf,len);
			break;
		}
	case MSC_SUB_PVP_LOW_CHALLER:
		{
			DisposeChallgerInsert(pBuf, len, CT_FALSE);
			break;
		}

	case MSC_SUB_PVP_HIGN_CHALLER:
		{
			DisposeChallgerInsert(pBuf, len, CT_TRUE);
			break;
		}
	case MSC_SUB_PVP_INSERT_CHALLER_FINISH:
		{
			DisposeInsertChallgerFinish(pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeThreadMessage :subID=%i", subID);
			break;
		}
	}
	return;
}


//刷新对手
CT_VOID CCPVPEvent::DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Refresh))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeRefresh :len != sizeof(CMD_G_Refresh)");
		return;
	}

	CMD_G_Refresh *pRefresh=(CMD_G_Refresh*)pBuf;

	//刷新验证
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRefresh->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeChest:No Find UserID:%d",pRefresh->dwUserID);
		return;
	}

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPREFRESH,&vr))
	{
		CMD_G_PVPErrorInfo PVPErrorInfo={0};
		PVPErrorInfo.dwUserID=pUser->GetUserID();
		PVPErrorInfo.iErrorInfo=PVP_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));

		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeRefresh G_GetValueRange VALUE_TYPE_PVPREFRESH");
		return;
	}

	PVPInfo PVPinfo={0};
	pUser->m_userPVPInfo.GetPVPInfo(PVPinfo);

	CT_INT64 lTime=(CT_INT64)time(0)-PVPinfo.lBeginTime;
	if(PVPinfo.lRefreshTime <= lTime || pRefresh->iPayDiamond != 0 || PVPinfo.lRefreshTime == 0)
	{
		//记录数据
		if(pRefresh->iPayDiamond != 0)
		{
			MasterVariableData msc={0};
			pUser->m_userBase.GetMasterData(msc);
			if(msc.stRes.iDiamod < -pRefresh->iPayDiamond || -pRefresh->iPayDiamond < -(CT_INT32)vr.fMaxVal)
			{
				pUser->CloseNet();
				return;
			}

			memset(&msc,0,sizeof(msc));
			msc.stRes.iDiamod=pRefresh->iPayDiamond;
			pUser->m_userBase.AddMastData(msc,SAVETAG_PVP_PVPREFRESH);

			MSC_LOGDB_Refresh LogdbRefresh={0};
			LogdbRefresh.dwUserID=pRefresh->dwUserID;
			LogdbRefresh.iDiamod=pRefresh->iPayDiamond;

			CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pRefresh->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PVPREFRESH,(CT_CHAR*)&LogdbRefresh,sizeof(LogdbRefresh));
		}

		//刷新对手
		MSC_DB_Refresh Refresh={0};
		Refresh.dwUserID=pRefresh->dwUserID;

		CT_DWORD threadid = GAME_THREAD_DATABASE + pRefresh->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_W_REFRESH,(CT_CHAR*)&Refresh,sizeof(Refresh));

		//冷却完毕
		if(PVPinfo.lRefreshTime <= lTime)
		{
			G_ValueRange CoolVale={0};
			if(!G_GetValueRange(VALUE_TYPE_PVPEVENTTIME,&CoolVale))
			{
				CMD_G_PVPErrorInfo PVPErrorInfo={0};
				PVPErrorInfo.dwUserID=pUser->GetUserID();
				PVPErrorInfo.iErrorInfo=PVP_ERROR;
				pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));

				CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeRefresh%s %d",__FILE__,__LINE__);
				return;
			}

			//设置冷却时间
			if(pUser->m_userPVPInfo.GetCoolTime((CT_INT64)time(0)))
			{
				pUser->m_userPVPInfo.SetBeginCoolTime((CT_INT64)time(0));
				pUser->m_userPVPInfo.SetAddCoolTime((CT_INT64)CoolVale.fMinVal);
			}
		}

	}
	else
	{
		CMD_G_ViewRefresh ViewRefresh={0};
		ViewRefresh.dwUserID=pRefresh->dwUserID;
		ViewRefresh.iDiamond=(CT_INT32)vr.fMinVal;
		ViewRefresh.lCoolTime=PVPinfo.lBeginTime+PVPinfo.lRefreshTime-(CT_INT64)time(0);
		ViewRefresh.lCoolTime=(ViewRefresh.lCoolTime>0?ViewRefresh.lCoolTime:0);

		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_VIEW_REFRESH,(CT_CHAR*)&ViewRefresh,sizeof(ViewRefresh));
	}

	return;
}

//打开宝箱
CT_VOID CCPVPEvent::DisposeChest(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_Chest))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeChest :len != sizeof(CMD_G_Chest)");
		return;
	}

	CMD_G_Chest *pChest=(CMD_G_Chest*)pBuf;

	//用户军衔
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pChest->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeChest:No Find UserID:%d",pChest->dwUserID);
		return;
	}

	PVPInfo pvpinfo={0};
	pUser->m_userPVPInfo.GetPVPInfo(pvpinfo);
	if(pvpinfo.cbChestStatus  != CHEST_STATUS_OPEN)
	{
		//更新竞技场信息
		CMD_G_PVPInfo SendPVPInfo={0};
		CT_INT32 iLen=sizeof(SendPVPInfo);
		SendPVPInfo.dwUserID=pUser->GetUserID();
		if(pUser->m_userPVPInfo.GetPVPInfo((CT_CHAR*)&SendPVPInfo,iLen))
		{
			CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
			memset(m_pBuffer,0,sizeof(COMPRESS_MAX_SIZE));
			if(CCCompress::getSingleton().Compress((CT_CHAR*)&SendPVPInfo,iLen,m_pBuffer,iOutLen))
			{
				pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_INFO, m_pBuffer, iOutLen);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCFightingEnd Compress Fail!");
			}
		}
		return;
	}

	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);

	//读取奖励
	MSC_LOGDB_Chest Chest={0};
	G_PVPChest PVPChest={0};
	CT_INT32 iRankLevel=MasterInfo.mvd.stRank.iLevel;
	if(!G_GetPVPChestFile(iRankLevel,pvpinfo.iPVPChestNum+1,&PVPChest))
	{
		CMD_G_PVPErrorInfo PVPErrorInfo={0};
		PVPErrorInfo.dwUserID=pUser->GetUserID();
		PVPErrorInfo.iErrorInfo=PVP_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));

		CCLogMeg::getSingleton().WriteError("读取宝箱脚本失败");
		return;
	}

	CT_INT32 iGoods1[GOODS_COUNT]={0};
	CT_INT32 iCount1[GOODS_COUNT]={0};
	FakeProBase fb={0};
	G_GetFakeProDta(pUser,SAVETAGEEX_PVP_CHEST,fb);
	G_GetGoodsGroupInfo(PVPChest.iGoodsGroup,iGoods1,iCount1,fb);
	G_ClearFakeProDat(pUser,SAVETAGEEX_PVP_CHEST,fb);

	for(CT_INT32 j=0;j<GOODS_COUNT;++j)
	{
		if(iGoods1[j] != 0 && iCount1[j] != 0)
		{
			Chest.iGoodsID3=iGoods1[j];
			Chest.iGoodsCount3=iCount1[j];
			break;
		}
	}

	//插入背包
	CT_INT32 iGoods[3]={Chest.iGoodsID3,PVPChest.iGoodsID1,PVPChest.iGoodsID2};
	CT_INT32 iCount[3]={Chest.iGoodsCount3,PVPChest.iCount1,PVPChest.iCount2};
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	if(!pUser->m_userItem.AutoAddItemData(SAVETAG_PVP_CHEST,iGoods,iCount,3,ITEM_OP_MODE_CHECK))
	{
		//空间不足
		CMD_G_PVPErrorInfo PVPErrorInfo={0};
		PVPErrorInfo.dwUserID=pUser->GetUserID();
		PVPErrorInfo.iErrorInfo=SLOT_FULL;
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));
	}
	else
	{
		//更新次数
		if(!pUser->m_userPVPInfo.UpDataChestNum())
		{
			CMD_G_PVPErrorInfo PVPErrorInfo={0};
			PVPErrorInfo.dwUserID=pUser->GetUserID();
			PVPErrorInfo.iErrorInfo=PVP_CHESTNUM;
			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));
			return;
		}

		//资源奖励
		MasterVariableData AddMasterInfo={0};
		AddMasterInfo.stRes.iTalentNum+=PVPChest.iTalentNum;
		AddMasterInfo.stRes.iGold+=PVPChest.iGold;
		AddMasterInfo.stRank.iCurrentExp+=PVPChest.iPrestige;
		Chest.iGold+=PVPChest.iGold;
		Chest.iTalentNum+=PVPChest.iTalentNum;
		Chest.iPrestige+=PVPChest.iPrestige;

		pUser->m_userItem.AutoAddItemData(SAVETAG_PVP_CHEST,iGoods,iCount,3,ITEM_OP_MODE_EXEC);
	

	    //发送奖励
	    CMD_G_ChestInfo ChestInfo={0};
	    ChestInfo.dwUserID=pUser->GetUserID();
	    ChestInfo.iGold=PVPChest.iGold;
	    ChestInfo.iTalentNum=PVPChest.iTalentNum;
	    ChestInfo.iPrestige=PVPChest.iPrestige;
	    ChestInfo.iGoodsID1=PVPChest.iGoodsID1;
	    ChestInfo.iGoodsID2=PVPChest.iGoodsID2;
	    ChestInfo.iGoodsID3=Chest.iGoodsID3;
	    ChestInfo.iCount1=PVPChest.iCount1;
	    ChestInfo.iCount2=PVPChest.iCount2;
	    ChestInfo.iCount3=Chest.iGoodsCount3;

	    pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_CHEST_INFO,(CT_CHAR*)&ChestInfo,sizeof(ChestInfo));

	    pUser->m_userBase.AddMastData(AddMasterInfo,SAVETAG_PVP_CHEST);

	    //记录数据
	    Chest.dwUserID=pChest->dwUserID;
	    Chest.iGoodsID1=PVPChest.iGoodsID1;
	    Chest.iGoodsID2=PVPChest.iGoodsID2;
	    Chest.iGoodsCount1=PVPChest.iCount1;
	    Chest.iGoodsCount2=PVPChest.iCount2;


	    threadid = GAME_THREAD_LOGDATABASE + pChest->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	    G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_CHEST,(CT_CHAR*)&Chest,sizeof(Chest));

		pUser->m_userPVPInfo.SetPVPDegree(-1);

	    //add by caihan
	    //判断竞技场宝箱任务
	    CMD_G_TaskEventInfo taskEventInfo = {0};
	    taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ARENA;
	    taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ARENA_CHEST;
	    //taskEventInfo.iSubType = pChest->iChestType;
	    pUser->m_userTask.DisposeTask(taskEventInfo);
	    //end
                     
        // 日常任务 - 竞技场宝箱 [4/7/2013 John]
        pUser->m_userDaily.UpdateQuestData(DAILY_ARENA_CHEST, 1);
    }
}

//竞技场战斗
CT_VOID CCPVPEvent::DisposePVPFighting(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_PVPFighting))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPFighting :len != sizeof(CMD_G_PVPFighting)");
		return;
	}

	CMD_G_PVPFighting *pPVPFighting=(CMD_G_PVPFighting*)pBuf;

	CCUser *pUser=NULL;
	if(!CCUserManager::getSingleton().Find(pPVPFighting->dwUserID, &pUser))
	{
		printf("%s %d",__FILE__,__LINE__);
		return;
	}


	//对手验证
	if(!pUser->m_userPVPInfo.GetChallengeStatce(pPVPFighting->iIndex))
	{
		CMD_G_PVPErrorInfo PVPInfo={0};
		PVPInfo.dwUserID=pUser->GetUserID();
		PVPInfo.iErrorInfo=NO_FIGHTING;
		PVPInfo.iPayDiamond=0;
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPInfo,sizeof(PVPInfo));

		return;
	}

	//战斗验证
	if(!PVPFightingValidate(pUser,pPVPFighting->iDiamond))
	{
		return;
	}

	CT_INT32 iPosition[POST_COUNT/2]={0};
	G_ValueRange SceneValue={0};
	G_GetValueRange(VALUE_TYPE_PVPSCENECOUNT,&SceneValue);
	MSC_PVP_Fighting_Info FightingInfo={0};

	FightingInfo.dwUserID=pPVPFighting->dwUserID;
	FightingInfo.iSceneID=rand()%((CT_INT32)SceneValue.fMinVal);
	FightingInfo.iSeed=rand()%RAND_MAX_EX;
	FightingInfo.iDifficulty=pUser->m_userPVPInfo.GetDifficulty(pPVPFighting->dwDstUserID);
	G_GetFightingData(pUser,FightingInfo.FightingData[0]);
	pUser->m_userArmy.DeleteBoss();

	//UserFightingStatus UserInfo={0};
	//if(!pUser->m_userPVPInfo.GetFightingUser(pPVPFighting->iIndex,pPVPFighting->dwDstUserID,FightingInfo.FightingData[1]))
	//{
	//	CCLogMeg::getSingleton().WriteError("无此挑战用户!");
	//	pUser->CloseNet();
	//	return;
	//}
	FightingData fd={0};
	CCFightingData::getSingleton().GetFightingData(pPVPFighting->dwDstUserID,fd);
	memcpy(&FightingInfo.FightingData[1],&fd.FData,sizeof(fd.FData));

	CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUS_PVP_FIGHTING, (CT_CHAR*)&FightingInfo, sizeof(FightingInfo));

	return;
}

//查看战报
CT_VOID CCPVPEvent::DisposePVPLookFighting(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_PVPLookFighting))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPLookFighting :len != sizeof(CMD_G_PVPLookFighting)");
		return;
	}

	CMD_G_PVPLookFighting *pPVPLookFighting=(CMD_G_PVPLookFighting*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pPVPLookFighting->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPLookFighting:%d No Find UserID",pPVPLookFighting->dwUserID);
		return;
	}

	MSC_PVP_Fighting_Info FightingInfo={0};
	if(!pUser->m_userPVPInfo.GetFightingInfo(pPVPLookFighting->iIndex,FightingInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposePVPLookFighting No Find Index:%d",pPVPLookFighting->iIndex);
		pUser->CloseNet();
		return;
	}
	FightingInfo.dwUserID=pPVPLookFighting->dwUserID;

	CT_DWORD threadid = GAME_THREAD_FIGHT + pPVPLookFighting->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_PVP_LOOK_FIGHTING, (CT_CHAR*)&FightingInfo,sizeof(FightingInfo));


	return;
}

//用户信息
CT_VOID CCPVPEvent::DisposeMSCUserInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	memset(m_pBuffer, 0, COMPRESS_MAX_SIZE);
	if(len != sizeof(MSC_DB_ChallengData))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCUserInfo :len != sizeof(MSC_DB_ChallengData)");
		return;
	}

	MSC_DB_ChallengData *pBaseInfo=(MSC_DB_ChallengData*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pBaseInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCUserInfo:%d No Find UserID",pBaseInfo->dwUserID);
		return;
	}


	//插入用户信息
	for(CT_INT32 i=0;i<PVP_CHALLENGE_COUNT;++i)
	{
		pUser->m_userPVPInfo.SetChallengInfo(i,pBaseInfo->UserFightinginfo[i],pBaseInfo->LookUserinfo[i]);
	}


	//更新竞技场信息
	CMD_G_PVPInfo PVPInfo={0};
	CT_INT32 iLen=sizeof(PVPInfo);
	pUser->m_userPVPInfo.GetPVPInfo((CT_CHAR*)&PVPInfo,iLen);
	PVPInfo.dwUserID=pUser->GetUserID();

	//CT_CHAR *pBuffer=0;
	//if(G_Compress((CT_CHAR*)&PVPInfo,iLen,&pBuffer,iLen))
	//{
	//	pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_INFO,pBuffer,iLen);
	//}
	////Decompress((CT_INT32*)pBuf,bufLen,(CT_INT32*)&buf);
	//delete[] pBuffer;
	//pBuffer=0;

	CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
	if(CCCompress::getSingleton().Compress((CT_CHAR*)&PVPInfo,iLen, m_pBuffer,iOutLen))
	{
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_INFO, m_pBuffer, iOutLen);
	}

	return;
}

//战斗结束
//CT_VOID  CCPVPEvent::DisposeMSCFightingEnd(CT_CHAR* pBuf,CT_DWORD len)
//{
//	memset(m_pBuffer, 0, COMPRESS_MAX_SIZE);
//	if(len != sizeof(MSC_PVP_Fighting_Info)+sizeof(MSC_DB_W_PVPFightingInfo))
//	{
//		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCFightingEnd :len != sizeof(MSC_PVP_Fighting_End)");
//		return;
//	}
//
//	//更新战斗信息
//	CCUser *pUser=0;
//	MSC_PVP_Fighting_Info FightingInfo={0};
//	memcpy(&FightingInfo,pBuf,sizeof(FightingInfo));
//	if(!CCUserManager::getSingleton().Find(FightingInfo.dwUserID,&pUser))
//	{
//		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCFightingEnd:%d No Find UserID",FightingInfo.dwUserID);
//		return;
//	}
//
//	const CT_INT32 iUpDataLen=sizeof(MSC_PVP_Fighting_Info);
//	CT_CHAR cPVPBuf[iUpDataLen]={0};
//	memcpy(cPVPBuf,pBuf,iUpDataLen);
//	pUser->m_userPVPInfo.UpdataFighting(cPVPBuf,iUpDataLen);
//	//保存竞技场战报
//	pUser->m_userPVPInfo.SaveFightingReacord();
//
//	//更新数据库
//	PVPInfo PVPBase={0};
//	pUser->m_userPVPInfo.GetPVPInfo(PVPBase,CT_TRUE);
//
//	MSC_DB_W_PVPInfo DBPVPinfo={0};
//	DBPVPinfo.dwUserID=pUser->GetUserID();
//	DBPVPinfo.iFightingCount=PVPBase.iFightingCount;
//	DBPVPinfo.lRefreshTime=PVPBase.lRefreshTime;
//	DBPVPinfo.cbChestStatus=PVPBase.cbChestStatus;
//	DBPVPinfo.iPVPDegree=PVPBase.iPVPDegree;
//	DBPVPinfo.iPVPChestNum=PVPBase.iPVPChestNum;
//
//	CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
//	G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_R_PVP_INFO,(CT_CHAR*)&DBPVPinfo,sizeof(DBPVPinfo));
//
//	//更新竞技场信息
//	CMD_G_PVPInfo SendPVPInfo={0};
//	CT_INT32 iLen=sizeof(SendPVPInfo);
//	SendPVPInfo.dwUserID=pUser->GetUserID();
//	if(pUser->m_userPVPInfo.GetPVPInfo((CT_CHAR*)&SendPVPInfo,iLen))
//	{
//		CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
//		memset(m_pBuffer,0,sizeof(COMPRESS_MAX_SIZE));
//		if(CCCompress::getSingleton().Compress((CT_CHAR*)&SendPVPInfo,iLen,m_pBuffer,iOutLen))
//		{
//			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_INFO, m_pBuffer, iOutLen);
//		}
//		else
//		{
//			CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCFightingEnd Compress Fail!");
//		}
//	}
//
//
//	//更新战报
//	CMD_G_PVPFightingRecord PVPFightingRecord={0};
//	iLen=sizeof(PVPFightingRecord);
//	if(pUser->m_userPVPInfo.GetPVPRectord((CT_CHAR*)&PVPFightingRecord,iLen))
//	{
//		CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
//		memset(m_pBuffer,0,sizeof(COMPRESS_MAX_SIZE));
//		if(CCCompress::getSingleton().Compress((CT_CHAR*)&PVPFightingRecord,iLen,m_pBuffer,iOutLen))
//		{
//			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_FIGHTING_RECORD, m_pBuffer, iOutLen);
//		}
//	}
//
//	MSC_DB_W_PVPFightingInfo PVPinfo={0};
//	memcpy(&PVPinfo,pBuf+sizeof(MSC_PVP_Fighting_Info),sizeof(MSC_DB_W_PVPFightingInfo));
//
//	//更新对手
//	CCUser *pDstUser=0;
//	if(CCUserManager::getSingleton().Find(FightingInfo.FightingData[1].dwUserID,&pDstUser))
//	{
//		pDstUser->m_userPVPInfo.UpdataFighting(cPVPBuf,iUpDataLen);
//
//		//更新对手战报
//		memset(&PVPFightingRecord,0,sizeof(PVPFightingRecord));
//		iLen=sizeof(PVPFightingRecord);
//		if(pDstUser->m_userPVPInfo.GetPVPRectord((CT_CHAR*)&PVPFightingRecord,iLen))
//		{
//			CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
//			memset(m_pBuffer,0,sizeof(COMPRESS_MAX_SIZE));
//			if(CCCompress::getSingleton().Compress((CT_CHAR*)&PVPFightingRecord,iLen,m_pBuffer,iOutLen))
//			{
//				pDstUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_FIGHTING_RECORD, m_pBuffer, iOutLen);
//			}
//		}
//
//		MasterVariableData MasterInfo={0};
//		MasterInfo.pvpData.iFightTimes=1;
//		if(PVPinfo.PVPFightinginfo.iWiner == WINER_RIGHT)
//			MasterInfo.pvpData.iVictoryTimes=1;
//		else
//		{
//			MasterVariableData mvd={0};
//			pDstUser->m_userBase.GetMasterData(mvd);
//			MasterInfo.pvpData.iCurrentContinuousWin=-mvd.pvpData.iCurrentContinuousWin;
//		}
//
//		pDstUser->m_userBase.AddMastData(MasterInfo,SAVETAG_PVP_VICTORYTIMES);
//	}
//	else
//	{
//		if(PVPinfo.PVPFightinginfo.iWiner == WINER_LIFT)
//		{
//			MSC_DB_PVPEnding ending={0};
//			ending.dwUserID=FightingInfo.FightingData[1].dwUserID;
//			threadid = GAME_THREAD_DATABASE + ending.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
//			G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_PVPENDING,(CT_CHAR*)&ending,sizeof(ending));
//
//			MasterData md={0};
//			LookUserInfo UserData={0};
//			pUser->m_userPVPInfo.GetFightingUser(FightingInfo.FightingData[1].dwUserID,UserData);
//			pUser->m_userBase.GetMasterData(md);
//
//			if(UserData.iContinuousWin > 0)
//			{
//				MSC_DB_MSG dbMSG = {0};
//				dbMSG.dwUserID = ending.dwUserID;
//				dbMSG.iMSGType = 2;
//				dbMSG.iTime = time(0);
//				sprintf(dbMSG.cMSG, "{73}<6:%d>{74}<1:%d:%s>{75}",UserData.iContinuousWin,md.dwUserID,md.cNickName);
//				CT_DWORD threadid = GAME_THREAD_DATABASE + dbMSG.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
//				G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_MSG_ADD,(CT_CHAR*)&dbMSG,sizeof(dbMSG));
//			}
//		}
//	}
//
//
//	//连胜统计
//	MasterVariableData addMvd={0};
//	MasterVariableData mvd={0};
//	pUser->m_userBase.GetMasterData(mvd);
//	addMvd.pvpData.iFightTimes=1;
//	CT_CHAR cBroadcastMes[CHAT_LEN] = {0};
//	CT_INT32 iMesLen = 0;
//	MasterData masterData = {0};
//	CT_INT32 iBroadcastType = 5;
//	CT_INT32 iBroadcastTime = 60;
//	G_ValueRange vr={0};
//	G_GetValueRange(VALUE_TYPE_PVPCONTINUOUSWIN,&vr);
//	if(PVPinfo.PVPFightinginfo.iWiner == WINER_LIFT)
//	{
//		memcpy(cBroadcastMes, &iBroadcastType, sizeof(iBroadcastType));
//		memcpy(cBroadcastMes + sizeof(iBroadcastType), &iBroadcastTime, sizeof(iBroadcastTime));
//		addMvd.pvpData.iVictoryTimes=1;
//		addMvd.pvpData.iCurrentContinuousWin=1;
//		if (mvd.pvpData.iCurrentContinuousWin >= 19 && 0==(mvd.pvpData.iCurrentContinuousWin+1)%5)
//		{
//			pUser->m_userBase.GetMasterData(masterData);
//			if (19 == mvd.pvpData.iCurrentContinuousWin)	//20 连胜
//			{
//				sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{43}",pUser->GetUserID(), masterData.cNickName);
//				iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//			}
//
//			if (29 == mvd.pvpData.iCurrentContinuousWin)	//30 连胜
//			{
//				sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{44}",pUser->GetUserID(), masterData.cNickName);
//				iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//			}
//
//			if (39 == mvd.pvpData.iCurrentContinuousWin)	//40 连胜
//			{
//				sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{45}",pUser->GetUserID(), masterData.cNickName);
//				iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//			}
//
//			if (49 == mvd.pvpData.iCurrentContinuousWin)	//50 连胜
//			{
//				sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{46}",pUser->GetUserID(), masterData.cNickName);
//				iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//			}
//
//			if (59 == mvd.pvpData.iCurrentContinuousWin)	//60 连胜
//			{
//				sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{47}",pUser->GetUserID(), masterData.cNickName);
//				iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//			}
//
//			if (69 < mvd.pvpData.iCurrentContinuousWin && 0==(mvd.pvpData.iCurrentContinuousWin+1)%5)	//70 + 5*N 连胜
//			{
//				sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{48}<6:%d>{49}",pUser->GetUserID(), masterData.cNickName, mvd.pvpData.iCurrentContinuousWin+1);
//				iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//				G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//			}
//		}
//		LookUserInfo UserData={0};
//		pUser->m_userPVPInfo.GetFightingUser(FightingInfo.FightingData[1].dwUserID,UserData);
//		if (UserData.iContinuousWin >= vr.fMaxVal && UserData.iContinuousWin > 19)	//达到终结连胜次数公告
//		{
//			pUser->m_userBase.GetMasterData(masterData);
//			sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{30}<1:%d:%s>{17}<6:%d>{72}",pUser->GetUserID(), masterData.cNickName, 
//				UserData.dwUserID, UserData.cUserName, UserData.iContinuousWin);
//			iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//			G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//		}
//	}
//	else
//	{
//		if (mvd.pvpData.iCurrentContinuousWin >= vr.fMaxVal && mvd.pvpData.iCurrentContinuousWin > 19)	//达到终结连胜次数公告
//		{
//			LookUserInfo UserData={0};
//			pUser->m_userPVPInfo.GetFightingUser(FightingInfo.FightingData[1].dwUserID,UserData);
//			pUser->m_userBase.GetMasterData(masterData);
//			sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{30}<1:%d:%s>{17}<6:%d>{72}",UserData.dwUserID, UserData.cUserName, 
//				pUser->GetUserID(), masterData.cNickName, mvd.pvpData.iCurrentContinuousWin);
//			iMesLen = strlen(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime)) + 1 + sizeof(iBroadcastType) + sizeof(iBroadcastTime);
//			G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD, cBroadcastMes,iMesLen);
//		}
//		addMvd.pvpData.iCurrentContinuousWin=-mvd.pvpData.iCurrentContinuousWin;
//	}
//
//	pUser->m_userBase.AddMastData(addMvd,SAVETAG_PVP_VICTORYTIMES);
//
//	//add by caihan
//	//判断竞技场任务
//	CMD_G_TaskEventInfo taskEventInfo = {0};
//	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ARENA;
//	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ARENA_FIGHT;
//	taskEventInfo.iSubType = PVPinfo.PVPFightinginfo.iWiner;
//	pUser->m_userTask.DisposeTask(taskEventInfo);
//	//end
//
//    // 日常任务 - 参加竞技场 [4/7/2013 John]
//   // if (PVPinfo.PVPFightinginfo.iWiner == WINER_LIFT) 
//	{
//        pUser->m_userDaily.UpdateQuestData(DAILY_ARENA_DUEL, 1);
//    }
//
//	return;
//}

CT_VOID  CCPVPEvent::DisposeMSCFightingEnd(CT_CHAR* pBuf,CT_DWORD len)
{
	memset(m_pBuffer, 0, COMPRESS_MAX_SIZE);
	if(len != sizeof(MSC_PVP_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCFightingEnd :len != sizeof(MSC_PVP_Fighting_End)");
		return;
	}

	//更新战斗信息
	CCUser *pUser=0;
	MSC_PVP_Fighting_Info FightingInfo={0};
	MSC_DB_W_PVPFightingInfo PVPinfo={0};
	memcpy(&FightingInfo,pBuf,sizeof(FightingInfo));
	if(!CCUserManager::getSingleton().Find(FightingInfo.dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCFightingEnd:%d No Find UserID",FightingInfo.dwUserID);
		return;
	}

	//竞技场奖励
	PVPFightingReward(pUser,FightingInfo,PVPinfo);
	//连胜统计
	ContinuousWin(pUser,FightingInfo);

	const CT_INT32 iUpDataLen=sizeof(MSC_PVP_Fighting_Info);
	CT_CHAR cPVPBuf[iUpDataLen]={0};
	memcpy(cPVPBuf,pBuf,iUpDataLen);
	pUser->m_userPVPInfo.UpdataFighting(cPVPBuf,iUpDataLen);
	//保存竞技场战报
	pUser->m_userPVPInfo.SaveFightingReacord();

	//更新数据库
	PVPInfo PVPBase={0};
	pUser->m_userPVPInfo.GetPVPInfo(PVPBase,CT_TRUE);

	MSC_DB_W_PVPInfo DBPVPinfo={0};
	DBPVPinfo.dwUserID=pUser->GetUserID();
	DBPVPinfo.iFightingCount=PVPBase.iFightingCount;
	DBPVPinfo.lRefreshTime=PVPBase.lRefreshTime;
	DBPVPinfo.cbChestStatus=PVPBase.cbChestStatus;
	DBPVPinfo.iPVPDegree=PVPBase.iPVPDegree;
	DBPVPinfo.iPVPChestNum=PVPBase.iPVPChestNum;

	CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_R_PVP_INFO,(CT_CHAR*)&DBPVPinfo,sizeof(DBPVPinfo));

	//更新竞技场信息
	CMD_G_PVPInfo SendPVPInfo={0};
	CT_INT32 iLen=sizeof(SendPVPInfo);
	SendPVPInfo.dwUserID=pUser->GetUserID();
	if(pUser->m_userPVPInfo.GetPVPInfo((CT_CHAR*)&SendPVPInfo,iLen))
	{
		CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
		memset(m_pBuffer,0,sizeof(COMPRESS_MAX_SIZE));
		if(CCCompress::getSingleton().Compress((CT_CHAR*)&SendPVPInfo,iLen,m_pBuffer,iOutLen))
		{
			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_INFO, m_pBuffer, iOutLen);
			printf("pvp info\n");
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeMSCFightingEnd Compress Fail!");
		}
	}


	//更新战报
	CMD_G_PVPFightingRecord PVPFightingRecord={0};
	iLen=sizeof(PVPFightingRecord);
	if(pUser->m_userPVPInfo.GetPVPRectord((CT_CHAR*)&PVPFightingRecord,iLen))
	{
		CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
		memset(m_pBuffer,0,sizeof(COMPRESS_MAX_SIZE));
		if(CCCompress::getSingleton().Compress((CT_CHAR*)&PVPFightingRecord,iLen,m_pBuffer,iOutLen))
		{
			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_FIGHTING_RECORD, m_pBuffer, iOutLen);
		}
	}


	//更新对手
	CCUser *pDstUser=0;
	if(CCUserManager::getSingleton().Find(FightingInfo.FightingData[1].dwUserID,&pDstUser))
	{
		pDstUser->m_userPVPInfo.UpdataFighting(cPVPBuf,iUpDataLen);

		//更新对手战报
		memset(&PVPFightingRecord,0,sizeof(PVPFightingRecord));
		iLen=sizeof(PVPFightingRecord);
		if(pDstUser->m_userPVPInfo.GetPVPRectord((CT_CHAR*)&PVPFightingRecord,iLen))
		{
			CT_INT32 iOutLen=COMPRESS_MAX_SIZE;
			memset(m_pBuffer,0,sizeof(COMPRESS_MAX_SIZE));
			if(CCCompress::getSingleton().Compress((CT_CHAR*)&PVPFightingRecord,iLen,m_pBuffer,iOutLen))
			{
				pDstUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_FIGHTING_RECORD, m_pBuffer, iOutLen);
			}
		}

		MasterVariableData MasterInfo={0};
		MasterInfo.pvpData.iFightTimes=1;
		if(PVPinfo.PVPFightinginfo.iWiner == WINER_RIGHT)
			MasterInfo.pvpData.iVictoryTimes=1;
		else
		{
			MasterVariableData mvd={0};
			pDstUser->m_userBase.GetMasterData(mvd);
			MasterInfo.pvpData.iCurrentContinuousWin=-mvd.pvpData.iCurrentContinuousWin;
		}

		pDstUser->m_userBase.AddMastData(MasterInfo,SAVETAG_PVP_VICTORYTIMES);
	}
	else
	{
		if(PVPinfo.PVPFightinginfo.iWiner == WINER_LIFT)
		{
			MSC_DB_PVPEnding ending={0};
			ending.dwUserID=FightingInfo.FightingData[1].dwUserID;
			threadid = GAME_THREAD_DATABASE + ending.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_PVPENDING,(CT_CHAR*)&ending,sizeof(ending));

			MasterData md={0};
			LookUserInfo UserData={0};
			pUser->m_userPVPInfo.GetFightingUser(FightingInfo.FightingData[1].dwUserID,UserData);
			pUser->m_userBase.GetMasterData(md);

			if(UserData.iContinuousWin > 0)
			{
				MSC_DB_MSG dbMSG = {0};
				dbMSG.dwUserID = ending.dwUserID;
				dbMSG.iMSGType = 2;
				dbMSG.iTime = time(0);
				sprintf(dbMSG.cMSG, "{73}<6:%d>{74}<1:%d:%s>{75}",UserData.iContinuousWin,md.dwUserID,md.cNickName);
				CT_DWORD threadid = GAME_THREAD_DATABASE + dbMSG.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_MSG_ADD,(CT_CHAR*)&dbMSG,sizeof(dbMSG));
			}
		}
	}

	//add by caihan
	//判断竞技场任务
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ARENA;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ARENA_FIGHT;
	taskEventInfo.iSubType = PVPinfo.PVPFightinginfo.iWiner;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end

	// 日常任务 - 参加竞技场 [4/7/2013 John]
	// if (PVPinfo.PVPFightinginfo.iWiner == WINER_LIFT) 
	{
		pUser->m_userDaily.UpdateQuestData(DAILY_ARENA_DUEL, 1);
	}

	return;
}

//PVP奖励信息
CT_VOID CCPVPEvent::PVPFightingReward(CCUser *pUser,const MSC_PVP_Fighting_Info &FightingInfo,MSC_DB_W_PVPFightingInfo &PVPFighting)
{
	//竞技度
	G_ValueRange vr={0};
	if(FightingInfo.iDifficulty == HIGH)
	{
		if(!G_GetValueRange(VALUE_TYPE_PVPARWARD_H,&vr))
		{
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::PVPFightingReward G_GetValueRange VALUE_TYPE_PVPARWARD_H");
			return;
		}
	}
	else
	{
		if(!G_GetValueRange(VALUE_TYPE_PVPARWARD_L,&vr))
		{
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::PVPFightingReward G_GetValueRange VALUE_TYPE_PVPARWARD_L");
			return;
		}
	}

	if(FightingInfo.iWiner == WINER_LIFT)
	{
		G_PVPWinReward PVPReward={0};
		MasterData MasterInfo={0};
		CT_INT32 iLen=sizeof(MasterInfo);
		pUser->m_userBase.GetMasterData(MasterInfo);
		if(!G_GetPVPWinReward(MasterInfo.mvd.stRank.iLevel,FightingInfo.iDifficulty,&PVPReward))
		{
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
			CCLogMeg::getSingleton().WriteError("读取PVP胜利脚本错误！RankGrade:%d Difficulty:%d",MasterInfo.mvd.stRank.iLevel,FightingInfo.iDifficulty);
			return;
		}

		MasterVariableData MasterReward={0};
		MasterReward.stRes.iGold+=G_GetPVPGold(PVPReward.iGold,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		MasterReward.stGrade.iCurrentExp+=G_GetPVPExperience(PVPReward.fExpCoefficient,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		MasterReward.stRank.iCurrentExp+=PVPReward.iPrestige;

		// 更新领主资源
		pUser->m_userBase.AddMastData(MasterReward, SAVETAG_WORLD_SCENE_REWARD);

		//道具奖励
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;

		if(PVPReward.iGoodsID1 != 0 && PVPReward.iCount1 != 0)
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID1,PVPReward.iCount1);
		if(PVPReward.iGoodsID2 != 0 && PVPReward.iCount2 != 0)
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID2,PVPReward.iCount2);
		if(PVPReward.iGoodsID3 != 0 && PVPReward.iCount3 != 0)
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID3,PVPReward.iCount3);
		if(PVPReward.iGoodsID4 != 0 && PVPReward.iCount4 != 0)
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID4,PVPReward.iCount4);

		//更新记录
		PVPFightingInfo FightingRectord={0};
		FightingRectord.dwUserID=FightingInfo.FightingData[1].dwUserID;
		FightingRectord.dwChallengUserID=pUser->GetUserID();
		FightingRectord.iWiner=FightingInfo.iWiner;

		FightingRectord.iGoods[0]=PVPReward.iGoodsID1;
		FightingRectord.iGoods[1]=PVPReward.iGoodsID2;
		FightingRectord.iGoods[2]=PVPReward.iGoodsID3;
		FightingRectord.iGoods[3]=PVPReward.iGoodsID4;

		FightingRectord.iCount[0]=PVPReward.iCount1;
		FightingRectord.iCount[1]=PVPReward.iCount2;
		FightingRectord.iCount[2]=PVPReward.iCount3;
		FightingRectord.iCount[3]=PVPReward.iCount4;

		FightingRectord.iGold=MasterReward.stRes.iGold;
		FightingRectord.iPrestige=PVPReward.iPrestige;

		CombatUnitData data={0};
		G_GetMasterFightingData(FightingInfo.FightingData[1],data);

		for(CT_INT32 i=0;i<POST_COUNT/2;++i)
		{
			CT_INT32 iType=FightingInfo.FightingData[1].units[i].type;
			if(iType == CLASS_TYPE_WARRIOR || iType == CLASS_TYPE_MAGE || iType == CLASS_TYPE_ARCHER)
			{
				memcpy(FightingRectord.cUserName,data.name,sizeof(FightingRectord.cUserName));
				break;
			}
		}

		pUser->m_userPVPInfo.UpdataPVPRectord(FightingRectord);

		//记录数据库
		PVPFighting.iDifficulty=FightingInfo.iDifficulty;
		PVPFighting.PVPFightinginfo.dwChallengUserID=pUser->GetUserID();
		memcpy(&PVPFighting.PVPFightinginfo,&FightingRectord,sizeof(FightingRectord));

		//更新对手记录
		CCUser *pDstUser=0;
		if(CCUserManager::getSingleton().Find(FightingInfo.FightingData[1].dwUserID,&pDstUser))
		{
			PVPFightingInfo FightingRectord={0};
			FightingRectord.dwUserID=FightingInfo.FightingData[0].dwUserID;
			FightingRectord.dwChallengUserID=pUser->GetUserID();
			FightingRectord.iWiner=WINER_RIGHT;

			FightingRectord.iGoods[0]=PVPReward.iGoodsID1;
			FightingRectord.iGoods[1]=PVPReward.iGoodsID2;
			FightingRectord.iGoods[2]=PVPReward.iGoodsID3;
			FightingRectord.iGoods[3]=PVPReward.iGoodsID4;

			FightingRectord.iCount[0]=PVPReward.iCount1;
			FightingRectord.iCount[1]=PVPReward.iCount2;
			FightingRectord.iCount[2]=PVPReward.iCount3;
			FightingRectord.iCount[3]=PVPReward.iCount4;

			FightingRectord.iGold=PVPReward.iGold;
			FightingRectord.iPrestige=PVPReward.iPrestige;

			CombatUnitData data={0};
			G_GetMasterFightingData(FightingInfo.FightingData[0],data);
			memcpy(FightingRectord.cUserName,data.name,sizeof(FightingRectord.cUserName));
			pDstUser->m_userPVPInfo.UpdataPVPRectord(FightingRectord);
		}

		//更新缓存
		pUser->m_userPVPInfo.UpdataChallengUser(FightingRectord.dwUserID,FightingInfo.iDifficulty,STATUS_WIN);

		//发送客户端
		CMD_G_PVPFightingReward Reward={0};
		Reward.dwUserID=pUser->GetUserID();
		Reward.iGold=G_GetPVPGold(PVPReward.iGold,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		Reward.iExp=G_GetPVPExperience(PVPReward.fExpCoefficient,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		Reward.iRankExp=PVPReward.iPrestige;
		Reward.iPVPDegree=(CT_INT32)vr.fMaxVal;

		Reward.iGoods[0]=PVPReward.iGoodsID1;
		Reward.iGoods[1]=PVPReward.iGoodsID2;
		Reward.iGoods[2]=PVPReward.iGoodsID3;
		Reward.iGoods[3]=PVPReward.iGoodsID4;

		Reward.iCount[0]=PVPReward.iCount1;
		Reward.iCount[1]=PVPReward.iCount2;
		Reward.iCount[2]=PVPReward.iCount3;
		Reward.iCount[3]=PVPReward.iCount4;

		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_PVP_FIGHTING_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));

		pUser->m_userPVPInfo.SetPVPDegree((CT_INT32)vr.fMaxVal);

	}
	else
	{
		G_PVPWinReward PVPReward={0};
		MasterData MasterInfo={0};
		CT_INT32 iLen=sizeof(MasterInfo);
		pUser->m_userBase.GetMasterData(MasterInfo);
		if(!G_GetPVPLostReward(MasterInfo.mvd.stRank.iLevel,FightingInfo.iDifficulty,&PVPReward))
		{
			CCLogMeg::getSingleton().WriteError("读取PVP失败脚本错误！");
			return;
		}

		MasterVariableData MasterReward={0};
		MasterReward.stRes.iGold+=G_GetPVPGold(PVPReward.iGold,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		MasterReward.stGrade.iCurrentExp+=G_GetPVPExperience(PVPReward.fExpCoefficient,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		MasterReward.stRank.iCurrentExp+=PVPReward.iPrestige;

		// 更新领主资源
		MasterVariableData addValue = {0};
		pUser->m_userBase.AddMastData(MasterReward, SAVETAG_WORLD_SCENE_REWARD);

		//道具奖励
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;

		CT_INT32 iCount=PVPReward.iCount1;
		if(PVPReward.iGoodsID1 != 0 && PVPReward.iCount1 != 0)
		{
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID1,iCount);
		}
		if(PVPReward.iGoodsID2 != 0 && PVPReward.iCount2 != 0)
		{
			iCount=PVPReward.iCount2;
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID2,PVPReward.iCount2);
		}
		if(PVPReward.iGoodsID3 != 0 && PVPReward.iCount3 != 0)
		{
			iCount=PVPReward.iCount3;
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID3,PVPReward.iCount3);
		}
		if(PVPReward.iGoodsID4 != 0 && PVPReward.iCount4 != 0)
		{
			iCount=PVPReward.iCount4;
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,PVPReward.iGoodsID4,PVPReward.iCount4);
		}

		//更新记录
		PVPFightingInfo FightingRectord={0};
		FightingRectord.dwUserID=FightingInfo.FightingData[1].dwUserID;
		FightingRectord.dwChallengUserID=pUser->GetUserID();
		FightingRectord.iWiner=FightingInfo.iWiner;

		FightingRectord.iGoods[0]=PVPReward.iGoodsID1;
		FightingRectord.iGoods[1]=PVPReward.iGoodsID2;
		FightingRectord.iGoods[2]=PVPReward.iGoodsID3;
		FightingRectord.iGoods[3]=PVPReward.iGoodsID4;

		FightingRectord.iCount[0]=PVPReward.iCount1;
		FightingRectord.iCount[1]=PVPReward.iCount2;
		FightingRectord.iCount[2]=PVPReward.iCount3;
		FightingRectord.iCount[3]=PVPReward.iCount4;

		FightingRectord.iGold=MasterReward.stRes.iGold;
		FightingRectord.iPrestige=PVPReward.iPrestige;

		CombatUnitData data={0};
		G_GetMasterFightingData(FightingInfo.FightingData[1],data);
		memcpy(FightingRectord.cUserName,data.name,sizeof(FightingRectord.cUserName));
		pUser->m_userPVPInfo.UpdataPVPRectord(FightingRectord);

		//记录数据库
		PVPFighting.iDifficulty=FightingInfo.iDifficulty;
		PVPFighting.PVPFightinginfo.dwChallengUserID=pUser->GetUserID();
		memcpy(&PVPFighting.PVPFightinginfo,&FightingRectord,sizeof(FightingRectord));

		//更新对手记录
		CCUser *pDstUser=0;
		if(CCUserManager::getSingleton().Find(FightingInfo.FightingData[1].dwUserID,&pDstUser))
		{
			PVPFightingInfo FightingRectord={0};
			FightingRectord.dwUserID=FightingInfo.FightingData[0].dwUserID;
			FightingRectord.dwChallengUserID=pUser->GetUserID();
			FightingRectord.iWiner=WINER_LIFT;

			FightingRectord.iGoods[0]=PVPReward.iGoodsID1;
			FightingRectord.iGoods[1]=PVPReward.iGoodsID2;
			FightingRectord.iGoods[2]=PVPReward.iGoodsID3;
			FightingRectord.iGoods[3]=PVPReward.iGoodsID4;

			FightingRectord.iCount[0]=PVPReward.iCount1;
			FightingRectord.iCount[1]=PVPReward.iCount2;
			FightingRectord.iCount[2]=PVPReward.iCount3;
			FightingRectord.iCount[3]=PVPReward.iCount4;

			FightingRectord.iGold=PVPReward.iGold;
			FightingRectord.iPrestige=PVPReward.iPrestige;

			CombatUnitData data={0};
			G_GetMasterFightingData(FightingInfo.FightingData[0],data);
			memcpy(FightingRectord.cUserName,data.name,sizeof(FightingRectord.cUserName));
			pDstUser->m_userPVPInfo.UpdataPVPRectord(FightingRectord);
		}

		//更新缓存
		pUser->m_userPVPInfo.UpdataChallengUser(FightingRectord.dwUserID,FightingInfo.iDifficulty,STATUS_LOSE);

		//发送客户端
		CMD_G_PVPFightingReward Reward={0};
		Reward.dwUserID=pUser->GetUserID();
		Reward.iGold=G_GetPVPGold(PVPReward.iGold,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		Reward.iExp=G_GetPVPExperience(PVPReward.fExpCoefficient,MasterInfo.mvd.stGrade.iLevel,MasterInfo.factor.fBattleFactor);
		Reward.iRankExp=PVPReward.iPrestige;
		Reward.iPVPDegree=(CT_INT32)vr.fMaxVal;

		Reward.iGoods[0]=PVPReward.iGoodsID1;
		Reward.iGoods[1]=PVPReward.iGoodsID2;
		Reward.iGoods[2]=PVPReward.iGoodsID3;
		Reward.iGoods[3]=PVPReward.iGoodsID4;

		Reward.iCount[0]=PVPReward.iCount1;
		Reward.iCount[1]=PVPReward.iCount2;
		Reward.iCount[2]=PVPReward.iCount3;
		Reward.iCount[3]=PVPReward.iCount4;

		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_PVP_FIGHTING_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));

		pUser->m_userPVPInfo.SetPVPDegree((CT_INT32)vr.fMinVal);
	}

	//记录数据库
	MSC_DB_ChallengeInfo ChallengeInfo={0};
	ChallengeInfo.dwUserID=pUser->GetUserID();
	ChallengeInfo.iDifficulty=FightingInfo.iDifficulty;
	pUser->m_userPVPInfo.GetChallengeInfo(FightingInfo.iDifficulty,ChallengeInfo.bWiner);
	pUser->m_userPVPInfo.GetChallengeID(FightingInfo.iDifficulty,ChallengeInfo.dwDstUserID);

	CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_CHALLENGE_INFO,(CT_CHAR*)&ChallengeInfo,sizeof(ChallengeInfo));

	return;
}

//连胜记录
CT_VOID CCPVPEvent::ContinuousWin(CCUser *pUser,const MSC_PVP_Fighting_Info &FightingInfo)
{
	//连胜统计
	MasterVariableData addMvd={0};
	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);
	addMvd.pvpData.iFightTimes=1;
	MasterData masterData = {0};
	G_ValueRange vr={0};
	G_GetValueRange(VALUE_TYPE_PVPCONTINUOUSWIN,&vr);
	if(FightingInfo.iWiner == WINER_LIFT)
	{
		addMvd.pvpData.iVictoryTimes=1;
		addMvd.pvpData.iCurrentContinuousWin=1;
		if (mvd.pvpData.iCurrentContinuousWin >= 19 && 0==(mvd.pvpData.iCurrentContinuousWin+1)%5)
		{
			RollBroadcast rb={0};
			pUser->m_userBase.GetMasterData(masterData);
			rb.SetValue(pUser->GetUserID(), masterData.cNickName);
			if (19 == mvd.pvpData.iCurrentContinuousWin)	//20 连胜
			{
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{43}",pUser->GetUserID(), masterData.cNickName);
				G_SendRollBroadecast(43,5,60,(CT_CHAR*)&rb,sizeof(rb));
			}

			if (29 == mvd.pvpData.iCurrentContinuousWin)	//30 连胜
			{
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{44}",pUser->GetUserID(), masterData.cNickName);
				G_SendRollBroadecast(44,5,60,(CT_CHAR*)&rb,sizeof(rb));
			}

			if (39 == mvd.pvpData.iCurrentContinuousWin)	//40 连胜
			{
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{45}",pUser->GetUserID(), masterData.cNickName);
				G_SendRollBroadecast(45,5,60,(CT_CHAR*)&rb,sizeof(rb));
			}

			if (49 == mvd.pvpData.iCurrentContinuousWin)	//50 连胜
			{
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{46}",pUser->GetUserID(), masterData.cNickName);
				G_SendRollBroadecast(46,5,60,(CT_CHAR*)&rb,sizeof(rb));
			}

			if (59 == mvd.pvpData.iCurrentContinuousWin)	//60 连胜
			{
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{47}",pUser->GetUserID(), masterData.cNickName);
				G_SendRollBroadecast(47,5,60,(CT_CHAR*)&rb,sizeof(rb));
			}

			if (69 < mvd.pvpData.iCurrentContinuousWin && 0==(mvd.pvpData.iCurrentContinuousWin+1)%5)	//70 + 5*N 连胜
			{
				RollBroadcast rb1[2]={0};
				rb1[0].SetValue(pUser->GetUserID(), masterData.cNickName);
				rb1[1].SetValue(mvd.pvpData.iCurrentContinuousWin+1,"");
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{48}<6:%d>{49}",pUser->GetUserID(), masterData.cNickName, mvd.pvpData.iCurrentContinuousWin+1);
				G_SendRollBroadecast(48,5,60,(CT_CHAR*)rb1,sizeof(rb1));
			}
		}

		FightingData fd={0};
		CCFightingData::getSingleton().GetFightingData(FightingInfo.FightingData[1].dwUserID,fd);
		if (fd.LookData.iContinuousWin >= vr.fMaxVal && fd.LookData.iContinuousWin > 19)	//达到终结连胜次数公告
		{
			RollBroadcast rb1[3]={0};
			pUser->m_userBase.GetMasterData(masterData);
			rb1[0].SetValue(pUser->GetUserID(), masterData.cNickName);
			rb1[1].SetValue(fd.LookData.dwUserID, fd.LookData.cUserName);
			rb1[2].SetValue(fd.LookData.iContinuousWin,"");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{30}<1:%d:%s>{17}<6:%d>{72}",pUser->GetUserID(), masterData.cNickName, 
				//fd.LookData.dwUserID, fd.LookData.cUserName, fd.LookData.iContinuousWin);
			G_SendRollBroadecast(30,5,60,(CT_CHAR*)rb1,sizeof(rb1));
		}
	}
	else
	{
		if (mvd.pvpData.iCurrentContinuousWin >= vr.fMaxVal && mvd.pvpData.iCurrentContinuousWin > 19)	//达到终结连胜次数公告
		{
			FightingData fd={0};
			RollBroadcast rb1[3]={0};
			pUser->m_userBase.GetMasterData(masterData);
			CCFightingData::getSingleton().GetFightingData(FightingInfo.FightingData[1].dwUserID,fd);
			rb1[0].SetValue(fd.LookData.dwUserID, fd.LookData.cUserName);
			rb1[1].SetValue(pUser->GetUserID(), masterData.cNickName);
			rb1[2].SetValue(mvd.pvpData.iCurrentContinuousWin,"");
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{30}<1:%d:%s>{17}<6:%d>{72}",fd.LookData.dwUserID, fd.LookData.cUserName, 
				//pUser->GetUserID(), masterData.cNickName, mvd.pvpData.iCurrentContinuousWin);
			G_SendRollBroadecast(30,5,60,(CT_CHAR*)rb1,sizeof(rb1));
		}
		addMvd.pvpData.iCurrentContinuousWin=-mvd.pvpData.iCurrentContinuousWin;
	}

	pUser->m_userBase.AddMastData(addMvd,SAVETAG_PVP_VICTORYTIMES);
	CCFightingData::getSingleton().SetPVPConWin((FightingInfo.iWiner == WINER_LIFT?FightingInfo.FightingData[1].dwUserID:FightingInfo.FightingData[0].dwUserID));
	return;
}
//竞技场验证
CT_BOOL CCPVPEvent::PVPFightingValidate(CCUser *pUser,CT_INT32 iDiamond)
{
	//挑战次数验证
	G_ValueRange FightingValue={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTINGCOUNT,&FightingValue))
	{
		CMD_G_PVPErrorInfo PVPErrorInfo={0};
		PVPErrorInfo.dwUserID=pUser->GetUserID();
		PVPErrorInfo.iErrorInfo=PVP_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));

		CCLogMeg::getSingleton().WriteError("CCPVPEvent::PVPFightingValidate:%d",VALUE_TYPE_PVPFIGHTINGCOUNT);
		return CT_FALSE;
	}
	//vip add 增加额外挑战次数
	if(pUser->m_userPVPInfo.GetFightingCount() >= FightingValue.fMinVal + pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_ARENA))
	{
		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTING,&vr))
		{
			CMD_G_PVPErrorInfo PVPErrorInfo={0};
			PVPErrorInfo.dwUserID=pUser->GetUserID();
			PVPErrorInfo.iErrorInfo=PVP_ERROR;
			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));

			CCLogMeg::getSingleton().WriteError("CCPVPEvent::PVPFightingValidate:%d",VALUE_TYPE_PVPFIGHTING);
			return CT_FALSE;

		}

		G_ValueRange vrmax={0};
		if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTINGMAX,&vrmax))
		{
			CMD_G_PVPErrorInfo PVPErrorInfo={0};
			PVPErrorInfo.dwUserID=pUser->GetUserID();
			PVPErrorInfo.iErrorInfo=PVP_ERROR;
			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));

			CCLogMeg::getSingleton().WriteError("CCPVPEvent::PVPFightingValidate:%d",VALUE_TYPE_PVPFIGHTINGMAX);
			return CT_FALSE;

		}
		CT_INT32 iNum=(CT_INT32)(pUser->m_userPVPInfo.GetFightingCount()-FightingValue.fMinVal-pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_ARENA)+1);
		CT_INT32 iDiamodCost=G_GetPVPFightingDiamod(( iNum > 0?iNum:0),vr.fMinVal);
		iDiamodCost=(-iDiamodCost > -vrmax.fMinVal?vrmax.fMinVal:iDiamodCost);

		//最大竞技次数
		if(pUser->m_userPVPInfo.GetFightingCount() >= FightingValue.fMaxVal)
		{
			CMD_G_PVPErrorInfo PVPInfo={0};
			PVPInfo.dwUserID=pUser->GetUserID();
			PVPInfo.iErrorInfo=FIGHTING_MAX;
			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPInfo,sizeof(PVPInfo));
			return CT_FALSE;
		}

		//付费挑战
		if(-iDiamond >= -iDiamodCost)
		{
			MasterVariableData mvd={0};
			pUser->m_userBase.GetMasterData(mvd);
			if(mvd.stRes.iDiamod < -iDiamodCost)
			{
				CMD_G_PVPErrorInfo PVPInfo={0};
				PVPInfo.dwUserID=pUser->GetUserID();
				PVPInfo.iErrorInfo=NO_DIAMOD;
				pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPInfo,sizeof(PVPInfo));
				return CT_FALSE;
			}

			MasterVariableData Master={0};
			Master.stRes.iDiamod+=iDiamodCost;
			pUser->m_userBase.AddMastData(Master,SAVETAG_PVP_COST);
		}
		else
		{
			CMD_G_PVPErrorInfo PVPInfo={0};
			PVPInfo.dwUserID=pUser->GetUserID();
			PVPInfo.iErrorInfo=PAY_FINGHTING;
			PVPInfo.iPayDiamond=(CT_INT32)((-vrmax.fMinVal<-iDiamodCost)?vrmax.fMinVal:iDiamodCost);
			pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPInfo,sizeof(PVPInfo));
			return CT_FALSE;
		}
	}

	//体力验证
	G_ValueRange PowerValue={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPPOWER,&PowerValue))
	{
		CMD_G_PVPErrorInfo PVPErrorInfo={0};
		PVPErrorInfo.dwUserID=pUser->GetUserID();
		PVPErrorInfo.iErrorInfo=PVP_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPErrorInfo,sizeof(PVPErrorInfo));

		CCLogMeg::getSingleton().WriteError("CCPVPEvent::PVPFightingValidate:%d",VALUE_TYPE_PVPPOWER);
		return CT_FALSE;
	}

	MasterVariableData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(MasterInfo.stRes.iStamina < -PowerValue.fMinVal)
	{
		CMD_G_PVPErrorInfo PVPInfo={0};
		PVPInfo.dwUserID=pUser->GetUserID();
		PVPInfo.iErrorInfo=NO_POWER;
		pUser->GetNetInterface()->Send(MAIN_G_PVP_EVENT,SUB_G_PVP_ERROR_INFO,(CT_CHAR*)&PVPInfo,sizeof(PVPInfo));
		return CT_FALSE;
	}

	//体力消耗
	MasterVariableData Master={0};
	Master.stRes.iStamina+=(CT_INT32)PowerValue.fMinVal;
	pUser->m_userBase.AddMastData(Master,SAVETAG_PVP_COST);

	//挑战次数(额外购买次数不记录)
	//if(iDiamond == 0)
		pUser->m_userPVPInfo.SetFightingCount();

	//更新数据库
	PVPInfo PVPinfo={0};
	pUser->m_userPVPInfo.GetPVPInfo(PVPinfo,CT_TRUE);

	MSC_DB_W_PVPInfo DBPVPinfo={0};
	DBPVPinfo.dwUserID=pUser->GetUserID();
	DBPVPinfo.iFightingCount=PVPinfo.iFightingCount;
	DBPVPinfo.lRefreshTime=PVPinfo.lRefreshTime;
	DBPVPinfo.cbChestStatus=PVPinfo.cbChestStatus;
	DBPVPinfo.iPVPDegree=PVPinfo.iPVPDegree;
	DBPVPinfo.iPVPChestNum=PVPinfo.iPVPChestNum;

	CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_PVP_DATA,MSC_SUB_DB_R_PVP_INFO,(CT_CHAR*)&DBPVPinfo,sizeof(DBPVPinfo));

	//记录日志数据
	MSC_LOGDB_PVPFighringCost PVPFighringCost={0};
	PVPFighringCost.dwUserID=pUser->GetUserID();
	PVPFighringCost.iUserGrade = MasterInfo.stGrade.iLevel;
	PVPFighringCost.iDiamod=iDiamond;
	PVPFighringCost.iStamina=(CT_INT32)PowerValue.fMinVal;
	threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PVPFIGHTINGCOST,(CT_CHAR*)&PVPFighringCost,sizeof(PVPFighringCost));
	return CT_TRUE;
}

//插入对手数据
CT_VOID CCPVPEvent::DisposeChallgerInsert(CT_CHAR* pBuf, CT_DWORD len, CT_BOOL bHign )
{
	if (len != sizeof(MSC_PVP_InsertChallger))
	{
		return;
	}
	MSC_PVP_InsertChallger* pPvp = (MSC_PVP_InsertChallger*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pPvp->dwUserID,&pUser))
	{
		return;
	}

	if (bHign)
	{
		pUser->m_userPVPInfo.InsertHUserID(pPvp->iPos , pPvp->dwChallger);
	}else
	{
		pUser->m_userPVPInfo.InsertLUserID(pPvp->iPos , pPvp->dwChallger);
	}
	
}


//对手数据插入完成
CT_VOID CCPVPEvent::DisposeInsertChallgerFinish(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CT_DWORD))
	{
		return;
	}

	CT_DWORD dwUserID = 0;
	memcpy(&dwUserID, pBuf, sizeof(dwUserID));

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPVPEvent::DisposeInsertChallgerFinish:%d No Find UserID",dwUserID);
		return;
	}

	//查找用户信息
	PVPInfo PVPinfo={0};
	pUser->m_userPVPInfo.GetPVPInfo(PVPinfo);


	CT_DWORD dbThreadID = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;

	MSC_DB_PVPChallgerSave pvpChallgerSave = {0};
	pvpChallgerSave.dwUserID = dwUserID;
	pvpChallgerSave.pvpInfo  = PVPinfo;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_PVP_DATA, MSC_SUB_DB_PVP_CHALLGER_SAVE, (CT_CHAR*)&pvpChallgerSave, sizeof(pvpChallgerSave));


	MSC_DB_Challeng Challeng={0};
	Challeng.dwUserID=pUser->GetUserID();
	memcpy(Challeng.dwChalleng,PVPinfo.dwChallengUserID,sizeof(PVPinfo.dwChallengUserID));
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_CHALLENG, (CT_CHAR*)&Challeng, sizeof(Challeng));

	//更新对手状态
	pUser->m_userPVPInfo.UpdataChallengUser(0,0,STATUS_NULL);
}



