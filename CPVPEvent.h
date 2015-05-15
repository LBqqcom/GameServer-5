/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CPVPEvent.h
Version:		1.0
Description:    �������¼���
Author:			wangchen
Create Date:	2012-10-20
History:             

******************************************************************/
#ifndef CCPVPEVNET_H_
#define CCPVPEVNET_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
struct MSC_Fighting_End;
struct MSC_PVP_Fighting_Info;
struct MSC_DB_W_PVPFightingInfo;

class CCPVPEvent:public CCGameMesEvent
{
private:
	CT_CHAR* m_pBuffer;
public:
	CCPVPEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCPVPEvent(void);

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
	//������Ϣ
	CT_VOID DisposeNetMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�߳���Ϣ
	CT_VOID DisposeThreadMessage(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);

private:
	//ˢ�¶���
	CT_VOID DisposeRefresh(CT_CHAR* pBuf,CT_DWORD len);
	//�򿪱���
	CT_VOID DisposeChest(CT_CHAR* pBuf,CT_DWORD len);
	//������ս��
	CT_VOID DisposePVPFighting(CT_CHAR* pBuf,CT_DWORD len);
	//�鿴ս��
	CT_VOID DisposePVPLookFighting(CT_CHAR* pBuf,CT_DWORD len);

private:
	//�û���Ϣ
	CT_VOID DisposeMSCUserInfo(CT_CHAR* pBuf,CT_DWORD len);
	//ս������
	CT_VOID DisposeMSCFightingEnd(CT_CHAR* pBuf,CT_DWORD len);
	//������ս������
	CT_VOID PVPFightingReward(CCUser *pUser,const MSC_PVP_Fighting_Info &FightingInfo,MSC_DB_W_PVPFightingInfo &PVPFighting);
	//��ʤ��¼
	CT_VOID ContinuousWin(CCUser *pUser,const MSC_PVP_Fighting_Info &FightingInfo);

private:
	//��������֤
	CT_BOOL PVPFightingValidate(CCUser *pUser,CT_INT32 iDiamond);
	//�����������
	CT_VOID DisposeChallgerInsert(CT_CHAR* pBuf, CT_DWORD len, CT_BOOL bHign );
	//�������ݲ������
	CT_VOID DisposeInsertChallgerFinish(CT_CHAR* pBuf, CT_DWORD len);
};
#endif