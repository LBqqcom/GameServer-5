/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTopEvent.h
Version:		1.0
Description:    ���а��¼���
Author:			wangchen
Create Date:	2013-5-23
History:             

******************************************************************/
#ifndef CCTOPEVNET_H_
#define CCTOPEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
class CCTopEvent:public CCGameMesEvent
{
private: 
	CT_CHAR				*m_pBuffer;			//���а񻺳�
public:
	CCTopEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCTopEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	//�������а�ĸ��� ��������
	CT_VOID DisposeUpdataTop();
	//�������ݷ����������������� ��������
	CT_VOID DisposeMasterInfo(CT_CHAR* pBuf, CT_DWORD len);
	//������Ϣ������  ��������
	CT_VOID DisposeMasterFinish();
	//�����û���Ϣ�� ��������
	CT_VOID DisposeUserMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//����������Ϣ
	CT_VOID DisposeTopInfo(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	//��ս����
	CT_VOID DisposeTopFighting(CT_CHAR* pBuf, CT_DWORD len);

private:
	//��������
	CT_VOID DisposeRanking(CT_CHAR *pBuf,CT_INT32 iLen);
	//�ȼ�����
	CT_VOID DisposeGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս��������
	CT_VOID DisposeFightingPower(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս������
	CT_VOID DisposeFightingTeam(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ʤ����
	CT_VOID DisposeContinuousWin(CT_CHAR *pBuf,CT_INT32 iLen);
	//�鿴���
	CT_VOID DisposeViewUser(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս������
	CT_VOID DisposeFightingFlagPower(CT_CHAR *pBuf,CT_INT32 iLen);
	//���а���ս
	CT_VOID DisposeChallenge(CT_CHAR *pBuf,CT_INT32 iLen);
	//�Ӷ�����
	CT_VOID DisposePlunderRes(CT_CHAR* pBuf, CT_INT32 iLen);

	//���а���
	CT_VOID DisposeTopReward(CT_CHAR* pBuf,CT_INT32 iLen);

	//���ͽ�����ȡ״̬
	CT_VOID SendTopRewardStatus(CCUser* pUser, CT_INT32 iType);

private:
	//��ս��֤
	CT_INT32 TopChallengeValidate(CCUser *pUser,CT_INT32 iDiamod,CT_INT32 iCoolTime,CMD_G_ReturnFail &rf);

};
#endif
