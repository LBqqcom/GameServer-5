/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingRecord.h
Version:		1.0
Description:    ���а�
Author:			wangchen
Create Date:	2012-10-25
History:             

******************************************************************/

#ifndef CCTOP_H_
#define CCTOP_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CGDefine.h"
#include "CThreadMes.h"
#include<list>
#include<map>

struct User_Fighting;
class CCMutex;


//�ȼ�������Ϣ
struct TopGrade
{
	CT_DWORD	dwUserID;					//�û�ID
	CT_CHAR		cName[NAME_SIZE];			//�û�����
	CT_INT32	iGrade;						//�û��ȼ�
	CT_INT32	iOccupation;				//�û�ְҵ
	CT_CHAR		cFightingTeam[NAME_SIZE];	//ս��
	CT_INT32	iFightingPower;				//ս����
	CT_INT32	iTopGrade;					//�ȼ�����
};

//�������а�
struct TopRanking
{
	CT_DWORD	dwUserID;					//�û�ID
	CT_CHAR		cName[NAME_SIZE];			//�û�����
	CT_INT32	iGrade;						//�û��ȼ�
	CT_INT32	iOccupation;				//�û�ְҵ
	CT_INT32	iRankingGrade;				//���εȼ�	
	CT_INT32	iFightingPower;				//ս����
	CT_INT32	iWinTimes;					//ʤ������
	CT_INT32	iTopRanking;				//�������а�
};

//ս�����а�
struct TopFightingPower
{
	CT_DWORD	dwUserID;					//�û�ID
	CT_CHAR		cName[NAME_SIZE];			//�û�����
	CT_INT32	iGrade;						//�û��ȼ�
	CT_INT32	iOccupation;				//�û�ְҵ
	CT_CHAR		cFightingTeam[NAME_SIZE];	//ս��
	CT_INT32	iFightingPower;				//ս����
	CT_INT32	iTopFightingPower;			//ս�������а�
};

//ս�����а�
struct TopFightingTeam
{
	CT_DWORD	dwFightingTeamID;				//ս��ID
	CT_INT32	iTopFightingTeam;				//ս�����а�
	CT_CHAR		cFightingTeamName[NAME_SIZE];	//ս������
	CT_CHAR		cMasterName[NAME_SIZE];			//��������
	CT_INT32	iPlayerNum;						//��Ա��Ŀ
	CT_INT32	iFightingPower;				//ս����
};

//��ʤ���а�
struct TopContinuousWin
{
	CT_DWORD	dwUserID;					//�û�ID
	CT_CHAR		cName[NAME_SIZE];			//�û�����
	CT_INT32	iGrade;						//�û��ȼ�
	CT_INT32	iOccupation;				//�û�ְҵ
	CT_INT32	iContinuousWinNum;			//��ʤ����
	CT_INT32	iFightingPower;				//ս����
};

//ս�����а�
struct TopFightingFlag
{
	CT_DWORD	dwUserID;
	CT_CHAR		cName[NAME_SIZE];
	CT_INT32	iGrade;
	CT_INT32	iOccupation;
	CT_INT32	iFightingFlagPower;
	CT_CHAR		cFightingTeam[NAME_SIZE];
};

//�Ӷ���Դ���а�
struct TopPlunderRes 
{
	CT_DWORD	dwUserID;
	CT_CHAR		cName[NAME_SIZE];
	CT_INT32	iGrade;
	CT_INT32	iOccupation;
	CT_INT32	iPlunderNum;
	CT_CHAR		cFightingTeam[NAME_SIZE];
};

//����
struct MaterInfo
{
	CT_DWORD	dwUserID;					//�û�id
	CT_INT32	iGrade;						//�ȼ�
	CT_INT32	iExp;						//����
	CT_INT32	iFightingPower;				//ս����
	CT_INT32	iCurrContWin;				//��ǰ��ʤ
	CT_INT32	iRankGrade;					//���εȼ�
	CT_INT32	iRankExp;					//���ξ���
	CT_INT32	iTopGrade;					//�ȼ�����
	CT_INT32	iTopRank;					//���εȼ�
	CT_INT32	iTopFightingPower;			//ս��������
	CT_INT32	iTopContWin;				//��ʤ����
	CT_INT32	iNowTopGrade;				//��ǰ�ȼ�����
	CT_INT32	iNowTopRank;				//��ǰ��������
	CT_INT32	iNowTopFightingPower;		//��ǰս��������
	CT_INT32	iNowTopContWin;				//��ǰ��ʤ����
	CT_INT32	iFightingFlagPower;			//ս��ս����
	CT_INT32	iNowTopFightingFlagPower;	//��ǰս������
	CT_INT32	iTopFightingFlagPower;		//ս�����а�
	CT_INT32	iPlunderScore;				//�Ӷ����
	CT_INT32	iTopPlunder;				//�Ӷ�����
	CT_INT32	iNowTopPlunder;				//��ǰ�Ӷ�����
};

class CCTop : public CCSingleton<CCTop>
{
	typedef std::map<CT_DWORD,LookUserInfo>				UserDataMap;
	typedef std::map<CT_DWORD,LookUserInfo>::iterator		UD_IT;
	typedef std::list<TopGrade>								GradeList;
	typedef std::list<TopGrade>::iterator					GL_IT;
	typedef std::list<TopRanking>							RankingList;
	typedef std::list<TopRanking>::iterator					RL_IT;
	typedef std::list<TopFightingPower>						FightingPowerList;
	typedef std::list<TopFightingPower>::iterator			FP_IT;
	typedef std::list<TopFightingTeam>						FightingTeamList;
	typedef std::list<TopFightingTeam>::iterator			FT_IT;
	typedef std::list<TopContinuousWin>						ContinuousWinList;
	typedef std::list<TopContinuousWin>::iterator			CW_IT;
	typedef std::list<TopFightingFlag>						FightingFlagList;
	typedef std::list<TopFightingFlag>::iterator			FF_IT;
	typedef std::list<TopPlunderRes>						PlunderResList;
	typedef std::list<TopPlunderRes>::iterator				PR_IT;

public:
	typedef std::map<CT_DWORD,MaterInfo>					MaterInfoMap;
	typedef std::map<CT_DWORD,MaterInfo>::iterator			MI_IT;
	typedef std::multimap<CT_INT64,CT_DWORD>				SequenceMap;
	typedef std::multimap<CT_INT64,CT_DWORD>::iterator		SM_IT;

private:
	CCMutex			m_Mutex;
	CT_INT32		m_iIndex;								//
	CT_DWORD		m_dwUserID[TOP_COUNT];			//�û�id
	User_Fighting	m_UserInfo[TOP_COUNT];			//

	UserDataMap			m_UserDataMap;							//�û���Ϣ
	GradeList			m_GradeList;							//�ȼ����а�
	RankingList			m_RankingList;							//�������а�
	FightingPowerList	m_FightingPowerList;					//ս�������а�
	FightingTeamList	m_FightingTeamList;						//ս�����а�
	ContinuousWinList	m_ContinuousWinList;					//��ʤ���а�
	FightingFlagList	m_FightingFlagList;						//ս�����а�
	PlunderResList		m_PlunderResList;						//�Ӷ���Դ���а�

	MaterInfoMap		m_MasterInfoMap;						//������Ϣ

	CT_BOOL				m_bInit;								//���а�����ȡ����Ϊ�����ʼ����ɱ���


public:
	CCTop(void);
	~CCTop(void);

public:
	//����ս������
	CT_VOID InsertTopFightingTeam(CT_DWORD *pTeamID,CT_INT32 iCount);
	//�����û���Ϣ(��ȡ���а�������Ϣ)
	CT_VOID InsertTopUserInfo(LookUserInfo *LookData,CT_INT32 iCount,ENUM_TOP Type);


public: 
	//��ȡ�ȼ�������Ϣ
	CT_VOID GetTopGradeInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ����������Ϣ
	CT_VOID GetTopRankingInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡս��������Ϣ
	CT_VOID GetTopFightingPowerInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡս��������Ϣ
	CT_VOID GetTopFightingTeamInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ��ʤ������Ϣ
	CT_VOID GetTopContinuousWinInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡս��������Ϣ
	CT_VOID GetTopFightingFlagInfo(CT_CHAR *pBuf,CT_INT32 &iLen);
	//��ȡ�Ӷ���Դ������Ϣ
	CT_VOID GetTopPlunderResInfo(CT_CHAR* pBuf, CT_INT32& iLen);
	//��ȡ�û���Ϣ
	CT_VOID GetTopUserInfo(const CT_DWORD dwUserID,LookUserInfo &UserInfo);
	//ǰ50���ȼ�ƽ��
	CT_INT32 GetTopGradeAverage();

private:
	//ת���û���Ϣ
	CT_VOID ChangeUserInfo(const User_Fighting &FightingData,LookUserInfo &UserData);

public:
	//���ú���
	CT_VOID ResetVariable();
	//����masterinfo
	CT_VOID ClearMasterInfo();

public:
	//
	CT_INT64 Get64Int(CT_INT32 high,CT_INT32 low);
	//����������Ϣ
	CT_VOID InsterMasterInfo(MaterInfo &mi);
	//����
	CT_VOID SequenceTop();
	//
	MaterInfoMap &GetTopMasterInfo(){_AUTOMUTEX(&m_Mutex);return m_MasterInfoMap;};
	//�������а��û�
	CT_BOOL FindTopUser(CT_DWORD dwUserID);

public:
	CT_VOID SetInit(CT_BOOL bInit){m_bInit=bInit;};
	CT_BOOL GetInit(){return m_bInit;};
};
#endif