/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCBuildingScriptSetting.h
Version:		1.0
Description:    城市建筑脚本
Author:			pengwen
Create Date:	2012-8-20
History:        城市建筑脚本     

******************************************************************/

#ifndef CCBUILDINGSCRIPTSETTING_H_
#define CCBUILDINGSCRIPTSETTING_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"

struct MasterLevelInfo;
class CCBuildingScriptSetting : public CCSingleton<CCBuildingScriptSetting>
{
public:
	CCBuildingScriptSetting(void);
	~CCBuildingScriptSetting(void);

public:
	//根据建筑id得到建筑升级信息
	 CT_BOOL GetBuildingUpgradeDataWithID(CT_INT32 buildingID, CT_INT32 grade, G_BuildingUpgrade* pBu);
    //根据建筑id得到建筑类型，建造信息
	 CT_BOOL GetBuildTypeWithID(CT_INT32 bID, G_BuildingType* bt);
    //得到建筑类型数据
	 CT_BOOL  GetBuildingTypeData(CT_INT32 iType, G_BuildingType* pGBT);
	//得到建筑数据
	 CT_BOOL  GetBuildingData(CT_INT32 iBuildID,  G_Building* pGB);
	//得到建筑升级数据
	 CT_BOOL  GetBuildingUpgradeData(CT_INT32 iType, CT_INT32 iGrade, G_BuildingUpgrade* pBu);

public:
	//得到文明升级数据
	 CT_BOOL  GetCivilUpgradeData(CT_INT32 iGrade,  MasterLevelInfo* cu);
	//查找生产内容
	 CT_BOOL  GetProductInfo(CT_INT32 ProductID, G_BuildingProductInfo* pb);
	//得到生产类型
	 CT_BOOL  GetProductType(CT_INT32 ProductID, G_BuildingProduct* pBp);
	//得到生产类型
	 CT_BOOL  GetProductType(CT_INT32 buildType, CT_INT32 iGrade,  G_BuildingProduct* pBp);
	//得到文明升级效果数据
	 CT_BOOL  GetBuildingUpgradeEffectData(CT_INT32 iType, CT_INT32 iGrade, G_BuildingUpgrdeEffect* pBUE);
	 //得到洞穴生产内容
	 CT_BOOL  GetPetCave(CT_INT32 ProductID,CT_INT32 iGrade,G_PetCave *pPetCave);
};

#endif