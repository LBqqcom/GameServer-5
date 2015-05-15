/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCTowerEvent.h
Version:		1.0
Description:    Ԫ��֮���¼�����
Author:			pengwen
Create Date:	2013-8-14
History:        �����û��¼�     

******************************************************************/

#ifndef CCTOWEREVNET_H_
#define CCTOWEREVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCTowerEvent:public CCGameMesEvent
{
public:
	CCTowerEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCTowerEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID);

private:
	//����Ԫ��֮����������Ϣ
	CT_VOID DisposeElementTower(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//��������Ϣ����
	CT_VOID DisposeElementTowerInfoApply(CT_CHAR* pBuf,CT_DWORD len);
	//����Ԫ��֮����������Ϣ
	CT_VOID DisposeElementTowerOccupy(CT_CHAR* pBuf,CT_DWORD len);
	//�����û���������
	CT_VOID DisposeTowerOfflineData(CT_CHAR* pBuf,CT_DWORD len);
	//����ս���̷߳�������Ϣ
	CT_VOID DisposTowerPVPFromFight(CT_CHAR* pBuf,CT_DWORD len);
	//��������Э��
	CT_VOID DisposeTowerFriendHelpApply(CT_CHAR* pBuf,CT_DWORD len);
	//�鿴ս��
	CT_VOID DisposeTowerLookFighting(CT_CHAR* pBuf,CT_DWORD len);
	//�û��˳�
	CT_VOID DisposeTowerUserExit(CT_CHAR* pBuf,CT_DWORD len);

	//�û�����
	CT_VOID DisposeElementTowerGiveupApply(CT_CHAR* pBuf,CT_DWORD len);

	//�ؿ�ս������
	CT_VOID DisposeTowerPassApply(CT_CHAR* pBuf,CT_DWORD len);
	//ɨ��
	CT_VOID DisposeTowerRaids(CT_CHAR* pBuf,CT_DWORD len);
	//ʹ�ñ���
	CT_VOID DisposeTowerUseTreasure(CT_CHAR* pBuf,CT_DWORD len);
	//ս�����
	CT_VOID DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//�鿴ϵͳ����
	CT_VOID DisposeTowerMSGRead(CT_CHAR* pBuf,CT_DWORD len);
	//�鿴����ս������
	CT_VOID DisposeTowerLookFight(CT_CHAR* pBuf,CT_DWORD len);
	//
	CT_VOID DisposeTowerSlotAwardGet(CT_CHAR* pBuf,CT_DWORD len);
	//
	CT_VOID DisposeTowerTreasureAbandon(CT_CHAR* pBuf,CT_DWORD len);


private:
	//ռ��ɹ�
	CT_VOID DisposeOccupySuc(const ElemTowerUserInfo& userInfo, CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlotID,const CMD_G_NormalReward&  costRes, CT_INT32 iOccupyTime);
	//ռ��ʧ��
	CT_VOID DisposeOccupyLose(CT_DWORD dwUserID, CT_INT32 iTower, CT_INT32 iFloor, CT_INT32 iSlotID,const CMD_G_NormalReward&  costRes);

};


#endif

