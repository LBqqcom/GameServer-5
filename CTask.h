#ifndef CTask_H_
#define CTask_H_

#include <map>
#include <vector>
//#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

struct SubTaskStatus
{
	CT_INT32	iStatus;		//是否完成
	CT_INT32	iTime;			//完成次数
};

struct TaskFinish
{
	CT_DWORD		dwUserID;		//用户id
	//G_TaskReward	taskReward;		//奖励   //彭文删除
	CT_INT32        iTaskID;       //任务id //彭文添加
};

struct TaskType
{
	CT_INT32	iMainType;			//主类型
	CT_INT32	iSubType;			//子类型
	CT_INT32	iType;				//对象类型，比如建筑类型id
	CT_INT32	iLevel;				//级别
	CT_INT32	iTime;				//次数
};

struct SystemTaskType
{
	CT_INT32	iMainType;          //主类型
	CT_INT32    iSubType;           //子类型
	CT_INT32    arrayParam[TASK_PARAMETER_NUM];	//参数
	CT_INT32	iprogress;			//任务进度
};

struct MSC_DB_TaskProgressData;
class  CCUser;
class CCUserTask
{
	friend class CCUser;
public:
	typedef std::vector<CT_INT32>			TaskCompleteVector;		//已完成任务

private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;

	typedef std::map<CT_INT32,std::map<CT_INT32,SubTaskStatus> >				TaskProgressMap;	//进行中任务
	typedef std::map<CT_INT32,std::map<CT_INT32,SubTaskStatus> >::iterator	TaskProgressMap_IT;

	TaskCompleteVector		m_taskCompleteVector;
	TaskProgressMap			m_taskProgressMap;
public:
	CCUserTask();
	~CCUserTask();
public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_taskCompleteVector.clear();
		m_taskProgressMap.clear();
	}

	//清理用户数据
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_taskCompleteVector.clear();
		m_taskProgressMap.clear();
	}


public:
	//插入已完成任务
	CT_VOID InsertFinishTask(const CT_INT32 iTaskID){_AUTOMUTEX(m_pMutex);m_taskCompleteVector.push_back(iTaskID);}
	//插入进行中任务
	CT_VOID InsertProgressTask(MSC_DB_TaskProgressData &taskPData);
	//推送任务
	CT_VOID	GetTaskAutomatic(CT_INT32 iMasterLevel);
	//获取未完成任务
	CT_BOOL GetUnFinishedTask(CT_CHAR *pBuf,CT_INT32 &bufLen);
	//查看任务是否完成,已完成返回true
	CT_BOOL ViewCompleteTask(CT_INT32 iTaskID);
	//查看任务是否进行，进行中返回true
	CT_BOOL ViewProgressTask(CT_INT32 iTaskID);
	//查看前置任务是否全部完成
	CT_BOOL ViewFrontTask(const G_TaskList &taskList);
	//查看任务是否完成，手动领取奖励任务，需要在进行任务map中查看
	CT_BOOL ViewCompleteTaskProgress(CT_INT32 iTaskID);
	//任务处理
	CT_VOID	DisposeTask(CMD_G_TaskEventInfo taskEventInfo,CT_INT32 iCompleteTime = 1);
    CT_VOID DisposeTask( CMD_G_TaskSystemInfo taskSysInfo,CT_INT32 iCompleteTime = 1);
	//手动领取奖励任务，快速完成后，更新数据，所有子任务完成次数更新
	CT_BOOL UpdateTaskCompleteTime(CT_INT32 iTaskID);
	//发送奖励
	CT_VOID SendTaskReward(CT_INT32 iTaskID,CT_INT32 iRewardType,G_TaskList &taskList,CT_INT32 iSendType);
	//新增任务
	CT_VOID AddNewTask(CT_INT32 iTaskID,CT_CHAR *pBuf,CT_INT32 iBufLen,CT_INT32 &iLen,G_TaskList &taskCompleted);
	
private:
	//子任务达成（单次）
	CT_VOID DisposeSubTaskCompleteOnce(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CT_INT32 iTime,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//获取任务的详细类型，判断是否需要获取历史记录（比如建筑拥有任务，要获取之前拥有多少建筑）
	CT_BOOL GetTaskType(CT_INT32 itaskSubID,TaskType &taskType);
	//获取通用任务的详细类型
	CT_BOOL GetTaskType(CT_INT32 itaskSubID,SystemTaskType &sysTask);
	//判断拥有型任务是否已经完成
	CT_VOID GetOwnTypeTaskStatus(TaskType taskType,CT_INT32 &iTime);
	//通用系统 判断拥有型任务是否已经完成
	CT_VOID GetOwnTypeTaskStatus(SystemTaskType sysTask,CT_INT32 &iTime,CT_INT32 iType,CT_INT32 iTaskID, CT_INT32 iSubTaskID);
	//处理升级任务
	CT_VOID DisposeUpgradeTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//处理副本任务
	CT_VOID DisposeDuplicateTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//处理物品任务
	CT_VOID DisposeItemTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//处理铁匠铺任务
	CT_VOID DisposeSmithyTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//处理潜能任务
	CT_VOID DisposePotentialTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//处理城市任务
	CT_VOID DisposeCityTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//竞技场任务
	CT_VOID DisposeArenaTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//战斗任务
	CT_VOID DisposeFightTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
    // 通用任务系统
    CT_VOID DisposeSubTask(CT_INT32 iTaskID, CT_INT32 iSubTaskID,CMD_G_TaskSystemInfo taskSysInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
private:
	//处理通过指定关卡（历史最高纪录）任务
	CT_BOOL DisposePointorTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理挑战指定区间的n次关卡的任务
	CT_BOOL DisposeChallengeTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理执行n次攻击指定关卡区间的生产点的任务
	CT_BOOL DisposeFightTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理执行n次祝福的任务
	CT_BOOL DisposeBlessTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理开启指定宝箱n次的任务
	CT_BOOL DisposeChestTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理获得指定类型、品质、等级的元素的任务
	CT_BOOL DisposeGetElmTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理指定角色的旗帜提升到指定等级的任务
	CT_BOOL DisposeFlagTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理吞噬任务
	CT_BOOL DisposeSwallowTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理捐献指定数量的资源/道具的任务
	CT_BOOL DisposeDonateTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理提升科技等级任务
	CT_BOOL DisposeUpgradeTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理参与召唤Boss任务
	CT_BOOL DisposeCallBossTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//处理击杀Boss任务
	CT_BOOL DisposeFitBossTask(const CMD_G_TaskSystemInfo &taskSysInfo,const G_TaskSystem &stTaskSys);
    // 处理装备元素任务
    CT_BOOL DisposeEquipElmTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// 处理指定类型建筑生产
	CT_BOOL DisposeBuidingProduct(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// 处理指定类型建筑升级
	CT_BOOL DisposeBuidingUpgrade(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// 加入/创建指定等级的战队
	CT_BOOL DisposeInFightTeam(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// 穿戴指定装备n次
	CT_BOOL DisposeWearEquipment(CT_INT32 iTaskID, CT_INT32 iSubTaskID, const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// 探访指定类型玩家n次
	CT_BOOL DisposeSeekTimes(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// 城墙上阵任务
	CT_BOOL DisposeWallArmy(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);

public:
	//处理增加任务或登录时战队升级任务
	CT_VOID DisposeFightTeamUpgrade(CT_CHAR *pBuf, CT_INT32 iLen);
	//龙岛任务完成
	CT_VOID  DisposeIslandTask(CT_INT32 iTaskID);
	//6点定时清楚龙岛任务
	CT_VOID  UpdateIslandTaskOnClock();
};

#endif