/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightTeam.h 
Version:		1.0
Description:    ս������
Author:			zhoujun
Create Date:	2013-05-03
History:        ս������     

******************************************************************/
#ifndef CCFIGHTTEAM_H_
#define CCFIGHTTEAM_H_
#include <map>
#include "CUser.h"
#include "CGDefine.h"
#include "CMutex.h"
#include "CThreadMes.h"
#include "CResourceGrabMgr.h"
#include <time.h>

//����
struct CSInspore
{
	CT_DWORD	dwUserID;
	CT_INT32	iGrade;
	CT_INT32	iPowerID;
	CT_INT32	iExp;
};

struct FightTeamBase	//���Ҫ���Ӵ���4�ֽ����͵��ֶ�,�뽫����� pragma pack(push, 4) �Ƶ�֮��֮��,���ⲻҪ����DB��
{
	CT_DWORD	dwFTID;							// ս��ID
	CT_CHAR		cFightTeamName[NAME_SIZE];		// ս������
	CT_CHAR		cDescription[DESCRIPTION_SIZE]; // ս�ӽ���
	CT_DWORD	dwBossUID;						// ����ID
	CT_CHAR		cBossName[NAME_SIZE];			// ��������
	CT_INT32	iFightingPower;					// ��ս����
	CT_INT32	iTop;							// ����
	CT_INT32	iPreTop;						// �ϴ�����(������̬)
	CT_INT32	iResource;						// ��Դ��
	CT_INT32	iIcon;							// 
	CT_DWORD	dwElite[CLAN_CRUSADE_MEMBER];	// ��ս��Ӣ
	CT_INT32	iResCount;						//��Դ����

	CT_INT32	iGrade;							//�ȼ�
	CT_INT32	iItemCount;						//������Դ��
	CT_INT32	iSummonCount;					//�ٻ�����
	CT_INT32	iBossMaxIndex;					//���BOSS����
	CT_INT32	iTimeBossMaxIndex;				//ʱ��boss�������
};

#  pragma pack(push, 4)  
struct FightTeamMember
{
	CT_DWORD	dwFTID;							// ս��ID
	CT_DWORD	dwUserID;						// ��ԱID
	CT_CHAR		cNickName[NAME_SIZE];			// ��Ա����
	CT_INT32	iFightingPower;					// ��Աս����
	CT_INT32	iOccupation;					// ְҵ
	CT_INT32	iGrade;							// �ȼ�
	CT_INT32	iRank;							// ����
	CT_INT32	iContribution;					// ����
	CT_INT32	iStatus;						// ״̬ 0.����,������ 1.פ��,������ 2.����,���� 3פ��,����
	CT_INT64	iTime;							// ����ʱ��/����ʱ��/����ʱ��
	CT_INT32	iTowerCPID;						// Ԫ�������ͨ�ز���
	CT_INT32	iContributeCount[CONTRIBUTE_TYPE];//���״���
	CT_INT32	iContributeGross;					//��������
	CT_INT64	iLastExitTime;
	CT_INT32	iTitleID;						//ͷ��ID
};
#  pragma pack(pop)  

//ս��BOSS
struct FightTeamSummonBoss
{
	CT_DWORD			dwFTID;							//ս��ID
	CT_DWORD			dwUserID[SUMMON_MAX];			//�û�id
	CT_INT32			iBossIndex;						//BOSS����
	CT_DWORD			dwChestIndex[SUMMON_MAX*2];		//��������
	CT_INT32			iWiner;							//�ٻ�ս��ʤ��
	CT_BOOL				bFightingStatus;				//ս��״̬
	CMD_G_NormalReward	Res[SUMMON_MAX];				//��Դ
};

//���߽�����Ϣ
typedef CMD_G_FTTBRewardInfo FightTeamSummonItem ;
//struct FightTeamSummonItem
//{
//	CT_DWORD			dwUserID;						//�û�id
//	CT_INT32			iChestIndex;					//��������
//	CT_INT32			iItemID;						//����ID
//	CT_INT32			iCount;							//��������
//};

//ʱ��BOSS��Ϣ
struct FTTimeBossInfo
{
	CT_INT32			iIndex;							//����
	CT_INT32			iSummonBeginTime;					//�ٻ���ʼʱ��
	CT_INT32			iHelpUser[HELP_SUMMON_COUNT];			//�����û�
	CT_INT32			iCurrentHP;						//��ǰhp
};

//ʱ��BOSS��Ա��Ϣ
struct FTTimeBossMember
{
	CT_DWORD			dwUserID;
	CT_INT32			iHurt;								//�˺�
	CT_INT32			iFTFood;							//ս����ʳ
	CT_INT32			iFTContribute;						//ս�ӹ���
	CT_INT32			iFightNum;							//ս������
};

typedef std::map<CT_DWORD, FightTeamMember>		FTMember;				//��Ա()
typedef std::map<CT_DWORD, UserFightingStatus>	FTMemberFightingData;	//��Աս��������
typedef std::multimap<CT_INT64, CMD_G_FightTeamEventLog>	FTEventData;		//ս�Ӷ�̬
typedef std::map<CT_INT32, FightTeamFightData>	FTFightData;			//ս������
typedef std::multimap<CT_DWORD,FightTeamSummonItem>	FTSummonRewardMap;		//����key:userid
typedef std::map<CT_INT32,CSInspore>		CSInsporeMap;			//������Ϣ
typedef std::map<CT_DWORD,CSInsporeMap>		CSUserInsporeMap;		//����������Ϣ
typedef std::map<CT_DWORD,FTTimeBossMember> TBMemberMap;			//ʱ��boss��Ա��Ϣ



//ս������
class CCFightTeam 
{	
public:
	enum FIGHT_MEMBER_TYPE
	{
		FIGHTTEAM_UNKNOW		=	(0),		//δ֪
		FIGHTTEAM_BOSS			=	(1),		//�ӳ�
		FIGHTTEAM_ELITE			=	(2),		//��Ӣ
		FIGHTTEAM_MEMBER		=	(3),		//��ͨ��Ա
	};

	enum FIGHT_CONTRIBUTE_TYPE
	{
		FIGHTTEAM_FOOD			=	(0),		//ʳ�����
		FIGHTTEAM_DIAMOD		=	(1),		//��ʯ����
		FIGHTTEAM_ITEM			=	(2),		//���߾���
	};

	friend class CCFightTeamManager;
	friend class CCFightTeamEvent;
private:
	CCMutex							m_mutex;
	CT_INT64						m_iLastActTime;
	CT_INT64						m_iLastUpdTime;
	CT_INT32						m_iResCount;

	FightTeamBase					m_stBase;				// ս������Ϣ
	FTMember						m_FTMemberMap;			// ս�ӳ�Ա��Ϣ
	//FTMemberFightingData			m_FTMemberFightingData;	// ��Աս������Ϣ
	FTMember						m_FTMemberReqMap;		// ����: ��������
	FTMember						m_FTMemberRspMap;		// ����: �������
	FTSummonRewardMap				m_FTSummonRewardMap;	// �ٻ�����
	FightTeamSummonBoss				m_FTSummonInfo;			//�ٻ���Ϣ
	CSUserInsporeMap				m_FTCSInspore;			//��������
	FTTimeBossInfo					m_FTTimeBossInfo;		//ʱ��boss��Ϣ
	TBMemberMap						m_TBMemberMap;			//ʱ��boss��Ա��Ϣ
	FTSummonRewardMap				m_TBRewardMap;			//ʱ��boss����

	FTEventData						m_FTEventMap;			// ս�Ӷ�̬
	FTFightData						m_FTFightMap;			// ս��ս��

public:
	//��ʼ��
	CT_VOID		Init();
	CT_VOID		Clear();

public:
	//����ս��id 
	CT_VOID		SetFightTeamID(CT_DWORD id) {_AUTOMUTEX(&m_mutex);   m_stBase.dwFTID = id;}

	CT_DWORD	GetFightTeamID() {_AUTOMUTEX(&m_mutex);  return m_stBase.dwFTID; }
	CT_VOID		GetFightTeamName(CT_CHAR* pName) {_AUTOMUTEX(&m_mutex); memcpy(pName, m_stBase.cFightTeamName, NAME_SIZE);}
	CT_VOID		SetFTName(CT_CHAR *pName){_AUTOMUTEX(&m_mutex); memcpy(m_stBase.cFightTeamName, pName, NAME_SIZE-1);}

	CT_DWORD	GetFightTeamBossID() {_AUTOMUTEX(&m_mutex);  return m_stBase.dwBossUID; }
	CT_VOID		GetFightTeamBossName(CT_CHAR* pName) {_AUTOMUTEX(&m_mutex);  memcpy(pName, m_stBase.cBossName, NAME_SIZE); }
	CT_VOID		SetFightTeamBossName(CT_CHAR* pName) {_AUTOMUTEX(&m_mutex);  memcpy(m_stBase.cBossName,pName,  NAME_SIZE); }

	CT_VOID		SetFightTeam(const CMD_G_FightTeamBase &data)					// ����ս������Ϣ
	{
		_AUTOMUTEX(&m_mutex); memcpy(&m_stBase, &data, sizeof(FightTeamBase));
	}

	CT_VOID		InitInsertMember(CT_INT32 iType, const CMD_G_FightTeamMember &data);	// ��ʼ������ս�ӳ�Ա
	CT_INT32	InsertMember(CT_INT32 iType, const CMD_G_FightTeamMember &data);		// ����ս�ӳ�Ա
	CT_VOID		InsertMemberFightingData(MSC_DB_FTMemberFightingData &dbFTMemberFightingData);	//�����Աս����
	CT_VOID		InsertSummonInfo(FightTeamSummonBoss &FTSB);
	CT_VOID		InsertSummonReward(FightTeamSummonItem &FTSM);
	CT_VOID		InsertInspore(CSInspore &csi);											//��������
	CT_VOID		InsertTimeBoss(FTTimeBossInfo &tbi);									//����ʱ��boss
	CT_VOID		InsertTimeBossMember(FTTimeBossMember &bm);								//����ʱ��boss��Ա��Ϣ
	CT_VOID		InsertTimeBossRewardr(FightTeamSummonItem &si,CT_BOOL bSave=CT_TRUE);							//����ʱ��boss����

	CT_INT32	DeleteMember(CT_INT32 iType, CT_DWORD dwMUID);								// ɾ��ս�ӳ�Ա

	CT_BOOL		GetFightTeamBase(CMD_G_FightTeamBase *pFightTeamBase);
	CT_BOOL		GetFightTeamInfo(CMD_G_FightTeamInfo *pFightTeamInfo);

	CT_BOOL		GetFightTeamMember(CT_CHAR *pBuf, CT_DWORD &bufLen);
	CT_BOOL		GetFightTeamReqMember(CT_CHAR *pBuf, CT_DWORD &bufLen);

	CT_BOOL		GetFightTeamMember(CT_INT32 iType, CT_DWORD dwUser, CMD_G_FightTeamMember* pFightTeamMember);
	//�������(Ŀǰֻ��һ����������һ����)
	CT_BOOL		GetInspore(CT_DWORD dwUserID,CSInspore &csi);
	//��������
	CT_VOID		UpGradeInspore(CT_DWORD dwUserID,CSInspore &csi);

	CT_BOOL		CheckFightTeamFull();	// ���ս���Ƿ��Ѿ���Ա

	CT_VOID		ClearReqRsp(CT_DWORD dwUser);		// ��ĳ�û��Ѽ���ĳս�Ӻ�, �������������ս�ӵ����������

	CT_INT32	GetResource() {_AUTOMUTEX(&m_mutex);  return m_stBase.iResource; }

	CT_VOID		AddResource(CT_INT32 iResource) {_AUTOMUTEX(&m_mutex);  m_stBase.iResource += iResource; }

	CT_INT32	Contribute(const CMD_G_FightTeam_Donate &Donate);	// ��Ա����
	CT_VOID		Contribute(const CT_DWORD dwUserID,const CT_INT32 iDonate);	// ��Ա����

	CT_VOID		UpdateData(CT_INT64 lTime);	//��ʱ��������

	CT_INT32	GetEliteIndex(CT_DWORD dwUser);		// ����Ǿ�Ӣ,�򷵻ؾ�Ӣ���, ���� -1

	CT_VOID		GetMemberName(CT_DWORD dwMemberID, CT_CHAR* pName);

	CT_VOID		InitEventLog(const CMD_G_FightTeamEventLog &fightTeamEventLog);	//��ʼ������ս�Ӷ�̬
	CT_VOID		AddEventLog(const CMD_G_FightTeamEventLog &fightTeamEventLog);	//����ս�Ӷ�̬
	CT_VOID		GetEventLog(CT_CHAR *pBuf, CT_INT32 &bufLen);					//��ȡս�Ӷ�̬
	CT_INT32	ADDFightData(const FightTeamFightData &ftFightData);			//����ս��,�����¼�ID

	CT_VOID		Broadcast(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_DWORD dwExcep = 0);	//Ⱥ�㲥
	CT_VOID		FightTeamChat(CMD_G_FightTeamChat *pFightTeamChat, CT_DWORD len);	//ս������

	CT_VOID		GetFightTeamMemberUID(FightTeamMemberUID *ftMemberUID);		// ��ȡս�ӳ�ԱUID

	CT_BOOL		GetUserFightData(UserFightingStatus *userFightingData, CT_INT32 iCount,CT_BOOL bFight=CT_FALSE);	// ��ȡ��Ӣս������

	CT_BOOL		GetUserFightData(CT_DWORD dwUserID, UserFightingStatus &rightFightingData,CT_BOOL bFight=CT_FALSE);	// ��ȡָ����Աս������

	CT_VOID		UserExitEvent(CCUser *pUser);		// �û�����,����ս�ӳ�Ա����
	CT_VOID		UserLoginEvent(CT_DWORD dwUser);	// �û�����, ���ͳ�Ա����״̬

	CT_VOID		ResReward(CT_INT32 iResource);		// ����Դ����

	FIGHT_MEMBER_TYPE	GetMemberPosition(CT_DWORD dwUserID);	// ��ȡ��Աְλ 1.�ӳ� 2.��Ӣ 3.��ͨ��Ա 0.δ�ҵ���Ա

	CT_BOOL		CheckResCountGrade(CT_INT32 iResCount);		// ���ռ���N����Դ������Ҫ�Ķӳ��ȼ��Ƿ�����

	CT_VOID		SetUsersGuard(const GuardTag &gdPrimary,const GuardTag &gd); // פ����Ա���
	CT_VOID		SendMemberGuard(const CT_DWORD dwUserID);

	CT_VOID		MemberRef(const CT_DWORD dwUser, const CT_INT32 iGrade, const CT_INT32 iFightingPower);

	CT_VOID		SetResCount(const CT_INT32 iResCount) {_AUTOMUTEX(&m_mutex);  m_stBase.iResCount = iResCount; }

	CT_VOID		FightTeamUpGrade();//ս������
	CT_VOID		UpdataFightTeamInfo(CT_DWORD dwUserID=0);	//����ս����Ϣ
	CT_VOID		CostContribute(CT_DWORD dwUserID,CT_INT32 iContribute);//��Ա��������
	CT_VOID		SetSummonInfo(FightTeamSummonBoss &FTSB);		//�����ٻ���Ϣ
	CT_INT32	GetSummonCount();				//��ȡ�ٻ�����
	CT_BOOL		GetSummonInfo(FightTeamSummonBoss &FTSB);		//��ȡ�ٻ���Ϣ
	CT_INT32	HelpSummonLeave(CT_DWORD dwUserID,FightTeamSummonBoss &FTSB);//�����뿪
	CT_VOID     SendSummonMessage(CT_DWORD dwUser[SUMMON_MAX],CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);//���ٻ���Ա������Ϣ
	CT_VOID		SetSummonReward(CT_INT32 iUserIndex,CT_INT32 iChestIndex,CMD_G_NormalReward &Reward,CT_INT32 iItem[GOODS_COUNT],CT_INT32 iItemCount[GOODS_COUNT]);//�����ٻ�����
	CT_BOOL		GetSummonRewardItem(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &len);	//��ȡս���ٻ�����
	CT_BOOL		GetSummonRewardRes(CT_DWORD dwUserID,CT_CHAR *pBuf,CT_INT32 &len);	//��ȡս���ٻ�����
	CT_VOID		SetSummonWin(CT_INT32 iBossIndex);				//�����ٻ�ʤ��
	CT_VOID     ClearSummonData();								//�����ٻ�����
	CT_VOID		SetSummonFightingStatus(CT_BOOL bFighting){m_FTSummonInfo.bFightingStatus=bFighting;};
	CT_BOOL		GetSummonFightingStatus(){return m_FTSummonInfo.bFightingStatus;};
	CT_VOID		GetTimeBossInfo(FTTimeBossInfo &tbi){_AUTOMUTEX(&m_mutex);memcpy(&tbi,&m_FTTimeBossInfo,sizeof(m_FTTimeBossInfo));};
	CT_VOID		SetTimeBossInfo(FTTimeBossInfo &tbi);
	CT_VOID		UpdataTimeBossMember(FTTimeBossMember &bm);								//����ʱ��boss��Ա��Ϣ
	CT_BOOL		GetTimeBossMember(CT_DWORD dwUserID,FTTimeBossMember &bm);				//��ȡʱ��boss��Ա��Ϣ
	CT_VOID		DeleteTBMember(CT_DWORD dwUserID);										//ɾ��ʱ��boss��Ա��Ϣ
	CT_VOID		ClearTBMemberFightCount();						//���ʱ��BOSSս������
	CT_BOOL		GetTBReward(CT_INT32 iIndex,FightTeamSummonItem &si);	//ʱ��boss����
	CT_BOOL		GetTBReward(CT_CHAR *pBuf,CT_INT32 &iLen);	//ʱ��boss����
	CT_INT32	GetTBCurrentHP();							//ʱ��BOSS��ǰѪ��
	CT_INT32    GetTBMaxHP();								//ʱ��BOSS���Ѫ��
	CT_VOID		SetTBIndex(CT_INT32 iIndex);				//����ʱ��boss�������
	CT_VOID		ClearTBInfo();								//����ʱ��boss��Ϣ

	CT_VOID		SetMemberTowerCP(CT_DWORD dwUID, CT_INT32 iMaxCPID);	//����ͨ������
	CT_INT32	GetTowerPassCount(CT_INT32 iCPID);			//��ȡս������ͨ������

public:
	CCFightTeam();
	~CCFightTeam();

	CCFightTeam(const CCFightTeam &fightTeam) 
	{
		m_iLastActTime = fightTeam.m_iLastActTime;
		m_iLastUpdTime = fightTeam.m_iLastUpdTime;

		m_stBase = fightTeam.m_stBase;
		m_FTMemberMap = fightTeam.m_FTMemberMap;
		m_FTMemberReqMap = fightTeam.m_FTMemberReqMap;
		m_FTMemberRspMap = fightTeam.m_FTMemberRspMap;
		m_TBMemberMap=fightTeam.m_TBMemberMap;
		m_TBRewardMap=fightTeam.m_TBRewardMap;
		memcpy(&m_FTSummonInfo, &fightTeam.m_FTSummonInfo, sizeof(m_FTSummonInfo));
		memcpy(&m_FTTimeBossInfo,&fightTeam.m_FTTimeBossInfo,sizeof(m_FTTimeBossInfo));
	}
	CCFightTeam operator = (const CCFightTeam& fightTeam) 
	{
		m_iLastActTime = fightTeam.m_iLastActTime;
		m_iLastUpdTime = fightTeam.m_iLastUpdTime;

		m_stBase = fightTeam.m_stBase;	
		m_FTMemberMap = fightTeam.m_FTMemberMap;
		m_FTMemberReqMap = fightTeam.m_FTMemberReqMap;
		m_FTMemberRspMap = fightTeam.m_FTMemberRspMap;
		m_TBMemberMap=fightTeam.m_TBMemberMap;
		m_TBRewardMap=fightTeam.m_TBRewardMap;
		memcpy(&m_FTSummonInfo, &fightTeam.m_FTSummonInfo, sizeof(m_FTSummonInfo));
		memcpy(&m_FTTimeBossInfo,&fightTeam.m_FTTimeBossInfo,sizeof(m_FTTimeBossInfo));
	}

private:
	CT_VOID ComputeFightingPower();		//����ս����
	CT_VOID ReCheckMember();
}; 

#endif
