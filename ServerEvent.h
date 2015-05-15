#ifndef  SERVEREVNT_H_
#define  SERVEREVNT_H_

#include "CGameMesEvent.h"

class CServerEvent:public CCGameMesEvent
{
private:
	CT_INT32	m_iPlayerCount;				//��Ϸ����

public:
	CServerEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CServerEvent(void);

public:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		CT_CHAR* pBuf,   CT_DWORD len,
		CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

	//�¼�����
private:
	//����ע�����
	CT_BOOL	ServerRegisterFinish(CT_CHAR* pBuf,CT_DWORD len,CCNetInterface* pNetHandle);
};

#endif
