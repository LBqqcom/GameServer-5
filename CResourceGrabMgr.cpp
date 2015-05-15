/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCResourceGrabMgr.cpp
Version:		1.0
Description:    资源争夺系统
Author:			LinQ
Create Date:	2013-05-04
History:        资源争夺系统，管理器

******************************************************************/
#include "CResourceGrabMgr.h"
#include "CLogMeg.h"
#include "../sdk/include/CMutex.h"
#include "ProtocolGame.h"
#include "CUser.h"
#include "CThreadMes.h"
#include "ProtocolGame.h"
#include "../sdk/include/CNetInterface.h"
#include "../sdk/include/GlobeScriptInterface.h"
#include "../sdk/include/CApi.h"
#include "CUserManager.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CFightTeamManager.h"
#include "CActive.h"
#include "CBroadcastScriptSetting.h"


template<> CCResourceGrabMgr* CCSingleton<CCResourceGrabMgr>::ms_Singleton = 0;


CT_INT32 G_ResSpotMaxProduct      = 0;

CT_FLOAT G_ResSpotBossProductUp   = 0.0f;
CT_FLOAT G_ResSpotEliteProductUP  = 0.0f;

#define MAX_BUFFER_LEN				5120
CCResourceGrabMgr::CCResourceGrabMgr(void) : m_mutex()
{
	m_mapSpots.clear();
	m_mapResSpotFind.clear();
	m_mapSenceUser.clear();
	m_SceneList.clear();
	m_bResActive=CT_FALSE;
	m_lLastTiem=-1;

	G_ValueRange vg ={0};

	                     
	if (!G_GetValueRange(VALUE_TYPE_TEAM_MAX_RES_KEEP_TIME, &vg))
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError("没有找到全局配置 VALUE_TYPE_TEAM_MAX_RES_KEEP_TIME");
		return ;
	}
	G_ResSpotMaxProduct = (CT_INT32)vg.fMaxVal;

	memset(&vg, 0, sizeof(vg));
	if (!G_GetValueRange(VALUE_TYPE_TEAM_MEMBER_ELITE_PRODUCT_UP, &vg))
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError("没有找到全局配置 VALUE_TYPE_TEAM_MEMBER_ELITE_PRODUCT_UP");
		return ;
	}
	G_ResSpotEliteProductUP = vg.fMaxVal;

	memset(&vg, 0, sizeof(vg));
	if (!G_GetValueRange(VALUE_TYPE_TEAM_MEMBER_BOSS_PRODUCT_UP, &vg))
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError("没有找到全局配置 VALUE_TYPE_TEAM_MEMBER_BOSS_PRODUCT_UP");
		return ;
	}
	G_ResSpotBossProductUp = vg.fMaxVal;	 
	
	ProduceResourceSpots();	//从配置得到所有的资源点数据
}

CCResourceGrabMgr::~CCResourceGrabMgr(void)
{
	m_mapSpots.clear();
	m_mapResSpotFind.clear();
	m_mapSenceUser.clear();
	m_SceneList.clear();
}

// 从配置中读取所有的资源点恢复到内存
CT_VOID CCResourceGrabMgr::ProduceResourceSpots(CT_VOID)
{
	_AUTOMUTEX(&m_mutex);
	G_ResourceSpot mySpots[RESOURCEGRAB_SPOT_LENEX] = {0};
	int num = G_GetAllResourceSpot(mySpots, RESOURCEGRAB_SPOT_LENEX);	// 返回所有的资源点配置信息
	// 创建所有的资源点对象
	for (int i=0; i<num; ++i)
	{
		// 创建新的资源点对象
		ResourceSpot resourceSpot = {0};
		m_mapSpots[mySpots[i].iSceneID].insert(SpotMap::value_type(mySpots[i].iSpotID, resourceSpot));
	}
}

//得到资源点生产数据
CT_BOOL CCResourceGrabMgr::GetResourceQuality(CT_INT32 iScenceID, CT_INT32 iSpotID, G_ResourceQuality& resQuality,CT_INT32 &iResType)
{
	G_ResourceSpot resSpot = {0};
	if (!G_GetResourceSpot(iScenceID, iSpotID, &resSpot))
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError(" CCResourceGrabMgr::GetResourceQuality 没有找到资源点配置 sence=%i, spot=%i", iScenceID, iSpotID);
		return CT_FALSE;
	}

	if(!G_GetResourceQuality(resSpot.iQuality, &resQuality))
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError(" CCResourceGrabMgr::GetResourceQuality 没有找到资源品质 quality=%i",resSpot.iQuality);
		return CT_FALSE;
	}

	if(m_bResActive)
	{
		CT_INT64 lKey=iScenceID;
		lKey<<=32;
		lKey|=iSpotID;
		ResActiveMap::iterator it=m_ResActiveMap.find(lKey);
		if(it != m_ResActiveMap.end())
		{
			//活动开启
			//resQuality.iBaseNum=resQuality.iActiveBaseNum;
			//resQuality.fLevelRatio=resQuality.fActiveLevelRatio;
			//resQuality.fTimeRatio=resQuality.fActiveTimeRatio;
			resQuality.iInterval=(*it).second.iProductTime;
			iResType=(*it).second.iProductType;
			resQuality.iResID=(*it).second.iResID;

		}
	}
	return CT_TRUE;
}

//计算资源点生产
CT_INT32 CCResourceGrabMgr::GetResourceSpot(CT_INT32 iScenceID,CT_INT32 iSpot,G_ResourceQuality& resQuality,CT_INT32 iGrade)
{
	CT_INT32 iRes=0;
	if(!m_bResActive)
	{
		iRes=G_GetSpotProduct(resQuality.iBaseNum, iGrade, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval);
	}
	else
	{
		CT_INT64 lKey=iScenceID;
		lKey<<=32;
		lKey|=iSpot;
		ResActiveMap::iterator it=m_ResActiveMap.find(lKey);
		if(it != m_ResActiveMap.end())
		{
			if((*it).second.iProductNum != 0 && (*it).second.iProductType != CLIENT_OBJ_ITEM)
				iRes=(*it).second.iProductNum;
			else if((*it).second.iProductNum == 0 && (*it).second.iProductType != CLIENT_OBJ_ITEM)
			{
				iRes=G_GetSpotProduct(resQuality.iBaseNum, iGrade, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval);
				iRes *=(*it).second.fProductMultiple;
			}
			else if((*it).second.iProductType == CLIENT_OBJ_ITEM)
			{
				iRes=(*it).second.iProductNum;
			}
		}
	}
	return iRes;
}

//产量控制
CT_FLOAT CCResourceGrabMgr::GetProductionLimit(CT_INT32 iSceneID,CT_INT32 iGrade)
{
	G_ResProductionLimit rpl={0};
	if(!G_GetResProductionLimit(iSceneID,iGrade,&rpl))
	{
		rpl.fProduction=1.0f;
		CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::GetProductionLimit G_GetResProductionLimit SceneID:%d iGrade:%d",iSceneID,iGrade);
	}
	return rpl.fProduction;
}

//得到资源点区域数据
CT_BOOL CCResourceGrabMgr::GetResourceArea(CT_INT32 iScenceID, G_ResourceArea& resArea)
{
	G_ResourceScene resSc = {0};
	if (!G_GetResourceScene(iScenceID,  &resSc))
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError(" CCResourceGrabMgr::GetResourceArea 没有找到资源场景配置 sence=%i", iScenceID);
		return CT_FALSE;
	}


	if(!G_GetResourceArea(resSc.iAreaID, &resArea))
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError(" CCResourceGrabMgr::GetResourceArea 没有找到资源区域  areaID=%i", resSc.iAreaID );
		return CT_FALSE;
	}
	return CT_TRUE;
}


//保存用户金币数据
CT_VOID CCResourceGrabMgr::SaveUserProduct(CT_DWORD fid,  CT_DWORD uid, CT_INT32 iRes, CT_INT32 iResID,CT_INT32 iResType,CT_INT32 iSCID, CT_INT32 iSpot, ENUM_RES_Propduct_Reason iReason )
{
	if(iRes == 0)return;

	CCUser* pUser = 0;
	if (CCUserManager::getSingleton().Find(uid, &pUser))
	{
		MasterVariableData mvd ={0};
		
		CT_INT32 res = iRes;
		if(iResType == CLIENT_OBJ_RES || iResType == CLIENT_OBJ_RES_SPOT)
		{
			G_ChangeResType(mvd, iResID, res);
			pUser->m_userBase.AddMastData(mvd, SAVETAG_RESOURCE_PRODUCT);
		}
		else
		{
			CT_DWORD threadid = GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
			if(pUser->m_userItem.AutoAddItemData(SAVETAG_RESOURCE_PRODUCT,iResID,iRes,ITEM_OP_MODE_CHECK) != 0)
			{
				MSC_DB_RESREWARD rr={0};
				rr.dwUID=pUser->GetUserID();
				rr.iPID=iResID;
				rr.iPNum=iRes;
				threadid = GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_RES_PRODUCT_REWARD,(CT_CHAR*)&rr,sizeof(rr));
				printf("CCResourceGrabMgr::SaveUserProduct rr.iPNum:%d\n",rr.iPNum);
			}
			else
				pUser->m_userItem.AutoAddItemData(SAVETAG_RESOURCE_PRODUCT,iResID,iRes,ITEM_OP_MODE_EXEC);
		}
		

	}else 
	{
		MSC_DB_MasterSaveData msd = {0};
		msd.dwUserID = uid;
		
		CT_INT32 res = iRes;
		if((iResType == CLIENT_OBJ_RES || iResType == CLIENT_OBJ_RES_SPOT) && G_ChangeResType(msd.mvd,iResID, res) )
		{
			msd.iSaveTag = SAVETAG_RESOURCE_PRODUCT;
			G_SaveMastvariableData(msd);
		}
		else if(iResType == CLIENT_OBJ_ITEM)
		{
			MSC_DB_RESREWARD rr={0};
			rr.dwUID=uid;
			rr.iPID=iResID;
			rr.iPNum=iRes;
			CT_DWORD threadid = GAME_THREAD_DATABASE + uid % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_RES_PRODUCT_REWARD,(CT_CHAR*)&rr,sizeof(rr));
		}
	}

	MSC_DB_ResourceProductRecord resProductRecord = {0};
	resProductRecord.dwFID = fid;
	resProductRecord.dwUID = uid;
	//resProductRecord.iGold = iGold;
	resProductRecord.iRes=iRes;
	resProductRecord.iResType=iResID;
	resProductRecord.iScenceID= iSCID;
	resProductRecord.iSpotID    = iSpot;
	resProductRecord.iReason    = iReason;

	CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + uid % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadID,  MSC_MAIN_LOGDB_DATA, MSC_SUB_DBLOG_RES_PRODUCT_SAVE, (CT_CHAR*)&resProductRecord, sizeof(resProductRecord));
}

//得到用户资源信息
CT_VOID CCResourceGrabMgr::GetResourceMes(CT_DWORD dwUserID, const ResourceSpot& resSpot,const CT_INT32 iResID, const CT_INT32 iScenceID,CMD_G_ResSpotInfo& rsi)
{
	rsi.dwUserID  =  dwUserID;
	MasterVariableData mvd={0};
	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::GetResourceMes No Find UserID:%d",dwUserID);
	}
	else
	{
		pUser->m_userBase.GetMasterData(mvd);
	}

	//得到战队信息
	CCFightTeam* pTeam = 0;
	if (CCFightTeamManager::getSingleton().Find(resSpot.resBase.dwFTID, &pTeam))
	{
		rsi.teamInfo.dwTeamID  = resSpot.resBase.dwFTID;
		pTeam->GetFightTeamName(rsi.teamInfo.cName);
		//需要读取家族中驻守人员的信息
		for (CT_INT32 i=0; i< SPOT_GUARD_MEMBER; ++i)
		{
			CMD_G_FightTeamMember fightTeamMember = {0};
			if(pTeam->GetFightTeamMember(1, resSpot.guardTag.dwUserID[i], &fightTeamMember))
			{
				memcpy(rsi.dependPlayers[i].cNickName, fightTeamMember.cNickName, NAME_SIZE);
				rsi.dependPlayers[i].dwUserID		= resSpot.guardTag.dwUserID[i];
				rsi.dependPlayers[i].iFightingPower = fightTeamMember.iFightingPower;
				rsi.dependPlayers[i].iOccupation    = fightTeamMember.iOccupation;
			}
		}           
	}
	             
	
	rsi.iLeftProtectTime = resSpot.resBase.iLeftProtectTime;                    
	memcpy(rsi.cVictorySign, resSpot.resBase.szNote, SPOT_VECTORY_SIGN_SIZE);
	rsi.iLeftOccupyTime= resSpot.resBase.iLeftOccupyTime;                     


	//G_ResourceSpot rs={0};
	//if(!G_GetResourceSpot(iScenceID,iResID,&rs))
	//{
	//	CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::GetResourceMes G_GetResourceSpot");
	//	return;
	//}

	//G_ResourceQuality rq={0};
	//if(!G_GetResourceQuality(rs.iQuality,&rq))
	//{
	//	CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::GetResourceMes G_GetResourceQuality");
	//	return;
	//}

	CT_INT32 iResType=CLIENT_OBJ_RES;
	G_ResourceQuality rq={0};
	if(!GetResourceQuality(iScenceID,iResID,rq,iResType))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::GetResourceMes GetResourceQuality");
		return;
	}

	rsi.iResType = iResType;
	rsi.iResID = rq.iResID;
	for (CT_INT32 count=0; count<CLAN_MAX_MEMBER; ++count)
	{
		if (resSpot.resProduct[count].dwUserID == rsi.dwUserID)
		{
			//CT_INT32 iRes=G_GetSpotProduct(rq.iBaseNum,resSpot.resProduct->iGrade,rq.fLevelRatio,rq.fTimeRatio,rq.iInterval)*resSpot.resProduct[count].iProductCount;
			CT_INT32 iRes=GetResourceSpot(iScenceID,iResID,rq,resSpot.resProduct->iGrade)*resSpot.resProduct[count].iProductCount;
			if(pTeam != 0)
				iRes=CalFightTeamType(pTeam, iRes, rsi.dwUserID )*GetProductionLimit(iScenceID,mvd.stGrade.iLevel);

			rsi.iLeftProductTime = resSpot.resProduct[count].iLeftProductTime;
			//rsi.iSumaryCount     = resSpot.resProduct[count].iProductCount;
			rsi.iOutput = iRes;
			rsi.iStatuts         = resSpot.resProduct[count].iStatus;
		}
	}
}


// 从数据库中找回资源点的信息；恢复到资源掠夺管理器
CT_VOID CCResourceGrabMgr::RecoverResourceSpot(CT_INT32 iScenceID, CT_INT32 iSpotID, ResourceSpot& spot)
{
	_AUTOMUTEX(&m_mutex);
	// 先找到这个资源点
	SenceMap::iterator it = m_mapSpots.find(iScenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpotID);
		if (itt != (*it).second.end() && spot.resBase.dwFTID != 0)
		{
			memcpy( &((*itt).second), &spot, sizeof(spot) );
			AddSpotToFTeam(iScenceID,  iSpotID, (*itt).second.resBase.dwFTID );
		}
	
	}else
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::RecoverResourceSpot : 没有发现场景，可能是脚本配置问题 sceneID=%i,spotID=%i", iScenceID, iSpotID);
	}

	
	for (ResSpotFindMap::iterator findIt = m_mapResSpotFind.begin();  findIt != m_mapResSpotFind.end(); ++findIt)
	{
		CCFightTeam* pFightTeam = 0;
		if (CCFightTeamManager::getSingleton().Find((*findIt).first, &pFightTeam )  )
		{	
			pFightTeam->SetResCount(GetResSpotNum((*findIt).first ) );
		}
	}
}

CT_VOID CCResourceGrabMgr::SetResourceScene(CT_INT32 iSceneID)
{
	_AUTOMUTEX(&m_mutex);
	m_SceneList.insert(iSceneID);
}

CT_BOOL CCResourceGrabMgr::GetResourceScene(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(&m_mutex);
	if (m_SceneList.size() * sizeof(CT_INT32) > bufLen)
	{
		return CT_FALSE;
	}

	CT_INT32 iSceneID = 0;
	std::set<CT_INT32>::iterator it = m_SceneList.begin();
	for (CT_INT32 i=0; it != m_SceneList.end(); ++it,++i)
	{
		iSceneID = *it;
		memcpy(pBuf + i*sizeof(CT_INT32), &iSceneID, sizeof(CT_INT32));
	}
	bufLen = m_SceneList.size() * sizeof(CT_INT32);
	return CT_TRUE;
}

//用户进入场景信息
CT_VOID CCResourceGrabMgr::EnterSence(CT_INT32 iSenceID, CCUser* pUser)
{
	_AUTOMUTEX(&m_mutex);
	// 先找到这个资源点
	CT_CHAR  buf[1024*10] = {0}; //10k缓冲区
	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.begin();
		SpotMap::iterator ittEnd = (*it).second.end();
		
		for (CT_INT32 i=0;itt != ittEnd; ++itt, ++i )
		{
			CMD_G_ResSpotInfo rsi = {0};
			rsi.iScenceID  = (*it).first;
			rsi.iSpotID    = (*itt).first;
			GetResourceMes(pUser->GetUserID(), (*itt).second,(*itt).first,iSenceID, rsi);
			if(1024*10 > (i+1)*sizeof(CMD_G_ResSpotInfo))
				memcpy(buf + i*sizeof(CMD_G_ResSpotInfo), &rsi, sizeof(rsi));
			else
				CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::EnterSence 1024*10 > (i+1)*sizeof(CMD_G_ResSpotInfo)");
		}
		
		m_mapSenceUser[iSenceID].insert(pUser->GetUserID());
		pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,SUB_G_RESSPOT_SCENCE_INFO, buf, ((*it).second.size())*sizeof(CMD_G_ResSpotInfo));
	}
}

//增加资源到战队
CT_VOID CCResourceGrabMgr::AddSpotToFTeam(CT_INT32 iScenceID, CT_INT32 iSpot, CT_DWORD fTeamID)
{
	_AUTOMUTEX(&m_mutex);
	CT_LONGLONG  sp =  G_Combine64( iScenceID,  iSpot);
	m_mapResSpotFind.insert(ResSpotFindMap::value_type(fTeamID, sp));
}

//删除资源到战队
CT_VOID CCResourceGrabMgr::EaseSpotFromFTeam(CT_INT32 iScenceID, CT_INT32 iSpot, CT_DWORD fTeamID)
{
	_AUTOMUTEX(&m_mutex);
	CT_LONGLONG ll = G_Combine64(iScenceID, iSpot);
	ResSpotFindMap::iterator itBegin = m_mapResSpotFind.lower_bound(fTeamID);
	ResSpotFindMap::iterator itEnd   = m_mapResSpotFind.upper_bound(fTeamID);
	ResSpotFindMap::iterator itCp    = itBegin;
	for (;itBegin != itEnd; )
	{	
		itCp = itBegin;
		++itCp;
		if (ll == (*itBegin).second )
		{
			m_mapResSpotFind.erase(itBegin);
		}	
		itBegin = itCp;
	}
}

//用户退出
CT_VOID CCResourceGrabMgr::UserExit(CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	SenceUserMap::iterator it = m_mapSenceUser.begin();
	SenceUserMap::iterator itEnd = m_mapSenceUser.end();
	for (; it!=itEnd; ++it)
	{
		SenceUserSet::iterator itt = (*it).second.find(dwUserID);
		if (itt != (*it).second.end())
		{
			(*it).second.erase(itt);
		}
	}
}

//同步生产信息
CT_VOID CCResourceGrabMgr::SysResourceProduct(const MSC_DB_ResourceProduct& rp)
{
	_AUTOMUTEX(&m_mutex);
	if (rp.iIndex < 0 || rp.iIndex >= CLAN_MAX_MEMBER)
	{
		//CT_ASSERT(false);
		CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::SysResourceProduct: 发现一个用户资源生产索引错误 uid=%i, iIndex=%i", rp.dwUserID, rp.iIndex);
		return;
	}

	SenceMap::iterator it = m_mapSpots.find(rp.iSenceID);
	if (it != m_mapSpots.end() && rp.dwUserID != 0)
	{
		SpotMap::iterator itt = (*it).second.find(rp.iSpotID);
		
		if (itt != (*it).second.end())
		{
			G_ResourceScene rs = {0};
			if (!G_GetResourceScene(rp.iSenceID, &rs))
			{
				CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::SysResourceProduct: 没有发现这个资源场景 uid=%i, senceID=%i,", rp.dwUserID,  rp.iSenceID);
				return;
			}

			G_ResourceArea   ra = {0};
			if (!G_GetResourceArea(rs.iAreaID, &ra ))
			{
				CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::SysResourceProduct: 没有发现这个资源区 uid=%i, iAreaID=%i,",  rp.dwUserID, rs.iAreaID );
			    return;
			}

			ResourceSpot& rspot = (*itt).second;
			rspot.resProduct[rp.iIndex].dwUserID =  rp.dwUserID;
			rspot.resProduct[rp.iIndex].iGrade   =	rp.iGrade;
			
			rspot.resProduct[rp.iIndex].iLeftProductTime = rp.iLeftProductTime;
			rspot.resProduct[rp.iIndex].iProductCount    = rp.iProductCount;
			rspot.resProduct[rp.iIndex].iStatus          = (ENUM_RES_SPOT_STATUS)rp.iProductStatus;
		}
	}
}

//战队得到拥有的资源点
CT_INT32 CCResourceGrabMgr::TeamFindResSpot(CT_DWORD teamID, ResSpotTag& rst)
{
	_AUTOMUTEX(&m_mutex);
	ResSpotFindMap::iterator itBegin = m_mapResSpotFind.lower_bound(teamID);
	ResSpotFindMap::iterator itEnd   = m_mapResSpotFind.upper_bound(teamID);

	for (CT_INT32 i=0; itBegin != itEnd; ++itBegin, ++i)
	{
		if (i < SPOT_RES_SIZE)
		{
			CT_DWORD main = 0;
			CT_DWORD sub  = 0;
			G_Separate64((*itBegin).second, main, sub);
			rst.resSpot[i].iSceneID = (CT_INT32)main;
            rst.resSpot[i].iSpotID  = (CT_INT32)sub;
		}else
		{
			//CT_ASSERT(i < SPOT_RES_SIZE);
            CCLogMeg::getSingleton().WriteError("CCUserResourceGrab::TeamFindResSpot: 发现战队资源点数量过多 team=%i, spot=%i", teamID, i);
		}
	}
	return m_mapResSpotFind.count(teamID);
}

//战队的得到资源点上守卫
CT_BOOL CCResourceGrabMgr::TeamFindResSpotGuard(CT_INT32 iSenceID, CT_INT32 iSpot, GuardTag& gt)
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpot);
		if (itt != (*it).second.end())
		{
			gt =  (*itt).second.guardTag;
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//设置战队在资源点上的守卫
CT_BOOL CCResourceGrabMgr::TeamSetResSpotGuard(CCFightTeam* pFightTeam,CT_INT32 iSenceID, CT_INT32 iSpot, GuardTag& gt)
{
	_AUTOMUTEX(&m_mutex);
	//G_ResourceSpot resSpot = {0};
	//if(!G_GetResourceSpot(iSenceID, iSpot, &resSpot))
	//{
	//	return CT_FALSE;
	//}

	//G_ResourceQuality resQuality = {0};
	//if(!G_GetResourceQuality(resSpot.iQuality, &resQuality))
	//{
	//	return CT_FALSE;
	//}

	CT_INT32 iResType=CLIENT_OBJ_RES;
	G_ResourceQuality resQuality={0};
	if(!GetResourceQuality(iSenceID,iSpot,resQuality,iResType))
	{
		return CT_FALSE;
	}

	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpot);
		if (itt != (*it).second.end())
		{
			(*itt).second.guardTag = gt;
			for (CT_INT32 i=0; i<SPOT_GUARD_MEMBER; ++i)
			{
				if (gt.dwUserID[i] != 0)
				{
					CMD_G_FightTeamEventLog eventLog = {0};
					RollBroadcast rb[2]={0};
					eventLog.lDate   = time(0);
					CT_CHAR buf[NAME_SIZE] = {0};
					pFightTeam->GetMemberName(gt.dwUserID[i], buf);
					rb[0].SetValue(0,buf);
					rb[1].SetValue(0,resQuality.cName);
					//sprintf(eventLog.cEventDesc, "{24}<%s>{25}%s{26}", buf, resQuality.cName);
					G_GetRollBroadecastData(112,eventLog.cEventDesc,sizeof(eventLog.cEventDesc),(CT_CHAR*)rb,sizeof(rb));
					pFightTeam->AddEventLog(eventLog);
				}
			}
			return CT_TRUE;
		}
	}

	

	return CT_FALSE;
}

//清除生产信息 用户被首领踢出队伍时候调用
CT_VOID CCResourceGrabMgr::TeamKickOutUser(CT_DWORD teamID, CT_DWORD uid)
{
	_AUTOMUTEX(&m_mutex);
	ResSpotTag resSpotTag = {0};
	if(TeamFindResSpot(teamID, resSpotTag) == 0)
	{
		return;
	}

	for (CT_INT32 i=0; i<SPOT_RES_SIZE; ++i)
	{
		SenceMap::iterator it = m_mapSpots.find( resSpotTag.resSpot[i].iSceneID);
		if (it != m_mapSpots.end())
		{
			SpotMap::iterator itt = (*it).second.find( resSpotTag.resSpot[i].iSpotID);
			if (itt != (*it).second.end())
			{
				CT_INT32 iResType=CLIENT_OBJ_RES;
				G_ResourceQuality resQuality = {0};
				if(!GetResourceQuality(resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID, resQuality,iResType))
				{
					return;
				}

				CT_INT32 iRes = 0;
				for (CT_INT32 j=0; j<CLAN_MAX_MEMBER; ++j)
				{
					if ( (*itt).second.resProduct[j].dwUserID == uid)
					{
						if ( (*itt).second.resProduct[j].iProductCount > 0)
						{
							//iRes += G_GetSpotProduct(resQuality.iBaseNum, (*itt).second.resProduct[j].iGrade, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval  )* (*itt).second.resProduct[j].iProductCount;
							iRes +=GetResourceSpot(resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID,resQuality,(*itt).second.resProduct[j].iGrade)* (*itt).second.resProduct[j].iProductCount;
						}
						memset(&((*itt).second.resProduct[j]), 0, sizeof(ResourceProduct));
					}
				}
				SaveUserProduct(teamID, uid, iRes,resQuality.iResID,iResType, resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID, ENUM_PRODUCT_REASON_KICK_OUT);
				

				CT_DWORD dwUserID[SPOT_GUARD_MEMBER] = {0};
				for (CT_INT32 j=0,k=0; j< SPOT_GUARD_MEMBER ; ++j)
				{
					if((*itt).second.guardTag.dwUserID[j] == uid)
					{
						(*itt).second.guardTag.dwUserID[j] = 0;
					}

					if ((*itt).second.guardTag.dwUserID[j] != 0)
					{
						dwUserID[k++] = (*itt).second.guardTag.dwUserID[j]; 
					}
				}
				memcpy((*itt).second.guardTag.dwUserID, dwUserID, sizeof(dwUserID));

				CCFightTeam*  pFightTeam = 0;
				if (CCFightTeamManager::getSingleton().Find(teamID, &pFightTeam))
				{
					BroadcastSenceGuard(resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID,  pFightTeam, (*itt).second.resBase.szNote, (*itt).second.guardTag.dwUserID);
				}
			}
			
		}
	}
}

//战队解散
CT_VOID CCResourceGrabMgr::TeamDissolution(CT_DWORD teamID)
{
	_AUTOMUTEX(&m_mutex);
	ResSpotTag resSpotTag = {0};
	if(TeamFindResSpot(teamID, resSpotTag) == 0)
	{
		return;
	}

	for (CT_INT32 i=0; i<SPOT_RES_SIZE; ++i)
	{
		EaseSpotFromFTeam(resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID, teamID);
	}
	

	for (CT_INT32 i=0; i<SPOT_RES_SIZE; ++i)
	{
		SenceMap::iterator it = m_mapSpots.find( resSpotTag.resSpot[i].iSceneID);
		if (it != m_mapSpots.end())
		{
			SpotMap::iterator itt = (*it).second.find( resSpotTag.resSpot[i].iSpotID);
			if (itt != (*it).second.end())
			{
				CT_INT32 iResType=CLIENT_OBJ_RES;
				G_ResourceQuality resQuality = {0};
				if(!GetResourceQuality(resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID, resQuality,iResType))
				{
					return;
				}

				for (CT_INT32 j=0; j<CLAN_MAX_MEMBER; ++j)
				{
					if ( (*itt).second.resProduct[j].dwUserID != 0)
					{
						if ( (*itt).second.resProduct[j].iProductCount > 0)
						{
							//CT_INT32 iRes  = G_GetSpotProduct(resQuality.iBaseNum, (*itt).second.resProduct[j].iGrade, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval ) * (*itt).second.resProduct[j].iProductCount;
							CT_INT32 iRes  = GetResourceSpot(resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID,resQuality,(*itt).second.resProduct[j].iGrade) * (*itt).second.resProduct[j].iProductCount;
							SaveUserProduct(teamID, (*itt).second.resProduct[j].dwUserID, iRes, resQuality.iResID,iResType,
											resSpotTag.resSpot[i].iSceneID, resSpotTag.resSpot[i].iSpotID, ENUM_PRODUCT_REASON_DISSOLUTION);
						}
					}
				}
				memset(&((*itt).second), 0, sizeof(ResourceSpot));
			}
		}
	}
}



//得到目前战队占领的资源点数量
CT_INT32 CCResourceGrabMgr::GetResSpotNum(CT_DWORD iFightTeamID)
{
	_AUTOMUTEX(&m_mutex);
	return	m_mapResSpotFind.count(iFightTeamID);
}

//清空资源点信息
CT_VOID  CCResourceGrabMgr::ResetResourceInfo(CT_INT32 iSenceID, CT_INT32 iSpot)
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpot);
		if (itt != (*it).second.end() )
		{	
			memset(&((*itt).second), 0, sizeof(ResourceSpot));
		}
	}
}

//得到资源保护时间
CT_BOOL CCResourceGrabMgr::GetResourceBase(CT_INT32 iSenceID, CT_INT32 iSpot, ResourceBase& protecBase)
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpot);
		if (itt != (*it).second.end() )
		{
			protecBase = (*itt).second.resBase;		
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//设置资源基本信息
CT_BOOL  CCResourceGrabMgr::SetResourceBase(CT_INT32 iSenceID, CT_INT32 iSpot, const ResourceBase& protecTime)
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpot);
		if (itt != (*it).second.end() )
		{
			(*itt).second.resBase = protecTime;		
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//得到目前的资源点上该用户生产状态
CT_BOOL  CCResourceGrabMgr::GetResProduct(CT_DWORD dwUserID, CT_INT32 iSenceID, CT_INT32 iSpot,ResourceProduct& rp)
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpot);
		if (itt != (*it).second.end())
		{
			for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i)
			{
				if ((*itt).second.resProduct[i].dwUserID == dwUserID)
				{
					rp = (*itt).second.resProduct[i];
					return CT_TRUE;
				}
			}
		}
	}
	return CT_FALSE;
}

//是否已经被占领
CT_BOOL  CCResourceGrabMgr::DoResSpotOccupy(CT_DWORD iFightTeamID, CT_INT32 iSenceID, CT_INT32 iSpot)
{
	_AUTOMUTEX(&m_mutex);
	ResSpotFindMap::iterator it = m_mapResSpotFind.find(iFightTeamID);
	if (it == m_mapResSpotFind.end())
	{
		return CT_FALSE;
	}

	ResSpotFindMap::iterator itBegin = m_mapResSpotFind.lower_bound(iFightTeamID);
	ResSpotFindMap::iterator itEnd   = m_mapResSpotFind.upper_bound(iFightTeamID);

	for (; itBegin != itEnd; ++itBegin)
	{
		if ((*itBegin).second  == G_Combine64(iSenceID, iSpot))
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//资源点生产
CT_BOOL  CCResourceGrabMgr::SpotProduct(CCUser* pUser, CT_INT32 iScenceID, CT_INT32 iSpotID, CT_DWORD dwTeamID)
{
	_AUTOMUTEX(&m_mutex);
	CT_DWORD dwUserID = pUser->GetUserID();
	SenceMap::iterator it = m_mapSpots.find(iScenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpotID);
		if (itt != (*it).second.end())
		{
			CT_INT32 iPos = -1;
			if (!FindProductPos((*itt).second.resProduct, dwUserID, iPos))
			{
				if (!FindProductPos((*itt).second.resProduct, 0, iPos))
				{
					for (CT_INT32 i=0; i<CLAN_MAX_MEMBER && iPos !=0; ++i)
					{
						if (CCFightTeamManager::getSingleton().GetFightTeamIDByUser((*itt).second.resProduct[i].dwUserID) != dwTeamID)
						{
							iPos = i;
						}
					}
					if (iPos == -1)
					{
						return CT_FALSE;
					}
				}
			}

			CT_INT32 iResType=CLIENT_OBJ_RES;
			G_ResourceQuality resQua = {0};
			if (!GetResourceQuality(iScenceID,  iSpotID, resQua,iResType))
			{
				return CT_FALSE;
			}

			(*itt).second.resProduct[iPos].iLeftProductTime       = resQua.iInterval;
			(*itt).second.resProduct[iPos].dwUserID               = dwUserID;
			(*itt).second.resProduct[iPos].iProductCount          = 0;
			(*itt).second.resProduct[iPos].iStatus                = SPOT_PRODUCTING;

			MasterData md = {0};
			pUser->m_userBase.GetMasterData(md);

			CMD_G_ResSpotProduct rsp = {0};
			rsp.dwUserID         = dwUserID;
			rsp.iLeftProductTime = (*itt).second.resProduct[iPos].iLeftProductTime;
			rsp.iScenceID        = iScenceID;
			rsp.iSpotID          = iSpotID;
			rsp.iStatuts         = (*itt).second.resProduct[iPos].iStatus;
			//rsp.iSumaryCount     = (*itt).second.resProduct[iPos].iProductCount;
			rsp.iResType		 = iResType;
			rsp.iResID			= resQua.iResID;
			//rsp.iOutput		     = G_GetSpotProduct(resQua.iBaseNum,md.mvd.stGrade.iLevel,resQua.fLevelRatio,resQua.fTimeRatio,resQua.iInterval)*(*itt).second.resProduct[iPos].iProductCount;
			rsp.iOutput			 = GetResourceSpot(iScenceID,iSpotID,resQua,md.mvd.stGrade.iLevel)*(*itt).second.resProduct[iPos].iProductCount;
			
			pUser->CalMaxWorker();
			pUser->CountFreeWorker();

			
			rsp.iMaxWorker       =  md.mvd.stRes.iMaxWorker;             //必须在后面
			rsp.iWorker          =  md.mvd.stRes.iWorker;
			(*itt).second.resProduct[iPos].iGrade = md.mvd.stGrade.iLevel;

			pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_PRODUCT_APPLY_SUC, (CT_CHAR*)&rsp, sizeof(rsp) );

			CMD_G_TaskEventInfo taskEventInfo = {0};
			taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_CITY;
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_CITY_PRODUCT;
			taskEventInfo.iSubType = 1003;				//跟小刚约定,资源点生产编号为1003
			taskEventInfo.iLevel = TASK_SUB_UN_CASE_VALUE;
			pUser->m_userTask.DisposeTask(taskEventInfo);

			return CT_TRUE;
		}
	}

	CMD_G_ResSpotApplyLost rsa = {0};
	rsa.dwUserID  = dwUserID;
	rsa.iScenceID = iScenceID;
	rsa.iSpotID   = iSpotID;
	rsa.iLostCode = SPOT_PRODUCT_LOSE_RES_NOT_FIND;
	pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_PRODUCT_APPLY_LOST, (CT_CHAR*)&rsa, sizeof(rsa));        //请求生产失败	
	return CT_FALSE;
}

//资源抢夺
CT_INT32 CCResourceGrabMgr::ResSpotOccupy(CT_INT32 iSenceID, CT_INT32 iSpot, CT_DWORD iTeamID)
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.find(iSenceID);
	if (it != m_mapSpots.end())
	{
		G_ResourceScene resSence = {0};
		if (!G_GetResourceScene(iSenceID, &resSence))
		{
			return 0;
		}

		G_ResourceArea ra = {0};
		if (!G_GetResourceArea(resSence.iAreaID, &ra))
		{
			return 0;
		}

		CT_INT32 iTatolGold = 0;
		SpotMap::iterator itt = (*it).second.find(iSpot);
		if (itt != (*it).second.end() )
		{
			CT_INT32 iResType=CLIENT_OBJ_RES;
			G_ResourceQuality resQuality = {0};
			if(!GetResourceQuality(iSenceID, iSpot, resQuality,iResType))
			{
				return 0;
			}
			
			for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i)
			{
				if ( (*itt).second.resProduct[i].iProductCount > 0)
				{
					//CT_INT32 iRes  = G_GetSpotProduct(resQuality.iBaseNum, (*itt).second.resProduct[i].iGrade, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval ) * (*itt).second.resProduct[i].iProductCount;
					CT_INT32 iRes  = GetResourceSpot(iSenceID, iSpot,resQuality,(*itt).second.resProduct[i].iGrade) * (*itt).second.resProduct[i].iProductCount;
					CT_FLOAT fOccupy =  ra.iGrabRate / 100.0f;
					iRes = iRes*GetProductionLimit(iSenceID,(*itt).second.resProduct[i].iGrade);
					
					CT_INT32 iOccupyRes = (CT_INT32)fOccupy*iRes;
					iTatolGold   = (CT_INT32)((1 - fOccupy)*iRes);
					SaveUserProduct((*itt).second.resBase.dwFTID, (*itt).second.resProduct[i].dwUserID, iRes,resQuality.iResID,iResType, iSenceID, iSpot, ENUM_PRODUCT_REASON_OCCUPY);
				}
			}

			CT_DWORD dwOldFTID = (*itt).second.resBase.dwFTID;
			CCFightTeam* pOldFightTeam = 0;
			EaseSpotFromFTeam((*it).first, (*itt).first, dwOldFTID);
			memset( &((*itt).second), 0, sizeof(ResourceSpot));

			//设置战队的资源数量
			if (CCFightTeamManager::getSingleton().Find(dwOldFTID, &pOldFightTeam))
			{
				ResSpotTag rst={0};	
				pOldFightTeam->SetResCount(TeamFindResSpot(pOldFightTeam->GetFightTeamID(), rst) );
			}
	

			(*itt).second.resBase.dwFTID           = iTeamID;
			(*itt).second.resBase.iLeftOccupyTime  = G_ResSpotMaxProduct;
			(*itt).second.resBase.iLeftProtectTime = (m_bResActive == CT_TRUE?ra.iProtectTimeACTV:ra.iProtectTime);
			AddSpotToFTeam((*it).first, (*itt).first, iTeamID);

			{
				//设置战队的资源数量
				CCFightTeam* pNewFightTeam = 0;
				if (CCFightTeamManager::getSingleton().Find(iTeamID, &pNewFightTeam))
				{
					ResSpotTag rst={0};	
					pNewFightTeam->SetResCount(TeamFindResSpot(pNewFightTeam->GetFightTeamID(), rst) );

					if (resQuality.iQuality%10 >= 3) //增加品质条件
					{
						CT_CHAR cFightTeamName[NAME_SIZE] = {0};
						pNewFightTeam->GetFightTeamName(cFightTeamName);
						if (CCFightTeamManager::getSingleton().Find(dwOldFTID, &pOldFightTeam))
						{
							RollBroadcast rb[4]={0};
							CT_CHAR cOldFightTeamName[NAME_SIZE] = {0};
							pOldFightTeam->GetFightTeamName(cOldFightTeamName);
							rb[0].SetValue(0,cFightTeamName);
							rb[1].SetValue(0,cOldFightTeamName);
							rb[2].SetValue(iSenceID,"");
							rb[3].SetValue(iSenceID*1000+iSpot,"");
							//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<6:%s>{38}<6:%s>{42}{16}<7:%d>{17}<8:%d>{18}", cFightTeamName,cOldFightTeamName,iSenceID,iSenceID*1000+iSpot);
							G_SendRollBroadecast(127,4,60,(CT_CHAR*)rb,sizeof(rb));
						}
						else
						{
							RollBroadcast rb[4]={0};
							rb[0].SetValue(0,cFightTeamName);
							rb[1].SetValue(0,"");
							rb[2].SetValue(iSenceID,"");
							rb[3].SetValue(iSenceID*1000+iSpot,"");
							//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<6:%s>{38}<13:%d>{16}<7:%d>{17}<8:%d>{18}", cFightTeamName,0,iSenceID,iSenceID*1000+iSpot);
							G_SendRollBroadecast(128,4,60,(CT_CHAR*)rb,sizeof(rb));
						}
					}
				}
			}

		}
		return iTatolGold;
	}
	return 0;
}


//查找资源点
CT_BOOL CCResourceGrabMgr::FindSpot(CT_INT32 iScenceID, CT_INT32 iSpotID)
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.find(iScenceID);
	if (it != m_mapSpots.end())
	{
		SpotMap::iterator itt = (*it).second.find(iSpotID);
		if (itt != (*it).second.end())
		{
             return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//用户退出场景
CT_VOID CCResourceGrabMgr::UserExitScence(CT_INT32 iScenceID, CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	SenceUserMap::iterator it = m_mapSenceUser.find(iScenceID);
	{
		SenceUserSet::iterator  itt = (*it).second.find(dwUserID);
		if (itt != (*it).second.end())
		{
			(*it).second.erase(dwUserID);
		}
	}
}

//战队放弃资源点
CT_VOID CCResourceGrabMgr::TeamGiveUpSpot( const CMD_G_ResSpotApply& rsa)
{
	_AUTOMUTEX(&m_mutex);
	CT_BOOL bFind = CT_FALSE;
	SenceMap::iterator it = m_mapSpots.find(rsa.iScenceID);
	{
		SpotMap::iterator  itt = (*it).second.find(rsa.iSpotID);
		if (itt != (*it).second.end())
		{
			CT_INT32 iResType=CLIENT_OBJ_RES;
			G_ResourceQuality resQuality = {0};
			if(!GetResourceQuality(rsa.iScenceID, rsa.iSpotID, resQuality,iResType))
			{
				return;
			}
		
			//记录日志
			CCFightTeam* pFightTeam = 0;
			if (CCFightTeamManager::getSingleton().Find(rsa.dwTeamID, &pFightTeam))
			{
				CMD_G_FightTeamEventLog eventLog = {0};
				RollBroadcast rb={0};
				eventLog.lDate   = time(0);
				rb.SetValue(0,resQuality.cName);
				//sprintf(eventLog.cEventDesc, "{6}%s", resQuality.cName);
				G_GetRollBroadecastData(113,eventLog.cEventDesc,sizeof(eventLog.cEventDesc),(CT_CHAR*)&rb,sizeof(rb));
				pFightTeam->AddEventLog(eventLog);
			}

			CT_BOOL bUserProduct = CT_FALSE;
			//放弃后的生产结算
			for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i )
			{
				if ((*itt).second.resProduct[i].dwUserID == 0 )
				{
					continue;
				}

				if ((*itt).second.resProduct[i].dwUserID == rsa.dwUserID)
				{
					bUserProduct = CT_TRUE;
				}
			
				CT_INT32 iRes = 0;
				CCUser* pUser = 0;
				if (CCUserManager::getSingleton().Find((*itt).second.resProduct[i].dwUserID, &pUser))
				{
				    
					CMD_G_ResSpotGiveUpSuc resSpotGiveUp = {0};
					resSpotGiveUp.iScenceID = rsa.iScenceID;
					resSpotGiveUp.iSpotID   = rsa.iSpotID;

					pUser->CalMaxWorker();
					pUser->CountFreeWorker();	
					MasterData md = {0};
					pUser->m_userBase.GetMasterData(md);

					if ((*itt).second.resProduct[i].iProductCount> 0)	
					{
						//iRes     = G_GetSpotProduct(resQuality.iBaseNum, md.mvd.stGrade.iLevel, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval  )* (*itt).second.resProduct[i].iProductCount;
						iRes	 = GetResourceSpot(rsa.iScenceID,rsa.iSpotID,resQuality,md.mvd.stGrade.iLevel)* (*itt).second.resProduct[i].iProductCount;
						iRes     = CalFightTeamType(pFightTeam, iRes, (*itt).second.resProduct[i].dwUserID );
						iRes	 = iRes*GetProductionLimit(rsa.iScenceID,md.mvd.stGrade.iLevel);
					}

					resSpotGiveUp.dwUserID    = (*itt).second.resProduct[i].dwUserID;
					//resSpotGiveUp.iGold       = md.mvd.stRes.iGold + iRes;
					resSpotGiveUp.iResType		= resQuality.iResID;

					CT_INT32  res = iRes;
					if (G_ChangeResType(md.mvd,  resQuality.iResID, res) )
					{
						resSpotGiveUp.iRes		  = res;
					}
					
					resSpotGiveUp.iMaxWorkers = md.mvd.stRes.iMaxWorker;
					resSpotGiveUp.iWorker     = md.mvd.stRes.iWorker;
					pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB, SUB_G_RESSPOT_GIVEUP_APPLY_SUC, (CT_CHAR*)&resSpotGiveUp, sizeof(resSpotGiveUp));
				
				}else 
				{
					if ( (*itt).second.resProduct[i].iProductCount > 0)
					{
						CMD_G_FightTeamMember ftm={0};
						pFightTeam->GetFightTeamMember(1,(*itt).second.resProduct[i].dwUserID,&ftm);
						//iRes = G_GetSpotProduct(resQuality.iBaseNum, (*itt).second.resProduct[i].iGrade, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval  )* (*itt).second.resProduct[i].iProductCount;
						iRes = GetResourceSpot(rsa.iScenceID,rsa.iSpotID,resQuality,(*itt).second.resProduct[i].iGrade)* (*itt).second.resProduct[i].iProductCount;
						iRes = CalFightTeamType(pFightTeam, iRes, (*itt).second.resProduct[i].dwUserID );
						iRes	 = iRes*GetProductionLimit(rsa.iScenceID,ftm.iGrade);
					}
				}

				SaveUserProduct(rsa.dwTeamID, (*itt).second.resProduct[i].dwUserID, iRes,resQuality.iResID,iResType, rsa.iScenceID, rsa.iSpotID, ENUM_PRODUCT_REASON_GIVEUP);
			}

			if (!bUserProduct)
			{
				CCUser* pUser = 0;
				if (CCUserManager::getSingleton().Find(rsa.dwUserID, &pUser))
				{
					CMD_G_ResSpotGiveUpSuc resSpotGiveUp = {0};
					resSpotGiveUp.iScenceID = rsa.iScenceID;
					resSpotGiveUp.iSpotID   = rsa.iSpotID;

					pUser->CalMaxWorker();
					pUser->CountFreeWorker();	
					MasterData md = {0};
					pUser->m_userBase.GetMasterData(md);
					resSpotGiveUp.dwUserID    = rsa.dwUserID;
					//resSpotGiveUp.iGold       = md.mvd.stRes.iGold;
					resSpotGiveUp.iResType		= resQuality.iResID;

					CT_INT32 res = 0 ;
					if (G_ChangeResType(md.mvd, resQuality.iResID, res))
					{
						resSpotGiveUp.iRes= res;
					}
					
					resSpotGiveUp.iResType=resQuality.iResID;
					resSpotGiveUp.iMaxWorkers = md.mvd.stRes.iMaxWorker;
					resSpotGiveUp.iWorker     = md.mvd.stRes.iWorker;
					pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB, SUB_G_RESSPOT_GIVEUP_APPLY_SUC, (CT_CHAR*)&resSpotGiveUp, sizeof(resSpotGiveUp));
				}
			}

		    CMD_GesSpotBase spotGiveUp = {0};
			spotGiveUp.iScenceID = (*it).first;
			spotGiveUp.iSpotID   = (*itt).first;
			BroadcastSence((*it).first, MAIN_G_RES_GRAB, SUB_G_RESSPOT_GIVEUP_APPLY_SUC_POST, (CT_CHAR*)&spotGiveUp, sizeof(spotGiveUp));
			pFightTeam->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_GIVEUP_APPLY_SUC_POST,  (CT_CHAR*)&spotGiveUp, sizeof(spotGiveUp));
			memset(&((*itt).second), 0, sizeof(ResourceSpot));
			EaseSpotFromFTeam(rsa.iScenceID, rsa.iSpotID, rsa.dwTeamID);
		}
	}
}

//用户生产
CT_VOID CCResourceGrabMgr::UserHavestSpotRes(CCFightTeam* pFightTeam, CCUser* pUser, const CMD_G_ResSpotApply& rsa)
{
	_AUTOMUTEX(&m_mutex);
	CT_BOOL bFind = CT_FALSE;
	SenceMap::iterator it = m_mapSpots.find(rsa.iScenceID);
	if(it != m_mapSpots.end())
	{
		SpotMap::iterator  itt = (*it).second.find(rsa.iSpotID);
		if (itt != (*it).second.end())
		{
			CT_INT32 iResType=CLIENT_OBJ_RES;
			G_ResourceQuality resQuality = {0};
			if(!GetResourceQuality(rsa.iScenceID, rsa.iSpotID, resQuality,iResType))
			{
				return;
			}
			
			for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i )
			{
				if (pUser->GetUserID() == (*itt).second.resProduct[i].dwUserID)
				{
					CMD_G_ResSpotHarvestSuc resSpotHarvestSuc = {0};
					resSpotHarvestSuc.dwUserID  = rsa.dwUserID;
					resSpotHarvestSuc.iScenceID = rsa.iScenceID;
					resSpotHarvestSuc.iSpotID   = rsa.iSpotID;
					

// 					pUser->CountFreeWorker();	
// 					pUser->CalMaxWorker();
					MasterData md = {0};
					pUser->m_userBase.GetMasterData(md);

					if ( (*itt).second.resProduct[i].iProductCount > 0)
					{
						(*itt).second.resProduct[i].iStatus       = SPOT_PRODUCTING;
						//CT_INT32 iRes       = G_GetSpotProduct(resQuality.iBaseNum, md.mvd.stGrade.iLevel, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval ) *  (*itt).second.resProduct[i].iProductCount;
						CT_INT32 iRes		= GetResourceSpot(rsa.iScenceID,rsa.iSpotID,resQuality,md.mvd.stGrade.iLevel) *  (*itt).second.resProduct[i].iProductCount;			
						iRes = CalFightTeamType(pFightTeam, iRes, (*itt).second.resProduct[i].dwUserID );
						iRes = iRes*GetProductionLimit(rsa.iScenceID,md.mvd.stGrade.iLevel);
						//resSpotHarvestSuc.iGold       = md.mvd.stRes.iGold + iGold;
						resSpotHarvestSuc.iResID	  = resQuality.iResID;

						CT_INT32 res = iRes;
						if (G_ChangeResType(md.mvd,resQuality.iResID, res))
						{
							resSpotHarvestSuc.iOutput	  = res;
						}
						
						resSpotHarvestSuc.iMaxWorkers = md.mvd.stRes.iMaxWorker;
						resSpotHarvestSuc.iWorker     = md.mvd.stRes.iWorker;
						resSpotHarvestSuc.iStatuts  = (*itt).second.resProduct[i].iStatus;
						pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB, SUB_G_RESSPOT_HARVEST_APPLY_SUC, (CT_CHAR*)&resSpotHarvestSuc, sizeof(resSpotHarvestSuc));
						
						SaveUserProduct(rsa.dwTeamID,  (*itt).second.resProduct[i].dwUserID, iRes,resQuality.iResID,iResType, rsa.iScenceID, rsa.iSpotID,  ENUM_RPODUCT_REASON_PRODUCTED);					
						 (*itt).second.resProduct[i].iProductCount = 0;
				
						CMD_G_FightTeamEventLog eventLog = {0};
						RollBroadcast rb[2]={0};
						eventLog.lDate   = time(0);
						rb[0].SetValue(0,md.cNickName);
						rb[1].SetValue(iRes,"");
						//sprintf(eventLog.cEventDesc, "{24}<%s>{27}%i{10}", md.cNickName, iRes);
						G_GetRollBroadecastData(114,eventLog.cEventDesc,sizeof(eventLog.cEventDesc),(CT_CHAR*)rb,sizeof(rb));
						pFightTeam->AddEventLog(eventLog);
					}
					return;
				}
			}

		}
	}
}

CT_VOID CCResourceGrabMgr::UserHavestSpotRes(CCFightTeam* pFightTeam, CT_DWORD dwUserID, const CMD_G_ResSpotApply& rsa)
{
	_AUTOMUTEX(&m_mutex);
	CT_BOOL bFind = CT_FALSE;
	SenceMap::iterator it = m_mapSpots.find(rsa.iScenceID);
	if(it == m_mapSpots.end())
	{
		return;
	}

	SpotMap::iterator  itt = (*it).second.find(rsa.iSpotID);
	if (itt != (*it).second.end())
	{
		CT_INT32 iResType=CLIENT_OBJ_RES;
		G_ResourceQuality resQuality = {0};
		if(!GetResourceQuality(rsa.iScenceID, rsa.iSpotID, resQuality,iResType))
		{
			return;
		}

		for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i )
		{
			if (dwUserID == (*itt).second.resProduct[i].dwUserID)
			{
				if ( (*itt).second.resProduct[i].iProductCount > 0)
				{
					CMD_G_FightTeamMember ftm={0};
					pFightTeam->GetFightTeamMember(1,dwUserID,&ftm);
					(*itt).second.resProduct[i].iStatus       = SPOT_PRODUCTING;
					//CT_INT32 iRes       = G_GetSpotProduct(resQuality.iBaseNum, (*itt).second.resProduct[i].iGrade, resQuality.fLevelRatio, resQuality.fTimeRatio, resQuality.iInterval ) *  (*itt).second.resProduct[i].iProductCount;
					CT_INT32 iRes		= GetResourceSpot(rsa.iScenceID, rsa.iSpotID,resQuality,(*itt).second.resProduct[i].iGrade) *  (*itt).second.resProduct[i].iProductCount;
					iRes = CalFightTeamType(pFightTeam, iRes, (*itt).second.resProduct[i].dwUserID );
					iRes = iRes*GetProductionLimit(rsa.iScenceID,ftm.iGrade);
					SaveUserProduct(rsa.dwTeamID,  (*itt).second.resProduct[i].dwUserID, iRes,resQuality.iResID,iResType, rsa.iScenceID, rsa.iSpotID,  ENUM_RPODUCT_REASON_PRODUCTED);					
					(*itt).second.resProduct[i].iProductCount = 0;
					printf("CCResourceGrabMgr::UserHavestSpotRes iProductCount:%d",(*itt).second.resProduct[i].iProductCount);
					printf("CCResourceGrabMgr::UserHavestSpotRes iRes:%d\n",iRes);

					CMD_G_FightTeamEventLog eventLog = {0};
					CT_CHAR cNickName[NAME_SIZE] = {0};
					RollBroadcast rb[2]={0};
					pFightTeam->GetMemberName(dwUserID, cNickName);
					eventLog.lDate   = time(0);
					rb[0].SetValue(0,cNickName);
					rb[1].SetValue(iRes,"");
					//sprintf(eventLog.cEventDesc, "{24}<%s>{27}%i{10}", cNickName, iRes);
					G_GetRollBroadecastData(114,eventLog.cEventDesc,sizeof(eventLog.cEventDesc),(CT_CHAR*)rb,sizeof(rb));
					pFightTeam->AddEventLog(eventLog);
				}
				return;
			}
		}
	}
}

//场景广播
CT_VOID CCResourceGrabMgr::BroadcastSence(CT_INT32 iScenceID, CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_INT32 bufLen)
{
	_AUTOMUTEX(&m_mutex);
	SenceUserMap::iterator it = m_mapSenceUser.find(iScenceID);
	if (it != m_mapSenceUser.end())
	{
		SenceUserSet::iterator  itt = (*it).second.begin();
		SenceUserSet::iterator  ittEnd = (*it).second.end();
		for (; itt != ittEnd; ++itt)
		{
			CCUser* pUser = 0;
			if (CCUserManager::getSingleton().Find((*itt), &pUser))
			{
				pUser->GetNetInterface()->Send(mainID, subID, pBuf, bufLen);
			}else
			{
				(*it).second.erase(itt);
			}
		}
	}
}

//更新生产信息
CT_VOID CCResourceGrabMgr::UpdateProduct()
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.begin();
	SenceMap::iterator itEnd = m_mapSpots.end();
	for (; it!=itEnd; ++it)
	{
		SpotMap::iterator  itt = (*it).second.begin();
		SpotMap::iterator  ittEnd = (*it).second.end();
		for ( ;itt != ittEnd; ++itt)
		{
			if (  (*itt).second.resBase.dwFTID == 0)
			{
				//清除残留数据（为什么残留还没找到，这个结构ResourceBase FTID被清空了，ResourceProduct这个结构没有清空 ）
				memset(&((*itt).second), 0, sizeof(ResourceSpot));
				continue;
			}

			//保护过期通知整个战队
			CCFightTeam* pTeam = 0;
			if (!CCFightTeamManager::getSingleton().Find((*itt).second.resBase.dwFTID, &pTeam))
			{
				continue;
			}
			//处理保护过期
			(*itt).second.resBase.iLeftProtectTime -= (TIME_RESOURCE_PRODUCT / 1000); 
			if ((*itt).second.resBase.iLeftProtectTime <= 0 && !(*itt).second.resBase.bSendProtectMes )
			{
				(*itt).second.resBase.iLeftProtectTime = 0;
				(*itt).second.resBase.bSendProtectMes  = CT_TRUE;
				FightTeamMemberUID fTeamUID = {0};
				pTeam->GetFightTeamMemberUID(&fTeamUID);
				
				for (CT_DWORD i=0; i<fTeamUID.dwCount; ++i)
				{
					//保护过期通知整个战队
					CMD_G_ResSpotApply resSpotApply = {0};
					resSpotApply.dwUserID  = fTeamUID.dwMemberUID[i];
					resSpotApply.dwTeamID  = (*itt).second.resBase.dwFTID;
					resSpotApply.iScenceID = (*it).first;
					resSpotApply.iSpotID   = (*itt).first;
					CCUser* pUser = 0; 
					if (CCUserManager::getSingleton().Find(resSpotApply.dwUserID, &pUser))
					{
						pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB, SUB_G_RESSPOT_PROTECT_TIMEOUT, (CT_CHAR*)&resSpotApply, sizeof(resSpotApply));
					}
				}

				CMD_GesSpotBase  gsb = {0};
				gsb.iScenceID = (*it).first;
				gsb.iSpotID   = (*itt).first;
				BroadcastSence((*it).first, MAIN_G_RES_GRAB, SUB_G_RESSPOT_PROTECT_TIMEOUT_POST,   (CT_CHAR*)&gsb, sizeof(gsb));
			}
		    
			//生产检测
			for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i)
			{
				if ((*itt).second.resProduct[i].iStatus == SPOT_PRODUCTING && (*itt).second.resProduct[i].dwUserID != 0)
				{
					CheckProduct((*it).first, (*itt).first, (*itt).second.resBase, (*itt).second.resProduct[i]);
				}
			}

			//占领过期
			(*itt).second.resBase.iLeftOccupyTime -= (TIME_RESOURCE_PRODUCT / 1000); 
			if ((*itt).second.resBase.iLeftOccupyTime <= 0)
			{
				for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i)
				{
					CCUser* pUser = 0;
					CMD_G_ResSpotApply rsa ={0};
					rsa.dwTeamID = (*itt).second.resBase.dwFTID;
					rsa.dwUserID = (*itt).second.resProduct[i].dwUserID;
					rsa.iScenceID = (*it).first;
					rsa.iSpotID   = (*itt).first;
					if (CCUserManager::getSingleton().Find((*itt).second.resProduct[i].dwUserID, &pUser))
					{
						UserHavestSpotRes(pTeam, pUser,  rsa);
					}
					else
					{
						UserHavestSpotRes(pTeam, (*itt).second.resProduct[i].dwUserID,  rsa);
						//资源点过期,离线收获
					}
				}
				
				//通知占领过期
				CMD_G_ResSpotOccupyChange resSpot = {0};
				//得到战队信息
				resSpot.teamInfo.dwTeamID = (*itt).second.resBase.dwFTID;
				pTeam->GetFightTeamName(resSpot.teamInfo.cName);
				resSpot.iChangeReaSon = SPOT_OCCUPY_CHANGE_TIMEOUT;     
				resSpot.iScenceID     = (*it).first;
				resSpot.iSpotID       = (*itt).first;
				pTeam->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_CHANGE, (CT_CHAR*)&resSpot, sizeof(resSpot));

				//通知战队驻守人员信息
				GuardTag gdPrimary = {0};
				if (CCResourceGrabMgr::getSingleton().TeamFindResSpotGuard((*it).first, (*itt).first, gdPrimary))
				{
					GuardTag gd = {0};
					pTeam->SetUsersGuard(gdPrimary,  gd);
				}
				BroadcastSence((*it).first, MAIN_G_RES_GRAB, SUB_G_RESSPOT_OCCUPY_CHANGE, (CT_CHAR*)&resSpot, sizeof(resSpot));
				pTeam->Broadcast(MAIN_G_RES_GRAB,	SUB_G_RESSPOT_OCCUPY_CHANGE,  (CT_CHAR*)&resSpot, sizeof(resSpot));
				EaseSpotFromFTeam((*it).first, (*itt).first, (*itt).second.resBase.dwFTID);


				CT_INT32 iResType=CLIENT_OBJ_RES;
				G_ResourceQuality resQuality= {0};
				if(!GetResourceQuality((*it).first, (*itt).first, resQuality,iResType))
				{
					return;
				}

				CMD_G_FightTeamEventLog eventLog = {0};
				eventLog.lDate   = time(0);
				//sprintf(eventLog.cEventDesc, "%s{28}", resQuality.cName);
				RollBroadcast rb={0};
				rb.SetValue(0,resQuality.cName);
				G_GetRollBroadecastData(115,eventLog.cEventDesc,sizeof(eventLog.cEventDesc),(CT_CHAR*)&rb,sizeof(rb));
				pTeam->AddEventLog(eventLog);

				//设置战队的资源数量
				ResSpotTag rst={0};	
				pTeam->SetResCount(TeamFindResSpot(pTeam->GetFightTeamID(), rst) );

				//清除生产数据
				memset(&((*itt).second), 0, sizeof(ResourceSpot));
			}
		}
	}
}


//得到全部的保存信息
CT_VOID CCResourceGrabMgr::GetAllInfoToSave(std::string& info)
{
	_AUTOMUTEX(&m_mutex);
	info += "delete from resourceproduct;\n";
	info += "delete from resourcespot;\n";
	info += "insert into resourceproduct(U_ID, P_Sequence, R_SenceID, R_SpotID, P_Grade, P_ProductStatus, P_Count,  P_LeftProductTime) \n   values";

	SenceMap::iterator it = m_mapSpots.begin();
	SenceMap::iterator itEnd = m_mapSpots.end();
	for (; it!=itEnd; ++it)
	{
		SpotMap::iterator  itt = (*it).second.begin();
		SpotMap::iterator  ittEnd = (*it).second.end();
		for ( ;itt != ittEnd; ++itt)
		{
			for (int i=0; i<CLAN_MAX_MEMBER ; ++i)
			{
				//if( (*itt).second.resProduct[i].dwUserID != 0)
				{
					CT_CHAR buf[512] = {0};
					sprintf(buf, "(%i, %i, %i, %i, %i, %i, %i, %i ),\n", (*itt).second.resProduct[i].dwUserID,      i,
																		 (*it).first,                               (*itt).first,
																		 (*itt).second.resProduct[i].iGrade,        (*itt).second.resProduct[i].iStatus, 
																		 (*itt).second.resProduct[i].iProductCount, (*itt).second.resProduct[i].iLeftProductTime);
					info +=  buf;
				}
			}
		}
	}

	size_t pos = info.rfind(",");
	if (pos != std::string::npos)
	{
		info.erase(pos);
	}
	info += ";";

	info += "insert into resourcespot(R_SpotID, R_SceneID, R_FTID, R_Note, R_LeftProtectTime,R_LeftOccupyTime,R_SendProtectMes, R_Guard1, R_Guard2, R_Guard3   ) \n   values";

	 it = m_mapSpots.begin();
	 itEnd = m_mapSpots.end();
	for (; it!=itEnd; ++it)
	{
		SpotMap::iterator  itt    = (*it).second.begin();
		SpotMap::iterator  ittEnd = (*it).second.end();
		for ( ;itt != ittEnd; ++itt)
		{
			CT_CHAR buf[512] = {0};
			sprintf(buf, "(%i, %i, %i, \'%s\', %i, %i, %i,%i, %i, %i ),\n", (*itt).first, (*it).first, (*itt).second.resBase.dwFTID,
																     (*itt).second.resBase.szNote, (*itt).second.resBase.iLeftProtectTime,
																	 (*itt).second.resBase.iLeftOccupyTime, (*itt).second.resBase.bSendProtectMes,
																	 (*itt).second.guardTag.dwUserID[0],  (*itt).second.guardTag.dwUserID[1],
																	 (*itt).second.guardTag.dwUserID[2]); 
			info += buf;	
		}
	}

	pos = info.rfind(",");
	if (pos != std::string::npos)
	{
		info.erase(pos);
	}

	info += ";";
	//CCLogMeg::getSingleton().WriteError( info.c_str());
}

//得到用户资源数据
CT_BOOL CCResourceGrabMgr::GetUserResData(CT_DWORD dwUserID, CT_CHAR* buf, CT_INT32& bufLen)
{
	_AUTOMUTEX(&m_mutex);

	CMD_G_ResSpotInfo resSpotInfo = {0};
	if (sizeof(resSpotInfo) * SPOT_RES_SIZE > bufLen)
	{
		return CT_FALSE;
	}

	CT_DWORD dwTeamID =  CCFightTeamManager::getSingleton().GetFightTeamIDByUser(dwUserID);
	if (dwTeamID == 0)
	{
		return CT_FALSE;
	}

	ResSpotTag rst = {0};
	CT_INT32   resNum = TeamFindResSpot(dwTeamID, rst);
	if( resNum == 0 )
	{
		return CT_FALSE;
	}
	
	for (CT_INT32 i=0; i<resNum; ++i)
	{
		SenceMap::iterator it = m_mapSpots.find(rst.resSpot[i].iSceneID);
		if (it != m_mapSpots.end())
		{
			SpotMap::iterator itt = (*it).second.find(rst.resSpot[i].iSpotID);
			if (itt != (*it).second.end() )
			{
				memset(&resSpotInfo, 0, sizeof(resSpotInfo));
				resSpotInfo.iScenceID = rst.resSpot[i].iSceneID;
				resSpotInfo.iSpotID   = rst.resSpot[i].iSpotID;
				GetResourceMes(dwUserID, (*itt).second,(*itt).first,rst.resSpot[i].iSceneID, resSpotInfo);
				memcpy(buf + i*sizeof(CMD_G_ResSpotInfo), &resSpotInfo, sizeof(resSpotInfo));
			}
		}
	}

	bufLen = resNum * sizeof(CMD_G_ResSpotInfo);
	return CT_TRUE;
}

//生产检测
CT_VOID CCResourceGrabMgr::CheckProduct(CT_INT32 iSceneID, CT_INT32 iSpotID,  const ResourceBase& resBase,  ResourceProduct& rp)
{
	rp.iLeftProductTime -= (TIME_RESOURCE_PRODUCT / 1000); 
	
	G_ResourceArea ra = {0};
	if (!GetResourceArea(iSceneID, ra))
	{
		return;
	}

	if (rp.iProductCount > ra.iLimit)
	{
		rp.iLeftProductTime = 0;
		return;
	}

	if (rp.iLeftProductTime <= 0 && rp.iProductCount < ra.iLimit + 1)
	{
		CT_INT32 iResType=CLIENT_OBJ_RES;
		G_ResourceQuality resQua = {0};
		if (!GetResourceQuality(iSceneID,  iSpotID, resQua,iResType))
		{
			return ;
		}
		rp.iProductCount += 1;

		if (rp.iProductCount == ra.iLimit)
		{
			rp.iStatus = SPOT_FULL;
		}

		rp.iLeftProductTime = resQua.iInterval;

		//战队信息xxx
		CCFightTeam* pTeam = 0;
		if (!CCFightTeamManager::getSingleton().Find(resBase.dwFTID, &pTeam))
		{
			return;
		}

		CCUser *pUser = 0;
		if (CCUserManager::getSingleton().Find(rp.dwUserID, &pUser))
		{
			pUser->CalMaxWorker();
			pUser->CountFreeWorker();

			MasterData md = {0};
			pUser->m_userBase.GetMasterData(md);
			CMD_G_ResSpotProduct rsp = {0};
			//CT_INT32 iRes=G_GetSpotProduct(resQua.iBaseNum,md.mvd.stGrade.iLevel,resQua.fLevelRatio,resQua.fTimeRatio,resQua.iInterval)*rp.iProductCount;
			CT_INT32 iRes=GetResourceSpot(iSceneID,iSpotID,resQua,md.mvd.stGrade.iLevel)*rp.iProductCount;
			if(pTeam != NULL)
				iRes=CalFightTeamType(pTeam, iRes, pUser->GetUserID() )*GetProductionLimit(iSceneID,md.mvd.stGrade.iLevel);;

			rsp.dwUserID = rp.dwUserID;
			rsp.iLeftProductTime = rp.iLeftProductTime;
			rsp.iMaxWorker       = md.mvd.stRes.iMaxWorker;
			rsp.iScenceID        = iSceneID;
			rsp.iSpotID          = iSpotID;
			rsp.iStatuts         = rp.iStatus;
			//rsp.iSumaryCount     = rp.iProductCount;
			rsp.iResType		 = iResType;
			rsp.iResID			 = resQua.iResID;
			rsp.iOutput			 = iRes;
			rsp.iWorker          = md.mvd.stRes.iWorker;
			pUser->GetNetInterface()->Send(MAIN_G_RES_GRAB, SUB_G_RESSPOT_PRODUCT_INFO, (CT_CHAR*)&rsp, sizeof(rsp) );
		}
	}
}

//查找生产位置
CT_BOOL CCResourceGrabMgr::FindProductPos(const ResourceProduct* pResProduct, CT_DWORD uid, CT_INT32& pos)
{

	for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i)
	{
		if (pResProduct[i].dwUserID == uid)
		{
			pos = i;
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//计算修正后的资源
CT_INT32 CCResourceGrabMgr::CalFightTeamType(CCFightTeam* pFightTeam, CT_INT32 iRes, CT_DWORD dwUserID)
{
	if(!m_bResActive)
	{
		CCFightTeam::FIGHT_MEMBER_TYPE type = pFightTeam->GetMemberPosition( dwUserID );
		if (type == CCFightTeam::FIGHTTEAM_BOSS)
		{
			iRes = (CT_INT32)(iRes * ( 1 + G_ResSpotBossProductUp));
		}else if (type == CCFightTeam::FIGHTTEAM_ELITE)
		{
			iRes = (CT_INT32)(iRes * ( 1 + G_ResSpotEliteProductUP));
		}
	}
	return iRes;
}


//广播驻守信息
CT_VOID CCResourceGrabMgr::BroadcastSenceGuard(CT_INT32 iScenceID, CT_INT32 iSpotID, CCFightTeam* pFightTeam, CT_CHAR pVictorySign[SPOT_VECTORY_SIGN_SIZE], CT_DWORD pDependPlayer[SPOT_GUARD_MEMBER])
{
	_AUTOMUTEX(&m_mutex);
	CMD_G_ResSpotDependPost  rsdp = {0};
	rsdp.iScenceID = iScenceID;
	rsdp.iSpotID   = iSpotID;
	rsdp.teamInfo.dwTeamID = pFightTeam->GetFightTeamID();
	pFightTeam->GetFightTeamName(rsdp.teamInfo.cName);
	memcpy(rsdp.cVictorySign, pVictorySign,  SPOT_VECTORY_SIGN_SIZE);
	for (CT_INT32 i=0; i< SPOT_GUARD_MEMBER; ++i)
	{
		CMD_G_FightTeamMember fightTeamMember = {0};
		if(pFightTeam->GetFightTeamMember(1, pDependPlayer[i], &fightTeamMember))
		{
			memcpy(rsdp.dependPlayers[i].cNickName, fightTeamMember.cNickName, NAME_SIZE);
			rsdp.dependPlayers[i].dwUserID		 =  pDependPlayer[i];
			rsdp.dependPlayers[i].iFightingPower = fightTeamMember.iFightingPower;
			rsdp.dependPlayers[i].iOccupation    = fightTeamMember.iOccupation;
		}
	}        
	CCResourceGrabMgr::getSingleton().BroadcastSence(iScenceID, MAIN_G_RES_GRAB, SUB_G_RESSPOT_DEPEND_APPLY_SUC_POST, (CT_CHAR*)&rsdp, sizeof(rsdp));

}

CT_VOID CCResourceGrabMgr::SetResActive(CT_BOOL bResActive)
{
	_AUTOMUTEX(&m_mutex);
	m_bResActive=bResActive;
	if (bResActive)
	{
		SenceMap::iterator it = m_mapSpots.begin();
		SenceMap::iterator itEnd = m_mapSpots.end();
		for (; it!=itEnd; ++it)
		{
			SpotMap::iterator  itt = (*it).second.begin();
			SpotMap::iterator  ittEnd = (*it).second.end(); 
			for ( ;itt != ittEnd; ++itt)    
			{
				if ( (*itt).second.resBase.dwFTID != 0)  
				{
					(*itt).second.resBase.iLeftProtectTime = 0;
				}
			}
		}
	}
}

//插入活动资源点
CT_VOID CCResourceGrabMgr::InsterResActive(CT_CHAR *pBuf,CT_INT32 iLen)
{
	_AUTOMUTEX(&m_mutex);

	if(iLen < sizeof(ResActive))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::InsterResActive iLen < sizeof(ResActive)");
		return;
	}

	for(CT_INT32 i=0;i<iLen/sizeof(ResActive);++i)
	{
		ResActive ra={0};
		memcpy(&ra,pBuf+i*sizeof(ResActive),sizeof(ResActive));
		if (m_SceneList.find(ra.iSceneID) == m_SceneList.end())
		{
			continue;
		}

		CT_INT64 lKey=ra.iSceneID;
		lKey<<=32;
		lKey|=ra.iSpot;
		m_ResActiveMap.insert(ResActiveMap::value_type(lKey,ra));
	}
	return;
}

//获取资源点
CT_BOOL CCResourceGrabMgr::GetResActiveSpot(CT_CHAR *pBuf,CT_INT32 &iLen)
{
	_AUTOMUTEX(&m_mutex);
	if(iLen < (sizeof(ResActive)*m_ResActiveMap.size()))
	{
		CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::GetResActiveSpot sizeof(ResActive)*m_ResActiveMap.size()");
		return CT_FALSE;
	}

	CT_INT32 iNum=0;
	for(ResActiveMap::iterator it=m_ResActiveMap.begin();it != m_ResActiveMap.end();++it)
	{
		memcpy(pBuf+iNum*sizeof(ResActive),&((*it).second),sizeof(ResActive));
		iNum++;
	}

	iLen=sizeof(ResActive)*m_ResActiveMap.size();

	return CT_TRUE;
}

//活动强制收获、
CT_VOID CCResourceGrabMgr::ResActiveHavest()
{
	_AUTOMUTEX(&m_mutex);
	SenceMap::iterator it = m_mapSpots.begin();
	SenceMap::iterator itEnd = m_mapSpots.end();
	for (; it!=itEnd; ++it)
	{
		SpotMap::iterator  itt = (*it).second.begin();
		SpotMap::iterator  ittEnd = (*it).second.end(); 
		for ( ;itt != ittEnd; ++itt)    
		{
			if (  (*itt).second.resBase.dwFTID == 0)  
			{
				continue;
			}

			//保护过期通知整个战队
			CCFightTeam* pTeam = 0;
			if (!CCFightTeamManager::getSingleton().Find((*itt).second.resBase.dwFTID, &pTeam))
			{
				continue;
			}

			//生产检测
			for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i)
			{
				if ((*itt).second.resProduct[i].iStatus == SPOT_PRODUCTING && (*itt).second.resProduct[i].dwUserID != 0)
				{
					//重置所有资源点生产剩余时间
					CT_INT32 iResType=CLIENT_OBJ_RES;
					G_ResourceQuality resQuality = {0};
					if(!GetResourceQuality((*it).first, (*itt).first, resQuality,iResType))
					{
						continue;
					}
					(*itt).second.resProduct[i].iLeftProductTime=resQuality.iInterval;
					CT_INT64 lKey=(*it).first;
					lKey<<=32;
					lKey|=(*itt).first;
					ResActiveMap::iterator rait=m_ResActiveMap.find(lKey);
					if(rait == m_ResActiveMap.end())
					{
						continue;
					}

					if(m_bResActive)
						(*itt).second.resProduct[i].iLeftProductTime=(*rait).second.iProductTime;

					CheckProduct((*it).first, (*itt).first, (*itt).second.resBase, (*itt).second.resProduct[i]);
				}
			}

			//占领过期
			//(*itt).second.resBase.iLeftOccupyTime -= (TIME_RESOURCE_PRODUCT / 1000); 
			//if ((*itt).second.resBase.iLeftOccupyTime <= 0)
			{
				for (CT_INT32 i=0; i<CLAN_MAX_MEMBER; ++i)
				{
					CCUser* pUser = 0;
					CMD_G_ResSpotApply rsa ={0};
					rsa.dwTeamID = (*itt).second.resBase.dwFTID;
					rsa.dwUserID = (*itt).second.resProduct[i].dwUserID;
					rsa.iScenceID = (*it).first;
					rsa.iSpotID   = (*itt).first;
					if (CCUserManager::getSingleton().Find((*itt).second.resProduct[i].dwUserID, &pUser))
					{
						UserHavestSpotRes(pTeam, pUser,  rsa);
					}
					else
					{
						UserHavestSpotRes(pTeam, (*itt).second.resProduct[i].dwUserID,  rsa);
						//资源点过期,离线收获
					}
				}

				//清除生产数据
				(*itt).second.resProduct->iProductCount=0;
				//memset(&((*itt).second), 0, sizeof(ResourceSpot));
			}
		}
	}
	return;
}

//获取活动资源点
CT_BOOL CCResourceGrabMgr::GetActiveResSpot(CT_INT32 iScenceID,CT_INT32 iSpotID)
{
	_AUTOMUTEX(&m_mutex);
	if(m_bResActive)
	{
		CT_INT64 lKey=iScenceID;
		lKey<<=32;
		lKey|=iSpotID;
		ResActiveMap::iterator it=m_ResActiveMap.find(lKey);
		if(it != m_ResActiveMap.end())
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//获取活动资源点
CT_BOOL CCResourceGrabMgr::GetActiveResSpot(CT_INT32 iScenceID,CT_INT32 iSpotID,ResActive &ra)
{
	_AUTOMUTEX(&m_mutex);
	if(m_bResActive)
	{
		CT_INT64 lKey=iScenceID;
		lKey<<=32;
		lKey|=iSpotID;
		ResActiveMap::iterator it=m_ResActiveMap.find(lKey);
		if(it != m_ResActiveMap.end())
		{
			memcpy(&ra,&((*it).second),sizeof(ResActive));
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//随机生产类型
CT_VOID CCResourceGrabMgr::RandProductType()
{
	_AUTOMUTEX(&m_mutex);

	if(!m_bResActive)return;

	std::map<CT_INT32,std::list<CT_INT32> >  AreaMap;

	//统计区域点总数
	ResActiveMap::iterator itend=m_ResActiveMap.end();
	for(ResActiveMap::iterator it=m_ResActiveMap.begin();it != itend;++it)
	{
		G_ResourceScene rs={0};
		if(!G_GetResourceScene((*it).second.iSceneID,&rs))
		{
			CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::RandProductType G_GetResourceScene iScenceID:%d",(*it).second.iSceneID);
			continue;
		}

		std::map<CT_INT32,std::list<CT_INT32> >::iterator Areait=AreaMap.find(rs.iAreaID);
		if(Areait != AreaMap.end())
		{
			(*Areait).second.push_back((*it).second.iSceneID);
		}
		else
		{
			std::list<CT_INT32> sceneList;
			sceneList.push_back((*it).second.iSceneID);
			AreaMap.insert(std::map<CT_INT32,std::list<CT_INT32> >::value_type(rs.iAreaID,sceneList));
		}
	}

	//读取规则分配
	std::vector<ResActive> ravec;
	std::map<CT_INT32,std::list<CT_INT32> >::iterator arit;
	for(arit=AreaMap.begin();arit != AreaMap.end();++arit)
	{
		CT_INT32 iRand=rand();
		CT_INT32 iResNum=0;
		CT_INT32 iAllResNum=(*arit).second.size();
		CT_CHAR cRtcBuf[MAX_BUFFER_LEN]={0};
		CT_INT32 iRtcLen=MAX_BUFFER_LEN;
		ravec.clear();

		//规则类型抽取
		if(!G_GetResActiveTypeChose((*arit).first,cRtcBuf,iRtcLen))
		{
			CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::RandProductType G_GetResActiveTypeChose");
			continue;
		}
		for(CT_INT32 k=0;k<iRtcLen/sizeof(G_ResActiveTypeChose);++k)
		{
			G_ResActiveTypeChose rtc={0};
			CT_CHAR cBuf[MAX_BUFFER_LEN]={0};
			CT_INT32 iLen=MAX_BUFFER_LEN;
			memcpy(&rtc,cRtcBuf+sizeof(G_ResActiveTypeChose)*k,sizeof(G_ResActiveTypeChose));

			if(!G_GetResoureActive((*arit).first,rtc.iTypeID,cBuf,iLen))
			{
				CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::RandProductType G_GetResoureActive area:%d TypeID:%d",(*arit).first,rtc.iTypeID);
				continue;
			}

			for(CT_INT32 j=0;j<iLen/sizeof(G_ResourceActive);++j)
			{
				CT_INT32 iResSprot=(*arit).second.size()*(rtc.fPercent);
				G_ResourceActive ra={0};
				memcpy(&ra,cBuf+j*sizeof(G_ResourceActive),sizeof(G_ResourceActive));

				iRand=rand();
				if(iRand%100 > ra.fAppearProbability*100)
					continue;
				iResNum=G_GetResCount(iResSprot,ra.iResMin,ra.iResMax,ra.fResMinPercent,ra.fResMaxPercent,iRand);
				if(iResNum == 0)
				{
					CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::RandProductType G_GetResCount resnum:%d iResMin:%d iResMax:%d fResMinPercent:%f fResMaxPercent:%f iRand:%d",
						(*arit).second.size(),ra.iResMin,ra.iResMax,ra.fResMinPercent,ra.fResMaxPercent,iRand);
					continue;
				}

				ResActive resA={0};
				resA.iProductType=ra.iProductType;
				resA.iResID=ra.iResID;
				resA.iProductNum=ra.iItemNum;
				resA.iProductTime=ra.iProductTime;
				resA.iProtectTime=ra.iProtectTime;
				resA.iFocusOccupyACTVType=ra.iFocusOccupyACTVType;
				resA.iFocusOccupyACTVNum=ra.iFocusOccupyACTVNum;
				resA.fProductMultiple=ra.fProductMultiple;

				for(CT_INT32 i=0;i<(CLIENT_OBJ_RES_SPOT == resA.iProductType ?iResSprot:iResNum);++i)
					ravec.push_back(resA);

				iResSprot-=iResNum;
				if(CLIENT_OBJ_RES_SPOT == resA.iProductType && ravec.size() >= (*arit).second.size())
					break;
			}
		}

		for(std::vector<ResActive>::iterator rit=ravec.begin();rit != ravec.end();++rit)
		{
			printf("********ra.iProductNum:%d ra.iResID:%d ra.iProductTime:%d\n",(*rit).iProductNum,(*rit).iResID,(*rit).iProductTime);
		}

		//分配规则
		for(ResActiveMap::iterator it=m_ResActiveMap.begin();it != itend;++it)
		{
			if(ravec.size() == 0)
				continue;

			G_ResourceScene rs={0};
			if(!G_GetResourceScene((*it).second.iSceneID,&rs))
			{
				CCLogMeg::getSingleton().WriteError("CCResourceGrabMgr::RandProductType G_GetResourceScene iScenceID:%d",(*it).second.iSceneID);
				continue;
			}

			if(rs.iAreaID != (*arit).first)
				continue;


			CT_INT32 iRand=rand()%ravec.size();
			ResActive ra=ravec.at(iRand);
			ravec.erase(ravec.begin()+iRand);

			(*it).second.iProductType=ra.iProductType;
			(*it).second.iResID=ra.iResID;
			(*it).second.iProductNum=ra.iProductNum;
			(*it).second.iProductTime=ra.iProductTime;
			(*it).second.iProtectTime=ra.iProtectTime;
			(*it).second.iFocusOccupyACTVType=ra.iFocusOccupyACTVType;
			(*it).second.iFocusOccupyACTVNum=ra.iFocusOccupyACTVNum;
			(*it).second.fProductMultiple=ra.fProductMultiple;
			(*it).second.iResEffect=ra.iResEffect;

			if(ra.iProductType != CLIENT_OBJ_ITEM && ra.iProductNum == 0)
			{
				CT_INT32 iResType=CLIENT_OBJ_RES;
				G_ResourceQuality resQuality = {0};
				if(!GetResourceQuality((*it).second.iSceneID, (*it).second.iSpot, resQuality,iResType))
				{
					continue;
				}

				(*it).second.iProductNum=GetResourceSpot((*it).second.iSceneID, (*it).second.iSpot, resQuality,1);
			}

			//更新产出时间
			SenceMap::iterator sit=m_mapSpots.find((*it).second.iSceneID);
			if(sit != m_mapSpots.end())
			{
				SpotMap::iterator spotit=(*sit).second.find((*it).second.iSpot);
				if(spotit != (*sit).second.end())
				{
					for(CT_INT32 i=0;i<CLAN_MAX_MEMBER;++i)
					{
						(*spotit).second.resProduct[i].iLeftProductTime=ra.iProductTime;
					}
				}
			}
		}
	}


	return;
}
