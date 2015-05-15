#ifndef CCFIGHTTEAMEVNET_H_
#define CCFIGHTTEAMEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCFightTeam;
class CCFightTeamEvent:public CCGameMesEvent
{
public:
	CCFightTeamEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCFightTeamEvent(void);

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
	CT_VOID DisposeFightTeamCreate(CT_CHAR* pBuf,CT_DWORD len);			// ����ս��

	CT_VOID DisposeFightTeamRequest(CT_CHAR* pBuf,CT_DWORD len);		// ����ս������

	CT_VOID DisposeFightTeamAgree(CT_CHAR* pBuf, CT_DWORD len);			// ͬ������

	CT_VOID DisposeFightTeamDisagree(CT_CHAR* pBuf, CT_DWORD len);		// ��ͬ������

	CT_VOID DisposeFightTeamInvite(CT_CHAR* pBuf, CT_DWORD len);		// �����Ա

	CT_VOID DisposeFightTeamInviteFind(CT_CHAR* pBuf, CT_DWORD len);

	CT_VOID DisposeFightTeamAccept(CT_CHAR* pBuf, CT_DWORD len);		// ͬ������

	CT_VOID DisposeFightTeamReject(CT_CHAR* pBuf, CT_DWORD len);		// ��ͬ������

	CT_VOID DisposeFightTeamContribute(CT_CHAR* pBuf, CT_DWORD len);	// ����

	CT_INT32 DisposeFightTeamContributeValidate(CCUser *pUser,CMD_G_FightTeam_Donate *pFtd,CCFightTeam *pFightTeam);//������֤

	CT_VOID DisposeFightTeamTransfer(CT_CHAR* pBuf, CT_DWORD len);		// ת�öӳ�

	CT_VOID DisposeFightTeamQuit(CT_CHAR* pBuf, CT_DWORD len);			// �˳�ս��

	CT_VOID DisposeFightTeamElite(CT_CHAR* pBuf, CT_DWORD len);			// ����ս�Ӿ�Ӣ

	CT_VOID DisposeFightTeamDesc(CT_CHAR* pBuf, CT_DWORD len);			// ����ս�ӹ���

	CT_VOID DisposeFightTeamSearch(CT_CHAR* pBuf, CT_DWORD len);		// ��ѯս��

	CT_VOID DisposeFightTeamRecruit(CT_CHAR* pBuf, CT_DWORD len);		// ������ļ

	CT_VOID DisposeFightTeamRecruitSure(CT_CHAR* pBuf, CT_DWORD len);	// ȷ�Ϸ�����ļ

	CT_VOID DisposeFightTeamLookRecruit(CT_CHAR* pBuf, CT_DWORD len);	// ������ļ�б�

	CT_VOID DisposeFightTeamDisband(CT_CHAR* pBuf, CT_DWORD len);		// ��ɢս��

	CT_VOID DisposeFightTeamLookEvent(CT_CHAR* pBuf, CT_DWORD len);		// ����ս�Ӷ�̬�б�

	CT_VOID DisposeFightTeamLookFight(CT_CHAR* pBuf, CT_DWORD len);		// �鿴ս��

	CT_VOID DisposeFightTeamMemberFire(CT_CHAR* pBuf, CT_DWORD len);	// �߳���Ա

	CT_VOID DisposeFightTeamList(CT_CHAR* pBuf, CT_DWORD len);			//����ս���б�

	CT_VOID DisposeFightTeamReqList(CT_CHAR* pBuf, CT_DWORD len);		//���������б�

	CT_VOID DisposeFightTeamInviteList(CT_CHAR* pBuf, CT_DWORD len);	//���������б�

	CT_VOID DisposeFightTeamPlayList(CT_CHAR* pBuf, CT_DWORD len);		//��������б�

	CT_VOID DisposeFightTeamMember(CT_CHAR* pBuf, CT_DWORD len);		//�����Ա�б�

	CT_VOID DisposeFightTeamFightingData(CT_CHAR* pBuf, CT_DWORD len);	//�����Աս��������

	CT_VOID DiposeFightTeamMemeRefresh(CT_CHAR* pBuf, CT_DWORD len);    //������ӣ�ˢ�¶�Ա����

	CT_VOID DiposeFightTeamUpGrade(CT_CHAR *pBuf,CT_DWORD len);		//ս������

	CT_INT32 FightTeamUpGradeValidate(CCUser *pUser,CMD_G_FightTeamBase &ftb);	//ս���������

	CT_VOID DiposeFightTeamNemberScienceUp(CT_CHAR *pBuf,CT_DWORD len);		//��Ա�Ƽ�����

	CT_INT32 FightTeamNemberScienceValidate(CCUser *pUser,CT_INT32 iScienceID,CCFightTeam *pFightTeam);//�Ƽ��������

	CT_VOID DisposeFightTeamSummonBoss(CT_CHAR *pBuf,CT_DWORD len);			//ս���ٻ�

	CT_INT32 SummonBossValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iBossIndex,CT_INT32 iBossType);		//�ٻ���֤
	
	CT_VOID DisposeFightTeamHelpSummon(CT_CHAR *pBuf,CT_DWORD len);			//�ٻ�����

	CT_INT32 HelpSummonValidate(CCUser *pUser,CCFightTeam *pFightTeam);		//������֤

	CT_VOID DisposeFightTeamSummonReward(CT_CHAR *pBuf,CT_DWORD len);		//�ٻ�����

	CT_INT32 SummonRewardValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iUserIndex,CT_INT32 iChestIndex,CT_DWORD dwFTID);//�콱��֤

	CT_VOID DiposeFightTeamHelpSummonLeave(CT_CHAR *pBuf,CT_DWORD len);		//�����뿪

	CT_VOID DisposeFightTeamSummonFighting(CT_CHAR *pBuf,CT_DWORD len);		//�ٻ�ս��

	CT_VOID DisposeFightTeamSummonFightingEnd(CT_CHAR *pBuf,CT_DWORD len);		//�ٻ�ս�����
	
	CT_VOID DisposeFightTeamInvit(CT_CHAR *pBuf,CT_DWORD len);				//�ٻ�����

	CT_VOID SummonFightingAdd(UserFightingStatus *FightingNember,const CT_INT32 iCount,const CT_INT32 iType);	//�ٻ�ս���ӳ�

	CT_VOID DiposeFightTeamTowerCP(CT_CHAR *pBuf,CT_DWORD len);

	CT_VOID DisposeFightTeamUpgreadTask(CT_CHAR *pBuf, CT_DWORD len);		//����ս����������
	//�޸�ս������
	CT_VOID DisposeChangeFTName(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOOS�ٻ�
	CT_VOID DisposeFTTBSummon(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOSS����
	CT_VOID DisposeFTTBAdd(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOOS����
	CT_VOID DisposeFTTBInvite(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOOS��ʼ
	CT_VOID DisposeFTTBBegin(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOOSս��
	CT_VOID DisposeFTTBFight(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOOSս������
	CT_VOID DisposeFTTBFightReturn(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOOS�뿪
	CT_VOID DisposeFTTBLeave(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��BOOS����
	CT_VOID DisposeFTTBReward(CT_CHAR *pBuf, CT_DWORD len);
	//ʱ��boss������֤
	CT_INT32 FTTBAddValidate(CCUser *pUser,CCFightTeam *pFightTeam);
	//ʱ��bossս����֤
	CT_INT32 FTTBFightValidate(CCUser *pUser,CCFightTeam *pFightTeam);
	//�콱��֤
	CT_INT32 TBRewardValidate(CCUser *pUser,CCFightTeam *pFightTeam,CT_INT32 iChestIndex);
	//ɾ��ʱ��boss��Ϣ
	CT_VOID DeleteTimeBossInfo(CT_DWORD dwUserID,CCFightTeam *pFightTeam);

};
#endif