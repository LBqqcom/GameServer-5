#include "CTopEvent.h"
#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUserManager.h"
#include "CTop.h"
#include "CCompress.h"
#include "CFightTeamManager.h"
#include "CFightingData.h"
#include "CUserBase.h"

#define	MAX_SIZE		100*1024
CCTopEvent::CCTopEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_RANKING);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_GRADE);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGPOWER);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGTEAM);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_CONTINUOUS_WIN);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_VIEW_USER);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGFLAG);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_CHALLENGE);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_PLUNDER);
	RegistMessage(MAIN_G_TOP_ENV,SUB_G_TOP_REWARD);

	m_pBuffer=new CT_CHAR[MAX_SIZE];

	if (GAME_THREAD_CITY == m_dwThread )
	{
		RegistTimer(IDI_TOP_INFO);
		SetTimer(IDI_TOP_INFO,TIME_TOP_INFO);
		RegistMessage(MSC_MAIN_TOP_INFO, MSC_SUB_TOP_MASTER_INFO);
		RegistMessage(MSC_MAIN_TOP_INFO, MSC_SUB_TOP_MASTER_FINISH);
		RegistMessage(MSC_MAIN_TOP_INFO, MSC_SUB_TOP_UPDATE);
	}
	RegistMessage(MSC_MAIN_TOP_INFO, MSC_SUB_TOP_FIGHTING);

}

CCTopEvent::~CCTopEvent(void)
{
	delete[] m_pBuffer;
	m_pBuffer=0;
}

//处理用户消息
CT_VOID CCTopEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MAIN_G_TOP_ENV:
		{
			DisposeUserMessage(subID, pBuf, len);
			break;
		}
	case MSC_MAIN_TOP_INFO:
		{
			DisposeTopInfo(subID, pBuf, len);
			break;
		}
	case MSC_SUB_TOP_FIGHTING:
		{
			DisposeTopFighting(pBuf, len);
			break;
		}
	default:
		break;
	}
}

//处理用户消息， 彭文整理
CT_VOID CCTopEvent::DisposeUserMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case SUB_G_TOP_RANKING:
		{
			DisposeRanking(pBuf,len);
			break;
		}
	case SUB_G_TOP_GRADE:
		{
			DisposeGrade(pBuf,len);
			break;
		}
	case SUB_G_TOP_FIGHTINGPOWER:
		{
			DisposeFightingPower(pBuf,len);
			break;
		}
	case SUB_G_TOP_FIGHTINGTEAM:
		{
			DisposeFightingTeam(pBuf,len);
			break;
		}
	case SUB_G_TOP_CONTINUOUS_WIN:
		{
			DisposeContinuousWin(pBuf,len);
			break;
		}
	case SUB_G_TOP_VIEW_USER:
		{
			DisposeViewUser(pBuf,len);
			break;
		}
	case SUB_G_TOP_FIGHTINGFLAG:
		{
			DisposeFightingFlagPower(pBuf,len);
			break;
		}
	case SUB_G_TOP_CHALLENGE:
		{
			DisposeChallenge(pBuf,len);
			break;
		}
	case SUB_G_TOP_PLUNDER:
		{
			DisposePlunderRes(pBuf, len);
			break;
		}
	case SUB_G_TOP_REWARD:
		{
			DisposeTopReward(pBuf, len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCTopEvent::OnUserEvent subid:%d",subID);
			return;
		}
	}
}

//处理排序信息
CT_VOID CCTopEvent::DisposeTopInfo(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch(subID)
	{
	case MSC_SUB_TOP_MASTER_INFO:
		{
			DisposeMasterInfo(pBuf, len);
			break;
		}
	case MSC_SUB_TOP_MASTER_FINISH:
		{
			DisposeMasterFinish();
			break;
		}
	case MSC_SUB_TOP_UPDATE:
		{
			DisposeUpdataTop();
			break;
		}
	case MSC_SUB_TOP_FIGHTING:
		{
			DisposeTopFighting(pBuf,len);
			break;
		}
	default:
		break;
	}
}

//挑战返回
CT_VOID CCTopEvent::DisposeTopFighting(CT_CHAR* pBuf, CT_DWORD len)
{
	if(sizeof(MSC_TOP_CHALLENGE) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeTopFighting sizeof(MSC_TOP_CHALLENGE) != len");
		return;
	}

	MSC_TOP_CHALLENGE *pTopf=(MSC_TOP_CHALLENGE*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pTopf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeTopFighting No Find UserID:%d",pTopf->dwUserID);
		return;
	}

	//
	if(pTopf->iWiner == WINER_LIFT)
	{
		//
	}
	return;
}

//军衔排行
CT_VOID CCTopEvent::DisposeRanking(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_G_RANKING))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeRanking iLen != sizeof(CMD_G_RANKING)");
		return;
	}

	CMD_G_RANKING *pRanking=(CMD_G_RANKING*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRanking->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeRanking No Find UserID:%d",pRanking->dwUserID);
		return;
	}

	CT_INT32 ilen=MAX_SIZE;
	CCTop::getSingleton().GetTopRankingInfo(m_pBuffer+sizeof(CMD_G_TOPINFO),ilen);

	// 发送奖励领取状态
	SendTopRewardStatus(pUser, TOP_TYPE_RANKING);

	CMD_G_TOPINFO TopInfo={0};
	CT_INT32 iInLen=ilen;
	CT_INT32 iOutLen=MAX_SIZE;
	CT_CHAR *cBuffer=m_pBuffer+sizeof(CMD_G_TOPINFO);
	if(CCCompress::getSingleton().Compress(m_pBuffer+sizeof(CMD_G_TOPINFO),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		TopInfo.iCompression=CT_TRUE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_RANKING_INFO,(CT_CHAR*)m_pBuffer,  iOutLen+sizeof(CMD_G_TOPINFO));
	}
	else
	{
		//压缩失败
		TopInfo.iCompression=CT_FALSE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_RANKING_INFO,m_pBuffer,  ilen+sizeof(CMD_G_TOPINFO));
	}

	return;
}

//等级排行
CT_VOID CCTopEvent::DisposeGrade(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_G_GRADE))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeGrade iLen != sizeof(CMD_G_GRADE)");
		return;
	}

	CMD_G_GRADE *pGrade=(CMD_G_GRADE*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pGrade->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeGrade No Find UserID:%d",pGrade->dwUserID);
		return;
	}

	CT_INT32 ilen=MAX_SIZE;
	CCTop::getSingleton().GetTopGradeInfo(m_pBuffer+sizeof(CMD_G_TOPINFO),ilen);

	// 发送奖励领取状态
	SendTopRewardStatus(pUser, TOP_TYPE_GRADE);

	CMD_G_TOPINFO TopInfo={0};
	CT_INT32 iInLen=ilen;
	CT_INT32 iOutLen=MAX_SIZE;
	CT_CHAR *cBuffer=m_pBuffer+sizeof(CMD_G_TOPINFO);
	if(CCCompress::getSingleton().Compress(m_pBuffer+sizeof(CMD_G_TOPINFO),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		TopInfo.iCompression=CT_TRUE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_GRADE_INFO,(CT_CHAR*)m_pBuffer,  iOutLen+sizeof(CMD_G_TOPINFO));
	}
	else
	{
		//压缩失败
		TopInfo.iCompression=CT_FALSE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_GRADE_INFO,m_pBuffer,  ilen+sizeof(CMD_G_TOPINFO));
	}

	return;
}

//战斗力排行
CT_VOID CCTopEvent::DisposeFightingPower(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_G_FIGHTINGPOWER))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeFightingPower iLen != sizeof(CMD_G_FIGHTINGPOWER)");
		return;
	}

	CMD_G_FIGHTINGPOWER *pFightingPower=(CMD_G_FIGHTINGPOWER*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingPower->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeFightingPower No Find UserID:%d",pFightingPower->dwUserID);
		return;
	}

	CT_INT32 ilen=MAX_SIZE;
	CCTop::getSingleton().GetTopFightingPowerInfo(m_pBuffer+sizeof(CMD_G_TOPINFO),ilen);

	// 发送奖励领取状态
	SendTopRewardStatus(pUser, TOP_TYPE_POWER);

	CMD_G_TOPINFO TopInfo={0};
	CT_INT32 iInLen=ilen;
	CT_INT32 iOutLen=MAX_SIZE;
	CT_CHAR *cBuffer=m_pBuffer+sizeof(CMD_G_TOPINFO);
	if(CCCompress::getSingleton().Compress(m_pBuffer+sizeof(CMD_G_TOPINFO),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		TopInfo.iCompression=CT_TRUE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGPOWER_INFO,(CT_CHAR*)m_pBuffer,  iOutLen+sizeof(CMD_G_TOPINFO));
	}
	else
	{
		//压缩失败
		TopInfo.iCompression=CT_FALSE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGPOWER_INFO,m_pBuffer,  ilen+sizeof(CMD_G_TOPINFO));
	}

	return;
}

//战队排行
CT_VOID CCTopEvent::DisposeFightingTeam(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_G_FIGHTINGTEAM))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeFightingTeam iLen != sizeof(CMD_G_FIGHTINGTEAM)");
		return;
	}

	CMD_G_FIGHTINGTEAM *pFightingTeam=(CMD_G_FIGHTINGTEAM*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingTeam->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeFightingTeam No Find UserID:%d",pFightingTeam->dwUserID);
		return;
	}

	CT_INT32 ilen=MAX_SIZE;
	CCTop::getSingleton().GetTopFightingTeamInfo(m_pBuffer+sizeof(CMD_G_TOPINFO),ilen);

	// 发送奖励领取状态
	SendTopRewardStatus(pUser, TOP_TYPE_TEAM);

	CMD_G_TOPINFO TopInfo={0};
	CT_INT32 iInLen=ilen;
	CT_INT32 iOutLen=MAX_SIZE;
	CT_CHAR *cBuffer=m_pBuffer+sizeof(CMD_G_TOPINFO);
	if(CCCompress::getSingleton().Compress(m_pBuffer+sizeof(CMD_G_TOPINFO),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		TopInfo.iCompression=CT_TRUE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGTEAM_INFO,(CT_CHAR*)m_pBuffer,  iOutLen+sizeof(CMD_G_TOPINFO));
	}
	else
	{
		//压缩失败
		TopInfo.iCompression=CT_FALSE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGTEAM_INFO,m_pBuffer,  ilen+sizeof(CMD_G_TOPINFO));
	}

	return;
}

//连胜排行
CT_VOID CCTopEvent::DisposeContinuousWin(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_G_CONTINUOUSWIN))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeContinuousWin iLen != sizeof(CMD_G_CONTINUOUSWIN)");
		return;
	}

	CMD_G_CONTINUOUSWIN *pContinuousWin=(CMD_G_CONTINUOUSWIN*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pContinuousWin->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeContinuousWin No Find UserID:%d",pContinuousWin->dwUserID);
		return;
	}

	CT_INT32 ilen=MAX_SIZE;
	CCTop::getSingleton().GetTopContinuousWinInfo(m_pBuffer+sizeof(CMD_G_TOPINFO),ilen);

	// 发送奖励领取状态
	SendTopRewardStatus(pUser, TOP_TYPE_WIN);

	CMD_G_TOPINFO TopInfo={0};
	CT_INT32 iInLen=ilen;
	CT_INT32 iOutLen=MAX_SIZE;
	CT_CHAR *cBuffer=m_pBuffer+sizeof(CMD_G_TOPINFO);
	if(CCCompress::getSingleton().Compress(m_pBuffer+sizeof(CMD_G_TOPINFO),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		TopInfo.iCompression=CT_TRUE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_CONTINUOUS_WIN_INFO,(CT_CHAR*)m_pBuffer,  iOutLen+sizeof(CMD_G_TOPINFO));
	}
	else
	{
		//压缩失败
		TopInfo.iCompression=CT_FALSE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_CONTINUOUS_WIN_INFO,m_pBuffer,  ilen+sizeof(CMD_G_TOPINFO));
	}

	return;
}

//查看玩家
CT_VOID CCTopEvent::DisposeViewUser(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_G_VIEWUSER))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeContinuousWin iLen != sizeof(CMD_G_VIEWUSER)");
		return;
	}

	CMD_G_VIEWUSER *pViewUser=(CMD_G_VIEWUSER*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pViewUser->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeViewUser No Find UserID:%d",pViewUser->dwUserID);
		return;
	}

	LookUserInfo UserInfo={0};
	CCTop::getSingleton().GetTopUserInfo(pViewUser->dwDstUserID,UserInfo);
	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_MASTER_ANOTHER_INFO,(CT_CHAR*)&UserInfo,sizeof(UserInfo));

	return;
}



//战旗排行
CT_VOID CCTopEvent::DisposeFightingFlagPower(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(iLen != sizeof(CMD_G_CONTINUOUSWIN))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeContinuousWin iLen != sizeof(CMD_G_CONTINUOUSWIN)");
		return;
	}

	CMD_G_FIGHTINGFLAG *pFightingFlag=(CMD_G_FIGHTINGFLAG*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFightingFlag->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeFightingFlagPower No Find UserID:%d",pFightingFlag->dwUserID);
		return;
	}

	CT_INT32 ilen=MAX_SIZE;
	CCTop::getSingleton().GetTopFightingFlagInfo(m_pBuffer+sizeof(CMD_G_TOPINFO),ilen);

	// 发送奖励领取状态
	SendTopRewardStatus(pUser, TOP_FLAG);

	CMD_G_TOPINFO TopInfo={0};
	CT_INT32 iInLen=ilen;
	CT_INT32 iOutLen=MAX_SIZE;
	CT_CHAR *cBuffer=m_pBuffer+sizeof(CMD_G_TOPINFO);
	if(CCCompress::getSingleton().Compress(m_pBuffer+sizeof(CMD_G_TOPINFO),iInLen,cBuffer,iOutLen))
	{
		//压缩成功
		TopInfo.iCompression=CT_TRUE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGFLAG_INFO,(CT_CHAR*)m_pBuffer,  iOutLen+sizeof(CMD_G_TOPINFO));
	}
	else
	{
		//压缩失败
		TopInfo.iCompression=CT_FALSE;
		memcpy(m_pBuffer,&TopInfo,sizeof(CMD_G_TOPINFO));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_FIGHTINGFLAG_INFO,m_pBuffer,  ilen+sizeof(CMD_G_TOPINFO));
	}

	return;
}

//排行榜挑战
CT_VOID CCTopEvent::DisposeChallenge(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_TOPCHALLENGE) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeChallenge sizeof(CMD_G_TOPCHALLENGE) != iLen");
		return;
	}

	CMD_G_TOPCHALLENGE *pc=(CMD_G_TOPCHALLENGE*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pc->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeChallenge No Find UserID:%d",pc->dwUserID);
		return;
	}

	CMD_G_ReturnFail rf={0};
	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_TOP_COOL_TIME,&vr))
	{
		rf.iStatus=TOPCHANLLENGE_DATA;
	}

	//TOPCHANLLENGE_ME客户端为更新，暂时用数据错误代替
	if(pc->dwDstUserID == pc->dwUserID)
	{
		rf.iStatus=TOPCHANLLENGE_DATA;
	}

	//挑战验证
	rf.iStatus=TopChallengeValidate(pUser,pc->iDiamod,vr.fMaxVal,rf);
	if(rf.iStatus != 0)
	{
		rf.dwDstUserID=pc->dwDstUserID;
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_RETURN_FAIL,(CT_CHAR*)&rf,sizeof(rf));
		return;
	}

	FightingData fd={0};
	if(CCFightingData::getSingleton().GetFightingData(pc->dwDstUserID,fd))
	{
		//MSC_TOP_CHALLENGE topf={0};
		//topf.dwUserID=pc->dwUserID;
		//topf.iSeed=rand();
		//topf.bLookLeft=CT_TRUE;
		//G_GetFightingData(pUser,topf.LeftFightingData);
		//memcpy(&topf.RightFightingData,&fd.FData,sizeof(fd.FData));
		//CT_DWORD dwThreadID=GAME_THREAD_FIGHT+pc->dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		//G_SendThreadInfo(dwThreadID,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_CS_FINAL_EIGHT,(CT_CHAR*)&topf,sizeof(topf));


		MSC_PVP_Fighting_Info topf={0};
		topf.dwUserID=pc->dwUserID;
		topf.iSeed=rand();
		topf.iDifficulty=HIGH;
		G_GetFightingData(pUser,topf.FightingData[0]);
		pUser->m_userArmy.DeleteBoss();
		memcpy(&topf.FightingData[1],&fd.FData,sizeof(fd.FData));
		CT_DWORD dwThreadID=GAME_THREAD_FIGHT+pc->dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		G_SendThreadInfo(dwThreadID,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TOP_FIGHTING,(CT_CHAR*)&topf,sizeof(topf));


		//插入冷却
		pUser->m_userBase.UpdateCooldown(TOPCHALLENGE_COOL,vr.fMaxVal);
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeChallenge dwDstUserID:%d",pc->dwDstUserID);
	}
	return;
}

//挑战验证
CT_INT32 CCTopEvent::TopChallengeValidate(CCUser *pUser,CT_INT32 iDiamod,CT_INT32 iCoolTime,CMD_G_ReturnFail &rf)
{
	if(rf.iStatus != 0)
		return rf.iStatus;

	time_t t=time(0);
	CooldownData data={0};
	if(pUser->m_userBase.GetCooldownData(TOPCHALLENGE_COOL,data))
	{
		if(t < data.toggleTime+data.leftTime)
		{
			rf.lTime=data.toggleTime+data.leftTime;
			return TOPCHANLLENGE_TIME;
		}
	}

	//挑战次数验证
	G_ValueRange FightingValue={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTINGCOUNT,&FightingValue))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::TopChallengeValidate VALUE_TYPE_PVPFIGHTINGCOUNT");
		return TOPCHANLLENGE_DATA;
	}

	//vip add 增加额外挑战次数
	if(pUser->m_userPVPInfo.GetFightingCount() >= FightingValue.fMinVal + pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_ARENA))
	{
		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTING,&vr))
		{
			CCLogMeg::getSingleton().WriteError("CCTopEvent::TopChallengeValidate VALUE_TYPE_PVPFIGHTING");
			return TOPCHANLLENGE_DATA;
		}

		G_ValueRange vrmax={0};
		if(!G_GetValueRange(VALUE_TYPE_PVPFIGHTINGMAX,&vrmax))
		{
			CCLogMeg::getSingleton().WriteError("CCTopEvent::TopChallengeValidate VALUE_TYPE_PVPFIGHTING");
			return TOPCHANLLENGE_DATA;
		}

		CT_INT32 iNum=(CT_INT32)(pUser->m_userPVPInfo.GetFightingCount()-FightingValue.fMinVal-pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_ARENA)+1);
		CT_INT32 iDiamodCost=G_GetPVPFightingDiamod(( iNum > 0?iNum:0),vr.fMinVal);
		iDiamodCost=(-iDiamodCost > -vrmax.fMinVal?vrmax.fMinVal:iDiamodCost);

		//最大竞技次数
		if(pUser->m_userPVPInfo.GetFightingCount() >= FightingValue.fMaxVal)
		{
			return TOPCHANLLENGE_NUM;
		}

		//付费挑战
		if(-iDiamod >= -iDiamodCost)
		{
			MasterVariableData mvd={0};
			pUser->m_userBase.GetMasterData(mvd);
			if(mvd.stRes.iDiamod < -iDiamodCost)
			{
				return TOPCHANLLENGE_DIAMOD;
			}

			MasterVariableData Master={0};
			Master.stRes.iDiamod+=iDiamodCost;
			pUser->m_userBase.AddMastData(Master,SAVETAG_PVP_COST);
		}
		else
		{
			rf.iDiamod=(CT_INT32)((-vrmax.fMinVal<-iDiamodCost)?vrmax.fMinVal:iDiamodCost);
			return TOPCHANLLENGE_PAY;
		}
	}


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
	MasterData md={0};
	pUser->m_userBase.GetMasterData(md);
	MSC_LOGDB_PVPFighringCost PVPFighringCost={0};
	PVPFighringCost.dwUserID=pUser->GetUserID();
	PVPFighringCost.iUserGrade = md.mvd.stGrade.iLevel;
	PVPFighringCost.iDiamod=iDiamod;
	PVPFighringCost.iStamina=0;
	threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_PVPFIGHTINGCOST,(CT_CHAR*)&PVPFighringCost,sizeof(PVPFighringCost));
	return 0;
}
//用户定时器处理  彭文整理
CT_VOID CCTopEvent::OnTime(CT_DWORD timeID)
{
	if(IDI_TOP_INFO == timeID)
	{	
		KillTimer(timeID);
		//((CCDBGlobeData*)m_pDBGlobeEvent)->DisposeReadGlobeData();
		G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_DB_GLOBEDATA,MSC_SUB_DB_GLOBEDATA_READ,0, 0);
		
		DisposeUpdataTop();
		return;
	}
}

//处理排行榜的更新 彭文整理
CT_VOID CCTopEvent::DisposeUpdataTop()
{
	CCTop::getSingleton().ResetVariable();

	CT_DWORD dwUserID[TOP_COUNT]={0};
	CT_INT32 iCount=TOP_COUNT;
	CCFightTeamManager::getSingleton().GetFightTeamTop(dwUserID,iCount);
	CCTop::getSingleton().InsertTopFightingTeam(dwUserID,iCount);
	G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_TOP_DB_INFO, MSC_SUB_DB_TOP_GET, 0, 0);
}


//处理数据发来的领主排行数据 彭文整理
CT_VOID CCTopEvent::DisposeMasterInfo(CT_CHAR* pBuf, CT_DWORD len)
{
	if (len != sizeof(MaterInfo))
	{
		return;
	}
	MaterInfo* pMasterInfo = (MaterInfo*)pBuf;
	CCTop::getSingleton().InsterMasterInfo(*pMasterInfo);
}

//领主信息发完了  彭文整理
CT_VOID CCTopEvent::DisposeMasterFinish()
{
	CCTop::getSingleton().SequenceTop();
	CCTop::MaterInfoMap miMap=CCTop::getSingleton().GetTopMasterInfo();
	if(miMap.begin() == miMap.end())return;

	CT_INT32 iCount=0;
	CT_INT32 iSize=miMap.size();
	std::string strInfo;
	CCTop::MI_IT itend=miMap.end();
	for(CCTop::MI_IT it=miMap.begin();it != itend;++it)
	{
		CT_CHAR cBuf[512]={0};
		sprintf(cBuf,"update topinfo set TopGrade = %d,TopFightingPower = %d,TopContWin = %d,TopRanking = %d,PreTopContWin = %d,PreTopFightingPower = %d,\
		PreTopGrade = %d,PreTopRanking = %d, TopFightingFlagPower = %d, PreFightingFlagPower = %d, TopPlunderRes = %d, PreTopPlunderRes = %d where U_ID = %d;\n",(*it).second.iNowTopGrade,(*it).second.iNowTopFightingPower,(*it).second.iNowTopContWin,(*it).second.iNowTopRank,
		(*it).second.iTopContWin,(*it).second.iTopFightingPower,(*it).second.iTopGrade,(*it).second.iTopRank,(*it).second.iNowTopFightingFlagPower,(*it).second.iTopFightingFlagPower, it->second.iNowTopPlunder, it->second.iTopPlunder,(*it).second.dwUserID);

		strInfo+=cBuf;
		iCount++;
		if(iCount%1000 == 0 || iCount == iSize)
		{
			G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_TOP_DB_INFO, MSC_SUB_DB_TOP_SAVE,(CT_CHAR*)strInfo.c_str(), strInfo.size());
			strInfo.clear();
		}
	}
}

//掠夺排行
CT_VOID CCTopEvent::DisposePlunderRes(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_TopPlunderReq))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposePlunderRes iLen != sizeof(CMD_G_TopPlunderReq)");
		return;
	}

	CMD_G_TopPlunderReq* pPlunder = (CMD_G_TopPlunderReq*)pBuf;

	CCUser* pUser = 0;
	if (!CCUserManager::getSingleton().Find(pPlunder->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposePlunderRes connot find uid:%d", pPlunder->dwUserID);
		return;
	}

	CMD_G_TOPINFO topInfo = {0};
	CT_INT32 len = MAX_SIZE;
	CCTop::getSingleton().GetTopPlunderResInfo(m_pBuffer + sizeof(CMD_G_TOPINFO), len);

	CT_INT32 iOutLen = MAX_SIZE;
	CT_CHAR* cBuffer = m_pBuffer + sizeof(CMD_G_TOPINFO);

	// 发送奖励领取状态
	SendTopRewardStatus(pUser, TOP_TYPE_PLUNDER);

	// 压缩成功
	if (CCCompress::getSingleton().Compress(m_pBuffer+sizeof(CMD_G_TOPINFO),len,cBuffer,iOutLen))
	{
		topInfo.iCompression = CT_TRUE;
		memcpy(m_pBuffer, &(topInfo), sizeof(topInfo));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_PLUNDER_INFO,(CT_CHAR*)m_pBuffer,  iOutLen+sizeof(CMD_G_TOPINFO));
	}
	else
	{
		topInfo.iCompression = CT_FALSE;
		memcpy(m_pBuffer, &(topInfo), sizeof(topInfo));
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV,SUB_G_TOP_PLUNDER_INFO,(CT_CHAR*)m_pBuffer,  len+sizeof(CMD_G_TOPINFO));
	}

	return;
}

//排行榜奖励
CT_VOID CCTopEvent::DisposeTopReward(CT_CHAR* pBuf,CT_INT32 iLen)
{
	if (iLen != sizeof(CMD_G_TopReward))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeTopReward iLen != sizeof(CMD_G_TopReward)");
		return;
	}

	CMD_G_TopReward* pTopReward = (CMD_G_TopReward*)pBuf;
	CCUser* pUser = 0;
	MasterData md = {0};
	G_TopReward gTopReward = {0};
	CT_INT32 enfail = 0;
	CT_INT32 iChestNum = 1;
	CMD_G_TopRewardReturn topReward = {0};
	MSC_DB_TopReward mdReward = {0};
	if (!CCUserManager::getSingleton().Find(pTopReward->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTopEvent::DisposeTopReward connot find user:%d", pTopReward->dwUserID);
		return;
	}

	pUser->m_userBase.GetMasterData(md);

	if (md.mvd.topData.TopInfo[pTopReward->iType].iRewardStatus == TOP_REWARD_RECEIVED)
	{
		G_SendErrorInfo(pUser, ERE_TOP, ETE_RECEIVED_REWARD);
		enfail = -1;
	}

	if (!G_GetTopReward(pTopReward->iType, md.mvd.topData.TopInfo[pTopReward->iType].iRankingCur, &gTopReward)
		|| ( pTopReward->iType == TOP_TYPE_PLUNDER && md.mvd.plunderData.iPlunderScore == 0))
	{
		G_SendErrorInfo(pUser, ERE_TOP, ETE_NO_REWARD);
		enfail = -1;
	}

	if (enfail == 0)
	{
		enfail = pUser->LootItems(SAVATAG_TOP_REWARD, &(gTopReward.iRewardID), &iChestNum, 1, ITEM_OP_MODE_CHECK);
	}
	
	if (enfail == LOOT_BAG_FULL || enfail == LOOT_ELM_FULL)
	{
		G_SendErrorInfo(pUser, ERE_SYSTEM, EEI_BUG_FULL);
	}

	if (enfail != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV, SUB_G_TOP_REWARD_FAIL);
		return;
	}

	// 获得奖励
	pUser->LootItems(SAVATAG_TOP_REWARD, &(gTopReward.iRewardID), &iChestNum, 1, ITEM_OP_MODE_EXEC);
	
	// 更新内存
	pUser->m_userBase.UpdateTopRewardStatus(pTopReward->iType, TOP_REWARD_RECEIVED);

	// 发送客户端
	topReward.iType = gTopReward.iType;
	topReward.iRanking = gTopReward.iTop;
	pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV, SUB_G_TOP_REWARD_SUC, (CT_CHAR*)&topReward, sizeof(topReward));

	// 保存数据库
	mdReward.dwUserID = pUser->GetUserID();
	mdReward.iType = pTopReward->iType;
	mdReward.iStatus = TOP_REWARD_RECEIVED;
	G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_TOP_DB_INFO, MSC_SUB_DB_TOP_REWARD, (CT_CHAR*)&mdReward, sizeof(mdReward));
}

//发送奖励领取状态
CT_VOID CCTopEvent::SendTopRewardStatus(CCUser* pUser, CT_INT32 iType)
{
	if (pUser == NULL || pUser == 0)
	{
		return;
	}

	MasterData md = {0};
	CMD_G_TopRewardStatus trs = {0};
	pUser->m_userBase.GetMasterData(md);

	trs.iType = iType;
	trs.iStatus = md.mvd.topData.TopInfo[iType].iRewardStatus;
	pUser->GetNetInterface()->Send(MAIN_G_TOP_ENV, SUB_G_TOP_REWARD_INFO, (CT_CHAR*)&trs, sizeof(trs));
}