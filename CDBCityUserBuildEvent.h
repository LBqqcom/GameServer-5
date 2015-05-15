/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDBCityUserBuildEvent.h
Version:		1.0
Description:    ���н�����������db��
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
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
								CT_CHAR* pBuf,		CT_DWORD len,
								CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{
	}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//����������
	//CT_VOID DisposeUpgradeBuilding(const MSC_DB_BuildUpgrade& buildCreate);
	//�����������
	//CT_VOID DisposeBuildUpgradeFinish(const MSC_DB_BuildUpgradeFinish& upgradeFinish);
	//���潨����Ϣ
    CT_VOID DisposeBuildSaveInfo(CT_DWORD subID,  CT_CHAR* pBuf,	    CT_DWORD len);
	//���潨����Ϣ
	CT_VOID DisposeSaveBuildingInfo(const MSC_DB_BuildingInfo& buildingInfo);
	//����������Ϣ
	CT_VOID DisposeSaveBuildingUpgradingInfo(const MSC_DB_BuildingUpgradingInfo&  bui);
	//ͬ������������Ϣ������ɾ�����ݹ�������û�����������Ϣ
	CT_VOID DisposeSysBuildingUpgrading(const MSC_DB_SysBuildingUpgrading& sysBuildingUpgrade);
};

#endif
