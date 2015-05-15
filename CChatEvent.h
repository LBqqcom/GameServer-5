#ifndef CChatEvent_H_
#define CChatEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CChatEvent : public CCGameMesEvent
{
public:
	CChatEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CChatEvent(void);
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	CT_VOID DisposeChatEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	CT_BOOL ForwardingPrivateChat(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID ForwardingWorldChat(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID ForwardingFightTeamChat(CT_CHAR* pBuf,CT_DWORD len);
};

#endif