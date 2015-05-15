#include "CMD_Game.h"
#include "GlobeScriptInterface.h"
#include "CLogMeg.h"
#include "CUser.h"
#include "CDatabaseMes.h"
#include "CMasterScriptSetting.h"
#include "CBuildingScriptSetting.h"
#include "CGameConfig.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CNetInterface.h"
#include "CRegion.h"

CCUserRegion::CCUserRegion()
{
}

CCUserRegion::~CCUserRegion()
{
}

CT_BOOL	CCUserRegion::GetRegionStatus(CT_INT32 rid)
{
	UR_IT it = m_regionStatusVec.begin();
	UR_IT itend = m_regionStatusVec.end();
	for(;it!=itend;++it)
	{
		if (*it==rid)
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_BOOL	CCUserRegion::GetRegionExist(CT_INT32 rid,CT_INT32 iRegionList[])
{
	for (int i=0;i<REGION_LENEX;++i)
	{
		if (rid==iRegionList[i])
		{
			return CT_TRUE;
		}
		else if (iRegionList[i]==0)
		{
			return CT_FALSE;
		}
	}
	return CT_FALSE;
}

CT_BOOL	CCUserRegion::GetRegionLink(CT_INT32 iRegionLink[])
{
	for(CT_INT32 i = 0;i<REGION_LENEX;++i)
	{
		if(iRegionLink[i]==0)
		{
			return CT_FALSE;
		}
		if(GetRegionStatus(iRegionLink[i]))
		{
			return CT_TRUE;
		}
	}
	return CT_FALSE;
}

CT_BOOL CCUserRegion::GetUnlockRegionData(CT_CHAR *pBuf,CT_INT32 &bufLen)
{
	int i = m_regionStatusVec.size();
	if (i<=0 || bufLen < m_regionStatusVec.size()*sizeof(int))
	{
		return CT_FALSE;
	}
	for (int j=0;j<i;j++)
	{
		memcpy(pBuf+j*sizeof(int),&m_regionStatusVec[j],sizeof(int));
	}
	bufLen = sizeof(int)*i;
	return CT_TRUE;
}