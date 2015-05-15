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
#include "GlobeScriptInterface.h"
#include "CUserActive.h"
#include "CActive.h"

#include <math.h>

#define  ACTIVE_MES_BUFF_LEN   (1024*5)

CCUserActive::CCUserActive()
{
	m_iRefTime = time(0);
}

CCUserActive::~CCUserActive()
{

}
//清理用户数据
CT_VOID CCUserActive::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_ActiveStatusMap.clear();
	m_PresentMap.clear();
}


CT_BOOL CCUserActive::CheckActiveReceive(CT_INT32 iAID)
{
	_AUTOMUTEX(m_pMutex);
	ActiveStatusMap_IT it = m_ActiveStatusMap.find(iAID);

	if (it!=m_ActiveStatusMap.end()&&it->second.iStatus==ACTIVE_RECEIVED)
	{
		return	CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCUserActive::CheckActiveWhetherComplete(CT_INT32 iAID)
{
	_AUTOMUTEX(m_pMutex);
	ActiveStatusMap_IT it = m_ActiveStatusMap.find(iAID);

	if (it!=m_ActiveStatusMap.end()&&it->second.iStatus==ACTIVE_COMPLETED)
	{
		return	CT_TRUE;
	}
	return CT_FALSE;
}


CT_VOID CCUserActive::ActiveRewardGet(CT_INT32 iAID)
{
	_AUTOMUTEX(m_pMutex);
	ActiveStatus as = {0};
	as.iStatus = ACTIVE_RECEIVED;
	m_ActiveStatusMap[iAID]=as;
}

CT_BOOL	CCUserActive::CheckActiveTypeUsed(CT_INT32 iType)
{
	_AUTOMUTEX(m_pMutex);
	G_ActiveEventType		activeType = {0};
	if (!G_GetActiveEventType(iType,&activeType))
	{
		CCLogMeg::getSingleton().WriteError("CCUserActive::CheckActiveTypeUsed,the active %d data error",iType);
		m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
		return CT_FALSE;
	}
	MasterVariableData mvd ={0};
	m_pUser->m_userBase.GetMasterData(mvd);
	if (mvd.stGrade.iLevel<activeType.iGradeDown||mvd.stGrade.iLevel>activeType.iGradeUp)
	{
		return CT_FALSE;
	}
	if (m_pUser->m_userVip.VipGradeGet()<activeType.iVipGrade)
	{
		return CT_FALSE;
	}
	return CT_TRUE;
}

CT_BOOL CCUserActive::CheckActiveComplete(CT_INT32 iAID)
{
	G_ActiveEventData	activeData = {0};
	if (!G_GetActiveEvent(iAID,&activeData))
	{
		CCLogMeg::getSingleton().WriteError("CCUserActive::CheckActiveComplete,the active %d data error",iAID);
		m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
		return CT_FALSE;
	}

	MasterVariableData mvd = {0};
	m_pUser->m_userBase.GetMasterData(mvd);
	//对活动类型进行判断，如果是领主等级等活动，需要直接进行判断
	ActiveStatusMap_IT it = m_ActiveStatusMap.find(iAID);
	switch(activeData.iType)
	{
	case ACTIVE_MASTER_LEVEL:
		if (mvd.stGrade.iLevel>=activeData.iProperty1)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_PET_LEVEL:
		break;
	case ACTIVE_ARM_LEVEL:
		if (m_pUser->m_userArmy.OwnLevelArm(activeData.iProperty1)>0)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_PET_GET:
		break;
	case ACTIVE_ARM_GET:
		if (m_pUser->m_userArmy.OwnIDArm(activeData.iProperty1))
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_RANK_LEVEL:
		if (mvd.stRank.iLevel>=activeData.iProperty1)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_ARENA_RANKING:
		if (mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingCur <=activeData.iProperty1 && mvd.topData.TopInfo[TOP_TYPE_RANKING].iRankingCur > 0)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_GOLD_ADDUP:
		if (mvd.stRes.iGold>=activeData.iProperty1)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_FOOD_ADDUP:
		if (mvd.stRes.iFood>=activeData.iProperty1)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_CIVIL_ADDUP:
		if (mvd.stCivili.iLevel>=activeData.iProperty1)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_POTENTIAL_GET:
		if (m_pUser->m_userTalent.HasData(activeData.iProperty1))
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_RECHARGE_FIRST:
		return CheckActiveWhetherComplete(activeData.iAEID);
	case ACTIVE_RECHARGE_ADDUP:
	case ACTIVE_CONSUME_ADDUP:
		if (it != m_ActiveStatusMap.end())
		{
			if (it->second.iQuantity >= activeData.iProperty1)
			{
				return CT_TRUE;
			}
		}
		else if (activeData.iProperty1 == 0)
		{
			return CT_TRUE;
		}
		break;
	case ACTIVE_RECHARGE_ONCE:
		if (it != m_ActiveStatusMap.end())
		{
			if (it->second.iQuantity >= activeData.iProperty1)
			{
				return CT_TRUE;
			}
		}
		break;
	case ACTIVE_LIMITATION:
	case ACTIVE_FESTIVAL_PACKET:
		if (it != m_ActiveStatusMap.end())
		{
			if (it->second.iQuantity >= activeData.iProperty1)
			{
				return CT_FALSE;
			}
		}
		return CT_TRUE;
	case ACTIVE_VIP:
		return CT_TRUE;
	case ACTIVE_YEAR_DINNER:
		{
			if (it != m_ActiveStatusMap.end())
			{
				if (it->second.iQuantity >= 4)//此活动只有4次，奖励也是4次，和刚爷确定
				{
					return CT_FALSE;
				}
			}
			return CT_TRUE;
		}
	case ACTIVE_RED_PACKET:
		{
			if (it != m_ActiveStatusMap.end())
			{
				MasterVariableData mvd={0};
				m_pUser->m_userBase.GetMasterData(mvd);
				if (it->second.iQuantity >= mvd.stRes.iMaxWorker)
				{
					return CT_FALSE;
				}
			}
			return CT_TRUE;
		}
	case ACTIVE_PRODUCTION:
		{
			return CT_TRUE;
		}
	case ACTIVE_MONTHCARD:
		{
			if (it != m_ActiveStatusMap.end())
			{
				if (it->second.iStatus == ACTIVE_RECEIVED)
				return CT_TRUE;
			}
			return CT_FALSE;
		}
	default:
		if (it != m_ActiveStatusMap.end())
		{
			if (it->second.iQuantity >= activeData.iProperty1)
			{
				return CT_TRUE;
			}
		}
		break;
	}
	return CT_FALSE;
}

CT_VOID CCUserActive::PushNewAddActive(MSC_DB_ActiveEventChange &activeAdd)
{
	CMD_G_ActiveEventRewardUpdate		activeUpdate = {0};
	CT_CHAR buf[ACTIVE_MES_BUFF_LEN] = {0};
	CT_INT32		iLen = 0;

	CT_DWORD threadDBID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	for (CT_INT32 i=0;i<activeAdd.iNum;++i)
	{
		if (m_ActiveStatusMap.find(activeAdd.iActive[i]) != m_ActiveStatusMap.end())
		{
			continue;
		}
		
		memset(&activeUpdate,0,sizeof(activeUpdate));
		G_ActiveEventData	activeData = {0};
		if (!G_GetActiveEvent(activeAdd.iActive[i],&activeData))
		{
			CCLogMeg::getSingleton().WriteError("CCUserActive::PushNewAddActive,the active %d data error",activeAdd.iActive[i]);
			m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
			return;
		}
		if (activeData.iType == ACTIVE_TURNTABLE || activeData.iType == ACTIVE_YEAR_DINNER || activeData.iType == ACTIVE_RED_PACKET || activeData.iType == ACTIVE_KUA_NIAN)
		{
			//PushTurntable(activeData.iProperty1);
			//continue; 客户端需要活动ID控制转盘的显示
		}
		if (!CheckActivePre(activeAdd.iActive[i]))
		{
			continue;
		}
		if (!CheckActiveTypeUsed(activeData.iType))
		{
			//不符合条件也推送
			
			if(ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate))
			{
				memcpy(buf+iLen,&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));
				iLen+=sizeof(CMD_G_ActiveEventRewardUpdate);
			}
			else
				CCLogMeg::getSingleton().WriteError("CCUserActive::PushNewAddActive ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate)");
			continue;
		}
		activeUpdate.iActiveID=activeAdd.iActive[i];
		//对活动类型进行判断，如果是领主等级等活动，需要直接进行判断
		if (CheckActiveComplete(activeUpdate.iActiveID))
		{
			activeUpdate.iActiveStatus=ACTIVE_COMPLETED;
			//如果已经完成了，需要记录数据库
			MSC_DB_ActiveEventStatus aes = {0};
			aes.dwUserID=m_pUser->GetUserID();
			aes.iAID=activeAdd.iActive[i];
			aes.iStatus=ACTIVE_COMPLETED;
			G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));
		}
		
		if(ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate))
		{
			memcpy(buf+iLen,&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));
			iLen+=sizeof(CMD_G_ActiveEventRewardUpdate);
		}
		else
			CCLogMeg::getSingleton().WriteError("CCUserActive::PushNewAddActive ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate)");
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,buf,iLen);
}

CT_VOID CCUserActive::PushDelActive(MSC_DB_ActiveEventChange &activeDel)
{
	_AUTOMUTEX(m_pMutex);
	CT_CHAR		buf[ACTIVE_MES_BUFF_LEN];
	CT_INT32	iLen=0;
	for (CT_INT32 i=0;i<activeDel.iNum;++i)
	{
		ActiveStatusMap_IT it = m_ActiveStatusMap.find(activeDel.iActive[i]);
		if (it!=m_ActiveStatusMap.end())
		{
// 			G_ActiveEventData	activeData = {0};
// 			if (!G_GetActiveEvent(activeDel.iActive[i],&activeData))
// 			{
// 				CCLogMeg::getSingleton().WriteError("CCUserActive::PushDelActive,the active %d data error",activeDel.iActive[i]);
// 				m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DATA_ERROR);
// 				return;
// 			}
// 			if (activeData.iType == ACTIVE_TURNTABLE)
// 			{
// 				PushTurntable();  设置次数之后统一推送
// 			}
			m_ActiveStatusMap.erase(it);
		}
		
		if(ACTIVE_MES_BUFF_LEN > iLen+sizeof(CT_INT32))
		{
			memcpy(buf+iLen,&activeDel.iActive[i],sizeof(CT_INT32));
			iLen+=sizeof(CT_INT32);
		}
		else
			CCLogMeg::getSingleton().WriteError("CCUserActive::PushDelActive ACTIVE_MES_BUFF_LEN > iLen+sizeof(CT_INT32)");
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_DEL,buf,iLen);
}

CT_VOID CCUserActive::SetActiveStatus(CT_INT32 iAID,CT_INT32 iStatus,CT_INT32 iQuanity)
{
	ActiveStatus	AS ={0};
	AS.iStatus=iStatus;
	AS.iQuantity=iQuanity;
	_AUTOMUTEX(m_pMutex);
	m_ActiveStatusMap[iAID] = AS;
}

CT_VOID CCUserActive::SendUserActiveInfo()
{
	CT_CHAR buf[ACTIVE_MES_BUFF_LEN] = {0};
	CT_INT32 iLen=0;
	CMD_G_ActiveEventRewardUpdate		activeUpdate = {0};
	CT_DWORD threadDBID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	//先copy数据中的活动,map中的数据登陆是数据库读取的
	if (m_ActiveStatusMap.size()>0)
	{
		ActiveStatusMap_IT it = m_ActiveStatusMap.begin();
		while(it!=m_ActiveStatusMap.end())
		{
			memset(&activeUpdate,0,sizeof(activeUpdate));
			activeUpdate.iActiveID=it->first;
			activeUpdate.iActiveStatus=it->second.iStatus;
			activeUpdate.iActiveQuanity=it->second.iQuantity;
			if (it->second.iStatus==ACTIVE_UNCOMPLETE && CheckActiveComplete(activeUpdate.iActiveID))
			{
				activeUpdate.iActiveStatus=ACTIVE_COMPLETED;
				//如果已经完成了，需要记录数据库
				MSC_DB_ActiveEventStatus aes = {0};
				aes.dwUserID=m_pUser->GetUserID();
				aes.iAID=it->first;
				aes.iStatus=ACTIVE_COMPLETED;
				G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));
			}
			CT_INT32 iActiveType = CCActive::getSingleton().GetActiveType(it->first);
			if (ACTIVE_YEAR_DINNER == iActiveType || ACTIVE_RED_PACKET == iActiveType || ACTIVE_KUA_NIAN == iActiveType)
			{
				CCActive::getSingleton().GetTimeActiveStatus(iActiveType, activeUpdate.iActiveID, activeUpdate.iTime, activeUpdate.iTimeStatus);
				if (activeUpdate.iActiveID != it->first)
				{
					++it;
					continue;
				}
				activeUpdate.iTime = (activeUpdate.iTime==-1?-1:activeUpdate.iTime - time(0));
			}
			
			CT_INT32 iTimeAEID = 0;
			CT_INT32 iActiveStatus = 0;
			CT_INT64 iTime = 0;
			CCActive::getSingleton().GetTimeActiveStatus(activeUpdate.iActiveID, iTimeAEID, iTime, iActiveStatus);
			if(ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate))
			{
				// 检查活动是否开启
				MSC_DB_ActiveEventChange aec = {0};
				CCActive::getSingleton().GetServerActive(aec);
				for (CT_INT32 ia = 0;ia < aec.iNum;++ia)
				{
					if (aec.iActive[ia] == activeUpdate.iActiveID)
					{
						memcpy(buf+iLen,&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));
						iLen+=sizeof(CMD_G_ActiveEventRewardUpdate);
						break;
					}
				}
			}
			else
				CCLogMeg::getSingleton().WriteError("CCUserActive::SendUserActiveInfo ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate)");
			it++;
		}
	}
	//添加活动，没有在数据库中记录的	
	MSC_DB_ActiveEventChange		activeList = {0};
	CCActive::getSingleton().GetServerActive(activeList);
	for (CT_INT32 i=0;i<activeList.iNum;++i)
	{
		ActiveStatusMap_IT it = m_ActiveStatusMap.find(activeList.iActive[i]);
		if (it==m_ActiveStatusMap.end())
		{
			CT_INT32 iActiveType = CCActive::getSingleton().GetActiveType(activeList.iActive[i]);
			if (ACTIVE_TURNTABLE == iActiveType)	//转盘不在这里推送
			{
//				continue;
			}
			if (!CheckActivePre(activeList.iActive[i]))			//前置活动未完成(在线奖励活动)
			{
				continue;
			}
			memset(&activeUpdate,0,sizeof(activeUpdate));
			if (ACTIVE_YEAR_DINNER == iActiveType || ACTIVE_RED_PACKET == iActiveType || ACTIVE_KUA_NIAN == iActiveType 
				|| ACTIVE_CS_FIGHT == iActiveType || ACTIVE_CS_SUPPORT == iActiveType)
			{
				CCActive::getSingleton().GetTimeActiveStatus(iActiveType, activeUpdate.iActiveID, activeUpdate.iTime, activeUpdate.iTimeStatus);
				if (activeUpdate.iActiveID != activeList.iActive[i])
				{
					continue;
				}
				activeUpdate.iTime = (activeUpdate.iTime==-1?-1:activeUpdate.iTime - time(0));
				activeUpdate.iActiveStatus = ACTIVE_COMPLETED;
			}
			else
				activeUpdate.iActiveID=activeList.iActive[i];
			//如果活动已经完成，记录数据库
			if (CheckActiveComplete(activeList.iActive[i]))
			{
				activeUpdate.iActiveStatus=ACTIVE_COMPLETED;
				//如果已经完成了，需要记录数据库
				MSC_DB_ActiveEventStatus aes = {0};
				aes.dwUserID=m_pUser->GetUserID();
				aes.iAID=activeList.iActive[i];
				aes.iStatus=ACTIVE_COMPLETED;
				G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));
			}

			if(ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate))
			{
				// 检查活动是否开启
				MSC_DB_ActiveEventChange aecc = {0};
				CCActive::getSingleton().GetServerActive(aecc);
				for (CT_INT32 iac = 0;iac < aecc.iNum;++iac)
				{
					if (aecc.iActive[iac] == activeUpdate.iActiveID)
					{
						memcpy(buf+iLen,&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));
						iLen+=sizeof(CMD_G_ActiveEventRewardUpdate);
						break;
					}
				}	
			}
			else
				CCLogMeg::getSingleton().WriteError("CCUserActive::SendUserActiveInfo ACTIVE_MES_BUFF_LEN > iLen+sizeof(CMD_G_ActiveEventRewardUpdate)");
		}
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,buf,iLen);
	//PushTurntable(); SetTurntableCount()时推送
}

CT_VOID CCUserActive::UpdateActiveStatus(CT_INT32 iAID,CT_INT32 iType,CT_INT32 iQuanity,G_ActiveEventData	&activeData)
{
	//先判断该活动是否已完成或者领取
	_AUTOMUTEX(m_pMutex);
	ActiveStatusMap_IT  it= m_ActiveStatusMap.find(iAID);
	if (it!=m_ActiveStatusMap.end()&&(it->second.iStatus==ACTIVE_RECEIVED))
	{
		return;
	}
	if (it!=m_ActiveStatusMap.end() && it->second.iStatus==ACTIVE_COMPLETED && activeData.iType!=ACTIVE_LIMITATION && activeData.iType!=ACTIVE_FESTIVAL_PACKET && 
		activeData.iType!=ACTIVE_YEAR_DINNER && activeData.iType!=ACTIVE_RED_PACKET)
	{
		return;
	}
	CMD_G_ActiveEventRewardUpdate		activeUpdate = {0};
	ActiveStatus  activeStatus = {0};
	if (activeData.iType==ACTIVE_MASTER_LEVEL||activeData.iType==ACTIVE_PET_LEVEL||activeData.iType==ACTIVE_ARM_LEVEL
		||activeData.iType==ACTIVE_RANK_LEVEL||activeData.iType==ACTIVE_GOLD_ADDUP||activeData.iType==ACTIVE_FOOD_ADDUP
		||activeData.iType==ACTIVE_CIVIL_ADDUP||activeData.iType==ACTIVE_RECHARGE_ONCE||activeData.iType==ACTIVE_RECHARGE_ONCE
		||activeData.iType==ACTIVE_CONSUME_ONCE||activeData.iType==ACTIVE_RECHARGE_FIRST)
	{
		if (iQuanity>=activeData.iProperty1)
		{
			activeStatus.iStatus = ACTIVE_COMPLETED;
			m_ActiveStatusMap[iAID]=activeStatus;
		}
		else
		{
			//只记录完成，领取的活动（累计充值除外）
			return;
		}
	}
	else if (activeData.iType==ACTIVE_PET_GET||activeData.iType==ACTIVE_ARM_GET||activeData.iType==ACTIVE_POTENTIAL_GET
		||activeData.iType==ACTIVE_DUPLICATE_PASS)
	{
		if (iQuanity==activeData.iProperty1)
		{
			activeStatus.iStatus = ACTIVE_COMPLETED;
			m_ActiveStatusMap[iAID]=activeStatus;
		}
		else
		{
			//只记录完成，领取的活动（累计充值除外）
			return;
		}
	}
	else if (activeData.iType==ACTIVE_ARENA_RANKING)
	{
		if (iQuanity<=activeData.iProperty1 && iQuanity >0)
		{
			activeStatus.iStatus = ACTIVE_COMPLETED;
			m_ActiveStatusMap[iAID]=activeStatus;
		}
		else
		{
			//只记录完成，领取的活动（累计充值除外）
			return;
		}
	}
	else if (activeData.iType==ACTIVE_RECHARGE_ADDUP||activeData.iType==ACTIVE_CONSUME_ADDUP||activeData.iType==ACTIVE_LOGIN_DAYS)
	{
		//之前没有这个活动累计
		if (it==m_ActiveStatusMap.end())
		{
			activeStatus.iQuantity=iQuanity;
			if (iQuanity>=activeData.iProperty1)
			{
				activeStatus.iStatus = ACTIVE_COMPLETED;
			}
			m_ActiveStatusMap[iAID] = activeStatus;
		}
		else
		{
			if (it->second.iQuantity+iQuanity>=activeData.iProperty1)
			{
				activeStatus.iStatus = ACTIVE_COMPLETED;
				it->second.iStatus=ACTIVE_COMPLETED;
			}
			it->second.iQuantity+=iQuanity;
			activeStatus.iQuantity=it->second.iQuantity;
		}
	}
	else if (activeData.iType==ACTIVE_LIMITATION || activeData.iType == ACTIVE_FESTIVAL_PACKET)
	{
		if (it==m_ActiveStatusMap.end())
		{
			activeStatus.iQuantity = iQuanity;
			if (iQuanity >= activeData.iProperty1)
			{
				activeStatus.iStatus = ACTIVE_RECEIVED;
			}
			else
			{
				activeStatus.iStatus = ACTIVE_COMPLETED;
			}
			m_ActiveStatusMap[iAID] = activeStatus;
		}
		else
		{
			it->second.iQuantity += iQuanity;
			activeStatus.iQuantity = it->second.iQuantity;
			if (it->second.iQuantity >= activeData.iProperty1)
			{
				activeStatus.iStatus = ACTIVE_RECEIVED;
				it->second.iStatus = ACTIVE_RECEIVED;
			}
			else
			{
				activeStatus.iStatus = ACTIVE_COMPLETED;
				it->second.iStatus = ACTIVE_COMPLETED;
			}
		}
	}
	else if(ACTIVE_YEAR_DINNER == activeData.iType)
	{
		if (it==m_ActiveStatusMap.end())
		{
			activeStatus.iQuantity = iQuanity;
			if (iQuanity >= 4)
			{
				activeStatus.iStatus = ACTIVE_RECEIVED;
			}
			else
			{
				activeStatus.iStatus = ACTIVE_COMPLETED;
			}
			m_ActiveStatusMap[iAID] = activeStatus;
		}
		else
		{
			it->second.iQuantity += iQuanity;
			activeStatus.iQuantity = it->second.iQuantity;
			if(activeStatus.iQuantity >= 4)
				activeStatus.iStatus = ACTIVE_RECEIVED;
			else
				activeStatus.iStatus = ACTIVE_COMPLETED;
		}
	}
	else if(ACTIVE_RED_PACKET == activeData.iType)
	{
		MasterVariableData mvd={0};
		m_pUser->m_userBase.GetMasterData(mvd);
		if (it==m_ActiveStatusMap.end())
		{
			activeStatus.iQuantity = iQuanity;
			if (iQuanity >= mvd.stRes.iMaxWorker)
			{
				activeStatus.iStatus = ACTIVE_RECEIVED;
			}
			else
			{
				activeStatus.iStatus = ACTIVE_COMPLETED;
			}
			m_ActiveStatusMap[iAID] = activeStatus;
		}
		else
		{
			it->second.iQuantity += iQuanity;
			activeStatus.iQuantity = it->second.iQuantity;
			if(activeStatus.iQuantity >= mvd.stRes.iMaxWorker)
				activeStatus.iStatus = ACTIVE_RECEIVED;
			else
				activeStatus.iStatus = ACTIVE_COMPLETED;
		}
	}

	MSC_DB_ActiveEventStatus aes = {0};
	if (activeData.iType == ACTIVE_YEAR_DINNER || activeData.iType == ACTIVE_RED_PACKET)
	{
		CMD_G_ActiveEventRewardUpdate aeru = {0};
		CCActive::getSingleton().GetTimeActiveStatus(activeData.iType, aeru.iActiveID, aeru.iTime, aeru.iTimeStatus);
		if (iAID == aeru.iActiveID && aeru.iTimeStatus == 1)
		{
			activeUpdate.iTime = aeru.iTime - time(0);
			activeUpdate.iTimeStatus = 1;
		}
		else
		{
			activeUpdate.iTime = -1;
		}
	}

	//如果活动完成，发送客户端消息，修改内存数据，修改数据库信息
	if(activeStatus.iStatus >= ACTIVE_COMPLETED || activeData.iType == ACTIVE_RECHARGE_ADDUP || activeData.iType == ACTIVE_CONSUME_ADDUP)
	{
		activeUpdate.iActiveID=iAID;
		activeUpdate.iActiveQuanity=activeStatus.iQuantity;
		activeUpdate.iActiveStatus=activeStatus.iStatus;
		//发送客户端
		m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(activeUpdate));
	}
	//修改数据库信息
	CT_DWORD threadDBID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	
	aes.dwUserID=m_pUser->GetUserID();
	aes.iAID=iAID;
	//累计充值没有完成也要记录数据库
	aes.iStatus=m_ActiveStatusMap[iAID].iStatus;
//	aes.iQuantity=m_ActiveStatusMap[iAID].iQuantity;
	aes.iQuantity=iQuanity;
	G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));
}

CT_VOID CCUserActive::UpdateActiveOnline()
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 AcviteList[100] = {0};
	ActiveStatus  activeStatus = {0};
	G_ActiveEventData	activeData = {0};
	CT_INT64 iTimeNow = time(0);
	CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_ONLINE_DUR,AcviteList);
	for (CT_INT32 i=0; i<n; ++i)
	{
		ActiveStatusMap_IT  it= m_ActiveStatusMap.find(AcviteList[i]);
		if (it!=m_ActiveStatusMap.end()&&(it->second.iStatus==ACTIVE_RECEIVED || it->second.iStatus==ACTIVE_COMPLETED))
		{
			continue;
		}

		if (!G_GetActiveEvent(AcviteList[i],&activeData))
		{
			CCLogMeg::getSingleton().WriteError("UpdateActiveOnline() the active %d data error",AcviteList[i]);
			continue;
		}
		if (!CheckActivePre(AcviteList[i]))
		{
			continue;
		}
		

		if (it==m_ActiveStatusMap.end())
		{
			activeStatus.iQuantity = (CT_INT32)(iTimeNow - m_iRefTime);
			activeStatus.iStatus = ACTIVE_UNCOMPLETE;
			m_ActiveStatusMap[AcviteList[i]] = activeStatus;
		}
		else
		{
			it->second.iQuantity += (CT_INT32)(iTimeNow - m_iRefTime);
			if (it->second.iQuantity > activeData.iProperty1)
			{
				it->second.iStatus = ACTIVE_COMPLETED;
			}
			else
			{
				it->second.iStatus = ACTIVE_UNCOMPLETE;
			}
		}
		if (m_ActiveStatusMap[AcviteList[i]].iStatus == ACTIVE_COMPLETED)
		{
			CMD_G_ActiveEventRewardUpdate		activeUpdate = {0};
			activeUpdate.iActiveID = AcviteList[i];
			activeUpdate.iActiveStatus = m_ActiveStatusMap[AcviteList[i]].iStatus;
			activeUpdate.iActiveQuanity = m_ActiveStatusMap[AcviteList[i]].iQuantity;
			m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(activeUpdate));
		}

		CT_DWORD threadDBID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
		MSC_DB_ActiveEventStatus aes = {0};
		aes.dwUserID=m_pUser->GetUserID();
		aes.iAID=AcviteList[i];
		aes.iStatus=m_ActiveStatusMap[AcviteList[i]].iStatus;
		aes.iQuantity=(CT_INT32)(iTimeNow - m_iRefTime);	//增量
		G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));
	}
	m_iRefTime = iTimeNow;
}

CT_BOOL CCUserActive::CheckActivePre(CT_INT32 iAEID)
{
	_AUTOMUTEX(m_pMutex);
	if (ACTIVE_ONLINE_DUR != CCActive::getSingleton().GetActiveType(iAEID))	//目前只有在线奖励有前置任务
	{
		return CT_TRUE;
	}

	G_ActiveEventData	activeData = {0};
	if (!G_GetActiveEvent(iAEID,&activeData))
	{
		CCLogMeg::getSingleton().WriteError("UpdateActiveOnline() the active %d data error",iAEID);
		return CT_FALSE;
	}

	if (activeData.iProperty2 > 0)
	{
		ActiveStatusMap_IT it2= m_ActiveStatusMap.find((CT_INT32)activeData.iProperty2);
		if (it2 == m_ActiveStatusMap.end() || it2->second.iStatus != ACTIVE_RECEIVED)
		{	
			return CT_FALSE;
		}
	}
	return CT_TRUE;
}

CT_VOID CCUserActive::DisposNextActive(CT_INT32 iAEID)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 AcviteList[100] = {0};
	ActiveStatus  activeStatus = {0};
	G_ActiveEventData	activeData = {0};
	CMD_G_ActiveEventRewardUpdate activeUpdate = {0};

	if (CCActive::getSingleton().GetActiveType(iAEID) != ACTIVE_ONLINE_DUR)		//目前只有在线奖励活动才...
	{
		return;
	}
	CT_INT32 n = CCActive::getSingleton().GetActiveByType(ACTIVE_ONLINE_DUR,AcviteList);
	for (CT_INT32 i=0; i<n; ++i)
	{
		if (!G_GetActiveEvent(AcviteList[i],&activeData))
		{
			CCLogMeg::getSingleton().WriteError("UpdateActiveOnline() the active %d data error",AcviteList[i]);
			continue;
		}

		if (activeData.iProperty2 == iAEID)
		{
			activeUpdate.iActiveID = AcviteList[i];
			activeUpdate.iActiveQuanity = 0;
			activeUpdate.iActiveStatus = ACTIVE_UNCOMPLETE;
			m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));
			ActiveStatusMap_IT  it= m_ActiveStatusMap.find(AcviteList[i]);
			if (it != m_ActiveStatusMap.end())
			{
				it->second.iQuantity = 0;
				it->second.iStatus = ACTIVE_UNCOMPLETE;
			}
		}
	}
}

CT_VOID CCUserActive::PushTurntable()
{
	_AUTOMUTEX(m_pMutex);
	CMD_G_TurntableInfo turntableInfo[3] = {0};
	TurntableItem tti[3] = {0};
	for(CT_INT32 i=0; i<3; i++)
	{
		CCActive::getSingleton().GetTurntableItem(i+1,tti+i);
		turntableInfo[i].iTurntableID = i+1;					//转盘ID
		turntableInfo[i].iOpen = tti[i].iOpen;							//是否开启
		memcpy(turntableInfo[i].reward, tti[i].gReward, sizeof(turntableInfo[i].reward)); //奖项列表
		turntableInfo[i].iCount = m_TurntableCount[i];
	}
	m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_TURNTABLE_UPDATE, (CT_CHAR*)turntableInfo, sizeof(turntableInfo));
}

CT_VOID CCUserActive::SetTurntableCount(CT_INT32 iTimes1,CT_INT32 iTimes2,CT_INT32 iTimes3)
{
	_AUTOMUTEX(m_pMutex);
	m_TurntableCount[0] = iTimes1;
	m_TurntableCount[1] = iTimes2;
	m_TurntableCount[2] = iTimes3;
	PushTurntable();
}

CT_INT32 CCUserActive::GetTurntableCount(CT_INT32 iTurntableID)
{
	_AUTOMUTEX(m_pMutex);
	if (iTurntableID < 1 || iTurntableID > 3)
	{
		return 0;
	}
	return m_TurntableCount[iTurntableID-1];
}

CT_INT32 CCUserActive::AddTurntableCount(CT_INT32 iTurntableID)
{
	_AUTOMUTEX(m_pMutex);
	if (iTurntableID < 1 || iTurntableID > 3)
	{
		return 0;
	}
	return m_TurntableCount[iTurntableID-1] += 1;
}

CT_INT32 CCUserActive::GetActiveQuanity(const CT_INT32 iAID)
{
	_AUTOMUTEX(m_pMutex);
	ActiveStatusMap_IT it=m_ActiveStatusMap.find(iAID);
	if(it != m_ActiveStatusMap.end())
	{
		return (*it).second.iQuantity;
	}
	return 0;
}

CT_VOID CCUserActive::InsertPresent(MSC_DB_Present &dbPresent)
{
	_AUTOMUTEX(m_pMutex); 
	CMD_G_Present present = {0};
	present.dwUID = dbPresent.dwUID;
	present.iIndex = dbPresent.iIndex;
	present.iP_ID = dbPresent.iP_ID;
	present.iP_Count = dbPresent.iP_Count;
	present.iSLotID = dbPresent.iSLotID;
	m_PresentMap.insert(ActivePresentMap::value_type(dbPresent.iIndex, present));

	if (m_PresentDescMap.find(dbPresent.iSLotID) == m_PresentDescMap.end())
	{
		PresentDesc presentDesc = {0};
		presentDesc.iSLotID = dbPresent.iSLotID;
		memcpy(presentDesc.Desc, dbPresent.cDesc, sizeof(presentDesc.Desc));
		m_PresentDescMap.insert(std::map<CT_INT32, PresentDesc>::value_type(dbPresent.iSLotID, presentDesc));
	}
}

CT_VOID CCUserActive::AddPresent(MSC_DB_Present &dbPresent)
{
	_AUTOMUTEX(m_pMutex); 

	if (m_PresentMap.find(dbPresent.iIndex) != m_PresentMap.end())
	{
		return;
	}

	CMD_G_Present present = {0};
	CMD_G_PresentAdd presendAdd = {0};
	present.dwUID = dbPresent.dwUID;
	present.iIndex = dbPresent.iIndex;
	present.iP_ID = dbPresent.iP_ID;
	present.iP_Count = dbPresent.iP_Count;
	present.iSLotID = dbPresent.iSLotID;
	m_PresentMap.insert(ActivePresentMap::value_type(dbPresent.iIndex, present));

	presendAdd.presend = present;
	memcpy(presendAdd.cDesc, dbPresent.cDesc, sizeof(presendAdd.cDesc));
	m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_ACTIVE_PRESENT_ADD, (CT_CHAR*)&presendAdd, sizeof(presendAdd));
	
	//下面这段只有重连时用到.
	if (m_PresentDescMap.find(dbPresent.iSLotID) == m_PresentDescMap.end())
	{
		PresentDesc presentDesc = {0};
		presentDesc.iSLotID = dbPresent.iSLotID;
		memcpy(presentDesc.Desc, dbPresent.cDesc, sizeof(presentDesc.Desc));
		m_PresentDescMap.insert(std::map<CT_INT32, PresentDesc>::value_type(dbPresent.iSLotID, presentDesc));
	}
}

CT_BOOL CCUserActive::GetPresent(CT_INT32 iIndex, CMD_G_Present& present)
{
	_AUTOMUTEX(m_pMutex);

	ActivePresentMap::iterator it = m_PresentMap.find(iIndex);
	if (it == m_PresentMap.end())
	{
		return CT_FALSE;
	}

	memcpy(&present, &(it->second), sizeof(present));
	return CT_TRUE;
}

CT_BOOL CCUserActive::GetPresent(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_PresentMap.size() * sizeof(CMD_G_Present);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	ActivePresentMap::iterator it = m_PresentMap.begin();
	for (int i = 0; it != m_PresentMap.end(); ++it, ++i)
	{
		memcpy(pBuf + sizeof(CMD_G_Present) * i, &(it->second), sizeof(CMD_G_Present));
	}
	bufLen = len;

	return CT_TRUE;
}

CT_BOOL CCUserActive::GetPresentDesc(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_PresentDescMap.size() * sizeof(CMD_G_PresentDesc);
	if (len > bufLen)
	{
		return CT_FALSE;
	}

	std::map<CT_INT32, PresentDesc>::iterator it = m_PresentDescMap.begin();
	for (int i = 0; it != m_PresentDescMap.end(); ++it, ++i)
	{
		memcpy(pBuf + sizeof(CMD_G_PresentDesc) * i, &(it->second), sizeof(CMD_G_PresentDesc));
	}
	bufLen = len;

	return CT_TRUE;
}

CT_BOOL CCUserActive::CheckPresent(CT_INT32 iIndex)
{
	_AUTOMUTEX(m_pMutex);
	if (m_PresentMap.find(iIndex) != m_PresentMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCUserActive::ActivedDragonBall(CMD_G_DragonBallFindFail &dbff)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iRandID = 0;
	CT_INT32 iQuantity = 0;
	CT_INT32 iStatus = 0;
	CT_INT32 iAID = 0;
	CT_INT32 iDBIndex = 1;
	CT_INT32 iResNum0 = 0;
	CT_INT32 iResNum1 = 0;
	ENUM_DragonBallFial enFail;
	G_DragonBall gdb = {0};
	MasterVariableData mvd = {0};
	CMD_G_NormalReward addReward = {0};
	CMD_G_DragonBallFindSuc dbfs = {0};
	CT_INT32 iActiveList[100] = {0};
	CT_INT32 iLen = CCActive::getSingleton().GetActiveByType(ACTIVE_DRAGONBALL, iActiveList);

	// 对应龙珠类型的活动ID应只有1个
	if (iLen != 1)
	{
		CCLogMeg::getSingleton().WriteError("CCUserActive::ActivedDragonBall had two activeid effective.");
		dbff.enFail = DRAGONBALL_CLOSE;
		return CT_FALSE;
	}
	iAID = iActiveList[0];

	//判断用户是否能参与
	if(!CheckActiveTypeUsed(ACTIVE_DRAGONBALL))
	{
		//用户未达到活动条件
		dbff.enFail = DRAGONBALL_NO_ACCESS;
		return CT_FALSE;
	}

	//判断是否已7颗集齐
	iQuantity = GetActiveQuanity(iAID);
	if (iQuantity == DRAGONBALL_FINAL_QUANTITY)
	{
		//已集齐7颗但未领取奖励
		dbff.enFail = DRAGONBALL_SEVERN_FULL;
		return CT_FALSE;
	}
	
	//检查活动状态
	if (CheckActiveWhetherComplete(iAID))
	{ 
		//当天已进行过活动
		dbff.enFail = DRAGONBALL_COMPLETE;
		return CT_FALSE;
	}

	//可以激活，搜寻
	iRandID = rand() % DRAGONBALL_NUM + 1;
	while (1)
	{
		// 当前随机数已激活时，找到其后第一个没激活的位置，激活
		if (!(iQuantity & (1 << (iRandID-1))))
		{
			break;
		}
		if (iRandID == DRAGONBALL_NUM)
		{
			iRandID = 1;
		}
		else
		{
			++iRandID;
		}
	}

	dbfs.iDragonBallID = iRandID;

	// 判断激活的龙珠为第几颗，依激活龙珠数量找到奖励消耗组
	for (CT_INT32 iNum = 0;iNum < 7;++iNum)
	{
		if (iQuantity & (1 << iNum))
		{
			++iDBIndex;
		}
	}

	//判断消耗
	if (!G_GetDragonBall(iDBIndex, &gdb))
	{
		CCLogMeg::getSingleton().WriteError("CCUserActive::ActivedDragonBall G_GetDragonBall error.");
		return CT_FALSE;
	}

	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	FakeProBase fb = {0};

	if (ENUM_CREC_RES_SUC != G_PerformCostReward(gdb.iCostID, m_pUser, ENUM_PCRT_TEST_PERFORM, addReward, iGoods, iCount, fb))
	{
		//资源不足
		dbff.enFail = DRAGONBALL_NO_RES;
		return CT_FALSE;
	}
	// 处理消耗
	memset(&mvd, 0, sizeof(mvd));
	m_pUser->m_userBase.GetMasterData(mvd);
	iResNum0 = addReward.iResNum0;
	iResNum1 = addReward.iResNum1;
	G_ChangeResType(mvd, addReward.iResType0, addReward.iResNum0);
	G_ChangeResType(mvd, addReward.iResType1, addReward.iResNum1);

	dbfs.normalValue[0].iType = 1;
	dbfs.normalValue[0].iID = addReward.iResType0;
	dbfs.normalValue[0].iCount = addReward.iResNum0;
	dbfs.normalValue[1].iType = 1;
	dbfs.normalValue[1].iID = addReward.iResType1;
	dbfs.normalValue[1].iCount = addReward.iResNum1;

	memset(&mvd, 0, sizeof(mvd));
	G_ChangeResType(mvd, addReward.iResType0, iResNum0);
	G_ChangeResType(mvd, addReward.iResType1, iResNum1);
	m_pUser->m_userBase.AddMastData(mvd, SAVATAG_DRAGONBALL_COST);

	//发放奖励
	dbfs.iDragonBallID = iRandID;
	if (!ActiveDragonBallReward(gdb.iRewardID, enFail, &dbfs))
	{
		dbff.enFail = enFail;
		return CT_FALSE;
	}

	return CT_TRUE;
}

CT_BOOL CCUserActive::ActiveDragonBallReward(CT_INT32 iRewardID,  ENUM_DragonBallFial &enfail, CMD_G_DragonBallFindSuc *pdbfs)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iEnFail = ENUM_CREC_RES_SUC;
	CT_INT32 iStatus = 0;
	CT_INT32 iPreQuantity = 0;
	CT_INT32 iQuantity = 0;
	CT_INT32 iAID = 0;
	CT_INT32 iActiveList[100] = {0};
	CT_INT32 iLen = CCActive::getSingleton().GetActiveByType(ACTIVE_DRAGONBALL, iActiveList);

	if (iLen != 1)
	{
		CCLogMeg::getSingleton().WriteError("CCUserActive::ActivedDragonBall had two activeid effective.");
		enfail = DRAGONBALL_CLOSE;
		return CT_FALSE;
	}
	iAID = iActiveList[0];

	// 判断是否达到许愿奖的领取条件
	if (pdbfs == 0)
	{
		//判断用户是否能参与
		if(!CheckActiveTypeUsed(ACTIVE_DRAGONBALL))
		{
			//用户未达到活动条件
			enfail = DRAGONBALL_NO_ACCESS;
			return CT_FALSE;
		}

		//判断是否已7颗集齐
		iQuantity = GetActiveQuanity(iAID);
		iPreQuantity = iQuantity;
		if (iQuantity != DRAGONBALL_FINAL_QUANTITY)
		{
			//未集齐七颗龙珠
			enfail = DRAGONBALL_NO_ACCESS;
			return CT_FALSE;
		}
	}
	
	//检查活动状态
	if (CheckActiveReceive(iAID))
	{ 
		//奖励已领取
		enfail = DRAGONBALL_RECEIVED;
		return CT_FALSE;
	}

	//判断背包
	CMD_G_NormalReward addReward;
	CT_INT32 iGoods[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	CT_INT32 iCount[GOODS_COUNT*2 + TEAM_ITEM_CNT] = {0};
	FakeProBase fb = {0};

	iEnFail = G_PerformCostReward(iRewardID, m_pUser, ENUM_PCRT_TEST, addReward, iGoods, iCount, fb);
	if (ENUM_CREC_ELMBAG_NOAREA == iEnFail)
	{
		// 纹章包不足
		enfail = DRAGONBALL_NO_ELM_BAG;
		return CT_FALSE;
	}
	if (ENUM_CREC_ITEM_FAIL == iEnFail) 
	{
		// 背包不足
		enfail = DRAGONBALL_NO_BAG;
		return CT_FALSE;
	}

	//发放奖励
	G_PerformCostReward(iRewardID, m_pUser, ENUM_PCRT_PERFORM, addReward, iGoods, iCount, fb);

    MasterVariableData mvd={0};
	MasterVariableData mvd1={0};
	m_pUser->m_userBase.GetMasterData(mvd1);
    G_ChangeResType(mvd,addReward.iResType0,addReward.iResNum0);
    G_ChangeResType(mvd,addReward.iResType1,addReward.iResNum1);
    G_ChangeResType(mvd,addReward.iResType2,addReward.iResNum2);
    G_ChangeResType(mvd,addReward.iResType3,addReward.iResNum3);
    m_pUser->m_userBase.AddMastData(mvd, SAVATAG_DRAGONBALL_REWARD);

    G_ChangeResType(mvd1,addReward.iResType0,addReward.iResNum0);
    G_ChangeResType(mvd1,addReward.iResType1,addReward.iResNum1);
    G_ChangeResType(mvd1,addReward.iResType2,addReward.iResNum2);
    G_ChangeResType(mvd1,addReward.iResType3,addReward.iResNum3);

	// 更新内存
	if (pdbfs == 0)
	{
		iStatus = ACTIVE_RECEIVED;
	}
	else
	{
		iQuantity = GetActiveQuanity(iAID);
		iPreQuantity = iQuantity;
		iStatus = ACTIVE_COMPLETED;
		iQuantity = iQuantity | (1 << (pdbfs->iDragonBallID - 1));
	}
	SetActiveStatus(iAID, iStatus, iQuantity);

	// 发送客户端 并 更新活动状态
	CMD_G_ActiveEventRewardUpdate activeUpdate = {0};
	activeUpdate.iActiveID =iAID;
	activeUpdate.iActiveQuanity = iQuantity;
	if (pdbfs != 0)
	{
		activeUpdate.iActiveStatus = ACTIVE_COMPLETED;
	}
	else
	{
		activeUpdate.iActiveStatus = ACTIVE_RECEIVED;
	}
	// 更新活动状态
	m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));

	// 发送成功信息
	if (pdbfs != 0)
	{	
		// 搜寻时normalValue数组的0、1为消耗，2、3为奖励
        /*pdbfs->normalValue[0].iType = 1;
        pdbfs->normalValue[0].iID = addReward.iResType0;
        pdbfs->normalValue[0].iCount = addReward.iResNum0;
        pdbfs->normalValue[1].iType = 1;
        pdbfs->normalValue[1].iID = addReward.iResType1;
        pdbfs->normalValue[1].iCount = addReward.iResNum1;*/
		pdbfs->normalValue[2].iType = 1;
		pdbfs->normalValue[2].iID = addReward.iResType0;
		pdbfs->normalValue[2].iCount = addReward.iResNum0;
		pdbfs->normalValue[3].iType = 1;
		pdbfs->normalValue[3].iID = addReward.iResType1;
		pdbfs->normalValue[3].iCount = addReward.iResNum1;
		m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_DRAGONBALL_SUC, (CT_CHAR*)pdbfs, sizeof(CMD_G_DragonBallFindSuc));
	}
	else
	{
		CMD_G_DragonBallWishSuc dbws = {0};
        dbws.normalValue[0].iType = 1;
		dbws.normalValue[0].iID = addReward.iResType0;
		dbws.normalValue[0].iCount = addReward.iResNum0;
        dbws.normalValue[1].iType = 1;
		dbws.normalValue[1].iID = addReward.iResType1;
		dbws.normalValue[1].iCount = addReward.iResNum1;
        dbws.normalValue[2].iType = 1;
        dbws.normalValue[2].iID = addReward.iResType2;
        dbws.normalValue[2].iCount = addReward.iResNum2;
        dbws.normalValue[3].iType = 1;
        dbws.normalValue[3].iID = addReward.iResType3;
        dbws.normalValue[3].iCount = addReward.iResNum3;
		m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY, SUB_G_DRAGONBALL_WISHING_SUC, (CT_CHAR*)&dbws, sizeof(dbws));
	}

	// 同步数据库
	MSC_DB_ActiveEventStatus aes = {0};
	aes.dwUserID=m_pUser->GetUserID();
	aes.iAID=iAID;
	aes.iStatus=iStatus;
	aes.iQuantity=iQuantity-iPreQuantity; //数据库中iQuantity自动加上以前的数据
	CT_DWORD threadDBID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));

	return CT_TRUE;
}

CT_VOID CCUserActive::UpdataDragonBallStatus()
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iActiveList[100] = {0};
	CT_INT32 iNum = 0;
	CT_INT32 iStatus = 0;
	CT_INT32 iQuantity = 0;
	CMD_G_ActiveEventRewardUpdate activeUpdate = {0};

	iNum = CCActive::getSingleton().GetActiveByType(ACTIVE_DRAGONBALL, iActiveList);

	for (CT_INT32 i = 0;i < iNum;++i)
	{
		iQuantity = GetActiveQuanity(iActiveList[i]);
		if (CheckActiveWhetherComplete(iActiveList[i]) && iQuantity != DRAGONBALL_FINAL_QUANTITY)
		{
			// 更新内存
			iStatus = ACTIVE_UNCOMPLETE;
			SetActiveStatus(iActiveList[i], iStatus, iQuantity);

			// 发送客户端
			activeUpdate.iActiveID =iActiveList[i];
			activeUpdate.iActiveStatus = ACTIVE_UNCOMPLETE;
			activeUpdate.iActiveQuanity = iQuantity;
			m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));

			// 同步数据库
			MSC_DB_ActiveEventStatus aes = {0};
			aes.dwUserID=m_pUser->GetUserID();
			aes.iAID=iActiveList[i];
			aes.iStatus=iStatus;
			CT_DWORD threadDBID = GAME_THREAD_DATABASE + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadDBID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&aes,sizeof(MSC_DB_ActiveEventStatus));
		}
	}
}

//零点更新月卡状态
CT_VOID	CCUserActive::UpdataMonthCardStatus()
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 iQuanity=GetActiveQuanity(ACTIVE_MONTH_CARD);
	if(iQuanity > 0)
	{
		SetActiveStatus(ACTIVE_MONTH_CARD,ACTIVE_COMPLETED,iQuanity-1);

		// 发送客户端
		CMD_G_ActiveEventRewardUpdate activeUpdate = {0};
		activeUpdate.iActiveID =ACTIVE_MONTH_CARD;
		activeUpdate.iActiveStatus = ACTIVE_COMPLETED;
		activeUpdate.iActiveQuanity = iQuanity-1;
		m_pUser->GetNetInterface()->Send(MAIN_G_ACTIVE_EVENT_CITY,SUB_G_ACTIVE_UPDATE,(CT_CHAR*)&activeUpdate,sizeof(CMD_G_ActiveEventRewardUpdate));

	}
	return;
}