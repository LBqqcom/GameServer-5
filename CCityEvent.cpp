#include "CCityEvent.h"
#include "CEventScriptSetting.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"
#include "CUserManager.h"
#include "CLogMeg.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CThreadMes.h"
#include "CFormula.h"
#include "CBroadcastScriptSetting.h"
#include "CFightTeamManager.h"
#include "CTowerResManager.h"

CCCityEvent::CCCityEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_EVENT_CITY, SUB_G_APPLY_AWARD_EVENT);
	RegistMessage(MSC_MAIN_EVENT_CITY, MSC_SUB_EVENT_FIGHTING_END);
	RegistMessage(MAIN_G_HEART_BEAT,0);
	RegistMessage(MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_WORLD);
	RegistMessage(MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_SYSTEM);
	RegistMessage(MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_GM);
	RegistMessage(MSC_MAIN_ANNOUNCEMENT_UPDATA,0);

	G_ValueRange vr = {0};
	if(G_GetValueRange(VALUE_TYPE_UPDATEEVENTTIME,  &vr))
	{
		//����������ⶨʱ��
		RegistTimer(IDI_EVENT_TIMER);
		SetTimer(IDI_EVENT_TIMER, (CT_INT32)(vr.fMaxVal * 1000 ));
	}

	//���涨ʱ��
	RegistTimer(IDI_ANNOUNCEMENT_UPDATA);
	SetTimer(IDI_ANNOUNCEMENT_UPDATA,TIME_ANNOUNCEMENT_UPDATA);

	//ÿ�ո���
	if(m_dwThread == GAME_THREAD_CITY)
	{
		RegistClock(IDI_UPDATA_DATA);
		SetClock(IDI_UPDATA_DATA,0,0,0);
	}
}

CCCityEvent::~CCCityEvent(void)
{
}

//�����û���Ϣ
CT_VOID CCCityEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_EVENT_CITY:
		{
			DisposeCityEvent(subID, pBuf, len);
			break;
		}
	case MSC_MAIN_EVENT_CITY:
		{
			DisposeMscCityEvent(subID, pBuf, len);
			break;
		}
	case MAIN_G_HEART_BEAT:			//����
		{
			DisposeHeartBeatEvent(pBuf,  len);
			break;
		}
	case MSC_MAIN_BROADCAST:
		{
			if (MSC_SUB_BROADCAST_WORLD == subID)
			{
				DisposeBroadcastWorldMessage(pBuf, len);
			}
			else if(MSC_SUB_BROADCAST_SYSTEM == subID)
			{
				DisposeBroadcast(pBuf, len);
			}else if (MSC_SUB_BROADCAST_GM == subID)
			{
				if (sizeof(MSC_Broadcast_GM) != len)
				{
					return;
				}
				MSC_Broadcast_GM* pGM = (MSC_Broadcast_GM*)pBuf;
				CCUserManager::getSingleton().SendGMMessage(pGM->dwMainID, pGM->dwSubID, (CT_CHAR*)&(pGM->gm),  sizeof(CMD_G_GMMessage));
			}
			break;
		}
	case MSC_MAIN_ANNOUNCEMENT_UPDATA:
		{
			DisposeAnnouncementUpdata(pBuf, len);
			break;
		}
	
	default:
		break;
	}
}

CT_VOID CCCityEvent::OnClock(CT_DWORD timeID)
{
	if(timeID == IDI_UPDATA_DATA)
	{
		//��ˢ���ڴ��ڸ������ݿ�
		//���¸���
		CCUserManager::getSingleton().RefreshDuplicate();
		//���¾�����
		CCUserManager::getSingleton().RefreshPVP();
		//������ʯ�������
		CCUserManager::getSingleton().RefreshWorship();
		//����ս������
		CCFightTeamManager::getSingleton().RefreshFightTeamTop();
		//������Ӹ���
		CCUserManager::getSingleton().UpdataTeamFightingTimes();
		//����Ԫ��֮��
		CCTowerResManager::getSingleton().UpdateSlotProduct(CT_TRUE);
		//����Ԫ����ͨ������
		CCUserManager::getSingleton().RefreshTowerPass();
		//����ս���ٻ�
		CCFightTeamManager::getSingleton().ClearFightTeamSummon();
		//���������������
		CCUserManager::getSingleton().UpdataBuyStaminaTimes();
		//��������״̬
		CCUserManager::getSingleton().UpdataDragonBallStatus();
		//����̽����Ϣ
		CCUserManager::getSingleton().UpdateSeekData();
		//�������а���״̬
		CCUserManager::getSingleton().UpdateTopReward();

		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_UPDATEDATA,MSC_SUB_DB_UPDATA_DATA,0,0);
	}
}

//�û���ʱ������
CT_VOID CCCityEvent::OnTime(CT_DWORD timeID)
{
	if (timeID == IDI_EVENT_TIMER)
	{
		CCUserManager::getSingleton().UpdateEvent(m_dwThread - GAME_THREAD_CITY, ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum );
	}
	else if (timeID == IDI_ANNOUNCEMENT_UPDATA)
	{
		G_SendThreadInfo(GAME_THREAD_DATABASE,MSC_MAIN_DB_ANNOUNCEMENT, 0, 0, 0);
	}
}

//��������¼�
CT_VOID  CCCityEvent::DisposeCityEvent(CT_DWORD subID,  CT_CHAR* pBuf, CT_DWORD len)
{
	switch (subID)
	{
	case SUB_G_APPLY_AWARD_EVENT:
		{
			if (len != sizeof(CMD_G_ApplyAwardEvent))
			{
				CCLogMeg::getSingleton().WriteError("CCCityEvent::OnUserEvent: len != sizeof(CMD_G_AppleAwardEvent");
				return;
			}

			CMD_G_ApplyAwardEvent* pAward = (CMD_G_ApplyAwardEvent*)pBuf;
			DisposeApplyAwardEvent(*pAward);
			break;
		}

	default:
		break;  
	}
}

//��������¼�
CT_VOID  CCCityEvent::DisposeMscCityEvent(CT_DWORD subID,  CT_CHAR* pBuf, CT_DWORD len)
{
	switch(subID)
	{
	case MSC_SUB_EVENT_FIGHTING_END:
		{
			if (len != sizeof(MSC_Event_Fighting_End))
			{
				CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeMscCityEvent: len != sizeof(MSC_Event_Fighting_End");
				return;
			}

			MSC_Event_Fighting_End* pEventFightingEnd = (MSC_Event_Fighting_End*)pBuf;
			DisposeMscFightingEventEnd(*pEventFightingEnd); //���벻��ȥ������

			break;
		}
	default:
		{
			break;
		}
	}
}

//�����û�����
CT_VOID CCCityEvent::DisposeApplyAwardEvent(const CMD_G_ApplyAwardEvent& awardEvent)
{
	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(awardEvent.dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeApplyAwardEvent, û�з�������û�id = %i", awardEvent.dwUserID);
		return;
	}

	switch (awardEvent.et)
	{
	case EVENT_FIGHT:
		{
			DisposeFightEventAward(pUser, awardEvent);
			return; //ս������ֱ�ӷ��أ���Ϊս���߳���Ҫ����ս����Ϣ
		}

	case EVENT_BUILDING:
		{
			DisposeBuildingEventAward(pUser, awardEvent);
			break;
		}
	case EVENT_PET:
		{	
			DisposePetEventAward(pUser, awardEvent);
			break;
		}
	case EVENT_TREASURE:
		{
			DisposeTreasureEventAward(pUser, awardEvent);
			break;
		}
	case EVENT_WORKER:
		{
			DisposeWorkerEventAward(pUser, awardEvent);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeApplyAwardEvent, �����˴����¼����� = %i", awardEvent.et);
			return; // ����ֱ�ӷ��� [4/7/2013 John]
		}
	}

    // �ճ����� - ����¼� [4/7/2013 John]
    pUser->m_userDaily.UpdateQuestData(DAILY_COMPLETE_EVENT, 1);
}

//�����û�����
CT_VOID CCCityEvent::DisposeApplyAwardEvent(CCUser* pUser, CT_INT32 eventID,  CT_INT32 index, CT_INT32 awardID, ENUM_EventType et)
{
	G_EventAward gea = {0};
	if (!CCEventScriptSetting::getSingleton().GetEventAward(eventID, awardID, gea))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeApplyAwardEvent, ���¼�������û�з����û� id = %i, eventid=%i, awardid=%i",
			                                pUser->GetUserID(), eventID, awardID);
		return;
	}
	
	MasterData md = {0};
	pUser->m_userBase.GetMasterData(md);

	MasterVariableData  addValue = {0};
	addValue.stRes.iGold += G_GetEventFinishAward(gea.fGold,md.mvd.stGrade.iLevel,md.factor.fProductFactor);
	addValue.stRes.iFood += G_GetEventFinishAward(gea.fFood,md.mvd.stGrade.iLevel,md.factor.fProductFactor);
	addValue.stCivili.iCurrentExp += gea.iCivi;
	addValue.stGrade.iCurrentExp += G_GetEventFinishAward(gea.fXp,md.mvd.stGrade.iLevel,md.factor.fProductFactor);        
	pUser->m_userBase.AddMastData(addValue,SAVETAG_EVENT_AWARD);
	
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);

	CMD_G_AwardEvent gae = {0};
	gae.dwUserID   = pUser->GetUserID();
	gae.iCiviliNum = mvd.stCivili.iCurrentExp;
	gae.iFood      = mvd.stRes.iFood;
	gae.iGold      = mvd.stRes.iGold;
	gae.iXp        = mvd.stGrade.iCurrentExp;
	gae.iIndex     = index;
	gae.et         = et;
	gae.iAwardID   = awardID;

	CT_INT32 iNum = gea.iPropNum1; 
	//if (pUser->m_userItem.AutoAddItemData(m_dwThread, gea.iProp1, iNum))
	if (LOOT_SUC == pUser->LootItems(SAVETAG_EVENT_AWARD, &(gea.iProp1), &iNum))
	{
		gae.iProp1 = gea.iProp1;
	}

	iNum = gea.iPropNum2; 
	//if (pUser->m_userItem.AutoAddItemData(m_dwThread, gea.iProp2, iNum))
	if (LOOT_SUC == pUser->LootItems(SAVETAG_EVENT_AWARD, &(gea.iProp2), &iNum))
	{
		gae.iProp2 = gea.iProp2;
	} 

//	G_SaveMastVariableData(pUser, SAVETAG_EVENT_REWARD);

	//������Ϣ
	pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_AWARD_EVENT, (CT_CHAR*)&gae, sizeof(gae));

	//��־��¼
	MSC_LOGDB_EventReward EventReward={0};
	EventReward.dwUserID=pUser->GetUserID();
	EventReward.iXp=addValue.stGrade.iCurrentExp;
	EventReward.iFood=addValue.stRes.iFood;
	EventReward.iGold=addValue.stRes.iGold;
	EventReward.iProp1=gea.iProp1;
	EventReward.iProp2=gea.iProp2;
	EventReward.iNum1=gea.iPropNum1;
	EventReward.iNum2=gea.iPropNum2;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_EVENTREWARD,(CT_CHAR*)&EventReward,sizeof(EventReward));					


//	MSC_DB_MasterSaveData  msd = {0};
//	pUser->m_userBase.GetMasterSaveData(msd);
	//MSC_DB_EventAward ea = {0};
	//ea.dwUserID =  pUser->GetUserID();
	//ea.iCiviliNum = addValue.stCivili.iCurrentExp;
	//ea.iFood      = addValue.stRes.iFood;
	//ea.iGold      = addValue.stRes.iGold;
	//ea.iProp1     = gea.iProp1;
	//ea.iPropNum1  = gea.iPropNum1;
	//ea.iProp2     = gea.iProp2;
	//ea.iPropNum2  = gea.iPropNum2;

	//CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	//G_SendThreadInfo(threadid, MSC_MAIN_DB_BUILDING_EVENT,  MSC_SUB_DB_SAVE_AWARD_DATA, (CT_CHAR*)&ea, sizeof(ea) ); 

	//���ݼ�¼
	G_ItemBase ItemBase1={0};
	G_ItemBase ItemBase2={0};
	if(G_GetItemBase(gea.iProp1,&ItemBase1) && G_GetItemBase(gea.iProp2,&ItemBase2))
	{
		MSC_LOGDB_EventAward EventAward={0};
		EventAward.dwUserID =  pUser->GetUserID();
		EventAward.iCiviliNum = addValue.stCivili.iCurrentExp;
		EventAward.iFood      = addValue.stRes.iFood;
		EventAward.iGold      = addValue.stRes.iGold;
		EventAward.iProp1     = gea.iProp1;
		EventAward.iPropNum1  = gea.iPropNum1;
		EventAward.iProp2     = gea.iProp2;
		EventAward.iPropNum2  = gea.iPropNum2;
		memcpy(EventAward.cProp1Name,ItemBase1.szName,sizeof(EventAward.cProp1Name));
		memcpy(EventAward.cProp2Name,ItemBase2.szName,sizeof(EventAward.cProp2Name));
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_SAVEEVENTFINISH,(CT_CHAR*)&EventAward,sizeof(EventAward));
	}

}


CT_VOID DisposeEventTimeOut(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent)
{
	CMD_G_EventTimeOut eto = {0};
	eto.dwUserID = pUser->GetUserID();
	eto.iIndex = awardEvent.iIndex;
	eto.et     = awardEvent.et;
	pUser->GetNetInterface()->Send(MAIN_G_EVENT_CITY, SUB_G_TIMEOUT_EVENT, (CT_CHAR*)&eto, sizeof(eto));
}

//����ս���¼�
CT_VOID CCCityEvent::DisposeFightEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent)
{
	if (!pUser->m_userEvent.FindFightEvent(awardEvent.iIndex))
	{
		DisposeEventTimeOut(pUser, awardEvent);
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeFightEventAward, û�����ս���¼� = %i", awardEvent.iIndex);
		return ;
	}

	if (pUser->m_userEvent.FindFightEvent(awardEvent.iIndex))
	{
		MSC_Fighting_Info FightingInfo={0};
		FightingInfo.dwUserID=awardEvent.dwUserID;
		FightingInfo.iWorldSceneID=awardEvent.iAwardID;
		FightingInfo.iSceneID=awardEvent.iIndex;
		FightingInfo.iSeed=rand()%RAND_MAX_EX;
		G_GetFightingData(pUser,FightingInfo.FightingData);
		pUser->m_userArmy.DeleteBoss();

		CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		G_SendThreadInfo(threadid, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_EVENT_FIGHTING, (CT_CHAR*)&FightingInfo, sizeof(FightingInfo));
		//add by caihan
		//����¼��������¼������ж�
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_EVENT;
		taskEventInfo.iSubType = awardEvent.et;
		pUser->m_userTask.DisposeTask(taskEventInfo);
		//end
	}
}

//����������
CT_VOID CCCityEvent::DisposeBuildingEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent)
{
	if (!pUser->m_userEvent.FindBuildingEventByIndex(awardEvent.iIndex))
	{
		DisposeEventTimeOut(pUser, awardEvent);
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeBuildingEventAward, û����������¼� = %i", awardEvent.iIndex);
		return ;
	}

	G_BuildingEventProbability bep = {0};
	if (!CCEventScriptSetting::getSingleton().GetBuildingEvent(awardEvent.iIndex, bep))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeBuildingEventAward, û�����������ҵ������¼� = %i", awardEvent.iIndex);
		return;
	}

	DisposeApplyAwardEvent(pUser, bep.iEventID, awardEvent.iIndex, awardEvent.iAwardID,  awardEvent.et);
	pUser->m_userEvent.DeleteBuildingEvent(awardEvent.iIndex);   
	//add by caihan
	//����¼��������¼������ж�
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_EVENT;
	taskEventInfo.iSubType = awardEvent.et;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
}

//�����˽���
CT_VOID CCCityEvent::DisposeWorkerEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent)
{
	if (!pUser->m_userEvent.FindWorkerEvent(awardEvent.iIndex))
	{
		DisposeEventTimeOut(pUser, awardEvent);
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeWorkerEventAward, û����������¼� = %i", awardEvent.iIndex);
		return ;
	}

	G_NormalEventProbability  nep = {0};
	if (!CCEventScriptSetting::getSingleton().GetWorkerEvent(awardEvent.iIndex, nep))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeWorkerEventAward, û�����������ҵ������¼� = %i", awardEvent.iIndex);
		return;
	}
	
	DisposeApplyAwardEvent(pUser, nep.iEventID, awardEvent.iIndex, awardEvent.iAwardID,  awardEvent.et);
	pUser->m_userEvent.DeleteWorkerEvent(awardEvent.iIndex);    
	//add by caihan
	//����¼��������¼������ж�
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_EVENT;
	taskEventInfo.iSubType = awardEvent.et;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
}

//������ｱ��
CT_VOID CCCityEvent::DisposePetEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent)
{
	if (!pUser->m_userEvent.FindPetEvent(awardEvent.iIndex))
	{
		DisposeEventTimeOut(pUser, awardEvent);

		CCLogMeg::getSingleton().WriteError("CCCityEvent::DispsePetEventAward, û����������¼� = %i", awardEvent.iIndex);
		return ;
	}

	G_NormalEventProbability  nep = {0};
	if (!CCEventScriptSetting::getSingleton().GetPetEvent(awardEvent.iIndex, nep))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposePetEventAward, û�����������ҵ������¼� = %i", awardEvent.iIndex);
		return;
	}

	DisposeApplyAwardEvent(pUser, nep.iEventID, awardEvent.iIndex, awardEvent.iAwardID,  awardEvent.et);
	pUser->m_userEvent.DeletePetEvent(awardEvent.iIndex);  
	//add by caihan
	//����¼��������¼������ж�
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_EVENT;
	taskEventInfo.iSubType = awardEvent.et;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
}

//�����佱��
CT_VOID CCCityEvent::DisposeTreasureEventAward(CCUser* pUser, const CMD_G_ApplyAwardEvent& awardEvent)
{
	if (!pUser->m_userEvent.FindTreasureEvent(awardEvent.iIndex))
	{
		DisposeEventTimeOut(pUser, awardEvent);
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeTreasureEventAward, û����������¼� = %i", awardEvent.iIndex);
		return ;
	}

	G_NormalEventProbability  nep = {0};
	if (!CCEventScriptSetting::getSingleton().GetTreasueEvent(awardEvent.iIndex, nep))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposePetEventAward, û�����������ҵ������¼� = %i", awardEvent.iIndex);
		return;
	}

	DisposeApplyAwardEvent(pUser, nep.iEventID, awardEvent.iIndex, awardEvent.iAwardID, awardEvent.et);
	pUser->m_userEvent.DeleteTreasureEvent(awardEvent.iIndex);    
	//add by caihan
	//����¼��������¼������ж�
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_EVENT;
	taskEventInfo.iSubType = awardEvent.et;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	//end
}

//����
CT_VOID CCCityEvent::DisposeHeartBeatEvent(CT_CHAR* pBuf, CT_DWORD len)
{
	CMD_G_Heart_Beat *pHeartBeat=(CMD_G_Heart_Beat*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pHeartBeat->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeHeartBeatEvent %d No Find UserID",pHeartBeat->dwUserID);
		return;
	}

	pUser->SetHeartBeat();
	return;
}


//�㲥
CT_VOID CCCityEvent::DisposeBroadcast(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_BroadCastMes))
	{
		CCLogMeg::getSingleton().WriteError(" CCCityEvent::DisposeBroadcast err: len != sizeof(CMD_G_BroadCastMes");
		return;
	}
	CMD_G_BroadCastMes* pBcm = (CMD_G_BroadCastMes*) pBuf;
	CCBroadcastScriptSetting::getSingleton().BroadcastMessage(*pBcm);
}

CT_VOID CCCityEvent::DisposeBroadcastWorldMessage(CT_CHAR* pBuf, CT_DWORD len)
{
	CCUserManager::getSingleton().BroadCastWorldMessage(pBuf,len);
}


//����ս���¼�
CT_VOID CCCityEvent::DisposeMscFightingEventEnd(const MSC_Event_Fighting_End &FightingEventEnd)
{
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(FightingEventEnd.dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeMscFightingEventEnd %d No Find UserID");
		return;
	}

	if (!pUser->m_userEvent.FindFightEvent(FightingEventEnd.iIndex))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeMscFightingEventEnd, û�����ս���¼� = %i", FightingEventEnd.iIndex);
		return ;
	}

	G_FightEventProbablity bep = {0};
	if (!CCEventScriptSetting::getSingleton().GetFightEvent(FightingEventEnd.iIndex, bep))
	{
		CCLogMeg::getSingleton().WriteError("CCCityEvent::DisposeMscFightingEventEnd, û�����������ҵ�ս���¼� = %i", FightingEventEnd.iIndex);
		return;
	}

	DisposeApplyAwardEvent(pUser, bep.iEventID, FightingEventEnd.iIndex, FightingEventEnd.iAwardID,  EVENT_FIGHT);
	pUser->m_userEvent.DeleteFightEvent(FightingEventEnd.iIndex);    

	return;
}

//���������
CT_VOID CCCityEvent::DisposeAnnouncementUpdata(CT_CHAR *pBuf, CT_DWORD len)
{
	Announcement *anno ;
	CMD_G_ANNOUNCEMENT cAnno = {0};
	if (len == sizeof(Announcement))
	{
		anno = (Announcement *)pBuf;
		anno->iFlag = 0;
		CCBroadcastScriptSetting::getSingleton().AnnouncementInit(anno->iIndex, *anno);

	    cAnno.iIndex = anno->iIndex;
		memcpy(cAnno.cContent, anno->cContent, ANNOUNCEMENT_CONTENT_LEN);
		CCUserManager::getSingleton().UpdataUserAnnouncement((CT_CHAR*)&cAnno, sizeof(CMD_G_ANNOUNCEMENT));
	}
	return;
}
