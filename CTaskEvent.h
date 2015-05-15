#ifndef CTaskEvent_H_
#define CTaskEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCTaskEvent : public CCGameMesEvent
{
public:
	CCTaskEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCTaskEvent(void);
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//����������
	CT_VOID DisposeRewardRequest(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeTaskFastComplete(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeTaskInquiry(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DispostTaskRole(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID DisposeFightTeamUpgrade(CT_CHAR* pBuf, CT_DWORD len);
};

#endif