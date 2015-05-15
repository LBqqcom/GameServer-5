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
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//�������
	CT_VOID DisposePetBorn(CT_CHAR *pBuf,CT_INT32 iLen);			
	//����û�
	CT_VOID DisposePetChange(CT_CHAR *pBuf,CT_INT32 iLen);				
};

#endif
