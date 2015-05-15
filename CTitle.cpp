#include "CTitle.h"

template<> CCTitle* CCSingleton<CCTitle>::ms_Singleton=0;

CT_INT32 CCTitle::GetTitleList(CT_CHAR* pBuf, CT_INT32 iLen)
{
	CT_INT32 ibuf = 0;
	for (CT_INT32 i = 0;i < m_TitleVector.size();++i)
	{
		ibuf += sizeof(int);
		if (iLen < ibuf)
		{
			return 0;
		}

		memcpy(pBuf + i * sizeof(int), &m_TitleVector[i], sizeof(int));
	}
	return m_TitleVector.size();
}

