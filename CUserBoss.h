#ifndef CUSERBOSS_H_
#define CUSERBOSS_H_

#include <set>
#include "CMutex.h"

class CCUserBoss
{
private:
	CCMutex*    m_pMutex;
public:
	typedef std::set<CT_INT32>						AEList;

public:
	CCUserBoss();
	~CCUserBoss();

	AEList											m_AEList;
public:
	CT_VOID Init(CCMutex* pMutex);
	//清理用户数据
	CT_VOID Clear();

	CT_BOOL InsertRecord(const CT_INT32 &iAEID);

	CT_BOOL GetRecord(CT_CHAR *pBuf, CT_INT32 &bufLen);

};
#endif

