#include "CMD_Game.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CMasterScriptSetting.h"
#include "CBuildingScriptSetting.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "CTask.h"
#include "CTaskScriptSetting.h"
#include "GlobeScriptInterface.h"
#include "CBroadcastScriptSetting.h"
#include "CRandEventManager.h"
#include "CThreadMes.h"
#include "CDragonIsland.h"
//#include "CFightTeamManager.h"

#define MAX_MESS_LEN_TASK		4096



CCUserTask::CCUserTask()
{
	m_taskCompleteVector.clear();
	m_taskProgressMap.clear();
}

CCUserTask::~CCUserTask()
{
}

CT_VOID CCUserTask::InsertProgressTask(MSC_DB_TaskProgressData &taskPData)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iSubType = 0;
	G_TaskSubTask gtst = {0};
	G_TaskSystem gts = {0};

	m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iTime=taskPData.iTime;
	m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iStatus=taskPData.iCompleted;

	if (CCTaskScriptSetting::getSingleton().GetSubTaskType(taskPData.iSubTaskID, iSubType))
	{
		if (iSubType == TASK_MAIN_TYPE_SYS)
		{
			if (CCTaskScriptSetting::getSingleton().GetSystemTaskInfo(taskPData.iSubTaskID, &gts))
			{
				if (taskPData.iTime >= gts.iTime && taskPData.iCompleted == 0)
				{
					m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iStatus = TASK_COMPLETED;
				}
			}
		}
		else if (CCTaskScriptSetting::getSingleton().GetSubTaskInfo(taskPData.iSubTaskID, &gtst))
		{
			if (taskPData.iTime >= gtst.iTime && taskPData.iCompleted ==0)
		    {
				m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iStatus = TASK_COMPLETED;
		    }
		}
	}
	
	TaskType taskType = {0};
	SystemTaskType sysTask = {0};
	CT_INT32 iCompleteTime = 0;
	//�����ӵ�����������ж��Ƿ��Ѿ����
	if (GetTaskType(taskPData.iSubTaskID,taskType))
	{
		GetOwnTypeTaskStatus(taskType,iCompleteTime);
		if (iCompleteTime>=taskType.iTime)
		{
			m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iTime = taskType.iTime;
			m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iStatus = TASK_COMPLETED;
		}
	}
	//�ж��������е�ӵ����������
	else if (GetTaskType(taskPData.iSubTaskID,sysTask))
	{
		GetOwnTypeTaskStatus(sysTask, iCompleteTime, TASK_FIGHTTEAM_ENTER, taskPData.iTaskID, taskPData.iSubTaskID);
		m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iTime = iCompleteTime;
		if (iCompleteTime>=sysTask.iprogress)
		{
			m_taskProgressMap[taskPData.iTaskID][taskPData.iSubTaskID].iStatus = TASK_COMPLETED;
		}
	}
}

CT_BOOL CCUserTask::ViewCompleteTask(CT_INT32 iTaskID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 i = m_taskCompleteVector.size();
	for (CT_INT32 j =0;j<i;j++)
	{
		if (m_taskCompleteVector[j]==iTaskID)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_BOOL CCUserTask::ViewProgressTask(CT_INT32 iTaskID)
{
	_AUTOMUTEX(m_pMutex);
	TaskProgressMap_IT it = m_taskProgressMap.find(iTaskID);
	if (it==m_taskProgressMap.end())
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCUserTask::ViewFrontTask(const G_TaskList &taskList)
{
	_AUTOMUTEX(m_pMutex);
	for (CT_INT32 i= 0; i < taskList.iTaskNum ;i++)
	{
		if (!ViewCompleteTask(taskList.iTaskList[i]))
		{
			return CT_FALSE;
		}
	}
	return CT_TRUE;
}

CT_BOOL CCUserTask::ViewCompleteTaskProgress(CT_INT32 iTaskID)
{
	_AUTOMUTEX(m_pMutex);
	TaskProgressMap_IT it1 = m_taskProgressMap.find(iTaskID);
	if (it1==m_taskProgressMap.end())
	{
		return CT_FALSE;
	}
	std::map<CT_INT32,SubTaskStatus>::iterator it2 = (*it1).second.begin();
	while(it2!=(*it1).second.end())
	{
		if ((*it2).second.iStatus==0)
		{
			return CT_FALSE;
		}
		it2++;
	}
	return CT_TRUE;
}

CT_BOOL CCUserTask::GetUnFinishedTask(CT_CHAR *pBuf,CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);
	CMD_G_TaskUpdate taskUpdata = {0};
	taskUpdata.dwUserID = m_pUser->GetUserID();
	TaskProgressMap_IT it1 = m_taskProgressMap.begin();
	CT_INT32 i=0;
	CT_INT32 iIslandNum = 0;
	while(it1!=m_taskProgressMap.end())
	{
		std::map<CT_INT32,SubTaskStatus>::iterator it2 = (*it1).second.begin();
		if (it1->first >= ISLAND_TASKID_MIN && it1->first <= ISLAND_TASKID_MAX)
		{
			// ���¿ͻ��˴���
			CMD_UpdateTaskTimes utt = {0};
			utt.iTimes = it2->second.iTime;
			m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_TASK_TIMES, (CT_CHAR*)&utt, sizeof(utt));

			if (it2!=(*it1).second.end() && it2->second.iStatus == TASK_COMPLETED)
			{
				MSC_IslandTask mIslandTask = {0};
				mIslandTask.dwUserID = m_pUser->GetUserID();
				mIslandTask.iTaskID = it1->first;
				CT_DWORD dwThreadID = GAME_THREAD_CITY +  m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
				G_SendThreadInfo(dwThreadID,   MSC_MAIN_ISLAND_EVENT, MSC_SUB_TASK_COMPLETE, (CT_CHAR*)&mIslandTask, sizeof(mIslandTask));
			}
			++it1;
			++iIslandNum;
			continue;
		}
		while(it2!=(*it1).second.end())
		{
			if ((CT_INT32)((i+1)*sizeof(CMD_G_TaskUpdate))>bufLen)
			{
				return CT_FALSE;
			}
			taskUpdata.iSubTaskID = (*it2).first;
			taskUpdata.iTime = (*it2).second.iTime;
			memcpy(pBuf+i*sizeof(CMD_G_TaskUpdate),&taskUpdata,sizeof(CMD_G_TaskUpdate));
			//�����������һ��û����ɣ���������û����ɣ���Ҫ�ж��Ƿ�Ҫ���¼�,������ֻ��һ��������
			if (it2->second.iStatus!=TASK_COMPLETED)
			{
				//������ɣ�����û���¼���Ҫ����
				G_TaskEvent taskEvent = {0};
				if (G_GetTaskEvent(it1->first,&taskEvent))
				{
					RandEvent re = {0};
					re.iIndex=taskEvent.iEventIndex;
					re.iEventType=ENUM_EventType(taskEvent.iEventType);
					re.iEventID=taskEvent.iEventID;
					m_pUser->m_userEvent.AssignEvent(re);
				}
				//end
			}
			i+=1;
			it2++;
		}
		it1++;
	}
	if (0 == iIslandNum)
	{
		CMD_UpdateTaskTimes utt = {0};
		m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_TASK_TIMES, (CT_CHAR*)&utt, sizeof(utt));
	}
	bufLen = i*sizeof(CMD_G_TaskUpdate);
	return CT_TRUE;
}

CT_BOOL CCUserTask::UpdateTaskCompleteTime(CT_INT32 iTaskID)
{
	_AUTOMUTEX(m_pMutex);
	TaskProgressMap_IT it1 = m_taskProgressMap.find(iTaskID);
	if (it1 == m_taskProgressMap.end())
	{
		return CT_FALSE;
	}
	std::map<CT_INT32,SubTaskStatus>::iterator it2 = (*it1).second.begin();
	while(it2!=(*it1).second.end())
	{
		if ((*it2).second.iStatus!=TASK_COMPLETED)
		{
			MSC_DB_TaskProgressData taskProgressData = {0};
			G_TaskSubTask taskSubTask = {0};
			if (CCTaskScriptSetting::getSingleton().GetSubTaskInfo((*it2).first,&taskSubTask))
			{
				taskProgressData.dwUserID = m_pUser->GetUserID();
				taskProgressData.iTaskID = iTaskID;
				taskProgressData.iSubTaskID = (*it2).first;
				taskProgressData.iTime = taskSubTask.iTime;  
				taskProgressData.iCompleted = TASK_COMPLETED;
				//�޸��ڴ�����
				it2->second.iStatus=TASK_COMPLETED;
				it2->second.iTime=taskSubTask.iTime;
				
				CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadID,MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_DATA,(CT_CHAR*)&taskProgressData,sizeof(taskProgressData));
			}
			else
			{
				m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
				CCLogMeg::getSingleton().WriteError("CCUserTask::UpdateTaskCompleteTime get subtask %d info failed",(*it2).first);
				return CT_FALSE;
			}
		}
		it2++;
	}
	return CT_TRUE;
}

CT_BOOL CCUserTask::GetTaskType(CT_INT32 itaskSubID,TaskType &taskType)
{
	G_TaskSubTask taskSubTask = {0};
	if (CCTaskScriptSetting::getSingleton().GetSubTaskInfo(itaskSubID,&taskSubTask))
	{
		taskType.iMainType = taskSubTask.iTaskType;
		switch(taskSubTask.iTaskType)
		{
		case TASK_MAIN_TYPE_UPGRADE:
			{
				//���������У���ָ�����𣬾Ͳ���ȥ�����ݿ����ݣ���0��ʼ����
				G_TaskUpgrade taskUpgrade = {0};
				if(!CCTaskScriptSetting::getSingleton().GetUpgradeTaskInfo(itaskSubID,&taskUpgrade))
				{
					m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
					CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetUpgradeTaskInfo");
					return CT_FALSE;
				}
				if (taskUpgrade.iLevel==TASK_SUB_CASE_VALUE)
				{
					return CT_FALSE;
				}
				taskType.iLevel = taskUpgrade.iLevel;
				taskType.iTime = taskUpgrade.iTime;
				if (taskUpgrade.iMaster!=TASK_SUB_UN_CASE_VALUE)
				{
					taskType.iSubType = TASK_SUB_TYPE_UPGRADE_MASTER;
					return CT_TRUE;
				}
				if (taskUpgrade.iArm!=TASK_SUB_UN_CASE_VALUE)
				{
					taskType.iSubType = TASK_SUB_TYPE_UPGRADE_ARM;
					//�������Ӧ����ָ����
					taskType.iType = taskUpgrade.iArm;
					return CT_TRUE;
				}
				if (taskUpgrade.iArmAdv!=TASK_SUB_UN_CASE_VALUE)
				{
					taskType.iSubType = TASK_SUB_TYPE_UPGRADE_ARMADV;
					//�������Ӧ����ָ����
					taskType.iType = taskUpgrade.iArmAdv;
					return CT_TRUE;
				}
				if (taskUpgrade.iBuild!=TASK_SUB_UN_CASE_VALUE)
				{
					taskType.iSubType = TASK_SUB_TYPE_UPGRADE_BUILD;
					taskType.iType = taskUpgrade.iBuild;
					return CT_TRUE;
				}
				if (taskUpgrade.iCivil!=TASK_SUB_UN_CASE_VALUE)
				{
					taskType.iSubType = TASK_SUB_TYPE_UPGRADE_CIVIL;
					return CT_TRUE;
				}
				if (taskUpgrade.iPet!=TASK_SUB_UN_CASE_VALUE)
				{
					taskType.iSubType = TASK_SUB_TYPE_UPGRADE_PET;
					taskType.iType = taskUpgrade.iPet;
					return CT_TRUE;
				}
				if (taskUpgrade.iRank!=TASK_SUB_UN_CASE_VALUE)
				{
					taskType.iSubType = TASK_SUB_TYPE_UPGRADE_RANK;
					return CT_TRUE;
				}
				break;
			}
		case TASK_MAIN_TYPE_DUPLICATE:
			{
				G_TaskDuplicate taskDuplicate = {0};
				if(!CCTaskScriptSetting::getSingleton().GetDuplicateTaskInfo(itaskSubID,&taskDuplicate))
				{
					m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
					CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetDuplicateTaskInfo");
					return CT_FALSE;
				}
				if (taskDuplicate.iCheckP>TASK_SUB_CASE_VALUE)
				{
					taskType.iType = taskDuplicate.iCheckP;
					taskType.iLevel = taskDuplicate.iLevel;
					taskType.iTime = taskDuplicate.iTime;
					taskType.iSubType = TASK_SUB_TYPE_DUPLICATE_POINTOR;
					return CT_TRUE;
				}
				break;
			}
		case TASK_MAIN_TYPE_ITEM:
			{
				G_TaskItem taskItem = {0};
				if(!CCTaskScriptSetting::getSingleton().GetItemTaskInfo(itaskSubID,&taskItem))
				{
					m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
					CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetItemTaskInfo");
					return CT_FALSE;
				}
				//ӵ��ָ���������ߣ��ж����ݿ�ĸ���
				if (taskItem.iOwnItem>TASK_SUB_CASE_VALUE)
				{
					taskType.iType = taskItem.iOwnItem;
					taskType.iLevel = taskItem.iLevel;
					taskType.iTime = taskItem.iTime;
					taskType.iSubType = TASK_SUB_TYPE_ITEM_OWN;
					return CT_TRUE;
				}
				break;
			}
		case TASK_MAIN_TYPE_POTENTIAL:
			{
				G_TaskPotential taskPotential = {0};
				if(!CCTaskScriptSetting::getSingleton().GetPotentialTaskInfo(itaskSubID,&taskPotential))
				{
					m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
					CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetPotentialTaskInfo");
					return CT_FALSE;
				}
				if (taskPotential.iLearn>TASK_SUB_CASE_VALUE)
				{
					taskType.iType = taskPotential.iLearn;
					taskType.iTime = taskPotential.iTime;
					taskType.iSubType = TASK_SUB_TYPE_POTENTIAL_LEARN;
					return CT_TRUE;
				}
				break;
			}
		case TASK_MAIN_TYPE_CITY:
			{
				G_TaskCity taskCity = {0};
				if(!CCTaskScriptSetting::getSingleton().GetCityTaskInfo(itaskSubID,&taskCity))
				{
					m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
					CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetCityTaskInfo");
					return CT_FALSE;
				}
				taskType.iTime = taskCity.iTime;
				if (taskCity.iRegion>TASK_SUB_CASE_VALUE)
				{
					taskType.iType = taskCity.iRegion;
					taskType.iSubType = TASK_SUB_TYPE_CITY_REGION;
					return CT_TRUE;
				}
				if (taskCity.iOwnBuilding>TASK_SUB_CASE_VALUE)
				{
					taskType.iType = taskCity.iOwnBuilding;
					taskType.iSubType = TASK_SUB_TYPE_CITY_BUILD;
					return CT_TRUE;
				}
				if (taskCity.iProduct>TASK_SUB_CASE_VALUE)
				{
					taskType.iType = taskCity.iProduct;
					taskType.iSubType = TASK_SUB_TYPE_CITY_PRODUCT;
					return CT_TRUE;
				}
				break;
			}
		case TASK_MAIN_TYPE_FIGHT:
			{
				G_TaskFight taskFight = {0};
				if (!CCTaskScriptSetting::getSingleton().GetFightTaskInfo(itaskSubID,&taskFight))
				{
					m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
					CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetFightTaskInfo");
					return CT_FALSE;
				}
				taskType.iTime = taskFight.iTime;
				if (taskFight.iFormation > 0)
				{
//					if(m_pUser->m_userBattleArray.CheckArmyArray(taskFight.iFormation))
//					{
					if (taskFight.iFormation > UNIT_ALL_UNIT)
					{
						taskType.iLevel = taskFight.iFormation/10;
					}
					else
					{
						taskType.iLevel = taskFight.iFormation;
					}
					taskType.iSubType = TASK_SUB_TYPE_FIGHT_FORMATION;
					return CT_TRUE;
//					}
				}
				break;
			}
		default:
			{
				return CT_FALSE;
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetSubTaskInfo");
		return CT_FALSE;
	}
	return CT_FALSE;
}

CT_BOOL CCUserTask::GetTaskType(CT_INT32 itaskSubID,SystemTaskType &sysTask)
{
	G_TaskSubTask taskSubTask = {0};
	if (!CCTaskScriptSetting::getSingleton().GetSubTaskInfo(itaskSubID, &taskSubTask))
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetSubTaskInfo,itaskSubID: %d.",itaskSubID);
		return CT_FALSE;
	}
	if (taskSubTask.iTaskType != TASK_MAIN_TYPE_SYS)
	{
		return CT_FALSE;
	}
	G_TaskSystem taskSystem = {0};
	if (!CCTaskScriptSetting::getSingleton().GetSystemTaskInfo(itaskSubID,&taskSystem))
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::GetTaskType CCTaskScriptSetting::getSingleton().GetSystemTaskInfo,itaskSubID: %d.",itaskSubID);
		return CT_FALSE;
	}

	sysTask.iMainType = taskSubTask.iTaskType;
	sysTask.iSubType = taskSystem.iType;
	switch(taskSystem.iType)
	{
		//ָ����������
	case TASK_SUB_TYPE_SYSTEM_FLAG_UPGRADE:
		{
			sysTask.arrayParam[0] = taskSystem.iParameter[0];    //����0����ɫID
			sysTask.iprogress = taskSystem.iTime;
			return CT_TRUE;
		}
		//ָ���Ƽ�����
	case TASK_SUB_TYPE_SYSTEM_SCIENCE_UP:
		{
			sysTask.arrayParam[0] = taskSystem.iParameter[0];     //����0: 0���޶����ͣ�1������2ս��
			sysTask.arrayParam[1] = taskSystem.iParameter[1];	  //����1������ID/��ɫID
			sysTask.arrayParam[2] = taskSystem.iParameter[2];     //����2��ս������ID������û�У�
			sysTask.iprogress = taskSystem.iTime;
			return CT_TRUE;
		}
		//ͨ��ָ���ؿ�
	case TASK_SUB_TYPE_SYSTEM_POINTOR:
		{
			sysTask.arrayParam[0] = taskSystem.iParameter[0];     //����0���ؿ�ID
			sysTask.iprogress = taskSystem.iTime;
			return CT_TRUE;
		}
		//���ָ��Ԫ��
	case TASK_SUB_TYPE_SYSTEM_GETELM:
		{
			sysTask.arrayParam[0] = taskSystem.iParameter[0];     //����0�����ͣ�0Ϊ��������
			sysTask.arrayParam[1] = taskSystem.iParameter[1];     //����1��Ʒ�ʣ�0Ϊ����Ʒ��
			sysTask.arrayParam[2] = taskSystem.iParameter[2];     //����2���ȼ���0Ϊ����ȼ�
			sysTask.iprogress = taskSystem.iTime;
			return CT_TRUE;
		}
		//ָ����������
	case TASK_SUB_TYPE_SYSTEM_BUILD_UPGRADE:
		{
			if (taskSystem.iParameter[2] == 1)                    //����2��ֵΪ1��ʾ����ָ��������
			{                                                     //       ֵΪ2��ʾ������ָ���ȼ�
				return CT_FALSE;
			}
			sysTask.arrayParam[0] = taskSystem.iParameter[0];     //����0����������
			sysTask.arrayParam[1] = taskSystem.iParameter[1];     //����1������ID
			sysTask.arrayParam[2] = taskSystem.iParameter[2];       
			sysTask.iprogress = taskSystem.iTime;
			return CT_TRUE;
		}
		// ����/����ָ���ȼ���ս��
	case TASK_SUB_TYPE_SYSTEM_INFIGHTTEAM:
		{
			sysTask.iprogress = taskSystem.iTime;
			return CT_TRUE;
		}
		// ����ָ����װ��n�Σ�����0��װ���ȼ�������1��װ��Ʒ�ʣ�����2������λ�ã�����3��װ��ID
	case TASK_SUB_TYPE_SYSTEM_WEAR_EQUIPMENT:
		{
			sysTask.arrayParam[0] = taskSystem.iParameter[0];
			sysTask.arrayParam[1] = taskSystem.iParameter[1];
			sysTask.arrayParam[2] = taskSystem.iParameter[2];
			sysTask.arrayParam[3] = taskSystem.iParameter[3];
			sysTask.iprogress = taskSystem.iTime;
			return CT_TRUE;
		}
	default:
		return CT_FALSE;
	}

	return CT_FALSE;
		
}

//�ж�ӵ��������״̬�����������ڻ�ȡʱ�����Ѿ����
CT_VOID CCUserTask::GetOwnTypeTaskStatus(TaskType taskType,CT_INT32 &iTime)
{
	switch(taskType.iSubType)
	{
	//����������
		//�ж����������ȼ�
	case TASK_SUB_TYPE_UPGRADE_MASTER:
		{
			MasterData masterData = {0};
			m_pUser->m_userBase.GetMasterData(masterData);
			if (masterData.mvd.stGrade.iLevel>=taskType.iLevel)
			{
				iTime = 1;
			}
			break;
		}
	case TASK_SUB_TYPE_UPGRADE_CIVIL:
		{
			MasterData masterData = {0};
			m_pUser->m_userBase.GetMasterData(masterData);
			if (masterData.mvd.stCivili.iLevel>=taskType.iLevel)
			{
				iTime = 1;
			}
			break;
		}
	case TASK_SUB_TYPE_UPGRADE_RANK:
		{
			MasterData masterData = {0};
			m_pUser->m_userBase.GetMasterData(masterData);
			if (masterData.mvd.stRank.iLevel>=taskType.iLevel)
			{
				iTime = 1;
			}
			break;
		}
	case TASK_SUB_TYPE_UPGRADE_BUILD:
		{
			//ͨ������id��ȡ��������,(����ָ��������ָ������),ӵ��һ��ĳ������
			iTime = m_pUser->m_userBuilding.GetUserBuildByType(taskType.iType,taskType.iLevel);
			break;
		}
	case TASK_SUB_TYPE_UPGRADE_PET:
		{
			//���ݳ�������id��ȡ������Ϣ,��������ָ������(Ŀǰ�߻��ĵ���û��˵ָ�����͵ĳ���)
			//iTime = m_pUser->m_userPet.UserOwnLevelPet(taskType.iLevel);
			break;;
		}
	case TASK_SUB_TYPE_UPGRADE_ARM:
		{
			//���ݱ�������id��ȡʿ����Ϣ
			ArmyInfo arm = {0};
			if (m_pUser->m_userArmy.OwnArmyAdvanced(taskType.iType))
			{
				iTime=1;
			}
			else if (m_pUser->m_userArmy.GetArmyInfo(taskType.iType,arm))
			{
				if (arm.iArmyGrade>=taskType.iLevel)
				{
					iTime=1;
				}
			}
			break;
		}
	//zhoujun ���ֽ�������
	case TASK_SUB_TYPE_UPGRADE_ARMADV:
		{
			if (m_pUser->m_userArmy.OwnArmyAdvanNum(taskType.iLevel,taskType.iType))
			{
				iTime=1;
			}
		}
		break;
	//����������
	case TASK_SUB_TYPE_DUPLICATE_POINTOR:
		{
			//���ݹؿ�����id��ȡͨ�ص���Ϣ,�ؿ�,����ؿ������Ǵ󳡾�id*100+С����id
			if (m_pUser->m_userDuplicate.GetUserPassedDuplicate(taskType.iType/100,taskType.iType%100,taskType.iLevel))
			{
				iTime=1;
			}
			break;
		}
	//����������
	case TASK_SUB_TYPE_ITEM_OWN:
		{
			//��ȡ������Ϣ
			iTime = m_pUser->m_userItem.UserOwnItemGet(taskType.iType,taskType.iLevel);
			break;
		}
	//Ǳ��������
	case TASK_SUB_TYPE_POTENTIAL_LEARN:
		{
			//��ȡǱ����Ϣ(ѧ��ָ��ID��Ǳ��	��:ѧ�Ἴ�ܿ�������)
			if (m_pUser->m_userTalent.HasData(taskType.iType))
			{
				iTime=1;
			}
			break;
		}
	//����������
	case TASK_SUB_TYPE_CITY_REGION:
		{
			//��ȡ������Ϣ,�鿴�����Ƿ��Ѿ�����
			if (m_pUser->m_userRegion.GetRegionStatus(taskType.iType))
			{
				iTime = 1;
			}
			break;
		}
	case TASK_SUB_TYPE_CITY_BUILD:
		{
			//��ȡ������Ϣ
			iTime = m_pUser->m_userBuilding.GetUserBuildByType(taskType.iType,taskType.iLevel);
			break;
		}
	case TASK_SUB_TYPE_CITY_PRODUCT:
		{
			MasterData md = {0};
			m_pUser->m_userBase.GetMasterData(md);
			if (taskType.iType == 1001 &&  md.dwFTID > 0)
			{
				iTime = 1;
			}
			break;
		}
	case TASK_SUB_TYPE_FIGHT_FORMATION:
		{
			if (m_pUser->m_userBattleArray.CheckArmyArray(taskType.iLevel))
			{
				iTime = 1;
			}
			break;
		}
	}
}

CT_VOID CCUserTask::GetOwnTypeTaskStatus(SystemTaskType sysTask,CT_INT32 &iTime, CT_INT32 iType,CT_INT32 iTaskID, CT_INT32 iSubTaskID)
{
	switch(sysTask.iSubType)
	{
		//ָ����������
	case TASK_SUB_TYPE_SYSTEM_FLAG_UPGRADE:
		{
			BattleFlag bf = {0};
			if (sysTask.arrayParam[0] == 0)                           //��ɫIDΪ0����ʾ��������
			{
				CT_INT32 len = FLAG_TYPE_COUNT * sizeof(BattleFlag);
				CT_CHAR pBuffer[FLAG_TYPE_COUNT * sizeof(BattleFlag)] = "";
				//��ȡ�������ĵ���Ϣ��ȡ���ĵȼ���ߵ��ж�
				m_pUser->m_userBattleFlag.GetAllData(pBuffer, len);
				len = len / sizeof(BattleFlag);
				BattleFlag *pbf = (BattleFlag*)pBuffer;
				for (int i = 0;i < len;++i)
				{
					if (iTime < pbf[i].iLevel)
					{
						iTime = pbf[i].iLevel;
					}
				}
			}
			//�õ�ָ�����ĵ���Ϣ
			else if (m_pUser->m_userBattleFlag.GetData(sysTask.arrayParam[0],bf))
			{
				iTime = bf.iLevel;
			}
			break;
		}
		//ָ���Ƽ�����
	case TASK_SUB_TYPE_SYSTEM_SCIENCE_UP:
		{
			CT_INT32 len = NEMBER_SCIENCE_NUM * sizeof(NemberScience);
			char pBuffer[NEMBER_SCIENCE_NUM * sizeof(NemberScience)] = "";
			//�õ����еĳ�Ա�Ƽ�
			if (m_pUser->m_userFightTeamNember.GetNemberScience(pBuffer,len))
			{
				len = len / sizeof(NemberScience);
				NemberScience *pNs = (NemberScience*)pBuffer;
				for (int i = 0;i < len; ++i)
				{
					G_FightTeamBless ftb = {0};
					G_GetFightTeamBless(pNs[i].iScienceID, &ftb);
					//����0��ֵΪ���ʾ���޶����ͣ���ʱȡ�ȼ�����ߣ�����1��ʾƷ��
					if (sysTask.arrayParam[0] == 0 && sysTask.arrayParam[1] == ftb.iAdditionObj)
					{
						if (iTime < pNs[i].iScienceGrade)
						{
							iTime = pNs[i].iScienceGrade;
						}
					}
					//����0Ϊ����
					else if(sysTask.arrayParam[0] == TASK_SCIENCE_BUILD && sysTask.arrayParam[1] == ftb.iAdditionObj )
					{
						iTime = pNs[i].iScienceGrade;
						break;
					}
					//����0Ϊս����ս��������Ҫ�жϲ���2��ս������
					else if (sysTask.arrayParam[0] == TASK_SCIENCE_FIGHT && sysTask.arrayParam[1] == ftb.iAdditionObj
						&& sysTask.arrayParam[2] == ftb.iAttrID)
					{
						iTime = pNs[i].iScienceGrade;
						break;
					}
				}

			}
			break;
		}
		//Ԫ��֮��ͨ������
	case TASK_SUB_TYPE_SYSTEM_POINTOR:
		{
			//�õ�����0��ָ���Ĺؿ�ID�Ƿ�ͨ��
			if (m_pUser->m_userTower.CheckPointPass((sysTask.arrayParam[0])))
			{
				iTime = 1;
			}
			break;
		}
		//���ָ����Ԫ��
	case TASK_SUB_TYPE_SYSTEM_GETELM:
		{
			CT_INT32 len = ELM_SLOT_COUNT * (sizeof(CMD_G_ItemData));
			CT_CHAR pBuffer[ELM_SLOT_COUNT * (sizeof(CMD_G_ItemData))] = "";
			//�õ����е�Ԫ����Ϣ
			m_pUser->m_userElmPower.GetAllData(pBuffer,len);
			len = len / sizeof(CMD_G_ItemData);
			CMD_G_ItemData *pElm = (CMD_G_ItemData*)pBuffer;
			for (int i = 0;i < len;++i)
			{
				G_ElmMarrow elmMarrow = {0};
				G_ItemBase itemBase = {0};
				ElmMarrowAttr elmAttr = {0};
				G_GetElmMarrow(pElm[i].iID, elmMarrow);
				G_GetItemBase(pElm[i].iID, &itemBase);
				//�õ�Ԫ������
				m_pUser->m_userElmAttr.GetData(pElm[i].key, elmAttr);
                //�жϵ�ǰԪ�ص�Ʒ����ȼ����ڻ��������
				if (itemBase.iQuality >= sysTask.arrayParam[1] && elmAttr.iLevel >= sysTask.arrayParam[2])
				{
					//����0��ʾ���ͣ�0Ϊ�������ͣ���ʱȡ��������Ԫ��
					if (sysTask.arrayParam[0] == 0)
					{
						iTime = iTime > pElm[i].iCount?iTime:pElm[i].iCount;
					}
					//�жϵ�ǰԪ���������ļ����
					else if (elmMarrow.iType == sysTask.arrayParam[0])
					{
						iTime = pElm[i].iCount;
						break;
					}
				}
			}
			break;
		}
	//ָ�����ͽ���������ָ���ȼ�������n��
	case TASK_SUB_TYPE_SYSTEM_BUILD_UPGRADE:
		{
			//����2��ֵΪ2����ʾ����������ָ���ȼ���ֵΪ1��ʾ����n��
			if (sysTask.arrayParam[2] != 2)
			{
				break;
			}
			CMD_G_BuildingData gbd = {0};
			G_Building gb = {0};
			//����0���������ͣ�0��ʾ���⽨�����ͣ�-1��ʾ���жϸ�����
			if (sysTask.arrayParam[0] == 0 || sysTask.arrayParam[0] == -1)
			{
				//����1������ID��Ϊ0��ʾ���⽨������ʱȡͼ�ڵĵȼ�
				if (sysTask.arrayParam[1] == 0 || sysTask.arrayParam[1] == -1)
				{
					m_pUser->m_userBuilding.GetBuildingData(CITY_TYPE_TOTEM,gbd);
				}
				else 
				{
					G_GetBuildingData(sysTask.arrayParam[1],&gb);
					m_pUser->m_userBuilding.GetBuildingData(gb.iBT,gbd);
				}	
			}
			else
			{
				//���ݽ������͵õ�������Ϣ
				m_pUser->m_userBuilding.GetBuildingData(sysTask.arrayParam[0], gbd);
			}
			iTime = gbd.iGrade;
			break;
		}
		// ����/����ָ���ȼ���ս��
	case TASK_SUB_TYPE_SYSTEM_INFIGHTTEAM:
		{
			MSC_FightTeamTask fightTeamTask = {0};
			fightTeamTask.iTaskID = iTaskID;
			fightTeamTask.iSubTaskID = iSubTaskID;
			fightTeamTask.iType = iType;
			fightTeamTask.dwUserID = m_pUser->GetUserID();
			fightTeamTask.sysTask = sysTask;
			CT_DWORD threadid = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			G_SendThreadInfo(threadid,MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_FINGTTEAM_UPGRADE_TASK, (CT_CHAR*)&fightTeamTask, sizeof(fightTeamTask));                
			break;
		}
		// ����ָ��װ��n�Σ�����0��װ���ȼ�������1��װ��Ʒ�ʣ�����2������λ�ã�����3��װ��ID
	case TASK_SUB_TYPE_SYSTEM_WEAR_EQUIPMENT:
		{
			CT_INT32 iCount = 0;
			CT_INT32 iLen = EQUIPED_ITEM_COUNT * sizeof(CMD_G_ItemData);
			CT_CHAR pBuf[EQUIPED_ITEM_COUNT * sizeof(CMD_G_ItemData)] = {0};
			CMD_G_ItemData *pItemBase;
			EquipmentData equipData = {0};
			G_ItemBase itemBase = {0};

			m_pUser->m_userItem.GetEquipedItemData(pBuf, iLen);
			iLen /= sizeof(CMD_G_ItemData);
			pItemBase = (CMD_G_ItemData *)pBuf;

			// ������λ��Ϊ0ʱ���ж���������������װ����
			if (sysTask.arrayParam[2] == 0)
			{
				for (int i = 0; i < iLen && pItemBase != NULL; ++i,++pItemBase)
				{
					m_pUser->m_userEquip.GetData(pItemBase->key, equipData);
					G_GetItemBase(pItemBase->iID, &itemBase);

					if ((equipData.iLevel >= sysTask.arrayParam[0] || sysTask.arrayParam[0] == 0)
						&& (itemBase.iQuality >= sysTask.arrayParam[1] || sysTask.arrayParam[1] == 0)
						&& (pItemBase->iID >= sysTask.arrayParam[3] || sysTask.arrayParam[3] == 0))
					{
						iCount++;
					}
				}
				iTime = iCount;
			}
			// ����λ�ò�Ϊ0������ָ��װ��1��
			else 
			{
				for (int i = 0; i < iLen && pItemBase != NULL; ++i,++pItemBase)
				{
					m_pUser->m_userEquip.GetData(pItemBase->key, equipData);
					G_GetItemBase(pItemBase->iID, &itemBase);

					if ((equipData.iLevel >= sysTask.arrayParam[0] || sysTask.arrayParam[0] == 0)
						&& (itemBase.iQuality >= sysTask.arrayParam[1] || sysTask.arrayParam[1] == 0)
						&& (pItemBase->iIndex == sysTask.arrayParam[2])
						&& (itemBase.iID == sysTask.arrayParam[3]))
					{
						iTime = 1;
						break;
					}
				}
			}
			break;
		}
	}
}

//���������ʱ����Ҫ���жϣ������ӵ����������Ҫ��ȡ֮ǰ�Ѿ�ӵ�ж�����Ʒ
CT_VOID CCUserTask::AddNewTask(CT_INT32 iTaskID,CT_CHAR *pBuf,CT_INT32 iBufLen,CT_INT32 &iLen,G_TaskList &taskCompleted)
{
	_AUTOMUTEX(m_pMutex);
	//������ܣ�����û���¼���Ҫ����
	G_TaskEvent taskEvent = {0};
	if (G_GetTaskEvent(iTaskID,&taskEvent))
	{
		if (taskEvent.iEventType == EVENT_BUILDING)
		{
			G_BuildingEventProbability  bep = {0};
			if(CCEventScriptSetting::getSingleton().GetBuildingEvent(taskEvent.iEventIndex, bep))	//zhoujun	�������æ,����������
			{
				if(m_pUser->m_userBuilding.FindAutoProductingBuilding(bep.iBuildID) ||
					m_pUser->m_userBuilding.FindBuildingProduct(bep.iBuildID)  ||
					m_pUser->m_userBuilding.FindUpgradeBuildData(bep.iBuildID) ||
					m_pUser->m_userEvent.FindBuildingEventByIndex(bep.iIndex) )
				{
					return;
				}
			}
		}

		RandEvent re = {0};
		re.iIndex=taskEvent.iEventIndex;
		re.iEventType=ENUM_EventType(taskEvent.iEventType);
		re.iEventID=taskEvent.iEventID;
		m_pUser->m_userEvent.AssignEvent(re);
	}
	G_TaskList taskList={0}; 
	CCTaskScriptSetting::getSingleton().GetSubTask(iTaskID,&taskList);
	for (CT_INT32 i = 0;i<taskList.iTaskNum;i++)
	{
		if(iLen >= iBufLen)
		{
			CCLogMeg::getSingleton().WriteError("CCUserTask::AddNewTask iLen >= iBufLen");
			break;
		}
		CMD_G_TaskUpdate taskUpdate = {0};
		SubTaskStatus taskStatus = {0};

		TaskType taskType = {0};
		SystemTaskType sysTask = {0};
		CT_INT32 iCompleteTime = 0;
		MSC_DB_TaskProgressData taskProgressData = {0};

		//�����ӵ�����������ж��Ƿ��Ѿ����
		if (GetTaskType(taskList.iTaskList[i],taskType))
		{
			GetOwnTypeTaskStatus(taskType,iCompleteTime);
			if (iCompleteTime>=taskType.iTime)
			{
				taskProgressData.iTime = taskType.iTime;
				taskProgressData.iCompleted = TASK_COMPLETED;
				taskStatus.iTime = taskType.iTime;
				taskStatus.iStatus = TASK_COMPLETED;

				// ��������
				if (iTaskID >= ISLAND_TASKID_MIN && iTaskID <= ISLAND_TASKID_MAX)
				{
					MSC_IslandTask mIslandTask = {0};
					mIslandTask.dwUserID = m_pUser->GetUserID();
					mIslandTask.iTaskID = iTaskID;
					CT_DWORD dwThreadID = GAME_THREAD_CITY +  m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
					G_SendThreadInfo(dwThreadID,   MSC_MAIN_ISLAND_EVENT, MSC_SUB_TASK_COMPLETE, (CT_CHAR*)&mIslandTask, sizeof(mIslandTask));
					break;
				}
			}
			else
			{
				taskProgressData.iTime = iCompleteTime;
				taskProgressData.iCompleted = 0;
				taskStatus.iTime = iCompleteTime;
				taskStatus.iStatus = 0;
			}
		}
		else if(GetTaskType(taskList.iTaskList[i],sysTask))
		{
			GetOwnTypeTaskStatus(sysTask,iCompleteTime,TASK_FIGHTTEAM_ADD,iTaskID,taskList.iTaskList[i]);
			if (iCompleteTime>=sysTask.iprogress)
			{
				taskProgressData.iTime = iCompleteTime;
				taskProgressData.iCompleted = TASK_COMPLETED;
				taskStatus.iTime = iCompleteTime;
				taskStatus.iStatus = TASK_COMPLETED;

				// ��������
				if (iTaskID >= ISLAND_TASKID_MIN && iTaskID <= ISLAND_TASKID_MAX)
				{
					MSC_IslandTask mIslandTask = {0};
					mIslandTask.dwUserID = m_pUser->GetUserID();
					mIslandTask.iTaskID = iTaskID;
					CT_DWORD dwThreadID = GAME_THREAD_CITY +  m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
					G_SendThreadInfo(dwThreadID,   MSC_MAIN_ISLAND_EVENT, MSC_SUB_TASK_COMPLETE, (CT_CHAR*)&mIslandTask, sizeof(mIslandTask));
					break;
				}
			}
			else
			{
				taskProgressData.iTime = iCompleteTime;
				taskProgressData.iCompleted = 0;
				taskStatus.iTime = iCompleteTime;
				taskStatus.iStatus = 0;
			}
		}
		m_taskProgressMap[iTaskID].insert(std::map<CT_INT32,SubTaskStatus>::value_type(taskList.iTaskList[i],taskStatus));

		// ���������������������ѭ��
		/*if (iTaskID >= ISLAND_TASKID_MIN && iTaskID <= ISLAND_TASKID_MAX)
		{
			break;
		}*/

		taskUpdate.dwUserID = m_pUser->GetUserID();
		taskUpdate.iSubTaskID = taskList.iTaskList[i];
		taskUpdate.iTime = taskStatus.iTime;
		memcpy(pBuf+iLen,&taskUpdate,sizeof(CMD_G_TaskUpdate));
		iLen+=sizeof(CMD_G_TaskUpdate);
		taskProgressData.dwUserID = m_pUser->GetUserID();
		taskProgressData.iTaskID = iTaskID;
		taskProgressData.iSubTaskID = taskList.iTaskList[i];
		CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_DATA,(CT_CHAR*)&taskProgressData,sizeof(taskProgressData));
	}
	//�ж��Զ���ȡ�����������Ƿ����Ѿ���ɵ�
	CT_INT32 iRewardType = 0;
	if (CCTaskScriptSetting::getSingleton().GetMainTaskRewardType(iTaskID,iRewardType))
	{
		//���Զ����ܣ��ж������Ƿ����
		if (iRewardType==TASK_REWARD_AUTO)
		{
			TaskProgressMap_IT it = m_taskProgressMap.find(iTaskID);
			std::map<CT_INT32,SubTaskStatus>::iterator it2 = (*it).second.begin();
			while(it2!=(*it).second.end())
			{
				if ((*it2).second.iStatus!=TASK_COMPLETED)
				{
					break;
				}
				it2++;
			}
			//���������������
			if (it2==(*it).second.end())
			{
				taskCompleted.iTaskList[taskCompleted.iTaskNum++] = iTaskID;
				return;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::AddNewTask CCTaskScriptSetting::getSingleton().GetMainTaskRewardType");
		return ;
	}
}

//���������ȼ�����ȡ�û�û�н�������
CT_VOID CCUserTask::GetTaskAutomatic(CT_INT32 iMasterLevel)
{
	G_TaskList taskMasterL = {0},tasklist={0},taskCompleted={0};
	CT_CHAR	pBuf[MAX_MESS_LEN_TASK];
	CT_INT32 iBufLen = 0;
	memset(pBuf,0,MAX_MESS_LEN_TASK);
	/*CT_DWORD dwUserID = m_pUser->GetUserID();
	memcpy(pBuf,&dwUserID,sizeof(CT_DWORD));
	iBufLen+=sizeof(CT_DWORD);*/
	for (int i=iMasterLevel;i>0;i--)
	{
		memset(&taskMasterL,0,sizeof(G_TaskList));
		CCTaskScriptSetting::getSingleton().GetTaskMasterLevel(i,&taskMasterL);
		for (CT_INT32 j=0;j<taskMasterL.iTaskNum;j++)
		{
			memset(&tasklist,0,sizeof(tasklist));
			//����ɻ����ڽ�����
			if (ViewCompleteTask(taskMasterL.iTaskList[j])||ViewProgressTask(taskMasterL.iTaskList[j]))
			{
				continue;
			}
			//ǰ�������Ƿ����,���û��ǰ�����񣬲��ô���
			if (CCTaskScriptSetting::getSingleton().GetTaskFrontTask(taskMasterL.iTaskList[j],&tasklist))
			{
				if (!ViewFrontTask(tasklist))
				{
					continue;
				}
			}
			//��������Խ���
			AddNewTask(taskMasterL.iTaskList[j],pBuf,MAX_MESS_LEN_TASK,iBufLen,taskCompleted);
#ifdef _DEBUG
			CCLogMeg::getSingleton().WriteInfo("user %d get task %d\n",m_pUser->GetUserID(),taskMasterL.iTaskList[j]);
#endif			
			/*memcpy(pBuf+iBufLen,&taskMasterL.iTaskList[j],sizeof(CT_INT32));
			iBufLen+=sizeof(CT_INT32);*/
		}
	}
	if (iBufLen>0)
	{
		//���ͽ���������ͻ��ˣ���Ϣ��ʽΪuserid+taskid1+taskid2+taskid3
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_UPDATE,pBuf,iBufLen);
	}
	//�Ѿ����Զ���ȡ��������������ˣ���Ҫ���Ϸ��ͽ���
	if (taskCompleted.iTaskNum>0)
	{
		for(CT_INT32 i=0;i<taskCompleted.iTaskNum && i<TASK_LEVEL_LENEX;i++)
		{	
			G_TaskList tasklist={0};
			SendTaskReward(taskCompleted.iTaskList[i],TASK_REWARD_AUTO,tasklist,TASK_SEND_REWARD_FAST);
		}	
	}
}

//���ͽ����������������������ɺͿ�����ɣ����������ʱ����Ҫ����map�����޸�map�����iterator�仯
CT_VOID CCUserTask::SendTaskReward(CT_INT32 iTaskID,CT_INT32 iRewardType,G_TaskList &taskFinishList,CT_INT32 iSendRewardType)
{
	//������ɣ��жϺڵ깦���Ƿ�����
	G_ValueRange vr1={0},vr2={0};
	if (G_GetValueRange(VALUE_TYPE_BLACK_MASTER_LEVEL,&vr1)&&G_GetValueRange(VALUE_TYPE_BLACK_TASK,&vr2))
	{
		MasterVariableData mvd ={0};
		m_pUser->m_userBase.GetMasterData(mvd);
		//�����ȼ���������������
		if (iTaskID==vr2.fMaxVal&&mvd.stGrade.iLevel>=vr1.fMaxVal)
		{
			m_pUser->m_userBlackMarket.OpenBlackMarketFucntion();
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::SendTaskReward CCTaskScriptSetting::getSingleton().G_GetValueRange");
		return ;
	}
	//��Ӧ������ɣ��������빦��
	G_ValueRange vr = {0};
	if (G_GetValueRange(VALUE_TYPE_WORSHIP_TASKID,&vr))
	{
		if (vr.fMaxVal==iTaskID)
		{
			MasterVariableData mvd ={0};
			m_pUser->m_userBase.GetMasterData(mvd);
			m_pUser->m_userWorship.GetWorshipByMasterLevel(mvd.stGrade.iLevel);
		}	
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::SendTaskReward CCTaskScriptSetting::getSingleton().VALUE_TYPE_WORSHIP_MASTER_LEVEL");
		return ;
	}
	G_TaskReward taskReward = {0};
	if (!CCTaskScriptSetting::getSingleton().GetRewardTaskInfo(iTaskID,&taskReward))
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::SendTaskReward of task %d fail",iTaskID);
		return;
	}


	if (iRewardType==TASK_REWARD_MANUAL)
	{
		CMD_G_TaskRequestFail trf = {0};
		trf.dwUserID = m_pUser->GetUserID();
		trf.iTaskID = iTaskID;
		trf.enFail = ERU_BAG_FULL_TASK;
		CT_DWORD threadid = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		CT_INT32 iItemNum = ((taskReward.iItemID1>0)?1:0) + ((taskReward.iItemID2>0)?1:0) + ((taskReward.iItemID3>0)?1:0) +((taskReward.iItemID4>0)?1:0);
		if (iItemNum>(m_pUser->m_userItem.CalcEmptySlot()))
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_REQUEST_FAIL,(CT_CHAR*)&trf,sizeof(CMD_G_TaskRequestFail));
			return;
		}
	}

	TaskFinish tr = {0};
	MasterData md = {0};
	m_pUser->m_userBase.GetMasterData(md);

	//������Զ���ȡ����������û�е���
	MasterVariableData mvd = {0};
	mvd.stRes.iGold+=taskReward.iGold;
	mvd.stRes.iDiamod+=taskReward.iDiamod;
	mvd.stRes.iFood+=taskReward.iFood;
	mvd.stRes.iSkillSoul+=taskReward.iSoul;
	mvd.stRes.iStamina+=taskReward.iStamina;
	mvd.stRes.iTalentNum+=taskReward.iPotential;
	mvd.stCivili.iCurrentExp+=taskReward.iCiviliNum;
	mvd.stGrade.iCurrentExp+=taskReward.iXp;
	mvd.stRank.iCurrentExp+=taskReward.iPrestige;
	m_pUser->m_userBase.AddMastData(mvd,SAVETAG_TASK_REWARD);

	MasterData masterData = {0};
	m_pUser->m_userBase.GetMasterData(masterData);

	CMD_G_TaskReward taskRewardSend = {0};
	taskRewardSend.dwUserID = m_pUser->GetUserID();
	taskRewardSend.iTaskID = iTaskID;
	taskRewardSend.iCiviliNum = masterData.mvd.stCivili.iCurrentExp;
	taskRewardSend.iDiamod = masterData.mvd.stRes.iDiamod;
	taskRewardSend.iFood = masterData.mvd.stRes.iFood;
	taskRewardSend.iGold = masterData.mvd.stRes.iGold;
	taskRewardSend.iPotential = masterData.mvd.stRes.iTalentNum;
	taskRewardSend.iPrestige = masterData.mvd.stRank.iCurrentExp;
	taskRewardSend.iSoul = masterData.mvd.stRes.iSkillSoul;
	taskRewardSend.iStamina = masterData.mvd.stRes.iStamina;
	taskRewardSend.iXp = masterData.mvd.stGrade.iCurrentExp;
	m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_REWARD,(CT_CHAR*)&taskRewardSend,sizeof(CMD_G_TaskReward));
	//������ֶ���ȡ�������жϱ���λ��
	if (iRewardType==TASK_REWARD_MANUAL)
	{
		CMD_G_TaskRequestFail trf = {0};
		trf.dwUserID = m_pUser->GetUserID();
		trf.iTaskID = iTaskID;
		trf.enFail = ERU_BAG_FULL_TASK;
		//CT_DWORD threadid = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		CT_INT32 iItemNum = ((taskReward.iItemID1>0)?1:0) + ((taskReward.iItemID2>0)?1:0) + ((taskReward.iItemID3>0)?1:0) +((taskReward.iItemID4>0)?1:0);
		if (iItemNum>(m_pUser->m_userItem.CalcEmptySlot()))
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_REQUEST_FAIL,(CT_CHAR*)&trf,sizeof(CMD_G_TaskRequestFail));
			return;
		}
		//���жϵ����Ƿ��ܶ���
		G_ItemBase ib1={0},ib2={0},ib3={0},ib4={0};
		if ((taskReward.iItemID1==0?0:!G_GetItemBase(taskReward.iItemID1,&ib1))||(taskReward.iItemID2==0?0:!G_GetItemBase(taskReward.iItemID2,&ib2))||(taskReward.iItemID3==0?0:!G_GetItemBase(taskReward.iItemID3,&ib3))||(taskReward.iItemID4==0?0:!G_GetItemBase(taskReward.iItemID4,&ib4)))
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
			CCLogMeg::getSingleton().WriteError("CCUserTask::SendTaskReward of task G_GetItemBase fail");
			return;
		}
		//����������Ʒ�㲥��Ϣ
		if (taskReward.iItemID1>0)
		{	
			CMD_G_BroadCastMes bcm = {0};
			bcm.dwUserID = m_pUser->GetUserID();
			bcm.iSubType = BROADCAST_SUB_TYPE_ITEM_TASK;
			bcm.iQuality = ib1.iQuality;
			memcpy(bcm.cResultName,ib1.szName,NAME_SIZE);

			//G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_SYSTEM, (CT_CHAR*)&bcm, sizeof(bcm));
			//���ӵ������ͺͽ�ɫ���жϣ���õĵ���������ǽ�ɫƥ��ģ���������
			if (ib1.iClass==0||ib1.iClass==md.iOccupation)
			{
				//m_pUser->m_userItem.AutoAddItemData(threadid,taskReward.iItemID1,taskReward.iNum1);
				m_pUser->LootItems(SAVETAG_TASK_REWARD, &(taskReward.iItemID1), &(taskReward.iNum1));
			}
		}
		if (taskReward.iItemID2>0)
		{
			CMD_G_BroadCastMes bcm = {0};
			bcm.dwUserID = m_pUser->GetUserID();
			bcm.iSubType = BROADCAST_SUB_TYPE_ITEM_TASK;
			bcm.iQuality = ib2.iQuality;
			memcpy(bcm.cResultName,ib2.szName,NAME_SIZE);


			//G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_SYSTEM, (CT_CHAR*)&bcm, sizeof(bcm));
			//���ӵ������ͺͽ�ɫ���жϣ���õĵ���������ǽ�ɫƥ��ģ���������
			if (ib2.iClass==0||ib2.iClass==md.iOccupation)
			{
				//m_pUser->m_userItem.AutoAddItemData(threadid,taskReward.iItemID2,taskReward.iNum2);
				m_pUser->LootItems(SAVETAG_TASK_REWARD, &(taskReward.iItemID2), &(taskReward.iNum2));
			}
		}
		if (taskReward.iItemID3>0)
		{
			CMD_G_BroadCastMes bcm = {0};
			bcm.dwUserID = m_pUser->GetUserID();
			bcm.iSubType = BROADCAST_SUB_TYPE_ITEM_TASK;
			bcm.iQuality = ib3.iQuality;
			memcpy(bcm.cResultName,ib3.szName,NAME_SIZE);
			//G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_SYSTEM, (CT_CHAR*)&bcm, sizeof(bcm));
			//���ӵ������ͺͽ�ɫ���жϣ���õĵ���������ǽ�ɫƥ��ģ���������
			if (ib3.iClass==0||ib3.iClass==md.iOccupation)
			{
				//m_pUser->m_userItem.AutoAddItemData(threadid,taskReward.iItemID3,taskReward.iNum3);
				m_pUser->LootItems(SAVETAG_TASK_REWARD, &(taskReward.iItemID3), &(taskReward.iNum3));
			}
		}
		if (taskReward.iItemID4>0)
		{
			CMD_G_BroadCastMes bcm = {0};
			bcm.dwUserID = m_pUser->GetUserID();
			bcm.iSubType = BROADCAST_SUB_TYPE_ITEM_TASK;
			bcm.iQuality = ib4.iQuality;
			memcpy(bcm.cResultName,ib4.szName,NAME_SIZE);


			//G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_BROADCAST, MSC_SUB_BROADCAST_SYSTEM, (CT_CHAR*)&bcm, sizeof(bcm));
			//���ӵ������ͺͽ�ɫ���жϣ���õĵ���������ǽ�ɫƥ��ģ���������
			if (ib4.iClass==0||ib4.iClass==md.iOccupation)
			{
				//m_pUser->m_userItem.AutoAddItemData(threadid,taskReward.iItemID4,taskReward.iNum4);
				m_pUser->LootItems(SAVETAG_TASK_REWARD, &(taskReward.iItemID4), &(taskReward.iNum4));
			}
		}
	}

	//�����ִ����ɵ�������Ҫ������ɵ����񣬵ȱ�����ɺ���erase map
	if (iSendRewardType==TASK_SEND_REWARD_AUTO)
	{
		taskFinishList.iTaskList[taskFinishList.iTaskNum++] = iTaskID;
	}
	else
	{
		//�޸ķ������ڴ�����
		_AUTOMUTEX(m_pMutex);
		m_taskCompleteVector.push_back(iTaskID);
		TaskProgressMap_IT it = m_taskProgressMap.find(iTaskID);
		if (it != m_taskProgressMap.end())
		{
			m_taskProgressMap.erase(it);
		}
		//���ͽ�����ɺ󣬱�ʾ������ɣ���Ҫ�ж��Ƿ�����������Ҫ����
		GetTaskAutomatic(masterData.mvd.stGrade.iLevel);
	}
	//�޸����ݿ�����
	tr.dwUserID = m_pUser->GetUserID();
	tr.iTaskID = iTaskID;
	CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_RECORD_DATA,(CT_CHAR*)&tr,sizeof(TaskFinish));

	//���ݼ�¼
	MSC_LOGDB_TaskReward TaskReward={0};
	TaskReward.dwUserID=m_pUser->GetUserID();
	TaskReward.iUserGrade = masterData.mvd.stGrade.iLevel;
	TaskReward.iGold=taskReward.iGold;
	TaskReward.iDiamod=taskReward.iDiamod;
	TaskReward.iFood=taskReward.iFood;
	//TaskReward.iSkillSoul=taskReward.iSoul;
	TaskReward.iStamina=taskReward.iStamina;
	TaskReward.iCiviliNum=taskReward.iCiviliNum;
	TaskReward.iXp=taskReward.iXp;
	TaskReward.iPrestige=taskReward.iPrestige;
	TaskReward.iItemID1=taskReward.iItemID1;
	TaskReward.iItemID2=taskReward.iItemID2;
	TaskReward.iItemID3=taskReward.iItemID3;
	TaskReward.iItemID4=taskReward.iItemID4;
	TaskReward.iNum1=taskReward.iNum1;
	TaskReward.iNum2=taskReward.iNum2;
	TaskReward.iNum3=taskReward.iNum3;
	TaskReward.iNum4=taskReward.iNum4;
	TaskReward.iPotential=taskReward.iPotential;
	TaskReward.iSoul=taskReward.iSoul;
	TaskReward.iTaskID=iTaskID;
	if (iSendRewardType == TASK_SEND_REWARD_FAST)
	{
		G_TaskRelation taskRelation = {0};
		if (CCTaskScriptSetting::getSingleton().GetTaskInfo(iTaskID,&taskRelation))
		{
			TaskReward.iDiamodCost = taskRelation.iDiamod;
		}
	}
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_TASKFINISH,(CT_CHAR*)&TaskReward,sizeof(TaskReward));

	//������ύ���͵������������ʱ����Ҫ�۳���Ӧ�ĵ���
	G_TaskList taskList={0};
	//��ȡ�����������ж����������ύ�������񣬿۳�����
	CCTaskScriptSetting::getSingleton().GetSubTask(iTaskID,&taskList);
	threadid = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	for (CT_INT32 i = 0;i<taskList.iTaskNum;i++)
	{
		//��ȡ����������
		CT_INT32 iSubTaskType=0;
		if (CCTaskScriptSetting::getSingleton().GetSubTaskType(taskList.iTaskList[i],iSubTaskType))
		{
			if (iSubTaskType==TASK_MAIN_TYPE_ITEM)
			{
				G_TaskItem taskItem = {0};
				CCTaskScriptSetting::getSingleton().GetItemTaskInfo(taskList.iTaskList[i],&taskItem);
				//���ύ��������
				if (taskItem.iSubmitItem>0)
				{
					m_pUser->m_userItem.AutoExpendItem(SAVETAG_TASK_COST,taskItem.iSubmitItem,taskItem.iTime);
				}
			}
		}
		else
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
			CCLogMeg::getSingleton().WriteError("CCTaskEvent::DisposeRewardRequest,get task type failed main %d sub %d",iTaskID,taskList.iTaskList[i]);
			return;
		}
	}
}

//���������һ��
CT_VOID CCUserTask::DisposeSubTaskCompleteOnce(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CT_INT32 iTime,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	_AUTOMUTEX(m_pMutex);

	if (!ViewProgressTask(iTaskID))
	{
		return;
	}

	m_taskProgressMap[iTaskID][iSubTaskID].iTime +=iCompleteTime;
	if (m_taskProgressMap[iTaskID][iSubTaskID].iTime<0)
	{
		m_taskProgressMap[iTaskID][iSubTaskID].iTime=0;
	}

	// �ж��Ƿ�Ϊ��������
	if (iTaskID >= ISLAND_TASKID_MIN && iTaskID <= ISLAND_TASKID_MAX)
	{
		// ���¿ͻ��˴���
		CMD_UpdateTaskTimes utt = {0};
		utt.iTimes = m_taskProgressMap[iTaskID][iSubTaskID].iTime;
		utt.iStatus = 1;
		m_pUser->GetNetInterface()->Send(MAIN_G_ISLAND, SUB_G_UPDATE_TASK_TIMES, (CT_CHAR*)&utt, sizeof(utt));
	}

	//��������ﵽ��ɴ���
	if (m_taskProgressMap[iTaskID][iSubTaskID].iTime>=iTime)
	{
		m_taskProgressMap[iTaskID][iSubTaskID].iStatus = TASK_COMPLETED;

		CT_INT32 iRewardType = 0;
		// �ж��Ƿ�Ϊ��������
		if (iTaskID >= ISLAND_TASKID_MIN && iTaskID <= ISLAND_TASKID_MAX)
		{
			MSC_IslandTask mIslandTask = {0};
			mIslandTask.dwUserID = m_pUser->GetUserID();
			mIslandTask.iTaskID = iTaskID;
			CT_DWORD dwThreadID = GAME_THREAD_CITY +  m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;  
			G_SendThreadInfo(dwThreadID,   MSC_MAIN_ISLAND_EVENT, MSC_SUB_TASK_COMPLETE, (CT_CHAR*)&mIslandTask, sizeof(mIslandTask));
		}
		else if (CCTaskScriptSetting::getSingleton().GetMainTaskRewardType(iTaskID,iRewardType))
		{
			//���Զ����ܣ��ж������Ƿ����
			if (iRewardType==TASK_REWARD_AUTO)
			{
				TaskProgressMap_IT it = m_taskProgressMap.find(iTaskID);
				std::map<CT_INT32,SubTaskStatus>::iterator it2 = (*it).second.begin();
				while(it2!=(*it).second.end())
				{
					if ((*it2).second.iStatus!=TASK_COMPLETED)
					{
						break;
					}
					it2++;
				}
				//���������������
				if (it2==(*it).second.end())
				{
					//���ͽ���,�����Զ���ȡ����������ֱ�ӷ��ͽ���������Ҫ���û����������������
					SendTaskReward(iTaskID,iRewardType,taskList,TASK_SEND_REWARD_AUTO);
					return;
				}
			}
		}
		else
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
			CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeSubTaskCompleteOnce of GetMainTaskRewardType error");
			return;
		}
	}
	//���ֶ�������������Զ��Ļ�û�з��ͽ���������������״̬
	CMD_G_TaskUpdate taskUpdate = {0};
	taskUpdate.dwUserID = m_pUser->GetUserID();
	taskUpdate.iSubTaskID = iSubTaskID;
	taskUpdate.iTime = m_taskProgressMap[iTaskID][iSubTaskID].iTime;
	// �����������Ϳͻ���
	if (iTaskID < ISLAND_TASKID_MIN || iTaskID > ISLAND_TASKID_MAX)
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_UPDATE,(CT_CHAR*)&taskUpdate,sizeof(CMD_G_TaskUpdate));
	}
	MSC_DB_TaskProgressData taskProgressData = {0};
	taskProgressData.dwUserID = m_pUser->GetUserID();
	taskProgressData.iTaskID = iTaskID;
	taskProgressData.iSubTaskID = iSubTaskID;
	taskProgressData.iTime = m_taskProgressMap[iTaskID][iSubTaskID].iTime;  
	taskProgressData.iCompleted = m_taskProgressMap[iTaskID][iSubTaskID].iStatus;
	CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_DATA,(CT_CHAR*)&taskProgressData,sizeof(taskProgressData));
}

CT_VOID CCUserTask::DisposeUpgradeTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskUpgrade taskUpgrade = {0};
	if (CCTaskScriptSetting::getSingleton().GetUpgradeTaskInfo(iSubTaskID,&taskUpgrade))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_UPGRADE_MASTER:
			{
				if (taskUpgrade.iMaster!=TASK_SUB_UN_CASE_VALUE)
				{
					//Ӣ�۵ȼ����ڼ���ȿ���
					if (taskUpgrade.iLevel==TASK_SUB_CASE_VALUE||(taskUpgrade.iLevel<=taskEventInfo.iLevel))
					{
						//�������һ��
						DisposeSubTaskCompleteOnce(iTaskID,taskUpgrade.iSubTaskID,taskUpgrade.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_UPGRADE_CIVIL:
			{
				if (taskUpgrade.iCivil!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskUpgrade.iLevel==TASK_SUB_CASE_VALUE||taskUpgrade.iLevel<=taskEventInfo.iLevel)
					{
						//�������һ��
						DisposeSubTaskCompleteOnce(iTaskID,taskUpgrade.iSubTaskID,taskUpgrade.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_UPGRADE_RANK:
			{
				if (taskUpgrade.iRank!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskUpgrade.iLevel==TASK_SUB_CASE_VALUE||taskUpgrade.iLevel<=taskEventInfo.iLevel)
					{
						//�������һ��
						DisposeSubTaskCompleteOnce(iTaskID,taskUpgrade.iSubTaskID,taskUpgrade.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_UPGRADE_BUILD:
			{
				if (taskUpgrade.iBuild!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskUpgrade.iBuild==TASK_SUB_CASE_VALUE||taskUpgrade.iBuild==taskEventInfo.iSubType)&&(taskUpgrade.iLevel==TASK_SUB_CASE_VALUE||taskUpgrade.iLevel<=taskEventInfo.iLevel))
					{
						//�������һ��
						DisposeSubTaskCompleteOnce(iTaskID,taskUpgrade.iSubTaskID,taskUpgrade.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_UPGRADE_PET:
			{
				if (taskUpgrade.iPet!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskUpgrade.iPet==TASK_SUB_CASE_VALUE||taskUpgrade.iPet==taskEventInfo.iSubType)&&(taskUpgrade.iLevel==TASK_SUB_CASE_VALUE||taskUpgrade.iLevel<=taskEventInfo.iLevel))
					{
						//�������һ��
						DisposeSubTaskCompleteOnce(iTaskID,taskUpgrade.iSubTaskID,taskUpgrade.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_UPGRADE_ARM:
			{
				if (taskUpgrade.iArm!=TASK_SUB_UN_CASE_VALUE)
				{
					if (((taskUpgrade.iArm==TASK_SUB_CASE_VALUE||taskUpgrade.iArm==taskEventInfo.iSubType)&&(taskUpgrade.iLevel==TASK_SUB_CASE_VALUE||taskUpgrade.iLevel<=taskEventInfo.iLevel)) || m_pUser->m_userArmy.OwnArmyAdvanced(taskEventInfo.iSubType))
					{
						//�������һ��
						DisposeSubTaskCompleteOnce(iTaskID,taskUpgrade.iSubTaskID,taskUpgrade.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_UPGRADE_ARMADV:
			{
				if (taskUpgrade.iArmAdv!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskUpgrade.iArmAdv==taskEventInfo.iSubType&&taskUpgrade.iLevel<=taskEventInfo.iLevel) || (taskUpgrade.iArmAdv==TASK_SUB_CASE_VALUE&&m_pUser->m_userArmy.OwnArmyAdvanNum(taskEventInfo.iLevel)))
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskUpgrade.iSubTaskID,taskUpgrade.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeUpgradeTask of GetUpgradeTaskInfo error");
		return;
	}
}

CT_VOID CCUserTask::DisposeDuplicateTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskDuplicate taskDuplicate = {0};
	if (CCTaskScriptSetting::getSingleton().GetDuplicateTaskInfo(iSubTaskID,&taskDuplicate))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_DUPLICATE_MONSTER:
			{
				if (taskDuplicate.iMonster!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskDuplicate.iMonster==TASK_SUB_CASE_VALUE||taskDuplicate.iMonster==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskDuplicate.iSubTaskID,taskDuplicate.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_DUPLICATE_POINTOR:
			{
				if (taskDuplicate.iCheckP!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskDuplicate.iCheckP==TASK_SUB_CASE_VALUE||taskDuplicate.iCheckP==taskEventInfo.iSubType)&&(taskDuplicate.iLevel==TASK_SUB_CASE_VALUE||taskDuplicate.iLevel==taskEventInfo.iLevel))
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskDuplicate.iSubTaskID,taskDuplicate.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeDuplicateTask of GetDuplicateTaskInfo error");
		return;
	}
}

CT_VOID CCUserTask::DisposeItemTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskItem taskItem = {0};
	if (CCTaskScriptSetting::getSingleton().GetItemTaskInfo(iSubTaskID,&taskItem))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_ITEM_SUBMIT:
			{
				if (taskItem.iSubmitItem!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskItem.iSubmitItem==TASK_SUB_CASE_VALUE||taskItem.iSubmitItem==taskEventInfo.iSubType)&&(taskItem.iLevel==TASK_SUB_CASE_VALUE||taskItem.iLevel==taskEventInfo.iLevel))
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskItem.iSubTaskID,taskItem.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_ITEM_OWN:
			{
				if (taskItem.iOwnItem!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskItem.iOwnItem==TASK_SUB_CASE_VALUE||taskItem.iOwnItem==taskEventInfo.iSubType)&&(taskItem.iLevel==TASK_SUB_CASE_VALUE||taskItem.iLevel==taskEventInfo.iLevel))
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskItem.iSubTaskID,taskItem.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_ITEM_USE:
			{
				if (taskItem.iUseItem!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskItem.iUseItem==TASK_SUB_CASE_VALUE||taskItem.iUseItem==taskEventInfo.iSubType)&&(taskItem.iLevel==TASK_SUB_CASE_VALUE||taskItem.iLevel==taskEventInfo.iLevel))
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskItem.iSubTaskID,taskItem.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
			//����û�м���
		case TASK_SUB_TYPE_ITEM_ENLARGE:
			{
				if (taskItem.iEnlargeItem!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskItem.iEnlargeItem==TASK_SUB_CASE_VALUE||taskItem.iEnlargeItem==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskItem.iSubTaskID,taskItem.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
			//��ħ
		case TASK_SUB_TYPE_ITEM_ENCHANT:
			{
				if (taskItem.iEnchant!=TASK_SUB_UN_CASE_VALUE)
				{
					if ((taskItem.iEnchant==TASK_SUB_CASE_VALUE||taskItem.iEnchant==taskEventInfo.iSubType)&&(taskItem.iLevel==TASK_SUB_CASE_VALUE||taskItem.iLevel==taskEventInfo.iLevel))
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskItem.iSubTaskID,taskItem.iTime,taskList,iCompleteTime);
					}
				}
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeItemTask of GetItemTaskInfo error");
		return;
	}
}

CT_VOID CCUserTask::DisposeSmithyTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskSmithy taskSmithy = {0};
	if (CCTaskScriptSetting::getSingleton().GetSmithyTaskInfo(iSubTaskID,&taskSmithy))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_SMITHY_STRENTH:
			{
				if (taskSmithy.iStrenth!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskSmithy.iStrenth==TASK_SUB_CASE_VALUE||taskSmithy.iStrenth==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,iSubTaskID,taskSmithy.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_SMITHY_MIX:
			{
				if (taskSmithy.iMix!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskSmithy.iMix==TASK_SUB_CASE_VALUE||taskSmithy.iMix==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,iSubTaskID,taskSmithy.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_SMITHY_MOSAIC:
			{
				if (taskSmithy.iMosaic!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskSmithy.iMosaic==TASK_SUB_CASE_VALUE||taskSmithy.iMosaic==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,iSubTaskID,taskSmithy.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_SMITHY_PUNCH:
			{
				if (taskSmithy.iPunch!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskSmithy.iPunch==TASK_SUB_CASE_VALUE||taskSmithy.iPunch==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,iSubTaskID,taskSmithy.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_SMITHY_REFINE:
			{
				if (taskSmithy.iRefine!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskSmithy.iRefine==TASK_SUB_CASE_VALUE||taskSmithy.iRefine==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,iSubTaskID,taskSmithy.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_SMITHY_INHERIT:
			{
				if (taskSmithy.iInherit!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskSmithy.iInherit==TASK_SUB_CASE_VALUE||taskSmithy.iInherit==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,iSubTaskID,taskSmithy.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeSmithyTask of GetSmithyTaskInfo error");
		return;
	}
}

CT_VOID CCUserTask::DisposePotentialTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskPotential taskPotential = {0};
	if (CCTaskScriptSetting::getSingleton().GetPotentialTaskInfo(iSubTaskID,&taskPotential))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_POTENTIAL_LEARN:
			{
				if (taskPotential.iLearn!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskPotential.iLearn==TASK_SUB_CASE_VALUE||taskPotential.iLearn==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskPotential.iSubTaskID,taskPotential.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_POTENTIAL_STRNETH:
			{
				if (taskPotential.iStrength!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskPotential.iStrength==TASK_SUB_CASE_VALUE||taskPotential.iStrength==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskPotential.iSubTaskID,taskPotential.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposePotentialTask of GetPotentialTaskInfo error");
		return;
	}
}

CT_VOID CCUserTask::DisposeCityTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskCity taskCity = {0};
	if (CCTaskScriptSetting::getSingleton().GetCityTaskInfo(iSubTaskID,&taskCity))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_CITY_REGION:
			{
				if (taskCity.iRegion!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskCity.iRegion==TASK_SUB_CASE_VALUE||taskCity.iRegion==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskCity.iSubTaskID,taskCity.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_CITY_BUILD:
			{
				if (taskCity.iOwnBuilding!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskCity.iOwnBuilding==TASK_SUB_CASE_VALUE||taskCity.iOwnBuilding==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskCity.iSubTaskID,taskCity.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_CITY_EVENT:
			{
				if (taskCity.iEvent!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskCity.iEvent==TASK_SUB_CASE_VALUE || taskCity.iEvent == taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskCity.iSubTaskID,taskCity.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_CITY_HAVEST:
			{
				if (taskCity.iHavest!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskCity.iHavest==TASK_SUB_CASE_VALUE||taskCity.iHavest==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskCity.iSubTaskID,taskCity.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_CITY_PRODUCT:
			{
				if (taskCity.iProduct!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskCity.iProduct==TASK_SUB_CASE_VALUE||taskCity.iProduct==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskCity.iSubTaskID,taskCity.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_CITY_WORSHIP:
			{
				if (taskCity.iWorship!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskCity.iWorship==TASK_SUB_CASE_VALUE||taskCity.iWorship==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskCity.iSubTaskID,taskCity.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_CITY_ROLE:
			{
				if (taskCity.iRole!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskCity.iRole==TASK_SUB_CASE_VALUE||taskCity.iRole==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskCity.iSubTaskID,taskCity.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeCityTask of GetCityTaskInfo error");
		return;
	}
}

CT_VOID CCUserTask::DisposeArenaTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskArena taskArena = {0};
	if (CCTaskScriptSetting::getSingleton().GetArenaTaskInfo(iSubTaskID,&taskArena))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_ARENA_FIGHT:
			{
				if (taskArena.iFight!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskArena.iFight==TASK_SUB_CASE_VALUE||taskArena.iFight==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskArena.iSubTaskID,taskArena.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_ARENA_CHEST:
			{
				if (taskArena.iChest!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskArena.iChest==TASK_SUB_CASE_VALUE||taskArena.iChest==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskArena.iSubTaskID,taskArena.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeArenaTask of GetArenaTaskInfo error");
		return;
	}
}

CT_VOID CCUserTask::DisposeFightTask(CT_INT32 iTaskID,CT_INT32 iSubTaskID,CMD_G_TaskEventInfo taskEventInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
	G_TaskFight taskFight = {0};
	if (CCTaskScriptSetting::getSingleton().GetFightTaskInfo(iSubTaskID,&taskFight))
	{
		switch(taskEventInfo.iTaskSubType)
		{
		case TASK_SUB_TYPE_FIGHT_BATTER:
			{
				if (taskFight.iBatter!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskFight.iBatter==TASK_SUB_CASE_VALUE||taskFight.iBatter==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskFight.iSubTaskID,taskFight.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_FIGHT_FORMATION:
			{
				if (taskFight.iFormation!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskFight.iFormation==TASK_SUB_CASE_VALUE
						||(taskFight.iFormation < UNIT_ALL_UNIT && taskFight.iFormation == taskEventInfo.iSubType)
						||(taskFight.iFormation > UNIT_ALL_UNIT && taskFight.iFormation/10 == taskEventInfo.iSubType))
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskFight.iSubTaskID,taskFight.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		case TASK_SUB_TYPE_FIGHT_SKILL:
			{
				if (taskFight.iSkill!=TASK_SUB_UN_CASE_VALUE)
				{
					if (taskFight.iSkill==TASK_SUB_CASE_VALUE||taskFight.iSkill==taskEventInfo.iSubType)
					{
						DisposeSubTaskCompleteOnce(iTaskID,taskFight.iSubTaskID,taskFight.iTime,taskList,iCompleteTime);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}
	else
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeFightTask of GetFightTaskInfo error");
		return;
	}
}


// �ж�ָ���ؿ�ͨ������,����0���ؿ�ID
CT_BOOL CCUserTask::DisposePointorTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0];
}

// ��ս�ؿ�����ؿ�n�Σ�����0���ؿ�id1������1���ؿ�id2
CT_BOOL CCUserTask::DisposeChallengeTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return (stTaskSys.iParameter[0] == 0) 
		|| (taskSysInfo.arrayParam[0] >= stTaskSys.iParameter[0] && taskSysInfo.arrayParam[0] <= stTaskSys.iParameter[1]);
}

// ����ָ������������,����0���ؿ�id1������1���ؿ�id2
CT_BOOL CCUserTask::DisposeFightTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return (stTaskSys.iParameter[0] == 0) 
		|| (taskSysInfo.arrayParam[0] >= stTaskSys.iParameter[0] && taskSysInfo.arrayParam[0] <= stTaskSys.iParameter[1]);
}

// ���n��ף��
CT_BOOL CCUserTask::DisposeBlessTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return CT_TRUE;
}

// ��������n�Σ�����0������id
CT_BOOL CCUserTask::DisposeChestTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return stTaskSys.iParameter[0] == 0 || taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0];
}

// ���ָ��Ԫ�أ�����0�����ͣ�����1��Ʒ�ʣ�����2���ȼ�
CT_BOOL CCUserTask::DisposeGetElmTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return (stTaskSys.iParameter[0] == 0 || taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0])
		&& (stTaskSys.iParameter[1] == 0 || taskSysInfo.arrayParam[1] >= stTaskSys.iParameter[1])
		&& (stTaskSys.iParameter[2] == 0 || taskSysInfo.arrayParam[2] >= stTaskSys.iParameter[2]);
}

// װ��ָ��Ԫ�أ�����0�����ͣ�����1��Ʒ�ʣ�����2���ȼ�
CT_BOOL CCUserTask::DisposeEquipElmTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return (stTaskSys.iParameter[0] == 0 || taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0])
		&& (stTaskSys.iParameter[1] == 0 || taskSysInfo.arrayParam[1] >= stTaskSys.iParameter[1])
		&& (stTaskSys.iParameter[2] == 0 || taskSysInfo.arrayParam[2] >= stTaskSys.iParameter[2]);	
    
}

// ����ָ�����ģ�����0����ɫid
CT_BOOL CCUserTask::DisposeFlagTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return (stTaskSys.iParameter[0] == 0 || (taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0]));
}

// ����
CT_BOOL CCUserTask::DisposeSwallowTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return CT_TRUE;
}

// ����ָ����Դ/���ߣ�����0�����ͣ�����1����ԴID
CT_BOOL CCUserTask::DisposeDonateTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return (taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0])
		&& (taskSysInfo.arrayParam[1] == stTaskSys.iParameter[1]);
}

// ָ���Ƽ�����������0: 0���޶����ͣ�1������2��ɫ������1������id/��ɫid������2��ս������id������û�У�
CT_BOOL CCUserTask::DisposeUpgradeTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	if (stTaskSys.iParameter[0] == 0 || stTaskSys.iParameter[0] == TASK_SCIENCE_BUILD )
	{
		return taskSysInfo.arrayParam[1] == stTaskSys.iParameter[1];
	}
	if (stTaskSys.iParameter[0] == TASK_SCIENCE_FIGHT)
	{
		return (taskSysInfo.arrayParam[1] == stTaskSys.iParameter[1])
			&& (taskSysInfo.arrayParam[2] == stTaskSys.iParameter[2]);
	}
	return CT_FALSE;
}

// �����ٻ�Boss������0��Boss id
CT_BOOL CCUserTask::DisposeCallBossTask(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0];
}

// ��ɱBoss,����0��Boss id
CT_BOOL CCUserTask::DisposeFitBossTask(const CMD_G_TaskSystemInfo &taskSysInfo,const G_TaskSystem &stTaskSys)
{
	return taskSysInfo.arrayParam[0] == stTaskSys.iParameter[0];
}

// ָ����������������0���������ͣ�����1������id������2����������
CT_BOOL CCUserTask::DisposeBuidingProduct(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	if (stTaskSys.iParameter[0] == -1 || stTaskSys.iParameter[0] == 0 || stTaskSys.iParameter[0] == taskSysInfo.arrayParam[0])
	{
		return (stTaskSys.iParameter[1] == 0 || stTaskSys.iParameter[1] == -1 || stTaskSys.iParameter[1] == taskSysInfo.arrayParam[1]) 
			&& (stTaskSys.iParameter[2] == taskSysInfo.arrayParam[2]);
	}

	return CT_FALSE;
}

// ָ��������������������0���������ͣ�����1������id������2: 1Ϊ����n�Σ�2Ϊ����ָ���ȼ�
CT_BOOL CCUserTask::DisposeBuidingUpgrade(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	if (!(stTaskSys.iParameter[0] == -1 || stTaskSys.iParameter[0] == 0 || stTaskSys.iParameter[0] == taskSysInfo.arrayParam[0]))
	{
		return CT_FALSE;
	}
	if (!(stTaskSys.iParameter[1] == 0 || stTaskSys.iParameter[1] == -1 || stTaskSys.iParameter[1] == taskSysInfo.arrayParam[1]))
	{
		return CT_FALSE;
	}
	if (!(stTaskSys.iParameter[2] == 1 || (stTaskSys.iParameter[2] == 2 && stTaskSys.iTime <= taskSysInfo.iProgress)))
	{
		return CT_FALSE;
	}

	return CT_TRUE;
}

// ����/����ָ���ȼ���ս��
CT_BOOL CCUserTask::DisposeInFightTeam(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return CT_TRUE;
}

// ����ָ��װ��n��:����0��װ���ȼ�������1��װ��Ʒ�ʣ�����2�����λ�ã�����3��ID
CT_BOOL CCUserTask::DisposeWearEquipment(CT_INT32 iTaskID, CT_INT32 iSubTaskID,const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	if (stTaskSys.iParameter[2] == 0)
	{
		CT_INT32 iCount = 0;
		CT_INT32 iLen = EQUIPED_ITEM_COUNT * sizeof(CMD_G_ItemData);
		CT_CHAR pBuf[EQUIPED_ITEM_COUNT * sizeof(CMD_G_ItemData)] = {0};
		CMD_G_ItemData *pItemBase;
		EquipmentData equipData = {0};
		G_ItemBase itemBase = {0};

		m_pUser->m_userItem.GetEquipedItemData(pBuf, iLen);
		iLen /= sizeof(CMD_G_ItemData);
		pItemBase = (CMD_G_ItemData *)pBuf;

		for (int i = 0; i < iLen && pItemBase != NULL; ++i,++pItemBase)
		{
			m_pUser->m_userEquip.GetData(pItemBase->key, equipData);
			G_GetItemBase(pItemBase->iID, &itemBase);

			if ((equipData.iLevel >= stTaskSys.iParameter[0] || stTaskSys.iParameter[0] == 0)
				&& (itemBase.iQuality >= stTaskSys.iParameter[1] || stTaskSys.iParameter[1] == 0)
				&& (pItemBase->iID >= stTaskSys.iParameter[3] || stTaskSys.iParameter[3] == 0))
			{
				iCount++;
			}
		}

		if (m_taskProgressMap[iTaskID][iSubTaskID].iTime < iCount)
		{
			return CT_TRUE;
		}
	}
	else 
	{
		if ((stTaskSys.iParameter[0] == 0 || stTaskSys.iParameter[0] <= taskSysInfo.arrayParam[0])
			&& (stTaskSys.iParameter[1] == 0 || stTaskSys.iParameter[1] <= taskSysInfo.arrayParam[1])
			&& (stTaskSys.iParameter[2] == taskSysInfo.arrayParam[2])
			&& (stTaskSys.iParameter[3] == 0 || stTaskSys.iParameter[3] == taskSysInfo.arrayParam[3]))
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

// ̽��ָ���������n��
CT_BOOL CCUserTask::DisposeSeekTimes(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	if (stTaskSys.iParameter[0] == taskSysInfo.arrayParam[0])
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

// ��ǽ��������
CT_BOOL CCUserTask::DisposeWallArmy(const CMD_G_TaskSystemInfo &taskSysInfo, const G_TaskSystem &stTaskSys)
{
	return CT_TRUE;
}

//���������͵���������
CT_VOID CCUserTask::DisposeSubTask(CT_INT32 iTaskID, CT_INT32 iSubTaskID,CMD_G_TaskSystemInfo taskSysInfo,G_TaskList &taskList,CT_INT32 iCompleteTime)
{
    G_TaskSystem stTaskSys = {0};
    if (G_GetTaskSystem(iSubTaskID, &stTaskSys))
    {
        CT_BOOL bJudge = CT_FALSE;
		if (stTaskSys.iType == taskSysInfo.iTaskSubType)
			{
        
				switch (taskSysInfo.iTaskSubType) {
					// TODO: ��������ִ�й����ж�
					case TASK_SUB_TYPE_SYSTEM_POINTOR: 
						{		
							bJudge = DisposePointorTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_CHALLENGE:
						{
			    			bJudge = DisposeChallengeTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_FIGHT:
						{
			    			bJudge = DisposeFightTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_BLESS:
						{
			    			bJudge = DisposeBlessTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_CHEST:
						{
			    			bJudge = DisposeChestTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_GETELM:
						{
		     				bJudge = DisposeGetElmTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_EQUIPELM:
						{
		    				bJudge = DisposeEquipElmTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_FLAG_UPGRADE:
						{
			    			bJudge = DisposeFlagTask(taskSysInfo, stTaskSys);
						}
					case TASK_SUB_TYPE_SYSTEM_SWALLOW:
						{
			    			bJudge = DisposeSwallowTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_DONATE:
						{
			    			bJudge = DisposeDonateTask(taskSysInfo, stTaskSys);
							iCompleteTime = taskSysInfo.iProgress;
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_SCIENCE_UP:
						{
			    			bJudge = DisposeUpgradeTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_CALLBOSS:
						{
			    			bJudge = DisposeCallBossTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_FITBOSS:
						{
			    			bJudge = DisposeFitBossTask(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_PRODUCT:
						{
							bJudge = DisposeBuidingProduct(taskSysInfo,stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_BUILD_UPGRADE:
						{
							bJudge = DisposeBuidingUpgrade(taskSysInfo,stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_INFIGHTTEAM:
						{
							bJudge = DisposeInFightTeam(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_WEAR_EQUIPMENT:
						{
							bJudge = DisposeWearEquipment(iTaskID, iSubTaskID, taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_SEEK_TIMES:
						{
							bJudge = DisposeSeekTimes(taskSysInfo, stTaskSys);
							break;
						}
					case TASK_SUB_TYPE_SYSTEM_WALL_ARMY:
						{
							bJudge = DisposeWallArmy(taskSysInfo, stTaskSys);
							break;
						}
					default:
						break;
				}
		}
        if (bJudge) {
            DisposeSubTaskCompleteOnce(iTaskID, iSubTaskID, stTaskSys.iTime, taskList, iCompleteTime);
        }
	}else {
        m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
        CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeSubTask of G_GetTaskSystem error");
    }    
}

CT_VOID CCUserTask::DisposeTask( CMD_G_TaskSystemInfo taskSysInfo,CT_INT32 iCompleteTime)
{
    TaskProgressMap_IT	it1 = m_taskProgressMap.begin();
	G_TaskList taskList = {0};
	CT_INT32 iSubTaskType;
	while(it1!=m_taskProgressMap.end())
	{
		std::map<CT_INT32,SubTaskStatus>::iterator it2 = (*it1).second.begin();
		while (it2!=(*it1).second.end())
		{
			//�����������
			if ((*it2).second.iStatus)
			{
				it2++;
				continue;
			}
			iSubTaskType = 0;
			if (CCTaskScriptSetting::getSingleton().GetSubTaskType((*it2).first, iSubTaskType))
			{
				if (iSubTaskType == TASK_MAIN_TYPE_SYS)
				{
					DisposeSubTask((*it1).first,(*it2).first, taskSysInfo, taskList, iCompleteTime);
				}
			}
            /*
			else
			{
				m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
				CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeSubTask of G_GetTaskSystem error.Cannot find iSubTaskType,iSubID: %d.",(*it2).first);
			}
            */
			it2++;
		}
		it1++;
	}
	//��������ɣ���Ҫ����map
	if (taskList.iTaskNum>0)
	{
		for (int j=0;j<taskList.iTaskNum;j++)
		{
			_AUTOMUTEX(m_pMutex);
			m_taskCompleteVector.push_back(taskList.iTaskList[j]);
			TaskProgressMap_IT it = m_taskProgressMap.find(taskList.iTaskList[j]);
			if (it != m_taskProgressMap.end())
			{
				m_taskProgressMap.erase(it);
			}
		}
	}
	//���ͽ�����ɺ󣬱�ʾ������ɣ���Ҫ�ж��Ƿ�����������Ҫ����
	MasterData masterData = {0};
	m_pUser->m_userBase.GetMasterData(masterData);
	CMD_G_TaskReward taskRewardSend = {0};
	GetTaskAutomatic(masterData.mvd.stGrade.iLevel);
}

CT_VOID CCUserTask::DisposeTask( CMD_G_TaskEventInfo taskEventInfo,CT_INT32 iCompleteTime)
{
	TaskProgressMap_IT	it1 = m_taskProgressMap.begin();
	G_TaskList taskList = {0};
	while(it1!=m_taskProgressMap.end())
	{
		std::map<CT_INT32,SubTaskStatus>::iterator it2 = (*it1).second.begin();
		while (it2!=(*it1).second.end())
		{
			//�����������
			if ((*it2).second.iStatus)
			{
				it2++;
				continue;
			}
			CT_INT32 iSubTaskType=0;
			if (CCTaskScriptSetting::getSingleton().GetSubTaskType((*it2).first,iSubTaskType))
			{
				if (iSubTaskType==taskEventInfo.iTaskMainType)
				{
					switch(taskEventInfo.iTaskMainType)
					{
					case TASK_MAIN_TYPE_UPGRADE:
						{
							DisposeUpgradeTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					case TASK_MAIN_TYPE_DUPLICATE:
						{
							DisposeDuplicateTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					case TASK_MAIN_TYPE_ITEM:
						{
							DisposeItemTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					case TASK_MAIN_TYPE_SMITHY:
						{
							DisposeSmithyTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					case TASK_MAIN_TYPE_POTENTIAL:
						{
							DisposePotentialTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					case TASK_MAIN_TYPE_CITY:
						{
							DisposeCityTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					case TASK_MAIN_TYPE_ARENA:
						{
							DisposeArenaTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					case TASK_MAIN_TYPE_FIGHT:
						{
							DisposeFightTask((*it1).first,(*it2).first,taskEventInfo,taskList,iCompleteTime);
							break;
						}
					default:
						{
							break;
						}
					}
				}
			}
			else
			{
				m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_DATA_ERROR);
				CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeTask of GetSubTaskType error, subTaskID:%d", (*it2).first);
				return;
			}
			it2++;
		}
		it1++;
	}
	//��������ɣ���Ҫ����map
	if (taskList.iTaskNum>0)
	{
		for (int j=0;j<taskList.iTaskNum;j++)
		{
			_AUTOMUTEX(m_pMutex);
			m_taskCompleteVector.push_back(taskList.iTaskList[j]);
			TaskProgressMap_IT it = m_taskProgressMap.find(taskList.iTaskList[j]);
			if (it != m_taskProgressMap.end())
			{
				m_taskProgressMap.erase(it);
			}
		}
	}
	//���ͽ�����ɺ󣬱�ʾ������ɣ���Ҫ�ж��Ƿ�����������Ҫ����
	MasterData masterData = {0};
	m_pUser->m_userBase.GetMasterData(masterData);
	CMD_G_TaskReward taskRewardSend = {0};
	GetTaskAutomatic(masterData.mvd.stGrade.iLevel);
}

CT_VOID CCUserTask::DisposeFightTeamUpgrade(CT_CHAR *pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_FightTeamTask))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTask::DisposeFightTeamUpgrade iLen != sizeof(MSC_FightTeamTask");
		return;
	}
	MSC_FightTeamTask *pft = (MSC_FightTeamTask*)pBuf;

	CT_INT32 iCompletTime = pft->iFightTeamGrade;
	CT_INT32 iStatus;
	iCompletTime >= pft->sysTask.iprogress? iStatus = TASK_COMPLETED:iStatus = 0;

	m_taskProgressMap[pft->iTaskID][pft->iSubTaskID].iTime = iCompletTime;
	m_taskProgressMap[pft->iTaskID][pft->iSubTaskID].iStatus = iStatus;

	CMD_G_TaskUpdate taskUpdata = {0};
	taskUpdata.dwUserID = pft->dwUserID;
	taskUpdata.iSubTaskID = pft->iSubTaskID;
	taskUpdata.iTime = iCompletTime;

	if (pft->iType == TASK_FIGHTTEAM_ADD)		//����ս�ӵȼ�����
	{
		MSC_DB_TaskProgressData taskData = {0};
		taskData.dwUserID = pft->dwUserID;
		taskData.iCompleted = iStatus;
		taskData.iSubTaskID = pft->iSubTaskID;
		taskData.iTaskID = pft->iTaskID;
		taskData.iTime = iCompletTime;
		//�������ݿ�
		CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadID,MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_DATA,(CT_CHAR*)&taskData,sizeof(taskData));

		
		//���Ϳͻ���
		m_pUser->GetNetInterface()->Send(MAIN_G_TASK_EVENT_CITY,SUB_G_TASK_UPDATE,(CT_CHAR*)&taskUpdata,sizeof(taskUpdata));
	}
	else if(pft->iType == TASK_FIGHTTEAM_ENTER)		//��½ʱ��ȡս�ӵȼ�����
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV, SUB_G_TASK_INFO, (CT_CHAR*)&taskUpdata, sizeof(taskUpdata));
	}
}

//�����������
CT_VOID  CCUserTask::DisposeIslandTask(CT_INT32 iTaskID)
{
	_AUTOMUTEX(m_pMutex);
	TaskProgressMap_IT it = m_taskProgressMap.find(iTaskID);
	if (it != m_taskProgressMap.end())
	{
		m_taskProgressMap.erase(it);
	}

	//�޸����ݿ�����
	TaskFinish tf = {0};
	tf.dwUserID = m_pUser->GetUserID();
	tf.iTaskID = iTaskID;
	CT_DWORD threadID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadID,MSC_MAIN_DB_TASK_UPDATE,MSC_SUB_DB_TASK_RECORD_DATA,(CT_CHAR*)&tf,sizeof(TaskFinish));
}

//6�㶨ʱ�����������
CT_VOID  CCUserTask::UpdateIslandTaskOnClock()
{
	_AUTOMUTEX(m_pMutex);
	TaskProgressMap_IT it = m_taskProgressMap.begin();
	TaskProgressMap_IT itTmp;
	for (;it != m_taskProgressMap.end();)
	{
		itTmp = it;
		++it;
		if (itTmp->first >= ISLAND_TASKID_MIN && itTmp->first <= ISLAND_TASKID_MAX)
		{
			m_taskProgressMap.erase(itTmp);
		}
	}
}