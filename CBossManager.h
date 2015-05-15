/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CBossManager.h
Version:		1.0
Description:    ����BOSS����
Author:			zhoujun
Create Date:	2013-12-9
History:            

******************************************************************/
#ifndef CCBOSS_MANAGER_H_
#define CCBOSS_MANAGER_H_
#include <map>
#include <string>
#include "CNetInterface.h"
#include "CSingleton.h"
#include "CMD_Game.h"
#include "CMutex.h"

#include "CThreadMes.h"

struct BossHPHurt
{
	CT_DWORD	dwUID;
	CT_INT32	iHP;
	CT_CHAR		cUserName[NAME_SIZE];
	CT_FLOAT	fHurtPercent;
	CT_INT64	iBattleTime;
	CT_INT64	iActTime;		//��һ�μ���ʱ��
	CT_INT32	iActCount;		//��������
	CT_BOOL		bBattleFlag;	//��ս��
	CT_BOOL		bActCoolFlag;	//��������ȴ
	CT_BOOL		bInActive;
};

struct BattleUser
{
	CT_DWORD	dwUID;
	CT_INT32	iDiamod;			//1.ǿ������ʯ��
};

typedef std::map<CT_DWORD, BossHPHurt>			UserHurtMap;	//��ǰ�˺��б� dwUID:HPHurt
typedef std::map<CT_INT32, BossHPHurt>			TopHurtMap;		//BOSS�Ӱ� iTop:HPHurt
typedef std::map<CT_INT32, TopHurtMap>		BossHurtMap;		//����BOSS���а� AEID:TopHurtMap
typedef std::multimap<CT_INT32, CT_DWORD>	HPTopMap;			//��ǰ���� iHP:dwUID
typedef std::map<CT_DWORD, CMD_G_WorldBossActiveReward> BossRewardMap;

//BOSS������
class CCBossManager : public CCSingleton<CCBossManager>
{
private:
	//������
	CCMutex								m_mutex;  

	CT_INT64							m_lBeginTime;			//��ʼʱ��
	CT_INT64							m_lEndTime;				//ʵ�ʽ���ʱ��
	CT_INT64							m_lLastTime;			//ʱ��
	CT_INT32							m_iBUffID;				//ȫ��BUFF
	CT_INT32							m_iAEID;				//�ID
	CT_BOOL								m_bActive;				//�״̬
	CT_INT32							m_iPreFinishTime;		//�ϴ����ʱ��(���ʼ�����,��������)
	CT_INT32							m_iPrePlayer;			//�ϴβ�������(���ʼ�����,��������)
	CT_INT32							m_iBossGrade;			//BOSS�ȼ�(�豣��)
	CT_INT32							m_iBossHPMax;
	CT_INT32							m_iBossHP;

	UserHurtMap							m_HPHurtMap;			//��ǰBOSS�˺�
	BossHurtMap							m_BossHurtMap;			//����BOSS�˺���
	HPTopMap							m_HPTopMap;				//HP, U_ID
	BossRewardMap						m_RewardMap;			//����

	std::list<BattleUser>				m_BattleList;			//ս���Ŷ�

public:
	CCBossManager(CT_VOID);
	~CCBossManager(CT_VOID);

public:
	CT_VOID SetActive(CT_BOOL bActive);
	CT_INT32 GetBossGrade(){_AUTOMUTEX(&m_mutex);return m_iBossGrade;};
	//��ȡ�
	CT_BOOL GetActive(){_AUTOMUTEX(&m_mutex);return m_bActive;};
	//����ʣ��ʱ��
	CT_VOID SetLastTime(CT_INT64 lLastTime){_AUTOMUTEX(&m_mutex);m_lLastTime=lLastTime;};
	//��ȡʣ��ʱ��
	CT_INT64 GetLastTime(){_AUTOMUTEX(&m_mutex);return m_lLastTime;};
	//���ûID
	CT_VOID SetAEID(CT_INT32 iAEID){_AUTOMUTEX(&m_mutex);m_iAEID=iAEID;};
	//��ȡ�ID
	CT_INT32 GetAEID(){_AUTOMUTEX(&m_mutex);return m_iAEID;};
	//��ȡ��ǰ�˺�����
	CT_BOOL GetHPHurt(CT_DWORD dwUID, CT_CHAR *pBuf, CT_INT32 bufLen);
	//����ʵʱ����
	CT_VOID SendHPHurt();
	//��ȡ�����˺�����
	CT_BOOL GetBossHurt(CT_DWORD dwUID, CT_CHAR *pBuf, CT_INT32 &bufLen);
	//�μӻ
	CT_VOID JoinActive(CT_DWORD dwUID);
	CT_VOID ExitActive(CT_DWORD dwUID);
	//����
	CT_VOID Encourage(CT_DWORD dwUID, CT_INT32 iType);
	//����ս�� iFlog==1Ϊǿ��
	CT_VOID BattleApply(CT_DWORD dwUID, CT_INT32 iFlag);
	//ս���߳�ս������
	CT_VOID BattleResult(CT_DWORD dwUID, CT_INT32 iHP);

	
	CT_VOID SendReward(CT_DWORD dwUID);		//���ͽ���
	CT_VOID RecvReward(CT_DWORD dwUID);		//��ȡ����
	CT_VOID UserLogin(CT_DWORD dwUID);

	CT_BOOL BossIsDie() {_AUTOMUTEX(&m_mutex); return (m_iBossHP<=0);};
	CT_INT32 GetBossHP() {_AUTOMUTEX(&m_mutex); return m_iBossHP;};
	CT_INT32 GetBossHPMax() {_AUTOMUTEX(&m_mutex); return m_iBossHPMax;};
	CT_VOID RefCurrTop();
	CT_VOID RefPlayer();

	CT_VOID InitBossHurt(MSC_DB_WorldBossTop &dbBossTop);
	CT_VOID SaveDB();
	CT_VOID RefTop();

private:
	CT_VOID RefBoss(CT_INT32 iBossID);
};


#endif