/****************************************************
	created:	2014/07/10		14:52
	file base:	DragonIsland
	file ext:	h
	author:		zhouhuan

	purpose:
*****************************************************/

#ifndef CDRAGONISLAND_H
#define CDRAGONISLAND_H
 
#include "CMutex.h"
#include "CGDefine.h"
#include <vector>

class CCUser;

struct Point 
{
	CT_INT32	iPointID;		// ���ID
	CT_INT32	iType;			// �¼�����
	CT_INT32	iEventID;		// �¼�ID
	CT_INT32	iStatus;		// �¼�״̬
	CT_INT32	iStep;			// Ͷ������
	CT_INT32	iBeginChest;	// ��㱦��ID��û��Ϊ0
	CT_INT32	iEventChest;	// �¼�����
};

struct DragonPlayer
{
	CT_INT32	iIsOpen;		// �����Ƿ���
	CT_INT32	iTotalTimes;	// ӵ��Ͷ������
	CT_INT32	iThrowTimes;	// ��Ͷ������		
	CT_INT32	iBeginTimes;	// ������ʼ��Ĵ���
	CT_INT32	iGetTimes;		// ���մ���
	CT_INT32	iFightTeamID;	// ����ս��ID
	CT_INT32	iScore;			// ս�ӻ���
	CT_INT32	iRewardTimes;	// �������Ͷ������
};

class CCUserIsland
{
private:
	typedef std::vector<CT_INT32>	GiveUserIDVector;
	typedef std::vector<CT_INT32>	TeamChestIDVector;

	typedef std::vector<std::string>	GetPresentNameVector;

	CCUser*			m_pUser;
	CCMutex*		m_pMutex;
	Point			m_Point;
	DragonPlayer	m_DPlayer;
	GiveUserIDVector		m_GiveUserIDVector;
	TeamChestIDVector		m_TeamChestIDVector;
	GetPresentNameVector	m_GetPresentNameVector;

public:
	CCUserIsland();
	~CCUserIsland();

public:
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		memset(&m_Point, 0, sizeof(m_Point));
		memset(&m_DPlayer, 0, sizeof(m_DPlayer));
		m_GiveUserIDVector.clear();
		m_TeamChestIDVector.clear();
		m_GetPresentNameVector.clear();
	}
	CT_VOID Clear()
	{
		memset(&m_Point, 0, sizeof(m_Point));
		memset(&m_DPlayer, 0, sizeof(m_DPlayer));
		m_GiveUserIDVector.clear();
		m_TeamChestIDVector.clear();
		m_GetPresentNameVector.clear();
	}

public:
	// ��ȡ��������
	CT_BOOL GetIslandData(CT_CHAR* pBuf, CT_INT32& iLen, CT_INT32 iFTID);
	// �������������ID
	CT_VOID InsertGiveUserID(CT_DWORD dwFID) { _AUTOMUTEX(m_pMutex); m_GiveUserIDVector.push_back(dwFID); }
	// ��������ȡս�ӱ���
	CT_VOID InsertTeamChest(CT_INT32 iTeamLimit) { _AUTOMUTEX(m_pMutex); m_TeamChestIDVector.push_back(iTeamLimit); }
	// ���������Լ�����������ǳ�
	CT_VOID InsertPresentName(std::string &strName) {  _AUTOMUTEX(m_pMutex); m_GetPresentNameVector.push_back(strName);	}
	// ������
	CT_INT32 Throw();
	// �жϲ���ɵ�ǰ����
	CT_VOID CheckAndDoTask(CT_INT32 iTaskID,CT_INT32 iFTID);
	// ս���¼�
	CT_INT32 FightEvent(CT_INT32 iEventID);
	// �������
	CT_INT32 CompleteImmediately(CT_INT32& iEventID,CT_INT32 iFTID);
	// �����¼�
	CT_VOID GiveUpEvent();
	// ��ȡ����
	CT_INT32 SendReward(CT_INT32 iType, CT_INT32 iChestID, CT_INT32 iLimit, CT_INT32 iFTID);
	// ��ȡ��ǰ�����Ϣ
	CT_VOID GetPoint(Point& point){ point = m_Point; }
	// ��ȡ�����Ϣ
	CT_VOID GetPlayerInfo(DragonPlayer& player){ player = m_DPlayer; }
	// �ı����
	CT_VOID SetPoint(Point point);
	// ���������Ϣ
	CT_VOID SetPlayerInfo(DragonPlayer player);
	// �˳�ս��
	CT_VOID TeamOut();
	// ����ս��
	CT_VOID TeamIn(CT_INT32 iFTID);
	// ��鵱���Ƿ�������ͬһ���
	CT_BOOL CheckGiveUid(CT_DWORD dwFUID);
	// ���ս�ӻ��ֱ����Ƿ�����ȡ
	CT_BOOL CheckScoreChest(CT_INT32 iChestID);
	// ��������
	CT_VOID OpenFunction(CT_INT32 iFTID);
	// ��ʱ������������
	CT_VOID UpdateIslandOnClock(CT_INT32 iFTID);
	// �鿴�����Ƿ���
	CT_BOOL ViewIsland() { return m_DPlayer.iIsOpen == 0?CT_FALSE:CT_TRUE; }
	// �������Ͷ������
	CT_VOID AddThrowTimes(CT_INT32 iCount);
	// ����ս�ӻ���
	CT_VOID UpdateTeamScore(CT_INT32 iTeamID);

private:
	// �������ս�ӻ���
	CT_VOID UpdateFTMemberScore(CT_INT32 iScore, CT_INT32 iTeamID);
	// �������ݵ����ݿ�
	CT_VOID UpdateIslandData();
	// �������ݿ���־
	CT_VOID UpdateIslandDataLog();
	// ����ս�ӻ��ֹ���
	CT_VOID TeamScoreBroadcast(CT_INT32 iLimit, CT_INT32 iIndex, CT_CHAR* pName);
};

#endif