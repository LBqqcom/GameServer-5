#include <ctime>
#include "CUserExitEvent.h"
#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "ProtocolGame.h"
#include "CGameConfig.h"
#include "CUserEnterManager.h"
#include "CResourceGrabMgr.h"
#include "CTowerResManager.h"
#include "CFightingData.h"

CCUserExitEvent::CCUserExitEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	//�û��뿪
	RegistMessage(MAIN_G_LEAVE_ENV,  0);
	RegistMessage(MSC_MAIN_USER_EXIT,0);
}

CCUserExitEvent::~CCUserExitEvent(void)
{
}

//�����û���Ϣ
CT_VOID CCUserExitEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, 
									  CT_DWORD len,    CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MSC_MAIN_USER_EXIT:
	case MAIN_G_LEAVE_ENV:
		{
			DisposeLeave((CCNetInterface*)reserve, subID, pBuf, len);
			break;
		}

	default:
		{
			CCLogMeg::getSingleton().WriteError("CCEnvThread::OnUserEvent: Can't find main id: %i", mainID);
			break;
		}
	}
}



//�û��뿪
CT_BOOL CCUserExitEvent::DisposeLeave(CCNetInterface* pNetHandle,CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len)
{
	//������֤
	if(len != sizeof(CMD_G_LeaveGame))
	{
		G_DisposeErrorMes(0, subID, "CCEnvThread::DisposeEnter : errorlen  != sizeof(CMD_G_LeaveGame)");
		pNetHandle->Close();
		return false;
	}

	CMD_G_LeaveGame *pLeaveGame=(CMD_G_LeaveGame*)pBuf;

	CMD_C_LeaveGame LeaveGame;
	memset(&LeaveGame,0,sizeof(LeaveGame));

	LeaveGame.dwUserID=pLeaveGame->dwUserID;
	LeaveGame.iServerID=g_iServerID;

	g_pPCentServer->Send(MAIN_C_GAME,SUB_C_USER_LEAVE,(CT_CHAR*)&LeaveGame,sizeof(LeaveGame));

	CCUser* pUser = 0;
	if(CCUserManager::getSingleton().Find(pLeaveGame->dwUserID, &pUser))
	{
		CT_INT64 currTime = time(0);
		CT_DWORD dbThreadID = GAME_THREAD_DATABASE + pLeaveGame->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;

		if(pUser->GetInit())
		{
			//ս�����ݸ���
			CCFightingData::getSingleton().UpdataFightingData(pLeaveGame->dwUserID);

			//����pvp��ȴʱ�䴦��
			pUser->m_userPVPInfo.UpdataPVPCoolTime();
			PVPInfo pvpInfo = {0};
			pUser->m_userPVPInfo.GetPVPInfo(pvpInfo,CT_TRUE);

			if(pvpInfo.dwUserID != 0)
			{
				MSC_DB_W_PVPInfo dbPVPInfo = {0};
				dbPVPInfo.dwUserID = pvpInfo.dwUserID;
				dbPVPInfo.iFightingCount = pvpInfo.iFightingCount;
				dbPVPInfo.lRefreshTime   = pvpInfo.lRefreshTime;
				dbPVPInfo.cbChestStatus  = pvpInfo.cbChestStatus;
				dbPVPInfo.iPVPDegree	 = pvpInfo.iPVPDegree;
				dbPVPInfo.iPVPChestNum	 = pvpInfo.iPVPChestNum;
				G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_PVP_DATA, MSC_SUB_DB_R_PVP_INFO, (CT_CHAR*)&dbPVPInfo, sizeof(dbPVPInfo));
			}

			// װ��ǿ����ȴʱ�䴦��
			pUser->m_userBase.SyncCooldown(currTime);
			G_SaveMastVariableData(pUser, SAVETAG_EVENT_EXIT); //�û��˳���Ϸ����Ҫ�洢����ֵ

			//��¼�û��������
			MSC_LOGDB_ChatTime	chatTime={0};
			chatTime.dwUserID=pUser->GetUserID();
			chatTime.iChatTime=pUser->GetChatTime();
			CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_CHATTIME,(CT_CHAR*)&chatTime,sizeof(chatTime));

			//��Ӹ���
			MSC_SUB_TeamDuplicateLeave TeamLeave={0};
			TeamLeave.dwDstID=pUser->GetUserID();
			TeamLeave.dwUserID=pUser->GetUserID();
			threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(threadid,MSC_MAIN_TEAMDUPLICATE,MSC_SUB_TEAMDUPLICATE_LEAVE,(CT_CHAR*)&TeamLeave,sizeof(TeamLeave));

			//ɨ������
			RaidsInfo RaidsData={0};
			MSC_DB_RaidsInfo Raidsinfo={0};
			MasterVariableData mvd={0};
			pUser->m_userBase.GetMasterData(mvd);
			pUser->m_userDuplicate.GetRaidsInfo(RaidsData);
			Raidsinfo.dwUserID=pUser->GetUserID();
			Raidsinfo.iWorldID=RaidsData.iWorldID;
			Raidsinfo.iSceneID=RaidsData.iSceneID;
			Raidsinfo.iDifficulty=RaidsData.iDifficulty;
			Raidsinfo.iAllTime=RaidsData.iAllTime;
			Raidsinfo.iCurrentTime=RaidsData.iCurrentTime;
			Raidsinfo.lBeginTime=RaidsData.lBeginTime;
			Raidsinfo.iLastStamina=mvd.stRes.iStamina;
			threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid, MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_RAIDS_INFO, (CT_CHAR*)&Raidsinfo, sizeof(Raidsinfo));

			//ս���ٻ��뿪
			CT_DWORD dwFTID=0;
			CCFightTeam *pFightTeam=0;
			dwFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pUser->GetUserID());
			if (CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
			{
				CMD_G_FightTeamHelpSummonLeave ftsl={0};
				ftsl.dwFTID=dwFTID;
				ftsl.dwUserID=pUser->GetUserID();
				threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				G_SendThreadInfo(threadid, MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_FIGHTTEAM_SUMMON_LEAVE, (CT_CHAR*)&ftsl, sizeof(ftsl));
			}

			//������� ǿ���û�����������ˢ�µ����ݿ�
			pUser->m_userBuilding.UpdateSaveBuildingData(CT_TRUE, CT_TRUE,  CT_TRUE);
			pUser->m_userBuilding.UpdateSaveProductingData(CT_TRUE, CT_TRUE);

			//��ӽ���

			// ͬ��������Ϣ [4/15/2013 John]
			pUser->m_userItem.SyncData(currTime, CT_TRUE);
            pUser->m_userElmPower.SyncData(currTime, CT_TRUE);
            pUser->m_userEquip.SyncData(currTime, CT_TRUE);
            pUser->m_userBuff.SyncData(currTime, CT_TRUE);
            pUser->m_userElmAttr.SyncData(currTime, CT_TRUE);

			//̽���б�
			/*MSC_DB_SeekPlayerList spl = {0};
			spl.dwUserID = pLeaveGame->dwUserID;
			pUser->m_userSeek.GetSeekListPlayer(SEEK_GOODWILL, (CT_CHAR*)spl.aWillPlayer, sizeof(spl.aWillPlayer));
			pUser->m_userSeek.GetSeekListPlayer(SEEK_SPITE, (CT_CHAR*)spl.aSpitePlayer, sizeof(spl.aSpitePlayer));
			threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid, MSC_MAIN_DB_SEEK, MSC_SUB_SEEK_LIST_UPDATE, (CT_CHAR*)&spl, sizeof(spl));*/
			pUser->m_userSeek.SyncData(currTime, CT_TRUE);

			//̽�ü�¼���͸��Է�
			VistorInfo vistorInfo[SEEK_DOTYPE_NUM] = {0};
			CT_INT32 iLen = sizeof(vistorInfo);
			pUser->m_userSeek.GetCurrentVistorInfo(vistorInfo,iLen);
			if (vistorInfo[0].seekInfo.iType != 0)
			{
				SeekData sd = {0};
				CCUser* pFUser = 0;
				pUser->m_userSeek.GetSeekData(sd);
				if (CCUserManager::getSingleton().Find(sd.dwSeekID, &pFUser))
				{
					for (CT_INT32 i = 0;i < iLen / sizeof(VistorInfo);++i)
					{
						pFUser->m_userSeek.AddVistorInfo(vistorInfo[i]);

						// ���Ϳͻ���
						CMD_G_UpdateVisitorInfo uvi = {0};
						uvi.vi = vistorInfo[i];
						uvi.vi.seekInfo.iType %= 100;
						pUser->m_userSeek.GetVisitorDes(uvi.vi, uvi.cInfo, sizeof(uvi.cInfo));
						if (strlen(uvi.cInfo) != 0)
						{
							pFUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_FRIEND_RECORD_UPDATE, (CT_CHAR*)&uvi, sizeof(uvi));
						}

						CMD_G_VisitorTotalInfo vti = {0};
						pFUser->m_userSeek.GetTotalInfo(vti.tDayTotal, sizeof(vti.tDayTotal), vti.iDayNum, CT_TRUE);
						pFUser->m_userSeek.GetTotalInfo(vti.wDayTotal, sizeof(vti.wDayTotal), vti.iWeekNum, CT_FALSE);
						pFUser->GetNetInterface()->Send(MAIN_G_SEEK, SUB_G_SEEK_UPDATE_STATISTIC, (CT_CHAR*)&vti, sizeof(vti));
					}
				}
				pUser->m_userSeek.SeekVistor();
			}

			pUser->UserExitEvent(time(0));
			CCFightTeamManager::getSingleton().UserExitEvent(pUser);	// ����ս�ӳ�Ա����
			CCResourceGrabMgr::getSingleton().UserExit(pUser->GetUserID()); //������Դ��������
			CCTowerResManager::getSingleton().DeleteUserEnterMap(pUser->GetUserID()); //�û��˳���
		}

		MSC_DB_UserExit userExit = {0};
		userExit.dwUserID = pUser->GetUserID();
		userExit.llTimeNow = currTime;
		G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_USER_EXIT, 0, (CT_CHAR*)&userExit, sizeof(userExit));

		//���ݼ�¼
		MSC_LOGDB_UserExit LogDBUserExit={0};
		LogDBUserExit.dwUserID=pUser->GetUserID();
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_USEREXIT,(CT_CHAR*)&LogDBUserExit,sizeof(LogDBUserExit));

		CCUserManager::getSingleton().Erase(pUser->GetUserID()); //ɾ���û�
		CCUserEnterManager::getSingleton().SetExitTime(pUser->GetUserID());
		//CCUserEnterManager::getSingleton().Erase(pUser->GetUserID());//ɾ����֤��(��ʱ��ɾ��)
		pUser->CloseNet();
		pUser->Clear();
	}
	return true;
}