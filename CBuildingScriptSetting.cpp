#include "CBuildingScriptSetting.h"
#include "CUser.h"

#ifdef _WIN32
#pragma warning(disable:4800)
#endif

template<> CCBuildingScriptSetting* CCSingleton<CCBuildingScriptSetting>::ms_Singleton=0;

CCBuildingScriptSetting::CCBuildingScriptSetting(void)
{
}

CCBuildingScriptSetting::~CCBuildingScriptSetting(void)
{
}


//�õ�����������Ϣ
CT_BOOL CCBuildingScriptSetting::GetBuildingUpgradeDataWithID(CT_INT32 buildingID, CT_INT32 grade, G_BuildingUpgrade* pBu)
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
CT_BOOL CCBuildingScriptSetting::GetBuildTypeWithID(CT_INT32 bID, G_BuildingType* bt)
{
	G_Building building;
	if (G_GetBuildingData(bID, & building))
	{
		if(G_GetBuildingTypeData(building.iBT, bt ))
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

//�õ�������������
CT_BOOL  CCBuildingScriptSetting::GetBuildingTypeData(CT_INT32 iType, G_BuildingType* pGBT)
{
	return (CT_BOOL)(G_GetBuildingTypeData(iType, pGBT));
}

//�õ���������
CT_BOOL  CCBuildingScriptSetting::GetBuildingData(CT_INT32 iBuildID,  G_Building* pGB)
{
	return (CT_BOOL)G_GetBuildingData(iBuildID, pGB);
}

//�õ�������������
CT_BOOL  CCBuildingScriptSetting::GetBuildingUpgradeData(CT_INT32 iType, CT_INT32 iGrade, G_BuildingUpgrade* pBu)
{
	return (CT_BOOL)G_GetBuildingUpgradeData(iType, iGrade, pBu);
}


//�õ�������������
CT_BOOL  CCBuildingScriptSetting::GetCivilUpgradeData(CT_INT32 iGrade,  MasterLevelInfo* ml)
{
	G_CivilUpgrade cu;
	memset(&cu, 0, sizeof(cu));
	CT_BOOL ret =  (CT_BOOL)G_GetCivilUpgrade(iGrade, &cu);
	ml->iLevel = cu.iCivilGrade;
	ml->iCurrentExp = cu.iCivilDegree;
	return ret;
}

//������������
CT_BOOL  CCBuildingScriptSetting::GetProductInfo(CT_INT32 ProductID, G_BuildingProductInfo* pb)
{
	return (CT_BOOL)G_GetProductInfo(ProductID, pb);
}


//�õ���������
CT_BOOL  CCBuildingScriptSetting::GetProductType(CT_INT32 ProductID, G_BuildingProduct* pBp)
{
	return (CT_BOOL)G_GetProductTypeByProductID(ProductID, pBp);
}

//�õ���������
CT_BOOL  CCBuildingScriptSetting::GetProductType(CT_INT32 buildType, CT_INT32 iGrade,  G_BuildingProduct* pBp)
{
	return (CT_BOOL)G_GetProductTypeByBuildTypeGrade(buildType, iGrade, pBp);
}

//�õ���������Ч������
CT_BOOL  CCBuildingScriptSetting::GetBuildingUpgradeEffectData(CT_INT32 iType, CT_INT32 iGrade, G_BuildingUpgrdeEffect* pBUE)
{
	return (CT_BOOL)G_GetBuildingUpgradeEffectData(iType, iGrade, pBUE);
}

//�õ���Ѩ��������
CT_BOOL  CCBuildingScriptSetting::GetPetCave(CT_INT32 ProductID,CT_INT32 iGrade,G_PetCave *pPetCave)
{
	return 0 !=  G_GetPetCave(ProductID,iGrade,pPetCave);
}