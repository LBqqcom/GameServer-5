#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
//#include "GlobeScriptInterface.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CGlobeData.h"
#include "CUserManager.h"
#include "CFightTeamManager.h"
#include "CFightTeamEvent.h"
#include "CResourceGrabMgr.h"
#include "CCSFightData.h"
#include "CMD_CrossServer.h"
#include "CBroadcastScriptSetting.h"
#include <math.h>

#define MAX_MES_MAX_BUFF_LEN		10*1024

CCFightTeamEvent::CCFightTeamEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE);					//创建战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST);					//申请加入战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE);					//同意加入申请
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_DISAGREE);				//不同意加入申请
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE);					//邀请玩家加入战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ACCEPT);					//接受邀请加入战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REJECT);					//拒绝被邀请加入战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CONTRIBUTE);				//捐献
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_TRANSFER);				//转让首领
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_QUIT);					//退出战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ELITE);					//设置战队精英
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SET_DESC);				//设置战队介绍
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SEARCH);					//查询战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RECRUIT);					//发布招募
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RECRUIT_SURE);			//确认发布招募
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RECRUIT_LIST);			//查看招募
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_DISBAND);					//解散战队
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_LOOKEVENT);				//查看战队动态
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_LOOKFIGHT);				//查看战队战斗
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_FIRE);				//踢人

	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER);					//查看成员列表
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_LIST);					//查看战队列表
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQ_LIST);				//队长查看玩家申请列表
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE_LIST);				//玩家查看邀请列表
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_PLAY_LIST);				//浏览玩家列表

	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_UP_GRADE);					//战队升级
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SCIENCE_UP_GRADE);			//成员科技升级
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_BOSS);				//战队召唤
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_HELP_SUMMON);				//召唤帮助
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_REWARD);			//召唤奖励
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_FIGHTING);			//召唤战斗
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_HELP_SUMMON_LEAVE);		//召唤离开
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_INVIT);				//召唤邀请
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_SUMMON);			//时限BOOS召唤
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_ADD);			//时限BOSS加入
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_INVITE);			//时限BOOS邀请
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_BEGIN);			//时限BOOS开始
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_FIGHT);			//时限BOOS战斗
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_LEAVE);			//时限BOOS离开
	RegistMessage(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_REWARD);			//时限BOOS奖励

	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_DB_FIGHTTEAMMEMBER_FIND);
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_DB_FIGHTTEAMFIGHTINGDATA);
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FIGHTTEAM_SUMMON_FIGHTING);
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FIGHTTEAM_SUMMON_LEAVE);			//召唤离开
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FIGHTTEAM_TOWER_CP);
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FT_TIME_BOSS);
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FT_TIME_BOSS_LEAVE);

	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT, MSC_SUB_DB_FIGHTTEAM_MEM_REFRESH);     //pengwen 解决在

																				   // CCUserBase::MasterUpgrage(bool) 调用 CCFightTeamManager::FightTeamMemberRef(unsigned int, int, int) 死锁问题
	
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FINGTTEAM_UPGRADE_TASK);			//处理战队任务升级相关	
	RegistMessage(MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_CHANGE_FTNAME); 

	if(GAME_THREAD_CITY == m_dwThread)
	{
		RegistTimer(IDI_FIGHTTEAM_RECRUIT);
		SetTimer(IDI_FIGHTTEAM_RECRUIT,  TIME_FIGHTTEAM_RECRUIT_SPOT);
	}
}

CCFightTeamEvent::~CCFightTeamEvent(void)
{
}

//处理用户消息
CT_VOID CCFightTeamEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_FIGHTTEAM_EVENT:
		{
			switch (subID)
			{
			case SUB_G_FIGHTTEAM_CREATE:
				DisposeFightTeamCreate(pBuf,len);
				break;
			case SUB_G_FIGHTTEAM_REQUEST:
				DisposeFightTeamRequest(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_AGREE:
				DisposeFightTeamAgree(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_DISAGREE:
				DisposeFightTeamDisagree(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_INVITE:
				DisposeFightTeamInvite(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_ACCEPT:
				DisposeFightTeamAccept(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_REJECT:
				DisposeFightTeamReject(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_CONTRIBUTE:
				DisposeFightTeamContribute(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_TRANSFER:
				DisposeFightTeamTransfer(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_QUIT:
				DisposeFightTeamQuit(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_ELITE:
				DisposeFightTeamElite(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_SET_DESC:
				DisposeFightTeamDesc(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_SEARCH:
				DisposeFightTeamSearch(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_RECRUIT:
				DisposeFightTeamRecruit(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_RECRUIT_SURE:
				DisposeFightTeamRecruitSure(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_RECRUIT_LIST:
				DisposeFightTeamLookRecruit(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_DISBAND:
				DisposeFightTeamDisband(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_LOOKEVENT:
				DisposeFightTeamLookEvent(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_LOOKFIGHT:
				DisposeFightTeamLookFight(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_MEMBER_FIRE:
				DisposeFightTeamMemberFire(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_LIST:
				DisposeFightTeamList(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_REQ_LIST:
				DisposeFightTeamReqList(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_INVITE_LIST:
				DisposeFightTeamInviteList(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_PLAY_LIST:
				DisposeFightTeamPlayList(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_MEMBER:
				DisposeFightTeamMember(pBuf, len);
				break;
			case SUB_G_FIGHTTEAM_UP_GRADE:
				DiposeFightTeamUpGrade(pBuf,len);
				break;
			case SUB_G_FIGHTTEAM_SCIENCE_UP_GRADE:
				DiposeFightTeamNemberScienceUp(pBuf,len);
				break;
			case SUB_G_FIGHTTEAM_SUMMON_BOSS:
				{
					DisposeFightTeamSummonBoss(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_HELP_SUMMON:
				{
					DisposeFightTeamHelpSummon(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_SUMMON_REWARD :
				{
					DisposeFightTeamSummonReward(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_SUMMON_FIGHTING:
				{
					DisposeFightTeamSummonFighting(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_HELP_SUMMON_LEAVE:
				{
					DiposeFightTeamHelpSummonLeave(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_SUMMON_INVIT:
				{
					DisposeFightTeamInvit(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_TIMEBOSS_SUMMON:
				{
					DisposeFTTBSummon(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_TIMEBOSS_ADD:
				{
					DisposeFTTBAdd(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_TIMEBOSS_INVITE:
				{
					DisposeFTTBInvite(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_TIMEBOSS_BEGIN:
				{
					DisposeFTTBBegin(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_TIMEBOSS_FIGHT:
				{
					DisposeFTTBFight(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_TIMEBOSS_LEAVE:
				{
					DisposeFTTBLeave(pBuf,len);
					break;
				}
			case SUB_G_FIGHTTEAM_TIMEBOSS_REWARD:
				{
					DisposeFTTBReward(pBuf,len);
					break;
				}
			default:
				CCLogMeg::getSingleton().WriteError("OnUserEvent subID %d",subID);
				break;
			}
			break;
		}
	case MSC_MAIN_FIGHTTEAM_EVENT:
		switch (subID)
		{
		case MSC_SUB_DB_FIGHTTEAMMEMBER_FIND:
			DisposeFightTeamInviteFind(pBuf, len);
			break;
		case MSC_SUB_DB_FIGHTTEAMFIGHTINGDATA:
			DisposeFightTeamFightingData(pBuf, len);
			break;
		case MSC_SUB_DB_FIGHTTEAM_MEM_REFRESH: //彭文添加
			{
				DiposeFightTeamMemeRefresh(pBuf, len);
				break;
			}
		case MSC_SUB_FIGHTTEAM_SUMMON_FIGHTING:
			{
				DisposeFightTeamSummonFightingEnd(pBuf,len);
				break;
			}
		case MSC_SUB_FIGHTTEAM_SUMMON_LEAVE:
			{
				DiposeFightTeamHelpSummonLeave(pBuf,len);
				break;
			}
		case MSC_SUB_FIGHTTEAM_TOWER_CP:
			{
				DiposeFightTeamTowerCP(pBuf,len);
				break;
			}
		case MSC_SUB_FINGTTEAM_UPGRADE_TASK:
			{
				DisposeFightTeamUpgreadTask(pBuf,len);
				break;
			}
		case MSC_SUB_CHANGE_FTNAME:
			{
				DisposeChangeFTName(pBuf,len);
				break;
			}
		case MSC_SUB_FT_TIME_BOSS:
			{
				DisposeFTTBFightReturn(pBuf,len);
				break;
			}
		case MSC_SUB_FT_TIME_BOSS_LEAVE:
			{
				DisposeFTTBLeave(pBuf,len);
				break;
			}
		default:
			CCLogMeg::getSingleton().WriteError("OnUserEvent subID %d",subID);
			break;
		}
		break;

	default:
		CCLogMeg::getSingleton().WriteError("OnUserEvent mainid %d",mainID);
		break;
	}
	return;
}

//用户定时器处理
CT_VOID CCFightTeamEvent::OnTime(CT_DWORD timeID)
{
	if (timeID == IDI_FIGHTTEAM_RECRUIT)
	{
		CCFightTeamManager::getSingleton().CheckFightTeamRecruit();
		return;
	}
	return;
}

// 创建战队
CT_VOID CCFightTeamEvent::DisposeFightTeamCreate(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Create))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamCreate: len != sizeof(CMD_G_FightTeam_Create)");
		return;
	}

	CMD_G_FightTeam_Create *pFightTeamCreate=(CMD_G_FightTeam_Create*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFightTeamCreate->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamCreate,can't find user %d",pFightTeamCreate->dwUserID);
		return;
	}

	CT_INT32 iRetCode = 0;
	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);	
	//检查是否已加入战队
	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamCreate->dwUserID) > 1)	
	{
		iRetCode = FT_HAVE_FT;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamCreate,user %d has join fight team",pFightTeamCreate->dwUserID);
		return;
	}

	//检查等级
	G_ValueRange vr={0};
	G_GetValueRange(VALUE_TYPE_FIGHTTEAM_GRADE,&vr);
	
	if (masterData.mvd.stGrade.iLevel < vr.fMaxVal)
	{
		iRetCode = FT_NO_GRADE;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//检查资源
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	G_GetValueRange(VALUE_TYPE_FIGHTTEAM_GOLD,&vr);
	if (mvd.stRes.iGold + vr.fMaxVal < 0)
	{
		iRetCode = FT_NO_GOLD;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}
	
	//检查战队名长度
	if (strlen(pFightTeamCreate->cFightTeamName) > 18)
	{
		iRetCode = FT_NAME_INVALID;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//检查战队名已存在
	CCFightTeam *pFightTeam = 0;
	if(CCFightTeamManager::getSingleton().Find(pFightTeamCreate->cFightTeamName, &pFightTeam))
	{
		iRetCode = FT_HAS_EXIST;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//读取脚本判断资源,等级
	G_FightTeamUpgrade ftug={0};
	if(!G_GetFightTeamUpgrade(1,FIGHTTEAM_BOSS_SUMMON,&ftug))
	{
		iRetCode = FT_DATA_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamCreate G_GetFightTeamUpgrade Grade:%d",1);
		return;
	}

	//离队惩罚
	CooldownData cd={0};
	pUser->m_userBase.GetCooldownData(FTNUMBERLEAVE_COOL,cd);
	if(cd.leftTime + cd.toggleTime >= time(0))
	{
		iRetCode = FT_LEAVE_TIME;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//到这里表示可以创建战队了
	CT_DWORD dwFTID = 0;
	memset(&mvd,0,sizeof(mvd));
	mvd.stRes.iGold = (CT_INT32)vr.fMaxVal;
	dwFTID = CCFightTeamManager::getSingleton().NewFightTeam(pUser, pFightTeamCreate->cFightTeamName, pFightTeamCreate->cDescription,ftug);
	pUser->m_userBase.AddMastData(mvd,SAVETAG_FIGHTTEAM_CREATE);
	memset(&mvd,0,sizeof(MasterVariableData));
	pUser->m_userBase.GetMasterData(mvd);

	if(CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
	{
		CMD_G_FightTeamBase fightTeamBase = {0};
		pFightTeam->GetFightTeamBase(&fightTeamBase);
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_BASE,(CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase));
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CREATE_SUC,(CT_CHAR*)&(mvd.stRes.iGold), sizeof(CT_INT32));
	}

	MSC_LOGDB_FightTeam logdbFightTeam = {0};
	logdbFightTeam.dwUserID = pFightTeamCreate->dwUserID;
	logdbFightTeam.iGoldCost = (CT_INT32)vr.fMaxVal;
	logdbFightTeam.iGrade = mvd.stGrade.iLevel;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + logdbFightTeam.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM,(CT_CHAR*)&logdbFightTeam,sizeof(MSC_LOGDB_FightTeam));

	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_PRODUCT;
	taskEventInfo.iSubType = 1001;				//跟小刚约定,创建/加入战队挂生产任务编号1001
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);

	// 判断加入/创建指定等级战队的任务
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_INFIGHTTEAM;
	taskSysInfo.iProgress = 1;
	pUser->m_userTask.DisposeTask(taskSysInfo);

}

// 申请加入战队 
CT_VOID CCFightTeamEvent::DisposeFightTeamRequest(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRequest: len != sizeof(CMD_G_FightTeam_ARG)");
		return;
	}

	CT_INT32 iRetCode = 0;
	CMD_G_FightTeam_ARG *pFightTeamArg=(CMD_G_FightTeam_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFightTeamArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRequest,can't find user %d",pFightTeamArg->dwUserID);
		return;
	}

	//检查是否已加入战队
	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamArg->dwUserID) > 1)
	{
		iRetCode = FT_HAVE_FT;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRequest,user %d has join fight team",pFightTeamArg->dwUserID);
		return;
	}

	//离队惩罚
	CooldownData cd={0};
	pUser->m_userBase.GetCooldownData(FTNUMBERLEAVE_COOL,cd);
	if(cd.leftTime+ cd.toggleTime >= time(0))
	{
		iRetCode = FT_LEAVE_TIME;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	MasterData masterData = {0};
	pUser->m_userBase.GetMasterData(masterData);

	//检查等级
	G_ValueRange vr={0};
	G_GetValueRange(VALUE_TYPE_FIGHTTEAM_GRADE,&vr);
	if (masterData.mvd.stGrade.iLevel < vr.fMinVal)
	{
		iRetCode = FT_NO_GRADE;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//检查战队是否存在
	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamArg->cFightTeamName, &pFightTeam))
	{
		iRetCode = FT_NO_FIND;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//检查战队是否已满员
	if (pFightTeam->CheckFightTeamFull())
	{
		iRetCode = FT_FULL;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//到这里可以发出申请了
	CT_DWORD dwFTID = pFightTeam->GetFightTeamID();
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST_S, (CT_CHAR*)&dwFTID, sizeof(CT_DWORD));

	CMD_G_FightTeamMember fightTeamMember = {0};
	fightTeamMember.dwFTID = pFightTeam->GetFightTeamID();

	fightTeamMember.dwUserID = pFightTeamArg->dwUserID;
	memcpy(fightTeamMember.cNickName, masterData.cNickName, NAME_SIZE);
	fightTeamMember.iFightingPower = masterData.mvd.stRes.iFightingPower;
	fightTeamMember.iOccupation = masterData.iOccupation;
	fightTeamMember.iGrade = masterData.mvd.stGrade.iLevel;
	fightTeamMember.iRank = masterData.mvd.stRank.iLevel;
	fightTeamMember.iTime = time(0);
	fightTeamMember.iLastExitTime = time(0);
	fightTeamMember.iTitleID = pUser->m_userTitle.GetUseTitleID();
	pFightTeam->InsertMember(2, fightTeamMember);

	// 通知队长审核
	CT_DWORD dwBossID = 0;
	dwBossID = pFightTeam->GetFightTeamBossID();
	CCUser *pBossUser=0;
	if (CCUserManager::getSingleton().Find(dwBossID,&pBossUser))
	{
		pBossUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQUEST, (CT_CHAR*)&fightTeamMember, sizeof(CMD_G_FightTeamMember));
	}

}

// 同意申请		1.加入队员 2.删除请求和邀请记录 3.通知成员 4.发战队信息给新成员
CT_VOID CCFightTeamEvent::DisposeFightTeamAgree(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Employ))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree: len != sizeof(CMD_G_FightTeam_Employ)");
		return;
	}

	CMD_G_FightTeam_Employ *pFightTeamEmploy=(CMD_G_FightTeam_Employ*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFightTeamEmploy->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find user %d",pFightTeamEmploy->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamEmploy->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamEmploy->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != pFightTeamEmploy->dwUserID)
	{
		return;
	}

	CT_INT32 iRetCode = 0;
	// 战队已满
	if (pFightTeam->CheckFightTeamFull())
	{
		pFightTeam->DeleteMember(2, pFightTeamEmploy->dwPlayerID);
		iRetCode = FT_FULL;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//检查是否已加入战队
	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamEmploy->dwPlayerID) > 1)
	{
		pFightTeam->DeleteMember(2, pFightTeamEmploy->dwPlayerID);
		iRetCode = FT_HAVE_FT;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//离队惩罚
	CCUser *pPlayUser = 0;
	if (CCUserManager::getSingleton().Find(pFightTeamEmploy->dwPlayerID, &pPlayUser))
	{
		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_TEAM_LEAVE_TEAM_TIME,&vr))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamAgree VALUE_TYPE_TEAM_LEAVE_TEAM_TIME");
			iRetCode = FT_DATA_ERROR;
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
			return;
		}
		CooldownData cd={0};
		pPlayUser->m_userBase.GetCooldownData(FTNUMBERLEAVE_COOL,cd);
		if(cd.leftTime > 0)
		{
			iRetCode = FT_LEAVE_TIME;
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
			return;
		}
	}

	//MasterDataEx md={0};
	//pUser->m_userBase.GetMasterDataEx(md);
	//if(vr.fMaxVal+ md.iLeaveTeamTime >= time(0))
	//{
	//	iRetCode = FT_LEAVE_TIME;
	//	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
	//	return;
	//}

	//现在可以加入战队了
	CMD_G_FightTeamMember fightTeamMember = {0};
	pFightTeam->GetFightTeamMember(2, pFightTeamEmploy->dwPlayerID, &fightTeamMember);
	fightTeamMember.iTime = time(0);
	if (CCUserManager::getSingleton().Find(pFightTeamEmploy->dwPlayerID))
	{
		fightTeamMember.iStatus = 2;
	}
	if (pFightTeam->InsertMember(1, fightTeamMember) > 0)
	{
		CCFightTeamManager::getSingleton().InsertUserMap(fightTeamMember.dwUserID, pFightTeamEmploy->dwFTID);
	}
	CCFightTeamManager::getSingleton().ClearReqRsp(pFightTeamEmploy->dwPlayerID);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE_S, (CT_CHAR*)&(pFightTeamEmploy->dwPlayerID), sizeof(CT_DWORD));

	// 通知各成员
	for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (it->first != pFightTeamEmploy->dwPlayerID)
		{
			if (CCUserManager::getSingleton().Find(it->first,&pMUser))
			{
				pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_ADD,(CT_CHAR*)&fightTeamMember, sizeof(CMD_G_FightTeamMember));
			}
		}
	}

	// 通知新成员
	pPlayUser = 0;
	if (CCUserManager::getSingleton().Find(pFightTeamEmploy->dwPlayerID, &pPlayUser))
	{
		CMD_G_FightTeamBase fightTeamBase = {0};
//		CT_CHAR cBuf[1024] = {0};
//		CT_DWORD dwLen = 1024;
		pFightTeam->GetFightTeamBase(&fightTeamBase);
//		pFightTeam->GetFightTeamMember(cBuf, dwLen);
		pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_AGREE,(CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase));
//		pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER, cBuf, dwLen);
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_PRODUCT;
		taskEventInfo.iSubType = 1001;				//跟小刚约定,创建/加入战队挂生产任务编号1001
		taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
		pPlayUser->m_userTask.DisposeTask(taskEventInfo);

		// 判断加入/创建指定等级战队的任务
		CMD_G_TaskSystemInfo taskSysInfo = {0};
        taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
		taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_INFIGHTTEAM;
		taskSysInfo.iProgress = fightTeamBase.iGrade;
		pPlayUser->m_userTask.DisposeTask(taskSysInfo);

		//更新领主战队ID
		pPlayUser->m_userBase.SetFightTeamID(pFightTeamEmploy->dwFTID);

		//战队召唤boss
		CT_CHAR buf[MAX_MES_MAX_BUFF_LEN]={0};
		CT_INT32 bufLen=MAX_MES_MAX_BUFF_LEN;
		FightTeamSummonBoss sb={0};
		if(pFightTeam->GetSummonInfo(sb))
		{
			CMD_G_FightTeamHelpSummonInfo hsInfo={0};
			hsInfo.dwUserID=pUser->GetUserID();
			hsInfo.iErrorID=(sb.iWiner == FT_SUMMON_WIN?sb.iWiner:FT_SUMMON_SUC);
			hsInfo.iBossIndex=sb.iBossIndex;
			memcpy(hsInfo.dwHelpSummonID,sb.dwUserID,sizeof(hsInfo.dwHelpSummonID));
			pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SUMMON_BOSS_INFO, (CT_CHAR*)&hsInfo, sizeof(hsInfo));
		}

		//召唤奖励
		memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
		bufLen = MAX_MES_MAX_BUFF_LEN;
		if(pFightTeam->GetSummonRewardRes(pUser->GetUserID(), buf, bufLen))
		{
			pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SUMMON_REWARD_INFO, buf, bufLen);
		}


		//召唤奖励
		memset(buf, 0, MAX_MES_MAX_BUFF_LEN);
		bufLen = MAX_MES_MAX_BUFF_LEN;
		if (pFightTeam->GetSummonRewardItem(pUser->GetUserID(), buf, bufLen))
		{
			pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SUMMON_REWARD_ITEM, buf, bufLen);
		}

		//龙岛战队积分判断
		pPlayUser->m_userIsland.TeamIn(pFightTeamEmploy->dwFTID);
	}

	CMD_G_FightTeamEventLog fightTeamEventLog={0};
	CT_CHAR cEventDesc[DESCRIPTION_SIZE] = {0};
	CT_CHAR cName[NAME_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	RollBroadcast rb={0};
	fightTeamEventLog.iReport = 0;
	fightTeamEventLog.lDate = time(0);
	pFightTeam->GetMemberName(pFightTeamEmploy->dwPlayerID, cName);
	rb.SetValue(0,cName);
	//sprintf(cEventDesc, "<%s>{1}", cName);
	G_GetRollBroadecastData(103,cEventDesc,iEventDescLen,(CT_CHAR*)&rb,sizeof(rb));
	memcpy(fightTeamEventLog.cEventDesc, cEventDesc, DESCRIPTION_SIZE);
	pFightTeam->AddEventLog(fightTeamEventLog);
}

// 不同意申请		1.删除申请记录	2.通知对方
CT_VOID CCFightTeamEvent::DisposeFightTeamDisagree(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Employ))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDisagree: len != sizeof(CMD_G_FightTeam_Employ)");
		return;
	}

	CMD_G_FightTeam_Employ *pFightTeamEmploy=(CMD_G_FightTeam_Employ*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFightTeamEmploy->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDisagree,can't find user %d",pFightTeamEmploy->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamEmploy->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamEmploy->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != pFightTeamEmploy->dwUserID)
	{
		return;
	}

	pFightTeam->DeleteMember(2, pFightTeamEmploy->dwPlayerID);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_DISAGREE_S, (CT_CHAR*)&(pFightTeamEmploy->dwPlayerID), sizeof(CT_DWORD));
	

	// 通知被拒绝
	CCUser *pPlayUser = 0;
	if (CCUserManager::getSingleton().Find(pFightTeamEmploy->dwPlayerID, &pPlayUser))
	{
		pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_DISAGREE, pFightTeam->m_stBase.cFightTeamName, NAME_SIZE);
	}
}

// 邀请成员
CT_VOID CCFightTeamEvent::DisposeFightTeamInvite(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamMember_ARG))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInvite: len != sizeof(CMD_G_FightTeamMember_ARG)");
		return;
	}

	CMD_G_FightTeamMember_ARG *pFightTeamMemberArg=(CMD_G_FightTeamMember_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFightTeamMemberArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInvite,can't find user %d",pFightTeamMemberArg->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamMemberArg->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamMemberArg->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != pFightTeamMemberArg->dwUserID)
	{
		return;
	}

	CT_INT32 iRetCode = 0;
	// 战队已满
	if (pFightTeam->CheckFightTeamFull())
	{
		iRetCode = FT_FULL;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	CCUser *pPlayUser = 0;
	if(CCUserManager::getSingleton().Find(pFightTeamMemberArg->cNickName,&pPlayUser))		//在线
	{
		pFightTeamMemberArg->dwPlayerID = pPlayUser->GetUserID();

		CMD_G_FightTeamMember fightTeamMember = {0};
		MasterData masterData = {0};
		pPlayUser->m_userBase.GetMasterData(masterData);
		fightTeamMember.dwFTID = pFightTeamMemberArg->dwFTID;
		fightTeamMember.dwUserID = pFightTeamMemberArg->dwPlayerID;
		memcpy(fightTeamMember.cNickName, pFightTeamMemberArg->cNickName, NAME_SIZE);
		fightTeamMember.iFightingPower = masterData.mvd.stRes.iFightingPower;
		fightTeamMember.iOccupation = masterData.iOccupation;
		fightTeamMember.iGrade = masterData.mvd.stGrade.iLevel;
		fightTeamMember.iRank = masterData.mvd.stRank.iLevel;
		fightTeamMember.iTitleID = pPlayUser->m_userTitle.GetUseTitleID();
		DisposeFightTeamInviteFind((CT_CHAR*)&fightTeamMember, sizeof(CMD_G_FightTeamMember));
	}
	else
	{
		CT_DWORD threadID = GAME_THREAD_DATABASE + pFightTeamMemberArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAMMEMBER_FIND,pBuf,len);
	}
}

CT_VOID CCFightTeamEvent::DisposeFightTeamInviteFind(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamMember))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInviteFind: len != sizeof(CMD_G_FightTeamMember)");
		return;
	}

	CMD_G_FightTeamMember *pFightTeamMember=(CMD_G_FightTeamMember*)pBuf;

	CCFightTeam *pFightTeam = 0; 
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamMember->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInviteFind,can't find FightTeam %d",pFightTeamMember->dwFTID);
		return;
	}

	CCUser *pUser=0;
	CT_DWORD dwBossID = pFightTeam->GetFightTeamBossID();
	if (!CCUserManager::getSingleton().Find(dwBossID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInviteFind,can't find user %d",dwBossID);
		return;
	}
	CT_INT32 iRetCode = 0;

	//检查是否已加入战队
	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamMember->dwUserID) > 1)
	{
		iRetCode = FT_HAVE_FT;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	if (pFightTeamMember->dwUserID < 1 || pFightTeamMember->dwUserID == dwBossID)
	{
		iRetCode = FT_NO_PLAY;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
	}

	//到这里可以保存和发送邀请了.
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE_S, (CT_CHAR*)&(pFightTeamMember->dwUserID), sizeof(CT_DWORD));
	pFightTeamMember->iTime = time(0);
	pFightTeamMember->iLastExitTime = time(0);
	pFightTeam->InsertMember(3, *pFightTeamMember);

	//如果受邀请方在线
	CCUser *pPlayUser = 0;
	if(CCUserManager::getSingleton().Find(pFightTeamMember->dwUserID,&pPlayUser))
	{
		CMD_G_FightTeamInfo fightTeamInfo = {0};
		pFightTeam->GetFightTeamInfo(&fightTeamInfo);
		pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE,(CT_CHAR*)&fightTeamInfo, sizeof(CMD_G_FightTeamInfo));
	}
}

// 玩家同意邀请	
CT_VOID CCFightTeamEvent::DisposeFightTeamAccept(CT_CHAR* pBuf, CT_DWORD len)
{

	if(len != sizeof(CMD_G_FightTeam_Served))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAccept: len != sizeof(CMD_G_FightTeam_Served)");
		return;
	}

	CMD_G_FightTeam_Served *pFightTeamServed=(CMD_G_FightTeam_Served*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pFightTeamServed->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAccept,can't find user %d",pFightTeamServed->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamServed->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAccept,can't find FightTeam %d",pFightTeamServed->dwFTID);
		return;
	}

	//检查是否已加入战队
	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamServed->dwUserID) > 1)
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAccept,user %d has join fight team",pFightTeamServed->dwUserID);
		return;
	}

	CT_INT32 iRetCode = 0;
	// 战队已满
	if (pFightTeam->CheckFightTeamFull())
	{
		pFightTeam->DeleteMember(3, pFightTeamServed->dwUserID);
		iRetCode = FT_FULL;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ACCEPT_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//离队惩罚
	CooldownData cd={0};
	pUser->m_userBase.GetCooldownData(FTNUMBERLEAVE_COOL,cd);
	CT_INT32 iTime=time(0);
	if(cd.leftTime+ cd.toggleTime >= iTime)
	{
		iRetCode = FT_LEAVE_TIME;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ACCEPT_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//现在可以加入战队了
	CMD_G_FightTeamMember fightTeamMember = {0};
	pFightTeam->GetFightTeamMember(3, pFightTeamServed->dwUserID, &fightTeamMember);
	fightTeamMember.iTime = time(0);
	fightTeamMember.iStatus = 2;
	if (pFightTeam->InsertMember(1, fightTeamMember) > 0)
	{
		CCFightTeamManager::getSingleton().InsertUserMap(fightTeamMember.dwUserID, pFightTeamServed->dwFTID);
	}
	CCFightTeamManager::getSingleton().ClearReqRsp(pFightTeamServed->dwUserID);

	// 通知新成员
	CMD_G_FightTeamBase fightTeamBase = {0};
	pFightTeam->GetFightTeamBase(&fightTeamBase);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ACCEPT_S,(CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase));
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_PRODUCT;
	taskEventInfo.iSubType = 1001;				//跟小刚约定,创建/加入战队挂生产任务编号1001
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);

	// 判断加入/创建指定等级战队的任务
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_INFIGHTTEAM;
	taskSysInfo.iProgress = fightTeamBase.iGrade;
	pUser->m_userTask.DisposeTask(taskSysInfo);

	//更新领主战队ID
	pUser->m_userBase.SetFightTeamID(pFightTeamServed->dwFTID);

	// 通知各成员
	for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		{
			if (pMUser->GetUserID() != pFightTeamServed->dwUserID)
			{
				pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_ADD,(CT_CHAR*)&fightTeamMember, sizeof(CMD_G_FightTeamMember));
			}
		}
	}

	//龙岛战队积分判断
	pUser->m_userIsland.TeamIn(pFightTeamServed->dwFTID);

	CMD_G_FightTeamEventLog fightTeamEventLog={0};
	CT_CHAR cEventDesc[DESCRIPTION_SIZE] = {0};
	CT_CHAR cName[NAME_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	RollBroadcast rb={0};
	fightTeamEventLog.iReport = 0;
	fightTeamEventLog.lDate = time(0);
	pFightTeam->GetMemberName(pFightTeamServed->dwUserID, cName);
	rb.SetValue(0,cName);
	//sprintf(cEventDesc, "<%s>{1}", cName);
	G_GetRollBroadecastData(103,cEventDesc,iEventDescLen,(CT_CHAR*)&rb,sizeof(rb));
	memcpy(fightTeamEventLog.cEventDesc, cEventDesc, DESCRIPTION_SIZE);
	pFightTeam->AddEventLog(fightTeamEventLog);

	// 通知队长 客户端队长不显示邀请列表
// 	CT_DWORD dwBossID = 0;
// 	dwBossID = pFightTeam->GetFightTeamBossID();
// 	CCUser *pBossUser=0;
// 	if (CCUserManager::getSingleton().Find(dwBossID,&pBossUser))
// 	{
// 		pBossUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ACCEPT, (CT_CHAR*)&fightTeamMember, sizeof(CMD_G_FightTeamMember));
// 	}
}

// 玩家不同意邀请
CT_VOID CCFightTeamEvent::DisposeFightTeamReject(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Served))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReject: len != sizeof(CMD_G_FightTeam_Served)");
		return;
	}

	CMD_G_FightTeam_Served *pFightTeamServed=(CMD_G_FightTeam_Served*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamServed->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReject,can't find user %d",pFightTeamServed->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamServed->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamServed->dwFTID);
		return;
	}

	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REJECT_S, (CT_CHAR*)&(pFightTeamServed->dwFTID), sizeof(CT_DWORD));
	pFightTeam->DeleteMember(3, pFightTeamServed->dwUserID);

	// 通知队长
	CT_DWORD dwBossID = 0;
	dwBossID = pFightTeam->GetFightTeamBossID();
	CCUser *pBossUser=0;
	if (CCUserManager::getSingleton().Find(dwBossID,&pBossUser))
	{
		pBossUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REJECT, (CT_CHAR*)&dwUID, sizeof(CT_DWORD));
	}
}

// 捐献
CT_VOID CCFightTeamEvent::DisposeFightTeamContribute(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Donate))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamContribute: len != sizeof(CMD_G_FightTeam_Donate)");
		return;
	}

	CMD_G_FightTeam_Donate *pFightTeamDonate=(CMD_G_FightTeam_Donate*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamDonate->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamContribute,can't find user %d",pFightTeamDonate->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamDonate->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamDonate->dwFTID);
		return;
	}

	//捐献验证
	CT_INT32 iReturnCode=DisposeFightTeamContributeValidate(pUser,pFightTeamDonate,pFightTeam);
	if(iReturnCode != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CONTRIBUTE_FAIL, (CT_CHAR*)&iReturnCode, sizeof(CT_INT32));
		return;
	}
	
	CT_INT32 iContribution = pFightTeam->Contribute(*pFightTeamDonate);

	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);
	//pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CONTRIBUTE_S, (CT_CHAR*)&(mvd.stRes.iFood), sizeof(CT_INT32));
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_PRODUCT;
	taskEventInfo.iSubType = 1002;				//跟小刚约定,战队捐献挂生产任务编号1001
	taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
	pUser->m_userTask.DisposeTask(taskEventInfo);

	//捐献完成，判断指定资源捐献任务
	CMD_G_TaskSystemInfo taskSysInfo = {0};
	taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
	taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_DONATE;
	//参数0：捐献类型，参数1：资源id
	if (pFightTeamDonate->iFood != 0)
	{   
		taskSysInfo.arrayParam[0] = CLIENT_OBJ_RES;
		taskSysInfo.arrayParam[1] = VALUE_TYPE_FOOD;
		taskSysInfo.iProgress = pFightTeamDonate->iFood;
	}
	else if (pFightTeamDonate->iDiamod != 0)
	{
		taskSysInfo.arrayParam[0] = CLIENT_OBJ_RES;
		taskSysInfo.arrayParam[1] = VALUE_TYPE_DIAMOND;
		taskSysInfo.iProgress = pFightTeamDonate->iDiamod;
	}
	else 
	{
		taskSysInfo.arrayParam[0] = CLIENT_OBJ_ITEM;
		taskSysInfo.arrayParam[1] = pFightTeamDonate->iItemID;
		taskSysInfo.iProgress = pFightTeamDonate->iItemNum;
	}
	pUser->m_userTask.DisposeTask(taskSysInfo);

	// 通知各成员
	CMD_G_FightTeam_DonateRet fightTeamDonateRet = {0};
	fightTeamDonateRet.iResource = pFightTeam->m_stBase.iResource;
	fightTeamDonateRet.iItemRes = pFightTeam->m_stBase.iItemCount;
	fightTeamDonateRet.dwPlay = dwUID;
	fightTeamDonateRet.iContribution = iContribution;
	fightTeamDonateRet.iFood=mvd.stRes.iFood;
	fightTeamDonateRet.iDiamod=mvd.stRes.iDiamod;

	for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		{
			pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_CONTRIBUTE,(CT_CHAR*)&fightTeamDonateRet, sizeof(CMD_G_FightTeam_DonateRet));
		}
	}

	MSC_LOGDB_FightTeam logdbFightTeam = {0};
	logdbFightTeam.dwUserID = dwUID;
	logdbFightTeam.iFoodCost = pFightTeamDonate->iFood;
	logdbFightTeam.iGrade = mvd.stGrade.iLevel;
	logdbFightTeam.iDiamodCost=pFightTeamDonate->iDiamod;
	logdbFightTeam.iItemID=pFightTeamDonate->iItemID;
	logdbFightTeam.iItemCount=pFightTeamDonate->iItemNum;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + logdbFightTeam.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM,(CT_CHAR*)&logdbFightTeam,sizeof(MSC_LOGDB_FightTeam));
}

//捐献验证
CT_INT32 CCFightTeamEvent::DisposeFightTeamContributeValidate(CCUser *pUser,CMD_G_FightTeam_Donate *pFtd,CCFightTeam *pFightTeam)
{
	MasterVariableData mvd={0};
	pUser->m_userBase.GetMasterData(mvd);

	CMD_G_FightTeamMember ftm={0};
	pFightTeam->GetFightTeamMember(1,pUser->GetUserID(),&ftm);

	//读取捐献脚本
	G_FightTeamDonate ftd={0};

	//食物捐献
	if(pFtd->iFood != 0)
	{
		if(!G_GetFightTeamDonateEx(CLIENT_OBJ_RES,VALUE_TYPE_FOOD,&ftd))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamContribute G_GetFightTeamDonateEx VALUE_TYPE_FOOD");
			return FT_DATA_ERROR;
		}

		if (mvd.stRes.iFood < pFtd->iFood || pFtd->iFood < 0)
		{
			return FT_NO_FOOD;
		}

		if(ftd.iDailyLimit != -1)
		{
			if(ftm.iContributeCount[CCFightTeam::FIGHTTEAM_FOOD] > ftd.iDailyLimit)
				return FT_NO_NUM;
		}
	}

	//钻石捐献
	if(pFtd->iDiamod != 0)
	{
		if(!G_GetFightTeamDonateEx(CLIENT_OBJ_RES,VALUE_TYPE_DIAMOND,&ftd))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamContribute G_GetFightTeamDonateEx VALUE_TYPE_DIAMOND");
			return FT_DATA_ERROR;
		}

		if (mvd.stRes.iDiamod < pFtd->iDiamod || pFtd->iDiamod < 0)
		{
			return FT_NO_DIAMOD;
		}

		if(ftd.iDailyLimit != -1)
		{
			if(ftm.iContributeCount[CCFightTeam::FIGHTTEAM_DIAMOD] >= ftd.iDailyLimit+pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_CONTRIBUTE))
				return FT_NO_NUM;
		}
	}

	//道具捐献
	if(pFtd->iItemID != 0 && pFtd->iItemNum != 0)
	{
		if(!G_GetFightTeamDonateEx(CLIENT_OBJ_ITEM,pFtd->iItemID,&ftd))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamContribute G_GetFightTeamDonateEx CLIENT_OBJ_ITEM");
			return FT_DATA_ERROR;
		}

		CT_INT32 iCount=pFtd->iItemNum;
		CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		if(!pUser->m_userItem.AutoExpendItem(SAVETAG_FIGHTTEAM_CONTRIBUTE,pFtd->iItemID,iCount,ITEM_OP_MODE_CHECK) || iCount < 0)
		{
			return FT_NO_ITEM;
		}

		if(ftd.iDailyLimit != -1)
		{
			if(ftm.iContributeCount[CCFightTeam::FIGHTTEAM_ITEM] > ftd.iDailyLimit)
				return FT_NO_NUM;
		}
	}

	return 0;
}	
// 转让队长
CT_VOID CCFightTeamEvent::DisposeFightTeamTransfer(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamMember_Transfer))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamContribute: len != sizeof(CMD_G_FightTeamMember_Transfer)");
		return;
	}

	CMD_G_FightTeamMember_Transfer *pFightTeamTransfer=(CMD_G_FightTeamMember_Transfer*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamTransfer->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamContribute,can't find user %d",pFightTeamTransfer->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamTransfer->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamTransfer->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != dwUID)
	{
		return;
	}

	// 检查是不是队员
	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamTransfer->dwPlayerID) != pFightTeamTransfer->dwFTID)
	{
		return;
	}

	// 可以转了.
	pFightTeam->m_stBase.dwBossUID = pFightTeamTransfer->dwPlayerID;
	pFightTeam->GetMemberName(pFightTeamTransfer->dwPlayerID, pFightTeam->m_stBase.cBossName);

	//检查精英
	CT_INT32 iPlayerEliteIndex = -1;
	iPlayerEliteIndex = pFightTeam->GetEliteIndex(pFightTeamTransfer->dwPlayerID);
	if (iPlayerEliteIndex < 0)
	{
		CT_INT32 iBossEliteIndex;
		iBossEliteIndex = pFightTeam->GetEliteIndex(pFightTeamTransfer->dwUserID);
		pFightTeam->m_stBase.dwElite[iBossEliteIndex] = pFightTeamTransfer->dwPlayerID;

		//跨服战更新阵位数据
		if(CCCSFightData::getSingleton().GetStatus() != CS_Close)
		{
			CMD_CS_ChangeFTPos cscftp={0};
			cscftp.dwFTID=pFightTeamTransfer->dwFTID;
			cscftp.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
			if(pFightTeam->GetUserFightData(cscftp.ufs,CLAN_CRUSADE_MEMBER))
			{
				g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_CHANGE_FT_POS,(CT_CHAR*)&cscftp,sizeof(cscftp));
			}
		}
	}

	CMD_G_FightTeamBase fightTeamBase = {0};
	pFightTeam->GetFightTeamBase(&fightTeamBase);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_TRANSFER_S,(CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase));

	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &fightTeamBase, sizeof(MSC_DB_FightTeamBase));
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

// 	CCUser *pPlayUser=0;
// 	if (CCUserManager::getSingleton().Find(pFightTeamTransfer->dwPlayerID,&pPlayUser))
// 	{
// 		pPlayUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_TRANSFER,(CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase));
// 	}
	pFightTeam->Broadcast(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_TRANSFER,(CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase), dwUID);

	//召唤中解散召唤
	CMD_G_FightTeamHelpSummonLeave sl={0};
	sl.dwFTID=pFightTeamTransfer->dwFTID;
	sl.dwUserID=pFightTeamTransfer->dwUserID;
	DiposeFightTeamHelpSummonLeave((CT_CHAR*)&sl,sizeof(sl));
}

// 退出战队
CT_VOID CCFightTeamEvent::DisposeFightTeamQuit(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Quit))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamContribute: len != sizeof(CMD_G_FightTeam_Quit)");
		return;
	}

	CMD_G_FightTeam_Quit *pFightTeamQuit=(CMD_G_FightTeam_Quit*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamQuit->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamContribute,can't find user %d",dwUID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamQuit->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamQuit->dwFTID);
		return;
	}

	G_ValueRange vrlt={0};
	if(!G_GetValueRange(VALUE_TYPE_TEAM_LEAVE_TEAM_TIME,&vrlt))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamMemberFire VALUE_TYPE_TEAM_LEAVE_TEAM_TIME");
	}

	//跨服战验证
	if(CCCSFightData::getSingleton().GetSeat(pFightTeamQuit->dwFTID) == Seat_True && CCCSFightData::getSingleton().GetStatus() != CS_Close)
	{
		CMD_G_FightTeamCSFail ftcsf={0};
		ftcsf.iStatus=FT_CS_FIGHT;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTRAM_CS_FAIL,(CT_CHAR*)&ftcsf,sizeof(ftcsf));
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() == dwUID)
	{
		return;
	}

	CT_CHAR cName[NAME_SIZE] = {0};
	pFightTeam->GetMemberName(dwUID, cName);
	CCResourceGrabMgr::getSingleton().TeamKickOutUser(pFightTeamQuit->dwFTID, dwUID);
	if (pFightTeam->DeleteMember(1, pFightTeamQuit->dwUserID) > 0)
	{
		CCFightTeamManager::getSingleton().DeleteUserMap(pFightTeamQuit->dwUserID);
	}
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_QUIT_S);

	//检查精英
	CT_INT32 iPlayerEliteIndex = -1;
	iPlayerEliteIndex = pFightTeam->GetEliteIndex(dwUID);
	if (iPlayerEliteIndex > -1)
	{
		pFightTeam->m_stBase.dwElite[iPlayerEliteIndex] = 0;
	}

	// 通知各成员
	for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		{
			pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_DEL, (CT_CHAR*)&(dwUID), sizeof(CT_DWORD));
		}
	}

	//更新领主战队ID
	pUser->m_userBase.SetFightTeamID(0);

	//科技惩罚
	CooldownData cd={0};
	CT_INT32 iTime=time(0);
	pUser->m_userBase.GetCooldownData(FTNUMBERLEAVE_COOL,cd);
	if(cd.leftTime+ cd.toggleTime < iTime)
	{
		pUser->m_userBase.UpdateCooldown(FTNUMBERLEAVE_COOL,vrlt.fMaxVal);
		pUser->m_userFightTeamNember.ScienceDownGrade();
	}
	
	// 龙岛战队积分更新
	pUser->m_userIsland.TeamOut();

	MSC_DB_FightTeamScienceDownGrade dg={0};
	dg.dwUserID=pUser->GetUserID();
	CT_INT32 threadID =  GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_DOWN_GRADE,(CT_CHAR*)&dg,sizeof(dg));

	CMD_G_FightTeamBase fightTeamBase = {0};
	pFightTeam->GetFightTeamBase(&fightTeamBase);
	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &fightTeamBase, sizeof(MSC_DB_FightTeamBase));
	threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	CMD_G_FightTeamEventLog fightTeamEventLog={0};
	CT_CHAR cEventDesc[DESCRIPTION_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	RollBroadcast rb={0};
	rb.SetValue(0,cName);
	fightTeamEventLog.iReport = 0;
	fightTeamEventLog.lDate = time(0);
	//sprintf(cEventDesc, "<%s>{4}", cName);
	G_GetRollBroadecastData(104,cEventDesc,iEventDescLen,(CT_CHAR*)&rb,sizeof(rb));
	memcpy(fightTeamEventLog.cEventDesc, cEventDesc, DESCRIPTION_SIZE);
	pFightTeam->AddEventLog(fightTeamEventLog);

	//时限BOSS
	DeleteTimeBossInfo(pUser->GetUserID(),pFightTeam);
}

// 设置战队精英
CT_VOID CCFightTeamEvent::DisposeFightTeamElite(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Crusade))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReject: len != sizeof(CMD_G_FightTeam_Crusade)");
		return;
	}

	CMD_G_FightTeam_Crusade *pFightTeamCrusade=(CMD_G_FightTeam_Crusade*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamCrusade->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReject,can't find user %d",pFightTeamCrusade->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamCrusade->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamCrusade->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != dwUID)
	{
		return;
	}
	// 1.队长必须出战 
	CT_BOOL bOK = CT_FALSE;
	CT_INT32 iRetCode = 0;
	CT_INT32 i,j;
	for(i = 0; i < CLAN_CRUSADE_MEMBER; ++i)
	{
		if (dwUID == pFightTeamCrusade->dwPlayerID[i])
		{
			bOK = CT_TRUE;
		}
	}
	if (!bOK)
	{
		iRetCode = FT_BOSS_ELITE;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ELITE_FAIL,(CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	// 2.队员重复
	for (i = 0; i < CLAN_CRUSADE_MEMBER; ++i)
	{
		for (j = i+1; j < CLAN_CRUSADE_MEMBER; ++j)
		{
			if (pFightTeamCrusade->dwPlayerID[i]>0 && pFightTeamCrusade->dwPlayerID[i] == pFightTeamCrusade->dwPlayerID[j])
			{
				iRetCode = FT_ELITE_DUP;
				pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ELITE_FAIL,(CT_CHAR*)&iRetCode, sizeof(CT_INT32));
				return;
			}
		}
	}

	// 3.队员无效
	for(i = 0; i < CLAN_CRUSADE_MEMBER; ++i)
	{
		if (pFightTeamCrusade->dwPlayerID[i]>0 && CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamCrusade->dwPlayerID[i]) != pFightTeamCrusade->dwFTID)
		{
			iRetCode = FT_MEMBER_INVALID;
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ELITE_FAIL,(CT_CHAR*)&iRetCode, sizeof(CT_INT32));
			return;
		}
	}

	CT_DWORD dwNewElite[CLAN_CRUSADE_MEMBER] = {0};
	for(i = 0; i < CLAN_CRUSADE_MEMBER; ++i)
	{
		if (pFightTeam->GetMemberPosition(pFightTeamCrusade->dwPlayerID[i]) == CCFightTeam::FIGHTTEAM_MEMBER)
		{
			dwNewElite[i] = pFightTeamCrusade->dwPlayerID[i];
		}
	}

	 // 到这里可以设置精英出战队员了
	 for(CT_INT32 i = 0; i < CLAN_CRUSADE_MEMBER; ++i)
	 {
		 pFightTeam->m_stBase.dwElite[i] = pFightTeamCrusade->dwPlayerID[i];
	 }

	 pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ELITE_S,pBuf, len);

	 //跨服战更新阵位数据
	 if(CCCSFightData::getSingleton().GetStatus() != CS_Close)
	 {
		 CMD_CS_ChangeFTPos cscftp={0};
		 cscftp.dwFTID=pFightTeamCrusade->dwFTID;
		 cscftp.iGSID=((CCGameConfig&)CCConfig::getSingleton()).m_iGSID;
		 if(pFightTeam->GetUserFightData(cscftp.ufs,CLAN_CRUSADE_MEMBER))
		 {
			 g_pCorssServer->Send(MAIN_CS_GAME,SUB_CS_CHANGE_FT_POS,(CT_CHAR*)&cscftp,sizeof(cscftp));
		 }
	 }
	 
	 // 通知各成员
	 for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	 {
		 CCUser *pMUser = 0;
		 if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		 {
			 if (pMUser->GetUserID() != dwUID)
			 {
				 pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_ELITE,pBuf, len);
			 }
		 }
	 }

	 CMD_G_FightTeamBase fightTeamBase = {0};
	 pFightTeam->GetFightTeamBase(&fightTeamBase);
	 MSC_DB_FightTeamBase dbFightTeamBase = {0};
	 memcpy(&dbFightTeamBase, &fightTeamBase, sizeof(MSC_DB_FightTeamBase));
	 CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	 G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	 for(i = 0; i < CLAN_CRUSADE_MEMBER; ++i)
	 {
		 if (dwNewElite[i] > 0)
		 {
			 CMD_G_FightTeamEventLog fightTeamEventLog={0};
			 CT_CHAR cEventDesc[DESCRIPTION_SIZE] = {0};
			 CT_CHAR cName[NAME_SIZE] = {0};
			 CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
			 RollBroadcast rb={0};
	
			 fightTeamEventLog.iReport = 0;
			 fightTeamEventLog.lDate = time(0);
			 pFightTeam->GetMemberName(dwNewElite[i], cName);
			 rb.SetValue(0,cName);
			 //sprintf(cEventDesc, "<%s>{5}", cName);
			 G_GetRollBroadecastData(105,cEventDesc,iEventDescLen,(CT_CHAR*)&rb,sizeof(rb));
			 memcpy(fightTeamEventLog.cEventDesc, cEventDesc, DESCRIPTION_SIZE);
			 pFightTeam->AddEventLog(fightTeamEventLog);
		 }
	 }
}

// 设置战队公告
CT_VOID CCFightTeamEvent::DisposeFightTeamDesc(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Announce))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDesc: len != sizeof(CMD_G_FightTeam_Announce)");
		return;
	}

	CMD_G_FightTeam_Announce *pFightTeamAnnounce=(CMD_G_FightTeam_Announce*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamAnnounce->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDesc,can't find user %d",pFightTeamAnnounce->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamAnnounce->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDesc,can't find FightTeam %d",pFightTeamAnnounce->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != dwUID)
	{
		return;
	}

	memcpy(pFightTeam->m_stBase.cDescription, pFightTeamAnnounce->cAnnounce, DESCRIPTION_SIZE);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SET_DESC_S, pFightTeamAnnounce->cAnnounce, DESCRIPTION_SIZE);
	CMD_G_FightTeamBase fightTeamBase = {0};
	pFightTeam->GetFightTeamBase(&fightTeamBase);
	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &fightTeamBase, sizeof(MSC_DB_FightTeamBase));
	CT_DWORD threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	// 通知各成员
	for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		{
			if (pMUser->GetUserID() != dwUID)
			{
				pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SET_DESC, pFightTeamAnnounce->cAnnounce, DESCRIPTION_SIZE);
			}
		}
	}
}

// 查询战队
CT_VOID CCFightTeamEvent::DisposeFightTeamSearch(CT_CHAR* pBuf, CT_DWORD len)
{
	//
	if(len != sizeof(CMD_G_FightTeam_Seek))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSearch: len != sizeof(CMD_G_FightTeam_Seek)");
		return;
	}

	CMD_G_FightTeam_Seek *pFightTeamSeek=(CMD_G_FightTeam_Seek*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamSeek->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSearch,can't find user %d",pFightTeamSeek->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	CMD_G_FightTeamInfo fightTeamInfo = {0};
	if (pFightTeamSeek->dwFTID > 0)
	{
		if(CCFightTeamManager::getSingleton().Find(pFightTeamSeek->dwFTID, &pFightTeam))
		{
			pFightTeam->GetFightTeamInfo(&fightTeamInfo);
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SEARCH, (CT_CHAR*)&fightTeamInfo, sizeof(CMD_G_FightTeamInfo));
			return;
		}
	}
	if(CCFightTeamManager::getSingleton().Find(pFightTeamSeek->cFightTeamName, &pFightTeam))
	{
		pFightTeam->GetFightTeamInfo(&fightTeamInfo);
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SEARCH, (CT_CHAR*)&fightTeamInfo, sizeof(CMD_G_FightTeamInfo));
	}
	else
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_SEARCH);
	}

}

// 发布招募
CT_VOID CCFightTeamEvent::DisposeFightTeamRecruit(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Recruit))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRecruit: len != sizeof(CMD_G_FightTeam_Recruit)");
		return;
	}

	CMD_G_FightTeam_Recruit *pFightTeamRecruit=(CMD_G_FightTeam_Recruit*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamRecruit->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRecruit,can't find user %d",pFightTeamRecruit->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamRecruit->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRecruit,can't find FightTeam %d",pFightTeamRecruit->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != dwUID)
	{
		return;
	}

	//检查资源
	G_FightTeamRecruit ftRecruit = {0};
	G_GetFightTeamRecruit(pFightTeamRecruit->iIndex, &ftRecruit);
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	CT_INT32 iDiamod = ftRecruit.iDiamod;
	CT_INT32 iDur =ftRecruit.iDuration;
	if (mvd.stRes.iDiamod + iDiamod < 0)
	{
		CT_INT32 iRetCode = FT_NO_DIAMOD;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RECRUIT_FAIL, (CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//判断是否已发布招募
	if (CCFightTeamManager::getSingleton().CheckFightTeamRecruit(pFightTeamRecruit->dwFTID))
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RECRUIT_SURE, pBuf, len);
		return;
	}

	//可以发布招募了.
	DisposeFightTeamRecruitSure(pBuf, len);
}

// 发布招募
CT_VOID CCFightTeamEvent::DisposeFightTeamRecruitSure(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Recruit))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRecruit: len != sizeof(CMD_G_FightTeam_Recruit)");
		return;
	}

	CMD_G_FightTeam_Recruit *pFightTeamRecruit=(CMD_G_FightTeam_Recruit*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamRecruit->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRecruit,can't find user %d",pFightTeamRecruit->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamRecruit->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamRecruit,can't find FightTeam %d",pFightTeamRecruit->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != dwUID)
	{
		return;
	}

	//检查资源
	G_FightTeamRecruit ftRecruit = {0};
	G_GetFightTeamRecruit(pFightTeamRecruit->iIndex, &ftRecruit);
	MasterVariableData mvd = {0};
	pUser->m_userBase.GetMasterData(mvd);
	CT_INT32 iDiamod = ftRecruit.iDiamod;
	CT_INT32 iDur =ftRecruit.iDuration;


	//发布招募, 减掉资源
	memset(&mvd,0,sizeof(mvd));
	mvd.stRes.iDiamod = iDiamod;
	pUser->m_userBase.AddMastData(mvd,SAVETAG_FIGHTTEAM_RECRUIT);

	MSC_DB_FightTeamRecruit fightTeamRecruit = {0};
	fightTeamRecruit.dwFTID = pFightTeamRecruit->dwFTID;
	fightTeamRecruit.iBeginTime = time(0);
	fightTeamRecruit.iLastingTime = ftRecruit.iDuration;
	memcpy(fightTeamRecruit.cRecruit, pFightTeamRecruit->cAnnounce, DESCRIPTION_SIZE);
	CCFightTeamManager::getSingleton().InsertFightTeamRecruit(fightTeamRecruit);

	CT_DWORD threadID = GAME_THREAD_DATABASE + fightTeamRecruit.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_RECRUIT_UPDATE,(CT_CHAR*)&fightTeamRecruit,sizeof(MSC_DB_FightTeamRecruit));

	RollBroadcast rb={0};
	CT_CHAR cFightTeamName[NAME_SIZE] = {0};
	pFightTeam->GetFightTeamName(cFightTeamName);
	rb.SetValue(0,cFightTeamName);
	//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<6:%s>{21}",cFightTeamName);
	G_SendRollBroadecast(122,3,1,(CT_CHAR*)&rb,sizeof(rb));

	//重新获取资源,通知客户端
	memset(&mvd,0,sizeof(MasterVariableData));
	pUser->m_userBase.GetMasterData(mvd);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RECRUIT, (CT_CHAR*)&(mvd.stRes.iDiamod), sizeof(CT_INT32));

	MSC_LOGDB_FightTeam logdbFightTeam = {0};
	logdbFightTeam.dwUserID = dwUID;
	logdbFightTeam.iDiamodCost = iDiamod;
	logdbFightTeam.iGrade = mvd.stGrade.iLevel;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + logdbFightTeam.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM,(CT_CHAR*)&logdbFightTeam,sizeof(MSC_LOGDB_FightTeam));
}



// 查看招募
CT_VOID CCFightTeamEvent::DisposeFightTeamLookRecruit(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Browse))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookRecruit: len != sizeof(CMD_G_FightTeam_Browse)");
		return;
	}

	CMD_G_FightTeam_Browse *pFightTeamBrowse=(CMD_G_FightTeam_Browse*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamBrowse->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookRecruit,can't find user %d",pFightTeamBrowse->dwUserID);
		return;
	}

	CMD_G_FightTeamRecruit	fightTeamRecruit[50] = {0};
	CT_INT32	iBufLen = sizeof(CMD_G_FightTeamRecruit) * 50;
	CCFightTeamManager::getSingleton().BrowseFightTeamRecruit((CT_CHAR*)&fightTeamRecruit, iBufLen);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_RECRUIT_LIST, (CT_CHAR*)&fightTeamRecruit, iBufLen);
}

// 解散战队
CT_VOID CCFightTeamEvent::DisposeFightTeamDisband(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Dismiss))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDisband: len != sizeof(CMD_G_FightTeam_Dismiss)");
		return;
	}

	CMD_G_FightTeam_Dismiss *pFightTeamDismiss=(CMD_G_FightTeam_Dismiss*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamDismiss->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDisband,can't find user %d",pFightTeamDismiss->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamDismiss->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamDisband,can't find FightTeam %d",pFightTeamDismiss->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != dwUID)
	{
		return;
	}

	//跨服战验证
	if(CCCSFightData::getSingleton().GetSeat(pFightTeamDismiss->dwFTID) == Seat_True && CCCSFightData::getSingleton().GetStatus() != CS_Close)
	{
		CMD_G_FightTeamCSFail ftcsf={0};
		ftcsf.iStatus=FT_CS_FIGHT;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTRAM_CS_FAIL,(CT_CHAR*)&ftcsf,sizeof(ftcsf));
		return;
	}

	G_ValueRange vrlt={0};
	if(!G_GetValueRange(VALUE_TYPE_TEAM_LEAVE_TEAM_TIME,&vrlt))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamMemberFire VALUE_TYPE_TEAM_LEAVE_TEAM_TIME");
	}

	CCResourceGrabMgr::getSingleton().TeamDissolution(pFightTeamDismiss->dwFTID);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_DISBAND);
	// 通知各成员
	time_t t=time(0);
	for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		{
			if (pMUser->GetUserID() != dwUID)
			{
				pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_DISBAND);
			}

			//科技降级
			pUser->m_userBase.UpdateCooldown(FTNUMBERLEAVE_COOL,vrlt.fMaxVal);
			pMUser->m_userFightTeamNember.ScienceDownGrade();

			// 龙岛战队积分更新
			pMUser->m_userIsland.TeamOut();
		}
		else
		{
			//离队惩罚
			CooldownData cd={0};
			cd.iType=FTNUMBERLEAVE_COOL;
			cd.leftTime=vrlt.fMaxVal;
			cd.toggleTime=t;
			CCUserBase::UpdateCooldownData(it->first,cd);
		}

		//科技惩罚
		MSC_DB_FightTeamScienceDownGrade dg={0};
		dg.dwUserID=it->first;
		CT_INT32 threadID =  GAME_THREAD_DATABASE + it->first % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_DOWN_GRADE,(CT_CHAR*)&dg,sizeof(dg));
	}

	//更新领主战队ID
	pUser->m_userBase.SetFightTeamID(0);

	//召唤中解散召唤
	CMD_G_FightTeamHelpSummonLeave sl={0};
	sl.dwFTID=pFightTeamDismiss->dwFTID;
	sl.dwUserID=pFightTeamDismiss->dwUserID;
	DiposeFightTeamHelpSummonLeave((CT_CHAR*)&sl,sizeof(sl));

	CCFightTeamManager::getSingleton().DeleteFightTeam(pFightTeamDismiss->dwFTID);
}

// 查看战报
CT_VOID CCFightTeamEvent::DisposeFightTeamLookEvent(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_CommonArg))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookEvent: len != sizeof(CMD_G_FightTeam_CommonArg)");
		return;
	}

	CMD_G_FightTeam_CommonArg *pFightTeamCommonArg=(CMD_G_FightTeam_CommonArg*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamCommonArg->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookEvent,can't find user %d",pFightTeamCommonArg->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamCommonArg->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookEvent,can't find FightTeam %d",pFightTeamCommonArg->dwFTID);
		return;
	}

	CMD_G_FightTeamEventLog	fightTeamEventLog[50] = {0};
	CT_INT32	iBufLen = sizeof(CMD_G_FightTeamEventLog) * 50;
	pFightTeam->GetEventLog((CT_CHAR*)&fightTeamEventLog, iBufLen);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_LOOKEVENT_S, (CT_CHAR*)&fightTeamEventLog, iBufLen);
}

//查看战斗
CT_VOID CCFightTeamEvent::DisposeFightTeamLookFight(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Fight))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookFight: len != sizeof(CMD_G_FightTeam_Fight)");
		return;
	}
	CMD_G_FightTeam_Fight *pFightTeamFight=(CMD_G_FightTeam_Fight*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamFight->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookEvent,can't find user %d",pFightTeamFight->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamFight->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamLookEvent,can't find FightTeam %d",pFightTeamFight->dwFTID);
		return;
	}

	FTFightData::iterator it = pFightTeam->m_FTFightMap.find(pFightTeamFight->iReport);
	if (it == pFightTeam->m_FTFightMap.end())
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_LOOKFIGHT_FAIL);
		return;
	}
	if (it->second.bPVP)	//PVP
	{
		MSC_TeamPVP_ResRobbery pvp = {0};
		pvp.dwUserID  = dwUID;
		pvp.iSeed     = it->second.iSeed;
		pvp.bLookLeft = pFightTeamFight->dwFTID==it->second.dwLeftFTID?CT_TRUE:CT_FALSE;
		memcpy(pvp.LeftFightingData, it->second.userFightingLeft, sizeof(pvp.LeftFightingData));
		memcpy(pvp.RightFightingData, it->second.userFightingRight, sizeof(pvp.RightFightingData));
		CT_DWORD dwTheadID = GAME_THREAD_FIGHT + pvp.dwUserID %  ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		G_SendThreadInfo(dwTheadID, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_FIGHTTEAM_PVP, (CT_CHAR*)&pvp, sizeof(pvp));
	}
	else					//PVE
	{
		MSC_TeamPVE_ResRobbery pve = {0};
		pve.dwUserID = dwUID;
		pve.iSeed = it->second.iSeed;
		pve.iResSpotQuality = it->second.iSpotQuality;	
		memcpy(pve.FightingData, it->second.userFightingLeft, sizeof(pve.FightingData));
		CT_DWORD dwTheadID = GAME_THREAD_FIGHT + pve.dwUserID %  ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
		G_SendThreadInfo(dwTheadID, MSC_MAIN_FIGHTING_EVENT, MSC_SUB_FIGHTTEAM_PVE,(CT_CHAR*)&pve, sizeof(pve));
	}	
}

// 踢除队员	
CT_VOID CCFightTeamEvent::DisposeFightTeamMemberFire(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamMember_Fire))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamMemberFire: len != sizeof(CMD_G_FightTeamMember_Fire)");
		return;
	}

	CMD_G_FightTeamMember_Fire *pFightTeamFire=(CMD_G_FightTeamMember_Fire*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamFire->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamMemberFire,can't find user %d",pFightTeamFire->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamFire->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamFire->dwFTID);
		return;
	}

	// 检查是不是队长
	if (pFightTeam->GetFightTeamBossID() != pFightTeamFire->dwUserID)
	{
		return;
	}

	//跨服战验证
	if(CCCSFightData::getSingleton().GetSeat(pFightTeamFire->dwFTID) == Seat_True && CCCSFightData::getSingleton().GetStatus() != CS_Close)
	{
		CMD_G_FightTeamCSFail ftcsf={0};
		ftcsf.iStatus=FT_CS_FIGHT;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTRAM_CS_FAIL,(CT_CHAR*)&ftcsf,sizeof(ftcsf));
		return;
	}

	// 不能踢自己
	if (pFightTeamFire->dwPlayerID == pFightTeamFire->dwUserID)
	{
		CT_INT32 iRetCode = FT_CANNOT_FIRE;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_FIRE_FAIL,(CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	CT_CHAR cName[NAME_SIZE] = {0};
	pFightTeam->GetMemberName(pFightTeamFire->dwPlayerID, cName);
	CCResourceGrabMgr::getSingleton().TeamKickOutUser(pFightTeamFire->dwFTID, pFightTeamFire->dwPlayerID);
	if (pFightTeam->DeleteMember(1, pFightTeamFire->dwPlayerID) > 0)
	{
		CCFightTeamManager::getSingleton().DeleteUserMap(pFightTeamFire->dwPlayerID);
	}

	//召唤中离开
	CMD_G_FightTeamHelpSummonLeave sl={0};
	sl.dwFTID=pFightTeamFire->dwFTID;
	sl.dwUserID=pFightTeamFire->dwPlayerID;
	DiposeFightTeamHelpSummonLeave((CT_CHAR*)&sl,sizeof(sl));

	CCUser *pPlayer=0;
	if (CCUserManager::getSingleton().Find(pFightTeamFire->dwPlayerID,&pPlayer))
	{
		pPlayer->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_FIRE);

		//更新领主战队ID
		pPlayer->m_userBase.SetFightTeamID(0);
	}

	//检查精英
	CT_INT32 iPlayerEliteIndex = -1;
	iPlayerEliteIndex = pFightTeam->GetEliteIndex(pFightTeamFire->dwPlayerID);
	if (iPlayerEliteIndex > -1)
	{
		pFightTeam->m_stBase.dwElite[iPlayerEliteIndex] = 0;
	}

	G_ValueRange vrlt={0};
	if(!G_GetValueRange(VALUE_TYPE_TEAM_LEAVE_TEAM_TIME,&vrlt))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamMemberFire VALUE_TYPE_TEAM_LEAVE_TEAM_TIME");
		CT_INT32 iRetCode = FT_DATA_ERROR;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_FIRE_FAIL,(CT_CHAR*)&iRetCode, sizeof(CT_INT32));
		return;
	}

	//科技惩罚
	CCUser *pPlayUser=0;
	CT_INT64 lTime=time(0);
	if(CCUserManager::getSingleton().Find(pFightTeamFire->dwPlayerID,&pPlayUser))
	{
		pPlayUser->m_userFightTeamNember.ScienceDownGrade();

		CooldownData cd={0};
		pPlayUser->m_userBase.GetCooldownData(FTNUMBERLEAVE_COOL,cd);
		if(cd.leftTime+ cd.toggleTime < lTime)
		{
			pPlayUser->m_userBase.UpdateCooldown(FTNUMBERLEAVE_COOL,vrlt.fMaxVal);
		}

		// 龙岛战队积分更新
		pPlayUser->m_userIsland.TeamOut();
	}
	else
	{
		//离队惩罚
		CooldownData cd={0};
		cd.iType=FTNUMBERLEAVE_COOL;
		cd.leftTime=vrlt.fMaxVal;
		cd.toggleTime=lTime;
		CCUserBase::UpdateCooldownData(pFightTeamFire->dwPlayerID,cd);
	}

	//科技惩罚
	MSC_DB_FightTeamScienceDownGrade dg={0};
	dg.dwUserID=pFightTeamFire->dwPlayerID;
	CT_INT32 threadID =  GAME_THREAD_DATABASE + pFightTeamFire->dwPlayerID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_DOWN_GRADE,(CT_CHAR*)&dg,sizeof(dg));
	
	// 通知各成员
	for (FTMember::iterator it = pFightTeam->m_FTMemberMap.begin(); it != pFightTeam->m_FTMemberMap.end(); ++it)
	{
		CCUser *pMUser = 0;
		if (CCUserManager::getSingleton().Find(it->first,&pMUser))
		{
			pMUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER_DEL, (CT_CHAR*)&(pFightTeamFire->dwPlayerID), sizeof(CT_DWORD));
		}
	}

	CMD_G_FightTeamBase fightTeamBase = {0};
	pFightTeam->GetFightTeamBase(&fightTeamBase);
	MSC_DB_FightTeamBase dbFightTeamBase = {0};
	memcpy(&dbFightTeamBase, &fightTeamBase, sizeof(MSC_DB_FightTeamBase));
	threadID = GAME_THREAD_DATABASE + dbFightTeamBase.dwFTID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_FIGHTTEAM_DATA,MSC_SUB_DB_FIGHTTEAM_UPDATE,(CT_CHAR*)&dbFightTeamBase,sizeof(MSC_DB_FightTeamBase));

	CMD_G_FightTeamEventLog fightTeamEventLog={0};
	CT_CHAR cEventDesc[DESCRIPTION_SIZE] = {0};
	CT_INT32 iEventDescLen=DESCRIPTION_SIZE;
	RollBroadcast rb={0};
	rb.SetValue(0,cName);
	fightTeamEventLog.iReport = 0;
	fightTeamEventLog.lDate = time(0);
	//sprintf(cEventDesc, "<%s>{4}", cName);
	G_GetRollBroadecastData(104,cEventDesc,iEventDescLen,(CT_CHAR*)&rb,sizeof(rb));
	memcpy(fightTeamEventLog.cEventDesc, cEventDesc, DESCRIPTION_SIZE);
	pFightTeam->AddEventLog(fightTeamEventLog);

	//时限BOSS
	DeleteTimeBossInfo(pUser->GetUserID(),pFightTeam);
}

//请求战队列表
CT_VOID CCFightTeamEvent::DisposeFightTeamList(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Browse))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamList: len != sizeof(CMD_G_FightTeam_Browse)");
		return;
	}

	CMD_G_FightTeam_Browse *pFightTeamBrowse=(CMD_G_FightTeam_Browse*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamBrowse->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamList,can't find user %d",pFightTeamBrowse->dwUserID);
		return;
	}

	CMD_G_FightTeamInfo	fightTeamInfo[50] = {0};
	CT_INT32	iBufLen = sizeof(CMD_G_FightTeamInfo) * 50;
	CCFightTeamManager::getSingleton().BrowseFightTeam((CT_CHAR*)&fightTeamInfo, iBufLen);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_LIST, (CT_CHAR*)&fightTeamInfo, iBufLen);

}

//请求申请列表
CT_VOID CCFightTeamEvent::DisposeFightTeamReqList(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_CommonArg))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReqList: len != sizeof(CMD_G_FightTeam_Crusade)");
		return;
	}

	CMD_G_FightTeam_CommonArg *pFightTeamCommonArg=(CMD_G_FightTeam_CommonArg*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamCommonArg->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReqList,can't find user %d",pFightTeamCommonArg->dwUserID);
		return;
	}

	if(CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamCommonArg->dwUserID) != pFightTeamCommonArg->dwFTID)
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamMember,FightTeam invalid user %d",pFightTeamCommonArg->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamCommonArg->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamAgree,can't find FightTeam %d",pFightTeamCommonArg->dwFTID);
		return;
	}

	CT_CHAR cBuf[50*72] = {0};
	CT_DWORD dwLen = 50*72;
	pFightTeam->GetFightTeamReqMember(cBuf, dwLen);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_REQ_LIST, cBuf, dwLen);

}

//请求邀请列表
CT_VOID CCFightTeamEvent::DisposeFightTeamInviteList(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Browse))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInviteList: len != sizeof(CMD_G_FightTeam_Browse)");
		return;
	}

	CMD_G_FightTeam_Browse *pFightTeamCrusade=(CMD_G_FightTeam_Browse*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamCrusade->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInviteList,can't find user %d",pFightTeamCrusade->dwUserID);
		return;
	}

	//检查是否已加入战队
	if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(dwUID) > 1)
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInviteList,user %d has join fight team",dwUID);
		return;
	}

	CMD_G_FightTeamInfo fightTeamInfo[50] = {0};
	CT_DWORD dwLen = sizeof(CMD_G_FightTeamInfo) * 50;
	CCFightTeamManager::getSingleton().GetFightTeamInviteList(dwUID, (CT_CHAR*)fightTeamInfo, dwLen);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_INVITE_LIST, (CT_CHAR*)fightTeamInfo, dwLen);

}

//请求玩家列表
CT_VOID CCFightTeamEvent::DisposeFightTeamPlayList(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_Browse))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReject: len != sizeof(CMD_G_FightTeam_Browse)");
		return;
	}

	CMD_G_FightTeam_Browse *pFightTeamBrowse=(CMD_G_FightTeam_Browse*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamBrowse->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamReject,can't find user %d",pFightTeamBrowse->dwUserID);
		return;
	}

	CMD_G_PlayerData	palyerData[100] = {0};
	CT_INT32 iBufLen = sizeof(CMD_G_PlayerData) * 100;
	CCUserManager::getSingleton().GetOnlineUserData(dwUID, (CT_CHAR*)&palyerData, iBufLen, 2);
	CT_INT32 iCount = iBufLen / sizeof(CMD_G_PlayerData);
	for (CT_INT32 i=0; i < iCount; ++i)
	{
		if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser(palyerData[i].dwPlayer) > 0)
		{
//			memset(&palyerData[i], 0, sizeof(CMD_G_PlayerData));
			palyerData[i].dwPlayer = 0;
		}
	}
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_PLAY_LIST, (CT_CHAR*)&palyerData, iBufLen);
}

CT_VOID CCFightTeamEvent::DisposeFightTeamMember(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeam_CommonArg))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamMember: len != sizeof(CMD_G_FightTeam_CommonArg)");
		return;
	}

	CMD_G_FightTeam_CommonArg *pFightTeamCommonArg=(CMD_G_FightTeam_CommonArg*)pBuf;
	CCUser *pUser=0;
	CT_DWORD dwUID = pFightTeamCommonArg->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamMember,can't find user %d",pFightTeamCommonArg->dwUserID);
		return;
	}

	if(CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pFightTeamCommonArg->dwUserID) != pFightTeamCommonArg->dwFTID)
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamMember,FightTeam invalid user %d",pFightTeamCommonArg->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFightTeamCommonArg->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamMember,can't find FightTeam %d",pFightTeamCommonArg->dwFTID);
		return;
	}

	CT_CHAR cBuf[2048] = {0};
	CT_DWORD dwLen = 2048;
	pFightTeam->GetFightTeamMember(cBuf, dwLen);
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_MEMBER, cBuf, dwLen);
}


CT_VOID CCFightTeamEvent::DisposeFightTeamFightingData(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FTMemberFightingData))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamFightingData: len != sizeof(MSC_DB_FTMemberFightingData)");
		return;
	}

	MSC_DB_FTMemberFightingData *pFTMemberFightingData=(MSC_DB_FTMemberFightingData*)pBuf;
	CT_DWORD dwUID = pFTMemberFightingData->dwUserID;

	//CT_DWORD dwFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(dwUID);

	//CCFightTeam *pFightTeam = 0;
	//if(!CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
	//{
	//	CCLogMeg::getSingleton().WriteError("DisposeFightTeamFightingData,can't find FightTeam %d",dwFTID);
	//	return;
	//}
	//pFightTeam->InsertMemberFightingData(*pFTMemberFightingData);
}

CT_VOID CCFightTeamEvent::DiposeFightTeamMemeRefresh(CT_CHAR* pBuf, CT_DWORD len)    //彭文添加，刷新队员数据
{
	if (len != sizeof(MSC_DB_FTMemeberRefresh))
	{
		CCLogMeg::getSingleton().WriteError("DiposeFightTeamMemeRefresh: len != sizeof(MSC_DB_FTMemeberRefresh)");
		return;
	}

	MSC_DB_FTMemeberRefresh* pDBMemRefresh = (MSC_DB_FTMemeberRefresh*)pBuf;
	CCFightTeamManager::getSingleton().FightTeamMemberRef(pDBMemRefresh->dwUser , pDBMemRefresh->iGrade, pDBMemRefresh->iFightingPower );
}

CT_VOID CCFightTeamEvent::DiposeFightTeamUpGrade(CT_CHAR *pBuf,CT_DWORD len)		//战队升级
{
	if (len != sizeof(CMD_G_FightTeam_UpGrade))
	{
		CCLogMeg::getSingleton().WriteError("DiposeFightTeamUpGrade: len != sizeof(CMD_G_FightTeam_UpGrade)");
		return;
	}

	CMD_G_FightTeam_UpGrade *pFtug=(CMD_G_FightTeam_UpGrade*)pBuf;

	CCUser *pUser=0;
	CT_DWORD dwUID = pFtug->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DiposeFightTeamUpGrade,can't find user %d",pFtug->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFtug->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DiposeFightTeamUpGrade,can't find FightTeam %d",pFtug->dwFTID);
		return;
	}

	CMD_G_FightTeamBase ftb={0};
	CMD_G_FightTeamUpGradeReturn ftreturn={0};
	ftreturn.dwUserID=pUser->GetUserID();
	pFightTeam->GetFightTeamBase(&ftb);

	ftreturn.iErrorID=FightTeamUpGradeValidate(pUser,ftb);
	if(ftreturn.iErrorID != FT_UP_GRADE_SUC)
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,ftreturn.iErrorID);
		//pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_UP_GRADE_RETURN, (CT_CHAR*)&ftreturn, sizeof(ftreturn));
		return;
	}

	pFightTeam->FightTeamUpGrade();
	pFightTeam->UpdataFightTeamInfo(pUser->GetUserID());
	return;
}

//战队升级检测
CT_INT32 CCFightTeamEvent::FightTeamUpGradeValidate(CCUser *pUser,CMD_G_FightTeamBase &ftb)
{
	//读取脚本判断资源,等级
	G_FightTeamUpgrade ftug={0};
	if(!G_GetFightTeamUpgrade(ftb.iGrade+1,FIGHTTEAM_BOSS_SUMMON,&ftug))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamUpGradeValidate G_GetFightTeamUpgrade Grade:%d",ftb.iGrade+1);
		return FT_DATA_ERROR;
	}

	G_CostRewardGroup crg={0};
	if(!G_GetCostRewardGroup(ftug.iItemGroupID,&crg) && ftug.iItemGroupID != 0)
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamUpGradeValidate G_GetCostRewardGroup ItemGroupID:%d",ftug.iItemGroupID);
		return FT_DATA_ERROR;
	}

	if(ftb.dwBossUID != pUser->GetUserID())
	{
		return FT_NO_BOSS;
	}

	for (CT_INT32 i = 0; i < sizeof(crg.res) / sizeof(*crg.res); ++i)
	{
		if (crg.res[i].iID == VALUE_TYPE_FT_RES) 
		{
			if (ftb.iResource + crg.res[i].iNum < 0) return FT_NO_FOOD;
		}
		else if (crg.res[i].iID == VALUE_TYPE_FT_ITEM) 
		{
			if (ftb.iItemCount + crg.res[i].iNum < 0) return FT_NO_ITEM;
		}

	}

	//魔神之井
	if(ftug.iKillBoss != 0)
	{
		for(CT_INT32 i=0;i<ftb.iBossMaxIndex;++i)
		{
			G_FightTeamUpgrade fg={0};
			G_FightTeamUpgrade tbfg={0};
			if(!G_GetFightTeamUpgrade(i+1,FIGHTTEAM_BOSS_SUMMON,&fg))
			{
				CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamUpGradeValidate G_GetFightTeamUpgrade Grade:%d",i+1);
				return FT_DATA_ERROR;
			}
			
			if(!G_GetFightTeamUpgrade(i+1,FIGHTTEAM_BOSS_TIME,&tbfg))
			{
				CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamUpGradeValidate G_GetFightTeamUpgrade Grade:%d",i+1);
				return FT_DATA_ERROR;
			}

			if(ftb.iBossMaxIndex >= fg.iOpenBoss && ftb.iTimeBossMaxIndex >= tbfg.iOpenBoss)
			{
				return FT_UP_GRADE_SUC;
			}
		}
		return FT_NO_PASS_BOSS;
	}

	//时限boss
	return FT_UP_GRADE_SUC;
}
//成员科技升级
CT_VOID CCFightTeamEvent::DiposeFightTeamNemberScienceUp(CT_CHAR *pBuf,CT_DWORD len)		
{
	if(len != sizeof(CMD_G_FightTeamScienceUpGrade))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DiposeFightTeamNemberScienceUp len != sizeof(CMD_G_FightTeamScienceUpGrade)");
		return;
	}

	CMD_G_FightTeamScienceUpGrade *pUpGrade=(CMD_G_FightTeamScienceUpGrade*)pBuf;

	CCUser *pUser=0;
	CT_DWORD dwUID = pUpGrade->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DiposeFightTeamNemberScienceUp,can't find user %d",pUpGrade->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pUpGrade->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DiposeFightTeamNemberScienceUp,can't find FightTeam %d",pUpGrade->dwFTID);
		return;
	}

	CMD_G_FightTeamScienceUpGradeReturn ftreturn={0};
	ftreturn.dwUserID=pUser->GetUserID();
	ftreturn.iErrorID=FightTeamNemberScienceValidate(pUser,pUpGrade->iScienceID,pFightTeam);
	if(ftreturn.iErrorID != FT_SCIENCE_UP_GRADE_SUC)
	{
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SCIENCE_UP_GRADE_RETURN,(CT_CHAR*)&ftreturn,sizeof(ftreturn));
		return;
	}

	CMD_G_FightTeamScienceUpGradeSuc sugs={0};
	CMD_G_FightTeamMember FTMember={0};
	sugs.dwUserID=pUser->GetUserID();
	sugs.iScienceID=pUpGrade->iScienceID;
	sugs.iScienceStatus=pUser->m_userFightTeamNember.ScienceUpGrade(pUpGrade->iScienceID,pUpGrade->iItemCount);
	if(sugs.iScienceStatus == FT_SCIENCE_UP_GRADE_SUC || sugs.iScienceStatus == FT_SCIENCE_UP_GRADE_FAIL)
	{
		NemberScience ns={0};
		pUser->m_userFightTeamNember.GetScienceInfo(pUpGrade->iScienceID,ns);

		CT_INT32 iGrade=(sugs.iScienceStatus == FT_SCIENCE_UP_GRADE_SUC?ns.iScienceGrade:ns.iScienceGrade+1);
		G_FightTeamBless ftb={0};
		if(!G_GetFightTeamBless(pUpGrade->iScienceID,&ftb))
		{
			CCLogMeg::getSingleton().WriteError("CCUserFightTeamNember::DiposeFightTeamNemberScienceUp iScienceID:%d",pUpGrade->iScienceID);
			return;
		}

		G_BlessUpgrade bug={0};
		if(!G_GetBlessUpgrade(ftb.iUpGradeRule,(iGrade == 0?1:iGrade),&bug))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DiposeFightTeamNemberScienceUp UpGradeRule:%d iGrade:%d",ftb.iUpGradeRule,iGrade);
			return;
		}

		pFightTeam->CostContribute(pUser->GetUserID(),bug.iExpend);
		if(pFightTeam->GetFightTeamMember(1,pUser->GetUserID(),&FTMember))
		{
			sugs.iScienceGrade=ns.iScienceGrade;
			sugs.iContribution=FTMember.iContribution;
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SCIENCE_UP_GRADE_SUC,(CT_CHAR*)&sugs,sizeof(sugs));

			MasterVariableData mvd={0};
			pUser->m_userBase.GetMasterData(mvd);
			MSC_LOGDB_FightTeamScienceUpGrade LogSug={0};
			LogSug.dwFTID=pUpGrade->dwFTID;
			LogSug.dwUserID=pUser->GetUserID();
			LogSug.iScienceID=pUpGrade->iScienceID;
			LogSug.iScienceGrade=iGrade;
			LogSug.iCostType=VALUE_TYPE_FT_DONATE;
			LogSug.iCostNum=bug.iExpend;
			LogSug.iUserGrade=mvd.stGrade.iLevel;
			CT_DWORD threadID = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(threadID,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM_SCIENCE_UPGRADE,(CT_CHAR*)&LogSug,sizeof(LogSug));
		}
	}

	return;
}

//科技升级检测
CT_INT32 CCFightTeamEvent::FightTeamNemberScienceValidate(CCUser *pUser,CT_INT32 iScienceID,CCFightTeam *pFightTeam)
{
	G_FightTeamBless ftb={0};
	if(!G_GetFightTeamBless(iScienceID,&ftb))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamNemberScienceValidate iScienceID:%d",iScienceID);
		return FT_DATA_ERROR;
	}
    
    if (ftb.iAdditionType == FT_SCIENCE_BUILD && !pUser->m_userBuilding.BuildValidate(ftb.iAdditionObj))
    {
        // 不存在此类建筑
        CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamNemberScienceValidate iScienceID:%d(own FAIL)",iScienceID);
        return FT_SCIENCE_OBJ_OWN;
    }

    if (ftb.iAdditionType == FT_SCIENCE_FIGHTING && !pUser->m_userArmy.OwnArmyOfType(ftb.iAdditionObj))
    {
        // 不存在此类军队
        CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamNemberScienceValidate iScienceID:%d(own FAIL)",iScienceID);
        return FT_SCIENCE_OBJ_OWN;
    }

	NemberScience ns={0};
	pUser->m_userFightTeamNember.GetScienceInfo(iScienceID,ns);

	G_BlessUpgrade bug={0};
	if(!G_GetBlessUpgrade(ftb.iUpGradeRule,ns.iScienceGrade+1,&bug))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::FightTeamNemberScienceValidate UpGradeRule:%d iScienceGrade:%d",ftb.iUpGradeRule,ns.iScienceGrade+1);
		return FT_SCIENCE_MAX;
	}

	CMD_G_FightTeamBase ftbase={0};
	pFightTeam->GetFightTeamBase(&ftbase);
	if(ftbase.iGrade < bug.iTeamGrade)
	{
		return FT_GRADE_FAIL;
	}

	//if(ns.iScienceGrade+1 > ftbase.iGrade)
	//{
	//	return FT_GRADE_FAIL;
	//}

	CMD_G_FightTeamMember ftmember={0};
	pFightTeam->GetFightTeamMember(1,pUser->GetUserID(),&ftmember);
	if(ftmember.iContribution < -bug.iExpend)
	{
		return FT_NO_CONTRIB;
	}

	return FT_SCIENCE_UP_GRADE_SUC;
}

//战队召唤
CT_VOID CCFightTeamEvent::DisposeFightTeamSummonBoss(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamSummonBoss))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamSummonBoss len != sizeof(DisposeFightTeamSummonBoss)");
		return;
	}

	CMD_G_FightTeamSummonBoss *pSb=(CMD_G_FightTeamSummonBoss*)pBuf;

	CCUser *pUser=0;
	CT_DWORD dwUID = pSb->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonBoss,can't find user %d",pSb->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pSb->dwFTID, &pFightTeam))
	{ 
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonBoss,can't find FightTeam %d",pSb->dwFTID);
		return;
	}

	FightTeamMemberUID ftMUID={0};
	pFightTeam->GetFightTeamMemberUID(&ftMUID);
	CMD_G_FightTeamHelpSummonInfo hsinfo={0};
	hsinfo.dwUserID=pUser->GetUserID();
	hsinfo.iErrorID=SummonBossValidate(pUser,pFightTeam,pSb->iBossIndex,FIGHTTEAM_BOSS_SUMMON);
	if(hsinfo.iErrorID != 0)
	{
		if(hsinfo.iErrorID == FT_SUMMON_NO_FREE)
		{
			FightTeamSummonBoss FTSB={0};
			pFightTeam->GetSummonInfo(FTSB);

			hsinfo.iBossIndex=FTSB.iBossIndex;
			hsinfo.iErrorID=FT_SUMMON_HELP_SUC;
			memcpy(hsinfo.dwHelpSummonID,FTSB.dwUserID,sizeof(FTSB.dwUserID));
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_HELP_SUMMON_INFO,(CT_CHAR*)&hsinfo,sizeof(hsinfo));
			return;
		}
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_BOSS_INFO,(CT_CHAR*)&hsinfo,sizeof(hsinfo));
		return;
	}

	hsinfo.dwHelpSummonID[0]=pUser->GetUserID();
	hsinfo.iBossIndex=pSb->iBossIndex;
	for(CT_INT32 i=0;i<ftMUID.dwCount;++i)
	{
		CCUser *pDstUser=0;
		if(ftMUID.dwMemberUID[i] == 0)continue;
		if(CCUserManager::getSingleton().Find(ftMUID.dwMemberUID[i],&pDstUser))
		{
			pDstUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_BOSS_INFO,(CT_CHAR*)&hsinfo,sizeof(hsinfo));
		}
	}

	FightTeamSummonBoss FTSB={0};
	FTSB.dwFTID=pSb->dwFTID;
	FTSB.iBossIndex=pSb->iBossIndex;
	FTSB.dwUserID[0]=pSb->dwUserID;
	pFightTeam->SetSummonInfo(FTSB);

	//召唤成功，判断召唤任务
	G_FightTeamUpgrade ftu = {0};
	if (G_GetFightTeamUpgrade(FTSB.iBossIndex,FIGHTTEAM_BOSS_SUMMON, &ftu))
	{
		CMD_G_TaskSystemInfo taskSysInfo = {0};
		taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
		taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_CALLBOSS;
		taskSysInfo.arrayParam[0] = ftu.iOpenBoss;       //参数0：Boss id
		taskSysInfo.iProgress = 1;
		pUser->m_userTask.DisposeTask(taskSysInfo);
	}

	return;
}

//召唤验证
CT_INT32 CCFightTeamEvent::SummonBossValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iBossIndex,CT_INT32 iBossType)
{
	G_FightTeamUpgrade ftu={0};
	if(!G_GetFightTeamUpgrade(iBossIndex,iBossType,&ftu))
	{
	return FT_DATA_ERROR;
	}

	G_CostRewardGroup cg={0};
	if(ftu.iBossExpend != 0 && !G_GetCostRewardGroup(ftu.iBossExpend,&cg))
	{
		return FT_DATA_ERROR;
	}

	FightTeamSummonBoss FTSB={0};
	pFightTeam->GetSummonInfo(FTSB);
	if(FTSB.dwUserID[0] != 0 && iBossType == FIGHTTEAM_BOSS_SUMMON)
	{
		return FT_SUMMON_NO_FREE;
	}

	CMD_G_FightTeamBase ftbase={0};
	pFightTeam->GetFightTeamBase(&ftbase);
	if(ftbase.dwBossUID != pUser->GetUserID())
	{
		return FT_NO_BOSS;
	}

	if(ftbase.iSummonCount >= SUMMON_NUMBER && iBossType == FIGHTTEAM_BOSS_SUMMON)
	{
		return FT_SUMMON_NO_NUM;
	}

	if(ftbase.iGrade < ftu.iTeamGrade)
	{
		return FT_GRADE_FAIL;
	}

	if(ftbase.iResource < -cg.res[0].iNum)
	{
		return FT_NO_CONTRIB;
	}

	if(iBossType == FIGHTTEAM_BOSS_TIME)
	{
		CT_INT64 lTime=time(0);
		CooldownData cd={0};
		pUser->m_userBase.GetCooldownData(FTTIMEBOSS_SUMMON,cd);
		if(cd.leftTime+ cd.toggleTime > lTime)
		{
			return FT_TIMEBOSS_SUMMON;
		}
	}
	

	return 0;
}

//召唤助阵
CT_VOID CCFightTeamEvent::DisposeFightTeamHelpSummon(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamHelpSummon))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamHelpSummon len != sizeof(CMD_G_FightTeamHelpSummon)");
		return;
	}

	CMD_G_FightTeamHelpSummon *pHs=(CMD_G_FightTeamHelpSummon*)pBuf;

	CCUser *pUser=0;
	CT_DWORD dwUID = pHs->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamHelpSummon,can't find user %d",pHs->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pHs->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamHelpSummon,can't find FightTeam %d",pHs->dwFTID);
		return;
	}

	FightTeamSummonBoss FTSB={0};
	CMD_G_FightTeamHelpSummonInfo hsinfo={0};
	hsinfo.dwUserID=pUser->GetUserID();
	hsinfo.iErrorID=HelpSummonValidate(pUser,pFightTeam);
	if(hsinfo.iErrorID == FT_SUMMON_HELP_SUC)
	{
		CMD_G_FightTeamMember ftMember={0};
		if(pFightTeam->GetFightTeamMember(1,pUser->GetUserID(),&ftMember))
		{
			memset(&FTSB,0,sizeof(FTSB));
			FTSB.dwFTID=ftMember.dwFTID;
			FTSB.dwUserID[0]=pUser->GetUserID();
			pFightTeam->SetSummonInfo(FTSB);

			memset(&FTSB,0,sizeof(FTSB));
			pFightTeam->GetSummonInfo(FTSB);

			hsinfo.iBossIndex=FTSB.iBossIndex;
			memcpy(hsinfo.dwHelpSummonID,FTSB.dwUserID,sizeof(hsinfo.dwHelpSummonID));
			memcpy(hsinfo.dwHelpSummonID,FTSB.dwUserID,sizeof(hsinfo.dwHelpSummonID));
            pFightTeam->SendSummonMessage(hsinfo.dwHelpSummonID,MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_HELP_SUMMON_INFO,(CT_CHAR*)&hsinfo,sizeof(hsinfo));

			//协助召唤成功，判断召唤任务
			G_FightTeamUpgrade ftu = {0};
			if (G_GetFightTeamUpgrade(FTSB.iBossIndex,FIGHTTEAM_BOSS_SUMMON, &ftu))
			{
				CMD_G_TaskSystemInfo taskSysInfo = {0};
				taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
				taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_CALLBOSS;
				taskSysInfo.arrayParam[0] = ftu.iOpenBoss;       // 参数0：Boss id
				taskSysInfo.iProgress = 1;
				pUser->m_userTask.DisposeTask(taskSysInfo);
			}

            return;
		}
	}

	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_HELP_SUMMON_INFO,(CT_CHAR*)&hsinfo,sizeof(hsinfo));
	
	return;
}

//助阵验证
CT_INT32 CCFightTeamEvent::HelpSummonValidate(CCUser *pUser,CCFightTeam *pFightTeam)
{
	CMD_G_FightTeamMember ftMember={0};
	if(!pFightTeam->GetFightTeamMember(1,pUser->GetUserID(),&ftMember))
	{
		return FT_MEMBER_INVALID;
	}

	if(pFightTeam->GetSummonCount() >= SUMMON_MAX)
	{
		return FT_SUMMON_MAX;
	}

	if(pFightTeam->GetSummonCount() <= 0)
	{
		return FT_NO_SUMMON;
	}

	FightTeamSummonBoss FTSB={0};
	pFightTeam->GetSummonInfo(FTSB);
	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(FTSB.dwUserID[i] == pUser->GetUserID())
		{
			return FT_SUMMON_NO_FREE;
		}
	}

	if(FTSB.iWiner == FT_SUMMON_WIN)
		return FT_SUMMON_WIN;	

	return FT_SUMMON_HELP_SUC;
}

//召唤奖励
CT_VOID CCFightTeamEvent::DisposeFightTeamSummonReward(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamSummonReward))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamSummonReward len != sizeof(CMD_G_FightTeamSummonReward)");
		return;
	}

	CMD_G_FightTeamSummonReward *pSr=(CMD_G_FightTeamSummonReward*)pBuf;

	CCUser *pUser=0;
	CT_DWORD dwUID = pSr->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonReward,can't find user %d",pSr->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pSr->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonReward,can't find FightTeam %d",pSr->dwFTID);
		return;
	}


	CT_INT32 iItem[GOODS_COUNT*2+2]={0};
	CT_INT32 iItemCount[GOODS_COUNT*2+2]={0};
	CMD_G_NormalReward nr={0};
	G_FightTeamUpgrade FTUG={0};
	FightTeamSummonBoss FTSB={0};
	CMD_G_FightTeamSummonRewardFail FTRF={0};
	FakeProBase fb={0};

	G_GetFakeProDta(pUser,SAVETAGEEX_FT_CHEST,fb);
	pFightTeam->GetSummonInfo(FTSB);
	FTRF.dwUserID=pUser->GetUserID();
	FTRF.iErrorID=SummonRewardValidate(pUser,pFightTeam,pSr->iUserIndex,pSr->iChestIndex,pSr->dwFTID);

	if(FTRF.iErrorID != FT_SUMMON_REWARD_SUC)
	{
	}
	else if(!G_GetFightTeamUpgrade(FTSB.iBossIndex,FIGHTTEAM_BOSS_SUMMON,&FTUG))
	{
		FTRF.iErrorID=FT_DATA_ERROR;
	}
	else if(G_PerformCostReward(FTUG.iBossReward,pUser,ENUM_PCRT_TEST,nr,iItem,iItemCount,fb) != ENUM_CREC_RES_SUC)
	{
		FTRF.iErrorID=FT_NO_BAG_ROOM;
	}
	else
	{
		MSC_LOGDB_FightTeamSummonRewardItem FTSummonReward={0};
		CMD_G_FightTeamSummonRewardInfo FTSR={0};
		FTSR.dwUserID=pUser->GetUserID();
		FTSR.iChestIndex=pSr->iChestIndex;
		FTSB.dwChestIndex[pSr->iChestIndex]=pUser->GetUserID();
		G_PerformCostReward(FTUG.iBossReward,pUser,ENUM_PCRT_PERFORM,nr,iItem,iItemCount,fb);
		G_ClearFakeProDat(pUser,SAVETAGEEX_FT_CHEST,fb);

		CT_CHAR cBuf[1024]={0};
		CT_INT32 iSize=0;
		for(CT_INT32 i=0;i<GOODS_COUNT*2+2;++i)
		{
			if(iItem[i] != 0 && iItemCount[i] != 0)
			{
				FightTeamSummonItem item={0};
				item.dwUserID=pUser->GetUserID();
				item.iChestIndex=pSr->iChestIndex;
				item.iItemID=iItem[i];
				item.iCount=iItemCount[i];
				if(1024 > iSize+sizeof(item))
				{
					memcpy(cBuf+iSize,&item,sizeof(item));
					iSize+=sizeof(item);
				}
				else
					CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamSummonReward 1024 > iSize+sizeof(item)");

				if (G_BroadcastFilter(8, iItem[i]))
				{
					CT_CHAR cFightTeamName[NAME_SIZE] = {0};
					MasterData masterData = {0};
					RollBroadcast rb[4]={0};
					pUser->m_userBase.GetMasterData(masterData);
					rb[0].SetValue(0,cFightTeamName);
					rb[1].SetValue(dwUID, masterData.cNickName);
					rb[2].SetValue(FTSB.iBossIndex,"");
					rb[3].SetValue(iItem[i],"");
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<6:%s>{17}<1:%d:%s>{56}<15:%d>{57}<2:%d>", 
						//cFightTeamName, dwUID, masterData.cNickName, FTSB.iBossIndex, iItem[i]);
					G_SendRollBroadecast(17,3,60,(CT_CHAR*)rb,sizeof(rb));
				}
			}
		}
		memset(&nr,0,sizeof(nr));
		nr.dwUserID=pUser->GetUserID();
		pFightTeam->SetSummonReward(pSr->iUserIndex,pSr->iChestIndex,nr,iItem,iItemCount);
		pFightTeam->SendSummonMessage(FTSB.dwUserID,MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_REWARD_ITEM,cBuf,iSize);

		
		//插入日志
		MasterVariableData mvd={0};
		pUser->m_userBase.GetMasterData(mvd);
		FTSummonReward.dwUserID=pSr->dwUserID;
		FTSummonReward.dwFTID=pSr->dwFTID;
		FTSummonReward.iBossIndex=FTSB.iBossIndex;
		FTSummonReward.iGarde=mvd.stGrade.iLevel;

		CT_INT32 iCount=0;
		for(CT_INT32 i=0;i<GOODS_COUNT*2+2;++i)
		{
			if(iItem[i] != 0 && iItemCount[i] != 0)
			{
				FTSummonReward.iItemID[iCount]=iItem[i];
				FTSummonReward.iItemCount[iCount]=iItemCount[i];
				iCount++;

				if(iCount%4 == 0 && iCount != 0)
				{
					CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pSr->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
					G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDITEM,(CT_CHAR*)&FTSummonReward,sizeof(FTSummonReward));
					iCount=0;
					memset(FTSummonReward.iItemID,0,sizeof(FTSummonReward.iItemID));
					memset(FTSummonReward.iItemCount,0,sizeof(FTSummonReward.iItemCount));
				}
			}
		}

		for(CT_INT32 i=0;i<4;++i)
		{
			if(FTSummonReward.iItemID[i] != 0 && FTSummonReward.iItemCount[i] != 0)
			{
				CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pSr->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDITEM,(CT_CHAR*)&FTSummonReward,sizeof(FTSummonReward));
				break;
			}
		}

		return;
	}


	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_REWARD_FAIL,(CT_CHAR*)&FTRF,sizeof(FTRF));
	return;
}

//领奖验证
CT_INT32 CCFightTeamEvent::SummonRewardValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iUserIndex,CT_INT32 iChestIndex,CT_DWORD dwFTID)
{
	FightTeamSummonBoss FTSB={0};
	pFightTeam->GetSummonInfo(FTSB);

	if(!(iChestIndex < SUMMON_MAX*2 && FTSB.dwChestIndex[iChestIndex] == 0))
	{
		return FT_SUMMON_REWARD;
	}

	for(CT_INT32 i=0;i<SUMMON_MAX*2;++i)
	{
		if(FTSB.dwChestIndex[i] == pUser->GetUserID() && i < SUMMON_MAX && iChestIndex < SUMMON_MAX ||
			FTSB.dwChestIndex[i] == pUser->GetUserID() && i >= SUMMON_MAX && iChestIndex >= SUMMON_MAX)
		{
			return FT_SUMMON_REWARD;
		}
	}

	if(FTSB.dwUserID[iUserIndex] != pUser->GetUserID())
	{
		return FT_NO_SUMMON_NEMBER;
	}

	CT_INT32 iNum=0;
	for(CT_INT32 i=0;i<SUMMON_MAX*2;++i)
	{
		if(FTSB.dwChestIndex[i] == pUser->GetUserID())
		{
			iNum++;
		}
	}

	if(iNum > 1+pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_CONTRIBUTE))
	{
		return FT_SUMMON_REWARD_MAX;
	}

	if(pUser->m_userVip.VipGradeGet() == 0 && iChestIndex >= SUMMON_MAX)
	{
		return FT_NO_VIP;
	}
	return FT_SUMMON_REWARD_SUC;
}
//助阵离开
CT_VOID CCFightTeamEvent::DiposeFightTeamHelpSummonLeave(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamHelpSummonLeave))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DiposeFightTeamHelpSummonLeave len != sizeof(CMD_G_FightTeamHelpSummonLeave)");
		return;
	}

	CMD_G_FightTeamHelpSummonLeave *pFTHsl=(CMD_G_FightTeamHelpSummonLeave*)pBuf;

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFTHsl->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DiposeFightTeamHelpSummonLeave,can't find FightTeam %d",pFTHsl->dwFTID);
		return;
	}

	if(pFightTeam->GetSummonFightingStatus())return;

	CT_DWORD dwUserID[SUMMON_MAX]={0};
	FightTeamSummonBoss FTSB={0};
	CMD_G_FightTeamHelpSummonInfo hsinfo={0};
	pFightTeam->GetSummonInfo(FTSB);
	if(FTSB.iWiner == FT_SUMMON_WIN)return;
	memcpy(dwUserID,FTSB.dwUserID,sizeof(dwUserID));
	memset(&hsinfo,0,sizeof(hsinfo));
	hsinfo.iErrorID=pFightTeam->HelpSummonLeave(pFTHsl->dwUserID,FTSB);
	hsinfo.iBossIndex=FTSB.iBossIndex;
	hsinfo.dwUserID=pFTHsl->dwUserID;

	memset(&FTSB,0,sizeof(FTSB));
	pFightTeam->GetSummonInfo(FTSB);
	memcpy(hsinfo.dwHelpSummonID,FTSB.dwUserID,sizeof(hsinfo.dwHelpSummonID));

	pFightTeam->SendSummonMessage(dwUserID,MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_HELP_SUMMON_INFO,(CT_CHAR*)&hsinfo,sizeof(hsinfo));

	return;
}

//召唤战斗
CT_VOID CCFightTeamEvent::DisposeFightTeamSummonFighting(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamFighting))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamSummonFighting len != sizeof(CMD_G_FightTeamFighting)");
		return;
	}

	CMD_G_FightTeamFighting *pFTF=(CMD_G_FightTeamFighting*)pBuf;

	CCUser *pUser=0;
	CT_DWORD dwUID = pFTF->dwUserID;
	if (!CCUserManager::getSingleton().Find(dwUID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonFighting,can't find user %d",pFTF->dwUserID);
		return;
	}

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFTF->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonFighting,can't find FightTeam %d",pFTF->dwFTID);
		return;
	}

	CMD_G_FightTeamFightingInfo FTFI={0};
	CMD_G_FightTeamBase ftbase={0};
	FTFI.dwUserID=pUser->GetUserID();
	pFightTeam->GetFightTeamBase(&ftbase);
	if(ftbase.dwBossUID != pUser->GetUserID())
	{
		FTFI.dwUserID=pUser->GetUserID();
		FTFI.iErrorID=FT_NO_BOSS;
		pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_FIGHTING_INFO,(CT_CHAR*)&FTFI,sizeof(FTFI));
		return;
	}

	FightTeamSummonBoss FTSB={0};
	pFightTeam->GetSummonInfo(FTSB);

	//召唤检测
	CT_INT32 iCount=0;
	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(FTSB.dwUserID[i] != 0)
			iCount++;
	}
	if(iCount < SUMMON_NUMBER)
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,FT_TIMEBOSS_FIGHT_NUM);
		//CMD_G_FightTeamHelpSummonInfo hsinfo={0};
		//hsinfo.dwUserID=pUser->GetUserID();
		//hsinfo.iErrorID= FT_TIMEBOSS_FIGHT_NUM;
		//pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_BOSS_INFO,(CT_CHAR*)&hsinfo,sizeof(hsinfo));
		return;
	}

	//发送战斗线程
	MSC_FightTeamSummonFighting FTSM={0};
	FTSM.dwFTID=pFTF->dwFTID;
	FTSM.iBossIndex=FTSB.iBossIndex;
	memcpy(FTSM.dwUserID,FTSB.dwUserID,sizeof(FTSB.dwUserID));
	pFightTeam->GetUserFightData(FTSM.FightingData,3,CT_TRUE);
	SummonFightingAdd(FTSM.FightingData,pFightTeam->GetSummonCount(),FIGHTTEAM_BOSS_SUMMON);

	CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_FT_SUMMON_BOSS,(CT_CHAR*)&FTSM,sizeof(FTSM));

	pFightTeam->SetSummonFightingStatus(CT_TRUE);
	
	return;
}

//召唤战斗结果
CT_VOID CCFightTeamEvent::DisposeFightTeamSummonFightingEnd(CT_CHAR *pBuf,CT_DWORD len)
{
	if (len != sizeof(MSC_FightTeamSummonFighting))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonFightingEnd: len != sizeof(MSC_FightTeamSummonFighting)");
		return;
	}

	MSC_FightTeamSummonFighting* pFTSF = (MSC_FightTeamSummonFighting*)pBuf;

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFTSF->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamSummonFightingEnd,can't find FightTeam %d",pFTSF->dwFTID);
		return;
	}

	G_FightTeamUpgrade FTUG={0};
	if(!G_GetFightTeamUpgrade(pFTSF->iBossIndex,FIGHTTEAM_BOSS_SUMMON,&FTUG))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamSummonFightingEnd pFTSF->iBossIndex%d",pFTSF->iBossIndex);
		return;
	}

	G_CostRewardGroup rewardGroup = {0};
	if(!G_GetCostRewardGroup(FTUG.iBossReward, &rewardGroup))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamSummonFightingEnd iRewardID",FTUG.iBossReward);
		return ;
	}

	pFightTeam->SetSummonFightingStatus(CT_FALSE);
	if(pFTSF->iWin != WINER_LIFT)return;

	//计算奖励
	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		MasterData md={0};
		MasterVariableData mvd={0};
		CMD_G_NormalReward nr={0};
		CMD_G_FightTeamSummonRewardInfo FTSR={0};
		MSC_LOGDB_FightTeamSummonRewardRes FTSummonReward={0};
		CT_INT32 iItem[GOODS_COUNT*2+2]={0};
		CT_INT32 iItemCount[GOODS_COUNT*2+2]={0};
		CCUser *pUser=0;

		if(!CCUserManager::getSingleton().Find(pFTSF->dwUserID[i],&pUser))
		{
			continue;
		}
		pUser->m_userBase.GetMasterData(md);

		FTSR.dwUserID=pFTSF->dwUserID[i];
		nr.dwUserID=pFTSF->dwUserID[i];
		nr.iResType0=rewardGroup.res[0].iID;
		nr.iResNum0=rewardGroup.res[0].iNum;
		nr.iResType1=rewardGroup.res[1].iID;
		nr.iResNum1=rewardGroup.res[1].iNum;
		nr.iResType2=rewardGroup.res[2].iID;
		nr.iResNum2=rewardGroup.res[2].iNum;
		nr.iResType3=rewardGroup.res[3].iID;
		nr.iResNum3=rewardGroup.res[3].iNum;

		//公式计算资源
		nr.iResNum0=G_GetFightTeamSummonBoss(nr.iResNum0,md.factor.fBattleFactor);
		nr.iResNum1=G_GetFightTeamSummonBoss(nr.iResNum1,md.factor.fBattleFactor);
		nr.iResNum2=G_GetFightTeamSummonBoss(nr.iResNum2,md.factor.fBattleFactor);
		nr.iResNum3=G_GetFightTeamSummonBoss(nr.iResNum3,md.factor.fBattleFactor);
		G_ChangeResType(mvd,nr.iResType0,nr.iResNum0);
		G_ChangeResType(mvd,nr.iResType1,nr.iResNum1);
		G_ChangeResType(mvd,nr.iResType2,nr.iResNum2);
		G_ChangeResType(mvd,nr.iResType3,nr.iResNum3);
		pUser->m_userBase.AddMastData(mvd,SAVETAG_FIGHTTEAM_SUMMONBOSS);

		CT_INT32 iRes[4]={0};
		memset(&mvd,0,sizeof(mvd));
		pUser->m_userBase.GetMasterData(mvd);
		G_ChangeResType(mvd,nr.iResType0,iRes[0]);
		G_ChangeResType(mvd,nr.iResType1,iRes[1]);
		G_ChangeResType(mvd,nr.iResType2,iRes[2]);
		G_ChangeResType(mvd,nr.iResType3,iRes[3]);
		FTSR.Res.iResNum0=iRes[0];
		FTSR.Res.iResNum1=iRes[1];
		FTSR.Res.iResNum2=iRes[2];
		FTSR.Res.iResNum3=iRes[3];
		FTSR.Res.iResType0=nr.iResType0;
		FTSR.Res.iResType1=nr.iResType1;
		FTSR.Res.iResType2=nr.iResType2;
		FTSR.Res.iResType3=nr.iResType3;

		nr.dwUserID=pFTSF->dwUserID[i];
		pFightTeam->SetSummonReward(i,-1,nr,iItem,iItemCount);
		pFightTeam->SendSummonMessage(pFTSF->dwUserID,MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_REWARD_INFO,(CT_CHAR*)&FTSR,sizeof(FTSR));

		FTSummonReward.dwUserID=FTSR.dwUserID;
		FTSummonReward.dwFTID=pFTSF->dwFTID;
		FTSummonReward.iGarde=md.mvd.stGrade.iLevel;
		FTSummonReward.iBossIndex=pFTSF->iBossIndex;
		memcpy(&FTSummonReward.Reward,&nr,sizeof(nr));
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + FTSR.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FIGHTTEAM_SUMMONREWARDRES,(CT_CHAR*)&FTSummonReward,sizeof(FTSummonReward));
	}

	//战队基本信息
	CMD_G_FightTeamBase fightTeamBase = {0};
	pFightTeam->GetFightTeamBase(&fightTeamBase);
	if(fightTeamBase.iBossMaxIndex < pFTSF->iBossIndex)
	{
		fightTeamBase.iBossMaxIndex=pFTSF->iBossIndex;
		pFightTeam->Broadcast(MAIN_G_FIGHTTEAM_EVENT, SUB_G_FIGHTTEAM_BASE,(CT_CHAR*)&fightTeamBase, sizeof(CMD_G_FightTeamBase));
	}

	pFightTeam->SetSummonWin(pFTSF->iBossIndex);

	FightTeamSummonBoss ftsm={0};
	pFightTeam->GetSummonInfo(ftsm);

	CMD_G_FightTeamFightingInfo FTFI={0};

	for(CT_INT32 i=0;i<SUMMON_MAX;++i)
	{
		if(ftsm.dwUserID[i] == 0)continue;

		CCUser *pUser=0;
		if(CCUserManager::getSingleton().Find(ftsm.dwUserID[i],&pUser))
		{
			FTFI.dwUserID=ftsm.dwUserID[i];
			FTFI.iErrorID=FT_SUMMON_WIN;
			pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_FIGHTING_INFO,(CT_CHAR*)&FTFI,sizeof(FTFI));

			//击杀Boss，判断任务
			G_FightTeamUpgrade ftu = {0};
			G_GetFightTeamUpgrade(ftsm.iBossIndex,FIGHTTEAM_BOSS_SUMMON, &ftu);
			CMD_G_TaskSystemInfo taskSysInfo = {0};
			taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
			taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_FITBOSS;
			taskSysInfo.arrayParam[0] = ftu.iOpenBoss;        //参数0：Boss id
			taskSysInfo.iProgress = 1;
			pUser->m_userTask.DisposeTask(taskSysInfo);
		}
	}

	return ;
}

//召唤邀请
CT_VOID CCFightTeamEvent::DisposeFightTeamInvit(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_FightTeamInvit))
	{
		CCLogMeg::getSingleton().WriteError("len != sizeof(CMD_G_FightTeamInvit)");
		return;
	}

	CMD_G_FightTeamInvit *pFTInvit=(CMD_G_FightTeamInvit*)pBuf;

	CCFightTeam *pFightTeam = 0;
	if(!CCFightTeamManager::getSingleton().Find(pFTInvit->dwFTID, &pFightTeam))
	{
		CCLogMeg::getSingleton().WriteError("DisposeFightTeamInvit,can't find FightTeam %d",pFTInvit->dwFTID);
		return;
	}

	pFightTeam->Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_SUMMON_INVIT,pBuf,len);
	return;
}

//召唤战斗加成
CT_VOID CCFightTeamEvent::SummonFightingAdd(UserFightingStatus *FightingNember,const CT_INT32 iCount,const CT_INT32 iType)
{
	G_SummonAttr sa={0};
	if(!G_GetSummonAttr(iCount,iType,&sa))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::SummonFightingAdd G_GetSummonAttr iCount:%d",iCount);
		return;
	}

	G_AttributesReward ar={0};
	if(!G_GetAttributesReward(sa.iAttrID,&ar))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::SummonFightingAdd G_GetAttributesReward iAttrID:%d",sa.iAttrID);
		return;
	}

	for(CT_INT32 i=0;i<CLAN_CRUSADE_MEMBER;++i)
	{
		if(FightingNember[i].dwUserID != 0)
		{
			for(CT_INT32 j=0;j<POST_COUNT/2;++j)
			{
				if(FightingNember[i].units[j].type == 0)continue;
				CCUserBase::UpdateBattleStatus(FightingNember[i].units[j].mcs,ar.iResType1,ar.fResNum1);
				CCUserBase::UpdateBattleStatus(FightingNember[i].units[j].mcs,ar.iResType2,ar.fResNum2);
				CCUserBase::UpdateBattleStatus(FightingNember[i].units[j].mcs,ar.iResType3,ar.fResNum3);
				CCUserBase::UpdateBattleStatus(FightingNember[i].units[j].mcs,ar.iResType4,ar.fResNum4);
			}
			
		}
	}
	return;
}

CT_VOID CCFightTeamEvent::DiposeFightTeamTowerCP(CT_CHAR *pBuf,CT_DWORD len)
{
	if(len != sizeof(MSC_Tower_Pass))
	{
		CCLogMeg::getSingleton().WriteError("len != sizeof(MSC_Tower_Pass)");
		return;
	}

	MSC_Tower_Pass *pTowerPass=(MSC_Tower_Pass*)pBuf;

	CT_DWORD dwFTID = CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pTowerPass->dwUserID);
	CCFightTeam *pFightTeam = 0;
	if (CCFightTeamManager::getSingleton().Find(dwFTID, &pFightTeam))
	{
		pFightTeam->SetMemberTowerCP(pTowerPass->dwUserID, pTowerPass->iCPID);
	}
}

CT_VOID CCFightTeamEvent::DisposeFightTeamUpgreadTask(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(MSC_FightTeamTask))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamUpgreadTask len != sizeof(MSC_FightTeamTask.");
		return;
	}
	MSC_FightTeamTask *pFightTeamTask = (MSC_FightTeamTask*)pBuf;

	CCUser *pUser;
	CCFightTeam *pft;
	MasterData md = {0};
	CMD_G_FightTeamBase base = {0};

	if (!CCUserManager::getSingleton().Find(pFightTeamTask->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamUpgreadTask find pUser error.");
		return;
	}
	
	pUser->m_userBase.GetMasterData(md);
	if (!CCFightTeamManager::getSingleton().Find(md.dwFTID, &pft))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamUpgreadTask find fightteam error.");
		return;
	}

	if (!pft->GetFightTeamBase(&base))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFightTeamUpgreadTask GetFightTeamBase error.");
		return;
	}

	pFightTeamTask->iFightTeamGrade = base.iGrade;
	
	CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	G_SendThreadInfo(threadid,MSC_MAIN_TASK_EVENT_DATA,MSC_SUB_TASK_FIGHTTEAM_EVENT, (CT_CHAR*)pFightTeamTask, sizeof(MSC_FightTeamTask));
}

//修改战队名称
CT_VOID CCFightTeamEvent::DisposeChangeFTName(CT_CHAR *pBuf, CT_DWORD len)
{
	if(len != sizeof(MSC_DB_FTName))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeChangeFTName len != sizeof(MSC_DB_FTName)");
		return;
	}

	MSC_DB_FTName *p=(MSC_DB_FTName*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(p->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeChangeFTName No Find UserID:%d",p->dwUserID);
		return;
	}

	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(p->dwUserID);
	if(dwFTID == 0)
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeChangeFTName No Find UserID:%d FTID",p->dwUserID);
		return;
	}

	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeChangeFTName No Find dwFTID:%d",dwFTID);
		return;
	}

	if(p->dwUserID != pFT->GetFightTeamBossID())
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeChangeFTName p->dwUserID != pFT->GetFightTeamBossID()");
		return;
	}

	CMD_G_UseItemFail stFail = {0};
	CCFightTeam *pTempFT=0;
	if(CCFightTeamManager::getSingleton().Find(p->cName,&pTempFT) || strlen(p->cName) == 0)
	{
		stFail.enFail=EUI_FTNAME_SAME;
	}

	if(!pUser->m_userItem.AutoExpendItem(SAVATAG_CHANGE_FTNAME,p->iPropID,1,ITEM_OP_MODE_CHECK))
	{
		stFail.enFail=EUI_NO_EXIST;
	}

	stFail.dwUserID =pUser->GetUserID();
	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);

	if(stFail.enFail != 0)
	{
		return;
	}

	CT_CHAR cFTName[NAME_SIZE+1];
	pFT->GetFightTeamName(cFTName);
	pUser->m_userItem.AutoExpendItem(SAVATAG_CHANGE_MASTERNAME,p->iPropID,1,ITEM_OP_MODE_CKEXE);
	CCFightTeamManager::getSingleton().ChangeFTName(cFTName,p->cName);
	pFT->SetFTName(p->cName);

	//记录日志
	MSC_LOGDB_UserItem LogUserItem={0};
	LogUserItem.dwUserID=pUser->GetUserID();
	LogUserItem.iItemID=p->iPropID;
	LogUserItem.iItemNum=1;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_USESITEM,(CT_CHAR*)&LogUserItem,sizeof(LogUserItem));					


	//add by caihan
	//使用道具，判断道具任务
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_USE;
	taskEventInfo.iSubType = p->iPropID;
	pUser->m_userTask.DisposeTask(taskEventInfo);
	return;
}

//时限BOOS召唤
CT_VOID CCFightTeamEvent::DisposeFTTBSummon(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FTTimeBossSummon))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBSummon len != sizeof(CMD_G_FTTimeBossSummon.");
		return;
	}
	CMD_G_FTTimeBossSummon *pBs = (CMD_G_FTTimeBossSummon*)pBuf;

	CCUser *pUser=0;
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBs->dwUserID);
	if(!CCUserManager::getSingleton().Find(pBs->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBSummon No Find UserID:%d",pBs->dwUserID);
		return;
	}

	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBSummon No Find FTID:%d",dwFTID);
		return;
	}

	//清楚信息
	pFT->ClearTBInfo();

	CT_INT32 iError=SummonBossValidate(pUser,pFT,pBs->iBossID,FIGHTTEAM_BOSS_TIME);
	if(iError != 0)
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,iError);
		return;
	}

	CMD_G_FTTimeBossInfo bi={0};
	FTTimeBossInfo tbi={0};
	pFT->GetTimeBossInfo(tbi);
	bi.iBossID=pBs->iBossID;
	bi.dwUserID=pUser->GetUserID();
	tbi.iHelpUser[0]=pUser->GetUserID();
	memcpy(bi.dwHelpSummonID,tbi.iHelpUser,sizeof(bi.dwHelpSummonID));
	pUser->GetNetInterface()->Send(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_INFO,(CT_CHAR*)&bi,sizeof(bi));

	
	tbi.iIndex=pBs->iBossID;
	pFT->SetTimeBossInfo(tbi);
	tbi.iCurrentHP=pFT->GetTBMaxHP();
	tbi.iSummonBeginTime=time(0);
	pFT->SetTimeBossInfo(tbi);

	return;
}

//时限BOSS加入
CT_VOID CCFightTeamEvent::DisposeFTTBAdd(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FTTimeBossAdd))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBAdd len != sizeof(CMD_G_FTTimeBossAdd.");
		return;
	}
	CMD_G_FTTimeBossAdd *pBa = (CMD_G_FTTimeBossAdd*)pBuf;

	CCUser *pUser=0;
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBa->dwUserID);
	if(!CCUserManager::getSingleton().Find(pBa->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBAdd No Find UserID:%d",pBa->dwUserID);
		return;
	}

	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBAdd No Find FTID:%d",dwFTID);
		return;
	}

	CT_INT32 iError=FTTBAddValidate(pUser,pFT);
	if(iError != 0)
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,iError);
		return;
	}

	CMD_G_FTTimeBossInfo bi={0};
	FTTimeBossInfo tbi={0};
	pFT->GetTimeBossInfo(tbi);
	bi.dwUserID=pUser->GetUserID();
	bi.iBossID=tbi.iIndex;
	memcpy(bi.dwHelpSummonID,tbi.iHelpUser,sizeof(bi.dwHelpSummonID));
	pFT->Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_INFO,(CT_CHAR*)&bi,sizeof(bi));
	return;
}

//时限BOOS邀请
CT_VOID CCFightTeamEvent::DisposeFTTBInvite(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FTTimeBossInvite))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBInvite len != sizeof(CMD_G_FTTimeBossInvite.");
		return;
	}
	CMD_G_FTTimeBossInvite *pBi = (CMD_G_FTTimeBossInvite*)pBuf;

	CCUser *pUser=0;
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBi->dwUserID);
	if(!CCUserManager::getSingleton().Find(pBi->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBInvite No Find UserID:%d",pBi->dwUserID);
		return;
	}

	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBInvite No Find FTID:%d",dwFTID);
		return;
	}

	if(pFT->GetFightTeamBossID() != pUser->GetUserID())
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,FT_NO_BOSS);
		return;
	}

	pFT->Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_INVITE,(CT_CHAR*)&pBi->dwUserID,sizeof(pBi->dwUserID));

	return;
}

//时限BOOS开始
CT_VOID CCFightTeamEvent::DisposeFTTBBegin(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FTTimeBossBegin))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin len != sizeof(CMD_G_FTTimeBossBegin.");
		return;
	}
	CMD_G_FTTimeBossBegin *pBa = (CMD_G_FTTimeBossBegin*)pBuf;

	CCUser *pUser=0;
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBa->dwUserID);
	if(!CCUserManager::getSingleton().Find(pBa->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin No Find UserID:%d",pBa->dwUserID);
		return;
	}

	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin No Find FTID:%d",dwFTID);
		return;
	}

	if(pFT->GetFightTeamBossID() != pUser->GetUserID())
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,FT_NO_BOSS);
		return;
	}

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_FT_TIME_BOSS_FIGHTTIME,&vr))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin VALUE_TYPE_FT_TIME_BOSS_FIGHTTIME");
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,FT_DATA_ERROR);
		return;
	}

	FTTimeBossInfo tbi={0};
	CT_INT32 iCount=0;
	pFT->GetTimeBossInfo(tbi);
	for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
	{
		if(tbi.iHelpUser[i] != 0)
			iCount++;
	}
	if(iCount < HELP_SUMMON_FIGHT_COUNT)
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,FT_TIMEBOSS_FIGHT_NUM);
		return;
	}

	CMD_G_FTTimeBossFightInfo bfi={0};
	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_DWORD dwLen=MAX_MES_MAX_BUFF_LEN;;
	bfi.iBossID=tbi.iIndex;
	bfi.iFTBossTime=vr.fMaxVal;
	bfi.iBossMaxHP=tbi.iCurrentHP;
	bfi.iCurrentHP=tbi.iCurrentHP;
	pFT->GetFightTeamMember(cBuf,dwLen);
	for(CT_INT32 i=0;i<dwLen/sizeof(CMD_G_FightTeamMember) && i < HELP_SUMMON_COUNT;++i)
	{
		CMD_G_FightTeamMember ftm={0};
		memcpy(&ftm,cBuf+i*sizeof(CMD_G_FightTeamMember),sizeof(CMD_G_FightTeamMember));
		bfi.dwUserID[i]=ftm.dwUserID;
	}


	for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
	{
		if(tbi.iHelpUser[i] != 0)
			bfi.iMemberCount++;
	}
	pFT->Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_FIGHT_INFO,(CT_CHAR*)&bfi,sizeof(bfi));

	G_FightTeamUpgrade ftu={0};
	if(!G_GetFightTeamUpgrade(tbi.iIndex,FIGHTTEAM_BOSS_TIME,&ftu))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin G_GetFightTeamUpgrade bossIndex:%d",tbi.iIndex);
	}

	G_CostRewardGroup cg={0};
	if(!G_GetCostRewardGroup(ftu.iBossExpend,&cg))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin G_GetCostRewardGroup Index:%d",ftu.iBossExpend);
	}

	RewardCost rc={0};
	FakeProBase fb={0};
	G_GetCostReward(ftu.iBossExpend,pUser,ITEM_OP_MODE_CKEXE,SAVETAG_FIGHTTEAM_TIMEBOSS,rc,fb);
	pUser->UpdateAndSendMasterData();

	return;
}

//时限BOOS战斗
CT_VOID CCFightTeamEvent::DisposeFTTBFight(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FTTimeBossFight))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBFight len != sizeof(CMD_G_FTTimeBossFight.");
		return;
	}
	CMD_G_FTTimeBossFight *pBf = (CMD_G_FTTimeBossFight*)pBuf;

	CCUser *pUser=0;
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBf->dwUserID);
	if(!CCUserManager::getSingleton().Find(pBf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin No Find UserID:%d",pBf->dwUserID);
		return;
	}

	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBBegin No Find FTID:%d",dwFTID);
		return;
	}

	CT_INT32 iError=FTTBFightValidate(pUser,pFT);
	if(iError != 0)
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,iError);
		return;
	}

	//发送战斗线程
	MSC_FTTimeBossFight tbf={0};
	FTTimeBossInfo tbi={0};
	pFT->GetTimeBossInfo(tbi);
	tbf.dwFTID=dwFTID;
	tbf.iBossIndex=tbi.iIndex;
	tbf.dwUserID=pUser->GetUserID();
	tbf.iSeed=rand();
	G_GetFightingData(pUser,tbf.FightingData[0]);

	CT_INT32 iCount=0;
	for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
	{
		if(tbi.iHelpUser[i] != 0)
			iCount++;
	}
	SummonFightingAdd(tbf.FightingData,iCount,FIGHTTEAM_BOSS_TIME);

	CT_DWORD threadid = GAME_THREAD_FIGHT + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadFightNum;
	G_SendThreadInfo(threadid,MSC_MAIN_FIGHTING_EVENT,MSC_SUB_FT_TIME_BOSS,(CT_CHAR*)&tbf,sizeof(tbf));

	pUser->m_userArmy.DeleteBoss();

	return;
}

//时限BOOS战斗返回
CT_VOID CCFightTeamEvent::DisposeFTTBFightReturn(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(MSC_FTTimeBossFight))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBFightReturn len != sizeof(MSC_FTTimeBossFight.");
		return;
	}
	MSC_FTTimeBossFight *pBf = (MSC_FTTimeBossFight*)pBuf;

	CCUser *pUser=0;
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBf->dwUserID);
	if(!CCUserManager::getSingleton().Find(pBf->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBFightReturn No Find UserID:%d",pBf->dwUserID);
		return;
	}
	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBFightReturn No Find FTID:%d",dwFTID);
		return;
	}

	CMD_G_FTTimeBossFightReturn fbr={0};
	FTTimeBossInfo tbi={0};
	MasterVariableData mvd={0};
	FTTimeBossMember bm={0};
	pFT->GetTimeBossMember(pBf->dwUserID,bm);
	fbr.iHurt=pBf->iHurt;
	fbr.dwUserID=pBf->dwUserID;
	pFT->GetTimeBossInfo(tbi);
	tbi.iCurrentHP-=(tbi.iCurrentHP < pBf->iHurt?tbi.iCurrentHP:pBf->iHurt);
	pFT->SetTimeBossInfo(tbi);
	pUser->m_userBase.GetMasterData(mvd);

	G_ValueRange vrfood={0};
	G_ValueRange vrContribute={0};
	if(!G_GetValueRange(VALUE_TYPE_FT_TIME_BOSS_FOOD,&vrfood))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBFightReturn G_GetValueRange VALUE_TYPE_FT_TIME_BOSS_FOOD");
	}
	if(!G_GetValueRange(VALUE_TYPE_FT_TIME_BOSS_CONTRIBUTE,&vrContribute))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBFightReturn G_GetValueRange VALUE_TYPE_FT_TIME_BOSS_CONTRIBUTE");
	}
	fbr.iFTFood=vrfood.fMinVal*mvd.stGrade.iLevel+pBf->iHurt*vrfood.fMaxVal;
	fbr.iFTContribute=vrContribute.fMinVal*mvd.stGrade.iLevel+pBf->iHurt*vrContribute.fMaxVal;
	fbr.iCurrentHP=tbi.iCurrentHP;
	fbr.iStamina=mvd.stRes.iStamina;
	fbr.iFightNum=++bm.iFightNum;
	pFT->Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_FIGHT_RETURN,(CT_CHAR*)&fbr,sizeof(fbr));

	CMD_G_FightTeamMember ftm={0};
	bm.dwUserID=pBf->dwUserID;
	bm.iHurt=pBf->iHurt;
	bm.iFTFood+=fbr.iFTFood;
	bm.iFTContribute+=fbr.iFTContribute;
	pFT->UpdataTimeBossMember(bm);
	pFT->Contribute(pBf->dwUserID,fbr.iFTContribute);

	//战斗结果判断
	if(tbi.iCurrentHP <= 0)
	{
		CMD_G_FightTeamBase Tb={0};
		pFT->GetFightTeamBase(&Tb);
		if(Tb.iTimeBossMaxIndex < pBf->iBossIndex)
		{
			pFT->SetTBIndex(pBf->iBossIndex);

			//首通奖励
			G_FightTeamUpgrade ftu={0};
			if(!G_GetFightTeamUpgrade(pBf->iBossIndex,FIGHTTEAM_BOSS_TIME,&ftu))
			{
				CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBFightReturn pBf->iBossIndex:%d",pBf->iBossIndex);
				return;
			}

			CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
			CT_DWORD dwLen=MAX_MES_MAX_BUFF_LEN;
			pFT->GetFightTeamMember(cBuf,dwLen);
			for(CT_INT32 i=0;i<dwLen/sizeof(CMD_G_FightTeamMember) && i < HELP_SUMMON_COUNT;++i)
			{
				CCUser *pDstUser=0;
				CMD_G_FightTeamMember ftm={0};
				memcpy(&ftm,cBuf+i*sizeof(CMD_G_FightTeamMember),sizeof(CMD_G_FightTeamMember));

				if(CCUserManager::getSingleton().Find(ftm.dwUserID,&pDstUser))
				{
					CT_INT32 iCount=1;
					if(pDstUser->m_userItem.AutoAddItemData(SAVETAG_FIGHTTEAM_TIMEBOSS,ftu.iBossFistReward,iCount) == 0)
					{
						continue;
					}
				}

				MSC_DB_RESREWARD rr={0};
				rr.dwUID=ftm.dwUserID;
				rr.iPID=ftu.iBossFistReward;
				rr.iPNum=1;
				G_SendDBThreadInfo(ftm.dwUserID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_RES_PRODUCT_REWARD,(CT_CHAR*)&rr,sizeof(rr));
			}
		}

	}
	return;
}

//时限BOOS离开
CT_VOID CCFightTeamEvent::DisposeFTTBLeave(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FTTimeBossLeave))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBLeave len != sizeof(CMD_G_FTTimeBossLeave.");
		return;
	}
	CMD_G_FTTimeBossLeave *pBl = (CMD_G_FTTimeBossLeave*)pBuf;

	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBl->dwUserID);
	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBLeave No Find FTID:%d",dwFTID);
		return;
	}

	FTTimeBossInfo tbi={0};
	pFT->GetTimeBossInfo(tbi);
	for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
	{
		if(tbi.iHelpUser[i] == pBl->dwUserID)
		{
			tbi.iHelpUser[i]=0;
			break;
		}
	}

	//if(tbi.iCurrentHP <= pFT->GetTBMaxHP() && tbi.iCurrentHP != 0)
	//	return;

	CMD_G_FTTimeBossInfo bi={0};
	bi.dwUserID=pBl->dwUserID;
	bi.iBossID=tbi.iIndex;
	memcpy(bi.dwHelpSummonID,tbi.iHelpUser,sizeof(bi.dwHelpSummonID));
	pFT->Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_INFO,(CT_CHAR*)&bi,sizeof(bi));

	pFT->SetTimeBossInfo(tbi);

	if(pBl->dwUserID == pFT->GetFightTeamBossID())
		pFT->ClearTBInfo();
	return;
}

//时限BOOS奖励
CT_VOID CCFightTeamEvent::DisposeFTTBReward(CT_CHAR *pBuf, CT_DWORD len)
{
	if (len != sizeof(CMD_G_FTTimeBossReward))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBReward len != sizeof(CMD_G_FTTimeBossReward.");
		return;
	}
	CMD_G_FTTimeBossReward *pBr = (CMD_G_FTTimeBossReward*)pBuf;

	CCUser *pUser=0;
	CCFightTeam *pFT=0;
	CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pBr->dwUserID);
	if(!CCUserManager::getSingleton().Find(pBr->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBReward No Find UserID:%d",pBr->dwUserID);
		return;
	}

	if(!CCFightTeamManager::getSingleton().Find(dwFTID,&pFT))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::DisposeFTTBReward No Find FTID:%d",dwFTID);
		return;
	}

	CT_INT32 iError=TBRewardValidate(pUser,pFT,pBr->iIndex);
	if(iError != 0)
	{
		G_SendErrorInfo(pUser,ERE_FIGHTTEAM,iError);
		return;
	}

	CMD_G_FTTBRewardInfo ri={0};
	pFT->GetTBReward(pBr->iIndex,ri);
	pFT->Broadcast(MAIN_G_FIGHTTEAM_EVENT,SUB_G_FIGHTTEAM_TIMEBOSS_REWARD_INFO,(CT_CHAR*)&ri,sizeof(ri));
	return;
}

//时限boss加入验证
CT_INT32 CCFightTeamEvent::FTTBAddValidate(CCUser *pUser,CCFightTeam *pFightTeam)
{
	FTTimeBossInfo tbi={0};
	pFightTeam->GetTimeBossInfo(tbi);

	for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
	{
		if(pUser->GetUserID() == tbi.iHelpUser[i])
		{
			return FT_SUMMON_NO_FREE;
		}
	}


	for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
	{
		if(tbi.iHelpUser[i] == 0)
		{
			tbi.iHelpUser[i]=pUser->GetUserID();
			break;
		}
	}
	pFightTeam->SetTimeBossInfo(tbi);

	return 0;
}

//时限boss战斗验证
CT_INT32 CCFightTeamEvent::FTTBFightValidate(CCUser *pUser,CCFightTeam *pFightTeam)
{
	FTTimeBossInfo tbi={0};
	pFightTeam->GetTimeBossInfo(tbi);
	if(tbi.iIndex == 0)
	{
		return FT_SUMMON_NO_BOSS;
	}

	G_FightTeamUpgrade ftu={0};
	if(!G_GetFightTeamUpgrade(tbi.iIndex,FIGHTTEAM_BOSS_TIME,&ftu))
	{
		return FT_DATA_ERROR;
	}

	if(tbi.iCurrentHP <= 0)
		return FT_SUMMON_NO_BOSS;

	CT_INT64 lTime=time(0);
	CooldownData cd={0};
	pUser->m_userBase.GetCooldownData(FTTIMEBOSS_FIGHT,cd);
	if(cd.leftTime+ cd.toggleTime < lTime)
	{
		pFightTeam->ClearTBMemberFightCount();

		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_FT_TIME_BOSS_FIGHT,&vr))
		{
			CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::SummonBossValidate G_GetValueRange VALUE_TYPE_FT_TIME_BOSS_FIGHT");
			return FT_DATA_ERROR;
		}
		pUser->m_userBase.UpdateCooldown(FTTIMEBOSS_FIGHT,vr.fMaxVal);
	}

	MasterVariableData mvd={0};
	FTTimeBossMember bm={0};
	pUser->m_userBase.GetMasterData(mvd);
	pFightTeam->GetTimeBossMember(pUser->GetUserID(),bm);
	if((mvd.stRes.iStamina + ftu.iBossFightExpend*pow(2.0f,bm.iFightNum)) < 0 && bm.iFightNum > 0)
	{
		return FT_TIMEBOSS_STAMINA;
	}

	G_ValueRange vrbs={0};
	if(!G_GetValueRange(VALUE_TYPE_FT_TIME_BOSS_SUMMON,&vrbs))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::SummonBossValidate G_GetValueRange VALUE_TYPE_FT_TIME_BOSS_SUMMON");
		return FT_DATA_ERROR;
	}

	memset(&cd,0,sizeof(cd));
	pUser->m_userBase.GetCooldownData(FTTIMEBOSS_SUMMON,cd);
	if(cd.leftTime+ cd.toggleTime < lTime)
	{

		cd.iType=FTTIMEBOSS_SUMMON;
		cd.toggleTime=lTime;
		cd.leftTime=tbi.iSummonBeginTime+vrbs.fMaxVal-lTime;
		pUser->m_userBase.InsertCooldownData(cd);
		pUser->m_userBase.UpdateCooldown(FTTIMEBOSS_SUMMON,0);
	}
	else if(cd.leftTime+ cd.toggleTime != tbi.iSummonBeginTime+vrbs.fMaxVal)
		return FT_TIMEBOSS_SUMMON;

	memset(&mvd,0,sizeof(mvd));
	mvd.stRes.iStamina=(bm.iFightNum > 0?ftu.iBossFightExpend*pow(2.0f,bm.iFightNum):0);
	pUser->m_userBase.AddMastData(mvd,SAVETAG_FIGHTTEAM_TIMEBOSS);

	return 0;
}

//领奖验证
CT_INT32 CCFightTeamEvent::TBRewardValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iChestIndex)
{
	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iLen=MAX_MES_MAX_BUFF_LEN;
	FTTimeBossInfo bi={0};
	if(!pFightTeam->GetTBReward(cBuf,iLen))
		return FT_DATA_ERROR;
	pFightTeam->GetTimeBossInfo(bi);

	CT_INT32 iCount=0;
	CT_INT32 iPos=-1;
	for(CT_INT32 i=0;i<iLen/sizeof(FightTeamSummonItem);++i)
	{
		FightTeamSummonItem si={0};
		memcpy(&si,cBuf+i*sizeof(FightTeamSummonItem),sizeof(FightTeamSummonItem));
		if(si.dwUserID == pUser->GetUserID())
		{
			iCount++;
			iPos=si.iChestIndex;
		}
	}

	if(iCount >= 2)
	{
		return FT_SUMMON_REWARD_MAX;
	}

	if(iPos == iChestIndex)
	{
		return FT_SUMMON_REWARD;
	}

	if(pUser->m_userVip.VipGradeGet() == 0 && iChestIndex >= SUMMON_MAX)
	{
		return FT_NO_VIP;
	}

	G_FightTeamUpgrade FTUG={0};
	FakeProBase fb={0};
	RewardCost rc={0};
	if(!G_GetFightTeamUpgrade(bi.iIndex,FIGHTTEAM_BOSS_SUMMON,&FTUG))
	{
		CCLogMeg::getSingleton().WriteError("CCFightTeamEvent::TBRewardValidate G_GetFightTeamUpgrade bi.iIndex:%d",bi.iIndex);
		return FT_DATA_ERROR;
	}
	CT_INT32 iError=G_GetCostReward(FTUG.iBossReward,pUser,ITEM_OP_MODE_CHECK,SAVETAG_FIGHTTEAM_TIMEBOSS,rc,fb);
	if(iError != 0)
	{
		return FT_NO_BAG_ROOM;
	}
	G_GetCostReward(FTUG.iBossReward,pUser,ITEM_OP_MODE_EXEC,SAVETAG_FIGHTTEAM_TIMEBOSS,rc,fb);

	FightTeamSummonItem si={0};
	si.dwUserID=pUser->GetUserID();
	si.iChestIndex=iChestIndex;
	si.iItemID=rc.iUseItem[0];
	si.iCount=rc.iUseItemCount[0];
	pFightTeam->InsertTimeBossRewardr(si);
	return 0;
}


//删除时限boss信息
CT_VOID CCFightTeamEvent::DeleteTimeBossInfo(CT_DWORD dwUser,CCFightTeam *pFightTeam)
{
	FTTimeBossInfo tbi={0};
	pFightTeam->GetTimeBossInfo(tbi);
	pFightTeam->DeleteTBMember(dwUser);
	for(CT_INT32 i=0;i<HELP_SUMMON_COUNT;++i)
	{
		if(tbi.iHelpUser[i] == dwUser)
		{
			tbi.iHelpUser[i]=0;
			break;
		}
	}
	return;
}