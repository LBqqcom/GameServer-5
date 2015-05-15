/********************************************************************
	created:	2013/08/15
	created:	15:8:2013   19:40
	filename: 	CUserEnchant.h
	file base:	CUserEnchant
	file ext:	h
	author:		John
	
	purpose:	
*********************************************************************/
#ifndef CUSERENCHANT_H_
#define CUSERENCHANT_H_

#include <map>
#include <vector>
#include "CGDefine.h"
#include "CMutex.h"
#include "CApi.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"
#include "CUserData.h"
#include "CUserBase.h"

class CCUser;

struct EnchantData
{
    CT_INT32         iBID;               // ��λ
    CT_INT32         iIDX;               // ���
    CT_INT32         iGrade;             // ��ħ�ȼ� 
	CT_INT32         iFailCount;		 // ʧ�ܴ���
};

class CCUserEnchant : public CCUserData<CT_INT64, EnchantData>
{
    friend class CCUserItem;
public:
	typedef std::vector<CT_INT32>        EnchantRewardVector;            //�ѻ�ý���
	EnchantRewardVector  m_EnchantRewardVector;
public: 
	inline CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser; 
		m_pMutex = pMutex; 
		m_DataMap.clear(); 
		m_lastSyncTime= time(0);
		m_EnchantRewardVector.clear();
	}
	inline CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex); 
		m_DataMap.clear(); 
		m_lastSyncTime= 0;
		m_EnchantRewardVector.clear();
	}
protected:
    virtual CT_INT32 transData(const EnchantData &src, CT_CHAR *pDst) const;
public:
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE) { }
    // ����/��ħ
    CT_INT32 ActiveEnchant(CT_INT32 iBID, CT_INT32 iIDX, CT_INT32 iEnchantItem);
    // ���������ȼ���鸽ħ�Զ�����
    CT_VOID CheckEnchatActive(const CT_INT32 iLevel);
	// ����ħ�ɹ�
	CT_VOID EnchantSuc(CT_INT32 iBID, CT_INT32 iIDX, CT_INT32 iGrade);
	// �����Ž���
	ENUM_EnchantRewardFail EnchantReward(CT_INT32 iIndex, ENUM_UseItemFail &itemFail);
	// �õ��ѷ��Ž���
	CT_BOOL GetEnchantReward(CT_CHAR *pBuf, CT_INT32 &bufLen);
	// �õ����Խ���
	CT_VOID GetEnchantAttrReward();
public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const DataMap &enchantMap, const EnchantRewardVector &rewardVector);
};
#endif