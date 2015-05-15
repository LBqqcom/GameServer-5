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
	//��ȡ������������
	CT_BOOL GetRegionIDByBuildID(int iBuildID,int *iRegionID);
	//��ȡ���������Դ
	CT_BOOL GetRegionResource(int iRegion,G_RegionResource *pRegionResource);
	//��ȡ�����ڽ�����
	CT_BOOL GetRegionLink(int iRegionID,G_RegionList *pRegionLink);
	//��ȡ��������id
	CT_VOID GetRegionList(G_RegionList *pRegionList);
};

#endif