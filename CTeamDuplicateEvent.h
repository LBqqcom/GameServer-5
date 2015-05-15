/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicateEevnt.h
Version:		1.0
Description:    ��Ӹ����¼���
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCTEAMDUPLICATEEVNET_H_
#define CCTEAMDUPLICATEEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
class CCTeamDuplicateEvent:public CCGameMesEvent
{
public:
	CCTeamDuplicateEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCTeamDuplicateEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	//��������
	CT_VOID DisposeCreateRoom(CT_CHAR* pBuf,CT_DWORD len);
	//���뷿��
	CT_VOID DisposeAddRoom(CT_CHAR* pBuf,CT_DWORD len);
	//���ټ���
	CT_VOID DisposeFastAddRoom(CT_CHAR* pBuf,CT_DWORD len);
	//��λ����
	CT_VOID DisposePos(CT_CHAR* pBuf,CT_DWORD len);
	//���׼��
	CT_VOID DisposeReady(CT_CHAR* pBuf,CT_DWORD len);
	//��ҿ�ʼ
	CT_VOID DisposeBegin(CT_CHAR* pBuf,CT_DWORD len);
	//������Ϣ����
	CT_VOID DisposeChangeRoomInfo(CT_CHAR* pBuf,CT_DWORD len);
	//�������
	CT_VOID DisposeInvitePlayer(CT_CHAR* pBuf,CT_DWORD len);
	//�Զ�ƥ��
	CT_VOID DiposeAutoMatch(CT_CHAR* pBuf,CT_DWORD len);
	//����뿪
	CT_VOID DisposeLeave(CT_CHAR* pBuf,CT_DWORD len);
	//������Ӹ���
	CT_VOID DiposeEnter(CT_CHAR* pBuf,CT_DWORD len,CT_INT32 iSubID);
	//�ܾ�����
	CT_VOID DiposeRefuse(CT_CHAR* pBuf,CT_DWORD len);
	//ս������
	CT_VOID DisposeFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//�Զ�ƥ��
	CT_VOID DisposeAutoMatchInfo(CT_CHAR* pBuf,CT_DWORD len);
	//��������
	CT_VOID DisposeClear(CT_CHAR *pBuf,CT_DWORD len);

private:
	//���³���
	CT_VOID UpdataScene(CT_DWORD dwUserID,CT_INT32 iScene,CT_INT32 iDifficlty);
	//��Ӹ�������
	CT_VOID TeamDuplicateReward(CCUser *pUser,const CT_INT32 iSceneID,const CT_INT32 iDifficulty);
};

#endif
