/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCCityBuildingSetting.h
Version:		1.0
Description:    ���н����ű�
Author:			pengwen
Create Date:	2012-8-20
History:        ���н����ű�     

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
	//���ݽ���id�õ�����������Ϣ
	CT_BOOL GetBuildingUpgradeDataWithID(CT_INT32 buildingID, CT_INT32 grade, G_BuildingUpgrade* pBu);
    //���ݽ���id�õ��������ͣ�������Ϣ
	CT_BOOL GetBuildTypeWithID(CT_INT32 bID, G_BuildingType& bt);
    //�õ�������������
	CT_BOOL  GetBuildingTypeData(int iType, G_BuildingType* pGBT);
	//�õ���������
	CT_BOOL  GetBuildingData(int iBuildID,  G_Building* pGB);
	//�õ�������������
	CT_BOOL  GetBuildingUpgradeData(int iType, int iGrade, G_BuildingUpgrade* pBu);
};

#endif