/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFightTeam.h
Version:		1.0
Description:    ս�����ݿ�
Author:			zhoujun
Create Date:	2013-05-05
History:        ����DBս���¼�   

******************************************************************/
#ifndef CCDBFIGHTTEAM_H_
#define CCDBFIGHTTEAM_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBFightTeam:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBFightTeam(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBFightTeam(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	CT_VOID DisposeFightTeamUpdate(CT_CHAR* pBuf,CT_DWORD len);	//����ս��

	CT_VOID DisposeFightTeamDelete(CT_CHAR* pBuf,CT_DWORD len);	//��ɢս��

	CT_VOID DisposeFightTeamFind(CT_CHAR* pBuf,CT_DWORD len);	//����ս��

	CT_VOID DisposeFightTeamMemberUpdate(CT_CHAR* pBuf,CT_DWORD len);	//����ս�ӳ�Ա

	CT_VOID DisposeFightTeamMemberDelete(CT_CHAR* pBuf,CT_DWORD len);	//ɾ��ս�ӳ�Ա

	CT_VOID DisposeFightTeamMemberFind(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeFightTeamRecruitUpdate(CT_CHAR* pBuf,CT_DWORD len);	//��ļ

	CT_VOID DisposeFightTeamRecruitDelete(CT_CHAR* pBuf,CT_DWORD len);	//ɾ����ļ

	CT_VOID DisposeFightTeamEventSave(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeFightTeamScienceUpGrade(CT_CHAR* pBuf,CT_DWORD len); //�Ƽ�����

	CT_VOID DisposeFightTeamSummonInfo(CT_CHAR *pBuf,CT_DWORD len);		//�ٻ���Ϣ

	CT_VOID DisposeFightTeamSummonItem(CT_CHAR *pBuf,CT_DWORD len);		//�ٻ�����

	CT_VOID DisposeFightTeamDownGrade(CT_CHAR *pBuf,CT_DWORD len);		//ս�ӿƼ��ͷ�
	//��������
	CT_VOID DisposeCSInsporeUp(CT_CHAR *pBuf,CT_INT32 iLen);
	//��ȡ����
	CT_VOID DisopseCSReadInspore(CT_CHAR *pBuf,CT_INT32 iLen);
	//����ʱ��boss
	CT_VOID DisposeClearTimeBoss(CT_CHAR *pBuf,CT_INT32 iLen);
	//ʱ��boss��Ϣ
	CT_VOID DisposeTimeBossInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//ʱ��boss��Ա
	CT_VOID DisposeTimeBossMumber(CT_CHAR *pBuf,CT_INT32 iLen);

	CT_VOID GetFightTeamDB();		// ��������20s������ݶ����ʼ������
};
#endif