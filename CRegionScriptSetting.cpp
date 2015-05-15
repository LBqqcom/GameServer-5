#include "CRegionScriptSetting.h"
#include "CUser.h"

template<> CCRegionScriptSetting* CCSingleton<CCRegionScriptSetting>::ms_Singleton=0;

CCRegionScriptSetting::CCRegionScriptSetting(void)
{
}

CCRegionScriptSetting::~CCRegionScriptSetting(void)
{
}

CT_BOOL CCRegionScriptSetting::GetRegionIDByBuildID(int iBuildID, int *iRegionID)
{
	if (G_GetRegionIDByBuildID(iBuildID,iRegionID))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCRegionScriptSetting::GetRegionResource(int iRegion,G_RegionResource *pRegionResource)
{
	if (G_GetRegionResource(iRegion,pRegionResource))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCRegionScriptSetting::GetRegionLink(int iRegionID,G_RegionList *pRegionLink)
{
	if (G_GetRegionLink(iRegionID,pRegionLink))
	{
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_VOID CCRegionScriptSetting::GetRegionList(G_RegionList *pRegionList)
{
	G_GetRegionList(pRegionList);
}