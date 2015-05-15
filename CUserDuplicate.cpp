#include "CUserDuplicate.h"
#include "CDatabaseMes.h"
#include "CLogMeg.h"
#include "CFightingScriptSetting.h"
#include "CNetInterface.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CCompress.h"

#define MAX_BUFFER			1024*5
CCUserDuplicate1::CCUserDuplicate1(void)
{
	m_iBuyFightingTime=1;
	m_RewardMap.clear();
	m_SceneMap.clear();
	m_RaidsRewardList.clear();

	memset(&m_RaidsInfo,0,sizeof(m_RaidsInfo));
}

CCUserDuplicate1::~CCUserDuplicate1(void)
{
	m_RewardMap.clear();
	m_SceneMap.clear();
	m_RaidsRewardList.clear();
}

//初始化
CT_VOID CCUserDuplicate1::Init(CCUser* pUser, CCMutex* pMutex)
{
	m_pUser  = pUser;
	m_pMutex = pMutex;

	m_RewardMap.clear();
	m_SceneMap.clear();
	m_RaidsRewardList.clear();

	memset(&m_RaidsInfo,0,sizeof(m_RaidsInfo));

}

//清理用户数据
CT_VOID CCUserDuplicate1::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_RewardMap.clear();
	m_SceneMap.clear();
	m_RaidsRewardList.clear();

	memset(&m_RaidsInfo,0,sizeof(m_RaidsInfo));
}

//插入场景
CT_VOID CCUserDuplicate1::InsterScene(SceneInfo &Sceneinfo)
{
	_AUTOMUTEX(m_pMutex);

	WM_IT wmit=m_SceneMap.find(Sceneinfo.iWorldSceneID);
	if(wmit != m_SceneMap.end())
	{
		SM_IT smit=(*wmit).second.find(Sceneinfo.iSceneID);
		if(smit != (*wmit).second.end())
		{
			(*smit).second.insert(DifficyltyMap::value_type(Sceneinfo.iDifficulty,Sceneinfo));
		}
		else
		{
			DifficyltyMap dm;
			dm.insert(DifficyltyMap::value_type(Sceneinfo.iDifficulty,Sceneinfo));
			(*wmit).second.insert(SceneMap::value_type(Sceneinfo.iSceneID,dm));
		}
	}
	else
	{
		SceneMap sm;
		DifficyltyMap dm;
		dm.insert(DifficyltyMap::value_type(Sceneinfo.iDifficulty,Sceneinfo));
		sm.insert(SceneMap::value_type(Sceneinfo.iSceneID,dm));
		m_SceneMap.insert(WorldMap::value_type(Sceneinfo.iWorldSceneID,sm));
	}

	return;
}

//插入奖励
CT_VOID CCUserDuplicate1::InsterReward(CT_INT32 iWorldID,CT_INT32 iCount)
{
	_AUTOMUTEX(m_pMutex);

	RD_IT rdit=m_RewardMap.find(iWorldID);
	if(rdit != m_RewardMap.end())
	{
		if((*rdit).second.iRewardID < iCount)
			(*rdit).second.iRewardID=iCount;
	}
	else
	{
		WorldReward wr={0};
		wr.iWorldID=iWorldID;
		wr.iRewardID=iCount;
		m_RewardMap.insert(RewardMap::value_type(iWorldID,wr));
	}
	return;
}

//插入扫荡信息
CT_VOID CCUserDuplicate1::InsterRaidsInfo(RaidsInfo &Raidsinfo)
{
	_AUTOMUTEX(m_pMutex); 
	memcpy(&m_RaidsInfo,&Raidsinfo,sizeof(Raidsinfo));
	return;
}

//插入扫荡记录
CT_VOID  CCUserDuplicate1::InsertRaidsReward(const CMD_G_RaidsReward RaidsReward)
{
	_AUTOMUTEX(m_pMutex); 

	G_ValueRange vrtime={0};
	if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vrtime))
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::InsertRaidsReward G_GetValueRange VALUE_TYPE_RAIDS_TIME");
		return;
	}

	//if(m_RaidsInfo.lBeginTime-vrtime.fMaxVal*m_RaidsInfo.iCurrentTime <= RaidsReward.iCurrentTime)
		m_RaidsRewardList.push_back(RaidsReward);
	return;
}

//设置场景信息
CT_VOID CCUserDuplicate1::SetSceneInfo(SceneInfo &Sceneinfo)
{
	_AUTOMUTEX(m_pMutex);

	WM_IT wmit=m_SceneMap.find(Sceneinfo.iWorldSceneID);
	if(wmit != m_SceneMap.end())
	{
		SM_IT smit=(*wmit).second.find(Sceneinfo.iSceneID);
		if(smit != (*wmit).second.end())
		{
			DM_IT dmit=(*smit).second.find(Sceneinfo.iDifficulty);
			if(dmit != (*smit).second.end())
			{
				memcpy(&((*dmit).second),&Sceneinfo,sizeof(Sceneinfo));
			}
			else
			{
				CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::SetSceneInfo dmit == (*smit).second.end()");
			}
		}
		else
		{
			CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::SetSceneInfo smit == (*wmit).second.end()");
		}
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::SetSceneInfo wmit != m_SceneMap.end()");
	}
	return;
}

//获取场景信息
CT_BOOL CCUserDuplicate1::GetSceneInfo(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iSize=0;
	CMD_G_SceneInfo si={0};

	WM_IT wmend=m_SceneMap.end();
	for(WM_IT wmit=m_SceneMap.begin();wmit != wmend;++wmit)
	{
		SM_IT smend=(*wmit).second.end();
		for(SM_IT smit=(*wmit).second.begin();smend != smit;++smit)
		{
			iSize+=(*smit).second.size()*sizeof(SceneInfo);
		}
	}

	RD_IT rdend=m_RewardMap.end();
	for(RD_IT rdit=m_RewardMap.begin();rdit != rdend;++rdit)
	{
		memcpy(pBuf+sizeof(si)+iSize,&((*rdit).second),sizeof((*rdit).second));
		iSize+=sizeof((*rdit).second);
	}

	if(iLen < iSize+sizeof(m_iBuyFightingTime))
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::GetSceneInfo iLen < iSize");
		return CT_FALSE;
	}


	iSize=0;
	memcpy(pBuf+sizeof(si),&m_iBuyFightingTime,sizeof(m_iBuyFightingTime));
	iSize+=sizeof(m_iBuyFightingTime);
	si.iBuyFightingSize=sizeof(m_iBuyFightingTime);

	rdend=m_RewardMap.end();
	for(RD_IT rdit=m_RewardMap.begin();rdit != rdend;++rdit)
	{
		memcpy(pBuf+sizeof(si)+iSize,&((*rdit).second),sizeof((*rdit).second));
		iSize+=sizeof((*rdit).second);
		si.iRewardSize+=sizeof((*rdit).second);
	}

	wmend=m_SceneMap.end();
	for(WM_IT wmit=m_SceneMap.begin();wmit != wmend;++wmit)
	{
		SM_IT smend=(*wmit).second.end();
		for(SM_IT smit=(*wmit).second.begin();smend != smit;++smit)
		{
			DM_IT dmend=(*smit).second.end();
			for(DM_IT dmit=(*smit).second.begin();dmit != dmend;++dmit)
			{
				memcpy(pBuf+sizeof(si)+iSize,&((*dmit).second),sizeof((*dmit).second));
				iSize+=sizeof((*dmit).second);
				si.iSceneSize+=sizeof((*dmit).second);
			}
		}
	}

	memcpy(pBuf,&si,sizeof(si));
	iLen=iSize;
	return CT_TRUE;
}

CT_BOOL CCUserDuplicate1::GetSceneInfo(CT_INT32 iWorldID,CT_INT32 iSceneID,CT_INT32 iDifficultyID,SceneInfo &Sceneinfo)
{
	_AUTOMUTEX(m_pMutex);
	WM_IT wmit=m_SceneMap.find(iWorldID);
	if(wmit != m_SceneMap.end())
	{
		SM_IT smit=(*wmit).second.find(iSceneID);
		if(smit != (*wmit).second.end())
		{
			DM_IT dmit=(*smit).second.find(iDifficultyID);
			if(dmit != (*smit).second.end())
			{
				memcpy(&Sceneinfo,&((*dmit).second),sizeof(Sceneinfo));
				return CT_TRUE;
			}
		}
	}
	return CT_FALSE;
}

CT_BOOL CCUserDuplicate1::GetUserPassedDuplicate(CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iLevel)
{
	if (iWorldScene>WORLD_SCENE_COUNT||iScene>SCENE_COUNT)
	{
		return CT_FALSE;
	}

	SceneInfo si={0};
	if(GetSceneInfo(iWorldScene,iScene,0,si))
	{
		if (si.iSceneStar>=iLevel)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//获取场景分数
CT_INT32 CCUserDuplicate1::GetSceneScore(CT_INT32 iWorldID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iScore=0;
	WM_IT wmit=m_SceneMap.find(iWorldID);
	if(wmit != m_SceneMap.end())
	{
		SM_IT smend=(*wmit).second.end();
		for(SM_IT smit=(*wmit).second.begin();smit != smend;++smit)
		{
			DM_IT dmend=(*smit).second.end();
			for(DM_IT dmit=(*smit).second.begin();dmit != dmend;++dmit)
			{
				iScore+=(*dmit).second.iSceneStar;
			}
		}
	}
	return iScore;
}

//获取奖励
CT_INT32 CCUserDuplicate1::GetWorldReward(const CT_INT32 iWorldID,const CT_INT32 iRewardID)
{
	_AUTOMUTEX(m_pMutex);

	RD_IT rdit=m_RewardMap.find(iWorldID);
	if(rdit != m_RewardMap.end())
	{
		if((*rdit).second.iRewardID >= iRewardID)
			return REWARD_CLOSE;

		CT_INT32 iSceneStar=GetSceneScore(iWorldID);

		//读取可获奖励
		G_WorldSceneReward wr={0};
		if(CCFightingScriptSetting::getSingleton().GetWorldSceneData(iWorldID,iRewardID,wr))
		{
			if(iSceneStar >= wr.iScore)
			{
				return GET_REWARD;
			}
		}
	}
	return REWARD_CLOSE;
}

//增加购买次数
CT_VOID CCUserDuplicate1::AddBuyFightingTime()
{
	_AUTOMUTEX(m_pMutex);

	m_iBuyFightingTime++;

	MSC_DB_BuyFightingTime BuyFightingTime={0};
	BuyFightingTime.dwUserID=m_pUser->GetUserID();
	BuyFightingTime.iBuyFightingTime=m_iBuyFightingTime;

	CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_W_BUYFIGHTINGTIME,(CT_CHAR*)&BuyFightingTime,sizeof(BuyFightingTime));

	return;
}

//增加战斗次数
CT_VOID CCUserDuplicate1::AddFightingNum(SceneInfo &Sceneinfo)
{
	_AUTOMUTEX(m_pMutex);

	SceneInfo si={0};
	if(!GetSceneInfo(Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID,Sceneinfo.iDifficulty,si))
	{
		si.iWorldSceneID=Sceneinfo.iWorldSceneID;
		si.iSceneID=Sceneinfo.iSceneID;
		si.iDifficulty=Sceneinfo.iDifficulty;
		si.iFightingNum++;
		SetSceneInfo(si);

		//更新数据库
		MSC_DB_SceneInfo Scene={0};
		Scene.dwUserID=m_pUser->GetUserID();
		Scene.iSceneID=si.iSceneID;
		Scene.iWorldSceneID=si.iWorldSceneID;
		Scene.iDifficulty=si.iDifficulty;
		Scene.iSceneStar=si.iSceneStar;
		Scene.iFightingNum=si.iFightingNum;

		CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_R_DUPLICATE_SCENE_INFO,(CT_CHAR*)&Scene,sizeof(Scene));
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::AddFightingNum GetSceneInfo w:%d s:%d d:%d",Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID,Sceneinfo.iDifficulty);
	}
	return;
}

//更新场景信息
CT_VOID CCUserDuplicate1::UpdataSceneInfo(SceneInfo &Sceneinfo)
{
	_AUTOMUTEX(m_pMutex);

	if(Sceneinfo.iSceneStar <= 0)return;

	CT_INT32 iLen=0;
	CT_CHAR cBuf[MAX_BUFFER]={0};
	G_WorldScene world={0};
	if(!CCFightingScriptSetting::getSingleton().GetWorldScene(Sceneinfo.iWorldSceneID,world))
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::UpdataSceneInfo GetWorldScene iWorldSceneID:%d",Sceneinfo.iWorldSceneID);
		return;
	}

	//难度开启
	SceneInfo CurrSi={0};
	GetSceneInfo(Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID,0,CurrSi);
	for(CT_INT32 i=Sceneinfo.iDifficulty;i<DIFFICULTY;++i)
	{
		G_FightingScene fs={0};
		if(CCFightingScriptSetting::getSingleton().GetFightingScene(Sceneinfo.iWorldSceneID,Sceneinfo.iSceneID,i+1,fs))
		{
			SceneInfo si={0};
			GetSceneInfo(Sceneinfo.iWorldSceneID,fs.iOpenCondition-1,0,si);
			if(CurrSi.iSceneStar == 0)
			{
				//开启新难度
				si.iWorldSceneID=Sceneinfo.iWorldSceneID;
				si.iSceneID=Sceneinfo.iSceneID;
				si.iDifficulty=i+1;
				si.iSceneStar=0;
				si.iFightingNum=0;
				InsterScene(si);

				//更新数据库
				MSC_DB_SceneInfo Scene={0};
				Scene.dwUserID=m_pUser->GetUserID();
				Scene.iSceneID=si.iSceneID;
				Scene.iWorldSceneID=si.iWorldSceneID;
				Scene.iDifficulty=si.iDifficulty;
				Scene.iSceneStar=si.iSceneStar;
				Scene.iFightingNum=si.iFightingNum;

				CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_R_DUPLICATE_SCENE_INFO,(CT_CHAR*)&Scene,sizeof(Scene));

				
				if(iLen+sizeof(si) < MAX_BUFFER)
				{
					memcpy(cBuf+iLen,&si,sizeof(si));
					iLen+=sizeof(si);
				}
				else
					CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::UpdataSceneInfo iLen+sizeof(si) >= MAX_BUFFER");
			}
		}
	}

	//场景开启
	for(CT_INT32 i=Sceneinfo.iSceneID;i<world.iSceneNum;++i)
	{
		G_FightingScene fs={0};
		if(CCFightingScriptSetting::getSingleton().GetFightingScene(Sceneinfo.iWorldSceneID,i,0,fs))
		{
			SceneInfo si={0};
			GetSceneInfo(Sceneinfo.iWorldSceneID,fs.iOpenCondition-1,0,si);
			if(fs.iOpenCondition-1 == Sceneinfo.iSceneID && CurrSi.iSceneStar == 0)
			{
				si.iWorldSceneID=Sceneinfo.iWorldSceneID;
				si.iSceneID=i;
				si.iDifficulty=0;
				si.iSceneStar=0;
				si.iFightingNum=0;
				InsterScene(si);

				//更新数据库
				MSC_DB_SceneInfo Scene={0};
				Scene.dwUserID=m_pUser->GetUserID();
				Scene.iSceneID=si.iSceneID;
				Scene.iWorldSceneID=si.iWorldSceneID;
				Scene.iDifficulty=si.iDifficulty;
				Scene.iSceneStar=0;
				Scene.iFightingNum=si.iFightingNum;

				CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_R_DUPLICATE_SCENE_INFO,(CT_CHAR*)&Scene,sizeof(Scene));

				memcpy(cBuf+iLen,&si,sizeof(si));

				if(iLen+sizeof(si) < MAX_BUFFER)
					iLen+=sizeof(si);
				else
					CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::UpdataSceneInfo iLen+sizeof(si) >= MAX_BUFFER");
			}
		}
	}

	//世界场景开启
	for(CT_INT32 i=Sceneinfo.iWorldSceneID+1;i<WORLD_SCENE_COUNT;++i)
	{
		memset(&world,0,sizeof(world));
		if(CCFightingScriptSetting::getSingleton().GetWorldScene(i,world))
		{
			SceneInfo si={0};
			GetSceneInfo(world.iWorldCondition-1,world.iSubCondition-1,0,si);
			if(si.iSceneStar == 0 && world.iSubCondition-1 == Sceneinfo.iSceneID && world.iWorldCondition-1 == Sceneinfo.iWorldSceneID)
			{
				si.iWorldSceneID=i;
				si.iSceneID=0;
				si.iDifficulty=0;
				si.iSceneStar=0;
				si.iFightingNum=0;
				InsterScene(si);

				//更新数据库
				MSC_DB_SceneInfo Scene={0};
				Scene.dwUserID=m_pUser->GetUserID();
				Scene.iSceneID=si.iSceneID;
				Scene.iWorldSceneID=si.iWorldSceneID;
				Scene.iDifficulty=si.iDifficulty;
				Scene.iSceneStar=0;
				Scene.iFightingNum=si.iFightingNum;

				CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_R_DUPLICATE_SCENE_INFO,(CT_CHAR*)&Scene,sizeof(Scene));

				//开启世界奖励
				MSC_DB_WorldSceneInfo WorldSceneInfo={0};
				WorldSceneInfo.dwUserID=m_pUser->GetUserID();
				WorldSceneInfo.iSceneID=si.iWorldSceneID;
				WorldSceneInfo.iReward=-1;

				threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_R_DUPLICATE_WORLD_SCENE,(CT_CHAR*)&WorldSceneInfo,sizeof(WorldSceneInfo));

				WorldReward wr={0};
				wr.iWorldID=si.iWorldSceneID;
				wr.iRewardID=-1;
				m_RewardMap.insert(RewardMap::value_type(si.iWorldSceneID,wr));

				memcpy(cBuf+iLen,&si,sizeof(si));
				if(iLen+sizeof(si) < MAX_BUFFER)
					iLen+=sizeof(si);
				else
					CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::UpdataSceneInfo iLen+sizeof(si) >= MAX_BUFFER");
			}
		}
	}

	//更新已有难度
	if(CurrSi.iSceneStar < Sceneinfo.iSceneStar || Sceneinfo.iFightingNum > 0)
	{
		CurrSi.iSceneStar=(Sceneinfo.iSceneStar > CurrSi.iSceneStar?Sceneinfo.iSceneStar:CurrSi.iSceneStar);
		CurrSi.iFightingNum=Sceneinfo.iFightingNum;
		CurrSi.iWorldSceneID=Sceneinfo.iWorldSceneID;
		CurrSi.iSceneID=Sceneinfo.iSceneID;
		CurrSi.iDifficulty=Sceneinfo.iDifficulty;
		SetSceneInfo(CurrSi);

		//更新数据库
		MSC_DB_SceneInfo Scene={0};
		Scene.dwUserID=m_pUser->GetUserID();
		Scene.iSceneID=CurrSi.iSceneID;
		Scene.iWorldSceneID=CurrSi.iWorldSceneID;
		Scene.iDifficulty=CurrSi.iDifficulty;
		Scene.iSceneStar=CurrSi.iSceneStar;
		Scene.iFightingNum=CurrSi.iFightingNum;

		CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DUPLICATE_EVENT,MSC_SUB_DB_R_DUPLICATE_SCENE_INFO,(CT_CHAR*)&Scene,sizeof(Scene));

		memcpy(cBuf+iLen,&CurrSi,sizeof(CurrSi));
		if(iLen+sizeof(CurrSi) < MAX_BUFFER)
			iLen+=sizeof(CurrSi);
		else
			CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::UpdataSceneInfo iLen+sizeof(CurrSi) >= MAX_BUFFER");
	}


	if(iLen != 0)
		m_pUser->GetNetInterface()->Send(MAIN_G_DUPLICATE_EVENT,SUB_G_SCENE_INFO,cBuf,iLen);
	return;
}

//刷新副本
CT_VOID CCUserDuplicate1::UpdataDuplicate()
{
	_AUTOMUTEX(m_pMutex);
	WM_IT wmend=m_SceneMap.end();
	for(WM_IT wmit=m_SceneMap.begin();wmit != wmend;++wmit)
	{
		SM_IT smend=(*wmit).second.end();
		for(SM_IT smit=(*wmit).second.begin();smit != smend;++smit)
		{
			DM_IT dmend=(*smit).second.end();
			for(DM_IT dmit=(*smit).second.begin();dmit != dmend;++dmit)
			{
				(*dmit).second.iFightingNum=0;
			}
		}
	}

	//副本信息
	CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
	CT_INT32 iLen = MAX_MES_MAX_BUFF_LEN;
	if(GetSceneInfo(cBuf,iLen))
	{
		CT_INT32 iOutLen=MAX_MES_MAX_BUFF_LEN;

		CMD_G_SceneInfo si={0};
		memcpy(&si,cBuf,sizeof(si));
		if(CCCompress::getSingleton().Compress(cBuf+sizeof(si),iLen,cBuf+sizeof(si),iOutLen))
		{
			si.iCompress=CT_TRUE;
			memcpy(cBuf,&si,sizeof(si));
			m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV, SUB_G_SCENE_INFO, cBuf, iOutLen+sizeof(si));
		}
		else
		{
			si.iCompress=CT_FALSE;
			memcpy(cBuf,&si,sizeof(si));
			m_pUser->GetNetInterface()->Send(MAIN_G_ENTER_ENV, SUB_G_SCENE_INFO, cBuf, iLen+sizeof(si));
		}
	}
	return;
}

//更新奖励
CT_VOID CCUserDuplicate1::UpdataSceneReward(const CT_INT32 iWorldID,const CT_INT32 iRewardID)
{
	_AUTOMUTEX(m_pMutex);

	RD_IT rdit=m_RewardMap.find(iWorldID);
	if(rdit != m_RewardMap.end())
	{
		(*rdit).second.iRewardID=iRewardID;
	}
	return;
}

//设置扫荡信息
CT_INT32 CCUserDuplicate1::SetRaidsInfo(RaidsInfo &Raidsinfo)
{
	_AUTOMUTEX(m_pMutex);
	if(m_RaidsInfo.iAllTime == 0)
		memcpy(&m_RaidsInfo,&Raidsinfo,sizeof(Raidsinfo));
	else
		return RAIDS_ERROR;
	return 0;
}

//获取扫荡信息
CT_VOID CCUserDuplicate1::GetRaidsInfo(RaidsInfo &Raidsinfo)
{
	_AUTOMUTEX(m_pMutex);
	memcpy(&Raidsinfo,&m_RaidsInfo,sizeof(Raidsinfo));
	return;
}

//更新扫荡
CT_INT32 CCUserDuplicate1::UpdataRaids()
{
	_AUTOMUTEX(m_pMutex);
	//扫荡中
	m_RaidsInfo.iCurrentTime++;
	if(m_RaidsInfo.iAllTime-m_RaidsInfo.iCurrentTime <= 0)
	{
		//扫荡完成
		m_RaidsInfo.lBeginTime=0;
		return 0;
	}
	else
	{
		G_ValueRange vr={0};
		if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vr))
		{
			CCLogMeg::getSingleton().WriteError("CCUserDuplicate1::UpdataRaids VALUE_TYPE_RAIDS_TIME");
			return RAIDS_DATA;
		}

		time_t t=time(0);
		if(m_RaidsInfo.lBeginTime+vr.fMaxVal > t)
			return RAIDS_TIME;

		m_RaidsInfo.lBeginTime=t;
	}
	return 0;
}

//清除扫荡信息
CT_VOID CCUserDuplicate1::ClearRaidsInfo()
{
	_AUTOMUTEX(m_pMutex);
	memset(&m_RaidsInfo,0,sizeof(m_RaidsInfo));
	m_RaidsRewardList.clear();

	MSC_DB_RaidsInfo dbRaidsinfo={0};

	dbRaidsinfo.dwUserID=m_pUser->GetUserID();
	dbRaidsinfo.iWorldID=0;
	dbRaidsinfo.iSceneID=0;
	dbRaidsinfo.iDifficulty=0;
	dbRaidsinfo.iAllTime=0;
	dbRaidsinfo.iCurrentTime=0;
	dbRaidsinfo.lBeginTime=0;
	CT_DWORD threadid = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid, MSC_MAIN_DUPLICATE_EVENT, MSC_SUB_DB_RAIDS_INFO, (CT_CHAR*)&dbRaidsinfo, sizeof(dbRaidsinfo));

	return;
}

//获取扫荡时间
CT_INT64 CCUserDuplicate1::GetLastTime(time_t t)
{
	_AUTOMUTEX(m_pMutex);

	G_ValueRange vr={0};
	if(!G_GetValueRange(VALUE_TYPE_RAIDS_TIME,&vr))
	{
		return 0;
	}

	if(m_RaidsInfo.lBeginTime == 0)return 0;

	CT_INT64 lEndTime=m_RaidsInfo.lBeginTime+vr.fMaxVal;
	CT_INT64 lTime=lEndTime-(t == 0?time(0):t);
	return (lTime < 0?0:lTime);
}

//插入奖励
CT_BOOL CCUserDuplicate1::InsertReward()
{
	_AUTOMUTEX(m_pMutex);

	if(m_RaidsInfo.lBeginTime == 0 && m_RaidsInfo.iAllTime == 0)
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate::GetRaidsReward m_RaidsInfo.lBeginTime:%d m_RaidsInfo.iAllTime:%d",m_RaidsInfo.lBeginTime,m_RaidsInfo.iAllTime);
		return CT_FALSE;
	}

	CMD_G_RaidsReward RaidsReward={0};

	//领取奖励
	G_SceneReward SceneReward={0};
	G_FightingScene FightingScene={0};
	if(!CCFightingScriptSetting::getSingleton().GetFightingScene(m_RaidsInfo.iWorldID,m_RaidsInfo.iSceneID,m_RaidsInfo.iDifficulty,FightingScene))
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate::GetRaidsReward GetFightingScene WorldScene:%d SceneID:%d Difficulty:%d",m_RaidsInfo.iWorldID+1,m_RaidsInfo.iSceneID+1,m_RaidsInfo.iDifficulty+1);
		return CT_FALSE;
	}
	if(!CCFightingScriptSetting::getSingleton().GetSceneReward(m_RaidsInfo.iSceneID,FightingScene.iReward,SceneReward))
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate::GetRaidsReward GetSceneReward SceneID:%d Reward:%d",m_RaidsInfo.iSceneID+1,FightingScene.iReward);
		return CT_FALSE;
	}
	MasterData MasterInfo={0};
	m_pUser->m_userBase.GetMasterData(MasterInfo);

	RaidsReward.dwUserID=m_pUser->GetUserID();
	RaidsReward.iDiamond=SceneReward.iDiamond;
	RaidsReward.iGold=G_GetFightingGold(SceneReward.iGold,MasterInfo.factor.fBattleFactor);
	RaidsReward.iSkillPoint=SceneReward.iSkillPrint;
	RaidsReward.iExperience=G_GetFightingExperience(SceneReward.iExperience,MasterInfo.factor.fBattleFactor);
	RaidsReward.iCurrentTime=m_RaidsInfo.iCurrentTime;


	CT_INT32 iGoods1[GOODS_COUNT*2]={0};
	CT_INT32 iCount1[GOODS_COUNT*2]={0};
	CT_INT32 iGoodsCount1=0;
	FakeProBase fb={0};
	if(SceneReward.iGoodsID1 != 0)
		G_GetGoodsGroupInfo(SceneReward.iGoodsID1,iGoods1,iCount1,fb);
	if(SceneReward.iGoodsID2 != 0)
		G_GetGoodsGroupInfo(SceneReward.iGoodsID2,iGoods1+GOODS_COUNT,iCount1+GOODS_COUNT,fb);

	for(CT_INT32 j=0;j<GOODS_COUNT;++j)
	{
		if(iGoods1[j] != 0 && iCount1[j] != 0)
		{
			RaidsReward.iGoods[iGoodsCount1]=iGoods1[j];
			RaidsReward.iGoodsCount[iGoodsCount1]=iCount1[j];
			iGoodsCount1=(iGoodsCount1+1)%GOODS_COUNT;
		}

		if(iGoods1[GOODS_COUNT+j] != 0 && iCount1[GOODS_COUNT+j] != 0)
		{
			RaidsReward.iGoods[iGoodsCount1]=iGoods1[GOODS_COUNT+j];
			RaidsReward.iGoodsCount[iGoodsCount1]=iCount1[GOODS_COUNT+j];
			iGoodsCount1=(iGoodsCount1+1)%GOODS_COUNT;
		}
	}

	m_RaidsRewardList.push_back(RaidsReward);

	if(m_RaidsRewardList.size() > RAIDS_REWARDCOUNT)
	{
		RR_IT It=m_RaidsRewardList.begin();
		m_RaidsRewardList.erase(It);
	}

	//添加奖励
	MasterVariableData mvd={0};
	mvd.stRes.iGold+=RaidsReward.iGold;
	mvd.stGrade.iCurrentExp+=RaidsReward.iExperience;
	mvd.stRes.iDiamod+=RaidsReward.iDiamond;
	mvd.stRes.iSkillSoul+=RaidsReward.iSkillPoint;
	mvd.stRes.iTalentNum+=RaidsReward.iPotential;
	m_pUser->m_userBase.AddMastData(mvd,SAVETAG_RAIDS_REWARD);

	CT_INT32 iCount=0;
	CT_DWORD threadid = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
	for(CT_INT32 i=0;i<GOODS_COUNT;++i)
	{
		if(RaidsReward.iGoods[i] != 0 && RaidsReward.iGoodsCount[i] != 0)
		{
			iCount=RaidsReward.iGoodsCount[i];
			m_pUser->m_userItem.AutoAddItemData(SAVETAG_RAIDS_REWARD,RaidsReward.iGoods[i],iCount);
		}
	}

	return CT_TRUE;
}

//获取奖励
CT_BOOL CCUserDuplicate1::GetRaidsReward(CT_CHAR *pBuf,CT_INT32 &iLen,CT_BOOL bAllReward)
{
	_AUTOMUTEX(m_pMutex);

	if(iLen < sizeof(CMD_G_RaidsReward))
	{
		CCLogMeg::getSingleton().WriteError("CCUserDuplicate::GetRaidsReward iLen < sizeof(CMD_G_RaidsReward)");
		return CT_FALSE;
	}

	if(bAllReward)
	{
		if(iLen < (CT_INT32)(sizeof(CMD_G_RaidsReward)*m_RaidsRewardList.size()) ||
			m_RaidsRewardList.size() == 0)
		{
			return CT_FALSE;
		}


		CT_INT32 iSize=0;
		RR_IT it;
		RaidsRewardList RRList;
		m_RaidsRewardList.reverse();
		for(it =m_RaidsRewardList.begin();it != m_RaidsRewardList.end();++it)
		{

			RRList.push_back((*it));
			iSize+=sizeof(CMD_G_RaidsReward);

			if((CT_INT32)(iSize/sizeof(CMD_G_RaidsReward)) >= m_RaidsInfo.iCurrentTime)
				break;
		}
		m_RaidsRewardList.reverse();

		iSize=0;
		it=RRList.begin();
		(*it).lTime=GetLastTime();
		RRList.reverse();
		for(it=RRList.begin();it != RRList.end();++it)
		{
			memcpy(pBuf+iSize,&(*it),sizeof(CMD_G_RaidsReward));
			iSize+=sizeof(CMD_G_RaidsReward);
		}

		iLen=  (CT_INT32) sizeof(CMD_G_RaidsReward)*( (CT_INT32)RRList.size() < m_RaidsInfo.iCurrentTime?RRList.size():m_RaidsInfo.iCurrentTime);

		if(iLen == 0)return CT_FALSE;
	}
	else
	{
		RR_IT it =--m_RaidsRewardList.end();
		memcpy(pBuf,&(*it),sizeof(CMD_G_RaidsReward));
		iLen=sizeof(CMD_G_RaidsReward);
	}

	return CT_TRUE;
}




