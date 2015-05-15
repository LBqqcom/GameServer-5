/********************************************************************
	created:	2013/08/15
	created:	15:8:2013   8:56
	filename: 	CUserElm.h
	file base:	CUserElm
	file ext:	h
	author:		John
	
	purpose:	元素精华的属性
*********************************************************************/

#ifndef CUSERELM_H_
#define CUSERELM_H_

#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserData.h"
#include "CUserItem.h"

struct ElmMarrowAttr
{
    CT_DWORD key;                       // 标识某一元素的key，唯一
    CT_INT32 iLevel;                    // 当前的等级
    CT_INT32 iExp;                      // 总经验值
    CT_FLOAT attrs[ELM_ATTR_CNT];       // 属性基础值 (残渣没有这个属性)
};

class CCUserElmAttr : public CCUserData<CT_INT32, ElmMarrowAttr>
{
protected:
    virtual CT_INT32 transData(const ElmMarrowAttr &src, CT_CHAR *pDst) const;
private:
    const static int FACTOR = 10000;

    inline DataMap_IT getElmAttrIT(CT_INT32 iIndex);
    // 一个精华获得经验
    CT_BOOL getMarrowExp(ElmMarrowAttr &srcAttr, G_ElmMarrow &srcMarrow, CT_INT32 iExp);

public:
    // 一个精华是否满级了
    CT_BOOL isMarrowFullLevel(ItemData &elmData) const;
public:
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);
    // 产生元素精华初始属性
    CT_BOOL GenerateAttr(const ItemData &itData,CT_INT32 iGrade=1);
    // 元素吞噬(1 vs 1)
    CT_INT32 DoEatElm(const CT_INT32 iSrc, const CT_INT32 iDst, CT_INT32 &nCrit, const CT_BOOL bSendData = CT_TRUE);
    // 元素吞噬(1 vs n)
    CT_INT32 DoEatSomeElm(const CT_INT32 iSrc, const CT_INT32 arrayDst[], const CT_INT32 nCount = ELM_EAT_COUNT);
    // 元素一键吞噬 (1 vs all)
    CT_INT32 DoEatAllElm(const CT_INT32 arrayLock[], const CT_INT32 nCount);

    // 修正元素属性
    CT_VOID FixElmAttr(void);
};
#endif