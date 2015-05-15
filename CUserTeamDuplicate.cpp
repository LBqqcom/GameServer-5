#include "CUserTeamDuplicate.h"
#include "CLogMeg.h"
#include "CMutex.h"
#include "CGameConfig.h"
#include "CDatabaseMes.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"

CCUserTeamDuplicate::CCUserTeamDuplicate(void)
{
	m_iFightingTime=0;
	m_TeamDuplicateMap.clear();
}

CCUserTeamDuplicate::~CCUserTeamDuplicate(void)
{
	m_TeamDuplicateMap.clear();
}

//初始化
CT_VOID CCUserTeamDuplicate::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser=pUser;
	m_pMutex=pMutex;
	m_iFightingTime=0;
	m_TeamDuplicateMap.clear();
	return;
}

//清理用户数据
CT_VOID CCUserTeamDuplicate::Clear()
{
	 _AUTOMUTEX(m_pMutex);
	m_iFightingTime=0;
	m_TeamDuplicateMap.clear();
	return;
}

//插入数据
CT_VOID CCUserTeamDuplicate::InsertSceneData(const CT_INT32 iSceneID,const CT_INT32 iDifficulty,CT_INT32 iWin)
{
	 _AUTOMUTEX(m_pMutex);
	 if(iDifficulty >= TEAMDUPLICATE_DIFFICULTY)
	 {
		 CCLogMeg::getSingleton().WriteError("CCUserTeamDuplicate::InsertSceneData iDifficulty >= TEAMDUPLICATE_DIFFICULTY");
		 return;
	 }

	TeamDuplicateScene Scene={0};
	Scene.iDifficulty[iDifficulty]=CT_TRUE;
	Scene.iSceneID=iSceneID;
	Scene.iWin[iDifficulty]=iWin;

	TD_IT it=m_TeamDuplicateMap.find(iSceneID);
	if(it != m_TeamDuplicateMap.end())
	{
		(*it).second.iDifficulty[iDifficulty]=CT_TRUE;
		(*it).second.iWin[iDifficulty]=iWin;
	}
	else
	{
		m_TeamDuplicateMap.insert(TeamDuplicateMap::value_type(iSceneID,Scene));
	}
	return;
}

//设置场景
CT_BOOL CCUserTeamDuplicate::SetSceneData(CT_INT32 iSceneID,CT_INT32 iDifficulty)
{
	_AUTOMUTEX(m_pMutex);

	if(iDifficulty >= TEAMDUPLICATE_DIFFICULTY)
	{
		CCLogMeg::getSingleton().WriteError("CCUserTeamDuplicate::SetSceneData iDifficulty >= TEAMDUPLICATE_DIFFICULTY");
		return CT_FALSE;
	}

	TD_IT it=m_TeamDuplicateMap.find(iSceneID);
	if(it != m_TeamDuplicateMap.end())
	{
		if(!(*it).second.iDifficulty[iDifficulty])
		{
			(*it).second.iDifficulty[iDifficulty]=CT_TRUE;
			return CT_TRUE;
		}
		else
		{
			return CT_FALSE;
		}
	}
	else
	{
		TeamDuplicateScene SceneInfo={0};
		SceneInfo.iSceneID=iSceneID;
		SceneInfo.iDifficulty[0]=CT_TRUE;
		SceneInfo.iDifficulty[1]=CT_FALSE;
		m_TeamDuplicateMap.insert(TeamDuplicateMap::value_type(iSceneID,SceneInfo));
		return CT_TRUE;
	}
	return CT_FALSE;
}

//获取场景信息
CT_VOID CCUserTeamDuplicate::GetSceneData(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);
	if( (CT_DWORD)iLen < m_TeamDuplicateMap.size()*sizeof(TeamDuplicateScene))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTeamDuplicate::GetSceneData iLen < m_TeamDuplicateMap.size()*sizeof(TeamDuplicateScene)");
		iLen=0;
		return;
	}

	CT_INT32 iSize=0;
	for(TD_IT it =m_TeamDuplicateMap.begin();it != m_TeamDuplicateMap.end();++it)
	{
		memcpy(pBuf+iSize,&((*it).second),sizeof(TeamDuplicateScene));
		iSize+=sizeof(TeamDuplicateScene);
	}

	iLen=iSize;
	return;
}

//验证关卡
CT_BOOL CCUserTeamDuplicate::VaildateScene(CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_BOOL bUpdata)
{
	_AUTOMUTEX(m_pMutex);

	if(iDifficulty >= TEAMDUPLICATE_DIFFICULTY)
	{
		CCLogMeg::getSingleton().WriteError("CCUserTeamDuplicate::VaildateScene iDifficulty >= TEAMDUPLICATE_DIFFICULTY");
		return CT_FALSE;
	}

	TD_IT it=m_TeamDuplicateMap.find(iSceneID);
	if(it != m_TeamDuplicateMap.end())
	{
		if(!bUpdata)
		{
			if((*it).second.iDifficulty[iDifficulty])
			{
				return CT_TRUE;
			}
			else
			{
				return CT_FALSE;
			}
		}
		else
		{
			if((*it).second.iWin[iDifficulty] == WINER_LIFT)
			{
				return CT_TRUE;
			}
			else
			{
				return CT_FALSE;
			}
		}
	}
	else
	{
		if(iSceneID == 0 && iDifficulty == 0 && bUpdata)
			return CT_TRUE;
	}

	return CT_FALSE;
}

//更新场景信息
CT_VOID CCUserTeamDuplicate::UpdataScene(CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iWin)
{
	_AUTOMUTEX(m_pMutex);

	if(iDifficulty >= TEAMDUPLICATE_DIFFICULTY)
	{
		CCLogMeg::getSingleton().WriteError("CCUserTeamDuplicate::UpdataScene iDifficulty >= TEAMDUPLICATE_DIFFICULTY");
		return;
	}

	TD_IT it=m_TeamDuplicateMap.find(iSceneID);
	if(it != m_TeamDuplicateMap.end())
	{
		(*it).second.iWin[iDifficulty]=iWin;

		//写入数据库
		MSC_DB_TeamDuplicateScene Scene={0};
		Scene.dwUserID=m_pUser->GetUserID();
		Scene.iScnenID=iSceneID;
		Scene.iDifficulty=iDifficulty;
		Scene.iWin=iWin;
		CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_TEAMDUPLICATE_SCENE,(CT_CHAR*)&Scene,sizeof(Scene));

	}
	return;
}

//更新奖励次数
CT_VOID CCUserTeamDuplicate::UpdataFightingReward()
{
	_AUTOMUTEX(m_pMutex);

	CMD_G_TeamDuplicateUpdata Updata={0};

#if 0
	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_TEAMDUPLICATE_FIGHTINGNUM,&vr))
	{
		CCLogMeg::getSingleton().WriteError("CCUserTeamDuplicate::UpdataFightingReward G_GetValueRange VALUE_TYPE_TEAMDUPLICATE_FIGHTINGNUM");
		return;
	}
#endif

	Updata.dwUserID=m_pUser->GetUserID();
	Updata.iFightingTimes=0;

	m_pUser->GetNetInterface()->Send(MAIN_G_TEAM_DUPLICATE,SUB_G_TEAMDUPLICATE_FIGHTINGTIMES,(CT_CHAR*)&Updata,sizeof(Updata));

	m_pUser->m_userBase.UpdataTeamFightingTimes();
	return;
}
