#ifndef	CREGIONEVENT_H_
#define CREGIONEVENT_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCRegionEvent : public CCGameMesEvent
{
public:
	CCRegionEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCRegionEvent(void);
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
	CT_VOID	DisposeRegionUnlock(CT_CHAR* pBuf,CT_DWORD len);
};

#endif