#ifndef CTITLE_H_
#define CTITLE_H_

#include "CTType.h"
#include "CSingleton.h"
#include <vector>

class CCTitle: public CCSingleton<CCTitle>
{
private:
	typedef std::vector<CT_INT32> TitleVector;

	TitleVector m_TitleVector;

public:
	CCTitle() {}
	~CCTitle() {}

public:
	CT_VOID DeleteTitleList() { m_TitleVector.clear(); }
	CT_VOID InsertTitle(CT_INT32 iTitleID) { m_TitleVector.push_back(iTitleID); }
	CT_INT32 GetTitleList(CT_CHAR* pBuf, CT_INT32 iLen);
};

#endif