/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightTeamManager.h
Version:		1.0
Description:    ս�ӹ���
Author:			zhoujun
Create Date:	2013-5-3
History:            

******************************************************************/
#ifndef CCFIGHTTEAM_MANAGER_H_
#define CCFIGHTTEAM_MANAGER_H_
#include <map>
#include <string>
#include "CNetInterface.h"
#include "CSingleton.h"
#include "CFightTeam.h"
#include "CMD_Game.h"
#include "CMutex.h"

#include "CThreadMes.h"

//��ļ
struct STRecruit
{
	CT_DWORD dwFTID;
	CT_INT64 iBeginTime;
	CT_INT32 iLastingTime;
	CT_CHAR  cRecruit[DESCRIPTION_SIZE];
	CT_INT32 iAuto;							// ����ս���Զ���ļ
};
//ս�ӹ�����
class CCFightTeamManager : public CCSingleton<CCFightTeamManager>
{

public:
	typedef std::map<CT_DWORD, CCFightTeam>					FightTeamMap;
	typedef std::map<CT_DWORD, CCFightTeam>::iterator			FTM_IT;

	typedef std::map<std::string, CT_DWORD>					FightTeamNameMap;
	typedef std::map<std::string, CT_DWORD>::iterator		FTNM_IT;

	typedef std::map<CT_DWORD, STRecruit>	FightTeamRecruitMap;
	typedef std::map<CT_DWORD, CT_DWORD>	FightTeamUserMap;

private:
	//ս����Ϣ��
	FightTeamMap					m_fightTeamMap;
	//ս�����Ʊ�
	FightTeamNameMap				m_fightTeamNameMap;
	//�û� -> ս��
	FightTeamUserMap				m_fightTeamUserMap;
	//��ļ��Ϣ
	FightTeamRecruitMap				m_fightTeamRecruitMap;
	//ս������
	std::map<CT_INT32, CT_DWORD>	m_ftRankingMap;
	//������
	CCMutex							m_mutex;  

public:
	CCFightTeamManager(CT_VOID);
	~CCFightTeamManager(CT_VOID);

public:
	
	//��������
	CT_BOOL Find(CT_DWORD id);
	//��������
	CT_BOOL Find(CT_DWORD id, CCFightTeam** ft);
	//ͨ���ǳƲ���
	CT_BOOL Find(std::string cFightTeamName, CCFightTeam** ft);
	//�������
	CT_VOID Erase(CT_DWORD id);
	//���
	CT_VOID Clear();
	//��ȡս����Ŀ
	CT_INT32 GetFightTeamCount(){return m_fightTeamMap.size();}

	CT_DWORD NewFightTeam(CCUser *pUser, CT_CHAR* pFightTeamName, CT_CHAR* pDescription,G_FightTeamUpgrade &ftug);		// �½�ս��ID
	CT_VOID DeleteFightTeam(CT_DWORD dwFTID);													//	��ɢս��

	CT_VOID	InitFightTeam(const MSC_DB_FightTeamBase &dbFightTeamBase);						// ��ʼ����������ս��
	CT_VOID	InitFightTeamMember(const MSC_DB_FightTeamMember &dbFightTeamMember);				// ��ʼ����������ս�ӵĶ�Ա
	CT_VOID	InsertFightTeamRecruit(const MSC_DB_FightTeamRecruit &dbFightTeamRecruit);			// ������ļ��Ϣ(����)
	CT_VOID	InitEventLog(const MSC_DB_FightTeamEventLog &dbFightTeamEventLog);					// ��ʼ������ս�Ӷ�̬
	CT_VOID InitFightTeamSummon(const MSC_DB_FightTeamSummonInfo &dbFightTeamSummoninfo);		//�ٻ���Ϣ
	CT_VOID InitFihgtTeamSummonItem(const MSC_DB_FightTeamSummInfoItem &dbFightTeamSummonItem);	//�ٻ����߽���
	CT_VOID InsertCSInspore(MSC_DB_INSPORE &DBCsi);										//����
	CT_VOID InsertTimeBossInfo(MSC_DB_FTTimeBossInfo &bi);														//����ʱ��boss
	CT_VOID InsertTimeBossMember(MSC_DB_FTTimeBossMember &bm);														//����ʱ��boss��Ա

    //�κ��˲��øĶ����������ʵ�֣�������أ� ����
	CT_DWORD GetFightTeamIDByUser(CT_DWORD dwUser);

	CT_VOID ClearReqRsp(CT_DWORD dwUser);			// ��ĳ�û��Ѽ���ĳս�Ӻ�, �������������ս�ӵ����������

	CT_VOID InsertUserMap(CT_DWORD dwUser, CT_DWORD dwFTID) {_AUTOMUTEX(&m_mutex); m_fightTeamUserMap.insert(FightTeamUserMap::value_type(dwUser, dwFTID));}

	CT_VOID DeleteUserMap(CT_DWORD dwUser) {_AUTOMUTEX(&m_mutex); m_fightTeamUserMap.erase(dwUser);}

//	CT_BOOL GetFightTeamBase(CT_DWORD dwUser, CMD_G_FightTeamBase *pFightTeamBase, CT_INT32 iResCount);

	CT_VOID BrowseFightTeam(CT_CHAR *out_buf, CT_INT32 &bufLen);
	CT_VOID BrowseFightTeamRecruit(CT_CHAR *out_buf, CT_INT32 &bufLen);

	CT_VOID GetFightTeamInviteList(CT_DWORD dwUser, CT_CHAR *pBuf, CT_DWORD &bufLen);	// ��ȡ�����б�

	CT_VOID CheckFightTeamRecruit();
	CT_BOOL CheckFightTeamRecruit(CT_DWORD dwFTID);

	CT_VOID UserExitEvent(CCUser *pUser);		// �û�����,����ս�ӳ�Ա����
	CT_VOID UserLoginEvent(CT_DWORD dwUser);	// �û�����, ���ͳ�Ա����״̬

	CT_VOID RefreshFightTeamTop();		// ÿ��ˢ��ս������

	CT_VOID ClearFightTeamSummon();		//����ս���ٻ�

	CT_VOID GetFightTeamTop(CT_DWORD *dwFTList, CT_INT32 &iCount);

	CT_VOID SendFightTeamChat(CMD_G_FightTeamChat *pFightTeamChat, CT_DWORD len);	//ս������

	CT_VOID FightTeamMemberRef(const CT_DWORD dwUser, const CT_INT32 iGrade, const CT_INT32 iFightingPower);

	CT_VOID SendFightTeamMemberGuard(const CT_DWORD dwUserID);

	// ������������
	CT_VOID UpdateIslandData();
	//�޸�ս������
	CT_VOID ChangeFTName(CT_CHAR cOldName[NAME_SIZE],CT_CHAR cNewName[NAME_SIZE]);

public:
	//�������ݿ�
	CT_VOID DBUpdataData();

public:

};


#endif