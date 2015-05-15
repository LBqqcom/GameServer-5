/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCDataBaseThread.h
Version:		1.0
Description:    ���ݿ����
Author:			pengwen
Create Date:	2012-8-16
History:        �������ݿ��¼�     

******************************************************************/

#ifndef CCDATABASETHREAD_H_
#define CCDATABASETHREAD_H_
#include "CTaskThread.h"
#include "CDBConnector.h"
#include "CGameMesEvent.h"

struct MSC_DB_UserEnter;

class CCDataBaseThread :
	public CCTaskThread
{
private:
	//�û���¼�¼�������
	CCGameMesEvent*  m_pDBUserEnterEvent;
	//�û������������ݿ⴦����
	CCGameMesEvent*  m_pDBCityUserBuildEvent;
	//�������ݿ⴦����
	CCGameMesEvent*  m_pDBCommonEvent; 
	//�����������ݿ⴦����CCDBBuildFunEvent
	CCGameMesEvent*  m_pDBBuildFunEvent;
    // �����¼�������
    CCGameMesEvent*  m_pDBMasterEvent;
	//�¼�������
	CCGameMesEvent* m_pDBBuildingEvent;
	//ս��������
	CCGameMesEvent *m_pDBDuplicateEvent;
	//�û������¼�����
	CCGameMesEvent *m_pDBRegionEvent;
	//gm��Ϣ�¼�����
	CCGameMesEvent *m_pDBChatEvent;
	//������Ϣ�¼�����
	CCGameMesEvent *m_pDBArmyEvent;
	//����Ϣ�¼�����
	CCGameMesEvent *m_pDBBattleArrayEvent;
	//������Ϣ����
	CCGameMesEvent *m_pDBTaskEvent;
	//��������Ϣ����
	CCGameMesEvent *m_pDBPVPEvent;
    // �ճ���Ϣ����
    CCGameMesEvent *m_pDBDailyEvent;
	//�̵���Ϣ����
	CCGameMesEvent *m_pDBMarketEvent;
	//������Ϣ
	CCGameMesEvent *m_pDBPetEvent;
	//�û��˳���Ϣ
	CCGameMesEvent *m_pDBUserExitEvent;
	//�ʼ���Ϣ
	CCGameMesEvent *m_pDBEmailEvent;
	//vip��Ϣ
	CCGameMesEvent *m_pDBVipEvent;
	//���Ϣ
	CCGameMesEvent *m_pDBActiveEvent;
	//������Ϣ
	CCGameMesEvent *m_pDBWorshipEvent;
	//������Ϣ
	CCGameMesEvent *m_pDBFriendEvent;
	//ս������
	CCGameMesEvent *m_pDBFightTeamEvent;
	//��Դ����
	CCGameMesEvent *m_pDBResourceGrap; 
	//���а�
	CCGameMesEvent *m_pDBTopEvent;
	//Ԫ��֮��
	CCGameMesEvent *m_pDBTowerEvent;
	//ȫ������
	CCGameMesEvent *m_pDBGlobeEvent;
	//BOSS��Ϣ
	CCGameMesEvent *m_pDBBossEvent;
	//���ս
	CCGameMesEvent *m_pDBCSFightEvent;
	//����
	CCGameMesEvent *m_pDBDragonIsland;
	//̽��
	CCGameMesEvent *m_pDBSeekEvent;

private:
	CCDBConnector m_dbConnector; 

public:
	CCDataBaseThread(CT_VOID);
	~CCDataBaseThread(CT_VOID);
public:
	//��
	virtual CT_VOID OnOpen(); 

	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//��ʱ��
	CT_VOID OnTimer(CT_DWORD timeID);
	CT_VOID OnClock(CT_DWORD timeID);

private:
	//����ս����¼
	CT_VOID DisposeFightingRecord();
	//����Ԫ����ս������
	CT_VOID DisposeTowerFightRecord();
	//�������а���Ϣ
	CT_VOID DisposeTopInfo();
	//CT_VOID DisposeGMMessage();
	//���빫����Ϣ
	CT_VOID DisposeAnnouncement();
	CT_VOID DisposeUpdateData();
};

#endif
