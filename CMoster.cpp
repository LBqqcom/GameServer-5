#include "CMoster.h"

CCMoster::CCMoster(void)
{
}

CCMoster::~CCMoster(void)
{
}

//��ȡ������Ϣ
MonsterBaseInfo CCMoster::GetMosterInfo(CT_INT32 iMosterID)
{
	MonsterBaseInfo Moster={0};
	//memset(&Moster,0,sizeof(Moster));

	return Moster;
}
//��ù�����Ϣ
CT_BOOL	CCMoster::GetMosterData(MonsterBaseInfo *pMosterInfo)
{
	return true;
}
