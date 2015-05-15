#include "CUserManager.h"
#include "CObjPool.h"
#include "CConfig.h"
#include "CLogMeg.h"
#include "CBroadcastScriptSetting.h"
#include "CItemScriptSetting.h"
#include "CMarket.h"
#include "CThreadMes.h"
#include "CGlobeFunc.h"
#include "CGameConfig.h"
#include "CUser.h"
#include "CCompress.h"
#include "CCSFightData.h"
template<> CCUserManager* CCSingleton<CCUserManager>::ms_Singleton=0;

CCObjPool<CCUser>  G_UserPool;

CCUserManager::CCUserManager(CT_VOID)
{
	CT_INT32 num = CCConfig::getSingleton().m_maxClientNum + 128 ;
	G_UserPool.Init(num);
	m_GMList.clear();

	m_pBuffer = new CT_CHAR[COMPRESS_MAX_SIZE];
}

CCUserManager::~CCUserManager(CT_VOID)
{
	delete m_pBuffer;
	m_pBuffer = 0;
	m_GMList.clear();
}

//得到一个空闲的用户
CCUser* CCUserManager::GetOneFreeUser(CT_DWORD userID, CCNetInterface* pNetInterface)
{
	_AUTOMUTEX(&m_mutex);
	if (Find(pNetInterface))
	{
		UFM_IT it =  m_userFindMap.find(pNetInterface);
		if (it != m_userFindMap.end())
		{
			m_userFindMap.erase(it);
		}
	}

	CCUser* pUser = 0;
	CT_BOOL bFind = Find(userID, &pUser);
	if (bFind)
	{
		pUser->Init();
		pUser->SetUserID(userID);
		pUser->SetNetInterface(pNetInterface);
		pUser->SetHeartBeat();
		m_userFindMap.insert(UserIDFindMap::value_type(pNetInterface, userID));
		return pUser;
	}

	pUser = G_UserPool.NewObj();
	CT_ASSERT(pUser != 0 || "G_UserPool.NewObj();");
	if (pUser == 0)
	{
		return 0;
	}
	
	if (!bFind && pUser != 0)
	{
		pUser->Init();
		pUser->SetUserID(userID);
		pUser->SetNetInterface(pNetInterface);
		pUser->SetHeartBeat();
		m_userDataMap.insert(UserDataMap::value_type(userID, pUser));
		m_userFindMap.insert(UserIDFindMap::value_type(pNetInterface, userID));
	}
	return pUser;
}

//设置通讯句柄
CT_BOOL CCUserManager::SetNetHandle(CT_DWORD userID, CCNetInterface* pNetHandle)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.find(userID);
	if (it != m_userDataMap.end())
	{
		(*it).second->SetNetInterface(pNetHandle);
	}

	UFM_IT itt = m_userFindMap.find(pNetHandle);
	if (itt != m_userFindMap.end())
	{
		m_userFindMap.erase(itt);
	}
	m_userFindMap.insert(UserIDFindMap::value_type(pNetHandle, userID));
	return CT_TRUE;
}



//查找数据 任何人不得改动这个函数的实现（会产生死锁问题）彭文
CT_BOOL CCUserManager::Find(CT_DWORD id)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.find(id);
	if (it != m_userDataMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//查找数据 任何人不得改动这个函数的实现（会产生死锁问题） 彭文
CT_BOOL CCUserManager::Find(CCNetInterface* pNetHandle)
{
	_AUTOMUTEX(&m_mutex);
	UFM_IT it = m_userFindMap.find(pNetHandle);
	if (it != m_userFindMap.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

//查找数据 任何人不得改动这个函数的实现（会产生死锁问题） 彭文
CT_BOOL CCUserManager::Find(CT_DWORD id, CCUser**  us)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.find(id);
	if (it != m_userDataMap.end())
	{
		*us = (*it).second;
		return CT_TRUE;
	}
	return CT_FALSE;
}

//查找数据 任何人不得改动这个函数的实现（会产生死锁问题） 彭文
CT_BOOL CCUserManager::Find(CCNetInterface* pNetHandle, CCUser**  us)
{
	_AUTOMUTEX(&m_mutex);
	UFM_IT it = m_userFindMap.find(pNetHandle);
	if (it != m_userFindMap.end())
	{
		//id = (*it).second;
		UDM_IT itt = m_userDataMap.find((*it).second);
		if (itt  != m_userDataMap.end())
		{
			(*us) = (*itt).second;
			return CT_TRUE;
		}

	}
	return CT_FALSE;
}

//通过昵称查找 任何人不得改动这个函数的实现（会产生死锁问题） 彭文
CT_BOOL CCUserManager::Find(std::string cNickName, CCUser** us)
{
	_AUTOMUTEX(&m_mutex);
	UDNM_IT it = m_userDataNameMap.find(cNickName);
	if(it!=m_userDataNameMap.end())
	{
		(*us) = (*it).second;
		return CT_TRUE;
	}
	return CT_FALSE;
}

//清除数据
CT_VOID CCUserManager::Erase(CT_DWORD id)
{
	_AUTOMUTEX(&m_mutex);
	CCNetInterface* p = 0;
	CCUser*         pUser = 0;
	{
		UDM_IT it = m_userDataMap.find(id);
		if (it != m_userDataMap.end())
		{
			pUser = (*it).second;
			p = (*it).second->GetNetInterface();
			G_UserPool.DeleteObj((*it).second);
			m_userDataMap.erase(it);
		}
	}
	
	{
		UFM_IT itt = m_userFindMap.find(p);
		if (itt != m_userFindMap.end())
		{
			m_userFindMap.erase(itt);
		}
	}

	{

		MasterData md;
		if (pUser)
		{
			pUser->m_userBase.GetMasterData(md);
		}
		UDNM_IT  it = m_userDataNameMap.find(std::string(md.cNickName));
		if (it != m_userDataNameMap.end())
		{
			m_userDataNameMap.erase(it);
		}
	}
}


//清空
CT_VOID CCUserManager::Clear()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT itEnd = m_userDataMap.end();
	for (; it != itEnd; ++it)
	{
		G_UserPool.DeleteObj((*it).second);
	}
	m_userDataMap.clear();
	m_userFindMap.clear();
}


//更新升级的建筑
CT_VOID CCUserManager::UpdateUpgradingBuilding(CT_INT32 threadIndex, CT_INT32 threadTotalNum,  CT_DWORD dwThreadID)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{ 
		if (threadIndex == (*it).second->GetUserID() % threadTotalNum )  //防止分配到别的线程
		{
			(*it).second->m_userBuilding.UpdateUpgradingBuilding(dwThreadID );
		}
	}
}

//保存建筑数据
CT_VOID CCUserManager::UpdateSaveBuildingData(CT_INT32 threadIndex, CT_INT32 threadTotalNum,  CT_DWORD dwThreadID)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{ 
		if (threadIndex == (*it).second->GetUserID() % threadTotalNum )  //防止分配到别的线程
		{
			(*it).second->m_userBuilding.UpdateSaveBuildingData(CT_FALSE, CT_FALSE, CT_FALSE );
		}
	}
}

//同步生产数据到数据库
CT_VOID CCUserManager::UpdateSaveProductingData(CT_INT32 threadIndex, CT_INT32 threadTotalNum,  CT_DWORD dwThreadID)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{ 
		if (threadIndex == (*it).second->GetUserID() % threadTotalNum )  //防止分配到别的线程
		{
			(*it).second->m_userBuilding.UpdateSaveProductingData(CT_FALSE, CT_FALSE );
		}
	}
}

CT_VOID CCUserManager::UpdateProductingBuilding(CT_INT32 threadIndex, CT_INT32 threadTotalNum)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		if (threadIndex == (*it).second->GetUserID() % threadTotalNum )  //防止分配到别的线程
		{
			(*it).second->m_userBuilding. UpdateProductingBuilding();
		}
	}
}

//更新事件
CT_VOID CCUserManager::UpdateEvent(CT_INT32 threadIndex, CT_INT32 threadTotalNum)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		if (threadIndex == (*it).second->GetUserID() % threadTotalNum )  //防止分配到别的线程
		{
			(*it).second->m_userEvent.UpdateEvent();
		}
	}
}

// 更新领主增益 [10/10/2012 John]
CT_VOID CCUserManager::UpdateItemBuff()
{
	_AUTOMUTEX(&m_mutex);
    UDM_IT it = m_userDataMap.begin();
    UDM_IT endIT = m_userDataMap.end();
    for (; it!=endIT; ++it)
    {
        (*it).second->m_userBuff.UpdateItemBuff();
    }
}

// 更新领主冷却 [10/30/2012 John]
CT_VOID CCUserManager::UpdateCooldown()
{
	_AUTOMUTEX(&m_mutex);
    UDM_IT it = m_userDataMap.begin();
    UDM_IT endIT = m_userDataMap.end();
    for (; it!=endIT; ++it)
    {
        (*it).second->m_userBase.RefreshCooldown();
    }
}
// 同步领主道具信息
CT_VOID CCUserManager::SyncItemData()
{
    _AUTOMUTEX(&m_mutex);
    UDM_IT it = m_userDataMap.begin();
    UDM_IT endIT = m_userDataMap.end();
    CT_INT64 currTime = time(0);
    for (; it!=endIT; ++it)
    {
		if (it->second->GetInit()) {
			(*it).second->m_userItem.SyncData(currTime);
			(*it).second->m_userElmPower.SyncData(currTime);
			(*it).second->m_userEquip.SyncData(currTime);
			(*it).second->m_userBuff.SyncData(currTime);
			(*it).second->m_userElmAttr.SyncData(currTime);
		}
    }
}

// 发送世界聊天消息
CT_VOID CCUserManager::SendWorldChat(CMD_G_WorldChat *pWorldChat,CT_DWORD len)
{	
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for(;it!=endIT;++it)
	{
		if((*it).first==pWorldChat->dwUserID)
			continue;
		//检查黑名单
		CCUser *pUser;
		if (Find((*it).first, &pUser))
		{
			if (pUser->m_userFriend.CheckBlackList(pWorldChat->dwUserID))
			{
				continue;
			}
		}
		CCNetInterface* pNetHandle = (*it).second->GetNetInterface();
		if (IsGM(pWorldChat->dwUserID))
			pNetHandle->Send(MAIN_G_CHAT_CITY,SUB_G_GMWORLD_CHAT,(CT_CHAR*)pWorldChat, len);
		else
			pNetHandle->Send(MAIN_G_CHAT_CITY,SUB_G_WORLD_CHAT,(CT_CHAR*)pWorldChat, len);
	}
}

CT_VOID CCUserManager::BroadCastWorldMessage(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for(;it!=endIT;++it)
	{
		CCNetInterface* pNetHandle = (*it).second->GetNetInterface();
		pNetHandle->Send(MAIN_G_CHAT_CITY,SUB_G_BROADCAST_CHAT,pBuf,iLen);
	}
}

CT_VOID CCUserManager::BroadCastMessage(CT_DWORD mainID, CT_DWORD subID,CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for(;it!=endIT;++it)
	{
		CCNetInterface* pNetHandle = (*it).second->GetNetInterface();
		pNetHandle->Send(mainID,subID,pBuf,iLen);
	}
}

CT_VOID CCUserManager::SendGMMessage(CT_DWORD mid, CT_DWORD sid, CT_CHAR* pMes,CT_INT32 len,CT_DWORD dwMeUserID)
{
	_AUTOMUTEX(&m_mutex);

	if(dwMeUserID == 0)
	{
		UDM_IT it = m_userDataMap.begin();
		UDM_IT endIT = m_userDataMap.end();
		for(;it!=endIT;++it)
		{
			CCNetInterface* pNetHandle = (*it).second->GetNetInterface();
			pNetHandle->Send(mid, sid ,(CT_CHAR*)pMes, len);
		}
	}
	else
	{
		UDM_IT it = m_userDataMap.find(dwMeUserID);
		if(it != m_userDataMap.end())
		{
			CCNetInterface* pNetHandle = (*it).second->GetNetInterface();
			pNetHandle->Send(mid, sid,(CT_CHAR*)pMes, len);
		}
	}
}

//系统消息
CT_VOID CCUserManager::SendServerMessage(CMD_G_GMMessage &pMes,CT_INT32 len,CT_DWORD dwMeUserID)
{
	_AUTOMUTEX(&m_mutex);

	if(dwMeUserID == 0)
	{
		UDM_IT it = m_userDataMap.begin();
		UDM_IT endIT = m_userDataMap.end();
		for(;it!=endIT;++it)
		{
			CCNetInterface* pNetHandle = (*it).second->GetNetInterface();
			pNetHandle->Send(MAIN_G_CHAT_CITY,SUB_G_SERVER_MESSAGE,(CT_CHAR*)&pMes, len);
		}
	}
	else
	{
		UDM_IT it = m_userDataMap.find(dwMeUserID);
		if(it != m_userDataMap.end())
		{
			CCNetInterface* pNetHandle = (*it).second->GetNetInterface();
			pNetHandle->Send(MAIN_G_CHAT_CITY,SUB_G_SERVER_MESSAGE,(CT_CHAR*)&pMes, len);
		}
	}
}
// 刷新签到信息
CT_VOID CCUserManager::UpdateSignIn(CT_INT32 threadIndex, CT_INT32 threadTotalNum)
{
	_AUTOMUTEX(&m_mutex);
    UDM_IT it = m_userDataMap.begin();
    UDM_IT endIT = m_userDataMap.end();
    for (; it!=endIT; ++it)
    {
        if (threadIndex == (*it).second->GetUserID() % threadTotalNum )  //防止分配到别的线程
        {
            (*it).second->m_userDaily.UpdateSignIn();
        }
    }
}



CT_VOID CCUserManager::UpdateSingleGoodsStatus(CMD_G_MarketGoodsInfo &pGoodsInfo)
{
	_AUTOMUTEX(&m_mutex);
	//更新所有用户该商品个数
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for(;it!=endIT;++it)
	{
		it->second->m_userBlackMarket.UpdateSingleGoodsStatus(pGoodsInfo);	
	}
}

//更新心跳
CT_VOID CCUserManager::UpdateHeartBeat()
{
	_AUTOMUTEX(&m_mutex);

	CT_INT64 lTime=time(0);
	CCUserManager::UDM_IT UserMapEnd=m_userDataMap.end();
	for(CCUserManager::UDM_IT i=m_userDataMap.begin(); i != UserMapEnd;++i)
	{
		(*i).second->UpdataHeartBeat(lTime);
	}
	return;
}

//更新竞技度
CT_VOID CCUserManager::UpdataPVPDegree()
{
	_AUTOMUTEX(&m_mutex);
	G_ValueRange Degree={0};
	if(!G_GetValueRange(VALUE_TYPE_PVPDEGREEUP,&Degree))
	{
		CCLogMeg::getSingleton().WriteError("UpdataPVPDegree G_GetValueRange VALUE_TYPE_PVPDEGREEUP");
		return;
	}

	CCUserManager::UDM_IT UserMapEnd=m_userDataMap.end();
	for(CCUserManager::UDM_IT i=m_userDataMap.begin(); i != UserMapEnd;++i)
	{
		(*i).second->UpdataPVPDegree((CT_INT32)Degree.fMinVal);
	}
}

CT_VOID CCUserManager::RefreshMarketGoods()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userBlackMarket.RefreshMarketGoods();
	}
}

CT_VOID CCUserManager::RefreshBlackMarketGoods()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		if (it->second->m_userBlackMarket.ViewBlackMarket())
		{
			it->second->m_userBlackMarket.RefreshBlackMarket(MARKET_BLACK_TYPE);
		}
	}
}

// 发放军衔俸禄
CT_VOID CCUserManager::PushRankSalary()
{
    _AUTOMUTEX(&m_mutex);
    UDM_IT it = m_userDataMap.begin();
    UDM_IT endIT = m_userDataMap.end();
    for (; it!=endIT; ++it)
    {
        it->second->m_userBase.PushRankSalary();
    }
}

// 恢复体力值
CT_VOID CCUserManager::RecoverStamina()
{
    _AUTOMUTEX(&m_mutex);
    UDM_IT it = m_userDataMap.begin();
    UDM_IT endIT = m_userDataMap.end();
    for (; it!=endIT; ++it)
    {
		if(it->second->GetInit())
			it->second->m_userBase.RecoverStamina();
    }
}

//更新副本
CT_VOID CCUserManager::RefreshDuplicate()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userDuplicate.UpdataDuplicate();
	}
	return;
}

//更新竞技场
CT_VOID CCUserManager::RefreshPVP()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		memset(m_pBuffer, 0,  COMPRESS_MAX_SIZE);
		it->second->m_userPVPInfo.UpdataPVPData(m_pBuffer,COMPRESS_MAX_SIZE);
	}
	return;
}

//更新祭祀
CT_VOID CCUserManager::RefreshWorship()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userWorship.UpdateWorshipData();
	}
}

CT_VOID CCUserManager::RefreshTurntable()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userActive.SetTurntableCount(0,0,0);
	}
}

CT_VOID CCUserManager::RefreshActiveStatus(CT_INT32 iAEID, CT_INT32 iStatus)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userActive.SetActiveStatus(iAEID, iStatus, 0);
	}

}

//跨服战状态信息
CT_VOID CCUserManager::SendCSStatusInfo(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		CT_INT32 iTempLen=iLen-sizeof(CMD_G_CrossBaseInfo);
		if(CCCSFightData::getSingleton().GetUserReward((*it).first,pBuf+sizeof(CMD_G_CrossBaseInfo),iTempLen))
			(*it).second->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CROSS_SERVER_BASEINFO,pBuf,sizeof(CMD_G_CrossBaseInfo));
		else
			(*it).second->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CROSS_SERVER_BASEINFO,pBuf,iTempLen+sizeof(CMD_G_CrossBaseInfo));
	}
	return;
}

//跨服战状态信息
CT_VOID CCUserManager::SendCSRewardInfo()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		CT_CHAR cBuf[MAX_MES_MAX_BUFF_LEN]={0};
		CT_INT32 iBufLen=MAX_MES_MAX_BUFF_LEN;
		if(CCCSFightData::getSingleton().GetUserReward((*it).first,cBuf,iBufLen))
			(*it).second->GetNetInterface()->Send(MAIN_G_CROSS_SERVER,SUB_G_CS_REWARD,cBuf,iBufLen);
	}
	return;
}

//跨服战全服奖励
CT_VOID CCUserManager::SendCSFullServerReward(G_FullServiceReward &fsr,CT_CHAR *pBuf,CT_INT32 iLen)
{
	std::map<CT_DWORD,CT_DWORD> UIDmap;
	for(CT_INT32 i=0;i<iLen/sizeof(CT_DWORD);++i)
	{
		CT_DWORD dwUserID=0;
		memcpy(&dwUserID,pBuf+sizeof(CT_DWORD)*i,sizeof(CT_DWORD));
		UIDmap.insert(std::map<CT_DWORD,CT_DWORD>::value_type(dwUserID,dwUserID));
	}

	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		std::map<CT_DWORD,CT_DWORD>::iterator UIDIt=UIDmap.find((*it).second->GetUserID());
		if(UIDmap.end() != UIDIt)continue;

		CCCSFightData::CSUserReward csur={0};
		csur.iType=CLIENT_OBJ_RES;
		csur.iResID=fsr.iResID;
		csur.iNum=fsr.iResNum;
		CCCSFightData::getSingleton().InsertReward((*it).second->GetUserID(),csur);

		memset(&csur,0,sizeof(csur));
		csur.iType=CLIENT_OBJ_ITEM;
		csur.iResID=fsr.iItem;
		csur.iNum=fsr.iItemNum;
		CCCSFightData::getSingleton().InsertReward((*it).second->GetUserID(),csur);
	}

	return;
}

CT_VOID CCUserManager::RefreshTowerPass()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userTower.RefreshTowerPass();
	}
}

CT_VOID CCUserManager::BuffReward(CT_INT32 iID)
{
	_AUTOMUTEX(&m_mutex);
	G_ItemBuff itBuff = {0};
	BuffData bfData = {0};
	if (CCItemScriptSetting::getSingleton().GetItemBuff(iID, itBuff))
	{
		UDM_IT it = m_userDataMap.begin();
		UDM_IT endIT = m_userDataMap.end();
		for (; it!=endIT; ++it)
		{
			bfData.iID = iID;
			bfData.iGroup = itBuff.iGroup;
			bfData.iDuration = itBuff.iDuration;
			bfData.toggleTime = time(0);
			it->second->m_userBuff.InsertData(bfData.iGroup, bfData);            
			it->second->UpdateFactorData();

			// 通知客户端
			CMD_G_UserItemBuff buff = {0};
			buff.dwUserID = it->second->GetUserID();
			buff.iGroup = bfData.iGroup;
			buff.iID = bfData.iID;
			buff.leftTime = bfData.iDuration;
			it->second->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_BUFF, (CT_CHAR *)&buff, sizeof(buff), false, true);
			it->second->UpdateAndSendMasterData(EMI_BATLLE);
		}
	}
}

//更新组队副本
CT_VOID CCUserManager::UpdataTeamFightingTimes()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userTeamDuplicate.UpdataFightingReward();
	}
	return;
}

//更新购买体力
CT_VOID CCUserManager::UpdataBuyStaminaTimes()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userBase.UpdataBuyStaminaTimes();
	}
	return;
}

//广播所有在在线用户
CT_VOID CCUserManager::SendUserMessage(CT_DWORD dwMainID,CT_DWORD dwSubID,CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->GetNetInterface()->Send(dwMainID,dwSubID,pBuf,iLen);
	}
	return;
}
CT_VOID CCUserManager::SendUserMessage(CT_DWORD dwMainID,CT_DWORD dwSubID)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->GetNetInterface()->Send(dwMainID,dwSubID);
	}
	return;
}

//更新个人限购
CT_VOID CCUserManager::UpdataMarketParsonal(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userBlackMarket.UpdataMarketPersonal(pBuf,iLen);
	}
	return;
}

//更新所有在线用户的公告
CT_VOID CCUserManager::UpdataUserAnnouncement(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT itEnd = m_userDataMap.end();

	CMD_G_ANNOUNCEMENT *pAnno = (CMD_G_ANNOUNCEMENT*)pBuf;

	for(;it != itEnd;++it)
	{
		if (pAnno->iIndex == ANNONCEMENT_TOWN)
		{
			it->second->GetNetInterface()->Send(MAIN_G_CHAT_CITY, SUB_G_ANNOUNCEMENT_UPDATA, pBuf, iLen);
		}
		else if (pAnno->iIndex == ANNONCEMENT_ACTIVE)
		{
			it->second->GetNetInterface()->Send(MAIN_G_CHAT_CITY, SUB_G_ACTIVENOTICE_UPDATA, pBuf, iLen);
		}
	}
	return;
}

//更新龙珠状态
CT_VOID CCUserManager::UpdataDragonBallStatus()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userActive.UpdataDragonBallStatus();
	}
	return;
}

//更新月卡活动状态
CT_VOID CCUserManager::UpdataMonthCardStatus()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userActive.UpdataMonthCardStatus();
	}
	return;
}

CT_VOID CCUserManager::InsertGMList(CT_DWORD dwUID)
{
	_AUTOMUTEX(&m_mutex);
	m_GMList.insert(dwUID);
}

CT_BOOL CCUserManager::IsGM(CT_DWORD dwUID)
{
	_AUTOMUTEX(&m_mutex);
	if (m_GMList.find(dwUID) != m_GMList.end())
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_VOID CCUserManager::ClearGMList()
{
	_AUTOMUTEX(&m_mutex);
	m_GMList.clear();
}

//查找竞技场用户是否引用
CT_BOOL CCUserManager::FindPVPUser(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	CCUserManager::UDM_IT UserMapEnd=m_userDataMap.end();
	for(CCUserManager::UDM_IT i=m_userDataMap.begin(); i != UserMapEnd;++i)
	{
		if((*i).second->m_userPVPInfo.GetDifficulty(dwUserID) != 0)
			return CT_TRUE;
	}
	return CT_FALSE;
}

//更新数据库
CT_VOID CCUserManager::DBUpdataData()
{
	_AUTOMUTEX(&m_mutex);

	CT_INT64 lTime=time(0);
	CCUserManager::UDM_IT UserMapEnd=m_userDataMap.end();
	for(CCUserManager::UDM_IT i=m_userDataMap.begin(); i != UserMapEnd;++i)
	{
		(*i).second->m_userBase.UpdataData(lTime);
	}
	return;
}

CT_VOID CCUserManager::VipTimeMonitor()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userVip.VipTimeMonitor();
	}
}

CT_VOID CCUserManager::VipGiftPush()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userVip.VipGiftPush();
	}
}

CT_VOID CCUserManager::GetOnlineUserNum()
{
	_AUTOMUTEX(&m_mutex);
	CT_DWORD threadid = 0;
	CT_INT32 iUserNum = 0;
	threadid = GAME_THREAD_LOGDATABASE ;
	iUserNum = m_userDataMap.size();
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ONLINEUSER,(CT_CHAR*)&iUserNum,sizeof(CT_INT32));
}

CT_VOID CCUserManager::ActiveAddPush(MSC_DB_ActiveEventChange &activeAdd)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (;it!=endIT;++it)
	{
		it->second->m_userActive.PushNewAddActive(activeAdd);
	}
}

CT_VOID CCUserManager::ActiveDeletePush(MSC_DB_ActiveEventChange &activeDel)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (;it!=endIT;++it)
	{
		it->second->m_userActive.PushDelActive(activeDel);
	}
}

//清退所有用户
CT_VOID CCUserManager::ExitAllUser()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (;it!=endIT;++it)
	{
		(*it).second->GetNetInterface()->Send(MAIN_G_ENTER_ENV,SUB_G_FORCE_EXIT);
		CMD_G_LeaveGame LeaveGame={0};
		LeaveGame.dwUserID= (*it).second->GetUserID();

		CT_DWORD dwThreadID=GAME_THREAD_CITY+(LeaveGame.dwUserID%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum);
		G_SendThreadInfo(dwThreadID,MSC_MAIN_USER_EXIT,0,(CT_CHAR*)&LeaveGame,sizeof(LeaveGame));
	}

	G_SendThreadInfo(GAME_THREAD_CITY, MSC_MAIN_RES_OCCUPY,    MSC_SUB_SYS_EXIT, 0, 0); 
	G_SendThreadInfo(GAME_THREAD_DATABASE, MSC_MAIN_DB_TOWER_DATA, MSC_SUB_DB_SYS_EXIT, 0, 0);
}

CT_VOID CCUserManager::GetOnlineUserData(CT_DWORD UserID, CT_CHAR *out_buf, CT_INT32 &bufLen, CT_INT32 iType)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();

	CMD_G_FriendData	friendData = {0};
	CMD_G_PlayerData	PlayerData = {0};
	MasterData masterData = {0};
	CT_INT32 n = 0;
	CT_DWORD F_UID = 0;
	CT_INT32 iCount = 50;

	CCUser *pUser=0;
	if (!Find(UserID,&pUser))
	{
		return;
	}

	PlayerData.dwUserID = UserID;
	friendData.dwUserID = UserID;

	G_ValueRange vr={0};
	if (iType == 2)
	{
		iCount = 100;	//因为要去掉已加入战队的玩家,所以增加50个.
		G_GetValueRange(VALUE_TYPE_FIGHTTEAM_GRADE,&vr);
	}
	for (; it!=endIT && n < iCount; ++it)
	{	
		F_UID = it->second->GetUserID();
		if (F_UID == UserID)	//去掉自己
		{
			continue;
		}
		if (iType == 1 && CT_FALSE == it->second->m_userFriend.IsEnable())
		{
			continue;
		}
		if (pUser->m_userFriend.CheckIsFriend(F_UID) || pUser->m_userFriend.CheckBlackList(F_UID))
		{
			// 去掉已经是好友,黑名单,反向黑名单
			continue;
		}
		it->second->m_userBase.GetMasterData(masterData);

 		if (iType == 2 && masterData.mvd.stGrade.iLevel < vr.fMaxVal)
 		{
 			continue;
 		}

		if (iType == 1)
		{
			friendData.dwF_UID = it->second->GetUserID();
			CCUser* pFUser;
			if (Find(friendData.dwF_UID, &pFUser))
			{
				friendData.iTitleID	= pFUser->m_userTitle.GetUseTitleID();
			}
			memcpy(friendData.cF_NickName, masterData.cNickName, NAME_SIZE);
			friendData.iF_Occupation	= masterData.iOccupation;
			friendData.iF_Grade			= masterData.mvd.stGrade.iLevel;
			friendData.iF_Status		= 1;
			friendData.iF_FightingPower	= masterData.mvd.stRes.iFightingPower;		
			memcpy(out_buf + (sizeof(CMD_G_FriendData) * n), (CT_CHAR*)&friendData, sizeof(CMD_G_FriendData));
		}
		else
		{
			PlayerData.dwPlayer = it->second->GetUserID();
			memcpy(PlayerData.cF_NickName, masterData.cNickName, NAME_SIZE);
			PlayerData.iF_Occupation	= masterData.iOccupation;
			PlayerData.iF_Grade			= masterData.mvd.stGrade.iLevel;
			PlayerData.iF_Status		= 1;
			PlayerData.iF_FightingPower	= masterData.mvd.stRes.iFightingPower;
			memcpy(out_buf + (sizeof(CMD_G_PlayerData) * n), (CT_CHAR*)&PlayerData, sizeof(CMD_G_PlayerData));
		}
		n++;
	}

	if (iType == 1)
	{
		bufLen = sizeof(CMD_G_FriendData) * n;
	}
	else
	{
		bufLen = sizeof(CMD_G_PlayerData) * n;
	}
}

CT_VOID CCUserManager::UpdateActiveOnline()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT endIT = m_userDataMap.end();
	for (; it!=endIT; ++it)
	{
		it->second->m_userActive.UpdateActiveOnline();
	}
}

// 更新头衔时限 
CT_VOID CCUserManager::UpdataTitleStatus()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT itEnd = m_userDataMap.end();
	for (;it != itEnd;++it)
	{
		it->second->m_userTitle.UpdataTitleStatus();
	}
}

//更新头衔列表
CT_VOID CCUserManager::UpdateTitleList()
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT itEnd = m_userDataMap.end();
	for (;it != itEnd;++it)
	{
		it->second->m_userTitle.UpdataTitleList();
	}
}

//判断用户是否在访问列表
CT_BOOL CCUserManager::IsInSeekedList(CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	UDM_IT it = m_userDataMap.begin();
	UDM_IT itEnd = m_userDataMap.end();
	for (;it != itEnd;++it)
	{
		if (it->second->m_userSeek.IsInSeekedList(dwUserID))
		{
			return CT_TRUE;
		}
	}

	return CT_FALSE;
}

//更新探访礼物
CT_VOID CCUserManager::UpdateSeekPresent()
{
	_AUTOMUTEX(&m_mutex);

	UDM_IT it = m_userDataMap.begin();
	UDM_IT itEnd = m_userDataMap.end();
	for (;it != itEnd;++it)
	{
		it->second->m_userSeek.UpdatePresent();
	}

	return ;
}

CT_VOID CCUserManager::UpdateSeekData()
{
	_AUTOMUTEX(&m_mutex);

	UDM_IT it = m_userDataMap.begin();
	UDM_IT itEnd = m_userDataMap.end();
	for (;it != itEnd;++it)
	{
		it->second->m_userSeek.UpdateAllSeekData();
	}

	return ;
}

// 更新排行榜奖励状态
CT_VOID CCUserManager::UpdateTopReward()
{
	_AUTOMUTEX(&m_mutex);
	
	MasterData md = {0};
	UDM_IT it = m_userDataMap.begin();
	UDM_IT itend = m_userDataMap.end();
	for (;it != itend;++it)
	{
		it->second->m_userBase.GetMasterData(md);
		for (CT_INT32 i = 0;i < TOP_TYPE_MAX;++i)
		{
			md.mvd.topData.TopInfo[i].iRewardStatus = 0;
			it->second->m_userBase.UpdateTopRewardStatus(i,TOP_REWARD_NOT_RECEIVE);
		}
	}
	return;
}