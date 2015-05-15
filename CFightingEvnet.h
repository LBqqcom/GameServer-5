#ifndef CCFIGHTINGEVNET_H_
#define CCFIGHTINGEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"
#include "CFightingLogic.h"

//ս��վλ
/************************************************************************/
/*   �û���			���﷽
	 3		0		5		8
		2				7
	 4		1		6		9											
	 
*/
/************************************************************************/

#define				ROUNDD_MAX					15				//���غ�
class CCUser; 

struct MSC_Fighting_End;
struct MSC_DB_W_PVPFightingInfo;
struct MSC_Fighting_Info;
struct MSC_PVP_Fighting_Info;
struct MSC_TeamPVP_ResRobbery;
struct MSC_TeamPVE_ResRobbery;

struct FightingUserInfo;
struct MasterCombatStatus;

struct G_MonsterGroup;
struct G_FightingScene;

enum ENUM_LoadResRobbery
{
	LOAD_FAIL			=	0,			//��Դ�Ӷ�ʧ��
	LOAD_SUC			=	1,			//��Դ�Ӷ�ɹ�
	LOAD_OVER			=	2,			//��Դ�Ӷ����
};


struct FightingTeamPVP
{
	CT_INT32							iLen[FIGHTING_NUM];				//ս������
	CT_INT32							iWiner[FIGHTING_NUM];			//ʤ����
	CMD_G_MasterInfo					MasterInfo[TEAM_NUM];		//������Ϣ
	CMD_G_MasterInfo					RivalInfo[TEAM_NUM];		//������Ϣ
	CCFightingLogic::FightingSaveList	FSList[FIGHTING_NUM];			//ս��
};

//(��Ҫ�ڲ�����,����������)
struct FightingTeamPVE
{
	CT_INT32							iLen[FIGHTING_NUM];				//ս������
	CT_INT32							iWiner[FIGHTING_NUM];			//ʤ����
	CMD_G_MasterInfo					MasterInfo[TEAM_NUM];		//������Ϣ
	CMD_G_MosterInfo					MosterInfo[TEAM_NUM];		//������Ϣ
	CCFightingLogic::FightingSaveList	FSList[FIGHTING_NUM];			//ս��
};

class CCFightingEvnet1:public CCGameMesEvent
{
private:
	CCFightingLogic		*m_pFightingLogic;			//ս���߼�
	CT_CHAR				*m_pFightingBuffer;			//ս������

public:
	CCFightingEvnet1(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCFightingEvnet1(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

	//��Ϣ����
private:
	//ս����Ϣ
	CT_BOOL DisposeFightingPVE(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//PVP
	CT_BOOL DisposeFightingPVP(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�ۿ�ս��
	CT_BOOL	DisposeLookFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�¼�ս��
	CT_BOOL	DisposeEventFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//PVP��ս
	CT_BOOL DisposePVPLookFighting(CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//��Դ�Ӷ�
	CT_BOOL DisposeResRobberyPVP(CT_CHAR* pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//��Դ�Ӷ�
	CT_BOOL DisposeResRobberyPVE(CT_CHAR* pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//��Ӹ���
	CT_BOOL DiposeTeamDuplicate(CT_CHAR* pBuf,CT_DWORD len);
	//Ԫ��֮��
	CT_BOOL DisposeTowerCheckPoint(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//
	CT_BOOL DisposeTowerPVPResRobbery(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//ս��BOSS
	CT_BOOL DiposeFTSummonBoss(CT_CHAR *pBuf,CT_DWORD len);
	//����BOSS
	CT_BOOL DiposeWorldBoss(CT_CHAR *pBuf,CT_DWORD len);
	//���ս��
	CT_BOOL DisposeCSFighting(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//8ǿս��
	CT_BOOL DisposeCSFinalEight(CT_CHAR *pBuf,CT_DWORD len);
	//����ս��
	CT_BOOL DisposeIslandFight(CT_CHAR *pBuf,CT_DWORD len);
	//�Ӷ�ս��
	CT_BOOL DisopsePlunderFight(CT_CHAR *pBuf,CT_DWORD len);
	//��Ʊս��
	CT_BOOL DisopseKidnapFight(CT_CHAR *pBuf,CT_DWORD len);
	//Ӫ��ս��
	CT_BOOL DisposeRescueFight(CT_CHAR *pBuf,CT_DWORD len,CT_DWORD dwSubID);
	//ʱ��bossս��
	CT_BOOL DisposeFTTimeBoss(CT_CHAR *pBuf,CT_DWORD len);

	//���ú���
private:
	//���ú���
	CT_VOID ResetVariable();

	//���غ���
private:
	//����ս��
	CT_BOOL	LoadFighting(const MSC_Fighting_Info &FightingInfo,const G_FightingScene &FightingScene,CCNetInterface* pNetHandle);
	//�����¼�ս��
	CT_BOOL	LoadEventFihting(const MSC_Fighting_Info &FightingInfo,CCNetInterface* pNetHandle);
	//����PVPս��
	CT_BOOL LoadPVPFighting(const MSC_PVP_Fighting_Info &FightingInfo,CCNetInterface* pNetHandle,CT_DWORD dwMeUserID);
	//�������ս��
	CT_BYTE LoadTeamFighting(const UserFightingStatus *FightingInfo,CT_BOOL bWin,CT_DWORD dwMeUserID,CT_INT32 iIndex);

private:
	//���ع���
	CT_BOOL LoadMoster(CCNetInterface* pNetHandle,CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iMasterGrade);
	//�����¼�����
	CT_BOOL LoadEventMoster(CCNetInterface* pNetHandle,CT_INT32 iEvent,CT_INT32 iMasterGrade);
	//������ӹ���
	CT_BYTE LoadTeamMoster(const CT_INT32 iTeamMoster[TEAM_NUM],CT_INT32 iIndex,CT_BOOL bWin,const CT_INT32 iSceneID,CT_INT32 iMasterGrade);

	//ս������
private:
	//��ʼս��
	CT_BOOL BeginFighting();
	//ս������
	CT_BOOL EndFighting(CCNetInterface* pNetHandle,CT_DWORD dwSubID);

	//ս������
private:
	//ս������
	CT_VOID SaveFighting(const MSC_Fighting_Info &FightingPVE);
	//PVPս������
	CT_VOID SavePVPFighting(const MSC_PVP_Fighting_Info &FightingPVP);

private:
#if 0 //��ֲ�����߳�
	//������Ϣ
	CT_VOID FightingReward(CCUser *pUser,CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iStarCount);
	//PVP������Ϣ
	CT_BOOL PVPFightingReward(CCUser *pUser,CT_INT32 iWiner,const MSC_PVP_Fighting_Info &FightingInfo,MSC_DB_W_PVPFightingInfo &PVPFighting);
	//��Ӹ�������
	CT_VOID TeamDuplicateReward(CCUser *pUser,const CT_INT32 iSceneID,const CT_INT32 iDifficulty);
#endif

private:
	//����Ǽ�
	CT_INT32 GetStar(CT_INT32 iWorldSceneID,CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iScore);
	//��ȡ�����Ϣ
	CT_BOOL GetFightingTeam(const UserFightingStatus *FightingInfo,CMD_G_MasterInfo *FightingTeam,CT_INT32 iNum=TEAM_NUM);
	//��ȡ��ӹ�����Ϣ
	CT_BOOL GetTeamMoster(const CT_INT32 iTeamMoster[TEAM_NUM],CMD_G_MosterInfo *FightingTeam,const CT_INT32 iSceneID);

private:
	//����ս��
	CT_VOID SendFighting(FightingTeamPVP &TeamPVP,CCNetInterface* pNetHandle,const CT_INT32 iSubID);
	//����ս��
	CT_VOID SendFighting(FightingTeamPVE &TeamPVE,CCNetInterface* pNetHandle,const CT_INT32 iSubID);

private:
	CT_VOID Decompress(CT_INT32 *pBuffer,CT_INT32 iLen,CT_INT32 *pFightinginfo);
	//////////////////////////////////////////////////////////////////////////
};
#endif
