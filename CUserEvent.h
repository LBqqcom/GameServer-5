/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserEvent.h
Version:		1.0
Description:    �û��¼�����
Author:			pengwen
Create Date:	2012-9-29
History:        �û��¼�����     

******************************************************************/
#ifndef CCUSEREVENT_H_
#define CCUSEREVENT_H_
#include <map>
#include <set>
#include "CTType.h"
#include "CGDefine.h"
class CCUser;
class CCMutex;
struct RandEvent;
class CCUserEvent
{
private:
	//�¼��нṹ���������¼�ִ���У�
	struct LastingEvent
	{
		CT_INT32    iIndex;      //�¼�����
		CT_LONGLONG llTimeBegin; //��ʼʱ��
		CT_INT32    iLasting;    //�¼�����ʱ��
	};
private:
	CCUser*    m_pUser;
	CCMutex*   m_pMutex;
	typedef std::map<CT_INT32, LastingEvent>             UEEventMap;        //�¼��н�������
	UEEventMap                m_buildingEventMap;        //�����¼�
	UEEventMap                m_workerEventMap;          //�����¼�
	UEEventMap                m_treasureEventMap;        //�����¼�
	UEEventMap                m_petEventMap;             //�����¼�
	UEEventMap                m_fightMap;                //ս���¼�
public:
	CCUserEvent(void);
	~CCUserEvent(void);
public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser  = pUser;
		m_pMutex = pMutex;
		m_buildingEventMap.clear();
		m_workerEventMap.clear();
		m_treasureEventMap.clear();
		m_petEventMap.clear();
		m_fightMap.clear();
	}
	//�����û�����
	CT_VOID Clear();
public:
	//���ҽ����¼�
	CT_BOOL  FindBuildingEventByIndex(CT_INT32 iIndex);
    //�����¼�
	CT_BOOL  FindPetEvent(CT_INT32 iIndex);
	//�����¼�
	CT_BOOL  FindFightEvent(CT_INT32 iIndex);
	//�����¼�
	CT_BOOL  FindWorkerEvent(CT_INT32 iIndex);
	//�����¼�
	CT_BOOL  FindTreasureEvent(CT_INT32 iIndex);
	//���ݽ���id�����¼�
	CT_BOOL	 FindBuildingEventByID(CT_INT32 iBuildID);

public:
	//�����¼�(��ʱ�������µ��¼�)
	CT_VOID UpdateEvent();
	

public:
	//�õ��¼�
	CT_BOOL GetEvents(CT_CHAR* pData, CT_INT32& dataLen);
	//�����¼�
	CT_VOID AssignEvent(const RandEvent& re);
private:   
	//�����¼�
	CT_VOID UpdateEvent(UEEventMap& eventMap, ENUM_EventType et);
	//���䱦���¼�
	CT_VOID AssignTreasureEvent(const RandEvent& re);
	//���乤���¼�
	CT_VOID AssignWorkerEvent(const RandEvent& re);
	//��������¼�
	CT_VOID AssignPetEvent(const RandEvent& re);
	//���佨���¼�
	CT_VOID AssginBuildingEvent(const RandEvent& re);
	//����ս���¼�
	CT_VOID AssginFightingEvent(const RandEvent& re);
public:
	//ɾ�������¼�
	CT_VOID DeleteBuildingEvent(CT_INT32 iIndex);
	//ɾ���¼�
	CT_VOID DeletePetEvent(CT_INT32   iIndex);
	//ɾ���¼�
	CT_VOID DeleteWorkerEvent(CT_INT32 iIndex);
	//ɾ���¼�
	CT_VOID DeleteTreasureEvent(CT_INT32 iIndex);
	//ɾ���¼�
	CT_VOID DeleteFightEvent(CT_INT32 iIndex);

};

#endif