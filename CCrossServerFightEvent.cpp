#include "CCrossServerFightEvent.h"
#include "CLogMeg.h"
#include "CMD5.h"
#include "CUserManager.h"
#include "CCrossServerFight.h"
#include "CCSFightData.h"
#include "CFightTeamManager.h"
#include "CMD_CrossServer.h"
#include "CActive.h"
#include "CTop.h"
#include "CBroadcastScriptSetting.h"
#include "CFightingData.h"

#define	CS_MAX_BUFF			100*1024
CCCrossServerFightEvent::CCCrossServerFightEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_SCHEDULE_REVIEW_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_REWARD_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_LOOK_ACTIVE);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_SUPPORT_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_POP_TOP_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_READY_INFO_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_INSPIRE_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_BEFOR_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_FIGHT_APPLY);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_UPDATE_USER_DATA);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_LOOK_FIGHT_RECORD);
	RegistMessage(MAIN_G_CROSS_SERVER,SUB_CS_CHANGE_FT_POS);

	RegistMessage(MAIN_CS_GAME,SUB_CS_SYN_CURRENT_FINAL_EIGHT);
	RegistMessage(MAIN_CS_GAME,SUB_CS_SYN_LAST_FINAL_EIGHT);
	RegistMessage(MAIN_CS_GAME,SUB_CS_REGISTER_FINISH);
	RegistMessage(MAIN_CS_GAME,SUB_CS_RETURN_STATUS);
	RegistMessage(MAIN_CS_GAME,SUB_CS_SEAT_FIGHT_TEAM);
	RegistMessage(MAIN_CS_GAME,SUB_CS_UPDATE_USER_DATA);
	RegistMessage(MAIN_CS_GAME,SUB_CS_SEAT_ENDING_RETURN);
	RegistMessage(MAIN_CS_GAME,SUB_CS_SYN_FIGHT_RECORD);
	RegistMessage(MAIN_CS_GAME,SUB_CS_RECORD_USER);

	RegistMessage(MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_ACTIVE_BEGIN);
	RegistMessage(MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_FIGHTING);
	RegistMessage(MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_FINAL_EIGHT);
	RegistMessage(MSC_MAIN_CORSS_FIGHT,MSC_SUB_CS_ACTIVE_FIGHTING);

	if(m_dwThread == GAME_THREAD_CITY)
	{
		SetTimer(IDI_CS_FIGHITN_DATA,60*1000);
		RegistTimer(IDI_CS_FIGHITN_DATA);
	}

	//test
#ifdef _WIN32
	if(m_dwThread == GAME_THREAD_CITY)
	{
		RegistTimer(IDI_CS_FIGHITN_DATA+1);
		RegistTimer(IDI_CS_FIGHITN_DATA+2);
		//SetTimer(IDI_CS_FIGHITN_DATA+1,100*1000);
	}
#endif
	
}

CCCrossServerFightEvent::~CCCrossServerFightEvent(void)
{
}

//处理用户消息
CT_VOID CCCrossServerFightEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	if(MAIN_G_CROSS_SERVER != mainID && MAIN_CS_GAME != mainID && MSC_MAIN_CORSS_FIGHT != mainID)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::OnUserEvent MAIN_G_CROSS_SERVER ！= mainID mainID:%d",mainID);
		return;
	}

	if(MAIN_G_CROSS_SERVER == mainID || MAIN_CS_GAME == mainID)
	{
		OnNetUserEvent(subID,pBuf,len);
	}
	else if(MSC_MAIN_CORSS_FIGHT == mainID)
	{
		OnMscUserEvent(subID,pBuf,len);
	}


	return;
}

//用户定时器处理
CT_VOID CCCrossServerFightEvent::OnTime(CT_DWORD timeID)
{
	if(IDI_CS_FIGHITN_DATA == timeID)
	{
		if(CCTop::getSingleton().GetInit())
		{
			KillTimer(timeID);
			//跨服战更新阵位数据
			CT_CHAR cBuf[CS_MAX_BUFF]={0};
			CT_INT32 iBuflen=CS_MAX_BUFF;
			if(CCCSFightData::getSingleton().GetFinalEightData(Current_Record,cBuf,iBuflen))
			{
				for(CT_INT32 i=0;i<iBuflen/sizeof(CCCrossServerFight::FinalEightData);++i)
				{
					CCCrossServerFight::FinalEightData fed={0};
					memcpy(&fed,cBuf+i*sizeof(fed),sizeof(fed));

					CCFightTeam *pFT=0;
					if(CCFightTeamManager::getSingleton().Find(fed.fe.FightTeamInfo.dwFTID,&pFT) &&
						fed.fe.iGameServerID == ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID)
					{
						if(CCCSFightData::getSingleton().GetStatus() >= Final_Eight && Seat_Fight <= CCCSFightData::getSingleton().GetStatus())
						{
							CMD_CS_ChangeFTPos cscftp={0};
							cscftp.dwFTID=fed.fe.FightTeamInfo.dwFTID;
							cscftp.iGSID=fed.fe.iGameServerID;
							if(pFT->GetUserFightData(cscftp.ufs,CLAN_CRUSADE_MEMBER))
							{
								for(CT_INT32 j=0;j<CLAN_CRUSADE_MEMBER;++j)
								{
									CSInspore csi={0};
									pFT->GetInspore(cscftp.ufs[j].dwUserID,csi);
									AddInsport(cscftp.ufs[j],csi);
								}
								g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_CHANGE_FT_POS,(CT_CHAR*)&cscftp,sizeof(cscftp));
							}
						}
					}
				}
			}
		}
	}
	else if(IDI_CS_FIGHITN_DATA+1 == timeID)
	{
		KillTimer(timeID);
		DisposeActiveBegin(0,0);
		SetTimer(IDI_CS_FIGHITN_DATA+2,60*1000);
	}
	else if(IDI_CS_FIGHITN_DATA+2 == timeID)
	{
		DisposeActiveBegin(0,0);
	}
	return;
}

//线程消息
CT_VOID CCCrossServerFightEvent::OnMscUserEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case MSC_SUB_CS_ACTIVE_BEGIN:
		{
			if(CCCSFightData::getSingleton().GetStatus() == CS_Close)
				DisposeActiveBegin(pBuf,len);
			break;
		}
	case MSC_SUB_CS_FIGHTING:
		{
			DisposeCSFightingEnd(pBuf,len);
			break;
		}
	case MSC_SUB_CS_FINAL_EIGHT:
		{
			DisposeCSFinelEightFightingEnd(pBuf,len);
			break;
		}
	case MSC_SUB_CS_ACTIVE_FIGHTING:
		{
			DisposeActiveBegin(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::OnMscUserEvent subID:%d",subID);
			return;
		}
	}
	return;
}

//网络消息
CT_VOID CCCrossServerFightEvent::OnNetUserEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case SUB_G_SCHEDULE_REVIEW_APPLY:
		{
			DisposeScheduleReviewApply(pBuf,len);
			break;
		}
	case SUB_G_CS_REWARD_APPLY:
		{
			DisposeRewardApply(pBuf,len);
			break;
		}
	case SUB_G_CS_LOOK_ACTIVE:
		{
			DisposeCSLookActive(pBuf,len);
			break;
		}
	case SUB_G_CS_SUPPORT_APPLY:
		{
			DisposeCSSupportApply(pBuf,len);
			break;
		}
	case SUB_G_CS_POP_TOP_APPLY:
		{
			DisposeCSPopTopApply(pBuf,len);
			break;
		}
	case SUB_G_CS_READY_INFO_APPLY:
		{
			DisposeCSReadyInfoApply(pBuf,len);
			break;
		}
	case SUB_G_CS_INSPIRE_APPLY:
		{
			DisposeCSInsporeApply(pBuf,len);
			break;
		}
	case SUB_G_CS_SEAT_BEFOR_APPLY:
		{
			DisposeSeatBeforApply(pBuf,len);
			break;
		}
	case SUB_CS_SYN_CURRENT_FINAL_EIGHT:
		{
			DisposeCurrentFinalEight(pBuf,len);
			break;
		}
	case SUB_CS_SYN_LAST_FINAL_EIGHT:
		{
			DisposeLastFinalEight(pBuf,len);
			break;
		}
	case SUB_CS_RETURN_STATUS:
		{
			DisposeReturnStatus(pBuf,len);
			break;
		}
	case SUB_CS_REGISTER_FINISH:
		{
			break;
		}
	case SUB_CS_SEAT_FIGHT_TEAM:
		{
			DisposeSeatFightTeam(pBuf,len);
			break;
		}
	case SUB_G_CS_SEAT_FIGHT_APPLY:
		{
			DisposeSeatFighting(pBuf,len,MSC_SUB_CS_FIGHTING);
			break;
		}
	case SUB_CS_SEAT_ENDING_RETURN:
		{
			DisposeSeatEndingReturn(pBuf,len);
			break;
		}
	case SUB_G_CS_UPDATE_USER_DATA:
		{
			DipsoseUpdateUserData(pBuf,len);
			break;
		}
	case SUB_CS_UPDATE_USER_DATA:
		{
			DisposeCSUpdateUserData(pBuf,len);
			break;
		}
	case SUB_G_CS_LOOK_FIGHT_RECORD:
		{
			DisposeCSLookFightRecord(pBuf,len);
			break;
		}
	case SUB_CS_CHANGE_FT_POS:
		{
			DisposeCSChangeFTPos(pBuf,len);
			break;
		}
	case SUB_CS_SYN_FIGHT_RECORD:
		{
			DisposeCSSynFightRecord(pBuf,len);
			break;
		}
	case SUB_CS_RECORD_USER:
		{
			DisposeRecordUser(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::OnNetUserEvent subID:%d",subID);
			return;
		}
	}
	return;
}

//赛程回顾
CT_VOID CCCrossServerFightEvent::DisposeScheduleReviewApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSScheduleReviewApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeScheduleReviewApply sizeof(CMD_G_CSScheduleReviewApply) != iLen");
		return;
	}

	CMD_G_CSScheduleReviewApply *pCSSra=(CMD_G_CSScheduleReviewApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSSra->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeScheduleReviewApply No Find UserID:%d",pCSSra->dwUserID);
		return;
	}

	CMD_G_CSFinalEight csef={0};
	const CT_INT32 iTempLen=sizeof(CCCrossServerFight::FinalEightData)*CROSS_SERVER_FINAL_EIGHT;
	CT_INT32 iTempLen1=iTempLen;
	CT_CHAR cBuf[iTempLen]={0};
	CCCSFightData::getSingleton().GetFinalEightData(Last_Rcord,cBuf,iTempLen1);
	for(CT_INT32 i=0;i<iTempLen1/sizeof(CCCrossServerFight::FinalEightData);++i)
	{
		CCCrossServerFight::FinalEightData fed={0};
		memcpy(&fed,cBuf+sizeof(CCCrossServerFight::FinalEightData)*i,sizeof(FinalEight));

		CT_INT32 iIndex=fed.fe.iPos%CROSS_SERVER_FINAL_EIGHT;
		fed.fe.iPos=fed.fe.iPos+1;//位置纠正
		memcpy(&csef.fe[iIndex],&fed,sizeof(FinalEight));
	}

	for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT_RECORD;++i)
	{
		sprintf(csef.fri[i].cIndex,"%d",Last_Index+i);
	}

	for(CT_INT32 i=0;i<Final_One;++i)
	{
		
		CCCSFightData::CSUserSupport css={0};
		CCCSFightData::getSingleton().GetSupport(pCSSra->dwUserID,i,css);

		csef.dwFTID[i]=css.dwFTID;
		csef.iGameServerID[i]=css.iGSID;
	}

	pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_SCHEDULE_REVIEW,(CT_CHAR*)&csef,sizeof(csef));
	return;
}

//领奖请求	
CT_VOID CCCrossServerFightEvent::DisposeRewardApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSRewardApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeRewardApply sizeof(CMD_G_CSRewardApply) != iLen");
		return;
	}

	CMD_G_CSRewardApply *pCSRa=(CMD_G_CSRewardApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSRa->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeRewardApply No Find UserID:%d",pCSRa->dwUserID);
		return;
	}

	CMD_G_CSRewardFail csrf={0};
	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	CT_CHAR cSendBuf[CS_MAX_BUFF]={0};
	CT_INT32 iSendBufLen=0;
	CT_INT32 iGoods[GOODS_COUNT]={0};
	CT_INT32 iCount[GOODS_COUNT]={0};
	CT_INT32 iNum=0;
	std::list<MSC_LOGDB_CS_REWARD> logrewardlist;
	if(CCCSFightData::getSingleton().GetUserReward(pCSRa->dwUserID,cBuf,iBufLen))
	{
		for(CT_INT32 i=0;i<iBufLen/sizeof(CCCSFightData::CSUserReward);++i)
		{
			MasterVariableData mvd={0};
			MSC_LOGDB_CS_REWARD csreward={0};
			CCCSFightData::CSUserReward csru={0};
			memcpy(&csru,cBuf+i*sizeof(csru),sizeof(csru));

			CT_INT32 iResNum=csru.iNum;
			if(csru.iType == CLIENT_OBJ_ITEM)
			{
				iGoods[iNum]=csru.iResID;
				iCount[iNum]=csru.iNum;
				if(iNum < GOODS_COUNT)
					iNum++;
			}

			csreward.dwUserID=pCSRa->dwUserID;
			csreward.iResID=csru.iResID;
			csreward.iNum=iResNum;
			csreward.iResType=csru.iType;
			logrewardlist.push_back(csreward);
		}

		if(pUser->LootItems(SAVATAG_CS_REWARD, iGoods,iCount,iNum,ITEM_OP_MODE_CHECK) != LOOT_SUC)
		{
			csrf.iStatus=REWARD_BEG;
			goto FAIL;
		}
		else
		{
			for(CT_INT32 i=0;i<iBufLen/sizeof(CCCSFightData::CSUserReward);++i)
			{
				MasterVariableData mvd={0};
				CCCSFightData::CSUserReward csru={0};
				memcpy(&csru,cBuf+i*sizeof(csru),sizeof(csru));

				CT_INT32 iResNum=csru.iNum;
				if(csru.iType == CLIENT_OBJ_RES)
				{
					G_ChangeResType(mvd,csru.iResID,iResNum);
					pUser->m_userBase.AddMastData(mvd,SAVATAG_CS_REWARD);

					csru.iNum=0;
					memset(&mvd,0,sizeof(mvd));
					pUser->m_userBase.GetMasterData(mvd);
					G_ChangeResType(mvd,csru.iResID,csru.iNum);

					memcpy(cSendBuf+iSendBufLen,&csru,sizeof(csru));
					iSendBufLen+=sizeof(csru);
				}
			}

			pUser->LootItems(SAVATAG_CS_REWARD, iGoods,iCount,iNum,ITEM_OP_MODE_EXEC);
			pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_REWARD_INFO,cSendBuf,iSendBufLen);

			CCCSFightData::getSingleton().ClearReward(pUser->GetUserID());
			//日志记录
			for(std::list<MSC_LOGDB_CS_REWARD>::iterator it=logrewardlist.begin();it != logrewardlist.end();++it)
			{
				CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
				G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CSREWARD,(CT_CHAR*)&(*it),sizeof((*it)));
			}
		}

		return;
	}
	else
	{
		csrf.iStatus=REWARD_DATA;
	}
FAIL:
	pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_REWARD_FAIL,(CT_CHAR*)&csrf,sizeof(csrf));

	return;
}
//查看活动	
CT_VOID CCCrossServerFightEvent::DisposeCSLookActive(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSLookActive) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSLookActive sizeof(CMD_G_CSLookActive) != iLen");
		return;
	}

	CMD_G_CSLookActive *pCsla=(CMD_G_CSLookActive*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCsla->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSLookActive No Find UserID:%d",pCsla->dwUserID);
		return;
	}

	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pCsla->dwUserID);
	//if(dwFTID == 0)
	//{
	//	CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSLookActive FTID:%d",dwFTID);
	//	return;
	//}

	CT_DWORD dwStatus=CCCSFightData::getSingleton().GetStatus();
	switch(dwStatus)
	{
	case Final_Eight:
	case Final_Four:
	case Final_Two:
	case Final_One:
	case CS_Close:
		{
			CMD_G_CSFinalEight csfe={0};
			CT_CHAR cBuf[CS_MAX_BUFF]={0};
			CT_INT32 iBufLen=CS_MAX_BUFF;
			if(CCCSFightData::getSingleton().GetFinalEightData(Current_Record,cBuf,iBufLen))
			{
				for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::FinalEightData) && i< CROSS_SERVER_FINAL_EIGHT;++i)
				{
					CCCrossServerFight::FinalEightData fed={0};
					memcpy(&fed,cBuf+i*sizeof(CCCrossServerFight::FinalEightData),sizeof(CCCrossServerFight::FinalEightData));
					memcpy(&csfe.fe[fed.fe.iPos%CROSS_SERVER_FINAL_EIGHT],&fed.fe,sizeof(fed.fe));
				}
			}

			//对应客户端POS
			for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT;++i)
			{
				csfe.fe[i].iPos=i+1;
			}

			for(CT_INT32 i=Final_Eight;i<Final_One;++i)
			{
				CCCSFightData::CSUserSupport csus={0};
				if(CCCSFightData::getSingleton().GetSupport(pCsla->dwUserID,i+1,csus))
				{
					csfe.iGameServerID[i]=csus.iGSID;
					csfe.dwFTID[i]=csus.dwFTID;
				}
			}

			for(CT_INT32 i=0;i<Last_Index;++i)
			{
				sprintf(csfe.fri[i].cIndex,"%d",i+1);
			}

			iBufLen=CS_MAX_BUFF;
			memset(cBuf,0,sizeof(cBuf));
			if(CCCSFightData::getSingleton().GetCSFightRecord(dwFTID,((CCGameConfig&)CCConfig::getSingleton()).m_iGSID,cBuf,iBufLen))
			{
				for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::CSFightRecord) && i < CROSS_SERVER_FIGHT_RECORD;++i)
				{
					CCCrossServerFight::CSFightRecord csfr={0};
					
					memcpy(&csfr,cBuf+i*sizeof(csfr),sizeof(csfr));
					memcpy(csfe.csfr[i].iGameServerID,csfr.iGameServerID,sizeof(csfr.iGameServerID));
					memcpy(csfe.csfr[i].cIndex,csfr.cIndex,CROSS_SERVER_RECORD_INDEX);
					memcpy(csfe.csfr[i].cFightTeamName1,csfr.cFightTeamName[0],NAME_SIZE);
					memcpy(csfe.csfr[i].cFightTeamName2,csfr.cFightTeamName[1],NAME_SIZE);
					csfe.csfr[i].iWiner=csfr.iWiner;
				}
			}
			pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_FINAL_EIGHT,(CT_CHAR*)&csfe,sizeof(csfe));

			break;
		}
	case Seat_Fight:
	case Seat_Fight_Befor:
		{
			CMD_G_CSSeatFightInfo cssfi={0};
			CT_CHAR cBuf[CS_MAX_BUFF]={0};
			CT_INT32 iBufLen=CS_MAX_BUFF;

			cssfi.lTime=CCCSFightData::getSingleton().GetFTCoolTime(dwFTID);
			cssfi.iQualification=CCCSFightData::getSingleton().GetSeat(dwFTID);

			if(CCCSFightData::getSingleton().GetFinalEightData(Current_Record,cBuf,iBufLen))
			{
				for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::FinalEightData) && i< CROSS_SERVER_FINAL_EIGHT;++i)
				{
					CCCrossServerFight::FinalEightData fed={0};
					memcpy(&fed,cBuf+i*sizeof(CCCrossServerFight::FinalEightData),sizeof(CCCrossServerFight::FinalEightData));

					CT_INT32 iPos=fed.fe.iPos%CROSS_SERVER_FINAL_EIGHT;
					if(fed.fe.FightTeamInfo.dwFTID == 0)
					{
						cssfi.iPos[iPos]=Pos_Null;
					}
					else if(fed.fe.FightTeamInfo.dwFTID == dwFTID && dwFTID != 0 && fed.fe.iGameServerID == ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID)
					{
						cssfi.iPos[iPos]=Pos_Me;
					}
					else
					{
						cssfi.iPos[iPos]=Pos_Other;
					}
				}
			}

			iBufLen=CS_MAX_BUFF;
			memset(cBuf,0,sizeof(cBuf));
			if(CCCSFightData::getSingleton().GetCSFightRecord(dwFTID,((CCGameConfig&)CCConfig::getSingleton()).m_iGSID,cBuf,iBufLen))
			{
				for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::CSFightRecord) || i< CROSS_SERVER_FIGHT_RECORD;++i)
				{
					CCCrossServerFight::CSFightRecord csfr={0};
					
					memcpy(&csfr,cBuf+i*sizeof(CCCrossServerFight::CSFightRecord),sizeof(CCCrossServerFight::CSFightRecord));
					memcpy(cssfi.csfr[i].cIndex,csfr.cIndex,CROSS_SERVER_RECORD_INDEX);
					memcpy(cssfi.csfr[i].cFightTeamName1,csfr.cFightTeamName[0],NAME_SIZE);
					memcpy(cssfi.csfr[i].cFightTeamName2,csfr.cFightTeamName[1],NAME_SIZE);
					memcpy(cssfi.csfr[i].iGameServerID,csfr.iGameServerID,sizeof(csfr.iGameServerID));
					cssfi.csfr[i].iWiner=csfr.iWiner;
				}
			}

			pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_FIGHT_INFO,(CT_CHAR*)&cssfi,sizeof(cssfi));
			break;
		}
	//case Seat_Fight_Befor:
	//	{
	//		CT_CHAR cBuf[CS_MAX_BUFF]={0};
	//		CT_INT32 iBufLen=CS_MAX_BUFF;
	//		CCCSFightData::getSingleton().GetSeatFightTeam(cBuf,iBufLen);
	//		pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_FIGHT_READY,(CT_CHAR*)&cBuf,iBufLen);
	//		break;
	//	}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSLookActive status:%d",CCCSFightData::getSingleton().GetStatus());
			break;
		}
	}
	return;
}
//支持请求	
CT_VOID CCCrossServerFightEvent::DisposeCSSupportApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSSupportApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSSupportApply sizeof(CMD_G_CSSupportApply) != iLen");
		return;
	}

	CMD_G_CSSupportApply *pCSSa=(CMD_G_CSSupportApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSSa->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSSupportApply No Find UserID:%d",pCSSa->dwUserID);
		return;
	}
	CT_INT32 iTimeAEID = 0;
	CT_INT32 iActiveStatus = 0;
	CT_INT64 iTime = 0;
	CCActive::getSingleton().GetTimeActiveStatus(ACTIVE_CS_SUPPORT, iTimeAEID, iTime, iActiveStatus);
	if (1 != iActiveStatus)
	{
		CCLogMeg::getSingleton().WriteError("CCActiveEvent::DisposeCSSupportApply ACTIVE_CS_SUPPORT 1 != iActiveStatus iActiveStatus：%d",iActiveStatus);
		return;
	}

	G_CSFightBet csb={0};
	CT_INT32 iStatus=SupportVaidate(pUser,*pCSSa,csb);
	if(iStatus != 0)
	{
		CMD_G_CSSupportFail cssf={0};
		cssf.iStatus=iStatus;
		pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_SUPPORT_FAIL,(CT_CHAR*)&cssf,sizeof(cssf));
		return;
	}

	CCCSFightData::CSUserSupport css={0};
	css.dwUserID=pCSSa->dwUserID;
	css.dwFTID=pCSSa->dwFTID;
	css.iGSID=pCSSa->iGameServerID;
	css.iResID=csb.iResID;
	css.iNum=abs(csb.iNum);
	css.iIndex=pCSSa->iIndex;
	CCCSFightData::getSingleton().InsertSupport(css);
	
	
	//扣除资源
	MasterVariableData mvd={0};
	CT_INT32 iTempNum=csb.iNum;
	G_ChangeResType(mvd,csb.iResID,iTempNum);
	pUser->m_userBase.AddMastData(mvd,SAVATAG_CS_SUPPORT);

	iTempNum=0;
	memset(&mvd,0,sizeof(mvd));
	pUser->m_userBase.GetMasterData(mvd);
	G_ChangeResType(mvd,csb.iResID,iTempNum);

	CMD_G_CSSupportSuc csss={0};
	csss.iGameServerID=pCSSa->iGameServerID;
	csss.dwFTID=pCSSa->dwFTID;
	csss.iResID=csb.iResID;
	csss.iNum=iTempNum;
	pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_SUPPORT_SUC,(CT_CHAR*)&csss,sizeof(csss));

	//日志记录
	MSC_LOGDB_CS_SUPPORT logcss={0};
	logcss.dwUserID=pUser->GetUserID();
	logcss.iResID=csb.iResID;
	logcss.iNum=csb.iNum;
	logcss.dwFTID=pCSSa->dwFTID;
	CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_SUPPORT,(CT_CHAR*)&logcss,sizeof(logcss));

	//通知跨服（无需验证哈哈哈）
	return;
}
//人气排行请求	
CT_VOID CCCrossServerFightEvent::DisposeCSPopTopApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSPopTopApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSPopTopApply sizeof(CMD_G_CSPopTopApply) != iLen");
		return;
	}
	CMD_G_CSPopTopApply *pCSPta=(CMD_G_CSPopTopApply*)pBuf;
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSPta->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSPopTopApply No Find UserID:%d",pCSPta->dwUserID);
		return;
	}

	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	std::multimap<CT_INT32,PopTopInfo> topmap;
	if(CCCSFightData::getSingleton().GetFinalEightData(Current_Record,cBuf,iBufLen))
	{
		for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::FinalEightData);++i)
		{
			CCCrossServerFight::FinalEightData fed={0};
			PopTopInfo Pti={0};
			CCCrossServerFight::CSSupport css={0};
			
			memcpy(&fed,cBuf+i*sizeof(CCCrossServerFight::FinalEightData),sizeof(CCCrossServerFight::FinalEightData));
			CCCSFightData::getSingleton().GetCSSupportData(pCSPta->iStatus,fed.fe.iPos,css);
			
			Pti.dwFTID=fed.fe.FightTeamInfo.dwFTID;
			Pti.iGameServerID=fed.fe.iGameServerID;
			Pti.iNum=css.iNum;
			Pti.iStatus=css.iStatus;
			memcpy(Pti.FightTeamName,fed.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);

			topmap.insert(std::map<CT_INT32,PopTopInfo>::value_type(Pti.iNum,Pti));
		}
	}

	CT_CHAR cSendBuf[CS_MAX_BUFF]={0};
	CT_INT32 iSendLen=0;
	std::multimap<CT_INT32,PopTopInfo>::iterator itend=topmap.end();
	for(std::multimap<CT_INT32,PopTopInfo>::iterator it=topmap.begin();it != itend;++it)
	{
		memcpy(cSendBuf+iSendLen,&((*it).second),sizeof(PopTopInfo));
		iSendLen+=sizeof(PopTopInfo);
	}
	pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_POP_TOP_INFO,(CT_CHAR*)&cSendBuf,iSendLen);
	return;
}
//准备信息请求	
CT_VOID CCCrossServerFightEvent::DisposeCSReadyInfoApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSReadyInfoApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSReadyInfoApply sizeof(CMD_G_CSReadyInfoApply) != iLen");
		return;
	}
	CMD_G_CSReadyInfoApply *pCSria=(CMD_G_CSReadyInfoApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSria->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSReadyInfoApply No Find UserID:%d",pCSria->dwUserID);
		return;
	}

	CMD_G_CSReadyInfo csri={0};
	CT_INT32 iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pCSria->dwUserID);
	CT_INT32 iDstPos=CCCSFightData::getSingleton().GetOpponentPos(dwFTID,iGSID,CCCSFightData::getSingleton().GetStatus()+1);
	CCCrossServerFight::FinalEightData fed={0};
	CCCSFightData::getSingleton().GetFTInfo(dwFTID,iGSID,fed);

	for(CT_INT32 i=0;i<CLAN_CRUSADE_MEMBER;++i)
	{
		csri.FightNumber[i].dwUserID=fed.dwUserID[i];
		csri.FightNumber[i].iGameServerID=fed.fe.iGameServerID;

		if(csri.FightNumber[i].dwUserID == pCSria->dwUserID)
		{
			csri.iPersonPower=fed.csi[i].iPowerID;
			csri.iPersonPowerGrade=fed.csi[i].iGrade;
			csri.iPersonPowerExp=fed.csi[i].iExp;
		}

		memcpy(csri.FightNumber[i].cFTName,fed.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
		memcpy(csri.FightNumber[i].cUserName,fed.cUserName[i],NAME_SIZE);
	}

	memset(&fed,0,sizeof(fed));
	CCCSFightData::getSingleton().GetFinalEightData(Current_Record,iDstPos,fed);
	for(CT_INT32 i=0;i<CLAN_CRUSADE_MEMBER;++i)
	{
		CT_INT32 iIndex=(CLAN_CRUSADE_MEMBER+i)%CLAN_CRUSADE_MEMBER+CLAN_CRUSADE_MEMBER;
		csri.FightNumber[iIndex].dwUserID=fed.dwUserID[i];
		csri.FightNumber[iIndex].iGameServerID=fed.fe.iGameServerID;
		memcpy(csri.FightNumber[iIndex].cFTName,fed.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
		memcpy(csri.FightNumber[iIndex].cUserName,fed.cUserName[i],NAME_SIZE);

	}

	pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_READY_INFO,(CT_CHAR*)&csri,sizeof(csri));

	
	return;
}

//资格赛前准备请求
CT_VOID CCCrossServerFightEvent::DisposeSeatBeforApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSSeatBeforApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatBeforApply sizeof(CMD_G_CSSeatBeforApply) != iLen");
		return;
	}

	CMD_G_CSSeatBeforApply *pCSSba=(CMD_G_CSSeatBeforApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSSba->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatBeforApply No Find UserID:%d",pCSSba->dwUserID);
		return;
	}

	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	if(CCCSFightData::getSingleton().GetSeatFightTeam(cBuf,iBufLen))
	{
		pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_FIGHT_READY,cBuf,iBufLen);
	}

	return;
}

//鼓舞请求	
CT_VOID CCCrossServerFightEvent::DisposeCSInsporeApply(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSInsporeApply) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSInsporeApply sizeof(CMD_G_CSInsporeApply) != iLen");
		return;
	}

	CMD_G_CSInsporeApply *pCSIa=(CMD_G_CSInsporeApply*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSIa->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSInsporeApply No Find UserID:%d",pCSIa->dwUserID);
		return;
	}

	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pUser->GetUserID());
	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSInsporeApply No Find FT:%d",pUser->GetUserID());
		return;
	}


	CMD_G_NormalReward nr={0};
	CT_INT32 iStatus=InsporeValidate(pUser,pFT,pCSIa->iPersonPowerID,nr);
	if(iStatus != 0)
	{
		CMD_G_CSInspireFail csif={0};
		csif.iErrorStatus=iStatus;
		pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_INSPIRE_FAIL,(CT_CHAR*)&csif,sizeof(csif));
		return;
	}

	CSInspore csi={0};
	CMD_G_CSInspireInfo csii={0};
	pFT->GetInspore(pUser->GetUserID(),csi);
	csii.reward=nr;
	csii.iPersonPowerID=csi.iPowerID;
	csii.iGrade=csi.iGrade;
	csii.iExp=csi.iExp;
	pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_INSPIRE_INFO,(CT_CHAR*)&csii,sizeof(csii));
	return;
}

//同步当前8强
CT_VOID CCCrossServerFightEvent::DisposeCurrentFinalEight(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen%sizeof(CCCrossServerFight::FinalEightData) != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCurrentFinalEight iLen / sizeof(CCCrossServerFight::FinalEightData) != 0");
		return;
	}

	for(CT_INT32 i=0;i<iLen/sizeof(CCCrossServerFight::FinalEightData);++i)
	{
		CCCrossServerFight::FinalEightData fed={0};
		memcpy(&fed,pBuf+i*sizeof(fed),sizeof(fed));
		CCCSFightData::getSingleton().InsertFinalEightData(Current_Record,fed);
	}

	//同步战斗数据到跨服
	if(m_dwThread == GAME_THREAD_CITY)
	{
		SetTimer(IDI_CS_FIGHITN_DATA,60*1000);
	}
	return;
}

//同步上届8强
CT_VOID CCCrossServerFightEvent::DisposeLastFinalEight(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen%sizeof(CCCrossServerFight::FinalEightData) != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeLastFinalEight iLen / sizeof(CCCrossServerFight::FinalEightData) != 0");
		return;
	}

	for(CT_INT32 i=0;i<iLen/sizeof(CCCrossServerFight::FinalEightData);++i)
	{
		CCCrossServerFight::FinalEightData fed={0};
		memcpy(&fed,pBuf+i*sizeof(fed),sizeof(fed));
		CCCSFightData::getSingleton().InsertFinalEightData(Last_Rcord,fed);
	}
	return;
}

//返回状态
CT_VOID CCCrossServerFightEvent::DisposeReturnStatus(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_CS_ReturnStatus) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeReturnStatus  sizeof(CMD_CS_ReturnStatus) != iLen");
		return;
	}

	CMD_CS_ReturnStatus *pRs=(CMD_CS_ReturnStatus*)pBuf;
	CCCSFightData::getSingleton().SetStatus(pRs->iStatus);
	if(CCCSFightData::getSingleton().GetSeat() == 0)
		CCCSFightData::getSingleton().InsertSeat(pRs->iSeat);

	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;

	CMD_G_CrossBaseInfo cbi={0};
	cbi.iStatus=pRs->iStatus;
	memcpy(cBuf,&cbi,sizeof(cbi));

	CCUserManager::getSingleton().SendCSStatusInfo(cBuf,iBufLen);

	//发送资格赛战队
	if(pRs->iStatus == Seat_Fight_Befor)
	{
		NewMatch();

		memset(cBuf,0,sizeof(cBuf));
		iBufLen=CS_MAX_BUFF;
		CCCSFightData::getSingleton().InsertSeat(pRs->iSeat);
		if(CCCSFightData::getSingleton().GetSeatFightTeam(cBuf,iBufLen))
		{
			g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_SEAT_FIGHT_TEAM,cBuf,iBufLen);
		}

		CCLogMeg::getSingleton().WriteError("*****new Match!!!!");
	}
	else if(pRs->iStatus > Final_Eight && Final_One >= pRs->iStatus)
	{
		FinalEightFight(pRs->iSeed);

		//奖励计算
		if(Final_One == pRs->iStatus)
		{
			CCUserManager::getSingleton().SendUserMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_ACTIVE_END);
			printf("****Final_One == pRs->iStatus\n");
		}
	}

	printf("*****Status:%d\n",pRs->iStatus);
	return;
}

//资格赛战队
CT_VOID CCCrossServerFightEvent::DisposeSeatFightTeam(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen%sizeof(CSSeatFightReady) != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerEvent::CCCrossServerFightEvent iLen/sizeof(CSSeatFightReady) != 0");
		return;
	}

	for(CT_INT32 i=0;i<iLen/sizeof(CSSeatFightReady);++i)
	{
		CSSeatFightReady cssfr={0};
		memcpy(&cssfr,pBuf+i*sizeof(CSSeatFightReady),sizeof(CSSeatFightReady));
		CCCSFightData::getSingleton().InsertSeatFightTeam(cssfr);
	}

	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	if(CCCSFightData::getSingleton().GetSeatFightTeam(cBuf,iBufLen))
		CCUserManager::getSingleton().SendUserMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_FIGHT_READY,cBuf,iBufLen);
	return;
}

//资格战斗
CT_VOID CCCrossServerFightEvent::DisposeSeatFighting(CT_CHAR *pBuf,CT_INT32 iLen,CT_DWORD dwSubID)
{
	if(sizeof(CMD_G_CSSeatFighting) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatFighting sizeof(CMD_G_CSSeatFighting) != iLen");
		return;
	}

	CMD_G_CSSeatFighting *pCSSf=(CMD_G_CSSeatFighting*)pBuf;

	CCFightTeam* pFightTeam = 0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pCSSf->dwUserID);
	if(!CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatFighting 没有发现战队 %i", dwFTID);
		return ;
	}

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSSf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatFighting No Find UserID:%d",pCSSf->dwUserID);
		return;
	}

	CMD_G_CSSeatFightReturn cssfr={0};

	//冷却时间
	G_ValueRange vr={0};
	if(G_GetValueRange(VALUE_TYPE_CS_COOL_TIME,&vr))
	{
		cssfr.lCoolTime=CCCSFightData::getSingleton().GetFTCoolTime(dwFTID);
		if(cssfr.lCoolTime > 0)
		{
			cssfr.iStatus=SEAT_FIGHT_COOL_TIME;
		}
		else
		{
			cssfr.lCoolTime=vr.fMaxVal;
		}
	}
	else
	{
		cssfr.iStatus=SEAT_FIGHT_DATA;
	}

	//位置验证
	CT_INT32 iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
	CCCrossServerFight::FinalEightData fed={0};
	if(CCCSFightData::getSingleton().GetFTInfo(dwFTID,iGSID,fed))
	{
		cssfr.iStatus=SEAT_FIGHT_POS;
	}

	//队长验证
	CMD_G_FightTeamBase ftb={0};
	pFightTeam->GetFightTeamBase(&ftb);
	if(ftb.dwBossUID != pCSSf->dwUserID)
	{
		cssfr.iStatus=SEAT_FIGHT_CAPTAIN;
	}

	//资格验证
	if(CCCSFightData::getSingleton().GetSeat(dwFTID) == 0)
	{
		cssfr.iStatus=SEAT_FIGHT_SEAT;
	}


	pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_FIGHT_RETURN,(CT_CHAR*)&cssfr,sizeof(cssfr));
	if(cssfr.iStatus != 0)
		return;


	MSC_CS_FIGHTING_DATA csf={0};
	csf.dwUserID=pCSSf->dwUserID;
	csf.iPos[0]=abs(pCSSf->iSeatID-1);
	csf.iSeed=rand();
	csf.bLookLeft=CT_TRUE;

	memset(&fed,0,sizeof(fed));
	if(CCCSFightData::getSingleton().GetFinalEightData(Current_Record,csf.iPos[0],fed))
	{
		if(!pFightTeam->GetUserFightData(csf.LeftFightingData,  SPOT_GUARD_MEMBER,CT_TRUE))
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatFighting pve没有发现战队成员的战斗数据 fightTeam=%i",dwFTID );
			return;
		}

		//威能加成
		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			CSInspore csi={0};
			pFightTeam->GetInspore(csf.LeftFightingData[i].dwUserID,csi);
			//AddInsport(csf.LeftFightingData[i],csi);
		}

		memcpy(csf.RightFightingData,fed.ufs,sizeof(fed.ufs));

		CT_DWORD dwThreadID=GAME_THREAD_FIGHT+pCSSf->dwFTID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		SendThreadInfo(dwThreadID,MSC_MAIN_FIGHTING_EVENT,dwSubID,(CT_CHAR*)&csf,sizeof(csf));

	}
	else
	{
		csf.iWiner=WINER_LIFT;

		if(!pFightTeam->GetUserFightData(csf.LeftFightingData,  SPOT_GUARD_MEMBER,CT_TRUE))
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatFighting pve没有发现战队成员的战斗数据 fightTeam=%i",dwFTID );
			return;
		}
		DisposeCSFightingEnd((CT_CHAR*)&csf,sizeof(csf));
	}

	time_t t=time(0);
	CCCSFightData::getSingleton().InsertCoolTime(dwFTID,t);

	//删除BOSS
	DeleteBoss(csf.iPos[0]);

	return;
}

//战斗结果返回
CT_VOID CCCrossServerFightEvent::DisposeSeatEndingReturn(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_CSSeatEnding) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatEndingReturn sizeof(CMD_CSSeatEnding) != iLen");
		return;
	}

	CMD_CSSeatEnding *pCSSe=(CMD_CSSeatEnding*)pBuf;

	CCCrossServerFight::FinalEightData fed={0};
	CMD_G_CSFightRecord csfe={0};
	CT_CHAR cBuf[CROSS_SERVER_RECORD_INDEX+1];

	CCCSFightData::getSingleton().GetFinalEightData(Current_Record,pCSSe->fe.iPos,fed);
	csfe.iWiner=pCSSe->fe.iWiner;
	csfe.iGameServerID[1]=fed.fe.iGameServerID;
	csfe.iGameServerID[0]=pCSSe->fe.iGameServerID;
	memcpy(csfe.cFightTeamName2,fed.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(csfe.cFightTeamName1,pCSSe->fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	sprintf(cBuf,"%i",time(0));
	CCMD5::getSingleton().Encrypt((CT_UCHAR*)csfe.cIndex,CROSS_SERVER_RECORD_INDEX,(CT_UCHAR*)cBuf,CROSS_SERVER_RECORD_INDEX);


	//本服战队广播
	CCFightTeam* pFightTeam = 0;
	if(csfe.iGameServerID[1] == ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID)
	{
		if(!CCFightTeamManager::getSingleton().Find(fed.fe.FightTeamInfo.dwFTID, &pFightTeam))
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatEndingReturn 没有发现战队 %i", fed.fe.FightTeamInfo.dwFTID);
		}
		else
			pFightTeam->Broadcast(MAIN_G_CROSS_SERVER,SUB_G_CS_FIGHT_RECORD,(CT_CHAR*)&csfe,sizeof(csfe));
	}
	if(csfe.iGameServerID[0] == ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID)
	{
		if(!CCFightTeamManager::getSingleton().Find(pCSSe->fe.FightTeamInfo.dwFTID, &pFightTeam))
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeSeatEndingReturn 没有发现战队 %i", pCSSe->fe.FightTeamInfo.dwFTID);
		}
		else
			pFightTeam->Broadcast(MAIN_G_CROSS_SERVER,SUB_G_CS_FIGHT_RECORD,(CT_CHAR*)&csfe,sizeof(csfe));
	}

	//保存战报
	CCCrossServerFight::CSFightRecord csfr={0};

	csfr.iSeed=pCSSe->iSeed;
	csfr.iWiner=pCSSe->fe.iWiner;
	csfr.iGameServerID[0]=pCSSe->fe.iGameServerID;
	csfr.iGameServerID[1]=fed.fe.iGameServerID;
	csfr.dwFTID[0]=pCSSe->fe.FightTeamInfo.dwFTID;
	csfr.dwFTID[1]=fed.fe.FightTeamInfo.dwFTID;
	memcpy(csfr.cFightTeamName[0],pCSSe->fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(csfr.cFightTeamName[1],fed.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(csfr.ufs[0],pCSSe->ufs,sizeof(pCSSe->ufs));
	memcpy(csfr.ufs[1],fed.ufs,sizeof(pCSSe->ufs));
	memcpy(csfr.cIndex,csfe.cIndex,CROSS_SERVER_RECORD_INDEX);

	CCCSFightData::getSingleton().InsertCSFightRecord(Current_Record,csfr);


	CCCrossServerFight::FinalEightData tempfed={0};
	if(csfe.iWiner == WINER_LIFT && pCSSe->fe.FightTeamInfo.dwFTID != 0 &&
		!CCCSFightData::getSingleton().GetFTInfo(pCSSe->fe.FightTeamInfo.dwFTID,pCSSe->fe.iGameServerID,tempfed))
	{
		//插入记录
		memset(&fed,0,sizeof(fed));
		memcpy(&fed.fe,&pCSSe->fe,sizeof(fed.fe));
		memcpy(fed.ufs,pCSSe->ufs,sizeof(fed.ufs));
		memcpy(fed.cUserName,pCSSe->cUserName,sizeof(fed.cUserName));
		memcpy(fed.dwUserID,pCSSe->dwUserID,sizeof(fed.dwUserID));
		memcpy(fed.csi,pCSSe->csi,sizeof(fed.csi));
		fed.fe.iWiner=WINER_NULL;//资格赛位置胜利信息可忽略
		CCCSFightData::getSingleton().InsertFinalEightData(Current_Record,fed);
		printf("name:%s\n",fed.cUserName[0]);

		//同步位置
		CT_CHAR cBuf[CS_MAX_BUFF];
		CT_INT32 iBufLen=CS_MAX_BUFF;
		if(CCCSFightData::getSingleton().GetFinalEightData(Current_Record,cBuf,iBufLen))
		{
			CMD_G_CSSeatFightSynPos cssfs={0};
			for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::FinalEightData) && i < CROSS_SERVER_FINAL_EIGHT;++i)
			{
				CCCrossServerFight::FinalEightData fed={0};
				memcpy(&fed,cBuf+i*sizeof(fed),sizeof(fed));

				CT_INT32 iPos=fed.fe.iPos%CROSS_SERVER_FINAL_EIGHT;
				cssfs.dwFTID[iPos]=fed.fe.FightTeamInfo.dwFTID;
				cssfs.iGSID[iPos]=fed.fe.iGameServerID;
			}
			CCUserManager::getSingleton().SendUserMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_SEAT_FIGHT_SYN_POS,(CT_CHAR*)&cssfs,sizeof(cssfs));
		}
	}

	return;
}

//同步用户数据
CT_VOID CCCrossServerFightEvent::DipsoseUpdateUserData(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSUpdateUserData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DipsoseUpdateUserData sizeof(CMD_G_CSUpdateUserData) != iLen");
		return;
	}

	CMD_G_CSUpdateUserData *pCSUD=(CMD_G_CSUpdateUserData*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCSUD->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DipsoseUpdateUserData No Find UserID:%d",pCSUD->dwUserID);
		return;
	}

	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pCSUD->dwUserID);
	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DipsoseUpdateUserData No Find FTID:%d",dwFTID);
		return;
	}

	CSInspore csi={0};
	pFT->GetInspore(pCSUD->dwUserID,csi);

	UserFightingStatus UserData={0};
	G_GetFightingData(pUser,UserData);

	//8强内生效
	if(CCCSFightData::getSingleton().GetStatus() >= Final_Eight && CCCSFightData::getSingleton().GetStatus() <= Final_One)
		AddInsport(UserData,csi);

	CMD_CS_UpdateUserData uud={0};
	uud.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
	uud.ufs=UserData;
	uud.csi=csi;
	uud.bSystem=CT_TRUE;
	g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_UPDATE_USER_DATA,(CT_CHAR*)&uud,sizeof(uud));
	return;
}

//跨服返回同步用户数据
CT_VOID CCCrossServerFightEvent::DisposeCSUpdateUserData(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_CS_UpdateUserData) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSUpdateUserData sizeof(CMD_CS_UpdateUserData) != iLen");
		return;
	}

	CMD_CS_UpdateUserData *pUud=(CMD_CS_UpdateUserData*)pBuf;

	CCCSFightData::getSingleton().UpdateUserData(pUud->iGSID,pUud->ufs,pUud->csi);

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pUud->ufs.dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSUpdateUserData No Find UserID:%d",pUud->ufs.dwUserID);
		return;
	}

	if(pUud->bSystem)
		pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_UPDATE_USER_SUC);
}

//查看战报
CT_VOID CCCrossServerFightEvent::DisposeCSLookFightRecord(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_CSLookFightRecord) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSLookFightRecord sizeof(CMD_G_CSLookFightRecord) != iLen");
		return;
	}

	CMD_G_CSLookFightRecord *pCslfr=(CMD_G_CSLookFightRecord*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCslfr->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSLookFightRecord No Find UserID:%d",pCslfr->dwUserID);
		return;
	}

	CCCrossServerFight::CSFightRecord csfr={0};
	if(!CCCSFightData::getSingleton().GetCSFightRecord(pCslfr->cIndex,CROSS_SERVER_RECORD_INDEX,csfr))
	{
		pUser->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_LOOK_FIGHT_FAIL);
		return;
	}

	MSC_CS_FIGHTING_DATA csf={0};
	csf.dwUserID=pCslfr->dwUserID;
	csf.iSeed=csfr.iSeed;
	csf.bLookLeft=CT_TRUE;
	memcpy(csf.LeftFightingData,csfr.ufs[0],sizeof(csf.LeftFightingData));
	memcpy(csf.RightFightingData,csfr.ufs[1],sizeof(csf.RightFightingData));

	CT_DWORD dwThreadID=GAME_THREAD_FIGHT+pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	SendThreadInfo(dwThreadID,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FIGHT_RECORD,(CT_CHAR*)&csf,sizeof(csf));
	return;
}

//战队阵法同步
CT_VOID CCCrossServerFightEvent::DisposeCSChangeFTPos(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_CS_ChangeFTPos) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSChangeFTPos sizeof(CMD_CS_ChangeFTPos) != iLen");
		return;
	}

	CMD_CS_ChangeFTPos *pCSCftp=(CMD_CS_ChangeFTPos*)pBuf;

	CCCSFightData::getSingleton().UpdateFTPos(pCSCftp->dwFTID,pCSCftp->iGSID,pCSCftp->ufs);
	return;
}

//同步战报
CT_VOID CCCrossServerFightEvent::DisposeCSSynFightRecord(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_CS_RecordBase))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSSynFightRecord iLen != sizeof(CCCrossServerFight::CSFightRecord)");
		return;
	}
	CMD_CS_RecordBase *pRb=(CMD_CS_RecordBase*)pBuf;

	CCCrossServerFight::CSFightRecord csfr={0};
	csfr.iWiner=pRb->iWiner;
	csfr.iSeed=pRb->iSeed;
	memcpy(csfr.cFightTeamName,pRb->cFightTeamName,sizeof(csfr.cFightTeamName));
	memcpy(csfr.cIndex,pRb->cIndex,CROSS_SERVER_RECORD_INDEX);
	memcpy(csfr.iGameServerID,pRb->iGameServerID,sizeof(csfr.iGameServerID));
	memcpy(csfr.dwFTID,pRb->dwFTID,sizeof(csfr.dwFTID));

	CCCSFightData::getSingleton().InsertCSFightRecord(Last_Rcord,csfr);
	return;
}

//同步战报用户
CT_VOID CCCrossServerFightEvent::DisposeRecordUser(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_CS_RecordUser) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeRecordUser sizeof(CMD_CS_RecordUser) != iLen");
		return;
	}

	CMD_CS_RecordUser *pCSur=(CMD_CS_RecordUser*)pBuf;
	CCCSFightData::getSingleton().InsertCSFightRecordUser(pCSur->cIndex,pCSur->iGroup,pCSur->iPos,pCSur->ufs);
	return;
}

//活动开启
CT_VOID CCCrossServerFightEvent::DisposeActiveBegin(CT_CHAR *pBuf,CT_INT32 iLen)
{
	CMD_CS_ActiveStatus as={0};
	as.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
	as.iStatus=CCCSFightData::getSingleton().GetStatus();
	g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_ACTIVE_STATUS,(CT_CHAR*)&as,sizeof(as));

	printf("active status:%d\n",CCCSFightData::getSingleton().GetStatus());
	return;
}

//战斗结果
CT_VOID CCCrossServerFightEvent::DisposeCSFightingEnd(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_CS_FIGHTING_DATA) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSFightingEnd sizeof(MSC_CS_FIGHTING_DATA) != iLen");
		return;
	}

	MSC_CS_FIGHTING_DATA *pCSfd=(MSC_CS_FIGHTING_DATA*)pBuf;

	CMD_CSSeatEnding csse={0};
	CCCrossServerFight::FinalEightData fed={0};
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pCSfd->dwUserID);
	//CCCSFightData::getSingleton().GetFTInfo(pCSfd->dwUserID,fed);
	CCFightTeam* pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSFightingEnd 没有发现战队 %i", dwFTID);
		return ;
	}
	
	pFightTeam->GetFightTeamInfo(&csse.fe.FightTeamInfo);
	csse.fe.iWiner=pCSfd->iWiner;
	csse.fe.iPos=pCSfd->iPos[0];
	csse.fe.iGameServerID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
	csse.iSeed=pCSfd->iSeed;
	memcpy(csse.ufs,pCSfd->LeftFightingData,sizeof(csse.ufs));

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		CombatUnitData cud={0};
		G_GetMasterFightingData(pCSfd->LeftFightingData[i],cud);
		memcpy(csse.cUserName[i],cud.name,NAME_SIZE);
		csse.dwUserID[i]=pCSfd->LeftFightingData[i].dwUserID;

		pFightTeam->GetInspore(pCSfd->LeftFightingData[i].dwUserID,csse.csi[i]);
	}

	g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_SEAT_ENDING,(CT_CHAR*)&csse,sizeof(csse));
	
	return;
}

//8强战斗结果
CT_VOID CCCrossServerFightEvent::DisposeCSFinelEightFightingEnd(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_CS_FIGHTING_DATA) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::DisposeCSFinelEightFightingEnd sizeof(MSC_CS_FIGHTING_DATA) != iLen");
		return;
	}

	MSC_CS_FIGHTING_DATA *pCSfd=(MSC_CS_FIGHTING_DATA*)pBuf;

	CCCrossServerFight::CSFightRecord csfr={0};
	CCCrossServerFight::FinalEightData fed1={0};
	CCCrossServerFight::FinalEightData fed2={0};
	CCCSFightData::getSingleton().GetFinalEightData(Current_Record,pCSfd->iPos[0],fed1);
	CCCSFightData::getSingleton().GetFinalEightData(Current_Record,pCSfd->iPos[1],fed2);

	csfr.iSeed=pCSfd->iSeed;
	csfr.iWiner=CCCSFightData::getSingleton().GetStatus()*10+(pCSfd->iWiner%10 == WINER_LIFT?CS_WIN:CS_LOST);
	csfr.dwFTID[0]=fed1.fe.FightTeamInfo.dwFTID;
	csfr.dwFTID[1]=fed2.fe.FightTeamInfo.dwFTID;
	csfr.iGameServerID[0]=fed1.fe.iGameServerID;
	csfr.iGameServerID[1]=fed2.fe.iGameServerID;
	memcpy(csfr.cFightTeamName[0],fed1.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(csfr.cFightTeamName[1],fed2.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(csfr.ufs[0],pCSfd->LeftFightingData,sizeof(pCSfd->LeftFightingData));
	memcpy(csfr.ufs[1],pCSfd->RightFightingData,sizeof(pCSfd->RightFightingData));
	FinalEightFightChangeIndex(CCCSFightData::getSingleton().GetStatus(),pCSfd->iPos[0],csfr.cIndex);
	CCCSFightData::getSingleton().InsertCSFinalEightRecord(csfr);

	//发送到跨服
	CMD_CS_RecordBase rb={0};
	rb.iSeed=pCSfd->iSeed;
	rb.iWiner=csfr.iWiner;
	rb.dwFTID[0]=fed1.fe.FightTeamInfo.dwFTID;
	rb.dwFTID[1]=fed2.fe.FightTeamInfo.dwFTID;
	rb.iGameServerID[0]=fed1.fe.iGameServerID;
	rb.iGameServerID[1]=fed2.fe.iGameServerID;
	memcpy(rb.cFightTeamName[0],fed1.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(rb.cFightTeamName[1],fed2.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(rb.cIndex,csfr.cIndex,CROSS_SERVER_RECORD_INDEX);
	g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_FINAL_EIGHT_ENDING,(CT_CHAR*)&rb,sizeof(rb));

	for(CT_INT32 i=0;i<2;++i)
	{
		for(CT_INT32 j=0;j<CLAN_CRUSADE_MEMBER;++j)
		{
			if(fed1.ufs[j].dwUserID == 0)continue;
			CMD_CS_RecordUser ru={0};
			ru.iGroup=i;
			ru.iPos=j;
			ru.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
			if(i == 0)
				memcpy(&ru.ufs,&fed1.ufs[j],sizeof(ru.ufs));
			else
				memcpy(&ru.ufs,&fed2.ufs[j],sizeof(ru.ufs));
			memcpy(ru.cIndex,rb.cIndex,CROSS_SERVER_RECORD_INDEX);
			g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_FINAL_EIGHT_ENDING_USER,(CT_CHAR*)&ru,sizeof(ru));
		}
	}

	//删除BOSS
	DeleteBoss(pCSfd->iPos[0]);
	DeleteBoss(pCSfd->iPos[1]);

	//更新位置胜利
	fed1.fe.iWiner=CCCSFightData::getSingleton().GetStatus()*10+(pCSfd->iWiner%10 == WINER_LIFT?CS_WIN:CS_LOST);
	fed2.fe.iWiner=CCCSFightData::getSingleton().GetStatus()*10+(pCSfd->iWiner%10 == WINER_LIFT?CS_LOST:CS_WIN);
	fed1.fe.iPos=pCSfd->iPos[0];
	fed2.fe.iPos=pCSfd->iPos[1];
	CCCSFightData::getSingleton().InsertFinalEightData(Current_Record,fed1);
	CCCSFightData::getSingleton().InsertFinalEightData(Current_Record,fed2);
	CCLogMeg::getSingleton().WriteError("*****index:%s Final Eight pos[0]:%d win:%d pos[1]:%d win:%d",csfr.cIndex,pCSfd->iPos[0],fed1.fe.iWiner,pCSfd->iPos[1],fed2.fe.iWiner);

	CMD_G_CSFightRecord csfe={0};
	csfe.iWiner=CCCSFightData::getSingleton().GetStatus()*10+(pCSfd->iWiner%10 == WINER_LIFT?CS_WIN:CS_LOST);
	csfe.iGameServerID[0]=fed1.fe.iGameServerID;
	csfe.iGameServerID[1]=fed2.fe.iGameServerID;
	csfe.iPos[0]=pCSfd->iPos[0]+1;
	csfe.iPos[1]=pCSfd->iPos[1]+1;
	memcpy(csfe.cFightTeamName1,fed1.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(csfe.cFightTeamName2,fed2.fe.FightTeamInfo.cFightTeamName,NAME_SIZE);
	memcpy(csfe.cIndex,csfr.cIndex,CROSS_SERVER_RECORD_INDEX);
	printf("sendPos[0]:%d sendpos[1]:%d\n",csfe.iPos[0],csfe.iPos[1]);

	//广播战报
	CCUserManager::getSingleton().SendUserMessage(MAIN_G_CROSS_SERVER,SUB_G_CS_FIGHT_RECORD,(CT_CHAR*)&csfe,sizeof(csfe));

	//支持奖励
	if(CCCSFightData::getSingleton().GetStatus() > Final_Eight && Final_One >= CCCSFightData::getSingleton().GetStatus())
	{
		CCCSFightData::getSingleton().SupportReward();
	}
	
	//8强奖励
	if(CCCSFightData::getSingleton().GetStatus() == Final_One)
	{
		FullServerReward();
		FinalEightReward();
		ChampionBroadcast();
		CCCSFightData::getSingleton().NewMatch();
	}

	CCUserManager::getSingleton().SendCSRewardInfo();
	return;
}

CT_INT32 CCCrossServerFightEvent::SupportVaidate(CCUser *pUser,CMD_G_CSSupportApply &CSSa,G_CSFightBet &csb)
{
	MasterVariableData mvd={0};
	CT_INT32 iTempNum=0;
	CT_INT32 iStatus=CCCSFightData::getSingleton().GetStatus()+1;

	pUser->m_userBase.GetMasterData(mvd);
	if(!G_GetCSFightBet(iStatus,CSSa.iIndex,&csb))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::SupportVaidate G_GetCSFightBet iStatus:%d iIndex:%d",iStatus,CSSa.iIndex);
		return SUPPORT_ERROR_DATA;
	}

	CCCrossServerFight::FinalEightData fed={0};
	CCCSFightData::getSingleton().GetFTInfo(CSSa.dwFTID,CSSa.iGameServerID,fed);
	if((fed.fe.iWiner/10 != CCCSFightData::getSingleton().GetStatus() || fed.fe.iWiner%10 != CS_WIN) && fed.fe.iWiner != 0)
		return SUPPORT_ERROR_TEAM;

	//资源验证
	iTempNum=csb.iNum;
	G_ChangeResType(mvd,csb.iResID,iTempNum);
	if(iTempNum < 0)
		return SUPPORT_ERROR_RES;

	//支持验证
	CCCSFightData::CSUserSupport css={0};
	if(CCCSFightData::getSingleton().GetSupport(pUser->GetUserID(),iStatus,css))
		return SUPPORT_ERROR_NO_NUM;

	//奖励领取
	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	if(CCCSFightData::getSingleton().GetUserReward(pUser->GetUserID(),cBuf,iBufLen))
	{
		if(iBufLen != 0)
			return SUPPORT_ERROR_REWARD;
	}
	else
		return SUPPORT_ERROR_DATA;

	return 0;
}
//8强战斗
CT_VOID CCCrossServerFightEvent::FinalEightFight(CT_INT32 iSeed)
{

	CT_INT32 iStatus=CCCSFightData::getSingleton().GetStatus();
	MSC_CS_FIGHTING_DATA csf={0};
	csf.iSeed=iSeed;
	csf.bLookLeft=CT_TRUE;
	switch(iStatus)
	{
	case Final_Four:
		{
			for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT;i+=2)
			{
				CCCrossServerFight::FinalEightData fed1={0};
				CCCrossServerFight::FinalEightData fed2={0};
				CCCSFightData::getSingleton().GetFinalEightData(Current_Record,i,fed1);
				CCCSFightData::getSingleton().GetFinalEightData(Current_Record,i+1,fed2);

				csf.iPos[0]=i;
				csf.iPos[1]=i+1;
				if((fed1.fe.FightTeamInfo.dwFTID == 0 && fed2.fe.FightTeamInfo.dwFTID == 0) ||
					(fed1.fe.FightTeamInfo.dwFTID != 0 && fed2.fe.FightTeamInfo.dwFTID == 0))
				{
					//生成假战报
					csf.iWiner=WINER_LIFT;
					DisposeCSFinelEightFightingEnd((CT_CHAR*)&csf,sizeof(csf));
				}
				else if(fed1.fe.FightTeamInfo.dwFTID == 0 && fed2.fe.FightTeamInfo.dwFTID != 0)
				{
					//生成假战报
					csf.iWiner=WINER_RIGHT;
					DisposeCSFinelEightFightingEnd((CT_CHAR*)&csf,sizeof(csf));
				}
				else
				{
					//真战报
					memcpy(csf.LeftFightingData,fed1.ufs,sizeof(csf.LeftFightingData));
					memcpy(csf.RightFightingData,fed2.ufs,sizeof(csf.RightFightingData));
					for(CT_INT32 j=0;j<TEAM_NUM;++j)
					{
						AddInsport(csf.LeftFightingData[j],fed1.csi[j]);
						AddInsport(csf.RightFightingData[j],fed2.csi[j]);
					}
					SendThreadInfo(GAME_THREAD_FIGHT,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FINAL_EIGHT,(CT_CHAR*)&csf,sizeof(csf));
				}
			}
			break;
		}
	case Final_Two:
		{
			CT_INT32 iNum=0;
			CT_INT32 iPos[2]={-1,-1};
			for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT;++i)
			{
				CCCrossServerFight::FinalEightData fed1={0};
				CCCSFightData::getSingleton().GetFinalEightData(Current_Record,i,fed1);
				if(fed1.fe.FightTeamInfo.dwFTID != 0 && fed1.fe.iWiner%10 == CS_WIN)
				{
					iNum=(i < (CROSS_SERVER_FINAL_EIGHT/2)?0:1);
					i=(i <( CROSS_SERVER_FINAL_EIGHT/2)?CROSS_SERVER_FINAL_EIGHT/2:CROSS_SERVER_FINAL_EIGHT);

					iPos[iNum]=fed1.fe.iPos;
				}
			}
			for(CT_INT32 i=0;i<2;++i)
			{
				CCCrossServerFight::FinalEightData fed1={0};
				CCCrossServerFight::FinalEightData fed2={0};
				CCCSFightData::getSingleton().GetFinalEightData(Current_Record,iPos[i],fed1);
				CT_INT32 iGSID=fed1.fe.iGameServerID;
				CT_INT32 iOpponentPos=CCCSFightData::getSingleton().GetOpponentPos(fed1.fe.FightTeamInfo.dwFTID,iGSID,iStatus);
				CCCSFightData::getSingleton().GetFinalEightData(Current_Record,iOpponentPos,fed2);
				printf("iOpponentPos :%d dwFTID:%d GSID:%d\n",iOpponentPos,fed2.fe.FightTeamInfo.dwFTID,fed2.fe.iGameServerID);

				if(fed2.fe.FightTeamInfo.dwFTID == 0)
				{
					//生成假战报
					csf.iPos[0]=iPos[i];
					if(csf.iPos[0] == -1 && i == 0)
						csf.iPos[0]=0;
					else if(csf.iPos[0] == -1 && i == 1)
						csf.iPos[0]=4;

					if(csf.iPos[0] == 0 || csf.iPos[0] == 1)
						csf.iPos[1]=2;
					else if(csf.iPos[0] == 2 || csf.iPos[0] == 3)
						csf.iPos[1]=0;
					else if(csf.iPos[0] == 4 || csf.iPos[0] == 5)
						csf.iPos[1]=6;
					else if(csf.iPos[0] == 6 || csf.iPos[0] == 7)
						csf.iPos[1]=4;
					csf.iWiner=WINER_LIFT;
					DisposeCSFinelEightFightingEnd((CT_CHAR*)&csf,sizeof(csf));
				}
				else
				{
					//真战报
					csf.iPos[0]=fed1.fe.iPos;
					csf.iPos[1]=fed2.fe.iPos;
					memcpy(csf.LeftFightingData,fed1.ufs,sizeof(csf.LeftFightingData));
					memcpy(csf.RightFightingData,fed2.ufs,sizeof(csf.RightFightingData));
					for(CT_INT32 j=0;j<TEAM_NUM;++j)
					{
						AddInsport(csf.LeftFightingData[j],fed1.csi[j]);
						AddInsport(csf.RightFightingData[j],fed2.csi[j]);
					}
					SendThreadInfo(GAME_THREAD_FIGHT,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FINAL_EIGHT,(CT_CHAR*)&csf,sizeof(csf));
				}
			}
			break;
		}
	case Final_One:
		{
			for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT;++i)
			{
				CCCrossServerFight::FinalEightData fed1={0};
				CCCrossServerFight::FinalEightData fed2={0};
				CCCSFightData::getSingleton().GetFinalEightData(Current_Record,i,fed1);
				CT_INT32 iGSID=fed1.fe.iGameServerID;

				if(fed1.fe.FightTeamInfo.dwFTID == 0 || fed1.fe.iWiner/10 != Final_Two || fed1.fe.iWiner%10 == CS_LOST)
					continue;

				CT_INT32 iOpponentPos=CCCSFightData::getSingleton().GetOpponentPos(fed1.fe.FightTeamInfo.dwFTID,iGSID,iStatus);
				CCCSFightData::getSingleton().GetFinalEightData(Current_Record,iOpponentPos,fed2);

				csf.iPos[0]=i;
				if(fed2.fe.FightTeamInfo.dwFTID == 0)
				{
					//生成假战报
					if(i < CROSS_SERVER_FINAL_EIGHT/2)
						csf.iPos[1]=4;
					else
						csf.iPos[1]=0;
					csf.iWiner=WINER_LIFT;
					DisposeCSFinelEightFightingEnd((CT_CHAR*)&csf,sizeof(csf));
				}
				else
				{
					//真战报
					csf.iPos[1]=fed2.fe.iPos;
					memcpy(csf.LeftFightingData,fed1.ufs,sizeof(csf.LeftFightingData));
					memcpy(csf.RightFightingData,fed2.ufs,sizeof(csf.RightFightingData));
					for(CT_INT32 j=0;j<TEAM_NUM;++j)
					{
						AddInsport(csf.LeftFightingData[j],fed1.csi[j]);
						AddInsport(csf.RightFightingData[j],fed2.csi[j]);
					}
					SendThreadInfo(GAME_THREAD_FIGHT,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FINAL_EIGHT,(CT_CHAR*)&csf,sizeof(csf));
				}
				break;
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::FinalEightFight iStatus:%d",iStatus);
			return;
		}
	}
	return;
}

//8强战报索引转换
CT_VOID CCCrossServerFightEvent::FinalEightFightChangeIndex(CT_INT32 iStatus,CT_INT32 iPos,CT_CHAR *pIndex)
{
	switch(iStatus)
	{
	case Final_Four:
		{
			if(iPos < 2)
			{
				sprintf(pIndex,"1");
			}
			else if(iPos >= 2 && iPos < 4)
			{
				sprintf(pIndex,"2");
			}
			else if(iPos >= 4 && iPos < 6)
			{
				sprintf(pIndex,"3");
			}
			else if(iPos >= 6 && iPos < 8)
			{
				sprintf(pIndex,"4");
			}
			break;
		}
	case Final_Two:
		{
			if(iPos < 4)
			{
				sprintf(pIndex,"5");
			}
			else if(iPos >= 4 && iPos < 8)
			{
				sprintf(pIndex,"6");
			}
			break;
		}
	case Final_One:
		{
			sprintf(pIndex,"7");
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::FinalEightFightChangeIndex iStatus:%d",iStatus);
			return;
		}
	}
	return;
}

//鼓舞资源验证
CT_INT32 CCCrossServerFightEvent::InsporeValidate(CCUser *pUser,CCFightTeam *pFT,CT_INT32 iID,CMD_G_NormalReward &nr)
{
	CSInspore csi={0};
	if(!pFT->GetInspore(pUser->GetUserID(),csi))
	{
		csi.iPowerID=iID;
		csi.dwUserID=pUser->GetUserID();
		csi.iGrade=0;
		pFT->UpGradeInspore(pUser->GetUserID(),csi);
	}

	if(csi.iPowerID != iID)
	{
		return INSPORE_ID;
	}

	G_CSFightInspire csfi={0};
	if(!G_GetCSFightInspire(csi.iPowerID,csi.iGrade+1,&csfi))
	{
		return INSPORE_DATA;
	}

	CMD_G_NormalReward addReward={0};
	MSC_LOGDB_CS_INSPORECOST logcsic={0};
	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT]={0};
	CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT]={0};
	FakeProBase fpb={0};
	CT_INT32 iStatus=G_PerformCostReward(csfi.iCostID,pUser,ENUM_PCRT_TEST,addReward,iGoods,iCount,fpb);

	//扣除资源
	if(iStatus == ENUM_CREC_RES_SUC)
	{
		MasterVariableData mvd={0};
		logcsic.iResID[0]=addReward.iResType0;
		logcsic.iResID[1]=addReward.iResType1;
		logcsic.iResID[2]=addReward.iResType2;
		logcsic.iResID[3]=addReward.iResType3;
		logcsic.iResNum[0]=addReward.iResNum0;
		logcsic.iResNum[1]=addReward.iResNum1;
		logcsic.iResNum[2]=addReward.iResNum2;
		logcsic.iResNum[3]=addReward.iResNum3;

		G_ChangeResType(mvd,addReward.iResType0,addReward.iResNum0);
		G_ChangeResType(mvd,addReward.iResType1,addReward.iResNum1);
		G_ChangeResType(mvd,addReward.iResType2,addReward.iResNum2);
		G_ChangeResType(mvd,addReward.iResType3,addReward.iResNum3);

		pUser->m_userBase.AddMastData(mvd,SAVATAG_CS_INSPORE);

		memset(&mvd,0,sizeof(mvd));
		pUser->m_userBase.GetMasterData(mvd);
		nr.iResType0=addReward.iResType0;
		nr.iResType1=addReward.iResType1;
		nr.iResType2=addReward.iResType2;
		nr.iResType3=addReward.iResType3;
		G_ChangeResType(mvd,addReward.iResType0,nr.iResNum0);
		G_ChangeResType(mvd,addReward.iResType1,nr.iResNum1);
		G_ChangeResType(mvd,addReward.iResType2,nr.iResNum2);
		G_ChangeResType(mvd,addReward.iResType3,nr.iResNum3);

		CT_INT32 iType=0;
		for(CT_INT32 i=0;i<GOODS_COUNT*2 + TEAM_ITEM_CNT;++i)
		{
			if(iGoods[i] != 0)
				iType++;
		}

		pUser->LootItems(SAVATAG_CS_INSPORE, iGoods, iCount, iType, ITEM_OP_MODE_EXEC);

		pFT->UpGradeInspore(pUser->GetUserID(),csi);

		//日志
		logcsic.dwUserID=pUser->GetUserID();
		logcsic.iID=iID;
		logcsic.iGrade=csi.iGrade;
		logcsic.iExp=csi.iExp;
		logcsic.iItemID[0]=iGoods[0];
		logcsic.iItemID[1]=iGoods[1];
		logcsic.iItemNum[0]=iCount[0];
		logcsic.iItemNum[1]=iCount[1];
		CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE+pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_INSPORE_COST,(CT_CHAR*)&logcsic,sizeof(logcsic));
	}



	return iStatus;
}

//8强奖励
CT_VOID CCCrossServerFightEvent::FinalEightReward()
{
	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	if(!CCCSFightData::getSingleton().GetFinalEightData(Current_Record,cBuf,iBufLen))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::FinalEightReward GetFinalEightData Current_Record");
		return;
	}

	std::map<CT_INT32,CT_INT32> CSFTMap;
	for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::FinalEightData);++i)
	{
		CCCrossServerFight::FinalEightData fed={0};
		memcpy(&fed,cBuf+i*sizeof(fed),sizeof(fed));

		if(fed.fe.FightTeamInfo.dwFTID == 0 ||
			fed.fe.iGameServerID != ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID)
			continue;

		CSFTMap.insert(std::map<CT_INT32,CT_INT32>::value_type(fed.fe.FightTeamInfo.dwFTID,fed.fe.iGameServerID));

		CCFightTeam *pFT=0;
		if(!CCFightTeamManager::getSingleton().Find(fed.fe.FightTeamInfo.dwFTID,&pFT))
			continue;

		FightTeamMemberUID FTUserID={0};
		pFT->GetFightTeamMemberUID(&FTUserID);

		for(CT_INT32 j=0;j<CLAN_MAX_MEMBER;++j)
		{
			if(FTUserID.dwMemberUID[j] == 0)
				continue;
			CCCSFightData::getSingleton().FinalEightReward(fed.fe.iWiner,FTUserID.dwMemberUID[j]);
		}
	}

	//未入围奖励
	memset(cBuf,0,sizeof(cBuf));
	iBufLen=CS_MAX_BUFF;
	CCCSFightData::getSingleton().GetSeatFightTeam(cBuf,iBufLen);
	for(CT_INT32 i=0;i<iBufLen/sizeof(CSSeatFightReady);++i)
	{
		CSSeatFightReady cssfr={0};
		std::map<CT_INT32,CT_INT32>::iterator it;

		memcpy(&cssfr,cBuf+i*sizeof(CSSeatFightReady),sizeof(CSSeatFightReady));
		it=CSFTMap.find(cssfr.dwFTID);
		if(cssfr.iGameServerID == ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID && it == CSFTMap.end())
		{
			CCFightTeam *pFT=0;
			if(!CCFightTeamManager::getSingleton().Find(cssfr.dwFTID,&pFT))
				continue;

			FightTeamMemberUID FTUserID={0};
			pFT->GetFightTeamMemberUID(&FTUserID);

			for(CT_INT32 j=0;j<CLAN_MAX_MEMBER;++j)
			{
				if(FTUserID.dwMemberUID[j] == 0)
					continue;
				CCCSFightData::getSingleton().FinalEightReward(WINER_RIGHT,FTUserID.dwMemberUID[j]);
			}
		}
	}
	return;
}

//全服奖励
CT_VOID CCCrossServerFightEvent::FullServerReward()
{
	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iBufLen=0;
	std::map<CT_INT32,CT_INT32> Statusmap;
	CT_INT32 iStatus=-1;
	for(CT_INT32 i=0;i<CROSS_SERVER_FINAL_EIGHT;++i)
	{
		CCCrossServerFight::FinalEightData fed={0};
		if(CCCSFightData::getSingleton().GetFinalEightData(Current_Record,i,fed))
		{
			if(fed.fe.iGameServerID == ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID)
			{
				if(fed.fe.iWiner%10 == WINER_RIGHT)
					Statusmap.insert(std::map<CT_INT32,CT_INT32>::value_type((fed.fe.iWiner/10)-1,(fed.fe.iWiner/10)-1));
				else
					Statusmap.insert(std::map<CT_INT32,CT_INT32>::value_type(fed.fe.iWiner/10,fed.fe.iWiner/10));

				if(fed.fe.iWiner/10 >= Final_Four && fed.fe.iWiner/10 <= Final_One)
				{
					CCFightTeam *pFT=0;
					if(!CCFightTeamManager::getSingleton().Find(fed.fe.FightTeamInfo.dwFTID,&pFT))
						continue;

					FightTeamMemberUID FTUserID={0};
					pFT->GetFightTeamMemberUID(&FTUserID);

					if(iBufLen+sizeof(FTUserID.dwMemberUID) < MAX_MES_MAX_BUFF_LEN)
					{
						memcpy(cBuf+iBufLen,FTUserID.dwMemberUID,sizeof(FTUserID.dwMemberUID));
						iBufLen+=sizeof(FTUserID.dwMemberUID);
					}
				}
			}
		}
	}

	if(Statusmap.size() == 0)
		return;
	
	for(std::map<CT_INT32,CT_INT32>::iterator it=Statusmap.begin();it != Statusmap.end();++it)
	{
		G_FullServiceReward fsr={0};
		if(!G_GetFullServiceReward(0,(*it).first,&fsr))
		{
			CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::FullServerReward G_GetFullServiceReward");
			continue;
		}

		CCUserManager::getSingleton().SendCSFullServerReward(fsr,cBuf,iBufLen);
	}

	return;
}

//新比赛
CT_VOID CCCrossServerFightEvent::NewMatch()
{
	//CCCSFightData::getSingleton().NewMatch();
	CCCSFightData::getSingleton().ClearReward();
	CCCSFightData::getSingleton().ClearSupport();
	return;
}

//威能加成
CT_VOID CCCrossServerFightEvent::AddInsport(UserFightingStatus &msc,CSInspore &csi)
{
	if(csi.iPowerID == 0 || csi.iGrade == 0)
		return;

	G_CSFightInspire csfi={0};
	if(!G_GetCSFightInspire(csi.iPowerID,csi.iGrade,&csfi))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::AddInsport id:%d grae:%d",csi.iPowerID,csi.iGrade);
		return;
	}

	CombatUnitData cud={0};
	G_GetMasterFightingData(msc,cud);
	CCUserBase::UpdateBattleStatus(cud.mcs,csfi.iPowerID[0],csfi.iNum[0]);
	CCUserBase::UpdateBattleStatus(cud.mcs,csfi.iPowerID[1],csfi.iNum[1]);
	CCUserBase::UpdateBattleStatus(cud.mcs,csfi.iPowerID[2],csfi.iNum[2]);
	CCUserBase::UpdateBattleStatus(cud.mcs,csfi.iPowerID[3],csfi.iNum[3]);
	memcpy(&msc.units[cud.position],&cud,sizeof(cud));
	return;
}

CT_BOOL ActiveTitleUserOut(CT_INT32 iTitleItemID, CT_DWORD dwUserID)
{
	G_ItemTitle git = {0};

	if (!G_GetItemTitle(iTitleItemID, &git))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTitle::ActiveTitle G_GetItemTitle data error.itemId:%d", iTitleItemID);
		return CT_FALSE;
	}

	// 更新数据库
	MSC_DB_TitleData DIT={0};
	DIT.dwUserID = dwUserID;
	DIT.iTitleID = git.iTitleID;
	DIT.iActiveTime = time(0);
	DIT.iEffectTime = git.iEffectiveTime;
	DIT.iStatus = TITLE_ACTIVE;
	CT_INT32 threadID = GAME_THREAD_DATABASE + dwUserID % ((CCGameConfig&)(CCConfig::getSingleton())).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID, MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_TITLE_DATA, (CT_CHAR*)&DIT, sizeof(DIT));

	// 更新数据库日志
	MSC_LOGDB_Title LDT = {0};
	LDT.dwUserID = dwUserID;
	LDT.iTitleID = git.iTitleID;
	LDT.iStatus = TITLE_ACTIVE;
	LDT.iLastTime = git.iEffectiveTime;
	CT_DWORD dwThreadID=GAME_THREAD_LOGDATABASE + dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(dwThreadID,MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_TITLE,(CT_CHAR*)&LDT,sizeof(LDT));
	return CT_TRUE;
}

//冠军公告
CT_VOID CCCrossServerFightEvent::ChampionBroadcast()
{
	CT_CHAR cBuf[CS_MAX_BUFF]={0};
	CT_INT32 iBufLen=CS_MAX_BUFF;
	if(!CCCSFightData::getSingleton().GetFinalEightData(Current_Record,cBuf,iBufLen))
	{
		CCLogMeg::getSingleton().WriteError("CCCrossServerFightEvent::FinalEightReward GetFinalEightData Current_Record");
		return;
	}

	std::map<CT_INT32,CT_INT32> CSFTMap;
	for(CT_INT32 i=0;i<iBufLen/sizeof(CCCrossServerFight::FinalEightData);++i)
	{
		CCCrossServerFight::FinalEightData fed={0};
		memcpy(&fed,cBuf+i*sizeof(fed),sizeof(fed));
		if(fed.fe.iWiner/10 == Final_One && fed.fe.iWiner%10 == WINER_LIFT && fed.fe.iGameServerID == ((CCGameConfig&)CCConfig::getSingleton()).m_iGSID)
		{
			CCFightTeam *pFT=0;
			if(CCFightTeamManager::getSingleton().Find(fed.fe.FightTeamInfo.dwFTID,&pFT))
			{
				CMD_G_FightTeamBase ftb={0};
				if(pFT->GetFightTeamBase(&ftb))
				{
					//队长发放‘天下第一’，全队发放‘第一战队’头衔
					CCUser *pBoss;
					if (!CCUserManager::getSingleton().Find(ftb.dwBossUID, &pBoss))
					{
						ActiveTitleUserOut(TITLE_CHOMPION, ftb.dwBossUID);
					}
					else
					{
						//在线发放
						pBoss->m_userTitle.ActiveTitle(TITLE_CHOMPION);
					}

					FightTeamMemberUID ftmu = {0};
					pFT->GetFightTeamMemberUID(&ftmu);
					for (CT_INT32 iFtm = 0;iFtm < ftmu.dwCount;++iFtm)
					{
						CCUser *pMember;
						if (!CCUserManager::getSingleton().Find(ftmu.dwMemberUID[iFtm], &pMember))
						{
							//不在线发放
							ActiveTitleUserOut(TITLE_THE_BEST_TEAM, ftmu.dwMemberUID[iFtm]);
						}
						else
						{
							//在线发放
							pMember->m_userTitle.ActiveTitle(TITLE_THE_BEST_TEAM);
						}
					}


					//播放公告
					RollBroadcast rb={0};
					rb.SetValue(ftb.dwBossUID, ftb.cBossName);
					G_SendRollBroadecast(96,4,1,(CT_CHAR*)&rb,sizeof(rb));
					break;
				}
			}
		}
	}
	return;
}

//删除BOSS
CT_VOID CCCrossServerFightEvent::DeleteBoss(const CT_INT32 iPos)
{
	CCCrossServerFight::FinalEightData fed={0};
	CCCSFightData::getSingleton().GetFinalEightData(Current_Record,iPos,fed);

	for(CT_INT32 i=0;i < CLAN_CRUSADE_MEMBER;++i)
	{
		for(CT_INT32 j=0;j<POST_COUNT/2;++j)
		{
			if(fed.ufs->units[j].type == 0)continue;
			if(GET_TARGET_TYPE(UNIT_ALL_BOSS) <= GET_TARGET_TYPE(fed.ufs->units[j].type) && fed.ufs->units[j].type > UNIT_ALL_ARMY)
			{
				memset(&fed.ufs->units[j].type,0,sizeof(fed.ufs->units[j].type));
				CCUser *pUser=0;
				if(CCUserManager::getSingleton().Find(fed.ufs->dwUserID,&pUser))
				{
					pUser->m_userArmy.DeleteBoss();
				}
				else
					CCFightingData::getSingleton().DeleteBoss(fed.ufs->dwUserID);
			}


		}
	}

	for(CT_INT32 i=0;i < CLAN_CRUSADE_MEMBER;++i)
	{
		CMD_CS_UpdateUserData uud={0};
		uud.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
		uud.ufs=fed.ufs[i];
		uud.csi=fed.csi[i];
		uud.bSystem=CT_FALSE;
		g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_UPDATE_USER_DATA,(CT_CHAR*)&uud,sizeof(uud));
	}
	return;
}
