#include <ctime>
#include "CTowerResManager.h"
#include "CUser.h"
#include "CUserManager.h"
#include "CThreadMes.h"
#include "../sdk/include/CApi.h"
#include "../sdk/include/CNetInterface.h"
#include "../sdk/include/CGameMesEvent.h"

template<> CCTowerResManager* CCSingleton<CCTowerResManager>::ms_Singleton=0;

CCTowerResManager::CCTowerResManager(void) : m_mutex()
{
	m_userTowerMap.clear();			
	m_towerFindMap.clear();			
	m_elementTowerResMap.clear(); 
	m_towerFightingMap.clear();
    RecoverTowerDataFromIni();                //从配置中恢复塔结构  
}

CCTowerResManager::~CCTowerResManager(void)
{
}

//用户查询加入
CT_VOID CCTowerResManager::InsertUserFind(CT_DWORD dwUserID, CT_INT32 iTowerID, CT_INT32 iFloor, CT_INT32 iSlot)
{
	_AUTOMUTEX(&m_mutex);
	CT_ASSERT(iSlot < ELEMENT_TOWER_MAX_POSITION_NUM );

	m_userTowerMap.insert(UserTowerMap::value_type(dwUserID, iTowerID));

	TowerFindMap::iterator it = m_towerFindMap.find(iTowerID);
	if (it != m_towerFindMap.end())
	{
		(*it).second.insert(TowerUserMap::value_type(dwUserID, G_Combine64(iFloor, iSlot) ));
	}
}

//用户查询删除
CT_VOID CCTowerResManager::DelUserFind(CT_DWORD dwUserID, CT_INT32 iTowerID)
{
	_AUTOMUTEX(&m_mutex);
	{
		UserTowerMap::iterator it = m_userTowerMap.find(dwUserID);
		if (it != m_userTowerMap.end())
		{
			m_userTowerMap.erase(it);
		}
	}

	{
		TowerFindMap::iterator it = m_towerFindMap.find(iTowerID);
		if (it != m_towerFindMap.end())
		{
			TowerUserMap::iterator itt = (*it).second.find(dwUserID);
			if (itt != (*it).second.end())
			{
				(*it).second.erase(itt);
			}
		}
	}
}

//用户是否在塔中
CT_BOOL CCTowerResManager::FindUserInTower(CT_DWORD dwUserID, CT_INT32& iTowerID, CT_INT32& iFloor, CT_INT32& iSlot)
{
	_AUTOMUTEX(&m_mutex);
	UserTowerMap::iterator it = m_userTowerMap.find(dwUserID);
	if (it != m_userTowerMap.end())
	{
		TowerFindMap::iterator itt = m_towerFindMap.find((*it).second);
		if (itt != m_towerFindMap.end())
		{
			TowerUserMap::iterator ittt = (*itt).second.find(dwUserID);
			if (ittt != (*itt).second.end())
			{
				CT_DWORD dwFroor = 0, dwSlot = 0;
				G_Separate64((*ittt).second, dwFroor, dwSlot );
				iFloor = (CT_INT32)dwFroor;
				iSlot  = (CT_INT32)dwSlot;
				iTowerID = (*it).second;
				return CT_TRUE;
			}
		}
	}
	return CT_FALSE;
}



//用户进入塔
CT_VOID CCTowerResManager::InsertUserEnterMap(CT_INT32 iTowerID, CT_DWORD userID)
{
	_AUTOMUTEX(&m_mutex);
	DeleteUserEnterMap(userID);
	UserEnterMap::iterator it = m_userEnterMap.find(iTowerID);
	if (it != m_userEnterMap.end())
	{
		(*it).second.insert(userID);
	}
}

//用户离开塔
CT_VOID CCTowerResManager::DeleteUserEnterMap( CT_DWORD userID)
{
	_AUTOMUTEX(&m_mutex);
	UserEnterMap::iterator itt    = m_userEnterMap.begin();
	UserEnterMap::iterator ittEnd = m_userEnterMap.end();
	for (; itt != ittEnd; ++itt)
	{
		UserEnterSet::iterator it = (*itt).second.find(userID);
		if (it != (*itt).second.end())
		{
			(*itt).second.erase(it);
		}
	}
}

//塔广播
CT_VOID CCTowerResManager::BroadCastInTower(CT_INT32 iTowerID,  CT_INT32 mainID, CT_INT32 subID, CT_CHAR* pBuf, CT_INT32 bufLen)
{
	_AUTOMUTEX(&m_mutex);
	UserEnterMap::iterator it = m_userEnterMap.find(iTowerID);
	if (it != m_userEnterMap.end())
	{
		UserEnterSet::iterator itt    = (*it).second.begin();
		UserEnterSet::iterator ittEnd = (*it).second.end();
		
		for (; itt != ittEnd; ++itt)
		{
			CCUser* pUser = 0;
			if (CCUserManager::getSingleton().Find( (*itt), &pUser))
			{
				pUser->GetNetInterface()->Send(mainID, subID, pBuf, bufLen );
			}
		}
	}
}



//设置用户在塔中占领数据
CT_BOOL CCTowerResManager::SetUserInTower(const ElemTowerUserInfo& userInfo, CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot)
{
	_AUTOMUTEX(&m_mutex);
	ElementTowerResMaps::iterator it = m_elementTowerResMap.find(iTowerID);
	if (it != m_elementTowerResMap.end())
	{
		ElementTowerResMap::iterator itt =  (*it).second.find(iFoor);
		if (itt != (*it).second.end())
		{
			//CT_ASSERT(iSlot < (*itt).second.iMaxSlotNums);
			if (itt->second.slotInfo[iSlot].userInfo.dwUser > 0)
			{
				DelUserFind(itt->second.slotInfo[iSlot].userInfo.dwUser, iTowerID);
			}
			(*itt).second.slotInfo[iSlot].llBeginTime = time(0);
			memcpy(&((*itt).second.slotInfo[iSlot].userInfo), &userInfo, sizeof(userInfo));
			InsertUserFind(userInfo.dwUser, iTowerID, iFoor, iSlot);
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//擦除用户在塔中的占领数据
CT_VOID CCTowerResManager::EaseUserInTower(CT_DWORD dwUserID, CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot)
{
	_AUTOMUTEX(&m_mutex);
	ElementTowerResMaps::iterator it = m_elementTowerResMap.find(iTowerID);
	if (it != m_elementTowerResMap.end())
	{
		ElementTowerResMap::iterator itt =  (*it).second.find(iFoor);
		if (itt != (*it).second.end())
		{
			memset(&((*itt).second.slotInfo[iSlot]), 0, sizeof(ElementTowerSlotInfo));
			//memset(&((*itt).second.slotInfo[iSlot].iHelpFriend), 0, sizeof(CT_DWORD)* ELEMENT_TOWER_MAX_FRIED_HELP_NUM);
			DelUserFind(dwUserID, iTowerID);
		}
	}
}

//向资源点添加协助者
CT_VOID InsertUserHelpInSlot(CCTowerResManager::ElementTowerSlotInfo& slotInfo, CT_DWORD dwHelpID)
{
	for (CT_INT32 i=0; i<ELEMENT_TOWER_MAX_FRIED_HELP_NUM; ++i)
	{
		if( slotInfo.iHelpFriend[i] == 0)
		{
			slotInfo.iHelpFriend[i] = dwHelpID;
			break;
		}
	}
}

//增加用户协助数据
CT_VOID CCTowerResManager::SetUserHelpInTower( CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot, CT_DWORD dwHelpID)
{
	_AUTOMUTEX(&m_mutex);
	ElementTowerResMaps::iterator it = m_elementTowerResMap.find(iTowerID);
	if (it != m_elementTowerResMap.end())
	{
		ElementTowerResMap::iterator itt =  (*it).second.find(iFoor);
		if (itt != (*it).second.end())
		{
			//CT_ASSERT(iSlot < (*itt).second.iMaxSlotNums);
			//CT_ASSERT(dwHelpID < (*itt).second.slotInfo[iSlot].iMaxHelpNum);
			
			InsertUserHelpInSlot((*itt).second.slotInfo[iSlot], dwHelpID);
		}
	}
}

//得到资源点数据
CT_BOOL CCTowerResManager::GetSlotInTower( CT_INT32 iTowerID, CT_INT32 iFoor, CT_INT32 iSlot, CCTowerResManager::ElementTowerSlotInfo& slotInfo)
{
	_AUTOMUTEX(&m_mutex);
	ElementTowerResMaps::iterator it = m_elementTowerResMap.find(iTowerID);
	if (it != m_elementTowerResMap.end())
	{
		ElementTowerResMap::iterator itt =  (*it).second.find(iFoor);
		if (itt != (*it).second.end())
		{
			//CT_ASSERT(iSlot < (*itt).second.iMaxSlotNums);
			slotInfo = (*itt).second.slotInfo[iSlot];
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//从全局配置恢复数据结构
CT_VOID CCTowerResManager::RecoverTowerDataFromIni()
{
	_AUTOMUTEX(&m_mutex);
	G_TowerAreaList  tal ={0};
	G_GetTowerAreaList(&tal);
	for (CT_INT32 i=0; i<tal.iCount; ++i)
	{
		G_TowerCheckPointList gtcpl = {0};
	    G_GetTowerCheckPointList(tal.iList[i], &gtcpl);
		{
			ElementTowerResMap etr; 
			for (CT_INT32 j=0; j<gtcpl.iCount; ++j)
			{
				ElementTowerFoorInfo foorInfo = {0};
				etr[gtcpl.iList[j]] = foorInfo;
			}
			m_elementTowerResMap[tal.iList[i]]  = etr;  
		}

		UserEnterSet ues;
		m_userEnterMap.insert(UserEnterMap::value_type(tal.iList[i], ues));

		TowerUserMap towerUserMap;
		m_towerFindMap.insert(TowerFindMap::value_type(tal.iList[i], towerUserMap ));
	}
}

//从数据库中得到元素之塔数据
CT_VOID CCTowerResManager::RecoverTowerDataFromDB(CT_INT32 iFloor, CT_INT32 iSlot, CCTowerResManager::ElementTowerSlotInfo& slotInfo)
{
	_AUTOMUTEX(&m_mutex);
	CT_ASSERT(iSlot < ELEMENT_TOWER_MAX_POSITION_NUM);
	
	if (slotInfo.userInfo.dwUser != 0)
	{
		//获取关卡信息
		G_TowerCheckPoint gtcp = {0};
		if ( G_GetTowerCheckPoint(iFloor, &gtcp))
		{
			ElementTowerResMaps::iterator it = m_elementTowerResMap.find(gtcp.iAreaID);
			if (it != m_elementTowerResMap.end())
			{
				ElementTowerResMap::iterator itt =  (*it).second.find(iFloor);
				if (itt != (*it).second.end())
				{
					(*itt).second.slotInfo[iSlot] = slotInfo;
					InsertUserFind(slotInfo.userInfo.dwUser, gtcp.iAreaID, iFloor, iSlot);
				}
			}

		}
	}
}

//得到元素之塔的数据准备写往数据库
CT_VOID CCTowerResManager::GetTowerDataToDB( std::string& data)
{
	_AUTOMUTEX(&m_mutex);
	G_TowerAreaList gtal = {0};
	G_GetTowerAreaList(&gtal);

	data = "delete from towerslot; insert into towerslot( U_ID, CPID, SlotID, BeginTime, HelpFriend1, HelpFriend2, HelpFriend3, HelpFriend4, HelpFriend5, HelpFriend6, HelpFriend7, HelpFriend8, HelpFriend9, HelpFriend10) values";
	for (CT_INT32 i=0; i<gtal.iCount; ++i)
	{
		ElementTowerResMaps::iterator it = m_elementTowerResMap.find(gtal.iList[i] );
		if (it != m_elementTowerResMap.end())
		{
			ElementTowerResMap::iterator iter = (*it).second.begin();
			ElementTowerResMap::iterator iterEnd = (*it).second.end();

			for (; iter!=iterEnd; ++iter)
			{
				for (CT_INT32 j=0; j<ELEMENT_TOWER_MAX_POSITION_NUM; ++j)
				{
					CT_CHAR buf[512] = {0};
					sprintf(buf, "(%i,%i,%i,%lld,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i),\n",
						(*iter).second.slotInfo[j].userInfo.dwUser, (*iter).first, j, (*iter).second.slotInfo[j].llBeginTime,
						(*iter).second.slotInfo[j].iHelpFriend[0],
						(*iter).second.slotInfo[j].iHelpFriend[1],
						(*iter).second.slotInfo[j].iHelpFriend[2],
						(*iter).second.slotInfo[j].iHelpFriend[3],
						(*iter).second.slotInfo[j].iHelpFriend[4],
						(*iter).second.slotInfo[j].iHelpFriend[5],
						(*iter).second.slotInfo[j].iHelpFriend[6],
						(*iter).second.slotInfo[j].iHelpFriend[7],
						(*iter).second.slotInfo[j].iHelpFriend[8],
						(*iter).second.slotInfo[j].iHelpFriend[9]);

					data += buf;
				}
			}
		}
	}

	size_t pos = data.rfind(",");
	if (pos != std::string::npos)
	{
		data.erase(pos);
	}
	data += ";";
}


//得到塔信息，登录时候发往用户
CT_BOOL CCTowerResManager::GetUserResData(CT_DWORD dwUserID, CT_CHAR* pBuf, CT_INT32& bufLen)
{
	_AUTOMUTEX(&m_mutex);	

	CT_INT32 num = sizeof(CMD_G_ElementTowerInfo);		
	if (num > bufLen)
	{
		return CT_FALSE;
	}
	

	CT_INT32 iTowerID = 0;
	CT_INT32 iFloor   = 0;
	CT_INT32 iSlot    = 0;
	if(FindUserInTower(dwUserID, iTowerID, iFloor, iSlot))
	{
		ElementTowerResMaps::iterator it = m_elementTowerResMap.find(iTowerID);
		if (it != m_elementTowerResMap.end())
		{
			ElementTowerResMap::iterator itt = (*it).second.find(iFloor);
			if (itt != (*it).second.end())
			{
				CMD_G_ElementTowerInfo towerInfo = {0};
				towerInfo.iTower = iTowerID ;
				towerInfo.iFoor  = iFloor;
				G_TowerCheckPoint tcp = {0};
				if(!G_GetTowerCheckPoint(iFloor, &tcp))
				{
					return CT_FALSE;
				}

				memset(towerInfo.slotInfo, 0, sizeof(towerInfo.slotInfo));
				for (CT_INT32 j=0; j<ELEMENT_TOWER_MAX_POSITION_NUM; ++j)
				{
					G_TowerProductReward tpr = {0};
					if (!G_GetTowerProductReward(tcp.iProductID, j+1, &tpr))
					{
						break;
					}
					if ((*itt).second.slotInfo[j].userInfo.dwUser != 0)
					{
						towerInfo.slotInfo[j].iLeftTime = tpr.iOccupyTime - ((CT_INT32)(time(0) - (*itt).second.slotInfo[j].llBeginTime));
					}else
					{
						towerInfo.slotInfo[j].iLeftTime = tpr.iOccupyTime;
					}
					memcpy(&towerInfo.slotInfo[j].iHelpFriend, &((*itt).second.slotInfo[j].iHelpFriend), sizeof(CT_DWORD)*ELEMENT_TOWER_MAX_FRIED_HELP_NUM );
					memcpy(&towerInfo.slotInfo[j].userInfo,    &((*itt).second.slotInfo[j].userInfo), sizeof(ElemTowerUserInfo));
				}
                memcpy(pBuf, &towerInfo, sizeof(towerInfo));
				bufLen = num;
				return CT_TRUE;
			}
		}
	}
	return CT_FALSE;
}

//得到塔信息
CT_BOOL CCTowerResManager::GetTowerResData(CT_INT32 iTower, CT_INT32 iFloorLow, CT_INT32 iFloorHight, CT_CHAR* pBuf, CT_INT32& bufLen)
{
	_AUTOMUTEX(&m_mutex);
	ElementTowerResMaps::iterator itt = m_elementTowerResMap.find(iTower);
	if (itt != m_elementTowerResMap.end())
	{	
		if ( (CT_INT32)((iFloorHight - iFloorLow + 1) * sizeof(CMD_G_ElementTowerInfo)) > bufLen)
		{
			return CT_FALSE;
		}

		CMD_G_ElementTowerInfo towerInfo = {0};
		ElementTowerResMap::iterator iter    = (*itt).second.begin();
		ElementTowerResMap::iterator iterEnd = (*itt).second.end();
		
		CT_INT32 i=0;
		for (; iter!= iterEnd; ++iter)
		{
			if ((*iter).first >= iFloorLow && (*iter).first <= iFloorHight)
			{
				towerInfo.iTower = iTower;
				towerInfo.iFoor  = (*iter).first;

				G_TowerCheckPoint tcp = {0};
				if(!G_GetTowerCheckPoint(towerInfo.iFoor, &tcp))
				{
					return CT_FALSE;
				}

				memset(towerInfo.slotInfo, 0, sizeof(towerInfo.slotInfo));
				for (CT_INT32 j=0; j<ELEMENT_TOWER_MAX_POSITION_NUM; ++j)
				{
					G_TowerProductReward tpr = {0};
					if (!G_GetTowerProductReward(tcp.iProductID, j+1, &tpr))
					{
						break;;
					}

					if ((*iter).second.slotInfo[j].userInfo.dwUser != 0)
					{
						towerInfo.slotInfo[j].iLeftTime = tpr.iOccupyTime - ((CT_INT32)(time(0) - (*iter).second.slotInfo[j].llBeginTime));
					}else
					{
						towerInfo.slotInfo[j].iLeftTime = tpr.iOccupyTime;
					}
					memcpy(&towerInfo.slotInfo[j].iHelpFriend, &((*iter).second.slotInfo[j].iHelpFriend), sizeof(CT_DWORD)*ELEMENT_TOWER_MAX_FRIED_HELP_NUM );
					memcpy(&towerInfo.slotInfo[j].userInfo,    &((*iter).second.slotInfo[j].userInfo), sizeof(ElemTowerUserInfo));
				}

				memcpy(pBuf + i*sizeof(towerInfo), &towerInfo, sizeof(towerInfo) );
				++i;
			}
		}

		bufLen =  sizeof(CMD_G_ElementTowerInfo) * i;
		return CT_TRUE;
	}
	return CT_FALSE;
}

//检查资源生产
CT_VOID CCTowerResManager::UpdateSlotProduct(CT_BOOL bForce )
{
	_AUTOMUTEX(&m_mutex);
	CT_LONGLONG timenow = time(0);
	ElementTowerResMaps::iterator itt = m_elementTowerResMap.begin();
	ElementTowerResMaps::iterator ittEnd = m_elementTowerResMap.end();
	for (; itt != ittEnd; ++itt)
	{
		ElementTowerResMap::iterator iter    = (*itt).second.begin();
		ElementTowerResMap::iterator iterEnd = (*itt).second.end();
		for ( ; iter!= iterEnd; ++iter)
		{

			G_TowerCheckPoint gtcp = {0};
			if(!G_GetTowerCheckPoint((*iter).first, &gtcp))
			{
				//CT_ASSERT(CT_FALSE);
				CCLogMeg::getSingleton().WriteError("CCTowerResManager::UpdateSlotProduct 没有找到关卡数据 cpid=%i",  (*iter).first);
				continue;;
			}

			for (CT_INT32 i=0; i<ELEMENT_TOWER_MAX_POSITION_NUM; ++i)
			{
				MSC_DB_TowerSlotReward  towerSlotReword = {0};
				if(CalHarvastRes(  (*iter).first, i, (*iter).second.slotInfo[i], gtcp.iProductID,  timenow,  CT_FALSE, towerSlotReword))
				{
					CT_DWORD dwUserID = 0;
					dwUserID = (*iter).second.slotInfo[i].userInfo.dwUser;
					Harvast( (*iter).first, i, dwUserID, towerSlotReword, 1 );
					EaseUserInTower(dwUserID, (*itt).first, (*iter).first, i);
					CMD_G_ElementFloor elementFloor = {0};
					elementFloor.iTower = gtcp.iAreaID;
					elementFloor.iFoor  = gtcp.iCPID;
					elementFloor.iSlotID = i;
					elementFloor.slotInfo.iLeftTime = 0;
					CCTowerResManager::getSingleton().BroadCastInTower(gtcp.iAreaID, MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FLOOR_CHANGE, (CT_CHAR*)&elementFloor, sizeof(elementFloor));
					CCUser *pUser=0;
					if (CCUserManager::getSingleton().Find(dwUserID, &pUser))
					{
						pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FLOOR_CHANGE, (CT_CHAR*)&elementFloor, sizeof(elementFloor));
					}
				}
			}
		}
	}
}


//放弃资源
CT_VOID CCTowerResManager::GiveUpSlot(CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlot)
{
	_AUTOMUTEX(&m_mutex);
	ElementTowerResMaps::iterator it = m_elementTowerResMap.find(iTower);

	if (it !=  m_elementTowerResMap.end())
	{
		ElementTowerResMap::iterator iter    = (*it).second.find(iFloor);
		if( iter!= (*it).second.end() )
		{
			if ( iSlot >= ELEMENT_TOWER_MAX_POSITION_NUM || iSlot < 0 ||  (*iter).second.slotInfo[iSlot].userInfo.dwUser == 0 )
			{
				return;
			}

			G_TowerCheckPoint gtcp = {0};
			if(!G_GetTowerCheckPoint((*iter).first, &gtcp))
			{
				//CT_ASSERT(CT_FALSE);
				CCLogMeg::getSingleton().WriteError("CCTowerResManager::GiveUpSlot 没有找到关卡数据 cpid=%i",  (*iter).first);
				return;
			}

			MSC_DB_TowerSlotReward  towerSlotReword = {0};
			if(CalHarvastRes(  iFloor,  iSlot, (*iter).second.slotInfo[iSlot], gtcp.iProductID,  time(0),  CT_TRUE, towerSlotReword))
		    {
				G_ValueRange vr = {0};
				if (!G_GetValueRange(VALUE_TYPE_TOWER_GIVEUP_LOSE_RATE_NUM, &vr))
				{
					CCLogMeg::getSingleton().WriteError(" G_GetValueRange VALUE_TYPE_TOWER_GIVEUP_LOSE_RATE_NUM = %i", VALUE_TYPE_TOWER_GIVEUP_LOSE_RATE_NUM);
					return;
				}
				Harvast(iFloor, iSlot,(*iter).second.slotInfo[iSlot].userInfo.dwUser, towerSlotReword, 1 - vr.fMaxVal );
		    }

			//memset( &((*iter).second.slotInfo[iSlot]), 0, sizeof( ElementTowerSlotInfo));
			CCTowerResManager::getSingleton().EaseUserInTower((*iter).second.slotInfo[iSlot].userInfo.dwUser, iTower, iFloor, iSlot);
			CMD_G_ElementFloor eleFloor = {0};
			eleFloor.iTower = iTower;
			eleFloor.iFoor  = iFloor;
			eleFloor.iSlotID = iSlot;
			BroadCastInTower(iTower, MAIN_G_ELEMENT_TOWER, SUB_G_ELEMENT_TOWER_FLOOR_CHANGE , (CT_CHAR*)&eleFloor, sizeof(eleFloor));
		}
	}
}

//结算
CT_VOID CCTowerResManager::Harvast( CT_INT32 iFloor, CT_INT32 iSlot,
								   CT_DWORD dwUserID, 
								   const MSC_DB_TowerSlotReward&  towerSlotReward, CT_FLOAT rate )
{
	_AUTOMUTEX(&m_mutex);
	if (dwUserID != 0)
	{
		CCUser* pUser = 0;
		if (CCUserManager::getSingleton().Find(dwUserID, &pUser))
		{
			TowerSlotAward tslotAward = {0};
			tslotAward.dwUserID = dwUserID;
			tslotAward.iFloor   = iFloor;				    //层
			tslotAward.iSlot    = iSlot;                    //资源点		
			tslotAward.gReward.iGetResType1 =   towerSlotReward.mnReward.iGetResType1;
			tslotAward.gReward.iGetResNum1	=	towerSlotReward.mnReward.iGetResNum1 * rate ;
			tslotAward.gReward.iGetResType2	=	towerSlotReward.mnReward.iGetResType2;
			tslotAward.gReward.iGetResNum2	=	towerSlotReward.mnReward.iGetResNum2 * rate ;
			tslotAward.gReward.iGetResType3	=	towerSlotReward.mnReward.iGetResType3;
			tslotAward.gReward.iGetResNum3	=	towerSlotReward.mnReward.iGetResNum3 * rate ;
			tslotAward.gReward.iGetResType4	=	towerSlotReward.mnReward.iGetResType4;
			tslotAward.gReward.iGetResNum4	=	towerSlotReward.mnReward.iGetResNum4  * rate;
										
			tslotAward.gReward.iGetItemID1	=	towerSlotReward.mnReward.iGetItemID1;
			tslotAward.gReward.iGetItemNum1	=	towerSlotReward.mnReward.iGetItemNum1 * rate;
			tslotAward.gReward.iGetItemID2	=	towerSlotReward.mnReward.iGetItemID2;
			tslotAward.gReward.iGetItemNum2	=	towerSlotReward.mnReward.iGetItemNum2 * rate;
			tslotAward.gReward.iGetItemGroupID1 = towerSlotReward.mnReward.iGetItemGroupID1;
			tslotAward.gReward.iGetItemGroupNum1 = towerSlotReward.mnReward.iGetItemGroupNum1 * rate;
			tslotAward.gReward.iGetItemGroupID2 = towerSlotReward.mnReward.iGetItemGroupID2;
			tslotAward.gReward.iGetItemGroupNum2 = towerSlotReward.mnReward.iGetItemGroupNum2 * rate;
			pUser->m_userTower.InsertTowerSlotAward( tslotAward);

		}else
		{
// 			CCGameMesEvent::SendThreadInfo(towerSlotReward.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum , 
// 				MSC_MAIN_DB_TOWER_DATA,  MSC_SUB_DB_TOWER_SLOT_REWARD, (CT_CHAR*)&towerSlotReward, sizeof(towerSlotReward) );
			CT_DWORD threadid = GAME_THREAD_DATABASE + towerSlotReward.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
			G_SendThreadInfo(threadid, MSC_MAIN_DB_TOWER_DATA,  MSC_SUB_DB_TOWER_SLOT_REWARD, (CT_CHAR*)&towerSlotReward, sizeof(towerSlotReward) );
		}
	}
}


//结算
CT_BOOL CCTowerResManager::CalHarvastRes( CT_INT32 iFoor, CT_INT32 iSlot,
										CCTowerResManager::ElementTowerSlotInfo& SlotInfo, 
										CT_INT32 iProductID, CT_LONGLONG timenow, CT_BOOL bFroce,
										MSC_DB_TowerSlotReward& towerSlotReward )
{
	_AUTOMUTEX(&m_mutex);
	if (SlotInfo.userInfo.dwUser != 0)
	{
		G_TowerProductReward  gtpr = {0};
		if(!G_GetTowerProductReward(iProductID, iSlot+1, &gtpr))
		{
			CCLogMeg::getSingleton().WriteError("CCTowerResManager::CheckSlotProduct productID=%i, slot=%i", iProductID, iSlot);
			//CT_ASSERT(false);
			return CT_FALSE;
		}

		if (timenow - SlotInfo.llBeginTime < gtpr.iOccupyTime && !bFroce)
		{
			return CT_FALSE;
		}

		CT_INT32 iNum = 0;
		if (timenow - SlotInfo.llBeginTime >= gtpr.iOccupyTime)
		{
			iNum =  gtpr.iOccupyTime / gtpr.iProductTime;
		}
		else
		{
			iNum = (timenow - SlotInfo.llBeginTime) / gtpr.iProductTime;
		}

		CT_INT32 iHelpNum = 0;
		for (CT_INT32 j=0; j<ELEMENT_TOWER_MAX_FRIED_HELP_NUM; ++j)
		{
			if (SlotInfo.iHelpFriend[j] != 0)
			{
				++iHelpNum;
			}
		}
		iNum +=iHelpNum;  


		Resoure  res[TEAM_RES_CNT] = {0}; 
		Item     item[TEAM_ITEM_CNT] = {0};
		CT_INT32 iGoods[GOODS_COUNT*2] = {0};
		CT_INT32 iCount[GOODS_COUNT*2] = {0};
		FakeProBase fb={0};

		G_GetReward(gtpr.iGetGroupID, res, item, iGoods, iCount,fb);

		towerSlotReward.dwUserID = SlotInfo.userInfo.dwUser;
		towerSlotReward.iFloor   = iFoor;
		towerSlotReward.iSlot    = iSlot;

		towerSlotReward.mnReward.iGetResType1 = res[0].iID;
		towerSlotReward.mnReward.iGetResNum1  = res[0].iNum * iNum;
		towerSlotReward.mnReward.iGetResType2 = res[1].iID;
		towerSlotReward.mnReward.iGetResNum2  = res[1].iNum * iNum;
		towerSlotReward.mnReward.iGetResType3 = res[2].iID;
		towerSlotReward.mnReward.iGetResNum3  = res[2].iNum * iNum;
		towerSlotReward.mnReward.iGetResType4 = res[3].iID;
		towerSlotReward.mnReward.iGetResNum4  = res[3].iNum* iNum;

		towerSlotReward.mnReward.iGetItemID1  = item[0].iID;
		towerSlotReward.mnReward.iGetItemNum1 = item[0].iNum * iNum;
		towerSlotReward.mnReward.iGetItemID2  = item[1].iID;
		towerSlotReward.mnReward.iGetItemNum2 = item[1].iNum* iNum;

		return CT_TRUE;

	}
	return CT_FALSE;
}

//重启时加载最早通关记录
CT_VOID CCTowerResManager::InitTowerFightExample(MSC_TowerCheckPoint_Fighting& fighting, CT_INT32 iFID)
{
	_AUTOMUTEX(&m_mutex);

	TowerFightingMap::iterator it = m_towerFightingMap.find(fighting.iCPID);
	if (it != m_towerFightingMap.end())
	{
		it->second.insert(CPFightingMap::value_type(iFID, fighting));
	}
	else
	{
		CPFightingMap cpFightingMap;
		cpFightingMap.clear();
		cpFightingMap.insert(CPFightingMap::value_type(iFID, fighting));
		m_towerFightingMap.insert(TowerFightingMap::value_type(fighting.iCPID, cpFightingMap));
	}
}

//插入最早通关记录
CT_INT32 CCTowerResManager::InsertTowerFightExample(MSC_TowerCheckPoint_Fighting& fighting)
{
	_AUTOMUTEX(&m_mutex);
	CT_INT32 iFID= 0;

	TowerFightingMap::iterator it = m_towerFightingMap.find(fighting.iCPID);
	if (it != m_towerFightingMap.end())
	{
		for (CPFightingMap::iterator it_cp = it->second.begin(); it_cp != it->second.end(); it_cp++)
		{
			if (it_cp->second.dwUserID == fighting.dwUserID)
			{
				return 0;
			}
		}
		iFID = it->second.size();
		if (iFID <3)
		{
			it->second.insert(CPFightingMap::value_type(++iFID, fighting));
		}
		else
			return 0;
	}
	else
	{
		CPFightingMap cpFightingMap;
		cpFightingMap.clear();
		cpFightingMap.insert(CPFightingMap::value_type(++iFID, fighting));
		m_towerFightingMap.insert(TowerFightingMap::value_type(fighting.iCPID, cpFightingMap));
	}

	MSC_DB_TowerFight dbTowerFight = {0};
	dbTowerFight.iCPID = fighting.iCPID;
	dbTowerFight.iFID = iFID;
	dbTowerFight.dwUserID = fighting.dwUserID;
	dbTowerFight.iSeed = fighting.iSeed;
	dbTowerFight.iScore = fighting.iScore;
	dbTowerFight.FightingData = fighting.FightingData;
// 		CCGameMesEvent::SendThreadInfo(dbTowerFight.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum , 
// 			MSC_MAIN_DB_TOWER_DATA,  MSC_SUB_DB_TOWER_FIGHT, (CT_CHAR*)&dbTowerFight, sizeof(MSC_DB_TowerFight) );

	CT_DWORD threadid = GAME_THREAD_DATABASE + dbTowerFight.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_DB_TOWER_DATA,MSC_SUB_DB_TOWER_FIGHT,(CT_CHAR*)&dbTowerFight,sizeof(MSC_DB_TowerFight));

	return iFID;
}

CT_BOOL  CCTowerResManager::GetTowerFightExample(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(&m_mutex);

	CT_INT32 iCount=0;
	TowerFightingMap::iterator it = m_towerFightingMap.begin();
	TowerFightingMap::iterator it_end = m_towerFightingMap.end();
	for (; it != it_end; it++)
	{
		iCount += it->second.size();
	}
	CT_INT32 len = iCount * sizeof(CMD_G_TowerFightExample);

	if (len > bufLen || len == 0)
	{
		return CT_FALSE;
	}

	it = m_towerFightingMap.begin();

	CT_INT32 i=0;
	CMD_G_TowerFightExample towerFightExample = {0};
	for (; it != it_end; it++)
	{
		for (CPFightingMap::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
			CombatUnitData data={0};
			towerFightExample.iCPID = it->first;
			towerFightExample.iTime = it2->first;
			towerFightExample.dwUserID = it2->second.dwUserID;
			G_GetMasterFightingData(it2->second.FightingData,data);
			memcpy(towerFightExample.cNickName, data.name, NAME_SIZE);
			memcpy(pBuf + sizeof(CMD_G_TowerFightExample) * i++, &towerFightExample, sizeof(CMD_G_TowerFightExample));
		}
	}
	bufLen = len;
	return CT_TRUE;
}
CT_BOOL  CCTowerResManager::GetTowerFightExample(CT_INT32 iCPID, CT_INT32 iFID,MSC_TowerCheckPoint_Fighting& fighting)
{
	_AUTOMUTEX(&m_mutex);
	TowerFightingMap::iterator it = m_towerFightingMap.find(iCPID);
	if (it != m_towerFightingMap.end())
	{
		CPFightingMap::iterator it2 = it->second.find(iFID);
		if (it2 != it->second.end())
		{
			fighting = it2->second;
			return CT_TRUE;
		}
		return CT_TRUE;
	}
	return CT_FALSE;
}

//战斗数据查询用户是否存在
CT_BOOL CCTowerResManager::FindTowerUser(const CT_DWORD dwUserID)
{
	_AUTOMUTEX(&m_mutex);
	TowerFightingMap::iterator itend=m_towerFightingMap.end();
	for(TowerFightingMap::iterator it=m_towerFightingMap.begin();it != itend;++it)
	{
		CPFightingMap::iterator cpitend=(*it).second.end();
		for(CPFightingMap::iterator cpit=(*it).second.begin();cpit != cpitend;++cpit)
		{
			if(dwUserID == (*cpit).second.dwUserID)
			{
				return CT_TRUE;
			}
		}
	}
	return CT_FALSE;
}