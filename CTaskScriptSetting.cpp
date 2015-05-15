#include "CTaskScriptSetting.h"
#include "CUser.h"

template<> CCTaskScriptSetting* CCSingleton<CCTaskScriptSetting>::ms_Singleton=0;

CCTaskScriptSetting::CCTaskScriptSetting()
{

}

CCTaskScriptSetting::~CCTaskScriptSetting()
{

}

CT_VOID CCTaskScriptSetting::GetTaskMasterLevel(CT_INT32 iMasterL,G_TaskList *taskMasterLevel)
{
	G_GetTaskMasterLevel(iMasterL,taskMasterLevel);
}

CT_BOOL CCTaskScriptSetting::GetTaskFrontTask(CT_INT32 iTaskID,G_TaskList *taskList)
{
	if (G_GetTaskFrontTask(iTaskID,taskList))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_VOID CCTaskScriptSetting::GetSubTask(CT_INT32 iTaskID,G_TaskList *taskList)
{
	G_GetSubTask(iTaskID,taskList);
}

CT_BOOL CCTaskScriptSetting::GetSubTaskType(CT_INT32 iSubTaskID,CT_INT32 &iSubTaskType)
{
	if (G_GetSubTaskType(iSubTaskID,iSubTaskType))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetSubTaskInfo(CT_INT32 iSubTaskID,G_TaskSubTask *taskSubTask)
{
	if (G_GetSubTaskInfo(iSubTaskID,taskSubTask))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetUpgradeTaskInfo(CT_INT32 iSubTaskID,G_TaskUpgrade *taskUpgrade)
{
	if (G_GetUpgradeTaskInfo(iSubTaskID,taskUpgrade))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetSystemTaskInfo(CT_INT32 iSubTaskID,G_TaskSystem *taskSystem)
{
	if (G_GetTaskSystem(iSubTaskID,taskSystem))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetMainTaskID(CT_INT32 iSubTaskID,CT_INT32 &iTaskID)
{
	if (G_GetMainTaskID(iSubTaskID,iTaskID))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetMainTaskRewardType(CT_INT32 iTaskID,CT_INT32 &iTaskRewardType)
{
	if (G_GetMainTaskRewardType(iTaskID,iTaskRewardType))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetRewardTaskInfo(CT_INT32 iTaskID,G_TaskReward *taskReward)
{
	if (G_GetRewardTaskInfo(iTaskID,taskReward))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetDuplicateTaskInfo(CT_INT32 iSubTaskID,G_TaskDuplicate *taskDuplicate)
{
	if (G_GetDuplicateTaskInfo(iSubTaskID,taskDuplicate))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetItemTaskInfo(CT_INT32 iSubTaskID,G_TaskItem *taskItem)
{
	if (G_GetItemTaskInfo(iSubTaskID,taskItem))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetSmithyTaskInfo(CT_INT32 iSubTaskID,G_TaskSmithy *taskSmithy)
{
	if (G_GetSmithyTaskInfo(iSubTaskID,taskSmithy))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL	CCTaskScriptSetting::GetPotentialTaskInfo(CT_INT32 iSubTaskID,G_TaskPotential *taskPotential)
{
	if (G_GetPotentialTaskInfo(iSubTaskID,taskPotential))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetCityTaskInfo(CT_INT32 iSubTaskID,G_TaskCity *taskCity)
{
	if (G_GetCityTaskInfo(iSubTaskID,taskCity))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetArenaTaskInfo(CT_INT32 iSubTaskID,G_TaskArena *taskArena)
{
	if (G_GetArenaTaskInfo(iSubTaskID,taskArena))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetFightTaskInfo(CT_INT32 iSubTaskID,G_TaskFight *taskFight)
{
	if (G_GetFightTaskInfo(iSubTaskID,taskFight))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCTaskScriptSetting::GetTaskInfo(int iTaskID,G_TaskRelation *taskRelaltion)
{
	if (G_GetTaskInfo(iTaskID,taskRelaltion))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}