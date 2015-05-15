/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCityThread.h
Version:		1.0
Description:    ���з���
Author:			pengwen
Create Date:	2012-8-16
History:        ��������¼��������¼�     

******************************************************************/

#ifndef CCITYTHREAD_H_
#define CCITYTHREAD_H_
#include <cassert>
#include "CTaskThread.h"
#include "CGameMesEvent.h"


class CCityThread :
	public CCTaskThread
{
private:
	//�û����콨���¼�������
	CCGameMesEvent* m_pCityUserBuildEvent;
	//���������¼�������
	CCGameMesEvent* m_pBuildFunEvent; 
	//�����¼�������
	CCGameMesEvent* m_pMasterEvent;
	//�����¼�������
	CCGameMesEvent* m_pCityEvent;
	//�����¼�������
	CCGameMesEvent* m_pDuplicateEvent;
	//�����¼�������
	CCGameMesEvent* m_pChatEvent;
    //�����¼�������
    CCGameMesEvent* m_pItemEvent;
	//�����¼�������
	CCGameMesEvent* m_pRegionEvent;
	//�����¼�������
	CCGameMesEvent* m_pArmyEvent;
	//���¼�������
	CCGameMesEvent* m_pBattleArrayEvent;
	//�����¼�������
	CCGameMesEvent* m_pTaskEvent;
	//�������¼�������
	CCGameMesEvent* m_pPVPEvent;
    //�ճ�/ǩ���¼�������
    CCGameMesEvent* m_pDailyEvent;
	//�̵��¼�������
	CCGameMesEvent* m_pMarketEvent;
	//�����¼�������
	CCGameMesEvent* m_pPetEvent;
	//�ʼ�������
	CCGameMesEvent* m_pEmailEvent;
	//vip������
	CCGameMesEvent* m_pVipEvent;
	//�����û���¼��Ϣ
	CCGameMesEvent* m_InsertUserData;
	//��¼�������
	CCGameMesEvent* m_pActiveEvent;
	//�����¼�������
	CCGameMesEvent *m_pWorshipEvent;
	//��Ϣ����������
	CCGameMesEvent *m_pFeedBackEvent;
	// ��Դ�Ӷ�ϵͳ
	CCGameMesEvent *m_pResourceGrabEvent;
	//�����¼�������
	CCGameMesEvent* m_pFriendEvent;
	//��Ӹ���
	CCGameMesEvent* m_pTeamDuplicate;
	//ս���¼�������
	CCGameMesEvent* m_pFightTeamEvent;
	//���а�
	CCGameMesEvent* m_pTopEvent;
	//Ԫ��֮��
	CCGameMesEvent* m_pElementTower;
	//����BOSS
	CCGameMesEvent* m_pBossEvent;
	//�û��˳�������
	CCGameMesEvent*  m_pUserExitEvent;
	//�û���¼�¼�������
	CCGameMesEvent*  m_pUserEnterEvent;
	//�����¼�����
	CCGameMesEvent*  m_pServerEvent;
	//�û����ݻ���
	CCGameMesEvent*  m_pUserData;
	//���ս��
	CCGameMesEvent* pCSFightEvent;
	//����
	CCGameMesEvent* m_pDragonIsland;
	//̽��
	CCGameMesEvent* m_pSeekEvent;
public:
	CCityThread(CT_VOID);
	~CCityThread(CT_VOID);


public:
	//��
	virtual CT_VOID OnOpen(); 
	//
	virtual CT_VOID OnTimer(CT_DWORD timeID);
};
#endif