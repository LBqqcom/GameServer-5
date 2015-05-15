/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCCityBuildingSetting.h
Version:		1.0
Description:    城市建筑脚本
Author:			pengwen
Create Date:	2012-8-20
History:        城市建筑脚本     

******************************************************************/

#ifndef CCCITYBUILDINGSETTING_H_
#define CCCITYBUILDINGSETTING_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"

class CCCityBuildingSetting : public CCSingleton<CCCityBuildingSetting>
{
public:
	CCCityBuildingSetting(void);
	~CCCityBuildingSetting(void);

public:
	//根据建筑id得到建筑升级信息
	CT_BOOL GetBuildingUpgradeDataWithID(CT_INT32 buildingID, CT_INT32 grade, G_BuildingUpgrade* pBu);
    //根据建筑id得到建筑类型，建造信息
	CT_BOOL GetBuildTypeWithID(CT_INT32 bID, G_BuildingType& bt);
    //得到建筑类型数据
	CT_BOOL  GetBuildingTypeData(int iType, G_BuildingType* pGBT);
	//得到建筑数据
	CT_BOOL  GetBuildingData(int iBuildID,  G_Building* pGB);
	//得到建筑升级数据
	CT_BOOL  GetBuildingUpgradeData(int iType, int iGrade, G_BuildingUpgrade* pBu);
};

#endif