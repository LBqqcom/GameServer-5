#ifndef CCBOSSEVNET_H_
#define CCBOSSEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCBossEvent:public CCGameMesEvent
{
	std::map<CT_INT32,CT_INT32>		m_ActiveMap;
public:
	CCBossEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCBossEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);
	//�����´λʱ��
	CT_INT64 NextActiveTime(CT_INT32 AcviteList[100],  CT_INT32 &iLastAEID);

	CT_VOID DisposeMAIN(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	CT_VOID DisposeMSC(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);

	CT_VOID DisposeInfoUpdate(CT_CHAR* pBuf, CT_DWORD len);		//��һ�δ�BOSS�ҳ��
	CT_VOID DisposeJoinActive(CT_CHAR* pBuf, CT_DWORD len);		//���뵱ǰBOSS�
	CT_VOID DisposeExitActive(CT_CHAR* pBuf, CT_DWORD len);		//�뿪�ҳ��
	CT_VOID DisposeEncourage(CT_CHAR* pBuf, CT_DWORD len);		//����
	CT_VOID DisposeBattle(CT_CHAR* pBuf, CT_DWORD len);			//ս��/����
	CT_VOID DisposeReward(CT_CHAR* pBuf, CT_DWORD len);			//��ȡ����
	CT_VOID DisposeCurrInfo(CT_CHAR* pBuf, CT_DWORD len);		//����Ѫ��������

	CT_VOID DisposeFighting(CT_CHAR* pBuf, CT_DWORD len);		//ս������

public:
	CT_VOID DisposeActiveOpen();

};
#endif