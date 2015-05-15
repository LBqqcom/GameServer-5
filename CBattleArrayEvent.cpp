#include "CBattleArrayEvent.h"
#include "CLogMeg.h"
#include "CGameConfig.h"
#include "CMD_Game.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CDatabaseMes.h"

CCBattleArrayEvent::CCBattleArrayEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_MASTER_EVENT_CITY,SUB_G_OPEN_BATTLE_ARRAY);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY,SUB_G_BATTLE_ARRAY_INFO);
	RegistMessage(MSC_MAIN_BATTLE_ARRAY,MSC_SUB_UPDATA_BATTLE_ARRAY);
}

CCBattleArrayEvent::~CCBattleArrayEvent(void)
{
}

//处理用户消息
CT_VOID CCBattleArrayEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_MASTER_EVENT_CITY:
		{
			DisposeNetMessage(subID,pBuf,len);
			break;
		}
	case MSC_MAIN_BATTLE_ARRAY:
		{
			DisposeThreadMessage(subID,pBuf,len);
			break;
		}
	}
	return;
}

//网络消息
CT_VOID CCBattleArrayEvent::DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
		case SUB_G_OPEN_BATTLE_ARRAY:
		{
			DisposeOpenBattleArray(pBuf,len);				//打开阵法
			break;
		}
		case SUB_G_BATTLE_ARRAY_INFO:
		{
			DisposeBattleArrayInfo(pBuf,len);			//阵法信息
			break;
		}
	}
	return;
}

//线程消息
CT_VOID CCBattleArrayEvent::DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	switch(subID)
	{
	case MSC_SUB_UPDATA_BATTLE_ARRAY:
		{
			DisposeBattleArrayInfo(pBuf,len);			//阵法信息
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCBattleArrayEvent::DisposeThreadMessage subid:%d",subID);
			break;
		}
	}
	return;
}

//用户定时器处理
CT_VOID CCBattleArrayEvent::OnTime(CT_DWORD timeID)
{
	return;
}

//打开阵法
CT_VOID CCBattleArrayEvent::DisposeOpenBattleArray(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_OpenBattleArray))
	{
		CCLogMeg::getSingleton().WriteError("DisposeOpenBattleArray : len != sizeof(CMD_G_OpenBattleArray)");
		return;
	}

	CMD_G_OpenBattleArray *pOpenBattleArray=(CMD_G_OpenBattleArray*)pBuf;

	CMD_G_BattleArrayInfo BattleArrayInfo={0};

	BattleArrayInfo.dwUserID=pOpenBattleArray->dwUserID;

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(pOpenBattleArray->dwUserID,&pUser))
	{
		pUser->m_userBattleArray.GetBattleArray(BattleArrayInfo.iPosition);

		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_BATTLE_ARRAY_INFO,(CT_CHAR*)&BattleArrayInfo,sizeof(BattleArrayInfo));
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("%s %d",__FILE__,__LINE__);
	}

	return;
}

//阵法信息
CT_VOID CCBattleArrayEvent::DisposeBattleArrayInfo(CT_CHAR* pBuf,CT_DWORD len)
{
	if(len != sizeof(CMD_G_BattleArrayInfo))
	{
		CCLogMeg::getSingleton().WriteError("DisposeBattleArrayInfo : len != sizeof(CMD_G_BattleArrayInfo)");
		return;
	}

	CMD_G_BattleArrayInfo *pBattleArrayInfo=(CMD_G_BattleArrayInfo*)pBuf;

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(pBattleArrayInfo->dwUserID,&pUser))
	{
		//军队验证
		if(!ArmyValidate(pUser,pBattleArrayInfo->iPosition))return;

		//更新用户信息
		pUser->m_userBattleArray.UpDataBattleArray(pBattleArrayInfo->iPosition);

		//更新数据库
		MSC_DB_BattleArray DBBattleArray={0};
		DBBattleArray.dwUserID=pBattleArrayInfo->dwUserID;
		memcpy(DBBattleArray.iPosition,pBattleArrayInfo->iPosition,sizeof(pBattleArrayInfo->iPosition));
		CT_DWORD threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_DB_BATTLE_ARRAY_DATA,MSC_SUB_DB_W_BATTLE_ARRAY_INFO,(CT_CHAR*)&DBBattleArray,sizeof(DBBattleArray));

		//更新客户端
		CMD_G_BattleArrayInfo BattleArrayInfo={0};
		BattleArrayInfo.dwUserID=pBattleArrayInfo->dwUserID;
		pUser->m_userBattleArray.GetBattleArray(BattleArrayInfo.iPosition);
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_BATTLE_ARRAY_INFO,(CT_CHAR*)&BattleArrayInfo,sizeof(BattleArrayInfo));

		pUser->UpdateAndSendMasterData(EMI_BATLLE);

	}
	else
	{
		CCLogMeg::getSingleton().WriteError("%s %d",__FILE__,__LINE__);
	}


	return;
}

//阵法验证
CT_BOOL	CCBattleArrayEvent::ArmyValidate(CCUser *pUser,CT_INT32 iPosition[POST_COUNT/2])
{
	MasterData MasterInfo={0};
	pUser->m_userBase.GetMasterData(MasterInfo);

	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPosition[i] == MasterInfo.iOccupation || iPosition[i] == 0)continue;

		ArmyInfo Armyinfo={0};
		if(!pUser->m_userArmy.GetArmyInfo(iPosition[i],Armyinfo))
		{
			CMD_G_ErrorBattleArray BattleArrayError={0};
			BattleArrayError.dwUserID=pUser->GetUserID();
			BattleArrayError.iErrorBattleArray=BATTLEARRAY_ERROE_ARMYID;
			pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ERROE_BATTLE_ARRAY,
				(CT_CHAR*)&BattleArrayError,sizeof(BattleArrayError));

			return CT_FALSE;
		}
		else
		{
			////军队验证（根据兵营）
			//if(!pUser->m_userBuilding.BuildValidate(CITY_TYPE_SOLDIER))
			//{
			//	CMD_G_ErrorBattleArray BattleArrayError={0};
			//	BattleArrayError.dwUserID=pUser->GetUserID();
			//	BattleArrayError.iErrorBattleArray=BATTLEARRAY_ERROE_ARMY_CAMP;
			//	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ERROE_BATTLE_ARRAY,
			//		(CT_CHAR*)&BattleArrayError,sizeof(BattleArrayError));
			//}

		}
	}

	//重复验证
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPosition[i] == 0)continue;
		for(CT_INT32 j=i+1;j<POST_COUNT/2;++j)
		{
			if(iPosition[j] == 0)continue;
			if(iPosition[i] == iPosition[j])
			{
				CMD_G_ErrorBattleArray BattleArrayError={0};
				BattleArrayError.dwUserID=pUser->GetUserID();
				BattleArrayError.iErrorBattleArray=BATTLEARRAY_ERROE_MASTER;
				pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ERROE_BATTLE_ARRAY,
					(CT_CHAR*)&BattleArrayError,sizeof(BattleArrayError));

				return CT_FALSE;   
			}
		}
	}

	//召唤兵种验证
	CT_INT32 iBoss=0;
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPosition[i] == 0)continue;
		if(iPosition[i] >= 1050)
		{
			iBoss++;  
		}
	}
	if(iBoss >= 2)
	{
		CMD_G_ErrorBattleArray BattleArrayError={0};
		BattleArrayError.dwUserID=pUser->GetUserID();
		BattleArrayError.iErrorBattleArray=BATTLEARRAY_ERROR_MAX;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ERROE_BATTLE_ARRAY,
			(CT_CHAR*)&BattleArrayError,sizeof(BattleArrayError));
		return CT_FALSE;
	}

	//上阵数目判断
	CT_INT32 iCount=0;
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPosition[i] != 0)
		{
			iCount++;
			if(iCount > 3)
			{
				CMD_G_ErrorBattleArray BattleArrayError={0};
				BattleArrayError.dwUserID=pUser->GetUserID();
				BattleArrayError.iErrorBattleArray=BATTLEARRAY_ERROR_MAX;
				pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ERROE_BATTLE_ARRAY,
					(CT_CHAR*)&BattleArrayError,sizeof(BattleArrayError));
				return CT_FALSE;
			}
		}
	}

	//领主验证
	for(CT_INT32 i=0;i<POST_COUNT/2;++i)
	{
		if(iPosition[i] == MasterInfo.iOccupation)
		{
			return CT_TRUE;
		}
	}

	CMD_G_ErrorBattleArray BattleArrayError={0};
	BattleArrayError.dwUserID=pUser->GetUserID();
	BattleArrayError.iErrorBattleArray=BATTLEARRAY_ERROE_MASTER;
	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_ERROE_BATTLE_ARRAY,
		(CT_CHAR*)&BattleArrayError,sizeof(BattleArrayError));

	return CT_FALSE;
}