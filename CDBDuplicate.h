/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFightingThread.h
Version:		1.0
Description:    ���ݿ⸱������
Author:			wangchen
Create Date:	2012-10-25
History:        ����DB�����¼�   

******************************************************************/
#ifndef CCDBDUPLICATETHREAD_H_
#define CCDBDUPLICATETHREAD_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBDuplicate:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBDuplicate(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBDuplicate(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
private:
	//ս����Ϣ
	CT_VOID DisposeFightingInfo(CT_CHAR* pBuf,CT_DWORD len);
	//������Ϣ
	CT_VOID DisposeWorldScene(CT_CHAR* pBuf,CT_DWORD len);
	//������Ϣ
	CT_VOID DisposeSceneInfo(CT_CHAR* pBuf,CT_DWORD len);
	//����ս������
	CT_VOID DisposeBuyFightingTime(CT_CHAR* pBuf,CT_DWORD len);
	//ɨ����Ϣ
	CT_VOID DisposeRaidsInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//ɨ������
	CT_VOID DisposeRaidsReward(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ӹ�����������
	CT_VOID DisposeTeamDuplicateScene(CT_CHAR *pBuf,CT_INT32 iLen);
	//��Ӹ����Զ�ƥ��
	CT_VOID DisposeTeamDuplicateAutoMatch(CT_CHAR *pBuf,CT_INT32 iLen);


};
#endif