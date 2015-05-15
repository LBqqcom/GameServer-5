#include "CMoster.h"

CCMoster::CCMoster(void)
{
}

CCMoster::~CCMoster(void)
{
}

//获取怪物信息
MonsterBaseInfo CCMoster::GetMosterInfo(CT_INT32 iMosterID)
{
	MonsterBaseInfo Moster={0};
	//memset(&Moster,0,sizeof(Moster));

	return Moster;
}
//获得怪物信息
CT_BOOL	CCMoster::GetMosterData(MonsterBaseInfo *pMosterInfo)
{
	return true;
}
