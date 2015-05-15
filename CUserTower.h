#ifndef CUSERTOWER_H_
#define CUSERTOWER_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"


struct TowerData
{
	CT_INT32	iCPID;
	CT_INT32	iTime;						// ������ͨ�ش���
	CT_INT32	iTreasure;					// ͨ�سɼ�(0��ʾ�Ѽ��δͨ�ع�)
};

struct TowerSlotAward
{
	CT_DWORD  dwUserID;                 //�û�id
	CT_INT32  iFloor;				    //��
	CT_INT32  iSlot;                    //��Դ��
	G_NormalReward gReward;			//����
};

struct TowerSlotAwardKey
{
	CT_DWORD dwUserID; 
	CT_INT32 iCPID; 	//�ؿ�
	CT_INT32 iSlot; 	//ռ���

	TowerSlotAwardKey()
	{
		memset(this, 0, sizeof(TowerSlotAwardKey));
	}

	TowerSlotAwardKey(const TowerSlotAwardKey &data)
	{
		memcpy(this, &data, sizeof(TowerSlotAwardKey));
	}

	TowerSlotAwardKey& operator=(const TowerSlotAwardKey &data)
	{
		memcpy(this, &data, sizeof(TowerSlotAwardKey));
	}

	bool operator<(const TowerSlotAwardKey& data) const
	{
		if (memcmp(this, &data, sizeof(TowerSlotAwardKey)) < 0)
		{
			return true;
		}
		return false;
	}
};

struct TowerMSG
{
	CT_DWORD	dwUserID;                 //�û�id
	CT_INT32	iType;					// 1.Ԫ���� 2.������
	CT_INT64	iTime;				  
	CT_CHAR		cMSG[255]; 
};

typedef CMD_G_ElementEvent TowerEvent;

struct MSC_TowerPVP_ResRobbery;
class CCUserTower
{
private:
	CCMutex*    m_pMutex;
	CCUser*     m_pUser;
	typedef std::map<CT_INT32, TowerData>						TowerDataMap;		// �ؿ�ID
	typedef std::map<CT_INT32, TowerData>::iterator				TD_IT;

	typedef std::map<TowerSlotAwardKey, G_NormalReward>			TowerSlotAwardMap;	// ռ����ջ�����
	typedef std::map<CT_INT64, TowerMSG>						TowerMSGMap;
	typedef std::map<CT_INT64, TowerEvent>						TowerEventMap;
	typedef std::map<CT_INT64, MSC_TowerPVP_ResRobbery>			TowerPVPMap;
	typedef std::set<CT_DWORD>									TowerHelpSet;

	CT_INT32												m_iMaxCPID;
	CT_BOOL													m_bEnable;
public:
	typedef std::map<CT_INT32, CT_FLOAT>						AwardStatusMap;		// ��������, ����ֵ

public:
	CCUserTower();
	~CCUserTower(void);

	TowerDataMap											m_TowerData;		// ��ͨ�صĹؿ�
	AwardStatusMap											m_AwardStatus;		// ��ͨ��������������

	TowerSlotAwardMap										m_TowerSlotAward;	// ռ����ջ�
	TowerMSGMap												m_TowerMSG;
	TowerEventMap											m_TowerEvent;
	TowerPVPMap												m_TowerPVP;
	TowerHelpSet											m_TowerHelp;

public:
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();

	static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const AwardStatusMap &awardStatusMap);
public:
	CT_BOOL IsEnable() {_AUTOMUTEX(m_pMutex);   return m_bEnable;}

	CT_VOID InsertTowerData(const TowerData &data, CT_BOOL bInit = CT_FALSE);

	CT_BOOL GetTowerData(CT_CHAR *pBuf, CT_INT32 &bufLen);			// �õ��ؿ���Ϣ
	CT_INT32 GetTowerMaxCP(){_AUTOMUTEX(m_pMutex);   return m_iMaxCPID;}

	CT_BOOL CheckPointPass(CT_INT32 iCPID, CT_INT32 iType=1);		// ���ĳ���Ƿ��Ѿ�ͨ�� iType:0.����ͨ��1.����ͨ�ع�
	CT_BOOL CheckPointRaids(CT_INT32 iEndCPID);			// ���ؿ��Ƿ����ɨ��
	CT_VOID UpdateRaids(CT_INT32 iBeginCPID, CT_INT32 iEndCPID);				// ����ɨ��
	CT_BOOL CheckHelpArea(CT_INT32 iCPID);

	CT_VOID UpdateCheckPoint(CT_INT32 iCPID, CT_INT32 iScore);		// ͨ�غ���¹ؿ����� & DB

	CT_VOID InsertTowerSlotAward(TowerSlotAward &towerSlotAward, CT_INT32 iDBInit=0);	// ����ռ�����������
	CT_BOOL GetTowerSlotAward(CT_CHAR *pBuf, CT_INT32 &bufLen);
	CT_VOID RecvTowerSlotAward(TowerSlotAwardKey &towerSlotAwardKey);		// ɾ��ռ�����������,������ȡ��
	CT_BOOL CheckTowerSlotAward(TowerSlotAwardKey &towerSlotAwardKey);		// ���ռ�����������
	CT_BOOL IsEmptySlotAward();										// �Ƿ���δ��ȡ����������

	CT_VOID InsertTowerMSG(const TowerMSG &msg);
	CT_VOID DeleteTowerMSG(CT_INT32 iType, CT_INT64 iTime);
	CT_BOOL GetTowerMSG(CT_CHAR *pBuf, CT_INT32 &bufLen);			// �õ��ؿ���Ϣ

	CT_INT32 GetPosByRandNum(CT_INT32 iTreasureGroupID);
	CT_VOID TreasureAbandon(CT_INT32 iCPID, CT_INT32 iTreasure);	//��������
	CT_VOID TreasureUse(CT_INT32 iCPID, CT_INT32 iTreasure,CT_INT32 iUseType);	//�򿪱���
	CT_BOOL HaveTreasure();		//����Ƿ���δ������

	CT_VOID RefreshTowerPass();

	CT_VOID InsertPVPInfo(const MSC_TowerPVP_ResRobbery &PVPInfo);
	CT_BOOL GetPVPInfo(CT_INT64 iFID, MSC_TowerPVP_ResRobbery *PVPInfo);
	CT_VOID  InsertEvent(const TowerEvent &towerEvent, CT_BOOL bLeftWin = CT_FALSE);
	CT_VOID InitPVPInfo(const MSC_TowerPVP_ResRobbery &PVPInfo);
	CT_VOID InitEvent(TowerEvent &towerEvent);
	CT_BOOL GetEventData(CT_CHAR *pBuf, CT_INT32 &bufLen);

	CT_VOID InitHelp(CT_DWORD dwFID) {_AUTOMUTEX(m_pMutex);   m_TowerHelp.insert(dwFID);}
	CT_VOID InsertHelp(CT_DWORD dwFID);
	CT_BOOL CheckHelp(CT_DWORD dwFID);
	CT_BOOL GetHelpData(CT_CHAR *pBuf, CT_INT32 &bufLen);

};
#endif

