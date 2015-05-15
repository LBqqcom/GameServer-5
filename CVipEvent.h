#ifndef CVipEvent_H_
#define CVipEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCVipEvent : public CCGameMesEvent
{
public:
	CCVipEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCVipEvent();
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	CT_VOID DisposeVipRenewals(CT_CHAR *pBuf,CT_DWORD len);
	CT_VOID DisposeVipHarvestOnce(CT_CHAR *pBuf,CT_DWORD len);
	CT_VOID DisposeVipGift(CT_CHAR *pBuf,CT_DWORD len);
	CT_VOID DisposeVipActive(CT_CHAR *pBuf,CT_DWORD len);
};

#endif