#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CDuplicateEvent.h"
#include "CUserManager.h"
#include "CFightingScriptSetting.h"
#include "CRand.h"
#include "CFightingRecord.h"
#include "CActive.h"
#include "CBroadcastScriptSetting.h"

#define   RAIDS_REWARD_SIZE			5*1024

CCDuplicateEvent1::CCDuplicateEvent1(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{;
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_ENTER_SCENE);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_GET_REWARD);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_LOOK_FIGHTING);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_DUPLICATE_RAIDS);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_RAIDS_SPEED);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_REQUEST_REWARD);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_DUPLICATE_RAIDS);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_RAIDS_SPEED);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_REQUEST_REWARD);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUG_G_RAIDS_FAIL);
	RegistMessage(MAIN_G_DUPLICATE_EVENT, SUB_G_RAIDS_SPEED_DIAMOD);

	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_FIGHTING_END);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_RAIDS_REWARD);
	RegistMessage(MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_FIGHTING_REWARD);

}

CCDuplicateEvent1::~CCDuplicateEvent1(void)
{
}

//�����û���Ϣ
CT_VOID CCDuplicateEvent1::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	if(MAIN_G_DUPLICATE_EVENT == mainID)
	{
		DisposeNetMessage(subID,pBuf,len);
		return;
	}
	else if(MSC_MAIN_DUPLICATE_EVENT == mainID)
	{
		DisposeThreadMessage(subID,pBuf,len);
		return;
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::OnUserEvent : mainID=%i, subID=%i", mainID, subID);
		return;
	}
	return;
}

//������Ϣ
CT_VOID CCDuplicateEvent1::DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case SUB_G_ENTER_SCENE:						//����ս������
		{
			DisposeEnterScene(pBuf,len);
			break;
		}
	case SUB_G_GET_REWARD:						//��ȡ����
		{
			DisposeGetReward(pBuf,len);
			break;
		}
	case SUB_G_LOOK_FIGHTING:					//��ս
		{
			DisposeLookFighting(pBuf,len);
			break;
		}
	case SUB_G_DUPLICATE_RAIDS:					//����ɨ��
		{
			DisposeDuplicateRaids(pBuf,len);
			break;
		}
	case SUB_G_RAIDS_SPEED:						//ɨ������
		{
			DisposeRaidsSpeed(pBuf,len);
			break;
		}
	case SUB_G_REQUEST_REWARD:					//������
		{
			DisposeRequestReward(pBuf,len);
			break;
		}
	case SUG_G_RAIDS_FAIL:						//ɨ��ʧ��
		{
			DisposeRaidsFail(pBuf,len);
			break;
		}
	case SUB_G_RAIDS_SPEED_DIAMOD:
		{
			DisposeRaidsSpeedDiamod(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeNetMessage :subID=%i", subID);
			return;
		}
	}
	return ;
}

//�߳���Ϣ
CT_VOID CCDuplicateEvent1::DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case MSC_SUB_FIGHTING_END:				//ս������
		{
			DisposeFightingEnd(pBuf,len);
			break;
		}
	case MSC_SUB_RAIDS_REWARD:
		{
			DisposeRaidsReward(pBuf,len);
			break;
		}
	case MSC_SUB_FIGHTING_REWARD:
		{
			DisposeFightingReward(pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeThreadMessage :subID=%i",subID);
			return;
		}
	}
	return;
}

//���볡��
CT_BOOL CCDuplicateEvent1::DisposeEnterScene(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_EnterScene))
	{
		CCLogMeg::getSingleton().WriteError("len != sizeof(CMD_G_EnterScene),%s %d",__FILE__,__LINE__);
		return CT_FALSE;
	}

	CMD_G_EnterScene *pEnterScene=(CMD_G_EnterScene*)pBuf;

	CCUser *pUser=NULL;
	if(!CCUserManager::getSingleton().Find(pEnterScene->dwUserID, &pUser))
	{
		printf("%s %d",__FILE__,__LINE__);
		return CT_FALSE;
	}

	//��ȡ������Ϣ
	SceneInfo si={0};
	if(!pUser->m_userDuplicate.GetSceneInfo(pEnterScene->iWorldSceneID,pEnterScene->iSceneID,pEnterScene->iDifficulty,si))
	{
		CMD_G_ErrorScene ErrorScene={0};
		ErrorScene.dwUserID=pUser->GetUserID();
		ErrorScene.cbErrorScene=ERROR_CONDITION;
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_SCENE_ERROR,(CT_CHAR*)&ErrorScene,sizeof(ErrorScene));
		return CT_FALSE;
	}

	//�����ж�
	CT_INT32 iEnterValidate=EnterSceneValidate(si,pUser,pEnterScene->iDiamon);
	if(iEnterValidate != 0)
	{
		if(iEnterValidate != ERROR_TASK && iEnterValidate != ERROR_NO_NUM && iEnterValidate != ERROR_PPHYSICAL)
		{
			CMD_G_ErrorScene ErrorScene={0};
			ErrorScene.dwUserID=pUser->GetUserID();
			ErrorScene.cbErrorScene=iEnterValidate;
			pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_SCENE_ERROR,(CT_CHAR*)&ErrorScene,sizeof(ErrorScene));
		}
		return CT_TRUE;
	}

	MSC_Fighting_Info FightingInfo={0};
	FightingInfo.dwUserID=pEnterScene->dwUserID;
	FightingInfo.iWorldSceneID=pEnterScene->iWorldSceneID;
	FightingInfo.iSceneID=pEnterScene->iSceneID;
	FightingInfo.iDifficulty=pEnterScene->iDifficulty;
	FightingInfo.iSeed=rand()%RAND_MAX_EX;
	G_GetFightingData(pUser,FightingInfo.FightingData);
	pUser->m_userArmy.DeleteBoss();

	CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_FIGHTING_EVENT, (CT_CHAR*)&FightingInfo, sizeof(FightingInfo));
	return CT_TRUE;
}

//��ȡ����
CT_BOOL CCDuplicateEvent1::DisposeGetReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_GetReward))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::DisposeGetReward len != sizeof(CMD_G_GetReward)");
		return CT_FALSE;
	}

	CMD_G_GetReward *pGetReward=(CMD_G_GetReward*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pGetReward->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeGetReward No Find UserID:%d",pGetReward->dwUserID);
		return CT_FALSE;
	}

	CT_INT32 iReward=pUser->m_userDuplicate.GetWorldReward(pGetReward->iWorldSceneID,pGetReward->iRewardID);

	//��ȡ����
	if(iReward == GET_REWARD)
	{
		G_WorldSceneReward WorldReward={0};
		CCFightingScriptSetting::getSingleton().GetWorldSceneData(pGetReward->iWorldSceneID,pGetReward->iRewardID,WorldReward);

		MasterVariableData MasterReward={0};
		MasterReward.stRes.iDiamod+=WorldReward.iDiamond;
		MasterReward.stRes.iGold+=WorldReward.iGold;
		MasterReward.stRes.iSkillSoul+=WorldReward.iSkillPrint;
		MasterReward.stGrade.iCurrentExp+=WorldReward.iExperience;
		MasterReward.stRes.iTalentNum+=WorldReward.iPotential;

		// ����������Դ
		MasterVariableData addValue = {0};
		pUser->m_userBase.AddMastData(MasterReward, SAVETAG_WORLD_SCENE_REWARD);

		//���߽���
		if(WorldReward.iGoodsID1 != 0 && WorldReward.iCount1 != 0)
		{
			CT_INT32 iCount=WorldReward.iCount1;
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,WorldReward.iGoodsID1,iCount);
		}
		if(WorldReward.iGoodsID2 != 0 && WorldReward.iCount2 != 0)
		{
			CT_INT32 iCount=WorldReward.iCount2;
			pUser->m_userItem.AutoAddItemData(SAVETAG_WORLD_SCENE_REWARD,WorldReward.iGoodsID2,iCount);
		}

		pUser->m_userDuplicate.UpdataSceneReward(pGetReward->iWorldSceneID,pGetReward->iRewardID);

		//��ȡ�ɹ�
		MasterData MasterInfo={0};
		pUser->m_userBase.GetMasterData(MasterInfo);
		CMD_G_GetRewardFinish GetRewardFinish={0};
		GetRewardFinish.iDiamond=MasterInfo.mvd.stRes.iDiamod;
		GetRewardFinish.iGold=MasterInfo.mvd.stRes.iGold;
		GetRewardFinish.iSkillPrint=MasterInfo.mvd.stRes.iSkillSoul;
		GetRewardFinish.iExperience=MasterInfo.mvd.stGrade.iCurrentExp;
		GetRewardFinish.iPotential=MasterInfo.mvd.stRes.iTalentNum;
		GetRewardFinish.iRewardID=pGetReward->iRewardID;
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_GET_REWARD_FINISH,(CT_CHAR*)&GetRewardFinish,sizeof(GetRewardFinish));

		//�������ݿ�
		MSC_DB_WorldSceneInfo WorldSceneInfo={0};
		WorldSceneInfo.dwUserID=pUser->GetUserID();
		WorldSceneInfo.iSceneID=pGetReward->iWorldSceneID;
		WorldSceneInfo.iReward=pGetReward->iRewardID;

		CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_R_DUPLICATE_WORLD_SCENE,(CT_CHAR*)&WorldSceneInfo,sizeof(WorldSceneInfo));

		//��¼��־���ݿ�
		MSC_LOGDB_DuplicateReward LogdbReward={0};
		LogdbReward.dwUserID=pUser->GetUserID();
		LogdbReward.iUserGrade  = MasterInfo.mvd.stGrade.iLevel;
		LogdbReward.iWorldSceneID=pGetReward->iWorldSceneID;
		LogdbReward.iRewardID=pGetReward->iRewardID;	
		LogdbReward.iExperience=WorldReward.iExperience;
		LogdbReward.iGold=WorldReward.iGold;		
		LogdbReward.iScore=pUser->m_userDuplicate.GetSceneScore(pGetReward->iWorldSceneID);		
		LogdbReward.iGoodsID1=WorldReward.iGoodsID1;	
		LogdbReward.iCount1=WorldReward.iCount1;	
		LogdbReward.iGoodsID2=WorldReward.iGoodsID2;	
		LogdbReward.iCount2=WorldReward.iCount2;	
		LogdbReward.iPotential=WorldReward.iPotential;	
		LogdbReward.iDiamond=WorldReward.iDiamond;	
		LogdbReward.iSkillPrint=WorldReward.iSkillPrint;

		threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATEREWARD,(CT_CHAR*)&LogdbReward,sizeof(LogdbReward));

	}
	else 
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeGetReward iReward != GET_REWARD");
		pUser->CloseNet();
		return CT_FALSE;
	}

	return CT_TRUE;
}

//��ս
CT_VOID CCDuplicateEvent1::DisposeLookFighting(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_LookFighting))
	{
		CCLogMeg::getSingleton().WriteError("len != sizeof(CMD_G_LookFighting),%s %d",__FILE__,__LINE__);
		return;
	}

	CMD_G_LookFighting *pLookFighting=(CMD_G_LookFighting*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pLookFighting->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeLookFighting No Find UserID:%d",pLookFighting->dwUserID);
		return;
	}

	MSC_Fighting_Info LookFighting={0};
	if(!CCFightingRecord::getSingleton().GetFightingRector(LookFighting,pLookFighting->iWorldSceneID,pLookFighting->iSceneID,pLookFighting->iDifficulty))
	{
		CMD_G_NoFightingRecord NoFightingRecord={0};
		NoFightingRecord.dwUserID=pUser->GetUserID();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_NO_FIGHTING_RECORD,(CT_CHAR*)&NoFightingRecord,sizeof(NoFightingRecord));

		return;
	}

	LookFighting.dwUserID=pLookFighting->dwUserID;


	CT_DWORD threadid = GAME_THREAD_FIGHT + pLookFighting->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_LOOK_FIGHTING, (CT_CHAR*)&LookFighting, sizeof(LookFighting));

}

//����ɨ��
CT_VOID	CCDuplicateEvent1::DisposeDuplicateRaids(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_DuplicateRaids))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeDuplicateRaids len != sizeof(CMD_G_DuplicateRaids)");
		return;
	}

	CMD_G_DuplicateRaids *pDuplicateRaids=(CMD_G_DuplicateRaids*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pDuplicateRaids->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeDuplicateRaids No Find UserID:%d",pDuplicateRaids->dwUserID);
		return;
	}

	//������֤
	if(pDuplicateRaids->iRaidsCount <= 0)
	{
		pUser->CloseNet();
		CCLogMeg::getSingleton().WriteError("pDuplicateRaids->iRaidsCount <= 0 userID:%d",pDuplicateRaids->dwUserID);
		return;
	}

	G_FightingScene FightingScene={0};
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(pDuplicateRaids->iWorldID,pDuplicateRaids->iSceneID,pDuplicateRaids->iDifficulty,FightingScene) == false)
	{
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_DATA;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::DisposeDuplicateRaids GetFightingScene w:%d s:%d d:%d",pDuplicateRaids->iWorldID,pDuplicateRaids->iSceneID,pDuplicateRaids->iDifficulty);
		return;
	}


	//������֤
	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);
	if(mvd.stRes.iStamina < pDuplicateRaids->iRaidsCount*FightingScene.iPphysical)
	{
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_STAMINA;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();

		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));

		return;
	}

	RaidsInfo ri={0};
	ri.iWorldID=pDuplicateRaids->iWorldID;
	ri.iSceneID=pDuplicateRaids->iSceneID;
	ri.iDifficulty=pDuplicateRaids->iDifficulty;
	ri.iAllTime=pDuplicateRaids->iRaidsCount;
	ri.lBeginTime=time(0);
	ri.iLastStamina=mvd.stRes.iStamina;

	CT_INT32 iRaidsInfo=pUser->m_userDuplicate.SetRaidsInfo(ri);
	if(iRaidsInfo == 0)
	{
		//ɨ���ɹ�
		CMD_G_RaidsSuc RaidsSuc={0};
		RaidsSuc.dwUserID=pUser->GetUserID();
		RaidsSuc.lTime=pUser->m_userDuplicate.GetLastTime();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_RAIDS_SUC,(CT_CHAR*)&RaidsSuc,sizeof(RaidsSuc));

		//д�����ݿ� 
		MSC_DB_RaidsInfo Raidsinfo={0};
		Raidsinfo.dwUserID=pUser->GetUserID();
		Raidsinfo.iWorldID=pDuplicateRaids->iWorldID;
		Raidsinfo.iSceneID=pDuplicateRaids->iSceneID;
		Raidsinfo.iDifficulty=pDuplicateRaids->iDifficulty;
		Raidsinfo.iAllTime=pDuplicateRaids->iRaidsCount;
		Raidsinfo.iCurrentTime=0;
		Raidsinfo.lBeginTime=ri.lBeginTime;
		CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid, MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_RAIDS_INFO, (CT_CHAR*)&Raidsinfo, sizeof(Raidsinfo));

	}
	else
	{
		//���ݴ���
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=iRaidsInfo;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));
	}

	return;
}

//ɨ������
CT_VOID	CCDuplicateEvent1::DisposeRaidsSpeed(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_RaidsSpeed))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsSpeed len != sizeof(CMD_G_RaidsSpeed)");
		return;
	}

	CMD_G_RaidsSpeed *pRaidsSpeed=(CMD_G_RaidsSpeed*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRaidsSpeed->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsSpeed No Find UserID:%d",pRaidsSpeed->dwUserID);
		return;
	}

	MasterVariableData mvd={0};
	MSC_LOGDB_RaidsSpeed RaidsSpeed={0};
	RaidsInfo RaidsData={0};
	pUser->m_userBase.GetMasterData(mvd);
	pUser->m_userDuplicate.GetRaidsInfo(RaidsData);
	RaidsSpeed.iGrade=mvd.stGrade.iLevel;

	//״̬�ж�
	if(RaidsData.iAllTime == 0)
	{
		CMD_G_RaidsInfo RaidsInfo={0};
		RaidsInfo.dwUserID=pUser->GetUserID();
		RaidsInfo.iSpeedInfo=RAIDSPEED_FAIL;
		RaidsInfo.iDiamod=0;
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_RAIDS_SPEED_INFO,(CT_CHAR*)&RaidsInfo,sizeof(RaidsInfo));
		return;
	}

	G_ValueRange vrtime={0};
	G_ValueRange vrCoefficient={0};
	if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vrtime) || !G_GetValueRange(VALUE_TYPE_RAIDS_SPEED,&vrCoefficient))
	{
		//���ݴ���
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_DATA;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();

		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));

		return;
	}

	CT_INT32 iTime=(CT_INT32)(pUser->m_userDuplicate.GetLastTime()+(RaidsData.iAllTime-RaidsData.iCurrentTime)*vrtime.fMaxVal);
	CT_INT32 iDiamod=G_GetRaidsSpeedDiamod(iTime,vrCoefficient.fMaxVal);
	if(mvd.stRes.iDiamod >= -iDiamod && pRaidsSpeed->iDiamod <= -iDiamod)
	{
		memset(&mvd,0,sizeof(mvd));
		mvd.stRes.iDiamod=iDiamod;
		pUser->m_userBase.AddMastData(mvd,SAVETAG_RAIDS_SPEED);

		//���ݼ�¼
		RaidsSpeed.dwUserID=pUser->GetUserID();
		RaidsSpeed.iDiamod=iDiamod;

		//��¼��־���ݿ�
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_RAIDSSPEED,(CT_CHAR*)&RaidsSpeed,sizeof(RaidsSpeed));

	}
	else
	{
		CMD_G_RaidsInfo RaidsInfo={0};
		RaidsInfo.dwUserID=pUser->GetUserID();
		RaidsInfo.iSpeedInfo=RAIDSPEED_DIAMOD;
		RaidsInfo.iDiamod=iDiamod;
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_RAIDS_SPEED_INFO,(CT_CHAR*)&RaidsInfo,sizeof(RaidsInfo));
		return;
	}

	//�����ж�
	CT_INT32 iMinTime=RaidsValidate(RaidsData,pUser,RaidsData.iAllTime-RaidsData.iCurrentTime);
	if(!iMinTime)
	{
		pUser->m_userDuplicate.ClearRaidsInfo();
		return;
	}

	//���뽱��
	for(CT_INT32 i=0;i<iMinTime;++i)
	{
		if(!pUser->m_userDuplicate.InsertReward())
		{
			//���ݴ���
			CMD_G_RaidsFail RaidsFail={0};
			RaidsFail.dwUserID=pUser->GetUserID();
			RaidsFail.iFailReason=RAID_DATA;
			RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();
			pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));
			return;
		}
		pUser->m_userDuplicate.UpdataRaids();

		CMD_G_RaidsReward RaidsReward={0};
		CT_INT32 iLen=sizeof(RaidsReward);
		pUser->m_userDuplicate.GetRaidsReward((CT_CHAR*)&RaidsReward,iLen);
		RaidsSave(RaidsReward,RaidsData);
	}

	//���ͽ���
	CT_CHAR Buf[RAIDS_REWARD_SIZE]={0};
	CT_INT32 iLen=RAIDS_REWARD_SIZE;
	if(pUser->m_userDuplicate.GetRaidsReward(Buf,iLen,CT_TRUE))
	{
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_RAIDS_REWARD,Buf,iLen);
	}

	//ͬ���ͻ�����Դ
	CMD_G_DuplicateMasterData DuplicateMasterData={0};
	MasterData master={0};
	pUser->m_userBase.GetMasterData(master);
	DuplicateMasterData.dwUserID=pUser->GetUserID();
	DuplicateMasterData.iFood=master.mvd.stRes.iFood;
	DuplicateMasterData.iStamina=master.mvd.stRes.iStamina;
	DuplicateMasterData.iDiamond=master.mvd.stRes.iDiamod;
	DuplicateMasterData.iGold=master.mvd.stRes.iGold;
	DuplicateMasterData.iExperience=master.mvd.stGrade.iCurrentExp;
	DuplicateMasterData.iPotential=master.mvd.stRes.iTalentNum;

	pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_DUPLICATE_MASTER_DATA,(CT_CHAR*)&DuplicateMasterData,sizeof(DuplicateMasterData));
	pUser->m_userDuplicate.ClearRaidsInfo();

	//add by caihan
	//����ͨ�أ��жϸ�������
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_DUPLICATE;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_DUPLICATE_POINTOR;
	taskEventInfo.iSubType = (RaidsData.iDifficulty+1)*100000+(RaidsData.iWorldID+1)*1000+(RaidsData.iSceneID+1);
	taskEventInfo.iLevel = 0;
	pUser->m_userTask.DisposeTask(taskEventInfo,RaidsData.iAllTime-RaidsData.iCurrentTime);
	//end

	// �ճ����� - �������� [4/7/2013 John]
	pUser->m_userDaily.UpdateQuestData(DAILY_FINISH_DUNGEON, RaidsData.iAllTime-RaidsData.iCurrentTime);
	return;
}

//������
CT_VOID	CCDuplicateEvent1::DisposeRequestReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_RequestReward))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRequestReward len != sizeof(CMD_G_RequestReward)");
		return;
	}

	CMD_G_RequestReward *pRequestReward=(CMD_G_RequestReward*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRequestReward->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRequestReward No Find UserID:%d",pRequestReward->dwUserID);
		return;
	}

	G_ValueRange vrtime={0};
	if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vrtime))
	{
		//���ݴ���
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_DATA;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));
		return;
	}

	RaidsInfo Raidsinfo={0};
	pUser->m_userDuplicate.GetRaidsInfo(Raidsinfo);

	time_t t=time(0);
	if(t-Raidsinfo.lBeginTime < vrtime.fMaxVal)
	{
		//���ݴ���
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_TIME;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));
		return;
	}

	//�����ж�
	CT_INT32 iMinTime=RaidsValidate(Raidsinfo,pUser);
	if(!iMinTime)
	{
		pUser->m_userDuplicate.ClearRaidsInfo();
		return;
	}


	CMD_G_RaidsReward RaidsReward={0};
	CT_INT32 iLen=sizeof(RaidsReward);
	if(pUser->m_userDuplicate.InsertReward())
	{
		//����ɨ��
		pUser->m_userDuplicate.UpdataRaids();
		pUser->m_userDuplicate.GetRaidsReward((CT_CHAR*)&RaidsReward,iLen);
		RaidsReward.lTime=pUser->m_userDuplicate.GetLastTime();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_RAIDS_REWARD,(CT_CHAR*)&RaidsReward,iLen);

		RaidsSave(RaidsReward,Raidsinfo);

		//ͬ���ͻ�����Դ
		CMD_G_DuplicateMasterData DuplicateMasterData={0};
		MasterData master={0};
		pUser->m_userBase.GetMasterData(master);
		DuplicateMasterData.dwUserID=pUser->GetUserID();
		DuplicateMasterData.iFood=master.mvd.stRes.iFood;
		DuplicateMasterData.iStamina=master.mvd.stRes.iStamina;
		DuplicateMasterData.iDiamond=master.mvd.stRes.iDiamod;
		DuplicateMasterData.iGold=master.mvd.stRes.iGold;
		DuplicateMasterData.iExperience=master.mvd.stGrade.iCurrentExp;
		DuplicateMasterData.iPotential=master.mvd.stRes.iTalentNum;

		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_DUPLICATE_MASTER_DATA,(CT_CHAR*)&DuplicateMasterData,sizeof(DuplicateMasterData));

		//���ɨ����Ϣ
		memset(&Raidsinfo,0,sizeof(Raidsinfo));
		pUser->m_userDuplicate.GetRaidsInfo(Raidsinfo);
		if(Raidsinfo.iAllTime - Raidsinfo.iCurrentTime <= 0)
		{
			pUser->m_userDuplicate.ClearRaidsInfo();
		}

		//add by caihan
		//����ͨ�أ��жϸ�������
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_DUPLICATE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_DUPLICATE_POINTOR;
		taskEventInfo.iSubType = (Raidsinfo.iDifficulty+1)*100000+(Raidsinfo.iWorldID+1)*1000+(Raidsinfo.iSceneID+1);
		taskEventInfo.iLevel = 0;
		pUser->m_userTask.DisposeTask(taskEventInfo);
		//end

		// �ճ����� - �������� [4/7/2013 John]
		pUser->m_userDaily.UpdateQuestData(DAILY_FINISH_DUNGEON, 1);

		return;
	}
	else
	{
		//���ݴ���
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_DATA;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));
		return;
	}
	return;
}

//ɨ��ʧ��
CT_VOID	CCDuplicateEvent1::DisposeRaidsFail(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_RaidsFail))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsFail len != sizeof(CMD_G_RaidsFail)");
		return;
	}

	CMD_G_RaidsFail *pRaidsFail=(CMD_G_RaidsFail*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRaidsFail->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsFail No Find UserID:%d",pRaidsFail->dwUserID);
		return;
	}

	CMD_G_RaidsFail RaidsFail={0};
	RaidsFail.dwUserID=pUser->GetUserID();
	RaidsFail.iFailReason=RAID_STOP;
	pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));

	pUser->m_userDuplicate.ClearRaidsInfo();

	return;
}

//ͬ��������ʯ
CT_VOID CCDuplicateEvent1::DisposeRaidsSpeedDiamod(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_RaidsSpeedDiamod))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsSpeedDiamod len != sizeof(CMD_G_RaidsFail)");
		return;
	}

	CMD_G_RaidsSpeedDiamod *pRaidsSpeedDiamod=(CMD_G_RaidsSpeedDiamod*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRaidsSpeedDiamod->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsSpeedDiamod No Find UserID: %d",pRaidsSpeedDiamod->dwUserID);
		return;
	}

	G_ValueRange vrtime={0};
	G_ValueRange vrCoefficient={0};
	if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vrtime) || !G_GetValueRange(VALUE_TYPE_RAIDS_SPEED,&vrCoefficient))
	{
		//���ݴ���
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_DATA;
		RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();

		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));

		return;
	}

	CMD_G_RaidsSpeedDiamod RaidsSpeedDiamod={0};
	RaidsInfo RaidsData={0};
	pUser->m_userDuplicate.GetRaidsInfo(RaidsData);
	CT_INT32 iTime=(CT_INT32)(pUser->m_userDuplicate.GetLastTime()+(RaidsData.iAllTime-RaidsData.iCurrentTime)*vrtime.fMaxVal);
	RaidsSpeedDiamod.dwUserID=pUser->GetUserID();
	RaidsSpeedDiamod.iDiamod=G_GetRaidsSpeedDiamod(iTime,vrCoefficient.fMaxVal);
	pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_RAIDS_SPEED_DIAMOD,(CT_CHAR*)&RaidsSpeedDiamod,sizeof(RaidsSpeedDiamod));
	return;
}



//ɨ����֤
CT_INT32 CCDuplicateEvent1::RaidsValidate(RaidsInfo &Raidsinfo,CCUser *pUser,CT_INT32 iTime)
{
	CT_INT32 iMinTime=iTime;
	CMD_G_RaidsFail RaidsFail={0};
	G_FightingScene FightingScene={0};

	RaidsFail.dwUserID=pUser->GetUserID();
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(Raidsinfo.iWorldID,Raidsinfo.iSceneID,Raidsinfo.iDifficulty,FightingScene) == CT_FALSE)
	{
		RaidsFail.iFailReason=RAID_DATA;
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::RaidsValidate GetFightingScene World:%d Scene:%d difficulty:%d",Raidsinfo.iWorldID,Raidsinfo.iSceneID,Raidsinfo.iDifficulty);
		return 0;
	}

	//������֤
	SceneInfo si={0};
	pUser->m_userDuplicate.GetSceneInfo(Raidsinfo.iWorldID,Raidsinfo.iSceneID,Raidsinfo.iDifficulty,si);
	if(si.iSceneStar == 0)
	{
		RaidsFail.iFailReason=RAID_PASS;
		return 0;
	}

	//������֤
	G_ValueRange DuplicateValue={0};
	G_ValueRange DuplicateBase={0};
	if(!G_GetValueRange(VALUE_TYPE_DUPLICATE_NUM,&DuplicateValue))
	{
		RaidsFail.iFailReason=RAID_DATA;
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::RaidsValidate G_GetValueRange VALUE_TYPE_DUPLICATE_NUM");
		return 0;
	}
	if(!G_GetValueRange(VALUE_TYPE_DUPLICATE_NUM,&DuplicateBase))
	{
		RaidsFail.iFailReason=RAID_DATA;
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::EnterSceneValidate VALUE_TYPE_DUPLICATE_BASENUM%s%d",__FILE__,__LINE__);
		return 0;
	}

	//vip add ���Ӹ�������
	if(FightingScene.iFreeNum != -1)
	{
		if(si.iFightingNum+iTime >= FightingScene.iFreeNum + pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_DUPLICATE)) 
		{
			RaidsFail.iFailReason=RAID_NUM;
			return 0;
		}
	}

	//�������
	if(FightingScene.iMaxNum != -1)
	{
		if(si.iFightingNum+iTime >= FightingScene.iMaxNum) 
		{
			RaidsFail.iFailReason=RAID_NUMMAX;
			return 0;
		}
	}

	if(RaidsFail.iFailReason != 0)
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));


	//ʳ�ﲻ��
	MasterVariableData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(MasterInfo.stRes.iFood < -FightingScene.iFood*iTime && FightingScene.iFood != 0)
	{
		if(iMinTime > MasterInfo.stRes.iFood/(-FightingScene.iFood))
		{
			RaidsFail.iFailReason=RAID_FOOD;
			iMinTime=MasterInfo.stRes.iFood/(-FightingScene.iFood);
		}
	}

	//��������
	if(MasterInfo.stRes.iStamina < -FightingScene.iPphysical*iTime && FightingScene.iPphysical != 0)
	{
		if(iMinTime > MasterInfo.stRes.iStamina/(-FightingScene.iPphysical))
		{
			RaidsFail.iFailReason=RAID_STAMINA;
			iMinTime=MasterInfo.stRes.iStamina/(-FightingScene.iPphysical);
		}
	}

	//���߲���
	CT_INT32 num =pUser->m_userItem.GetItemCount(FightingScene.iGoods);
	if(num < -FightingScene.iCount3*iTime && FightingScene.iCount3 != 0)
	{
		if(iMinTime > num/(-FightingScene.iCount3))
		{
			RaidsFail.iFailReason=RAID_GOODS;
			iMinTime=num/(-FightingScene.iCount3);
		}
	}

	//����ID
	if(RaidsFail.iFailReason != 0)
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));


	//��Ʒ����
	MasterVariableData Master={0};
	Master.stRes.iStamina+=FightingScene.iPphysical*iMinTime;
	Master.stRes.iFood+=FightingScene.iFood*iMinTime;
	pUser->m_userBase.AddMastData(Master,SAVETAG_DUPLICATE_STAMINA);
	pUser->m_userItem.AutoExpendItem(SAVETAG_DUPLICATE_STAMINA,FightingScene.iGoods,-FightingScene.iCount3*iMinTime);

	//��¼��־����
	MSC_LOGDB_DuplicateEnter DuplicateEnter={0};
	DuplicateEnter.dwUserID=pUser->GetUserID();
	DuplicateEnter.iFood=FightingScene.iFood*iMinTime;
	DuplicateEnter.iPphysical=FightingScene.iPphysical*iMinTime;
	DuplicateEnter.iGoods=FightingScene.iGoods;
	DuplicateEnter.iCount=FightingScene.iCount3*iMinTime;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATEENTER,(CT_CHAR*)&DuplicateEnter,sizeof(DuplicateEnter));

	return iMinTime;
}

//ɨ����¼ 
CT_VOID CCDuplicateEvent1::RaidsSave(CMD_G_RaidsReward &Reward,RaidsInfo &Raidsinfo)
{
	G_ValueRange vrtime={0};
	if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vrtime))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::RaidsSave G_GetValueRange VALUE_TYPE_RAIDS_TIME");
		return;
	}

	MasterVariableData mvd={0};
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(Reward.dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::RaidsSave No Find UserID:%d",Reward.dwUserID);
		return;
	}
	pUser->m_userBase.GetMasterData(mvd);

	//д�����ݿ�
	MSC_DB_RaidsInfo ri={0};
	ri.dwUserID=Reward.dwUserID;
	ri.iWorldID=Raidsinfo.iWorldID;
	ri.iSceneID=Raidsinfo.iSceneID;
	ri.iDifficulty=Raidsinfo.iDifficulty;
	ri.iAllTime=Raidsinfo.iAllTime;
	ri.iCurrentTime=Raidsinfo.iCurrentTime;
	ri.lBeginTime=Raidsinfo.lBeginTime;
	CT_DWORD threadid = GAME_THREAD_DATABASE + Reward.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid, MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_RAIDS_INFO, (CT_CHAR*)&ri, sizeof(ri));

	MSC_DB_RaidsReward dbRaidsReward={0};
	dbRaidsReward.dwUserID=Reward.dwUserID;
	dbRaidsReward.iDiamond=Reward.iDiamond;
	dbRaidsReward.iExperience=Reward.iExperience;
	dbRaidsReward.iGold=Reward.iGold;
	dbRaidsReward.iPotential=Reward.iPotential;
	dbRaidsReward.iSkillPoint=Reward.iSkillPoint;
	dbRaidsReward.iCurrentTime=Raidsinfo.iCurrentTime;
	dbRaidsReward.lTime= (CT_INT64) (Raidsinfo.lBeginTime+Raidsinfo.iCurrentTime*vrtime.fMaxVal);
	memcpy(dbRaidsReward.iGoods,Reward.iGoods,sizeof(dbRaidsReward.iGoods));
	memcpy(dbRaidsReward.iGoodsCount,Reward.iGoodsCount,sizeof(dbRaidsReward.iGoodsCount));

	threadid = GAME_THREAD_DATABASE + Reward.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid, MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_RAIDS_REWARD, (CT_CHAR*)&dbRaidsReward, sizeof(dbRaidsReward));

	//��¼������������־���ݿ�
	MSC_LOGDB_FightRecord  dbFR={0};
	dbFR.dwUserID=Reward.dwUserID;
	dbFR.iAid=Raidsinfo.iWorldID;
	dbFR.iFid = Raidsinfo.iSceneID;
	dbFR.iDifficulty=Raidsinfo.iDifficulty;
	dbFR.iGold=Reward.iGold;
	dbFR.iSkillNum=Reward.iSkillPoint;
	dbFR.iTalent=Reward.iPotential;
	dbFR.iXp=Reward.iExperience;
	dbFR.iWin=WINER_LIFT;
	dbFR.iUserGrade=mvd.stGrade.iLevel;
	memcpy(dbFR.iPropID,Reward.iGoods,sizeof(CT_INT32)*PVP_GOODS_COUNT);
	memcpy(dbFR.iPropNum,Reward.iGoodsCount,sizeof(CT_INT32)*PVP_GOODS_COUNT);

	//��¼��־���ݿ�
	threadid = GAME_THREAD_LOGDATABASE + Reward.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATE,(CT_CHAR*)&dbFR,sizeof(dbFR));
	return;
}

//ս������
CT_BOOL CCDuplicateEvent1::DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_Fighting_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeFightingEnd len != sizeof(MSC_Fighting_Info)");
		return CT_FALSE;
	}

	MSC_Fighting_Info *pFightingEnd=(MSC_Fighting_Info*)pBuf;

	//ʧ�ܲ�����
	if(pFightingEnd->iSceneStar == 0)return CT_TRUE;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingEnd->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeFightingEnd %d No Find UserID");
		return CT_FALSE;
	}

	//����ս���б�
	CCFightingRecord::getSingleton().SetFightingRector(*pFightingEnd,pFightingEnd->iWorldSceneID,pFightingEnd->iSceneID,pFightingEnd->iDifficulty);
	CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_W_FIGHTING_INFO,pBuf,len);

	//��������
	SceneInfo si={0};
	pUser->m_userDuplicate.GetSceneInfo(pFightingEnd->iWorldSceneID,pFightingEnd->iSceneID,pFightingEnd->iDifficulty,si);
	si.iSceneStar=pFightingEnd->iSceneStar;
	si.iFightingNum++;
	pUser->m_userDuplicate.UpdataSceneInfo(si);

	//������Դ
	G_FightingScene FightingScene={0};
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(pFightingEnd->iWorldSceneID,pFightingEnd->iSceneID,pFightingEnd->iDifficulty,FightingScene))
	{
		//��Ʒ����
		MasterVariableData Master={0};
		Master.stRes.iStamina+=FightingScene.iPphysical;
		Master.stRes.iFood+=FightingScene.iFood;
		pUser->m_userBase.AddMastData(Master,SAVETAG_DUPLICATE_STAMINA);
		pUser->m_userItem.AutoExpendItem(SAVETAG_DUPLICATE_STAMINA,FightingScene.iGoods,-FightingScene.iCount3);

		//ͬ���ͻ�����Դ
		CMD_G_DuplicateMasterData DuplicateMasterData={0};
		MasterData master={0};
		pUser->m_userBase.GetMasterData(master);
		DuplicateMasterData.dwUserID=pUser->GetUserID();
		DuplicateMasterData.iFood=master.mvd.stRes.iFood;
		DuplicateMasterData.iStamina=master.mvd.stRes.iStamina;
		DuplicateMasterData.iDiamond=master.mvd.stRes.iDiamod;
		DuplicateMasterData.iGold=master.mvd.stRes.iGold;
		DuplicateMasterData.iExperience=master.mvd.stGrade.iCurrentExp;
		DuplicateMasterData.iPotential=master.mvd.stRes.iTalentNum;

		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_DUPLICATE_MASTER_DATA,(CT_CHAR*)&DuplicateMasterData,sizeof(DuplicateMasterData));

		//��¼��־����
		MSC_LOGDB_DuplicateEnter DuplicateEnter={0};
		DuplicateEnter.dwUserID=pUser->GetUserID();
		DuplicateEnter.iFood=FightingScene.iFood;
		DuplicateEnter.iPphysical=FightingScene.iPphysical;
		DuplicateEnter.iGoods=FightingScene.iGoods;
		DuplicateEnter.iCount=FightingScene.iCount3;
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATEENTER,(CT_CHAR*)&DuplicateEnter,sizeof(DuplicateEnter));
	}

	//add by caihan
	//����ͨ�أ��жϸ�������
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_DUPLICATE;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_DUPLICATE_POINTOR;
	taskEventInfo.iSubType = (pFightingEnd->iDifficulty+1)*100000+(pFightingEnd->iWorldSceneID+1)*1000+(pFightingEnd->iSceneID+1);
	taskEventInfo.iLevel = pFightingEnd->iSceneStar;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end

	// �ճ����� - �������� [4/7/2013 John]
	pUser->m_userDaily.UpdateQuestData(DAILY_FINISH_DUNGEON, 1);

	//�ж��Ƿ��и����
	CCActive::getSingleton().ActiveProcess(ACTIVE_DUPLICATE_PASS,(pFightingEnd->iDifficulty+1)*100000+(pFightingEnd->iWorldSceneID+1)*1000+(pFightingEnd->iSceneID+1),pUser);

	return CT_TRUE;
}

//ɨ������
CT_VOID CCDuplicateEvent1::DisposeRaidsReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_RaidsReward))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsReward len != sizeof(MSC_RaidsReward)");
		return;
	}

	MSC_RaidsReward *pRaidsReward=(MSC_RaidsReward*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRaidsReward->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::DisposeRaidsReward No Find UserID:%d",pRaidsReward->dwUserID);
		return;
	}

	RaidsInfo Raidsinfo={0};
	pUser->m_userDuplicate.GetRaidsInfo(Raidsinfo);

	G_FightingScene FightingScene={0};
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(Raidsinfo.iWorldID,Raidsinfo.iSceneID,Raidsinfo.iDifficulty,FightingScene) == false)
	{
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_DATA;
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));

		CCLogMeg::getSingleton().WriteError("CCFightingScriptSetting::getSingleton().GetFightingScene world:%d scene:%d difficulty:%d",Raidsinfo.iWorldID,Raidsinfo.iSceneID,Raidsinfo.iDifficulty);
		return;
	}

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vr))
	{
		CMD_G_RaidsFail RaidsFail={0};
		RaidsFail.dwUserID=pUser->GetUserID();
		RaidsFail.iFailReason=RAID_DATA;
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));

		return;
	}

	//�����ж�
	time_t t=time(0);
	CT_INT32 iTime=(CT_INT32)((t -Raidsinfo.lBeginTime)/vr.fMaxVal);
	CT_INT32 iMinTime=0;
	if(Raidsinfo.iAllTime != 0)
	{
		CT_INT32 iNum=(iTime>Raidsinfo.iAllTime-Raidsinfo.iCurrentTime?Raidsinfo.iAllTime-Raidsinfo.iCurrentTime:iTime);
		iMinTime=RaidsValidate(Raidsinfo,pUser,iNum);
	}
	if(!iMinTime && iTime != 0)
	{
		pUser->m_userDuplicate.ClearRaidsInfo();
		return;
	}

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	for(CT_INT32 i=0;i<iMinTime;++i)
	{
		CMD_G_RaidsReward RaidsReward={0};
		CT_INT32 iLen=sizeof(RaidsReward);

		if(pUser->m_userDuplicate.InsertReward())
		{
			pUser->m_userDuplicate.UpdataRaids();
			pUser->m_userDuplicate.GetRaidsReward((CT_CHAR*)&RaidsReward,iLen);
			RaidsSave(RaidsReward,Raidsinfo);
		}
		else
		{
			//���ݴ���
			CMD_G_RaidsFail RaidsFail={0};
			RaidsFail.dwUserID=pUser->GetUserID();
			RaidsFail.iFailReason=RAID_DATA;
			RaidsFail.lTime=pUser->m_userDuplicate.GetLastTime();
			pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUG_G_RAIDS_FAIL,(CT_CHAR*)&RaidsFail,sizeof(RaidsFail));
			return;
		}
	}

	//ɨ����Ϣ
	CT_CHAR Buf[RAIDS_REWARD_SIZE]={0};
	CT_INT32 iLen=RAIDS_REWARD_SIZE;
	RaidsInfo ri={0};
	pUser->m_userDuplicate.GetRaidsInfo(ri);
	if(ri.iAllTime != 0)
	{
		CMD_G_RaidsRecord rr={0};
		rr.dwUserID=pUser->GetUserID();
		rr.iWorldID=ri.iWorldID;
		rr.iSceneID=ri.iSceneID;
		rr.iDifficulty=ri.iDifficulty;
		rr.iAllTime=ri.iAllTime;
		rr.iCurrentTime=ri.iCurrentTime;
		rr.lTime=pUser->m_userDuplicate.GetLastTime(t);
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT, SUB_G_RAIDS_RECORD, (CT_CHAR*)&rr, sizeof(rr));

		//���ͽ���
		memset(Buf,0,RAIDS_REWARD_SIZE);
		iLen=RAIDS_REWARD_SIZE;
		if(pUser->m_userDuplicate.GetRaidsReward(Buf,iLen,CT_TRUE))
		{
			pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_RAIDS_REWARD,Buf,iLen);
		}

		// �����������޵���ʧ
		MasterData master={0};
		pUser->m_userBase.GetMasterData(master);
		G_ValueRange vr = {0};
		CT_INT32 nInterval = 0;
		CT_INT32 nRecover = 0;
		CT_INT32 nMaxStamina = 0;
		CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_MAXSTAMINA, vr);
		nMaxStamina = (CT_INT32)vr.fMaxVal;
		//if (nMaxStamina >= Raidsinfo.iLastStamina)
		{
			CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_STAMINAINTERVAL, vr);
			nInterval = (CT_INT32)vr.fMinVal;
			CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_STAMINARESTOR, vr);
			nRecover = (CT_INT32)vr.fMinVal;
			CT_INT64 passTime = time(0) - master.llLastExitTime;
			if (master.llLastExitTime != 0) 
			{
				MasterVariableData mvd={0};
				CT_INT32 iAddStamina=(CT_INT32)(passTime / nInterval * nRecover);
				CT_INT32 iCurrentStamina=(Raidsinfo.iLastStamina+iAddStamina > nMaxStamina?nMaxStamina:Raidsinfo.iLastStamina+iAddStamina);
				CT_INT32 iAddStamina2=iAddStamina-(iCurrentStamina-Raidsinfo.iLastStamina);

				if(iAddStamina2>0 && master.mvd.stRes.iStamina < nMaxStamina)
				{
					mvd.stRes.iStamina=(iAddStamina2>nMaxStamina-master.mvd.stRes.iStamina?nMaxStamina-master.mvd.stRes.iStamina:iAddStamina2);
					pUser->m_userBase.AddMastData(mvd,SAVETAG_DUPLICATE_STAMINA);
				}
			}
		} 

		//ͬ���ͻ�����Դ
		memset(&master,0,sizeof(master));
		pUser->m_userBase.GetMasterData(master);
		CMD_G_DuplicateMasterData DuplicateMasterData={0};
		DuplicateMasterData.dwUserID=pUser->GetUserID();
		DuplicateMasterData.iFood=master.mvd.stRes.iFood;
		DuplicateMasterData.iStamina=master.mvd.stRes.iStamina;
		DuplicateMasterData.iDiamond=master.mvd.stRes.iDiamod;
		DuplicateMasterData.iGold=master.mvd.stRes.iGold;
		DuplicateMasterData.iExperience=master.mvd.stGrade.iCurrentExp;
		DuplicateMasterData.iPotential=master.mvd.stRes.iTalentNum;

		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_DUPLICATE_MASTER_DATA,(CT_CHAR*)&DuplicateMasterData,sizeof(DuplicateMasterData));

		//add by caihan
		//����ͨ�أ��жϸ�������
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_DUPLICATE;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_DUPLICATE_POINTOR;
		taskEventInfo.iSubType = (Raidsinfo.iDifficulty+1)*100000+(Raidsinfo.iWorldID+1)*1000+(Raidsinfo.iSceneID+1);
		taskEventInfo.iLevel = 0;
		pUser->m_userTask.DisposeTask(taskEventInfo,iMinTime);
		//end

		// �ճ����� - �������� [4/7/2013 John]
		pUser->m_userDaily.UpdateQuestData(DAILY_FINISH_DUNGEON, iMinTime);
	}

	memset(&Raidsinfo,0,sizeof(Raidsinfo));
	pUser->m_userDuplicate.GetRaidsInfo(Raidsinfo);
	if(iTime > Raidsinfo.iAllTime-Raidsinfo.iCurrentTime && iMinTime < Raidsinfo.iAllTime-Raidsinfo.iCurrentTime || 
		Raidsinfo.iAllTime-Raidsinfo.iCurrentTime == 0)
	{
		pUser->m_userDuplicate.ClearRaidsInfo();

	}

	return;
}



//���볡����֤
CT_INT32 CCDuplicateEvent1::EnterSceneValidate(SceneInfo &Sceneinfo,CCUser *pUser,CT_INT32 iDiamod)
{
	//��ȡ�ű�
	G_FightingScene FightingScene={0};
	if(CCFightingScriptSetting::getSingleton().GetFightingScene(Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID,Sceneinfo.iDifficulty,FightingScene) == CT_FALSE)
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::EnterSceneValidate GetFightingScene w:%d s:%d d:%d",
			Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID,Sceneinfo.iDifficulty);
		return ERROR_DATA;
	}

	//������֤
	SceneInfo si={0};
	if(Sceneinfo.iSceneID != 0 && !pUser->m_userDuplicate.GetSceneInfo(Sceneinfo.iWorldSceneID,FightingScene.iOpenCondition-1,0,si))
	{
		return ERROR_CONDITION;
	}

	memset(&si,0,sizeof(si));
	if(Sceneinfo.iSceneID != 0 && pUser->m_userDuplicate.GetSceneInfo(Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID-1,0,si))
	{
		G_FightingScene fs={0};
		if(CCFightingScriptSetting::getSingleton().GetFightingScene(Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID-1,0,fs) == CT_FALSE)
		{
			CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::EnterSceneValidate GetFightingScene w:%d s:%d d:%d",
				Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID-1,Sceneinfo.iDifficulty);
			return ERROR_DATA;
		}
		if(fs.iOpenCondition != -1 && si.iSceneStar == 0)
			return ERROR_PASS_SCENE;
	}

	//�ȼ�����
	MasterVariableData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	if(FightingScene.iOpenGrade > MasterInfo.stGrade.iLevel)
	{
		return ERROR_SCENE_GRADE;
	}

	//��������
	if(!pUser->m_userTask.ViewCompleteTask(FightingScene.iOpenTask) && FightingScene.iOpenTask != 0)
	{
		CMD_G_ErrorScene ErrorScene={0};
		ErrorScene.dwUserID=pUser->GetUserID();
		ErrorScene.cbErrorScene=ERROR_TASK;
		ErrorScene.iTaskID=FightingScene.iOpenTask;
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_SCENE_ERROR,(CT_CHAR*)&ErrorScene,sizeof(ErrorScene));
		return ERROR_TASK;
	}

	//������֤
	G_ValueRange DuplicateValue={0};
	if(!G_GetValueRange(VALUE_TYPE_DUPLICATE_NUM,&DuplicateValue))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::EnterSceneValidate G_GetValueRange VALUE_TYPE_DUPLICATE_NUM");
		return ERROR_DATA;
	}
	G_ValueRange DuplicateBase={0};
	if(!G_GetValueRange(VALUE_TYPE_DUPLICATE_BASENUM,&DuplicateBase))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent::EnterSceneValidate VALUE_TYPE_DUPLICATE_BASENUM");
		return ERROR_DATA;
	}
	//vip add ���Ӹ�������
	if(FightingScene.iFreeNum != -1)
	{
		if(Sceneinfo.iFightingNum >= FightingScene.iFreeNum + pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_DUPLICATE)) 
		{
			CMD_G_ErrorScene ErrorScene={0};
			ErrorScene.dwUserID=pUser->GetUserID();
			ErrorScene.cbErrorScene=ERROR_NO_NUM;
			ErrorScene.iDiamod=G_GetFightingDiamod(pUser->m_userDuplicate.GetBuyFightingTime(),(CT_INT32)DuplicateBase.fMinVal,DuplicateValue.fMinVal);

			if(-ErrorScene.iDiamod > -iDiamod)
			{
				pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_SCENE_ERROR,(CT_CHAR*)&ErrorScene,sizeof(ErrorScene));
				return ERROR_NO_NUM;
			}
		}
	}

	//�������
	if(FightingScene.iMaxNum != -1 && Sceneinfo.iFightingNum >= FightingScene.iMaxNum) 
	{
		return ERROR_MAX_NUM;
	}

	//��ʯ��֤
	if(iDiamod > MasterInfo.stRes.iDiamod)
	{
		return ERROR_DIAMOD;
	}

	//ʳ�ﲻ��
	if(MasterInfo.stRes.iFood < -FightingScene.iFood)
	{
		return ERROR_FOOD;
	}

	//��������
	if(MasterInfo.stRes.iStamina < -FightingScene.iPphysical)
	{
		CMD_G_ErrorScene ErrorScene={0};
		ErrorScene.dwUserID=pUser->GetUserID();
		ErrorScene.cbErrorScene=ERROR_PPHYSICAL;
		ErrorScene.iDiamod=G_GetStatminaCost(pUser);
		pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_SCENE_ERROR,(CT_CHAR*)&ErrorScene,sizeof(ErrorScene));
		return ERROR_PPHYSICAL;
	}

	//���߲���
	CT_INT32 num =pUser->m_userItem.GetItemCount(FightingScene.iGoods);
	if(num < -FightingScene.iCount3)
	{
		return ERROR_GOODS;
	}

	//��Ʒ����
	if(iDiamod != 0)
	{
		MasterVariableData Master={0};
		Master.stRes.iDiamod+=G_GetFightingDiamod(pUser->m_userDuplicate.GetBuyFightingTime(),(CT_INT32)DuplicateBase.fMinVal,DuplicateValue.fMinVal);
		pUser->m_userBase.AddMastData(Master,SAVETAG_DUPLICATE_STAMINA);

		MSC_LOGDB_BuyFightingTime BuyFightingTime={0};
		BuyFightingTime.dwUserID=pUser->GetUserID();
		BuyFightingTime.iUserGrade = Master.stGrade.iLevel;
		BuyFightingTime.iBuyFightingTime=pUser->m_userDuplicate.GetBuyFightingTime();
		BuyFightingTime.iDiamod=Master.stRes.iDiamod;

		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_BUYFIGHTINGTIE,(CT_CHAR*)&BuyFightingTime,sizeof(BuyFightingTime));

		//���¹������
		pUser->m_userDuplicate.AddBuyFightingTime();
	}

	//ͬ���ͻ�����Դ
	CMD_G_DuplicateMasterData DuplicateMasterData={0};
	MasterData master={0};
	pUser->m_userBase.GetMasterData(master);
	DuplicateMasterData.dwUserID=pUser->GetUserID();
	DuplicateMasterData.iFood=master.mvd.stRes.iFood;
	DuplicateMasterData.iStamina=master.mvd.stRes.iStamina;
	DuplicateMasterData.iDiamond=master.mvd.stRes.iDiamod;
	DuplicateMasterData.iGold=master.mvd.stRes.iGold;
	DuplicateMasterData.iExperience=master.mvd.stGrade.iCurrentExp;
	DuplicateMasterData.iPotential=master.mvd.stRes.iTalentNum;

	pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_DUPLICATE_MASTER_DATA,(CT_CHAR*)&DuplicateMasterData,sizeof(DuplicateMasterData));

	return 0;
}

//������Ϣ
CT_VOID CCDuplicateEvent1::DisposeFightingReward(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_PVEFighting_Reward))
	{
		CCLogMeg::getSingleton().WriteError("len != sizeof(MSC_PVEFighting_Reward),%s %d",__FILE__,__LINE__);
		return;
	}

	MSC_PVEFighting_Reward *pReward=(MSC_PVEFighting_Reward*)pBuf;

	CT_DWORD dwUser = pReward->dwUserID;
	CT_INT32 iWorldSceneID = pReward->iWorldSceneID;
	CT_INT32 iSceneID = pReward->iSceneID;
	CT_INT32 iDifficulty = pReward->iDifficulty;
	CT_INT32 iStarCount = pReward->iSceneStar;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(dwUser,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDuplicateEvent1::DisposeFightingReward %d No Find UserID",dwUser);
		return;
	}
	//�ͻ��˽���
	CMD_G_FightingReward Reward={0};

	//��ȡ����
	G_SceneReward SceneReward={0};
	G_FightingScene FightingScene={0};
	if(!CCFightingScriptSetting::getSingleton().GetFightingScene(iWorldSceneID,iSceneID,iDifficulty,FightingScene))
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::FightingReward GetFightingScene WorldScene:%d SceneID:%d Difficulty:%d",iWorldSceneID+1,iSceneID+1,iDifficulty+1);
		return;
	}
	if(!CCFightingScriptSetting::getSingleton().GetSceneReward(iSceneID,FightingScene.iReward,SceneReward))
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::FightingReward GetSceneReward SceneID:%d Reward:%d",iSceneID+1,FightingScene.iReward);
		return;
	}

	//��¼������������־���ݿ�
	MSC_LOGDB_FightRecord  dbFR={0};
	dbFR.dwUserID=pUser->GetUserID();
	dbFR.iAid=iWorldSceneID;
	dbFR.iFid = iSceneID;
	dbFR.iDifficulty=iDifficulty;

	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);
	dbFR.iUserGrade = MasterInfo.mvd.stGrade.iLevel;

	MasterVariableData MasterReward={0};
	MasterReward.stRes.iDiamod+=SceneReward.iDiamond;
	MasterReward.stRes.iGold+=G_GetFightingGold(SceneReward.iGold,MasterInfo.factor.fBattleFactor);
	MasterReward.stRes.iSkillSoul+=SceneReward.iSkillPrint;

	MasterReward.stGrade.iCurrentExp+=G_GetFightingExperience(SceneReward.iExperience,MasterInfo.factor.fBattleFactor);

	dbFR.iDiamod=SceneReward.iDiamond;
	dbFR.iGold=G_GetFightingGold(SceneReward.iGold,MasterInfo.factor.fBattleFactor);
	dbFR.iSkillNum=SceneReward.iSkillPrint;
	dbFR.iXp=G_GetFightingExperience(SceneReward.iExperience,MasterInfo.factor.fBattleFactor);
	dbFR.iWin=WINER_LIFT;

	// ����������Դ
	MasterVariableData addValue = {0};
	pUser->m_userBase.AddMastData(MasterReward, SAVETAG_DUPLICATE_REWARD);


	CT_INT32 iGoods1[GOODS_COUNT]={0};
	CT_INT32 iCount1[GOODS_COUNT]={0};
	CT_INT32 iGoods2[GOODS_COUNT]={0};
	CT_INT32 iCount2[GOODS_COUNT]={0};
	CT_INT32 iGoodsCount1=0;
	FakeProBase fb={0};
	//CT_INT32 iGoodsCount1=0;
	if(SceneReward.iGoodsID1 != 0)
		G_GetGoodsGroupInfo(SceneReward.iGoodsID1,iGoods1,iCount1,fb);
	if(SceneReward.iGoodsID2 != 0)
		G_GetGoodsGroupInfo(SceneReward.iGoodsID2,iGoods2,iCount2,fb);

	for(CT_INT32 j=0;j<GOODS_COUNT;++j)
	{
		if(iGoods1[j] != 0 && iCount1[j] != 0)
		{
			Reward.iGoods[iGoodsCount1]=iGoods1[j];
			Reward.iGoodsCount[iGoodsCount1]=iCount1[j];
			if (iGoodsCount1<PVE_GOODS_COUNT)
			{
				dbFR.iPropID[iGoodsCount1]=iGoods1[j];
				dbFR.iPropNum[iGoodsCount1]=iCount1[j];
			}
			iGoodsCount1++;

			CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			pUser->m_userItem.AutoAddItemData(SAVETAG_DUPLICATE_REWARD,iGoods1[j],iCount1[j]);

			//
			if (G_BroadcastFilter(1, iGoods1[j]))
			{
				MasterData masterData = {0};
				RollBroadcast rb[2]={0};
				pUser->m_userBase.GetMasterData(masterData);
				rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
				rb[1].SetValue(iGoods1[j], "");
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{2}<2:%d>", pUser->GetUserID(), masterData.cNickName,iGoods1[j]);
				G_SendRollBroadecast(118,3,60,(CT_CHAR*)rb,sizeof(rb));
			}
		}

		if(iGoods2[j] != 0 && iCount2[j] != 0)
		{
			Reward.iGoods[iGoodsCount1]=iGoods2[j];
			Reward.iGoodsCount[iGoodsCount1]=iCount2[j];
			if (iGoodsCount1<PVE_GOODS_COUNT)
			{
				dbFR.iPropID[iGoodsCount1]=iGoods2[j];
				dbFR.iPropNum[iGoodsCount1]=iCount2[j];
			}
			iGoodsCount1++;

			CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			pUser->m_userItem.AutoAddItemData(SAVETAG_DUPLICATE_REWARD,iGoods2[j],iCount2[j]);

			//
			if (G_BroadcastFilter(1, iGoods2[j]))
			{
				MasterData masterData = {0};
				RollBroadcast rb[2]={0};
				pUser->m_userBase.GetMasterData(masterData);
				rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
				rb[1].SetValue(iGoods2[j], "");
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{2}<2:%d>", pUser->GetUserID(), masterData.cNickName,iGoods2[j]);
				G_SendRollBroadecast(118,3,60,(CT_CHAR*)rb,sizeof(rb));
			}
		}
	}

	//�״ν���
	SceneInfo Sceneinfo={0};
	pUser->m_userDuplicate.GetSceneInfo(iWorldSceneID,iSceneID,iDifficulty,Sceneinfo);

	if(Sceneinfo.iSceneStar == 0 || iDifficulty > 0)
	{
		memset(&MasterReward,0,sizeof(MasterReward));
		MasterReward.stRes.iTalentNum+=SceneReward.iPotential;

		// ����������Դ
		MasterVariableData addValue = {0};
		pUser->m_userBase.AddMastData(MasterReward, SAVETAG_DUPLICATE_REWARD);
		//printf("Ǳ�ܵ�:%d\n",MasterReward.stRes.iTalentNum);

		//�ͻ�����Ϣ
		Reward.iPotential=SceneReward.iPotential;

		dbFR.iTalent=SceneReward.iPotential;
	}

	if (Sceneinfo.iSceneStar == 0 && iDifficulty > 0)	//�״�ͨ�ؾ�Ӣ����
	{
		CT_INT32 iDupID = (iDifficulty+1)*100000 + (iWorldSceneID+1)*1000 + (iSceneID+1);
		MasterData masterData = {0};
		RollBroadcast rb[2]={0};
		pUser->m_userBase.GetMasterData(masterData);
		rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
		rb[1].SetValue(iDupID, "");
		//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{3}<3:%d>", pUser->GetUserID(), masterData.cNickName,iDupID);
		G_SendRollBroadecast(119,5,60,(CT_CHAR*)rb,sizeof(rb));
	}

	Reward.iDiamond=SceneReward.iDiamond;
	Reward.iGold=G_GetFightingGold(SceneReward.iGold,MasterInfo.factor.fBattleFactor);
	Reward.iSkillPoint=SceneReward.iSkillPrint;
	Reward.iExperience=G_GetFightingExperience(SceneReward.iExperience,MasterInfo.factor.fBattleFactor);
	Reward.iStarCount=iStarCount;

	pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));

	//��¼��־���ݿ�
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_DUPLICATE,(CT_CHAR*)&dbFR,sizeof(dbFR));
}

