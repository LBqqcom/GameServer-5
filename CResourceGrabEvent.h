/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCResourceGrabMgr.h
Version:		1.0
Description:    ��Դ����ϵͳ��Ϣ����
Author:			LinQ
Create Date:	2013-05-08
History:        ��Դ����ϵͳ����Ϣ����

//�û���Ҫ��ʾ����Ϣ
6 ս�ӷ�����
7 ������
8 ��
9  ,�Ӷ���
10 ���
11 ս�ӳɹ���������
12 ս��:
13 �ɹ�������������
14 �����Ӷ���

15 ս�ӹ���
16 ʧ��
17 ս�ӳɹ��ķ���
18 ��
19 �Ĺ���
20 ,���ű���Ŷ!
21 ս�ӳɹ���ռ��
22 ս��ռ��
23 ��ʱ�򱻹�����
24 ս�ӳ�Ա
25 פ����
26 ��
27 �ջ���
28 �Ѿ����ڣ���������ռ��

******************************************************************/
#ifndef _CRESOURCE_GRAB_EVENT_H_
#define _CRESOURCE_GRAB_EVENT_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include<map>

class CCFightTeam;
struct G_ResourceQuality;
struct G_ResourceArea;

class CCResourceGrabEvent : public CCGameMesEvent
{
	std::map<CT_INT32,CT_INT32>		m_ResActiveMap;
public:
	CCResourceGrabEvent(CCGameMesManager *pMesManager, CT_WORD id);
	~CCResourceGrabEvent(void);
private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);
private:
	//������Դ����
	CT_VOID DisposeResGrab(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//������Դ����
	CT_VOID DisposeResFight(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//�����û���������
	CT_VOID DisposeScenceApple(CT_CHAR* pBuf, CT_DWORD len);
	//����ռ������
	CT_VOID DisposeOccupyApply(CT_CHAR* pBuf, CT_DWORD len);
	//��Դ����
	CT_VOID DisposeSpotProductApply(CT_CHAR* pBuf, CT_DWORD len);
	//��Դ����
	CT_VOID DisposeSpotDependApply(CT_CHAR* pBuf, CT_DWORD len);
	//�û��˳�����
	CT_VOID DisposeSecneExit(CT_CHAR* pBuf, CT_DWORD len);
	//������Դ
	CT_VOID DisposeSpotGiveUpApply(CT_CHAR* pBuf, CT_DWORD len);
	//��Դ�ջ�
	CT_VOID DisposeSpotHarvest(CT_CHAR* pBuf, CT_DWORD len);
	//���pvpս��
	CT_VOID DisposeResFightPVP(CT_CHAR* pBuf, CT_DWORD len);
	//���pveս��
	CT_VOID DisposeResFightPVE(CT_CHAR* pBuf, CT_DWORD len);
	//��Դ������
	CT_VOID DisposeResActiveOpen(CT_CHAR* pBuf, CT_DWORD len);
	//��Դ���ȡ���
	CT_VOID DisposeResActiveReadFinish(CT_CHAR* pBuf, CT_DWORD len);
	//��Դ�����
	CT_VOID DisposeResInsert(CT_CHAR* pBuf, CT_DWORD len);

	//��¼��־
	CT_VOID DisposePvPWinEventLog(CCFightTeam* pChanllageTeam,  CCFightTeam* pFightTeam, 
								  G_ResourceQuality& resQuality,  
								  CT_INT32 iWinIndex, CT_INT32 iLoseIndex, CT_INT32 iResOccupy);

	//������Դռ���¼
	CT_VOID DisposeResOccupyRecord(CT_DWORD dwTeamID, CT_INT32 iScenceID, CT_INT32 iSpotID, CT_INT32 iFood, CT_INT32 iOccupyGold);

	//��¼��־
	CT_VOID DisposePvPLoseEventLog(CCFightTeam* pChanllageTeam,  CCFightTeam* pFightTeam,   G_ResourceQuality& resQuality,  
		CT_INT32 iLoseIndex, CT_INT32  iWinIndex);

	//��¼��־
	CT_VOID DisposePvEWinEventLog(CCFightTeam* pChanllageTeam, G_ResourceArea& ra, G_ResourceQuality& resQuality,  
		CT_INT32 iFightIndex);

	//��¼��־
	CT_VOID DisposePvELoseEventLog(CCFightTeam* pChanllageTeam, G_ResourceArea& ra, G_ResourceQuality& resQuality,  
		CT_INT32 iFightIndex);


private:
	//���͹�������ɹ�
	CT_VOID SendOccupySuc(CCFightTeam* pTeam, CT_DWORD dwUserID, CT_INT32 iScenceID, CT_INT32 iSpotID);
	//����ռ������(�޵���)
	CT_VOID SetOccupyData(CT_INT32 iScenceID, CT_INT32 iSpotID, CT_DWORD dwTeamID, CT_INT32 iProtectTime);

private: 
	//�����´λʱ��
	CT_INT64 NextActiveTime(CT_INT32 AcviteList[100]);

	//��������
	CT_VOID SaveDataToDB();

};

#endif