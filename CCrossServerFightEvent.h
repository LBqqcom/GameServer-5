/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicateEevnt.h
Version:		1.0
Description:    ���ս�¼���
Author:			wangchen
Create Date:	2014-2-17
History:             

******************************************************************/
#ifndef CCCROSSSERVERFIGHTEVENT_H_
#define CCCROSSSERVERFIGHTEVENT_H_
#include <map>
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCUser;
class CCFightTeam;
struct CSInspore;
struct UserFightingStatus;
class CCCrossServerFightEvent:public CCGameMesEvent
{
public:
	CCCrossServerFightEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCCrossServerFightEvent(void);

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
	//�߳���Ϣ
	CT_VOID OnMscUserEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//������Ϣ
	CT_VOID OnNetUserEvent(CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//���̻ع�
	CT_VOID DisposeScheduleReviewApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//�콱����	
	CT_VOID DisposeRewardApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//�鿴�	
	CT_VOID DisposeCSLookActive(CT_CHAR *pBuf,CT_INT32 iLen);
	//֧������	
	CT_VOID DisposeCSSupportApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//������������	
	CT_VOID DisposeCSPopTopApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//׼����Ϣ����	
	CT_VOID DisposeCSReadyInfoApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//�ʸ���ǰ׼������
	CT_VOID DisposeSeatBeforApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//��������
	CT_VOID DisposeCSInsporeApply(CT_CHAR *pBuf,CT_INT32 iLen);
	//ͬ����ǰ8ǿ
	CT_VOID DisposeCurrentFinalEight(CT_CHAR *pBuf,CT_INT32 iLen);
	//ͬ���Ͻ�8ǿ
	CT_VOID DisposeLastFinalEight(CT_CHAR *pBuf,CT_INT32 iLen);
	//����״̬
	CT_VOID DisposeReturnStatus(CT_CHAR *pBuf,CT_INT32 iLen);
	//�ʸ���ս��
	CT_VOID DisposeSeatFightTeam(CT_CHAR *pBuf,CT_INT32 iLen);
	//�ʸ�ս��
	CT_VOID DisposeSeatFighting(CT_CHAR *pBuf,CT_INT32 iLen,CT_DWORD dwSubID);
	//ս���������
	CT_VOID DisposeSeatEndingReturn(CT_CHAR *pBuf,CT_INT32 iLen);
	//ͬ���û�����
	CT_VOID DipsoseUpdateUserData(CT_CHAR *pBuf,CT_INT32 iLen);
	//�������ͬ���û�����
	CT_VOID DisposeCSUpdateUserData(CT_CHAR *pBuf,CT_INT32 iLen);
	//�鿴ս��
	CT_VOID DisposeCSLookFightRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս����ͬ��
	CT_VOID DisposeCSChangeFTPos(CT_CHAR *pBuf,CT_INT32 iLen);
	//ͬ��ս��
	CT_VOID DisposeCSSynFightRecord(CT_CHAR *pBuf,CT_INT32 iLen);
	//ͬ��ս���û�
	CT_VOID DisposeRecordUser(CT_CHAR *pBuf,CT_INT32 iLen);

private:
	//�����
	CT_VOID DisposeActiveBegin(CT_CHAR *pBuf,CT_INT32 iLen);
	//ս�����
	CT_VOID DisposeCSFightingEnd(CT_CHAR *pBuf,CT_INT32 iLen);
	//8ǿս�����
	CT_VOID DisposeCSFinelEightFightingEnd(CT_CHAR *pBuf,CT_INT32 iLen);

private:
	//֧����Դ��֤
	CT_INT32 SupportVaidate(CCUser *pUser,CMD_G_CSSupportApply &CSSa,G_CSFightBet &csb);
	//8ǿս��
	CT_VOID FinalEightFight(CT_INT32 iSeed);
	//8ǿս������ת��
	CT_VOID FinalEightFightChangeIndex(CT_INT32 iStatus,CT_INT32 iPos,CT_CHAR *pIndex);
	//������Դ��֤
	CT_INT32 InsporeValidate(CCUser *pUser,CCFightTeam *pFT,CT_INT32 iID,CMD_G_NormalReward &nr);
	//8ǿ����
	CT_VOID FinalEightReward();
	//ȫ������
	CT_VOID FullServerReward();
	//�±���
	CT_VOID NewMatch();
	//���ܼӳ�
	CT_VOID AddInsport(UserFightingStatus &msc,CSInspore &csi);
	//�ھ�����
	CT_VOID ChampionBroadcast();
	//ɾ��BOSS
	CT_VOID DeleteBoss(const CT_INT32 iPos);

};
#endif
