#ifndef CTaskScriptSetting_H_
#define CTaskScriptSetting_H_

#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"

class CCTaskScriptSetting: public CCSingleton<CCTaskScriptSetting>
{
public:
	CCTaskScriptSetting();
	~CCTaskScriptSetting();
public:
	//获取领主级别任务
	CT_VOID GetTaskMasterLevel(CT_INT32 iMasterL,G_TaskList *taskMasterLevel);
	//获取前置任务
	CT_BOOL GetTaskFrontTask(CT_INT32 iTaskID,G_TaskList *taskList);
	//获取子任务
	CT_VOID GetSubTask(CT_INT32 iTaskID,G_TaskList *taskList);
	CT_BOOL GetSubTaskInfo(CT_INT32 iSubTaskID,G_TaskSubTask *taskSubTask);
	CT_BOOL GetUpgradeTaskInfo(CT_INT32 iSubTaskID,G_TaskUpgrade *taskUpgrade);
	CT_BOOL GetDuplicateTaskInfo(CT_INT32 iSubTaskID,G_TaskDuplicate *taskDuplicate);
	CT_BOOL GetItemTaskInfo(CT_INT32 iSubTaskID,G_TaskItem *taskItem);
	CT_BOOL GetSmithyTaskInfo(CT_INT32 iSubTaskID,G_TaskSmithy *taskSmithy);
	CT_BOOL GetPotentialTaskInfo(CT_INT32 iSubTaskID,G_TaskPotential *taskPotential);
	CT_BOOL GetCityTaskInfo(CT_INT32 iSubTaskID,G_TaskCity *taskCity);
	CT_BOOL GetArenaTaskInfo(CT_INT32 iSubTaskID,G_TaskArena *taskArena);
	CT_BOOL GetFightTaskInfo(CT_INT32 iSubTaskID,G_TaskFight *taskFight);
	CT_BOOL GetRewardTaskInfo(CT_INT32 iTaskID,G_TaskReward *taskReward);
	CT_BOOL GetSystemTaskInfo(CT_INT32 iSubTaskID,G_TaskSystem *taskSystem);
	CT_BOOL GetSubTaskType(CT_INT32 iSubTaskID,CT_INT32 &iSubTaskType);
	CT_BOOL GetMainTaskID(CT_INT32 iSubTaskID,CT_INT32 &iTaskID);
	CT_BOOL GetMainTaskRewardType(CT_INT32 iTaskID,CT_INT32 &iTaskRewardType);
	CT_BOOL GetTaskInfo(int iTaskID,G_TaskRelation *taskRelaltion);
};

#endif