/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBPVPEvent.h
Version:		1.0
Description:    ���ݿ⾺��������
Author:			wangchen
Create Date:	2012-10-25
History:        ����DB�������¼�   

******************************************************************/
#ifndef CCDBPVPEVENT_H_
#define CCDBPVPEVENT_H_

#include "CGameMesEvent.h"
class CCDBConnector;
class CCUser;

class CCDBPVPEvent:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
	CCUser			*m_pUser;
public:
	CCDBPVPEvent(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBPVPEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//��������Ϣ
	CT_VOID DisposePVPInfo(CT_CHAR* pBuf,CT_DWORD len);
	//ˢ�¶���
	CT_VOID DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len);
	//ս����¼
	CT_VOID DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//��ս״��
	CT_VOID DisposeChallengeInfo(CT_CHAR* pBuf,CT_DWORD len);
	//�������ս�
	CT_VOID DisposePVPEnding(CT_CHAR *pBuf,CT_DWORD len);
	//���������Ϣ
	CT_VOID DisposePVPChallgerSave(CT_CHAR* pBuf, CT_DWORD len);
};
#endif