#ifndef CDBSEEKEVENT_H_
#define CDBSEEKEVENT_H_

#include "CGameMesEvent.h"

class CCGameMesManager;
#include "CThreadMes.h"

class CCDBConnector;

class CCDBSeekEvnet : public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBSeekEvnet(CCGameMesManager *pMesManager, CT_DWORD id, CCDBConnector& dbConnect);
	~CCDBSeekEvnet();

private:
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	virtual CT_VOID OnTime(CT_DWORD timeID);
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//��ǽ�ӳ���Ϣ
	CT_VOID DisposeWallAddition(CT_CHAR* pBuf, CT_INT32 iLen);
	//��ǽ����
	CT_VOID DisposeWallArmy(CT_CHAR* pBuf, CT_INT32 iLen);
	// �����ߴ�������
	CT_VOID DisposeSeekedTimes(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��̽��״̬
	CT_VOID DisposeSeekedPlayerInsert(CT_CHAR* pBuf, CT_INT32 iLen);
	// ̽�ý������
	CT_VOID DisposeSeekListUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// ���±�̽���ߵĻ�����Ϣ
	CT_VOID DisposeSeekedDataUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// ����̽������Ϣ
	CT_VOID DisposeSeekDateUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// ����ս��
	CT_VOID DisposeTeamFlag(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��������
	CT_VOID DisposeSeekPresent(CT_CHAR* pBuf, CT_INT32 iLen);
	// ɾ������
	CT_VOID DisposeSeekPresentDel(CT_CHAR* pBuf, CT_INT32 iLen);
	// ɾ��ս��
	CT_VOID DisposeTeamFlagDelete(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��������
	CT_VOID DisposeKidnapUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
	// ɾ������
	CT_VOID DisposeKidnapDelete(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��ӷÿͼ�¼
	CT_VOID DisposeSeekVistorInfo(CT_CHAR* pBuf, CT_INT32 iLen);
	//���̽��ս��
	CT_VOID DisposeSeekFightRecord(CT_CHAR* pBuf, CT_INT32 iLen);
	// ��¼�Ӷ�Ľ�Һ���ʳ
	CT_VOID DisposePlunderResUpdate(CT_CHAR* pBuf, CT_INT32 iLen);
};
#endif