/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDuplicateEvent.h
Version:		1.0
Description:    �����¼���
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCDUPLICATEEVNET_H_
#define CCDUPLICATEEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCDuplicateEvent1:public CCGameMesEvent
{
public:
	CCDuplicateEvent1(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCDuplicateEvent1(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID){};
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//������Ϣ
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//�߳���Ϣ
	CT_VOID DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//���볡��
	CT_BOOL DisposeEnterScene(CT_CHAR* pBuf,CT_DWORD len);
	//��ȡ����
	CT_BOOL DisposeGetReward(CT_CHAR* pBuf,CT_DWORD len);
	//��ս
	CT_VOID	DisposeLookFighting(CT_CHAR* pBuf,CT_DWORD len);
	//����ɨ��
	CT_VOID	DisposeDuplicateRaids(CT_CHAR* pBuf,CT_DWORD len);
	//ɨ������
	CT_VOID	DisposeRaidsSpeed(CT_CHAR* pBuf,CT_DWORD len);
	//������
	CT_VOID	DisposeRequestReward(CT_CHAR* pBuf,CT_DWORD len);
	//ɨ��ʧ��
	CT_VOID	DisposeRaidsFail(CT_CHAR* pBuf,CT_DWORD len);
	//ͬ��������ʯ
	CT_VOID DisposeRaidsSpeedDiamod(CT_CHAR* pBuf,CT_DWORD len);


private:
	//ս������
	CT_BOOL DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//ɨ������
	CT_VOID DisposeRaidsReward(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeFightingReward(CT_CHAR* pBuf,CT_DWORD len);
public:
	//���볡����֤
	CT_INT32 EnterSceneValidate(SceneInfo &Sceneinfo,CCUser *pUser,CT_INT32 iDiamod);
	//ɨ����֤
	CT_INT32 RaidsValidate(RaidsInfo &Raidsinfo,CCUser *pUser,CT_INT32 iTime=1);
	//ɨ����¼ 
	CT_VOID RaidsSave(CMD_G_RaidsReward &Reward,RaidsInfo &Raidsinfo);



};

#endif
