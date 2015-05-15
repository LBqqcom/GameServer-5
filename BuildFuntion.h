/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		BuildFuntion.h
Version:		1.0
Description:    城市建筑功能函数
Author:			pengwen
Create Date:	2012-9-17
History:        城市建筑功能函数     

******************************************************************/

#ifndef CBUILDFUNTION_H_
#define CBUILDFUNTION_H_
#include "CTType.h"
class CCUser;
struct G_BuildingProductInfo;
struct G_BuildingType;
struct ProductingBuilding;
struct G_PetCave;

//处理收获完成
CT_VOID G_CityFinishProduct(CCUser* pUser, const CT_DWORD dwThread, const G_BuildingProductInfo& bpi, const G_BuildingType& bt, const ProductingBuilding& pb);
//处理洞穴收获
CT_VOID G_CityFinishCave(CCUser* pUser, const CT_DWORD dwThread, const G_BuildingType& bt, const G_PetCave &pc,const ProductingBuilding& pb);
//收获失败
CT_VOID G_CityHarvestGainFail(CCUser *pUser ,CT_INT32 iBID, int pf);

#endif