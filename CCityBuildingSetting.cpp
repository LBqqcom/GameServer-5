#include "CCityBuildingSetting.h"

template<> CCCityBuildingSetting* CCSingleton<CCCityBuildingSetting>::ms_Singleton=0;

CCCityBuildingSetting::CCCityBuildingSetting(void)
{
}

CCCityBuildingSetting::~CCCityBuildingSetting(void)
{
}


//�õ�����������Ϣ
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


//���ݽ���id�õ�����������Ϣ
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

//�õ�������������
CT_BOOL  CCCityBuildingSetting::GetBuildingTypeData(int iType, G_BuildingType* pGBT)
{
	return (CT_BOOL)G_GetBuildingTypeData(iType, pGBT);
}

//�õ���������
CT_BOOL  CCCityBuildingSetting::GetBuildingData(int iBuildID,  G_Building* pGB)
{
	return (CT_BOOL)G_GetBuildingData(iBuildID, pGB);
}

//�õ�������������
CT_BOOL  CCCityBuildingSetting::GetBuildingUpgradeData(int iType, int iGrade, G_BuildingUpgrade* pBu)
{
	return (CT_BOOL)G_GetBuildingUpgradeData(iType, iGrade, pBu);
}
