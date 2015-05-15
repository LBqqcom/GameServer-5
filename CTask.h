#ifndef CTask_H_
#define CTask_H_

#include <map>
#include <vector>
//#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

struct SubTaskStatus
{
	CT_INT32	iStatus;		//�Ƿ����
	CT_INT32	iTime;			//��ɴ���
};

struct TaskFinish
{
	CT_DWORD		dwUserID;		//�û�id
	//G_TaskReward	taskReward;		//����   //����ɾ��
	CT_INT32        iTaskID;       //����id //�������
};

struct TaskType
{
	CT_INT32	iMainType;			//������
	CT_INT32	iSubType;			//������
	CT_INT32	iType;				//�������ͣ����罨������id
	CT_INT32	iLevel;				//����
	CT_INT32	iTime;				//����
};

struct SystemTaskType
{
	CT_INT32	iMainType;          //������
	CT_INT32    iSubType;           //������
	CT_INT32    arrayParam[TASK_PARAMETER_NUM];	//����
	CT_INT32	iprogress;			//�������
};

struct MSC_DB_TaskProgressData;
class  CCUser;
class CCUserTask
{
	friend class CCUser;
public:
	typedef std::vector<CT_INT32>			TaskCompleteVector;		//���������

private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;

	typedef std::map<CT_INT32,std::map<CT_INT32,SubTaskStatus> >				TaskProgressMap;	//����������
	typedef std::map<CT_INT32,std::map<CT_INT32,SubTaskStatus> >::iterator	TaskProgressMap_IT;

	TaskCompleteVector		m_taskCompleteVector;
	TaskProgressMap			m_taskProgressMap;
public:
	CCUserTask();
	~CCUserTask();
public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_taskCompleteVector.clear();
		m_taskProgressMap.clear();
	}

	//�����û�����
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_taskCompleteVector.clear();
		m_taskProgressMap.clear();
	}


public:
	//�������������
	CT_VOID InsertFinishTask(const CT_INT32 iTaskID){_AUTOMUTEX(m_pMutex);m_taskCompleteVector.push_back(iTaskID);}
	//�������������
	CT_VOID InsertProgressTask(MSC_DB_TaskProgressData &taskPData);
	//��������
	CT_VOID	GetTaskAutomatic(CT_INT32 iMasterLevel);
	//��ȡδ�������
	CT_BOOL GetUnFinishedTask(CT_CHAR *pBuf,CT_INT32 &bufLen);
	//�鿴�����Ƿ����,����ɷ���true
	CT_BOOL ViewCompleteTask(CT_INT32 iTaskID);
	//�鿴�����Ƿ���У������з���true
	CT_BOOL ViewProgressTask(CT_INT32 iTaskID);
	//�鿴ǰ�������Ƿ�ȫ�����
	CT_BOOL ViewFrontTask(const G_TaskList &taskList);
	//�鿴�����Ƿ���ɣ��ֶ���ȡ����������Ҫ�ڽ�������map�в鿴
	CT_BOOL ViewCompleteTaskProgress(CT_INT32 iTaskID);
	//������
	CT_VOID	DisposeTask(CMD_G_TaskEventInfo taskEventInfo,CT_INT32 iCompleteTime = 1);
    CT_VOID DisposeTask( CMD_G_TaskSystemInfo taskSysInfo,CT_INT32 iCompleteTime = 1);
	//�ֶ���ȡ�������񣬿�����ɺ󣬸������ݣ�������������ɴ�������
	CT_BOOL UpdateTaskCompleteTime(CT_INT32 iTaskID);
	//���ͽ���
	CT_VOID SendTaskReward(CT_INT32 iTaskID,CT_INT32 iRewardType,G_TaskList &taskList,CT_INT32 iSendType);
	//��������
	CT_VOID AddNewTask(CT_INT32 iTaskID,CT_CHAR *pBuf,CT_INT32 iBufLen,CT_INT32 &iLen,G_TaskList &taskCompleted);
	
private:
	//�������ɣ����Σ�
	CT_VOID DisposeSubTaskCompleteOnce(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CT_INT32 iTime,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//��ȡ�������ϸ���ͣ��ж��Ƿ���Ҫ��ȡ��ʷ��¼�����罨��ӵ������Ҫ��ȡ֮ǰӵ�ж��ٽ�����
	CT_BOOL GetTaskType(CT_INT32 itaskSubID,TaskType &taskType);
	//��ȡͨ���������ϸ����
	CT_BOOL GetTaskType(CT_INT32 itaskSubID,SystemTaskType &sysTask);
	//�ж�ӵ���������Ƿ��Ѿ����
	CT_VOID GetOwnTypeTaskStatus(TaskType taskType,CT_INT32 &iTime);
	//ͨ��ϵͳ �ж�ӵ���������Ƿ��Ѿ����
	CT_VOID GetOwnTypeTaskStatus(SystemTaskType sysTask,CT_INT32 &iTime,CT_INT32 iType,CT_INT32 iTaskID, CT_INT32 iSubTaskID);
	//������������
	CT_VOID DisposeUpgradeTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//����������
	CT_VOID DisposeDuplicateTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//������Ʒ����
	CT_VOID DisposeItemTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//��������������
	CT_VOID DisposeSmithyTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//����Ǳ������
	CT_VOID DisposePotentialTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//�����������
	CT_VOID DisposeCityTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//����������
	CT_VOID DisposeArenaTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
	//ս������
	CT_VOID DisposeFightTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
    // ͨ������ϵͳ
    CT_VOID DisposeSubTask(CT_INT32 iTaskID, CT_INT32 iSubTaskID,CMD_G_TaskSystemInfo taskSysInfo,G_TaskList &taskList,CT_INT32 iCompleteTime);
private:
	//����ͨ��ָ���ؿ�����ʷ��߼�¼������
	CT_BOOL DisposePointorTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//������սָ�������n�ιؿ�������
	CT_BOOL DisposeChallengeTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//����ִ��n�ι���ָ���ؿ�����������������
	CT_BOOL DisposeFightTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//����ִ��n��ף��������
	CT_BOOL DisposeBlessTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//������ָ������n�ε�����
	CT_BOOL DisposeChestTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//������ָ�����͡�Ʒ�ʡ��ȼ���Ԫ�ص�����
	CT_BOOL DisposeGetElmTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//����ָ����ɫ������������ָ���ȼ�������
	CT_BOOL DisposeFlagTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//������������
	CT_BOOL DisposeSwallowTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//�������ָ����������Դ/���ߵ�����
	CT_BOOL DisposeDonateTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//���������Ƽ��ȼ�����
	CT_BOOL DisposeUpgradeTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//��������ٻ�Boss����
	CT_BOOL DisposeCallBossTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	//�����ɱBoss����
	CT_BOOL DisposeFitBossTask(const CMD_G_TaskSystemInfo &taskSysInfo,const G_TaskSystem &stTaskSys);
    // ����װ��Ԫ������
    CT_BOOL DisposeEquipElmTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// ����ָ�����ͽ�������
	CT_BOOL DisposeBuidingProduct(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// ����ָ�����ͽ�������
	CT_BOOL DisposeBuidingUpgrade(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// ����/����ָ���ȼ���ս��
	CT_BOOL DisposeInFightTeam(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// ����ָ��װ��n��
	CT_BOOL DisposeWearEquipment(CT_INT32 iTaskID, CT_INT32 iSubTaskID, const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// ̽��ָ���������n��
	CT_BOOL DisposeSeekTimes(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);
	// ��ǽ��������
	CT_BOOL DisposeWallArmy(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys);

public:
	//��������������¼ʱս����������
	CT_VOID DisposeFightTeamUpgrade(CT_CHAR *pBuf, CT_INT32 iLen);
	//�����������
	CT_VOID  DisposeIslandTask(CT_INT32 iTaskID);
	//6�㶨ʱ�����������
	CT_VOID  UpdateIslandTaskOnClock();
};

#endif