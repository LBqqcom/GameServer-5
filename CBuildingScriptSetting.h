/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCBuildingScriptSetting.h
Version:		1.0
Description:    ���н����ű�
Author:			pengwen
Create Date:	2012-8-20
History:        ���н����ű�     

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
	//���ݽ���id�õ�����������Ϣ
	 CT_BOOL GetBuildingUpgradeDataWithID(CT_INT32 buildingID, CT_INT32 grade, G_BuildingUpgrade* pBu);
    //���ݽ���id�õ��������ͣ�������Ϣ
	 CT_BOOL GetBuildTypeWithID(CT_INT32 bID, G_BuildingType* bt);
    //�õ�������������
	 CT_BOOL  GetBuildingTypeData(CT_INT32 iType, G_BuildingType* pGBT);
	//�õ���������
	 CT_BOOL  GetBuildingData(CT_INT32 iBuildID,  G_Building* pGB);
	//�õ�������������
	 CT_BOOL  GetBuildingUpgradeData(CT_INT32 iType, CT_INT32 iGrade, G_BuildingUpgrade* pBu);

public:
	//�õ�������������
	 CT_BOOL  GetCivilUpgradeData(CT_INT32 iGrade,  MasterLevelInfo* cu);
	//������������
	 CT_BOOL  GetProductInfo(CT_INT32 ProductID, G_BuildingProductInfo* pb);
	//�õ���������
	 CT_BOOL  GetProductType(CT_INT32 ProductID, G_BuildingProduct* pBp);
	//�õ���������
	 CT_BOOL  GetProductType(CT_INT32 buildType, CT_INT32 iGrade,  G_BuildingProduct* pBp);
	//�õ���������Ч������
	 CT_BOOL  GetBuildingUpgradeEffectData(CT_INT32 iType, CT_INT32 iGrade, G_BuildingUpgrdeEffect* pBUE);
	 //�õ���Ѩ��������
	 CT_BOOL  GetPetCave(CT_INT32 ProductID,CT_INT32 iGrade,G_PetCave *pPetCave);
};

#endif