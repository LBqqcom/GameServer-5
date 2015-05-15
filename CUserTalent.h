/********************************************************************
	created:	2013/08/20
	created:	20:8:2013   14:42
	filename: 	CUserTalent.h
	file base:	CUserTalent
	file ext:	h
	author:		John
	
	purpose:	
*********************************************************************/
#ifndef CUSERTALENT_H_
#define CUSERTALENT_H_
#include <map>
#include "CGDefine.h"
#include "CMD_Game.h"
#include "CMutex.h"
#include "CUserData.h"
#include "CUserBase.h"
#include "GlobeScriptInterface.h"

#define GET_TARGET_TYPE(ID) ((ID) > UNIT_ALL_ARMY ? (ID) / 10 : (ID))

// Ǳ����Ϣ
enum TALENT_STATUS
{
    TALENT_ACTIVATED    = 0,        // Ǳ���Ѽ���
    TALENT_ACTIVATABLE  = 1,        // Ǳ�ܿɼ���
    TALENT_UNACTIVED    = 2,        // Ǳ��δ����
};

struct TalentData
{
    CT_INT32 iID;                   // Ǳ��ID
    CT_CHAR  szName[NAME_SIZE];     // Ǳ������
    CT_INT32 iStatus;               // Ǳ��״̬ TALENT_STATUS
};

class CCUserTalent : public CCUserData<CT_INT32, TalentData>
{
    friend class CCUserSkill;
protected:
    virtual CT_INT32 transData(const TalentData &src, CT_CHAR *pDst) const;
public:
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE) { }
    CT_INT32 GetDataCount(void) { _AUTOMUTEX(m_pMutex); return m_DataMap.size(); }
    CT_INT32 LearnTalent(CT_INT32 iTalentID);
public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 *arrayTargets, const CT_INT32 nTarget, const DataMap &talMap);
};

#endif // CUSERTALENT_H_