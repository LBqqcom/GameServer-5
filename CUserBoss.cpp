#include "CUserBoss.h"

CCUserBoss::CCUserBoss()
{
}

CCUserBoss::~CCUserBoss(void)
{
}

CT_VOID CCUserBoss::Init(CCMutex* pMutex)
{
	m_pMutex = pMutex;
	
	m_AEList.clear();
}

//清理用户数据
CT_VOID CCUserBoss::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_AEList.clear();
}


CT_BOOL CCUserBoss::InsertRecord(const CT_INT32 &iAEID)
{
	_AUTOMUTEX(m_pMutex);
	AEList::iterator it = m_AEList.find(iAEID);
	if (it == m_AEList.end())
	{
		m_AEList.insert(iAEID);
		return CT_TRUE;
	}
	return CT_FALSE;
}

CT_BOOL CCUserBoss::GetRecord(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
	_AUTOMUTEX(m_pMutex);

	CT_INT32 len = m_AEList.size() * sizeof(CT_INT32);
	if (len > bufLen || len == 0)
	{
		return CT_FALSE;
	}

	AEList::iterator it = m_AEList.begin();
	AEList::iterator it_end = m_AEList.end();
	CT_INT32 iAEID=0;
	for (int i = 0; it != it_end; ++it, ++i)
	{
		iAEID = *it;
		memcpy(pBuf + sizeof(CT_INT32) * i, &iAEID, sizeof(CT_INT32));
	}
	bufLen = len;

	return CT_TRUE;
}
