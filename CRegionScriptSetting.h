#ifndef CRegionScriptSetting_H_
#define CRegionScriptSetting_H_

#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"

struct G_RegionResource;

class CCRegionScriptSetting : public CCSingleton<CCRegionScriptSetting>
{
public:
	CCRegionScriptSetting(void);
	~CCRegionScriptSetting(void);
public:
	//获取建筑所属区域
	CT_BOOL GetRegionIDByBuildID(int iBuildID,int *iRegionID);
	//获取区域解锁资源
	CT_BOOL GetRegionResource(int iRegion,G_RegionResource *pRegionResource);
	//获取区域邻接区域
	CT_BOOL GetRegionLink(int iRegionID,G_RegionList *pRegionLink);
	//获取所有区域id
	CT_VOID GetRegionList(G_RegionList *pRegionList);
};

#endif