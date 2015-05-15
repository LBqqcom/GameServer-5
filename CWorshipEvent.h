#ifndef CWorshipEvent_H_
#define CWorshipEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCWorshipEvent: public CCGameMesEvent
{
public:
	CCWorshipEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCWorshipEvent();
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID){}
private:
	CT_VOID	DisposeWorship(CT_CHAR* pBuf,CT_DWORD len);
	CT_VOID	DisposeWorshipReward(CT_CHAR* pBuf,CT_DWORD len);
	CT_INT32 GetWorshipType(CT_INT32 iType);
};

#endif