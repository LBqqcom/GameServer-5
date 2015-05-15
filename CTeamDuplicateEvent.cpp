#include "CTeamDuplicateEvent.h"
#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CTeamDuplicate.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CFightingScriptSetting.h"
#include "CBroadcastScriptSetting.h"

CCTeamDuplicateEvent::CCTeamDuplicateEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_CREATEROOM);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ADDROOM);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_FASTADDROOM);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_POS);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_READY);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_BEGIN);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_CHANGEROOMINFO);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_INVITEPLAYER);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_AUTOMATCH);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_LEAVE);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ENTER);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_UPDATROOM);
	RegistMessage(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_CLEAR);

	RegistMessage(MSC_MAIN_TEAMDUPLICATE,MSC_SUB_TEAMDUPLICATE_LEAVE);
	RegistMessage(MSC_MAIN_TEAMDUPLICATE,MSC_SUB_TEAMDUPLICATE_FIGHTINGEND);
	RegistMessage(MSC_MAIN_TEAMDUPLICATE,MSC_SUB_TEAMDUPLICATE_AUTOMATCHINFO);
}

CCTeamDuplicateEvent::~CCTeamDuplicateEvent(void)
{
}
//用户定时器处理
CT_VOID CCTeamDuplicateEvent::OnTime(CT_DWORD timeID)
{
	return;
}
//处理用户消息
CT_VOID CCTeamDuplicateEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	if(MAIN_G_TEAM_DUPLICATE != mainID && MSC_MAIN_TEAMDUPLICATE != mainID)
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::OnUserEvent MAIN_G_TEAM_DUPLICATE != mainID");
		return;
	}

	switch (subID)
	{
	case SUB_G_TEAMDUPLICATE_CREATEROOM:
		{
			DisposeCreateRoom(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_ADDROOM:
		{
			DisposeAddRoom(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_FASTADDROOM:
		{
			DisposeFastAddRoom(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_POS:
		{
			DisposePos(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_READY:
		{
			DisposeReady(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_BEGIN:
		{
			DisposeBegin(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_CHANGEROOMINFO:
		{
			DisposeChangeRoomInfo(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_INVITEPLAYER:
		{
			DisposeInvitePlayer(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_AUTOMATCH:
		{
			DiposeAutoMatch(pBuf,len);
			break;
		}
	case MSC_SUB_TEAMDUPLICATE_LEAVE:
	case SUB_G_TEAMDUPLICATE_LEAVE:
		{
			DisposeLeave(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_UPDATROOM:
		{
			DiposeEnter(pBuf,len,SUB_G_TEAMDUPLICATE_UPDATROOM);
			break;
		}
	case SUB_G_TEAMDUPLICATE_ENTER:
		{
			DiposeEnter(pBuf,len,SUB_G_TEAMDUPLICATE_ENTER);
			break;
		}
	case SUB_G_TEAMDUPLICATE_REFUSE:
		{
			DiposeRefuse(pBuf,len);
			break;
		}
	case MSC_SUB_TEAMDUPLICATE_FIGHTINGEND:
		{
			DisposeFightingEnd(pBuf,len);
			break;
		}
	case MSC_SUB_TEAMDUPLICATE_AUTOMATCHINFO:
		{
			DisposeAutoMatchInfo(pBuf,len);
			break;
		}
	case SUB_G_TEAMDUPLICATE_CLEAR:
		{
			DisposeClear(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::OnUserEvent subID:%d",subID);
			return;
		}
	}
	return;
}

//创建房间
CT_VOID CCTeamDuplicateEvent::DisposeCreateRoom(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateCreateRoom))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeCreateRoom len != sizeof(CMD_G_TeamDuplicateCreateRoom)");
		return;
	}

	CMD_G_TeamDuplicateCreateRoom *pCreateRoom=(CMD_G_TeamDuplicateCreateRoom*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pCreateRoom->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeCreateRoom No Find UserID:%d",pCreateRoom->dwUserID);
		return;
	}

	if(!pUser->m_userTeamDuplicate.VaildateScene(pCreateRoom->iSceneID,pCreateRoom->iDifficulty))
	{
		pUser->CloseNet();
		return;
	}

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	TeamDuplicateRoomInfo RoomInfo={0};
	RoomInfo.dwUserID=pCreateRoom->dwUserID;
	RoomInfo.iSceneID=pCreateRoom->iSceneID;
	RoomInfo.iDifficulty=pCreateRoom->iDifficulty;
	RoomInfo.iFightingTime[0]=mvd.stRes.iTeamDuplicateFightingTimes;
	G_GetFightingData(pUser,RoomInfo.FightingData[0]);

	CT_INT32 iCreateInfo=CCTeamDuplicate::getSingleton().CreateRoom(RoomInfo);
	if(iCreateInfo == TEAMDUPLICATE_CREATE_SUC)
	{
		TeamDuplicateRoomInfo RoomInfo={0};
		CCTeamDuplicate::getSingleton().GetRoomInfo(pCreateRoom->dwUserID,RoomInfo);

		CMD_G_TeamDuplicateRoomInfo RoomData={0};
		RoomData.dwUserID=pCreateRoom->dwUserID;
		RoomData.dwRoomMasterID=RoomInfo.dwUserID;
		RoomData.iRoomID=RoomInfo.iRoomID;
		RoomData.iSceneID=pCreateRoom->iSceneID;
		RoomData.iDifficulty=pCreateRoom->iDifficulty;
		memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
		memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

		PTRLinkUserData ud={0};
		UserFightingStatus us={0};
		pUser->GetPTRLinkUserData(ud);
		G_GetLookUserInfo(ud,us,RoomData.UserInfo[0]);

		CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));
		return;
	}
	else
	{
		CMD_G_TeamDuplicateCreateInfo CreateInfo={0};
		CreateInfo.dwUserID=pCreateRoom->dwUserID;
		CreateInfo.iCreateInfo=iCreateInfo;
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_CREATEINFO,(CT_CHAR*)&CreateInfo,sizeof(CreateInfo));
		return;
	}

	return;
}

//加入房间
CT_VOID CCTeamDuplicateEvent::DisposeAddRoom(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateAddRoom))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeAddRoom len != sizeof(CMD_G_TeamDuplicateAddRoom)");
		return;
	}

	CMD_G_TeamDuplicateAddRoom *pAddRoom=(CMD_G_TeamDuplicateAddRoom*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pAddRoom->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeAddRoom No Find UserID:%d",pAddRoom->dwUserID);
		return;
	}
	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	UserFightingStatus UserData={0};
	G_GetFightingData(pUser,UserData);
	CT_INT32 iAddInfo=CCTeamDuplicate::getSingleton().AddRoom(pAddRoom->iRoomID,mvd.stRes.iTeamDuplicateFightingTimes,UserData);
	if(iAddInfo == TEAMDUPLICATE_ADD_SUC)
	{
		TeamDuplicateRoomInfo RoomInfo={0};
		CCTeamDuplicate::getSingleton().GetRoomInfo(pAddRoom->dwUserID,RoomInfo);

		CMD_G_TeamDuplicateRoomInfo RoomData={0};
		RoomData.dwUserID=pAddRoom->dwUserID;
		RoomData.dwRoomMasterID=RoomInfo.dwUserID;
		RoomData.iRoomID=RoomInfo.iRoomID;
		RoomData.iSceneID=RoomInfo.iSceneID;
		RoomData.iDifficulty=RoomInfo.iDifficulty;
		memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
		memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if(RoomInfo.FightingData[i].dwUserID == 0)continue;

			if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
			{

				CCUser *pDstUser=0;
				if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
				{
					CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeAddRoom No Find UserID:%d",RoomInfo.FightingData[i].dwUserID);
					continue;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
				}
				PTRLinkUserData ud={0};
				UserFightingStatus us={0};
				pDstUser->GetPTRLinkUserData(ud);
				G_GetLookUserInfo(ud,us,RoomData.UserInfo[i]);
				//CCTeamDuplicate::getSingleton().AutoMatch(RoomInfo.iRoomID,us,RoomData.UserInfo[i]);
			}
			else
			{
				memcpy(&RoomData.UserInfo[i],&RoomInfo.AutoMatchUser[i],sizeof(RoomData.UserInfo[i]));
			}
		}

		CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));

		return;
	}
	else
	{
		CMD_G_TeamDuplicateAddInfo AddInfo={0};
		AddInfo.dwUserID=pAddRoom->dwUserID;
		AddInfo.iAddInfo=iAddInfo;
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ADDINFO,(CT_CHAR*)&AddInfo,sizeof(AddInfo));
		return;
	}
	return;
}

//快速加入
CT_VOID CCTeamDuplicateEvent::DisposeFastAddRoom(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateFastAddRoom))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeFastAddRoom len != sizeof(CMD_G_TeamDuplicateFastAddRoom)");
		return;
	}

	CMD_G_TeamDuplicateFastAddRoom *pFastAdd=(CMD_G_TeamDuplicateFastAddRoom*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pFastAdd->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeFastAddRoom No Find UserID:%d",pFastAdd->dwUserID);
		return;
	}

	//获取已开场景
	CT_INT32 iSceneID=0;
	for(CT_INT32 i=0;i<TEAMDUPLICATE_SCENENUM;++i)
	{
		if(pUser->m_userTeamDuplicate.VaildateScene(i,0))
		{
			iSceneID=i;
		}
	}
	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	UserFightingStatus UserData={0};
	G_GetFightingData(pUser,UserData);
	CT_INT32 iFastAddInfo=TEAMDUPLICATE_FASTADD_FAIL;
	for(CT_INT32 i=TEAMDUPLICATE_SCENENUM;i>=0;--i)
	{
		for(CT_INT32 j=TEAMDUPLICATE_DIFFICULTY-1;j>=0;--j)
		{
			if(pUser->m_userTeamDuplicate.VaildateScene(i,j))
			{
				iFastAddInfo=CCTeamDuplicate::getSingleton().AddFastRoom(mvd.stRes.iTeamDuplicateFightingTimes,UserData,i,j);
				if(iFastAddInfo == TEAMDUPLICATE_FASTADD_SUC)break;
			}
		}
		if(iFastAddInfo == TEAMDUPLICATE_FASTADD_SUC)break;
	}
	if(iFastAddInfo == TEAMDUPLICATE_FASTADD_SUC)
	{
		TeamDuplicateRoomInfo RoomInfo={0};
		CCTeamDuplicate::getSingleton().GetRoomInfo(pFastAdd->dwUserID,RoomInfo);

		CMD_G_TeamDuplicateRoomInfo RoomData={0};
		RoomData.dwUserID=pFastAdd->dwUserID;
		RoomData.dwRoomMasterID=RoomInfo.dwUserID;
		RoomData.iRoomID=RoomInfo.iRoomID;
		RoomData.iSceneID=RoomInfo.iSceneID;
		RoomData.iDifficulty=RoomInfo.iDifficulty;
		memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
		memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if(RoomInfo.FightingData[i].dwUserID == 0)continue;

			if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
			{
				CCUser *pDstUser=0;
				if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
				{
					CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeFastAddRoom No Find UserID:%d",RoomInfo.FightingData[i].dwUserID);
					continue;
				}

				PTRLinkUserData ud={0};
				UserFightingStatus us={0};
				pDstUser->GetPTRLinkUserData(ud);
				G_GetLookUserInfo(ud,us,RoomData.UserInfo[i]);
			}
			else
			{
				memcpy(&RoomData.UserInfo[i],&RoomInfo.AutoMatchUser[i],sizeof(RoomData.UserInfo[i]));
			}
		}

		CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));

		return;
	}
	else
	{
		CMD_G_TeamDuplicateAddInfo AddInfo={0};
		AddInfo.dwUserID=pFastAdd->dwUserID;
		AddInfo.iAddInfo=iFastAddInfo;
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ADDINFO,(CT_CHAR*)&AddInfo,sizeof(AddInfo));
		return;
	}

	return;
}

//阵位调整
CT_VOID CCTeamDuplicateEvent::DisposePos(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicatePos))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposePos len != sizeof(CMD_G_TeamDuplicatePos)");
		return;
	}

	CMD_G_TeamDuplicatePos *pPos=(CMD_G_TeamDuplicatePos*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pPos->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposePos No Find UserID:%d",pPos->dwUserID);
		return;
	}

	UserFightingStatus UserData={0};
	G_GetFightingData(pUser,UserData);
	CT_INT32 iChangeInfo=CCTeamDuplicate::getSingleton().ChangePos(pPos->iRoomID,pPos->dwUserID,pPos->dwUserPos);
	if(iChangeInfo == TEAMDUPLICATE_CHANGEPOS_SUC)
	{
		TeamDuplicateRoomInfo RoomInfo={0};
		CCTeamDuplicate::getSingleton().GetRoomInfo(pPos->dwUserID,RoomInfo);

		CMD_G_TeamDuplicateRoomInfo RoomData={0};
		RoomData.dwUserID=pPos->dwUserID;
		RoomData.dwRoomMasterID=RoomInfo.dwUserID;
		RoomData.iRoomID=RoomInfo.iRoomID;
		RoomData.iSceneID=RoomInfo.iSceneID;
		RoomData.iDifficulty=RoomInfo.iDifficulty;
		memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
		memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if(RoomInfo.FightingData[i].dwUserID == 0)continue;

			if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
			{
				CCUser *pDstUser=0;
				if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
				{
					CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposePos No Find UserID:%d",RoomInfo.FightingData[i].dwUserID);
					continue;
				}

				PTRLinkUserData ud={0};
				UserFightingStatus us={0};
				pDstUser->GetPTRLinkUserData(ud);
				G_GetLookUserInfo(ud,us,RoomData.UserInfo[i]);
			}
			else
			{
				memcpy(&RoomData.UserInfo[i],&RoomInfo.AutoMatchUser[i],sizeof(RoomData.UserInfo[i]));
			}
		}

		CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));

		return;
	}
	else
	{
		CMD_G_TeamDuplicatePosInfo PosInfo={0};
		PosInfo.dwUserID=pPos->dwUserID;
		PosInfo.iChangeInfo=iChangeInfo;
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_POSINFO,(CT_CHAR*)&PosInfo,sizeof(PosInfo));
		return;
	}

	return;
}

//玩家准备
CT_VOID CCTeamDuplicateEvent::DisposeReady(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateReady))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeReady len != sizeof(CMD_G_TeamDuplicateReady)");
		return;
	}

	CMD_G_TeamDuplicateReady *pReady=(CMD_G_TeamDuplicateReady*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pReady->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeReady No Find UserID:%d",pReady->dwUserID);
		return;
	}

	CT_INT32 UserStatusInfo=CCTeamDuplicate::getSingleton().SetUserStatus(pReady->iRoomID,pReady->dwUserID);
	if(UserStatusInfo == TEAMDUPLICATE_STATUS_SUC)
	{
		CCTeamDuplicate::getSingleton().SendRoomMessage(pReady->iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_READY,pBuf,len);
		return;
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeReady UserStatusInfo != TEAMDUPLICATE_STATUS_SUC");
		pUser->CloseNet();
		return;
	}
	return;
}

//玩家开始
CT_VOID CCTeamDuplicateEvent::DisposeBegin(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateBegin))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeBegin len != sizeof(CMD_G_TeamDuplicateBegin)");
		return;
	}

	CMD_G_TeamDuplicateBegin *pBegin=(CMD_G_TeamDuplicateBegin*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pBegin->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeBegin No Find UserID:%d",pBegin->dwUserID);
		return;
	}

	TeamDuplicateRoomInfo RoomInfo={0};
	CCTeamDuplicate::getSingleton().GetRoomInfo(pBegin->iRoomID,RoomInfo);

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_TRUE || RoomInfo.FightingData[i].dwUserID == 0 ||
			RoomInfo.FightingData[i].dwUserID == RoomInfo.dwUserID)continue;
		if(RoomInfo.iStatus[i] != TEAMDUPLICATE_STATUS_READY)
		{
			CMD_G_TeamDuplicateBeginInfo BeginInfo={0};
			BeginInfo.dwUserID=pUser->GetUserID();
			BeginInfo.dwDstID=RoomInfo.FightingData[i].dwUserID;
			BeginInfo.iBeginInfo=TEAMDUPLICATE_BEGIN_STATUS;
			pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_BEGININFO,(CT_CHAR*)&BeginInfo,sizeof(BeginInfo));
			return;
		}
	}

	//判断人数
	CT_INT32 iNum=0;
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(RoomInfo.FightingData[i].dwUserID != 0)
			iNum++;
	}
	if(iNum < TEAM_NUM)
	{
		CMD_G_TeamDuplicateBeginInfo BeginInfo={0};
		BeginInfo.dwUserID=pUser->GetUserID();
		BeginInfo.dwDstID=pUser->GetUserID();
		BeginInfo.iBeginInfo=TEAMDUPLICATE_BEGIN_COUNT;
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_BEGININFO,(CT_CHAR*)&BeginInfo,sizeof(BeginInfo));
		return;
	}

	G_TeamDuplicate TeamDuplicate={0};
	if(!G_GetTeamDuplicate(RoomInfo.iSceneID,RoomInfo.iDifficulty,&TeamDuplicate))
	{
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_DATAERROR);
		return;
	}

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(RoomInfo.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE ||
			RoomInfo.FightingData[i].dwUserID == 0)continue;

		CCUser *pDstUser=0;
		if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeBegin No Find DstUserID:%d",RoomInfo.FightingData[i].dwUserID);
			return;
		}
		if(!pDstUser->m_userTeamDuplicate.VaildateScene(RoomInfo.iSceneID,RoomInfo.iDifficulty))
		{
			CMD_G_TeamDuplicateBeginInfo BeginInfo={0};
			BeginInfo.dwUserID=pUser->GetUserID();
			BeginInfo.dwDstID=pDstUser->GetUserID();
			BeginInfo.iBeginInfo=TEAMDUPLICATE_BEGIN_SCENE;
			pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_BEGININFO,(CT_CHAR*)&BeginInfo,sizeof(BeginInfo));
			return;
		}



		MasterVariableData mvd={0};
		pDstUser->m_userBase.GetMasterData(mvd);
		if(mvd.stRes.iStamina < -TeamDuplicate.iCostStamina)
		{
			CMD_G_TeamDuplicateBeginInfo BeginInfo={0};
			BeginInfo.dwUserID=pUser->GetUserID();
			BeginInfo.dwDstID=pDstUser->GetUserID();
			BeginInfo.iBeginInfo=TEAMDUPLICATE_BEGIN_STAMINA;
			pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_BEGININFO,(CT_CHAR*)&BeginInfo,sizeof(BeginInfo));
			return;
		}
	}

	//删除召唤BOSS
	CCTeamDuplicate::getSingleton().DeleteBoss(pBegin->iRoomID);
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(RoomInfo.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE ||
			RoomInfo.FightingData[i].dwUserID == 0)continue;

		CCUser *pDstUser=0;
		if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeBegin No Find DstUserID:%d",RoomInfo.FightingData[i].dwUserID);
			return;
		}
		pDstUser->m_userArmy.DeleteBoss();
	}

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		CCUser *pDstUser=0;
		if(RoomInfo.FightingData[i].dwUserID == 0 ||RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_TRUE)continue;
		if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeBegin No Find DstUserID:%d",RoomInfo.FightingData[i].dwUserID);
			return;
		}

		MasterVariableData mvd={0};
		mvd.stRes.iStamina+=TeamDuplicate.iCostStamina;
		pDstUser->m_userBase.AddMastData(mvd,SAVETAG_TEAMDUOLICATE_COST);

		MSC_LOGDB_TeamDuplicateCost Cost={0};
		Cost.dwUserID=RoomInfo.FightingData[i].dwUserID;
		Cost.iSceneID=RoomInfo.iSceneID;
		Cost.iDifficulty=RoomInfo.iDifficulty;
		Cost.iStaminaCost=TeamDuplicate.iCostStamina;

		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pDstUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TEAMDUPLICATECOST,(CT_CHAR*)&Cost,sizeof(Cost));

	}

	//进入战斗
	MSC_TeamDuplicate_Fighting Fighting={0};
	Fighting.dwUserID=RoomInfo.dwUserID;
	Fighting.iSceneID=RoomInfo.iSceneID;
	Fighting.iDifficulty=RoomInfo.iDifficulty;
	Fighting.iSeed=rand()%RAND_MAX;
	Fighting.iWiner=WINER_NULL;

	iNum=0;
	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(RoomInfo.FightingData[i].dwUserID != 0)
		{
			memcpy(&Fighting.FightingData[iNum],&RoomInfo.FightingData[i],sizeof(Fighting.FightingData[i]));
			iNum++;
		}
	}

	CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_TEAMDUPLICATE_FIGHTING,(CT_CHAR*)&Fighting,sizeof(Fighting));

	return;
}

//房间信息调整
CT_VOID CCTeamDuplicateEvent::DisposeChangeRoomInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateChangeRoomInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeChangeRoomInfo len != sizeof(CMD_G_TeamDuplicateChangeRoomInfo)");
		return;
	}

	CMD_G_TeamDuplicateChangeRoomInfo *pChangeRoomInfo=(CMD_G_TeamDuplicateChangeRoomInfo*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pChangeRoomInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeChangeRoomInfo No Find UserID:%d",pChangeRoomInfo->dwUserID);
		return;
	}

	TeamDuplicateRoomInfo RoomInfo={0};
	CCTeamDuplicate::getSingleton().GetRoomInfo(pChangeRoomInfo->dwUserID,RoomInfo);
	if(RoomInfo.dwUserID != pChangeRoomInfo->dwUserID)
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeChangeRoomInfo (RoomInfo.dwUserID != pChangeRoomInfo->dwUserID");
		pUser->CloseNet();
		return;
	}

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		CCUser *pDstUser=0;
		if(RoomInfo.FightingData[i].dwUserID == 0 ||
			RoomInfo.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE)continue;
		if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeChangeRoomInfo No Find DstUserID:%d",RoomInfo.FightingData[i].dwUserID);
			return;
		}

		if(!pDstUser->m_userTeamDuplicate.VaildateScene(pChangeRoomInfo->iSceneID,pChangeRoomInfo->iDifficulty))
		{
			//验证个玩家条件
			CMD_G_TeamDuplicateChangeInfo ChangeInfo={0};
			ChangeInfo.dwUserID=pChangeRoomInfo->dwUserID;
			ChangeInfo.dwDstUserID=pDstUser->GetUserID();
			ChangeInfo.iChangeInfo=TEAMDUPLICATE_CHANGEROOM_SCENE;
			pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_CHANGEINFO,(CT_CHAR*)&ChangeInfo,sizeof(ChangeInfo));

			return;
		}
	}

	UserFightingStatus UserData={0};
	G_GetFightingData(pUser,UserData);
	CT_INT32 iChangeInfo=CCTeamDuplicate::getSingleton().ChangeRoomInfo(RoomInfo.iRoomID,pChangeRoomInfo->dwUserID,pChangeRoomInfo->iSceneID,pChangeRoomInfo->iDifficulty);
	if(iChangeInfo == TEAMDUPLICATE_CHANGEROOM_SUC)
	{
		memset(&RoomInfo,0,sizeof(RoomInfo));
		CCTeamDuplicate::getSingleton().GetRoomInfo(pChangeRoomInfo->dwUserID,RoomInfo);

		CMD_G_TeamDuplicateRoomInfo RoomData={0};
		RoomData.dwUserID=pChangeRoomInfo->dwUserID;
		RoomData.dwRoomMasterID=RoomInfo.dwUserID;
		RoomData.iRoomID=RoomInfo.iRoomID;
		RoomData.iSceneID=RoomInfo.iSceneID;
		RoomData.iDifficulty=RoomInfo.iDifficulty;
		memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
		memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if(RoomInfo.FightingData[i].dwUserID == 0)continue;

			if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
			{
				CCUser *pDstUser=0;
				if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
				{
					CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeChangeRoomInfo No Find UserID:%d",RoomInfo.FightingData[i].dwUserID);
					continue;
				}

				PTRLinkUserData ud={0};
				UserFightingStatus us={0};
				pDstUser->GetPTRLinkUserData(ud);
				G_GetLookUserInfo(ud,us,RoomData.UserInfo[i]);
			}
			else
			{
				memcpy(&RoomData.UserInfo[i],&RoomInfo.AutoMatchUser[i],sizeof(RoomData.UserInfo[i]));
			}
		}

		CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));

		return;
	}
	else
	{
		pUser->CloseNet();
		return;
	}
	return;
}

//邀请好友
CT_VOID CCTeamDuplicateEvent::DisposeInvitePlayer(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateInvitePlayer))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeInvitePlayer len != sizeof(CMD_G_TeamDuplicateInvitePlayer)");
		return;
	}

	CMD_G_TeamDuplicateInvitePlayer *pInvitePlayer=(CMD_G_TeamDuplicateInvitePlayer*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pInvitePlayer->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeInvitePlayer No Find UserID:%d",pInvitePlayer->dwUserID);
		return;
	}

	TeamDuplicateRoomInfo RoomInfo={0};
	if(!CCTeamDuplicate::getSingleton().GetRoomInfo(pInvitePlayer->dwUserID,RoomInfo))
	{
		pUser->CloseNet();
		return;
	}

	if(pInvitePlayer->iType == TEAMDUPLICATE_INVAIT_FIREND)
	{
		if(!pUser->m_userFriend.CheckIsFriend(pInvitePlayer->dwFirendID))
		{
			pUser->CloseNet();
			return;
		}

		CCUser *pDstUser=0;
		if(!CCUserManager::getSingleton().Find(pInvitePlayer->dwFirendID,&pDstUser))
		{
			CMD_G_TeamDuplicateInvitePlayer InviteInfo={0};
			InviteInfo.dwFirendID=pInvitePlayer->dwFirendID;
			InviteInfo.dwUserID=pInvitePlayer->dwUserID;
			InviteInfo.iType=TEAMDUPLICATE_INVAIT_FAIL;
			InviteInfo.iRoomID=RoomInfo.iRoomID;
			InviteInfo.iSceneID=RoomInfo.iSceneID;
			InviteInfo.iDifficulty=RoomInfo.iDifficulty;
			pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_INVITEINFO,(CT_CHAR*)&InviteInfo,sizeof(InviteInfo));

			return;
		}

		pDstUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_INVITEINFO,pBuf,len);

		CMD_G_TeamDuplicateInvitePlayer InviteInfo={0};
		InviteInfo.dwFirendID=pInvitePlayer->dwFirendID;
		InviteInfo.dwUserID=pInvitePlayer->dwUserID;
		InviteInfo.iType=TEAMDUPLICATE_INVAIT_SUC;
		InviteInfo.iRoomID=RoomInfo.iRoomID;
		InviteInfo.iSceneID=RoomInfo.iSceneID;
		InviteInfo.iDifficulty=RoomInfo.iDifficulty;
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_INVITEINFO,(CT_CHAR*)&InviteInfo,sizeof(InviteInfo));
	}
	else if(pInvitePlayer->iType == TEAMDUPLICATE_INVAIT_TEAM)
	{
		CT_INT32 dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pInvitePlayer->dwUserID);
		if(dwFTID != CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pInvitePlayer->dwFirendID))
		{
			pUser->CloseNet();
			return;
		}

		CCUser *pDstUser=0;
		if(!CCUserManager::getSingleton().Find(pInvitePlayer->dwFirendID,&pDstUser))
		{
			CMD_G_TeamDuplicateInvitePlayer InviteInfo={0};
			InviteInfo.dwFirendID=pInvitePlayer->dwFirendID;
			InviteInfo.dwUserID=pInvitePlayer->dwUserID;
			InviteInfo.iType=TEAMDUPLICATE_INVAIT_FAIL;
			InviteInfo.iRoomID=RoomInfo.iRoomID;
			InviteInfo.iSceneID=RoomInfo.iSceneID;
			InviteInfo.iDifficulty=RoomInfo.iDifficulty;
			pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_INVITEINFO,(CT_CHAR*)&InviteInfo,sizeof(InviteInfo));
			return;
		}

		pDstUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_INVITEINFO,pBuf,len);

		CMD_G_TeamDuplicateInvitePlayer InviteInfo={0};
		InviteInfo.dwFirendID=pInvitePlayer->dwFirendID;
		InviteInfo.dwUserID=pInvitePlayer->dwUserID;
		InviteInfo.iType=TEAMDUPLICATE_INVAIT_SUC;
		InviteInfo.iRoomID=RoomInfo.iRoomID;
		InviteInfo.iSceneID=RoomInfo.iSceneID;
		InviteInfo.iDifficulty=RoomInfo.iDifficulty;
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_INVITEINFO,(CT_CHAR*)&InviteInfo,sizeof(InviteInfo));
		return;
	}
	return;
}

//自动匹配
CT_VOID CCTeamDuplicateEvent::DiposeAutoMatch(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateAutoMatch))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DiposeAutoMatch len != sizeof(CMD_G_TeamDuplicateAutoMatch)");
		return;
	}
	
	CMD_G_TeamDuplicateAutoMatch *pAutoMatch=(CMD_G_TeamDuplicateAutoMatch*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pAutoMatch->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DiposeAutoMatch No Find UserID:%d",pAutoMatch->dwUserID);
		return;
	}
	TeamDuplicateRoomInfo RoomInfo={0};
	if(!CCTeamDuplicate::getSingleton().GetRoomInfo(pAutoMatch->dwUserID,RoomInfo))
	{
		//pUser->CloseNet();
		return;
	}

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	MSC_DB_TeamDuplicateAutoMatch AutoMatch={0};
	AutoMatch.dwUserID=pAutoMatch->dwUserID;
	AutoMatch.iFightingPower=mvd.stRes.iFightingPower;
	AutoMatch.iGrade=mvd.stGrade.iLevel;

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		AutoMatch.dwDstUserID[i]=RoomInfo.FightingData[i].dwUserID;
	}

	CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_TEAMDUPLICATE_AUTOMATCH,(CT_CHAR*)&AutoMatch,sizeof(AutoMatch));

	return;
}

//玩家离开
CT_VOID CCTeamDuplicateEvent::DisposeLeave(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateLeave))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeLeave len != sizeof(CMD_G_TeamDuplicateLeave)");
		return;
	}

	CMD_G_TeamDuplicateLeave *pLeave=(CMD_G_TeamDuplicateLeave*)pBuf;

	TeamDuplicateRoomInfo RoomInfo={0};
	if(!CCTeamDuplicate::getSingleton().GetRoomInfo(pLeave->dwUserID,RoomInfo))
	{
		//pUser->CloseNet();
		return;
	}

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pLeave->dwUserID,&pUser))
	{
		//CCTeamDuplicate::getSingleton().LeaveRoom(RoomInfo.iRoomID,pLeave->dwUserID);
		//return;
	}

	if(RoomInfo.dwUserID != pLeave->dwUserID && pLeave->dwUserID != pLeave->dwDstID)
	{
		if(pUser != NULL)
			pUser->CloseNet();
		return;
	}

	CT_INT32 iLeave=CCTeamDuplicate::getSingleton().LeaveRoom(RoomInfo.iRoomID,pLeave->dwDstID);
	if(iLeave == TEAMDUPLICATE_LEAVE_SUC)
	{
		CMD_G_TeamDuplicateLeave LeaveInfo={0};
		LeaveInfo.dwUserID=pLeave->dwUserID;
		LeaveInfo.dwDstID=pLeave->dwDstID;
		if(pLeave->dwUserID == pLeave->dwDstID)
		{
			if(pUser != NULL)
				pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_LEAVEINFO,(CT_CHAR*)&LeaveInfo,sizeof(LeaveInfo));
		}
		else
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if(RoomInfo.FightingData[i].dwUserID == pLeave->dwDstID)
				{
					if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_TRUE)break;
					CCUser *pDstUser=0;
					if(!CCUserManager::getSingleton().Find(pLeave->dwDstID,&pDstUser))
					{
						CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeLeave No Find DstUserID:%d",pLeave->dwUserID);
						return;
					}

					pDstUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_LEAVEINFO,(CT_CHAR*)&LeaveInfo,sizeof(LeaveInfo));
				}
			}
		}

		CT_INT32 iRoomID=RoomInfo.iRoomID;
		memset(&RoomInfo,0,sizeof(RoomInfo));
		if(!CCTeamDuplicate::getSingleton().GetRoomInfo(iRoomID,RoomInfo))
		{
			return;
		}

		CMD_G_TeamDuplicateRoomInfo RoomData={0};
		RoomData.dwUserID=pLeave->dwUserID;
		RoomData.dwRoomMasterID=RoomInfo.dwUserID;
		RoomData.iRoomID=iRoomID;
		RoomData.iSceneID=RoomInfo.iSceneID;
		RoomData.iDifficulty=RoomInfo.iDifficulty;
		memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
		memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if(RoomInfo.FightingData[i].dwUserID == 0)continue;

			if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
			{
				CCUser *pDstUser=0;
				if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
				{
					CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeLeave No Find UserID:%d",RoomInfo.FightingData[i].dwUserID);
					continue;
				}

				PTRLinkUserData ud={0};
				UserFightingStatus us={0};
				pDstUser->GetPTRLinkUserData(ud);
				G_GetLookUserInfo(ud,us,RoomData.UserInfo[i]);
			}
			else
			{
				memcpy(&RoomData.UserInfo[i],&RoomInfo.AutoMatchUser[i],sizeof(RoomData.UserInfo[i]));
			}
		}

		CCTeamDuplicate::getSingleton().SendRoomMessage(iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));

		return;
	}
	else
	{
		if(pUser != NULL)
			pUser->CloseNet();
		return;
	}
	return;
}

//进入组队副本
CT_VOID CCTeamDuplicateEvent::DiposeEnter(CT_CHAR* pBuf,CT_DWORD len,CT_INT32 iSubID)
{
	if(len != sizeof(CMD_G_TeamDuplicateEnter))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DiposeEnter len != sizeof(CMD_G_TeamDuplicateEnter)");
		return;
	}

	CMD_G_TeamDuplicateEnter *pEnter=(CMD_G_TeamDuplicateEnter*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pEnter->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DiposeEnter No Find UserID:%d",pEnter->dwUserID);
		return;
	}

	CMD_G_TeamDuplicateEnterRoom RoomInfo={0};
	CT_CHAR cBuf[MAX_BUFFER]={0};
	RoomInfo.iSceneLen=MAX_BUFFER-sizeof(RoomInfo);
	UpdataScene(pUser->GetUserID(),0,0);
	pUser->m_userTeamDuplicate.GetSceneData(cBuf+sizeof(RoomInfo),RoomInfo.iSceneLen);

	RoomInfo.iRoomLen=MAX_BUFFER-sizeof(RoomInfo)-RoomInfo.iSceneLen;
	CCTeamDuplicate::getSingleton().GetRoomList(cBuf+sizeof(RoomInfo)+RoomInfo.iSceneLen,RoomInfo.iRoomLen);

	memcpy(cBuf,&RoomInfo,sizeof(RoomInfo));

	if(SUB_G_TEAMDUPLICATE_UPDATROOM == iSubID)
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_UPDATROOMINFO,cBuf,RoomInfo.iSceneLen+RoomInfo.iRoomLen+sizeof(RoomInfo));
	else
		pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMLIST,cBuf,RoomInfo.iSceneLen+RoomInfo.iRoomLen+sizeof(RoomInfo));
	
	return;
}

//拒绝邀请
CT_VOID CCTeamDuplicateEvent::DiposeRefuse(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateRefuse))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DiposeRefuse len != sizeof(CMD_G_TeamDuplicateRefuse)");
		return;
	}

	CMD_G_TeamDuplicateRefuse *pRefuse=(CMD_G_TeamDuplicateRefuse*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pRefuse->dwDstID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DiposeRefuse No Find UserID:%d",pRefuse->dwDstID);
		return;
	}

	pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_REFUSE,pBuf,len);
	return;
}

//战斗结束
CT_VOID CCTeamDuplicateEvent::DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_TeamDuplicate_Fighting))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeFightingEnd len != sizeof(MSC_TeamDuplicate_Fighting)");
		return;
	}

	MSC_TeamDuplicate_Fighting *pFighting=(MSC_TeamDuplicate_Fighting*)pBuf;

	TeamDuplicateRoomInfo RoomInfo={0};
	if(!CCTeamDuplicate::getSingleton().GetRoomInfo(pFighting->dwUserID,RoomInfo))
	{
		return;
	}

	CCTeamDuplicate::getSingleton().ClearUserStatus(RoomInfo.iRoomID);

	memset(&RoomInfo,0,sizeof(RoomInfo));
	if(!CCTeamDuplicate::getSingleton().GetRoomInfo(pFighting->dwUserID,RoomInfo))
	{
		return;
	}

	//组队副本奖励
	if(WINER_LIFT == pFighting->iWiner)
	{
		for(CT_INT32 i=0;i<TEAM_NUM;i++)
		{
			CCUser *pDstUser=0;
			if(RoomInfo.FightingData[i].dwUserID == 0 ||
				RoomInfo.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE)continue;
			if(CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
			{
				TeamDuplicateReward(pDstUser,RoomInfo.iSceneID,RoomInfo.iDifficulty);
			}
		}
	}

	CMD_G_TeamDuplicateRoomInfo RoomData={0};
	RoomData.dwUserID=pFighting->dwUserID;
	RoomData.dwRoomMasterID=RoomInfo.dwUserID;
	RoomData.iRoomID=RoomInfo.iRoomID;
	RoomData.iSceneID=RoomInfo.iSceneID;
	RoomData.iDifficulty=RoomInfo.iDifficulty;
	memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
	memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		if(RoomInfo.FightingData[i].dwUserID == 0)continue;

		if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
		{
			CCUser *pDstUser=0;
			if(!CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
			{
				CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeFightingEnd No Find UserID:%d",RoomInfo.FightingData[i].dwUserID);
				continue;
			}

			PTRLinkUserData ud={0};
			UserFightingStatus us={0};
			pDstUser->GetPTRLinkUserData(ud);
			G_GetLookUserInfo(ud,us,RoomData.UserInfo[i]);

			//写入数据库
			if(pFighting->iWiner == WINER_LIFT)
			{
				pDstUser->m_userTeamDuplicate.UpdataScene(RoomInfo.iSceneID,RoomInfo.iDifficulty,pFighting->iWiner);
				UpdataScene(RoomInfo.FightingData[i].dwUserID,RoomInfo.iSceneID,RoomInfo.iDifficulty);

				//更新客户端
				CMD_G_TeamDuplicateEnterRoom EnterRoom={0};
				CT_CHAR cBuf[MAX_BUFFER]={0};
				EnterRoom.iSceneLen=MAX_BUFFER-sizeof(EnterRoom);
				pDstUser->m_userTeamDuplicate.GetSceneData(cBuf+sizeof(EnterRoom),EnterRoom.iSceneLen);
				memcpy(cBuf,&EnterRoom,sizeof(EnterRoom));

				pDstUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_UPDATROOMINFO,cBuf,EnterRoom.iSceneLen+sizeof(RoomInfo));

				CMD_G_TaskEventInfo taskEventInfo = {0};
				taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_DUPLICATE;
				taskEventInfo.iTaskSubType = TASK_SUB_TYPE_DUPLICATE_POINTOR;
				taskEventInfo.iSubType = (RoomInfo.iDifficulty+1)*1000+(RoomInfo.iSceneID+1);
				taskEventInfo.iLevel = 0;
				pDstUser->m_userTask.DisposeTask(taskEventInfo,1);
			}
		}
		else
		{
			memcpy(&RoomData.UserInfo[i],&RoomInfo.AutoMatchUser[i],sizeof(RoomData.UserInfo[i]));
		}
	}

	CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));

	return;
}

//自动匹配
CT_VOID CCTeamDuplicateEvent::DisposeAutoMatchInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_DB_TeamDuplicateUserInfo))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeAutoMatch len != sizeof(MSC_DB_TeamDuplicateUserInfo)");
		return;
	}

	MSC_DB_TeamDuplicateUserInfo *pUserInfo=(MSC_DB_TeamDuplicateUserInfo*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pUserInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeAutoMatchInfo No Find UserID:%d",pUserInfo->dwUserID);
		return;
	}

	TeamDuplicateRoomInfo RoomInfo={0};
	CCTeamDuplicate::getSingleton().GetRoomInfo(pUserInfo->dwUserID,RoomInfo);

	//匹配失败
	if(pUserInfo->UserFightinginfo[0].dwUserID == 0 && pUserInfo->UserFightinginfo[1].dwUserID == 0)
	{
		CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_AUTOMATCHFAIL);
		return;
	}


	for(CT_INT32 i=0;i<TEAM_NUM-1;++i)
	{
		CCTeamDuplicate::getSingleton().AutoMatch(RoomInfo.iRoomID,pUserInfo->UserFightinginfo[i],pUserInfo->LookUserinfo[i]);
	}

	memset(&RoomInfo,0,sizeof(RoomInfo));
	CCTeamDuplicate::getSingleton().GetRoomInfo(pUserInfo->dwUserID,RoomInfo);

	CMD_G_TeamDuplicateRoomInfo RoomData={0};
	RoomData.dwUserID=pUserInfo->dwUserID;
	RoomData.dwRoomMasterID=RoomInfo.dwUserID;
	RoomData.iRoomID=RoomInfo.iRoomID;
	RoomData.iSceneID=RoomInfo.iSceneID;
	RoomData.iDifficulty=RoomInfo.iDifficulty;
	memcpy(RoomData.iFightingTime,RoomInfo.iFightingTime,sizeof(RoomData.iFightingTime));
	memcpy(RoomData.iStatus,RoomInfo.iStatus,sizeof(RoomData.iStatus));

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		CCUser *pDstUser=0;
		if(CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
		{
			PTRLinkUserData ud={0};
			UserFightingStatus us={0};
			pDstUser->GetPTRLinkUserData(ud);
			G_GetLookUserInfo(ud,us,RoomData.UserInfo[i]);
		}
		else if(RoomInfo.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_TRUE)
			memcpy(&RoomData.UserInfo[i],&RoomInfo.AutoMatchUser[i],sizeof(RoomData.UserInfo[i]));
		else
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeAutoMatchInfo RoomData.UserInfo=NULL");
		}
	}

	CCTeamDuplicate::getSingleton().SendRoomMessage(RoomInfo.iRoomID,MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMINFO,(CT_CHAR*)&RoomData,sizeof(RoomData));
	return;
}

//解锁房间
CT_VOID CCTeamDuplicateEvent::DisposeClear(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_TeamDuplicateClear))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeClear len != sizeof(CMD_G_TeamDuplicateClear)");
		return;
	}

	CMD_G_TeamDuplicateClear *pClear=(CMD_G_TeamDuplicateClear*)pBuf;

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_TEAMDUPLICATE_CLEAR,&vr))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeClear G_GetValueRange VALUE_TYPE_TEAMDUPLICATE_CLEAR");
		return;
	}

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pClear->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::DisposeClear No Find UserID:%d",pClear->dwUserID);
		return;
	}

	if(pClear->dwUserID != vr.fMaxVal)
	{
		pUser->CloseNet();
		return;
	}

	TeamDuplicateRoomInfo RoomInfo={0};
	if(!CCTeamDuplicate::getSingleton().GetRoomInfo(pClear->dwUserID,RoomInfo))
	{
		return;
	}

	CCTeamDuplicate::getSingleton().ClearRoom(RoomInfo.iRoomID);

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		CCUser *pDstUser=0;
		if(CCUserManager::getSingleton().Find(RoomInfo.FightingData[i].dwUserID,&pDstUser))
		{
			CMD_G_TeamDuplicateLeave LeaveInfo={0};
			LeaveInfo.dwUserID=RoomInfo.FightingData[i].dwUserID;
			LeaveInfo.dwDstID=RoomInfo.FightingData[i].dwUserID;
			pDstUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_LEAVEINFO,(CT_CHAR*)&LeaveInfo,sizeof(LeaveInfo));
		}
	}

	return;
}

//更新场景
CT_VOID CCTeamDuplicateEvent::UpdataScene(CT_DWORD dwUserID,CT_INT32 iScene,CT_INT32 iDifficlty)
{
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicateEvent::UpdataScene No Find UserID:%d",dwUserID);
		return;
	}

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	for(CT_INT32 i=iScene;i<TEAMDUPLICATE_SCENENUM;++i)
	{
		for(CT_INT32 j=iDifficlty;j<TEAMDUPLICATE_DIFFICULTY;++j)
		{
			G_TeamDuplicate TeamDuplicate={0};
			if(!G_GetTeamDuplicate(i,j,&TeamDuplicate))
			{
				continue;
			}

 			if(TeamDuplicate.iOpenGrade <= mvd.stGrade.iLevel && pUser->m_userTask.ViewCompleteTask(TeamDuplicate.iOpenTask) && 
				(pUser->m_userTeamDuplicate.VaildateScene(TeamDuplicate.iOpenSceneID-1,TeamDuplicate.iOpenDifficulty-1,CT_TRUE) || 
				(TeamDuplicate.iOpenSceneID == 0 && TeamDuplicate.iOpenDifficulty == 0)))
			{
				if(pUser->m_userTeamDuplicate.SetSceneData(i,j))
				{
					//写入数据库
					MSC_DB_TeamDuplicateScene Scene={0};
					Scene.dwUserID=pUser->GetUserID();
					Scene.iScnenID=i;
					Scene.iDifficulty=j;
					Scene.iWin=WINER_NULL;
					CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
					G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_TEAMDUPLICATE_SCENE,(CT_CHAR*)&Scene,sizeof(Scene));

				}
			}
		}
	}
	return;
}

//组队副本奖励
CT_VOID CCTeamDuplicateEvent::TeamDuplicateReward(CCUser *pUser,const CT_INT32 iSceneID,const CT_INT32 iDifficulty)
{
	//领取奖励
	CMD_G_TeamDuplicateReward Reward={0};
	G_TeamDuplicate Team={0};
	G_SceneReward SceneReward={0};
	if(!G_GetTeamDuplicate(iSceneID,iDifficulty,&Team))
	{
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::TeamDuplicateReward G_GetTeamDuplicate SceneID:%d",iSceneID+1);
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		return;
	}

	if(!CCFightingScriptSetting::getSingleton().GetSceneReward(iSceneID,Team.iRewardID,SceneReward))
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::TeamDuplicateReward GetSceneReward SceneID:%d Reward:%d",iSceneID+1,Team.iRewardID);
		return;
	}

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_TEAMDUPLICATE_FIGHTINGNUM,&vr))
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_FIGHTING_ERROR);
		CCLogMeg::getSingleton().WriteError("CCFightingEvnet1::TeamDuplicateReward G_GetValueRange VALUE_TYPE_TEAMDUPLICATE_FIGHTINGNUM");
		return;
	}

	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);


	if(MasterInfo.mvd.stRes.iTeamDuplicateFightingTimes < vr.fMaxVal)
	{
		//记录副本奖励到日志数据库
		MSC_LOGDB_TeamDuplicateRecord  dbTD={0};
		dbTD.dwUserID=pUser->GetUserID();
		dbTD.iFid = iSceneID;
		dbTD.iDifficulty=iDifficulty;
		dbTD.iUserGrade = MasterInfo.mvd.stGrade.iLevel;

		MasterVariableData MasterReward={0};
		MasterReward.stRes.iDiamod+=SceneReward.iDiamond;
		MasterReward.stRes.iGold+=G_GetFightingGold(SceneReward.iGold,MasterInfo.factor.fBattleFactor);
		MasterReward.stRes.iSkillSoul+=SceneReward.iSkillPrint;
		MasterReward.stRes.iTalentNum+=SceneReward.iPotential;
		MasterReward.stGrade.iCurrentExp+=G_GetFightingExperience(SceneReward.iExperience,MasterInfo.factor.fBattleFactor);
		MasterReward.stRes.iTeamDuplicateFightingTimes+=1;

		dbTD.iTalent=SceneReward.iPotential;
		dbTD.iDiamod=SceneReward.iDiamond;
		dbTD.iGold=G_GetFightingGold(SceneReward.iGold,MasterInfo.factor.fBattleFactor);
		dbTD.iSkillNum=SceneReward.iSkillPrint;
		dbTD.iXp=G_GetFightingExperience(SceneReward.iExperience,MasterInfo.factor.fBattleFactor);
		dbTD.iWin=WINER_LIFT;

		// 更新领主资源
		MasterVariableData addValue = {0};
		pUser->m_userBase.AddMastData(MasterReward, SAVETAG_TEAMDUOLICATE_REWARD);


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
					dbTD.iPropID[iGoodsCount1]=iGoods1[j];
					dbTD.iPropNum[iGoodsCount1]=iCount1[j];
				}
				iGoodsCount1++;

				CT_INT32 iCount1J = iCount1[j];
				CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				pUser->m_userItem.AutoAddItemData(SAVETAG_TEAMDUOLICATE_REWARD,iGoods1[j],iCount1[j]);

				if (G_BroadcastFilter(2, iGoods1[j]))
				{
					CT_INT32 iDupID = (iDifficulty)*1000+(iSceneID);
					MasterData masterData = {0};
					RollBroadcast rb[3]={0};
					pUser->m_userBase.GetMasterData(masterData);
					rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
					rb[1].SetValue(iDupID, "");
					rb[2].SetValue(iGoods1[j], "");
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{4}<4:%d>{5}<2:%d>", pUser->GetUserID(), masterData.cNickName,iDupID,iGoods1[j]);
					if (iCount1[j] > 1)
					{
						//CT_CHAR cNum[10] = {0};
						RollBroadcast rb1[4]={0};
						//sprintf(cNum, "x<6:%d>",iCount1J);
						//strcat(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), cNum);

						rb1[0].SetValue(pUser->GetUserID(), masterData.cNickName);
						rb1[1].SetValue(iDupID, "");
						rb1[2].SetValue(iGoods1[j], "");
						rb1[3].SetValue(iCount1J, "");
						G_SendRollBroadecast(120,3,60,(CT_CHAR*)rb1,sizeof(rb1));
					}
					else
						G_SendRollBroadecast(120,3,60,(CT_CHAR*)rb,sizeof(rb));
				}
			}

			if(iGoods2[j] != 0 && iCount2[j] != 0)
			{
				Reward.iGoods[iGoodsCount1]=iGoods2[j];
				Reward.iGoodsCount[iGoodsCount1]=iCount2[j];
				if (iGoodsCount1<PVE_GOODS_COUNT)
				{
					dbTD.iPropID[iGoodsCount1]=iGoods2[j];
					dbTD.iPropNum[iGoodsCount1]=iCount2[j];
				}
				iGoodsCount1++;

				CT_INT32 iCount2J = iCount2[j];
				CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
				pUser->m_userItem.AutoAddItemData(SAVETAG_TEAMDUOLICATE_REWARD,iGoods2[j],iCount2[j]);

				if (G_BroadcastFilter(2, iGoods2[j]))
				{
					CT_INT32 iDupID = (iDifficulty)*1000+(iSceneID);
					MasterData masterData = {0};
					RollBroadcast rb[3]={0};
					pUser->m_userBase.GetMasterData(masterData);
					rb[0].SetValue(pUser->GetUserID(), masterData.cNickName);
					rb[1].SetValue(iDupID, "");
					rb[2].SetValue(iGoods2[j], "");
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{4}<4:%d>{5}<2:%d>", pUser->GetUserID(), masterData.cNickName,iDupID,iGoods2[j]);
					if (iCount2[j] > 1)
					{
						//CT_CHAR cNum[10] = {0};
						//sprintf(cNum, "x<6:%d>",iCount2J);
						//strcat(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), cNum);
						RollBroadcast rb1[4]={0};
						rb1[0].SetValue(pUser->GetUserID(), masterData.cNickName);
						rb1[1].SetValue(iDupID, "");
						rb1[2].SetValue(iGoods2[j], "");
						rb1[3].SetValue(iCount2J, "");
						G_SendRollBroadecast(120,3,60,(CT_CHAR*)rb1,sizeof(rb1));
					}
					else
						G_SendRollBroadecast(120,3,60,(CT_CHAR*)rb,sizeof(rb));
				}
			}
		}

		Reward.iPotential=SceneReward.iPotential;
		Reward.iDiamond=SceneReward.iDiamond;
		Reward.iGold=G_GetFightingGold(SceneReward.iGold,MasterInfo.factor.fBattleFactor);
		Reward.iSkillPoint=SceneReward.iSkillPrint;
		Reward.iExperience=G_GetFightingExperience(SceneReward.iExperience,MasterInfo.factor.fBattleFactor);

		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_TEAMDUPLICATE_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));

		//记录日志数据库
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TEAMDUPLICATEREWARD,(CT_CHAR*)&dbTD,sizeof(dbTD));
	}
	else
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTING_EVENT,SUB_G_TEAMDUPLICATE_REWARD,(CT_CHAR*)&Reward,sizeof(Reward));
	}
	return;
}
