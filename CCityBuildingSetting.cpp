#include "CCityBuildingSetting.h"

template<> CCCityBuildingSetting* CCSingleton<CCCityBuildingSetting>::ms_Singleton=0;

CCCityBuildingSetting::CCCityBuildingSetting(void)
{
}

CCCityBuildingSetting::~CCCityBuildingSetting(void)
{
}


//得到建筑升级信息
CT_BOOL CCCityBuildingSetting::GetBuildingUpgradeDataWithID(CT_INT32 buildingID, CT_INT32 grade, G_BuildingUpgrade* pBu)
{
	G_Building building;
	if (G_GetBuildingData(buildingID, & building))
	{
		if(G_GetBuildingUpgradeData(building.iBT, grade, pBu ))
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}


//根据建筑id得到建筑建造信息
CT_BOOL CCCityBuildingSetting::GetBuildTypeWithID(CT_INT32 bID, G_BuildingType& bt)
{
	G_Building building;
	if (G_GetBuildingData(bID, & building))
	{
		if(G_GetBuildingTypeData(building.iBT, &bt ))
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//得到建筑类型数据
CT_BOOL  CCCityBuildingSetting::GetBuildingTypeData(int iType, G_BuildingType* pGBT)
{
	return (CT_BOOL)G_GetBuildingTypeData(iType, pGBT);
}

//得到建筑数据
CT_BOOL  CCCityBuildingSetting::GetBuildingData(int iBuildID,  G_Building* pGB)
{
	return (CT_BOOL)G_GetBuildingData(iBuildID, pGB);
}

//得到建筑升级数据
CT_BOOL  CCCityBuildingSetting::GetBuildingUpgradeData(int iType, int iGrade, G_BuildingUpgrade* pBu)
{
	return (CT_BOOL)G_GetBuildingUpgradeData(iType, iGrade, pBu);
}
