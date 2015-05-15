/********************************************************************
	created:	2013/08/14
	created:	14:8:2013   16:50
	filename: 	CUserBattleFlag.h
	file base:	CUserBattleFlag
	file ext:	h
	author:		John
	
	purpose:	战旗！
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
 * 战旗ID说明
 * 领主的战旗，ID = 领主的职业ID
 * 军队的战旗，ID = 军队类型 = 军队ID / 10
 * 战旗上的插孔索引：参考宏: MAKE_ELM_INDEX
 * 非战旗上的孔：起始索引为: ITEM_SLOT_ELM + 1
 */

// 根据ID计算战旗类型
#define GET_FLAG_TYPE(ID) ((ID) > UNIT_ALL_ARMY ? (ID) / 10 : (ID))
// 根据战旗类型，计算装备格子的索引号
#define MAKE_ELM_INDEX(TYPE, IDX) (((TYPE) << 16) + (IDX))
// 判断一个索引号是否属于装备已装备的
#define IS_ELMIDX_EQUIPPED(IDX) ((IDX) > ITEM_SLOT_BATTLEFLAG)
#define GET_FLAG_BY_INDEX(IDX) ((IDX >> 16) & 0xffff)

struct BattleFlag
{
    CT_INT32 iType;     // 类型：领主职业/士兵ID
    CT_INT32 iLevel;    // 等级
};

class CCUserBattleFlag : public CCUserData<CT_INT32, BattleFlag>
{
protected:
    virtual CT_INT32 transData(const BattleFlag &src, CT_CHAR *pDst) const;
public:    
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE) { }
    // 获得战旗上插的元素
    CT_BOOL GetEquippedElms(CT_INT32 iType, CT_CHAR *pBuf, CT_INT32 &bufLen) const;
    // 指定的战旗上是否装备了某种类型的元素精华
    CT_BOOL HasElmOfType(const CT_INT32 flagType, const CT_INT32 elmType, const CT_INT32 IgnoreSlot = 0) const;
public:    
    // 移动元素
    CT_INT32 DoEquipElm(CT_INT32 iType, CT_INT32 iSlotFrom, CT_INT32 iSlotTo);
    // 战旗升级
    CT_INT32 UpgradeFlag(const CT_INT32 iType);
public:
    // 军队ID转为战旗ID
    inline static CT_INT32 UnitID2FlagID(int iType) { if (iType > CLASS_TYPE_MAX) iType /= 10; return iType; }
    // 更新战旗提供的战斗属性
    static CT_INT32 UpdateBattleStatus(MasterCombatStatus &mcs, const BattleFlag &flag, const CCUserItem::DataMap &elmPowerMap, const CCUserElmAttr::DataMap &empAttrMap);
};

#endif