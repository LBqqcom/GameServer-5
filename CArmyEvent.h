#ifndef CCARMYEVNET_H_
#define CCARMYEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCArmyEvent:public CCGameMesEvent
{
public:
	CCArmyEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCArmyEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	//������Ϣ
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//����������Ϣ
	CT_VOID DisposeArmyGrade(CT_CHAR* pBuf,CT_DWORD len);
	//���ӽ�����Ϣ
	CT_VOID DisposeArmyAdvanced(CT_CHAR* pBuf,CT_DWORD len);

private:
	//��������
	CT_BOOL	ArmyGradeValidate(CCUser *pUser,CT_INT32 iArmyID);
	//���ӽ���
	CT_BOOL	ArmyAdvancedValidate(CCUser *pUser,CT_INT32 iArmyID);
};
#endif