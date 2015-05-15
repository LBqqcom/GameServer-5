#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CTaskEvent.h"
#include "CTaskScriptSetting.h"

#define  TASK_BUF_SIZE		(5*1024)

CCTaskEvent::CCTaskEvent(CCGameMesManager *pMesManager,CT_WORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_REWARD_REQUEST);
	RegistMessage(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_COMPLETE_FAST);
	RegistMessage(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_COMPLETE_INQUIRY);
	RegistMessage(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_ROLE);

	RegistMessage(MSC_MAIN_TASK_EVENT_DATA, MSC_SUB_TASK_FIGHTTEAM_EVENT);
}

CCTaskEvent::~CCTaskEvent(void)
{

}

CT_VOID CCTaskEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_TASK_EVENT_CITY:
		{
			if (subID==SUB_G_TASK_REWARD_REQUEST)
			{
				DisposeRewardRequest(pBuf,len);
			}
			else if (subID==SUB_G_TASK_COMPLETE_FAST)
			{
				DisposeTaskFastComplete(pBuf,len);
			}
			else if (subID==SUB_G_TASK_COMPLETE_INQUIRY)
			{
				DisposeTaskInquiry(pBuf,len);
			}
			else if (subID==SUB_G_TASK_ROLE)
			{
				DispostTaskRole(pBuf,len);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCTaskEvent::OnUserEvent : subID=%i", subID);
				return;
			}
			break;
		}
	case MSC_MAIN_TASK_EVENT_DATA:
		{
			if (subID == MSC_SUB_TASK_FIGHTTEAM_EVENT)
			{
				DisposeFightTeamUpgrade(pBuf,len);
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCTaskEvent::OnUserEvent : subID=%i", subID);
				return;
			}
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCTaskEvent::OnUserEvent : mainID=%i", mainID);
			break;
		}
	}
}

CT_VOID CCTaskEvent::DisposeRewardRequest(CT_CHAR* pBuf,CT_DWORD len)
{
	CMD_G_TaskRequest *taskReward = (CMD_G_TaskRequest *)pBuf;
	if (len!=sizeof(CMD_G_TaskRequest))
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest mess len error %d",len);
		return;
	}
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(taskReward->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest,can't find user %d",taskReward->dwUserID);
		return;
	}
	if (!pUser->m_userTask.ViewCompleteTaskProgress(taskReward->iTaskID))
	{
		CMD_G_TaskRequestFail trf = {0};
		trf.dwUserID = pUser->GetUserID();
		trf.iTaskID = taskReward->iTaskID;
		trf.enFail = ERU_UNCOMPLETED_TASK;
		pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_REQUEST_FAIL,(CT_CHAR*)&trf,sizeof(CMD_G_TaskRequestFail));
		return;
	}

	CT_INT32 iRewardType = 0;
	if (CCTaskScriptSetting::getSingleton().GetMainTaskRewardType(taskReward->iTaskID,iRewardType))
	{
		G_TaskList taskList={0};
		pUser->m_userTask.SendTaskReward(taskReward->iTaskID,iRewardType,taskList,TASK_SEND_REWARD_FAST);
	}
}

CT_VOID CCTaskEvent::DisposeTaskFastComplete(CT_CHAR* pBuf,CT_DWORD len)
{
	CMD_G_TaskRequest *taskRequest = (CMD_G_TaskRequest *)pBuf;
	if (len!=sizeof(CMD_G_TaskRequest))
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeTaskFastComplete mess len error %d",len);
		return;
	}
	CCUser *pUser;
	if (!CCUserManager::getSingleton().Find(taskRequest->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest,can't find user %d",taskRequest->dwUserID);
		return;
	}
	//vip add 只有vip能快速完成任务
	if (pUser->m_userVip.VipGradeGet()<=0)
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest,user %d not vip",taskRequest->dwUserID);
		pUser->CloseNet();
		return;
	}
	//任务是否在进行中
	if (!pUser->m_userTask.ViewProgressTask(taskRequest->iTaskID))
	{
		pUser->CloseNet();
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest,the user %d don't having the task %d",taskRequest->dwUserID,taskRequest->iTaskID);
		return;
	}
	//任务是否已经完成，只是没有领取奖励
	if (pUser->m_userTask.ViewCompleteTaskProgress(taskRequest->iTaskID))
	{
		pUser->CloseNet();
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest,the task %d has been finish,only wait reward request",taskRequest->iTaskID);
		return;
	}
	G_TaskRelation taskRelation = {0};
	if (CCTaskScriptSetting::getSingleton().GetTaskInfo(taskRequest->iTaskID,&taskRelation))
	{
		if (taskRelation.iFastComplete!=TASK_COULD_FAST_COMPLETE)
		{
			pUser->CloseNet();
			CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeTaskFastComplete task %d can't fast complete",taskRequest->iTaskID);
			return;
		}
		MasterData masterData = {0};
		pUser->m_userBase.GetMasterData(masterData);
		if (masterData.mvd.stRes.iDiamod+taskRelation.iDiamod<0)
		{
			CMD_G_TaskRequestFail trf = {0};
			trf.dwUserID = pUser->GetUserID();
			trf.iTaskID = taskRelation.iTaskID;
			trf.enFail = ERU_DIAMOD_LACK_TASK;
			pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_REQUEST_FAIL,(CT_CHAR*)&trf,sizeof(CMD_G_TaskRequestFail));
			return;
		}
		MasterVariableData mvd = {0};
		mvd.stRes.iDiamod = taskRelation.iDiamod;
		pUser->m_userBase.AddMastData(mvd,SAVETAG_TASK_FAST_COMPLETE);
		if (taskRelation.iRewardGet==TASK_REWARD_AUTO)
		{
			G_TaskList taskList = {0};
			pUser->m_userTask.SendTaskReward(taskRelation.iTaskID,taskRelation.iRewardGet,taskList,TASK_SEND_REWARD_FAST);
		}
		else if (taskRelation.iRewardGet==TASK_REWARD_MANUAL)
		{
			MasterVariableData mvd = {0};
			pUser->m_userBase.GetMasterData(mvd);
			if(!pUser->m_userTask.UpdateTaskCompleteTime(taskRequest->iTaskID))
			{
				CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest,UpdateTaskCompleteTime error");
				return;
			}
			CMD_G_TaskFastSuc taskFastSuc = {0};
			taskFastSuc.dwUserID = taskRequest->dwUserID;
			taskFastSuc.iTaskID = taskRequest->iTaskID;
			taskFastSuc.iDiamod = mvd.stRes.iDiamod;
			pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_FAST_SUC,(CT_CHAR*)&taskFastSuc,sizeof(CMD_G_TaskFastSuc));
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeTaskFastComplete CCTaskScriptSetting::getSingleton().GetTaskInfo(taskRequest->iTaskID error");
		pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		return;
	}
}

CT_VOID CCTaskEvent::DisposeTaskInquiry(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len%sizeof(CMD_G_TaskInquiry)!=0)
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeTaskInquiry  len error %d",len);
		return;
	}
	CT_CHAR buf[TASK_BUF_SIZE] = {0};
	CT_INT32 iLen=0;
	CT_INT32 iNum = len/sizeof(CMD_G_TaskInquiry);
	CMD_G_TaskInquiry taskInquiry = {0};
	CCUser *pUser=NULL;
	for (CT_INT32 i=0;i<iNum;i++)
	{
		memcpy(&taskInquiry,pBuf+i*sizeof(CMD_G_TaskInquiry),sizeof(CMD_G_TaskInquiry));
		CMD_G_TaskReply  taskReply = {0};
		taskReply.iTaskID = taskInquiry.iTaskID;
		if (pUser==NULL)
		{
			if (!CCUserManager::getSingleton().Find(taskInquiry.dwUserID,&pUser))
			{
				CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeTaskInquiry,can't find user %d",taskInquiry.dwUserID);
				return;
			}
		}
		
		if (pUser->m_userTask.ViewCompleteTask(taskInquiry.iTaskID))
		{
			taskReply.iStatus = TASK_COMPLETED;
		}
		memcpy(buf+iLen,&(taskReply),sizeof(CMD_G_TaskReply));
		iLen+=sizeof(CMD_G_TaskReply);
	}
	pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_COMPLETE_INQUIRY,buf,iLen);
}

CT_VOID CCTaskEvent::DispostTaskRole(CT_CHAR* pBuf,CT_DWORD len)
{
	if (len%sizeof(CMD_G_TaskRole)!=0)
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DispostTaskRole  len error %d",len);
		return;
	}

	CMD_G_TaskRole *pTaskRole = (CMD_G_TaskRole*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pTaskRole->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DispostTaskRole,can't find user %d",pTaskRole->dwUserID);
		return;
	}

	pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_ROLE,pBuf,len);

	CMD_G_TaskEventInfo taskEventInfo;
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_ROLE;
	taskEventInfo.iSubType = pTaskRole->iRoleType;
	taskEventInfo.iLevel = 1;
	pUser->m_userTask.DisposeTask(taskEventInfo);
}

CT_VOID CCTaskEvent::DisposeFightTeamUpgrade(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(MSC_FightTeamTask) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeFightTeamUpgrade sizeof(MSC_FightTeamTask) != len.");
		return;
	}

	MSC_FightTeamTask *pft = (MSC_FightTeamTask *)pBuf;
	CCUser *pUser = 0;
	if (!CCUserManager::getSingleton().Find(pft->dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeFightTeamUpgrade find pUser error.");
		return;
	}

	pUser->m_userTask.DisposeFightTeamUpgrade(pBuf, len);
}