/********************************************************************
	created:	2013/08/14
	created:	14:8:2013   16:50
	filename: 	CUserBattleFlag.h
	file base:	CUserBattleFlag
	file ext:	h
	author:		John
	
	purpose:	ս�죡
*********************************************************************/
#ifndef CCUSERBATTLEFLAG_H_
#define CCUSERBATTLEFLAG_H_

#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserData.h"
#include "CUserElmAttr.h"
#include "CUserBase.h"
#include "CUserItem.h"
/*
 * ս��ID˵��
 * ������ս�죬ID = ������ְҵID
 * ���ӵ�ս�죬ID = �������� = ����ID / 10
 * ս���ϵĲ���������ο���: MAKE_ELM_INDEX
 * ��ս���ϵĿף���ʼ����Ϊ: ITEM_SLOT_ELM + 1
 */

// ����ID����ս������
#define GET_FLAG_TYPE(ID) ((ID) > UNIT_ALL_ARMY ? (ID) / 10 : (ID))
// ����ս�����ͣ�����װ�����ӵ�������
#define MAKE_ELM_INDEX(TYPE, IDX) (((TYPE) << 16) + (IDX))
// �ж�һ���������Ƿ�����װ����װ����
#define IS_ELMIDX_EQUIPPED(IDX) ((IDX) > ITEM_SLOT_BATTLEFLAG)
#define GET_FLAG_BY_INDEX(IDX) ((IDX >> 16) & 0xffff)

struct BattleFlag
{
    CT_INT32 iType;     // ���ͣ�����ְҵ/ʿ��ID
    CT_INT32 iLevel;    // �ȼ�
};

class CCUserBattleFlag : public CCUserData<CT_INT32, BattleFlag>
{
protected:
    virtual CT_INT32 transData(const BattleFlag &src, CT_CHAR *pDst) const;
public:    
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE) { }
    // ���ս���ϲ��Ԫ��
    CT_BOOL GetEquippedElms(CT_INT32 iType, CT_CHAR *pBuf, CT_INT32 &bufLen) const;
    // ָ����ս�����Ƿ�װ����ĳ�����͵�Ԫ�ؾ���
    CT_BOOL HasElmOfType(const CT_INT32 flagType, const CT_INT32 elmType, const CT_INT32 IgnoreSlot = 0) const;
public:    
    // �ƶ�Ԫ��
    CT_INT32 DoEquipElm(CT_INT32 iType, CT_INT32 iSlotFrom, CT_INT32 iSlotTo);
    // ս������
    CT_INT32 UpgradeFlag(const CT_INT32 iType);
public:
    // ����IDתΪս��ID
    inline static CT_INT32 UnitID2FlagID(int iType) { if (iType > CLASS_TYPE_MAX) iType /= 10; return iType; }
    // ����ս���ṩ��ս������
    static CT_INT32 UpdateBattleStatus(MasterCombatStatus &mcs, const BattleFlag &flag, const CCUserItem::DataMap &elmPowerMap, const CCUserElmAttr::DataMap &empAttrMap);
};

#endif