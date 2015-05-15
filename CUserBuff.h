/********************************************************************
	created:	2012/10/10
	created:	10:10:2012   13:42
	file base:	CUserBuff
	file ext:	h
	author:		xingweizhen
	
	purpose:	�û�����
*********************************************************************/
#ifndef CUSERBUFF_H
#define CUSERBUFF_H
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CUserData.h"
#include "CUserBase.h"

class CCUser;
class CCMutex;

struct CMD_G_BuffData;
struct G_ItemBuff;
struct ItemData;
struct BuffData
{
    CT_INT32 key;           // Ψһ��ʶ
    CT_INT32 iGroup;        // ������ID
    CT_INT32 iID;           // ����(����)ID    
    CT_INT64 toggleTime;    // ��������ʱ��
    CT_INT32 iDuration;     // ����ʱ��
};
class CCUserBuff : public CCUserData<CT_INT32, BuffData>
{
    friend class CCUserItem;
protected:
    virtual CT_INT32 transData(const BuffData &src, CT_CHAR *pDst) const;
public:
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);
    // ��������״̬
    CT_VOID UpdateItemBuff(void);
    // ���������Ƿ����
    CT_BOOL SearchBuffData(const G_ItemBuff &itBuff, BuffData &buffData);

	CT_VOID BuffReward(CT_INT32 iID, CT_INT64 iBeginTime);
public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const DataMap &buffMap);
};
#endif // CUSERBUFF_H