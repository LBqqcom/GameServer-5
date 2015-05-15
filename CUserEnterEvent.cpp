  #include <ctime>
#include <cstdlib>
#include "CUserEnterEvent.h"
#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CMarket.h"
#include "CUserEnterManager.h"
#include "CGameConfig.h"
#include "CTop.h"
#include "CCompress.h"
#include "CResourceGrabMgr.h"
#include "CTowerResManager.h"
#include "CBossManager.h"
#include "CActive.h"
#include "CBroadcastScriptSetting.h"
#include "CCSFightData.h"



CCUserEnterEvent::CCUserEnterEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	//��¼��Ϣ
	RegistMessage(MAIN_G_ENTER_ENV,  0);
	//������Ϣ����

	RegistMessage(MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NET_ERROR  );
	RegistMessage(MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_NO_USER    );
	RegistMessage(MSC_MAIN_DB_LOGIN, MSC_SUB_DB_R_ENTER_FINISH);


	RegistMessage(MAIN_C_GAME,SUB_C_ENTER_GANE);

	RegistMessage(MSC_MAIN_USER_ENTER, MSC_SUB_ENTER_ACTIEVE_ADD);
	RegistMessage(MSC_MAIN_USER_ENTER, MSC_SUB_ENTER_TRUNTABLE_ADD);
	RegistMessage(MSC_MAIN_USER_ENTER, MSC_SUB_ENTER_WORSHIP_ADD);
		
}

CCUserEnterEvent::~CCUserEnterEvent(void)
{
}

//�����û���Ϣ
CT_VOID CCUserEnterEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, 
									  CT_DWORD len,    CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_ENTER_ENV: 
		{
			
			CT_INT32 iHandler = 0;
			memcpy(&iHandler, pBuf + len - sizeof(CT_INT32), sizeof(CT_INT32) );
			DisposeEnter((CCNetInterface*)iHandler  , subID, pBuf, len - sizeof(CT_INT32));
				
			break;
		}

	case MSC_MAIN_DB_LOGIN:
		{
			DisposeDBLogin( subID,  pBuf,  len);

			break;
		}
	case MAIN_C_GAME:
		{
			DisposeValidate(subID,  pBuf,  len);
			break;
		}
	case MSC_MAIN_USER_ENTER:
		{
			if (MSC_SUB_ENTER_ACTIEVE_ADD == subID )
			{
				DisposeActiveAdd(pBuf, len);
			}else if (MSC_SUB_ENTER_TRUNTABLE_ADD == subID)
			{
				DisposeTrunTableAdd(pBuf, len);
			}else if (MSC_SUB_ENTER_WORSHIP_ADD == subID)
			{
				DisposeWorshipAdd(pBuf, len);
			}
			break;
		}

	default:
		{
			CCLogMeg::getSingleton().WriteError("CCEnvThread::OnUserEvent: Can't find main id: %i", mainID);
			break;
		}
	}
}




//�����½
CT_VOID CCUserEnterEvent::DisposeEnter(CCNetInterface* pNetHandle, CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len)
{
	if (len  != sizeof(CMD_G_EnterGame))
	{
		G_DisposeErrorMes(0, subID, "CCEnvThread::DisposeEnter : errorlen  != sizeof(CMD_G_EnterGame)");
		return;
	}

	CMD_G_EnterGame* pGeg = (CMD_G_EnterGame*)pBuf;	

	
	CCLogMeg::getSingleton().WriteError("һ�����û����룬 uid=%i, �������е��û���Ŀ=%i", pGeg->dwUserID,  CCUserManager::getSingleton().GetUserCount());

	if(!CCTop::getSingleton().GetInit())
	{
		CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::DisposeEnter Init No Finish");
#ifndef _WIN32
		//�û�ʧЧ
		CMD_G_EnterFail ef;
		ef.efReason = EF_SERVER_REFUSE;
		pNetHandle->Send(MAIN_G_ENTER_ENV,  SUB_G_ENTER_FAIL, (CT_CHAR*)&ef, sizeof(CMD_G_EnterFail)); 
		pNetHandle->Close();
		return;
#endif
	}

#ifndef _DEBUG
	if(CCUserEnterManager::getSingleton().Validate(*pGeg))
#endif
	{
		//�û�����
		if(CCUserManager::getSingleton().GetUserCount() >= CCConfig::getSingleton().m_maxClientNum)
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_PLAY_FULL);
			pNetHandle->Close();
			return;
		}

        CCUser *pUser=0;
        if(CCUserManager::getSingleton().Find(pGeg->dwUserID,&pUser))
		{
			//��ǰ�û�����
			//CCUser *pUser=0;
			//if(CCUserManager::getSingleton().Find(pGeg->dwUserID,&pUser))
			{
				if(pUser->GetNetInterface() != pNetHandle)
				{
					pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_REPEAT_LOGIN);
					pUser->CloseNet();
				}
			}

            pUser->ResetSyncTime(time(0));

            if(pUser->GetInit())
            {
                UserEnterInfo UserEnterinfo={0};
                if(CCUserEnterManager::getSingleton().Find(pGeg->dwUserID,UserEnterinfo))
                {
                    pUser->SetTalk(UserEnterinfo.bTalk);
                }

                //�����ػ�
                CCUserManager::getSingleton().SetNetHandle(pGeg->dwUserID, pNetHandle);        
                // �ػغ� [11/14/2012 John]
                CCUser* user = 0;
                if (CCUserManager::getSingleton().Find(pGeg->dwUserID, &user))
                {
                    user->m_userDaily.UpdateSignData();
                }
                SendUserData(pGeg->dwUserID, pNetHandle);

                //��Ӹ���
                MSC_SUB_TeamDuplicateLeave TeamLeave={0};
                TeamLeave.dwDstID=pGeg->dwUserID;
                TeamLeave.dwUserID=pGeg->dwUserID;
                CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
                G_SendThreadInfo(threadid,MSC_MAIN_TEAMDUPLICATE,MSC_SUB_TEAMDUPLICATE_LEAVE,(CT_CHAR*)&TeamLeave,sizeof(TeamLeave));

                return ;
            }
		}

		UserEnter(pGeg, pNetHandle);
	}
#ifndef _DEBUG
 	else
 	{
		CCLogMeg::getSingleton().WriteError("�û���֤��ʧЧ");
 		//�û�ʧЧ
 		CMD_G_EnterFail ef;
 		ef.efReason = EF_SERVER_REFUSE;
 		pNetHandle->Send(MAIN_G_ENTER_ENV,  SUB_G_ENTER_FAIL, (CT_CHAR*)&ef, sizeof(CMD_G_EnterFail)); 
		pNetHandle->Close();
 	}
#endif
}


CT_VOID CCUserEnterEvent::UserEnter(CMD_G_EnterGame* pGeg, CCNetInterface* pNetHandle)
{
	CCUser* pUser =  CCUserManager::getSingleton().GetOneFreeUser(pGeg->dwUserID, pNetHandle);
	if (pUser == 0)
	{
		CCLogMeg::getSingleton().WriteInfo(" CCEnvThread::UserEnter: Fail to Apply user, id=%i", pGeg->dwUserID);
		pNetHandle->Close();
		return ;
	}
	UserEnterInfo UserEnterinfo={0};
	if(CCUserEnterManager::getSingleton().Find(pGeg->dwUserID,UserEnterinfo))
	{
		pUser->SetTalk(UserEnterinfo.bTalk);
	}
	//������¼
	MSC_DB_UserEnter ue = {0};
	ue.dwUserID = pGeg->dwUserID;
	ue.pNetInterface = pNetHandle;
    CT_DWORD threadid = GAME_THREAD_CITY + pGeg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	ue.dwThreadID    = threadid;

	CT_DWORD dbThreadID = GAME_THREAD_DATABASE + pGeg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_USRE_ENTER, (CT_CHAR*)&ue, sizeof(ue) );
}


//���ݿ��¼��Ϣ
CT_VOID CCUserEnterEvent::DisposeDBLogin(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch (subID)
	{
	case MSC_SUB_DB_R_ENTER_FINISH:
		{
			if (len  != sizeof(MSC_DB_R_BaseInfo))
			{
				G_DisposeErrorMes(0, subID, "CCEnvThread::DisposeEnter : errorlen  != sizeof(MSC_DB_R_BuildingData)");
				return;
			}

			MSC_DB_R_BaseInfo* pUser = (MSC_DB_R_BaseInfo*)pBuf;
            // ��½�ɹ��� [11/14/2012 John]
            CCUser* user = 0;
            if (CCUserManager::getSingleton().Find(pUser->dwUserID, &user))
            {
				MasterVariableData mvd={0};
				user->m_userBase.GetMasterData(mvd);
				CCLogMeg::getSingleton().WriteError("enter finish user:%d Grade:%d",user->GetUserID(),mvd.stGrade.iLevel);

                user->m_userDaily.UpdateSignData();
                user->m_userDaily.UpdateQuestData(DAILY_QUEST_SIGNIN, 1);
				user->SetInit(CT_TRUE);
                user->m_userElmAttr.FixElmAttr();
				user->m_userBase.SyncStamina();
				user->UpdateFactorData();

            }

			SendUserData(pUser->dwUserID, pUser->pNetInterface);
			break;
		}
	case  MSC_SUB_DB_R_NET_ERROR:
		{
			if (len  != sizeof(MSC_DB_R_BaseInfo))
			{
				G_DisposeErrorMes(0, subID, "CCEnvThread::DisposeEnter : errorlen  != sizeof(MSC_DB_NetError)");
				return;
			}

			MSC_DB_R_BaseInfo* p = (MSC_DB_R_BaseInfo*)pBuf;
			CMD_G_EnterFail ef;
			ef.efReason = EF_SERVER_NET_ERROR;
			p->pNetInterface->Send(MAIN_G_ENTER_ENV, SUB_G_ENTER_FAIL, (CT_CHAR*)&ef, sizeof(ef));
			break;
		}

	case MSC_SUB_DB_R_NO_USER:
		{
			if (len  != sizeof(MSC_DB_R_BaseInfo))
			{
				G_DisposeErrorMes(0, subID, "CCEnvThread::DisposeEnter : errorlen  != sizeof(MSC_DB_NetError)");
				return;
			}

			MSC_DB_R_BaseInfo* p = (MSC_DB_R_BaseInfo*)pBuf;
			CMD_G_EnterFail ef;
			ef.efReason = EF_SERVER_DB_NO_USER;
			p->pNetInterface->Send(MAIN_G_ENTER_ENV, SUB_G_ENTER_FAIL, (CT_CHAR*)&ef, sizeof(ef));
			break;
		}
	default:
		break;
	}
}

//��¼��֤��
CT_VOID CCUserEnterEvent::DisposeValidate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch(subID)
	{
	case SUB_C_ENTER_GANE:
		{
			if(sizeof(CMD_C_EnterGameSuccess) != len)
			{
				CCLogMeg::getSingleton().WriteError("sizeof(CMD_C_EnterGameSuccess) != len");
				return;
			}

			CMD_C_EnterGameSuccess *pEnterInfo=(CMD_C_EnterGameSuccess*)pBuf;

			UserEnterInfo UserInfo={0};
			UserInfo.iID=pEnterInfo->dwGameID;
			memcpy(UserInfo.cbValidate,pEnterInfo->cbValidate,sizeof(UserInfo.cbValidate));
			UserInfo.llEnterTime=time(0);
			UserInfo.bTalk=pEnterInfo->bTalk;
			CCUserEnterManager::getSingleton().Insert(UserInfo);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::DisposeValidate subID:%d",subID);
			return;
		}
	}

	return;
}
//�����û���Ϣ
CT_VOID CCUserEnterEvent::SendUserData(CT_DWORD userID, CCNetInterface* pNetHandle)
{
	CT_CHAR  buf[MAX_MES_MAX_BUFF_LEN];
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	CT_INT32 bufLen = MAX_MES_MAX_BUFF_LEN;


	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(userID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::SendUserData, fail to find %i", userID);
		return ;
	}

	//�����Զ���������
	pUser->m_userBuilding. SysProductingBuilding();	

	//�������������
	pUser->CalMaxWorker();
	//������й�����Ŀ
	pUser->CountFreeWorker();    

	if( pUser->m_userBase.GetMasterData( buf, bufLen))
	{        
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_MASTER_INFO,  (CT_CHAR*)buf, bufLen);
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen=MAX_MES_MAX_BUFF_LEN;
	if(pUser->m_userBase.GetMasterFightAdd(buf,bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_MASTER_FIGHTADD,(CT_CHAR*)buf, bufLen);
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;

	if( pUser->m_userBuilding. GetBuildingData( buf, bufLen))
	{
		if (bufLen > 0)
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_BUILDING_INFO, buf, bufLen);
		}
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;

	if( pUser->m_userBuilding. GetUpgardingBuildingData( buf, bufLen))
	{
		if (bufLen > 0)
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_UPGRADING_BUILDING_INFO, buf, bufLen);
		}
	}


	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userEvent.GetEvents(buf, bufLen))
	{
		if (bufLen > 0)
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_EVENTS, buf, bufLen);
		}
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;

	if (pUser->m_userBuilding. GetProductingData(buf, bufLen))
	{
		if (bufLen > 0)
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_PRODUCTING_BUILDING_INFO, buf, bufLen);
		}
	}

    // ���ͼ�����Ϣ
    pUser->m_userSkill.SendClientData(MAIN_G_ENTER_ENV, SUB_G_SKILL_INFO, buf, sizeof(buf));
	// ����Ǳ����Ϣ
    pUser->m_userTalent.SendClientData(MAIN_G_ENTER_ENV, SUB_G_TALENT_INFO, buf, sizeof(buf));
	// ���͵�����Ϣ
    pUser->m_userItem.SendClientData(MAIN_G_ENTER_ENV, SUB_G_ITEM_INFO, buf, sizeof(buf));
    // ����װ����Ϣ
    pUser->m_userEquip.SendClientData(MAIN_G_ENTER_ENV, SUB_G_EQUIP_INFO, buf, sizeof(buf));
    // ����Ԫ��֮����Ϣ
    pUser->m_userElmPower.SendClientData(MAIN_G_ENTER_ENV, SUB_G_ELMPOWER_INFO, buf, sizeof(buf));
    // ����ս����Ϣ
    pUser->m_userBattleFlag.SendClientData(MAIN_G_ENTER_ENV, SUB_G_BATTLEFLAG_INFO, buf, sizeof(buf));
    // ����Ԫ��������Ϣ
    pUser->m_userElmAttr.SendClientData(MAIN_G_ENTER_ENV, SUB_G_ELMATTR_INFO, buf, sizeof(buf));
    // ����������Ϣ
    pUser->m_userBuff.SendClientData(MAIN_G_ENTER_ENV, SUB_G_BUFF_INFO, buf, sizeof(buf));   
	// ���;�����Ϣ
	pUser->m_userArmy.SendClientData(MAIN_G_ENTER_ENV, SUB_G_ARMY_INFO, buf, sizeof(buf));   

	//// ���;�����Ϣ
	//memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	//bufLen = MAX_MES_MAX_BUFF_LEN;
	//if (pUser->m_userArmy.GetArmyInfo(buf, bufLen))
	//{
	//	pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ARMY_INFO, buf, bufLen);
	//}

	// ���ͳ�����Ϣ
	/*memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->GetPetData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_PET_INFO, buf, bufLen);
	}*/

	// Begin [10/22/2012 wangchen]
	// ���͸�����Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->GetDuplicateData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_DUPLICATE_INFO, buf, bufLen);
	}

	// End [10/22/2012 wangchen]
	//����������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userRegion.GetUnlockRegionData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_REGION_INFO, buf, bufLen);
	}
	//����δ���������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userTask.GetUnFinishedTask(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_TASK_INFO, buf, bufLen);
	}

    // �����ճ�������Ϣ
    memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
    bufLen = MAX_MES_MAX_BUFF_LEN;
    if (pUser->m_userDaily.GetDailyQuestData(buf, bufLen))
    {
        pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_DAILY_QUEST, buf, bufLen);
    }

    // ���ͻ�Ծ����Ϣ
    memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
    bufLen = MAX_MES_MAX_BUFF_LEN;
    if (pUser->m_userDaily.GetActiveData(buf, bufLen))
    {
        pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ACTIVE_DATA, buf, bufLen);
    }  

	// ���;�������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userPVPInfo.GetPVPInfo(buf, bufLen))
	{
		CT_INT32 iOutLen=MAX_MES_MAX_BUFF_LEN;
		if(CCCompress::getSingleton().Compress(buf,bufLen,buf,iOutLen))
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_PVP_INFO, buf, iOutLen);
		}
	}    

	//���;�����ս��
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if(pUser->m_userPVPInfo.GetPVPRectord(buf,bufLen))
	{
		CT_INT32 iOutLen=MAX_MES_MAX_BUFF_LEN;
		if(CCCompress::getSingleton().Compress(buf,bufLen,buf,iOutLen))
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_PVP_FIGHTING_RECORD, buf, iOutLen);
		}

	}
	
	//�����̵���Ϣ
	CCMarket::getSingleton().SendUserMarketInfo(pUser);
	pUser->m_userBlackMarket.SendBlackRefreshTime();
//	pUser->m_userBlackMarket.RefreshShopTime();

	// ���͸����޹�
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userBlackMarket.GetMarketPersonal(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_MARKET_PARSONAL, buf, bufLen);
	}

	// ����������ȴ��Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userBase. GetCooldownData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_COOLDOWN_INFO, buf, bufLen);
	}

	//���ͳ�����Ϣ
	pUser->m_userPet.SendUserPetInfo();
	//����vip��Ϣ
	pUser->m_userVip.VipSendInfo();
	//���ͻ��Ϣ
	pUser->m_userActive.SendUserActiveInfo();
	//���ͼ�����Ϣ
    pUser->m_userWorship.SendUserWorshipInfo();

    // ���͸�ħ��Ϣ
    pUser->m_userEnchant.SendClientData(MAIN_G_ENTER_ENV, SUB_G_ENCHANT_DATA, buf, sizeof(buf));
	// �����ѻ�ø�ħ����
    memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
    bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userEnchant.GetEnchantReward(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ENCHANT_REWARD_DATA, buf, bufLen * sizeof(CT_INT32));
	}

	// ���ͺ�����Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userFriend.GetFriendData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FRIEND_INFO, buf, bufLen);
	} 

	// ���ͺ���������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userFriend.GetFriendReq(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FRIEND_REQ, buf, bufLen);
	}

	// ���ͺ��Ѻ�������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userFriend.GetBlackName(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FRIEND_BLACKNAME, buf, bufLen);
	}

	//������Դ�㳡��������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCResourceGrabMgr::getSingleton().GetResourceScene(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_RESSPOT_SCENE_OPEN, buf, bufLen);
	}

	// ������Դ���᳡����Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCResourceGrabMgr::getSingleton().GetUserResData(userID, buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_RESSPOT_SCENCE_INFO, buf, bufLen);
	}

	//����Ԫ��֮����Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCTowerResManager::getSingleton().GetUserResData(userID, buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ELEMENT_TOWER_SCENCE_INFO, buf, bufLen);
	}

	//Ԫ����ͨ������
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userTower.GetTowerData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_TOWER_DATA_INFO, buf, bufLen);
	}

	//ռ�����������
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userTower.GetTowerSlotAward(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_TOWER_SLOTAWARD_INFO, buf, bufLen);
	}

	//ϵͳ����
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userTower.GetTowerMSG(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_TOWER_MSG_INFO, buf, bufLen);
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userTower.GetEventData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ELEMENT_TOWER_EVENT_INFO, buf, bufLen);
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCTowerResManager::getSingleton().GetTowerFightExample(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_TOWER_FIGHT_INFO, buf, bufLen);
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userTower.GetHelpData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ELEMENT_TOWER_HELP_INFO, buf, bufLen);
	}

	//���ͻ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userActive.GetPresent(buf, bufLen) && bufLen > 0)
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ACTIVE_PRESENT, buf, bufLen);
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userActive.GetPresentDesc(buf, bufLen) && bufLen > 0)
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ACTIVE_PRESENT_DESC, buf, bufLen);
	}

	//���ͻ����
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCActive::getSingleton().GetActiveDsc(buf,bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ACTIVE_DEC, buf, bufLen);
	}

	//����δ��������
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->GetFunctionOpen(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FUNCTION_OPEN, buf, bufLen);
	}

	//���ͻ��Դ��
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if(CCResourceGrabMgr::getSingleton().GetResActive())
	{
		CMD_G_ResActive ra={0};
		time_t t=time(0);
		ra.iTime=CCResourceGrabMgr::getSingleton().GetLastTime();
		ra.iTime=(ra.iTime <= -1?-1:ra.iTime-t);
		memcpy(buf,&ra,sizeof(ra));
		if (CCResourceGrabMgr::getSingleton().GetResActiveSpot(buf+sizeof(ra),bufLen))
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_RESSPOT_ACTIVE_OPEN, buf, bufLen+sizeof(ra));
		}
	}
	else
	{
		CMD_G_ResActive ra={0};
		time_t t=time(0);
		ra.iTime=CCResourceGrabMgr::getSingleton().GetLastTime();
		ra.iTime=(ra.iTime <= -1?-1:ra.iTime-t);
		if(ra.iTime != -1)
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_RESSPOT_ACTIVE_CLOSE, (CT_CHAR*)&ra, sizeof(ra));
	}

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if(pUser->m_userBoss.GetRecord(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_WORLD_BOSS_RECORD, buf, bufLen);
	}

	//��������BOSS�
	CMD_G_BossActive ra={0};
	ra.iTime=CCBossManager::getSingleton().GetLastTime();
	ra.iTime=(ra.iTime <= -1?-1:ra.iTime-time(0));
	ra.iAEID=CCBossManager::getSingleton().GetAEID();
	ra.iHPMax=CCBossManager::getSingleton().GetBossHPMax();
	ra.iHP=CCBossManager::getSingleton().GetBossHP();
	ra.iBossGrade=CCBossManager::getSingleton().GetBossGrade();
	if(CCBossManager::getSingleton().GetActive())
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_WORLD_BOSS_ACTIVE_OPEN, (CT_CHAR*)&ra, sizeof(ra));
	}
	else
	{
		if(ra.iTime != -1)
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_WORLD_BOSS_ACTIVE_CLOSE, (CT_CHAR*)&ra, sizeof(ra));
		else
			CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::BOSS_ACTIVE, iTime == -1");
	}
	CCBossManager::getSingleton().UserLogin(userID);

	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCActive::getSingleton().GetReward(buf,bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ACTIVE_REWARD_UPDATE, buf, bufLen);
	}

	//������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if(pUser->m_userDuplicate.GetSceneInfo(buf,bufLen))
	{
		CT_INT32 iOutLen=MAX_MES_MAX_BUFF_LEN;

		CMD_G_SceneInfo si={0};
		memcpy(&si,buf,sizeof(si));
		//si.iCompress=CT_TRUE;
		//memcpy(buf,&si,sizeof(si));
		if(CCCompress::getSingleton().Compress(buf+sizeof(si),bufLen,buf+sizeof(si),iOutLen))
		//if(CCCompress::getSingleton().Compress(buf,bufLen+sizeof(si),buf,iOutLen))
		{
			si.iCompress=CT_TRUE;
			memcpy(buf,&si,sizeof(si));
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_SCENE_INFO, buf, iOutLen+sizeof(si));
			//pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_SCENE_INFO, buf, iOutLen);
		}
		else
		{
			si.iCompress=CT_FALSE;
			memcpy(buf,&si,sizeof(si));
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_SCENE_INFO, buf, bufLen+sizeof(si));
		}
	}

	//����Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if(pUser->m_userBattleArray.GetBattleArray(buf,bufLen))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_BATTLE_ARRAY_INFO,buf,bufLen);
	}

	//������Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if(pUser->m_userBase.GetMasterTopInfo(buf,bufLen))
	{
		pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_RANKINGLIST_INFO,buf,bufLen);
	}


	// ����ս��״̬ - Ҫ�������������֮����� [10/15/2012 John]
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->GetBattleStatusData(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_BUTTLE_STATUS_INFO, buf, bufLen);
	}

	// ����ս����Ϣ    UpdateBattleStatus()֮������û�ս����������
	CMD_G_FightTeamBase fightTeamBase = {0};
	CT_DWORD dwFTID=0;
	CCFightTeam *pFightTeam=0;
	dwFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(userID);
	if (CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
	{
		pFightTeam->GetFightTeamBase(&fightTeamBase);
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FIGHTTEAM_BASE, (CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase));

		//ս���ٻ�boss
		FightTeamSummonBoss sb={0};
		if(pFightTeam->GetSummonInfo(sb))
		{
			CMD_G_FightTeamHelpSummonInfo hsInfo={0};
			hsInfo.dwUserID=userID;
			hsInfo.iErrorID=(sb.iWiner == FT_SUMMON_WIN?sb.iWiner:FT_SUMMON_SUC);
			hsInfo.iBossIndex=sb.iBossIndex;
			memcpy(hsInfo.dwHelpSummonID,sb.dwUserID,sizeof(hsInfo.dwHelpSummonID));
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FIGHTTEAM_SUMMON_BOSS_INFO, (CT_CHAR*)&hsInfo, sizeof(hsInfo));
		}

		//�ٻ�����
		memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
		bufLen = MAX_MES_MAX_BUFF_LEN;
		if(pFightTeam->GetSummonRewardRes(userID, buf, bufLen))
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FIGHTTEAM_SUMMON_REWARD_INFO, buf, bufLen);
		}


		//�ٻ�����
		memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
		bufLen = MAX_MES_MAX_BUFF_LEN;
		if (pFightTeam->GetSummonRewardItem(userID, buf, bufLen))
		{
			pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FIGHTTEAM_SUMMON_REWARD_ITEM, buf, bufLen);
		}

		//ʱ��boss
		FTTimeBossInfo tbi={0};
		CT_INT32 iHelpCount=0;
		pFightTeam->GetTimeBossInfo(tbi);
		for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
		{
			if(tbi.iHelpUser[i] != 0)
				iHelpCount++;
		}

		G_ValueRange vrft={0};
		if(!G_GetValueRange(VALUE_TYPE_FT_TIME_BOSS_FIGHTTIME,&vrft))
		{
			CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::SendUserData G_GetValueRange VALUE_TYPE_FT_TIME_BOSS_FIGHTTIME");
		}
		G_ValueRange vrbs={0};
		if(!G_GetValueRange(VALUE_TYPE_FT_TIME_BOSS_SUMMON,&vrbs))
		{
			CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::SendUserData G_GetValueRange VALUE_TYPE_FT_TIME_BOSS_SUMMON");
		}

		//������ȴʱ��
		CooldownData cd={0};
		CT_INT32 lTime=time(0);
		pUser->m_userBase.GetCooldownData(FTTIMEBOSS_SUMMON,cd);
		if(cd.leftTime+ cd.toggleTime <= lTime)
		{
			cd.iType=FTTIMEBOSS_SUMMON;
			cd.toggleTime=lTime;
			cd.leftTime=tbi.iSummonBeginTime+vrbs.fMaxVal-lTime;
			pUser->m_userBase.InsertCooldownData(cd);
			pUser->m_userBase.UpdateCooldown(FTTIMEBOSS_SUMMON,0);
		}

		if(tbi.iCurrentHP == pFightTeam->GetTBMaxHP() && tbi.iSummonBeginTime != 0 && iHelpCount < HELP_SUMMON_FIGHT_COUNT)
		{
			CMD_G_FTTimeBossInfo bi={0};
			bi.dwUserID=pUser->GetUserID();
			bi.iBossID=tbi.iIndex;
			memcpy(bi.dwHelpSummonID,tbi.iHelpUser,sizeof(bi.dwHelpSummonID));
			pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_FIGHTTEAM_TIMEBOSS_INFO,(CT_CHAR*)&bi,sizeof(bi));
		}
		else if(tbi.iCurrentHP <= pFightTeam->GetTBMaxHP() && tbi.iCurrentHP != 0)
		{
			CMD_G_FTTimeBossFightInfo bfi={0};
			FTTimeBossMember bm={0};
			CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
			CT_DWORD dwLen=MAX_MES_MAX_BUFF_LEN;
			pFightTeam->GetTimeBossMember(userID,bm);
			bfi.iBossID=tbi.iIndex;
			bfi.iBossMaxHP=pFightTeam->GetTBMaxHP();
			bfi.iCurrentHP=tbi.iCurrentHP;
			bfi.iFTBossTime=tbi.iSummonBeginTime+vrft.fMaxVal-lTime;
			bfi.iFTContribute=bm.iFTContribute;
			bfi.iFTFood=bm.iFTFood;
			bfi.iFightNum=bm.iFightNum;
			
			pFightTeam->GetFightTeamMember(cBuf,dwLen);
			for(CT_INT32 i=0;i<dwLen/sizeof(CMD_G_FightTeamMember) && i < HELP_SUMMON_COUNT;++i)
			{
				CMD_G_FightTeamMember ftm={0};
				memcpy(&ftm,cBuf+i*sizeof(CMD_G_FightTeamMember),sizeof(CMD_G_FightTeamMember));
				bfi.dwUserID[i]=ftm.dwUserID;
				memset(&bm,0,sizeof(bm));
				pFightTeam->GetTimeBossMember(ftm.dwUserID,bm);
				bfi.iHurt[i]=bm.iHurt;
				
			}


			for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
			{
				if(tbi.iHelpUser[i] != 0)
					bfi.iMemberCount++;
			}
			pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_FIGHTTEAM_TIMEBOSS_FIGHT_INFO,(CT_CHAR*)&bfi,sizeof(bfi));
		}
		else if(tbi.iIndex != 0)
		{
			CMD_G_FTTimeBossIndex tbbi={0};
			tbbi.iIndex=tbi.iIndex;
			pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_FIGHTTEAM_TIMEBOSS_INDEX,(CT_CHAR*)&tbbi,sizeof(tbbi));

			memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
			bufLen = MAX_MES_MAX_BUFF_LEN;
			if(pFightTeam->GetTBReward(buf,bufLen))
				pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_FIGHTTEAM_TIMEBOSS_REWARD_INFO,buf,bufLen);
		}
	}

	CCFightTeamManager::getSingleton().UserLoginEvent(userID);	// ����ս�ӳ�Ա����״̬

	//�Ƽ���Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userFightTeamNember.GetNemberScience(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_FIGHTTEAM_SCIENCE_INFO, buf, bufLen);
	}

	//���͹���
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCBroadcastScriptSetting::getSingleton().GetAnnouncementByIndex(buf,bufLen,ANNONCEMENT_TOWN))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ANNOUNCEMENT, buf, bufLen);
	}

	//���ͻ����
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (CCBroadcastScriptSetting::getSingleton().GetAnnouncementByIndex(buf,bufLen,ANNONCEMENT_ACTIVE))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ACTIVENOTICE, buf, bufLen);
	}

	//���ս
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	CMD_G_CrossBaseInfo cb={0};
	cb.iStatus=CCCSFightData::getSingleton().GetStatus();
	memcpy(buf,&cb,sizeof(cb));
	bufLen-=sizeof(cb);
	CCCSFightData::getSingleton().GetUserReward(pUser->GetUserID(),buf+sizeof(cb),bufLen);
	pNetHandle->Send(MAIN_G_CROSS_SERVER,SUB_G_CROSS_SERVER_BASEINFO, buf, bufLen+sizeof(cb));

	//ͷ��
	pUser->m_userTitle.SendClientData(MAIN_G_ENTER_ENV, SUB_G_TITLE_DATA, buf, sizeof(buf));
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userTitle.GetTitleList(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_TITLE_LIST, buf, bufLen);
	}

	//����
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userIsland.GetIslandData(buf, bufLen, fightTeamBase.dwFTID))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ISLAND_DATA, buf, bufLen);
	}

	//��ǽ�ӳ���Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userSeek.GetWallAddition(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_SEEK_WALL_ADDITION_INFO, buf, bufLen);
	}

	//��ǽ����
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userSeek.GetWallArmy(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_SEEK_ARMY_INFO, buf, bufLen);
	}

	//̽����Ϣ
	memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
	bufLen = MAX_MES_MAX_BUFF_LEN;
	if (pUser->m_userSeek.GetSeekInfo(buf, bufLen))
	{
		pNetHandle->Send(MAIN_G_ENTER_ENV,SUB_G_SEEK_DATA, buf, bufLen);
	}


	//�������(�ͻ���δʹ��)
	//pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ENTER_SUC);

	//����ɨ������
	MSC_RaidsReward RaidsReward={0};
	RaidsReward.dwUserID=pUser->GetUserID();
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_RAIDS_REWARD,(CT_CHAR*)&RaidsReward,sizeof(RaidsReward));	

	pNetHandle->Send(MAIN_G_ENTER_ENV, SUB_G_ENTER_FINISH);
	//���߹㲥
	{
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		if(strlen(masterData.cNickName) == 0)
			return;
		// ս�������й㲥
		if (masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingCur > 0 && masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingCur < 4)
		{
			RollBroadcast rb[2]={0};
			rb[0].SetValue(masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingCur,"");
			rb[1].SetValue(userID, masterData.cNickName);
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{26}<6:%d>{17}<1:%d:%s>{27}",masterData.mvd.topData.TopInfo[TOP_TYPE_POWER].iRankingCur, userID, masterData.cNickName);
			G_SendRollBroadecast(26,2,1,(CT_CHAR*)rb,sizeof(rb));
		}

		// �ȼ����й㲥
		if (masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingCur > 0 && masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingCur < 4)
		{
			RollBroadcast rb[2]={0};
			rb[0].SetValue(masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingCur,"");
			rb[1].SetValue(userID, masterData.cNickName);
			//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{41}<6:%d>{17}<1:%d:%s>{27}",masterData.mvd.topData.TopInfo[TOP_TYPE_GRADE].iRankingCur,userID, masterData.cNickName);
			G_SendRollBroadecast(41,2,1,(CT_CHAR*)rb,sizeof(rb));
		}

		// װ��ӵ�й㲥
		memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
		bufLen = MAX_MES_MAX_BUFF_LEN;
		if (pUser->m_userItem. GetEquipedItemData(buf, bufLen))
		{
			CMD_G_ItemData *pItemData = 0;
			CT_INT32 iCount = bufLen / sizeof(CMD_G_ItemData);
			for(CT_INT32 i=0; i<iCount; ++i)
			{
				pItemData = (CMD_G_ItemData *)(buf + i * sizeof(CMD_G_ItemData));
				if (G_BroadcastFilter(8, pItemData->iID))
				{
					RollBroadcast rb[2]={0};
					rb[0].SetValue(pItemData->iID,"");
					rb[1].SetValue(userID, masterData.cNickName);
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{36}<2:%d>{17}<1:%d:%s>{37}",pItemData->iID, userID, masterData.cNickName);
					G_SendRollBroadecast(36,2,1,(CT_CHAR*)rb,sizeof(rb));
				}
				else if (G_BroadcastFilter(9, pItemData->iID))
				{
					RollBroadcast rb[2]={0};
					rb[0].SetValue(pItemData->iID,"");
					rb[1].SetValue(userID, masterData.cNickName);
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "{36}<2:%d>{17}<1:%d:%s>{37}",pItemData->iID, userID, masterData.cNickName);
					G_SendRollBroadecast(129,2,1,(CT_CHAR*)rb,sizeof(rb));
				}
			}
		}

		//ͷ�ι㲥
		if(pUser->m_userTitle.GetUseTitleID() == TITLE_CHOMPION_ID)
		{
			RollBroadcast rb={0};
			rb.SetValue(pUser->GetUserID(),masterData.cNickName);
			G_SendRollBroadecast(132,2,1,(CT_CHAR*)&rb,sizeof(rb));
		}
		else if(pUser->m_userTitle.GetUseTitleID() == TITLE_THE_BEST_TEAM_ID)
		{
			RollBroadcast rb={0};
			rb.SetValue(pUser->GetUserID(),masterData.cNickName);
			G_SendRollBroadecast(133,2,1,(CT_CHAR*)&rb,sizeof(rb));
		}
		else if(pUser->m_userTitle.GetUseTitleID() == TITLE_BIG_LOCAL_TYRANTS)
		{
			RollBroadcast rb={0};
			rb.SetValue(pUser->GetUserID(),masterData.cNickName);
			G_SendRollBroadecast(134,2,1,(CT_CHAR*)&rb,sizeof(rb));
		}
		else if(pUser->m_userTitle.GetUseTitleID() == TITLE_MOST_RICH)
		{
			RollBroadcast rb={0};
			rb.SetValue(pUser->GetUserID(),masterData.cNickName);
			G_SendRollBroadecast(138,2,1,(CT_CHAR*)&rb,sizeof(rb));
		}
	}
}

CT_VOID CCUserEnterEvent::Decompress(CT_INT32 *InBuffer,CT_INT32 iInLen,CT_INT32 *OutBuffer)
{
#if 0
	//����ѹ��
	CT_INT32 *pBuffer1=0;
	CT_INT32 iOutLen1=0;
	CT_INT32 iLen1=iInLen;
	CT_INT32 iOutLen=0;
	while(CT_TRUE)
	{
		iLen1=iLen1*2;
		pBuffer1=new CT_INT32[iLen1];
		CT_INT32 *pInBuffer1=InBuffer;
		iOutLen=LZF_Decompress(&pInBuffer1,iInLen,(CT_INT32**)&pBuffer1,iLen1);
		if(iOutLen != 0)
			break;

		if(iLen1 > MAX_MES_MAX_BUFF_LEN)
		{
			printf("0");
			return; 
		}
		delete[] pBuffer1;
		pBuffer1=0;
	}

	if(memcmp(OutBuffer,pBuffer1,iOutLen) == 0)
	{
		printf("0");
	}

	delete[] pBuffer1;
	pBuffer1=0;
#endif
	return;
}

//��ͨ�����
CT_VOID CCUserEnterEvent::DisposeActiveAdd(CT_CHAR* pBuf,  CT_DWORD len)
{
	if ( sizeof(MSC_ACTIVE_Status ) != len)
	{
		return;
	}
	MSC_ACTIVE_Status* pStatus = (MSC_ACTIVE_Status*)pBuf;
	
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pStatus->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::DisposeActiveAddInfo No Find UserID:%d",pStatus->dwUserID);
		return ;
	}
	pUser->m_userActive.SetActiveStatus(pStatus->iAID,pStatus->iStatus,pStatus->iQuanity);
}

//ת�̻���
CT_VOID CCUserEnterEvent::DisposeTrunTableAdd(CT_CHAR* pBuf,  CT_DWORD len)
{
	if ( sizeof(MSC_TurnTable_Status ) != len)
	{
		return;
	}
	MSC_TurnTable_Status* pStatus = (MSC_TurnTable_Status*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pStatus->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::DisposeActiveAddInfo No Find UserID:%d",pStatus->dwUserID);
		return ;
	}
	pUser->m_userActive.SetTurntableCount(pStatus->iTimes1,pStatus->iTimes2,pStatus->iTimes3);
}

//�������
CT_VOID CCUserEnterEvent::DisposeWorshipAdd(CT_CHAR* pBuf,  CT_DWORD len)
{
	if (sizeof(MSC_DB_WorshipStatus) != len)
	{
		return;
	}

	MSC_DB_WorshipStatus* pStatus = (MSC_DB_WorshipStatus*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pStatus->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCUserEnterEvent::DisposeWorshipAdd No Find UserID:%d",pStatus->dwUserID);
		return ;
	}

	pUser->m_userWorship.GetWorshipStatus(pStatus->iType,pStatus->iStatus,pStatus->iXp);
}
