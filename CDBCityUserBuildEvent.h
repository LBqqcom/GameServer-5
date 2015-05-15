/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBCityUserBuildEvent.h
Version:		1.0
Description:    城市建筑升级数据db类
Author:			pengwen
Create Date:	2012-8-16
History:             

******************************************************************/


#ifndef CCDBCITYUSERBUILD_H_
#define CCDBCITYUSERBUILD_H_

#include "CGameMesEvent.h"

class CCDBConnector;
//struct MSC_DB_BuildUpgrade;
//struct MSC_DB_BuildUpgradeFinish;
//struct MSC_DB_MasterSaveData;
struct MSC_DB_BuildingInfo;
struct MSC_DB_BuildingUpgradingInfo;
struct MSC_DB_SysBuildingUpgrading;

class CCDBCityUserBuildEvent :
	public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;

private:
	CCDBCityUserBuildEvent();
public:
	CCDBCityUserBuildEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBCityUserBuildEvent(void);
private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
								CT_CHAR* pBuf,		CT_DWORD len,
								CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{
	}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//处理建筑升级
	//CT_VOID DisposeUpgradeBuilding(const MSC_DB_BuildUpgrade& buildCreate);
	//处理升级完成
	//CT_VOID DisposeBuildUpgradeFinish(const MSC_DB_BuildUpgradeFinish& upgradeFinish);
	//保存建筑信息
    CT_VOID DisposeBuildSaveInfo(CT_DWORD subID,  CT_CHAR* pBuf,	    CT_DWORD len);
	//保存建筑信息
	CT_VOID DisposeSaveBuildingInfo(const MSC_DB_BuildingInfo& buildingInfo);
	//保存升级信息
	CT_VOID DisposeSaveBuildingUpgradingInfo(const MSC_DB_BuildingUpgradingInfo&  bui);
	//同步建筑升级信息，就是删除数据关于这个用户的升级表信息
	CT_VOID DisposeSysBuildingUpgrading(const MSC_DB_SysBuildingUpgrading& sysBuildingUpgrade);
};

#endif
