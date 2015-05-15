#ifndef CDBPetEvent_H_
#define CDBPetEvent_H_

#include "CGameMesEvent.h"

class CCGameMesManager;
class CCDBConnector;

class CCDBPetEvent: public CCGameMesEvent
{
private:
	CCDBConnector &m_dbConnector;
public:
	CCDBPetEvent(CCGameMesManager *pMesManager,CT_DWORD id,CCDBConnector& dbConnector);
	~CCDBPetEvent(){}
private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf,		CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//宠物出生
	CT_VOID DisposePetBorn(CT_CHAR *pBuf,CT_INT32 iLen);			
	//宠物幻化
	CT_VOID DisposePetChange(CT_CHAR *pBuf,CT_INT32 iLen);				
};

#endif
