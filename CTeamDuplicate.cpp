#include "CTeamDuplicate.h"
#include "CUserManager.h"

template<> CCTeamDuplicate* CCSingleton<CCTeamDuplicate>::ms_Singleton=0;

CCTeamDuplicate::CCTeamDuplicate(void)
{
	m_RoomIDList.clear();
	m_RoomMap.clear();
	m_RoomInfoMap.clear();
	m_UserRoomMap.clear();
	m_SceneMap.clear();
}

CCTeamDuplicate::~CCTeamDuplicate(void)
{
	m_RoomIDList.clear();
	m_RoomMap.clear();
	m_RoomInfoMap.clear();
	m_UserRoomMap.clear();
	m_SceneMap.clear();
}

//创建房间
CT_INT32 CCTeamDuplicate::CreateRoom(TeamDuplicateRoomInfo &RoomInfo)
{
	_AUTOMUTEX(&m_Mutex);
	UR_IT it=m_UserRoomMap.find(RoomInfo.dwUserID);
	if(it != m_UserRoomMap.end())
	{
		RI_IT riit=m_RoomInfoMap.find((*it).second);
		if(riit != m_RoomInfoMap.end())
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if(RoomInfo.dwUserID == (*riit).second.FightingData[i].dwUserID)
				{
					if((*riit).second.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
						return TEAMDUPLICATE_CREATE_NOFREE;
				}
			}
		}
	}

	//分配ID
	if(m_RoomIDList.begin() != m_RoomIDList.end())
	{
		RID_IT it=m_RoomIDList.begin();

		TeamDuplicateRoom Room={0};
		Room.dwUserID=RoomInfo.dwUserID;
		Room.iCount=1;
		Room.iDifficulty=RoomInfo.iDifficulty;
		Room.iSceneID=RoomInfo.iSceneID;
		Room.iRoomID=(*it);
		RoomInfo.iStatus[0]=TEAMDUPLICATE_STATUS_FREE;
		RoomInfo.iStatus[1]=TEAMDUPLICATE_STATUS_FREE;
		RoomInfo.iStatus[2]=TEAMDUPLICATE_STATUS_FREE;
		RoomInfo.iRoomID=(*it);
		RoomInfo.iAutoMatch[0]=TEAMDUPLICATE_AUTOMATCH_FALSE;

		if(m_RoomMap.find(Room.iRoomID) != m_RoomMap.end())
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::CreateRoom m_RoomMap.find(Room.iRoomID) != m_RoomMap.end()");
			return TEAMDUPLICATE_CREATE_FAIL;
		}
		if(m_RoomInfoMap.find(RoomInfo.iRoomID) != m_RoomInfoMap.end())
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::CreateRoom m_RoomInfoMap.find(RoomInfo.iRoomID) != m_RoomInfoMap.end()");
			return TEAMDUPLICATE_CREATE_FAIL;
		}

		m_RoomMap.insert(RoomMap::value_type(Room.iRoomID,Room));
		m_RoomInfoMap.insert(RoomInfoMap::value_type(RoomInfo.iRoomID,RoomInfo));
		m_UserRoomMap.insert(UserRoomMap::value_type(Room.dwUserID,Room.iRoomID));

		m_RoomIDList.erase(it);

		SM_IT smit=m_SceneMap.find(Room.iSceneID);
		if(smit != m_SceneMap.end())
		{
			(*smit).second.push_back(Room.iRoomID);
		}
		else
		{
			RoomIDList RIDList;
			RIDList.push_back(Room.iRoomID);
			m_SceneMap.insert(SceneMap::value_type(Room.iSceneID,RIDList));
		}
		return TEAMDUPLICATE_CREATE_SUC;
	}
	else
	{
		TeamDuplicateRoom Room={0};
		Room.dwUserID=RoomInfo.dwUserID;
		Room.iCount=1;
		Room.iDifficulty=RoomInfo.iDifficulty;
		Room.iSceneID=RoomInfo.iSceneID;
		RoomInfo.iStatus[0]=TEAMDUPLICATE_STATUS_FREE;
		RoomInfo.iStatus[1]=TEAMDUPLICATE_STATUS_FREE;
		RoomInfo.iStatus[2]=TEAMDUPLICATE_STATUS_FREE;
		Room.iRoomID=m_RoomInfoMap.size()+1;
		RoomInfo.iRoomID=m_RoomInfoMap.size()+1;
		RoomInfo.iAutoMatch[0]=TEAMDUPLICATE_AUTOMATCH_FALSE;

		RM_IT it=m_RoomMap.find(Room.iRoomID);
		if(it != m_RoomMap.end())
		{
			//CT_ASSERT(CT_FALSE);
			return TEAMDUPLICATE_CREATE_FAIL;
		}

		if(m_RoomMap.find(Room.iRoomID) != m_RoomMap.end())
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::CreateRoom m_RoomMap.find(Room.iRoomID) != m_RoomMap.end()");
			return TEAMDUPLICATE_CREATE_FAIL;
		}
		if(m_RoomInfoMap.find(RoomInfo.iRoomID) != m_RoomInfoMap.end())
		{
			CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::CreateRoom m_RoomInfoMap.find(RoomInfo.iRoomID) != m_RoomInfoMap.end()");
			return TEAMDUPLICATE_CREATE_FAIL;
		}

		m_RoomMap.insert(RoomMap::value_type(Room.iRoomID,Room));
		m_RoomInfoMap.insert(RoomInfoMap::value_type(RoomInfo.iRoomID,RoomInfo));
		m_UserRoomMap.insert(UserRoomMap::value_type(Room.dwUserID,Room.iRoomID));

		SM_IT smit=m_SceneMap.find(Room.iSceneID);
		if(smit != m_SceneMap.end())
		{
			(*smit).second.push_back(Room.iRoomID);
		}
		else
		{
			RoomIDList RIDList;
			RIDList.push_back(Room.iRoomID);
			m_SceneMap.insert(SceneMap::value_type(Room.iSceneID,RIDList));
		}

		return TEAMDUPLICATE_CREATE_SUC;
	}
	return TEAMDUPLICATE_CREATE_FAIL;
}

//加入房间
CT_INT32 CCTeamDuplicate::AddRoom(CT_INT32 iRoomID,CT_INT32 iFightingTime,UserFightingStatus &UserData)
{
	_AUTOMUTEX(&m_Mutex);
	UR_IT urit=m_UserRoomMap.find(UserData.dwUserID);
	if(urit != m_UserRoomMap.end())
	{
		return TEAMDUPLICATE_ADD_NOFREE;
	}

	RM_IT rmit=m_RoomMap.find(iRoomID);
	if(rmit != m_RoomMap.end())
	{
		RI_IT riit=m_RoomInfoMap.find(iRoomID);
		if(riit != m_RoomInfoMap.end())
		{
			//替代已匹配用户
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if((*riit).second.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_TRUE && 
					UserData.dwUserID == (*riit).second.FightingData[i].dwUserID)
				{
					(*riit).second.iFightingTime[i]=iFightingTime;
					(*riit).second.iStatus[i]=TEAMDUPLICATE_STATUS_FREE;
					(*riit).second.iAutoMatch[i]=TEAMDUPLICATE_AUTOMATCH_FALSE;
					memcpy(&((*riit).second.FightingData[i]),&UserData,sizeof(UserData));

					m_UserRoomMap.insert(UserRoomMap::value_type(UserData.dwUserID,iRoomID));
					return TEAMDUPLICATE_ADD_SUC;
				}
			}

			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if((*riit).second.FightingData[i].dwUserID == 0)
				{
					(*riit).second.iFightingTime[i]=iFightingTime;
					(*riit).second.iStatus[i]=TEAMDUPLICATE_STATUS_FREE;
					(*riit).second.iAutoMatch[i]=TEAMDUPLICATE_AUTOMATCH_FALSE;
					memcpy(&((*riit).second.FightingData[i]),&UserData,sizeof(UserData));

					m_UserRoomMap.insert(UserRoomMap::value_type(UserData.dwUserID,iRoomID));

					(*rmit).second.iCount++;
					return TEAMDUPLICATE_ADD_SUC;
				}
			}
		}
	}
	return TEAMDUPLICATE_ADD_FAIL;
}

//快速加入
CT_INT32 CCTeamDuplicate::AddFastRoom(CT_INT32 iFightingTime,UserFightingStatus &UserData,CT_INT32 iSceneID,CT_INT32 iDifficutly)
{
	_AUTOMUTEX(&m_Mutex);
	UR_IT urit=m_UserRoomMap.find(UserData.dwUserID);
	if(urit != m_UserRoomMap.end())
	{
		return TEAMDUPLICATE_FASTADD_NOFREE;
	}

	SM_IT smit=m_SceneMap.find(iSceneID);
	if(smit != m_SceneMap.end())
	{
		for(RID_IT it=(*smit).second.begin();it != (*smit).second.end();++it)
		{
			RI_IT riit=m_RoomInfoMap.find((*it));
			if(riit != m_RoomInfoMap.end())
			{
				if((*riit).second.iDifficulty == iDifficutly)
				{
					if(AddRoom((*it),iFightingTime,UserData) == TEAMDUPLICATE_ADD_SUC)
					{
						return TEAMDUPLICATE_FASTADD_SUC;
					}
				}
			}
		}
	}
	//else
	//{
	//	if(iSceneID-1 >= 0)
	//	{
	//		if(AddFastRoom(iFightingTime,UserData,iSceneID-1) == TEAMDUPLICATE_FASTADD_SUC)
	//			return TEAMDUPLICATE_FASTADD_SUC;
	//	}
	//}
	return TEAMDUPLICATE_FASTADD_FAIL;
}

//阵位调整
CT_INT32 CCTeamDuplicate::ChangePos(CT_INT32 iRoomID,CT_DWORD dwUserID,CT_DWORD dwPosID[TEAM_NUM])
{
	_AUTOMUTEX(&m_Mutex);
	UR_IT urit=m_UserRoomMap.find(dwUserID);
	if(urit == m_UserRoomMap.end())
	{
		return TEAMDUPLICATE_CHANGEPOS_FAIL;
	}

	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit != m_RoomInfoMap.end())
	{
		if((*riit).second.dwUserID == dwUserID)
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				for(CT_INT32 j=0;j<TEAM_NUM;++j)
				{
					if(dwPosID[i] == (*riit).second.FightingData[j].dwUserID)
					{
						LookUserInfo AutoMatchUser={0};
						UserFightingStatus UserData={0};
						CT_INT32 iFightingTime=0;
						CT_INT32 iStatus=0;
						CT_INT32 iAutoMatch=0;

						iFightingTime=(*riit).second.iFightingTime[i];
						(*riit).second.iFightingTime[i]=(*riit).second.iFightingTime[j];
						(*riit).second.iFightingTime[j]=iFightingTime;

						iStatus=(*riit).second.iStatus[i];
						(*riit).second.iStatus[i]=(*riit).second.iStatus[j];
						(*riit).second.iStatus[j]=iStatus;


						iAutoMatch=(*riit).second.iAutoMatch[i];
						(*riit).second.iAutoMatch[i]=(*riit).second.iAutoMatch[j];
						(*riit).second.iAutoMatch[j]=iAutoMatch;

						memcpy(&UserData,&((*riit).second.FightingData[i]),sizeof(UserData));
						memcpy(&((*riit).second.FightingData[i]),&((*riit).second.FightingData[j]),sizeof(UserData));
						memcpy(&((*riit).second.FightingData[j]),&UserData,sizeof(UserData));

						memcpy(&AutoMatchUser,&((*riit).second.AutoMatchUser[i]),sizeof(AutoMatchUser));
						memcpy(&((*riit).second.AutoMatchUser[i]),&((*riit).second.AutoMatchUser[j]),sizeof(AutoMatchUser));
						memcpy(&((*riit).second.AutoMatchUser[j]),&AutoMatchUser,sizeof(AutoMatchUser));
					}
				}
			}
			return TEAMDUPLICATE_CHANGEPOS_SUC;
		}
	}
	return TEAMDUPLICATE_CHANGEPOS_FAIL;
}

//房间信息调整
CT_INT32 CCTeamDuplicate::ChangeRoomInfo(CT_INT32 iRoomID,CT_DWORD dwUserID,CT_INT32 iSceneID,CT_INT32 iDifficutly)
{
	_AUTOMUTEX(&m_Mutex);
	UR_IT urit=m_UserRoomMap.find(dwUserID);
	if(urit == m_UserRoomMap.end())
	{
		return TEAMDUPLICATE_CHANGEROOM_FAIL;
	}

	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit != m_RoomInfoMap.end())
	{
		if((*riit).second.dwUserID == dwUserID)
		{
			(*riit).second.iSceneID=iSceneID;
			(*riit).second.iDifficulty=iDifficutly;

			RM_IT rmit=m_RoomMap.find(iRoomID);
			if(rmit != m_RoomMap.end())
			{
				(*rmit).second.iSceneID=iSceneID;
				(*rmit).second.iDifficulty=iDifficutly;
			}

			return TEAMDUPLICATE_CHANGEROOM_SUC;
		}
	}
	return TEAMDUPLICATE_CHANGEROOM_FAIL;
}

//玩家离开
CT_INT32 CCTeamDuplicate::LeaveRoom(CT_INT32 iRoomID,CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);
	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	UR_IT urit=m_UserRoomMap.find(dwUserID);  
	if(urit == m_UserRoomMap.end())
	{
		if(riit != m_RoomInfoMap.end())
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if((*riit).second.FightingData[i].dwUserID == dwUserID)
				{
					if((*riit).second.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
						return TEAMDUPLICATE_LEAVE_FAIL;
				}
			}
		}
		else
			return TEAMDUPLICATE_LEAVE_FAIL;
	}
	else
	{
		if((*urit).second == iRoomID)
			m_UserRoomMap.erase(urit);
	}

	//删除用户信息
	if(riit != m_RoomInfoMap.end())
	{
		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if((*riit).second.FightingData[i].dwUserID == dwUserID)
			{
				(*riit).second.iFightingTime[i]=0;
				(*riit).second.iStatus[i]=TEAMDUPLICATE_STATUS_FREE;
				(*riit).second.iAutoMatch[i]=0;
				memset(&((*riit).second.FightingData[i]),0,sizeof((*riit).second.FightingData[i]));
				memset(&((*riit).second.AutoMatchUser[i]),0,sizeof((*riit).second.AutoMatchUser[i]));

				RM_IT rmit=m_RoomMap.find((*riit).second.iRoomID);
				if(rmit != m_RoomMap.end())
				{
					(*rmit).second.iCount--;
				}
			}
		}

		//房主传位
		if(dwUserID == (*riit).second.dwUserID)
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if((*riit).second.FightingData[i].dwUserID != 0 && (*riit).second.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
				{
					if(dwUserID == (*riit).second.dwUserID)
					{
						(*riit).second.dwUserID=(*riit).second.FightingData[i].dwUserID;
						RM_IT rmit=m_RoomMap.find((*riit).second.iRoomID);
						if(rmit != m_RoomMap.end())
						{
							CombatUnitData data={0};
							G_GetMasterFightingData((*riit).second.FightingData[i],data);
							(*rmit).second.dwUserID=(*riit).second.FightingData[i].dwUserID;
							memcpy(((*rmit).second.cName),data.name,sizeof((*rmit).second.cName));
						}
					}
					return TEAMDUPLICATE_LEAVE_SUC;
				}
			}
		}
		else
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if((*riit).second.FightingData[i].dwUserID != 0)
				{
					return TEAMDUPLICATE_LEAVE_SUC;
				}
			}
		}


		//解散房间
		SM_IT smit=m_SceneMap.find((*riit).second.iSceneID);
		if(smit != m_SceneMap.end())
		{
			(*smit).second.remove(iRoomID);

			if((*smit).second.begin() == (*smit).second.end())
			{
				m_SceneMap.erase((*riit).second.iSceneID);
			}
		}

		m_RoomIDList.push_back(iRoomID);
		m_RoomMap.erase(iRoomID);
		m_RoomInfoMap.erase(riit);
		return TEAMDUPLICATE_LEAVE_SUC;
	}
	return TEAMDUPLICATE_LEAVE_FAIL;
}

//设置状态
CT_INT32 CCTeamDuplicate::SetUserStatus(CT_INT32 iRoomID,CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_Mutex);
	UR_IT urit=m_UserRoomMap.find(dwUserID);
	if(urit == m_UserRoomMap.end())
	{
		return TEAMDUPLICATE_STATUS_FAIL;
	}

	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit != m_RoomInfoMap.end())
	{
		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if((*riit).second.FightingData[i].dwUserID == dwUserID)
			{
				if((*riit).second.iStatus[i] == TEAMDUPLICATE_STATUS_READY)
					(*riit).second.iStatus[i]=TEAMDUPLICATE_STATUS_FREE;
				else
					(*riit).second.iStatus[i]=TEAMDUPLICATE_STATUS_READY;
			}
		}
		return TEAMDUPLICATE_STATUS_SUC;
	}
	return TEAMDUPLICATE_STATUS_FAIL;
}

//清空状态
CT_VOID CCTeamDuplicate::ClearUserStatus(CT_INT32 iRoomID)
{
	_AUTOMUTEX(&m_Mutex);
	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit != m_RoomInfoMap.end())
	{
		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if((*riit).second.FightingData[i].dwUserID != 0 && (*riit).second.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
			{
				(*riit).second.iStatus[i]=TEAMDUPLICATE_STATUS_FREE;
			}
		}
	}
	return;
}

//自动匹配
CT_INT32 CCTeamDuplicate::AutoMatch(CT_INT32 iRoomID,UserFightingStatus &FightingData,LookUserInfo &Userdata)
{
	_AUTOMUTEX(&m_Mutex);
	RM_IT rmit=m_RoomMap.find(iRoomID);
	if(rmit != m_RoomMap.end())
	{
		RI_IT riit=m_RoomInfoMap.find(iRoomID);
		if(riit != m_RoomInfoMap.end())
		{
			for(CT_INT32 i=0;i<TEAM_NUM;++i)
			{
				if((*riit).second.FightingData[i].dwUserID == 0 && FightingData.dwUserID != 0)
				{
					(*riit).second.iFightingTime[i]=0;
					(*riit).second.iStatus[i]=TEAMDUPLICATE_STATUS_READY;
					(*riit).second.iAutoMatch[i]=TEAMDUPLICATE_AUTOMATCH_TRUE;
					memcpy(&((*riit).second.FightingData[i]),&FightingData,sizeof(FightingData));
					memcpy(&((*riit).second.AutoMatchUser[i]),&Userdata,sizeof(Userdata));

					if((*rmit).second.iCount < TEAM_NUM)
						(*rmit).second.iCount++;;
					//m_UserRoomMap.insert(UserRoomMap::value_type(UserData.MasterInfo.dwUserID,iRoomID));
					return TEAMDUPLICATE_ADD_SUC;
				}
			}
		}
	}
	return TEAMDUPLICATE_ADD_FAIL;
}

//房间解散
CT_VOID CCTeamDuplicate::ClearRoom(CT_INT32 iRoomID)
{
	_AUTOMUTEX(&m_Mutex);
	RI_IT riit=m_RoomInfoMap.find(iRoomID);

	for(CT_INT32 i=0;i<TEAM_NUM;++i)
	{
		UR_IT urit=m_UserRoomMap.find((*riit).second.FightingData[i].dwUserID);
		if(urit != m_UserRoomMap.end())
		{
			m_UserRoomMap.erase(urit);
		}
	}

	m_RoomInfoMap.erase(iRoomID);
	m_RoomMap.erase(iRoomID);
	return;
}

//删除BOSS
CT_VOID CCTeamDuplicate::DeleteBoss(const CT_INT32 iRoomID)
{
	_AUTOMUTEX(&m_Mutex);
	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit == m_RoomInfoMap.end())
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::DeleteBoss No Find RoomID:%d",iRoomID);
		return ;
	}

	for(CT_INT32 i=0;i < TEAM_NUM;++i)
	{
		if((*riit).second.FightingData[i].dwUserID != 0)
		{
			for(CT_INT32 j=0;j < POST_COUNT/2;++j)
			{
				if(GET_TARGET_TYPE(UNIT_ALL_BOSS) <= GET_TARGET_TYPE((*riit).second.FightingData[i].units[j].type) && 
					(*riit).second.FightingData[i].units[j].type > UNIT_ALL_ARMY)
				{
					memset(&(*riit).second.FightingData[i].units[j],0,sizeof((*riit).second.FightingData[i].units[j]));
				}
			}
		}
	}
	return;
}

//向房间每个人发消息
CT_VOID CCTeamDuplicate::SendRoomMessage(CT_INT32 iRoomID,CT_INT32 iMainID,CT_INT32 iSubID,CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_Mutex);
	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit != m_RoomInfoMap.end())
	{
		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if((*riit).second.FightingData[i].dwUserID == 0 || 
				(*riit).second.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE)continue;
			CCUser *pUser=0;
			if(CCUserManager::getSingleton().Find((*riit).second.FightingData[i].dwUserID,&pUser))
			{
				pUser->GetNetInterface()->Send(iMainID,iSubID,pBuf,iLen);
			}
		}
	}
}

//向房间每个人发消息
CT_VOID CCTeamDuplicate::SendRoomMessage(CT_INT32 iRoomID,CT_INT32 iMainID,CT_INT32 iSubID)
{
	_AUTOMUTEX(&m_Mutex);
	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit != m_RoomInfoMap.end())
	{
		for(CT_INT32 i=0;i<TEAM_NUM;++i)
		{
			if((*riit).second.FightingData[i].dwUserID == 0 || 
				(*riit).second.iAutoMatch[i] != TEAMDUPLICATE_AUTOMATCH_FALSE)continue;
			CCUser *pUser=0;
			if(CCUserManager::getSingleton().Find((*riit).second.FightingData[i].dwUserID,&pUser))
			{
				pUser->GetNetInterface()->Send(iMainID,iSubID);
			}
		}
	}
}

//发送房间列表
CT_VOID CCTeamDuplicate::SendRoomList()
{
	_AUTOMUTEX(&m_Mutex);
	CT_CHAR cBuf[MAX_BUFFER]={0};
	CT_INT32 iLen=MAX_BUFFER;
	GetRoomList(cBuf,iLen);

	for(RI_IT it=m_RoomInfoMap.begin();it != m_RoomInfoMap.end();++it)
	{
		for(CT_INT32 i=0;i < TEAM_NUM;++i)
		{
			CCUser *pUser=0;
			CT_DWORD dwUserID=(*it).second.FightingData[i].dwUserID;
			if(dwUserID != 0 && (*it).second.iAutoMatch[i] == TEAMDUPLICATE_AUTOMATCH_FALSE)
			{
				if(CCUserManager::getSingleton().Find(dwUserID,&pUser))
				{
					pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_ROOMLIST,cBuf,iLen);
				}
			}
		}
	}
	return;
}

//获取房间信息 任何人不得改动这个函数的实现（死锁相关） 彭文
CT_BOOL CCTeamDuplicate::GetRoomInfo(CT_INT32 iRoomID,TeamDuplicateRoomInfo &RoomInfo)
{
	_AUTOMUTEX(&m_Mutex);
	RI_IT riit=m_RoomInfoMap.find(iRoomID);
	if(riit == m_RoomInfoMap.end())
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::GetRoomInfo No Find RoomID:%d",iRoomID);
		return CT_FALSE;
	}

	memcpy(&RoomInfo,&((*riit).second),sizeof(RoomInfo));
	return CT_TRUE;
}

//获取房间信息    任何人不得改动这个函数的实现（死锁相关） 彭文
CT_BOOL CCTeamDuplicate::GetRoomInfo(CT_DWORD dwUserID,TeamDuplicateRoomInfo &RoomInfo)
{
	_AUTOMUTEX(&m_Mutex);
	UR_IT urit=m_UserRoomMap.find(dwUserID);
	if(urit == m_UserRoomMap.end())
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::GetRoomInfo No Find UserID:%d",dwUserID);
		return CT_FALSE;
	}

	RI_IT riit=m_RoomInfoMap.find((*urit).second);
	if(riit == m_RoomInfoMap.end())
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::GetRoomInfo No Find RoomID:%d",(*urit).second);
		return CT_FALSE;
	}

	memcpy(&RoomInfo,&((*riit).second),sizeof(RoomInfo));
	return CT_TRUE;
}

//获取房间列表
CT_VOID CCTeamDuplicate::GetRoomList(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_Mutex);
	if((CT_DWORD)iLen < sizeof(TeamDuplicateRoom)*m_RoomMap.size())
	{
		CCLogMeg::getSingleton().WriteError("CCTeamDuplicate::GetRoomList len < sizeof(TeamDuplicateRoom)*m_RoomMap.size()");
		return;
	}

	CT_INT32 iSize=0;
#if 0 //客户端显示问题，暂时去掉房间列表显示
	RM_IT rmit=m_RoomMap.end();
	for(RM_IT it=m_RoomMap.begin();it != m_RoomMap.end();++it)
	{
		memcpy(pBuf+iSize,&((*it).second),sizeof(TeamDuplicateRoom));
		iSize+=sizeof(TeamDuplicateRoom);
	}
#endif

	iLen=iSize;
	return;
}