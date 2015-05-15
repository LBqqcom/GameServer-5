/********************************************************************
	created:	2013/08/15
	created:	15:8:2013   8:56
	filename: 	CUserElm.h
	file base:	CUserElm
	file ext:	h
	author:		John
	
	purpose:	Ԫ�ؾ���������
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
    CT_DWORD key;                       // ��ʶĳһԪ�ص�key��Ψһ
    CT_INT32 iLevel;                    // ��ǰ�ĵȼ�
    CT_INT32 iExp;                      // �ܾ���ֵ
    CT_FLOAT attrs[ELM_ATTR_CNT];       // ���Ի���ֵ (����û���������)
};

class CCUserElmAttr : public CCUserData<CT_INT32, ElmMarrowAttr>
{
protected:
    virtual CT_INT32 transData(const ElmMarrowAttr &src, CT_CHAR *pDst) const;
private:
    const static int FACTOR = 10000;

    inline DataMap_IT getElmAttrIT(CT_INT32 iIndex);
    // һ��������þ���
    CT_BOOL getMarrowExp(ElmMarrowAttr &srcAttr, G_ElmMarrow &srcMarrow, CT_INT32 iExp);

public:
    // һ�������Ƿ�������
    CT_BOOL isMarrowFullLevel(ItemData &elmData) const;
public:
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE);
    // ����Ԫ�ؾ�����ʼ����
    CT_BOOL GenerateAttr(const ItemData &itData,CT_INT32 iGrade=1);
    // Ԫ������(1 vs 1)
    CT_INT32 DoEatElm(const CT_INT32 iSrc, const CT_INT32 iDst, CT_INT32 &nCrit, const CT_BOOL bSendData = CT_TRUE);
    // Ԫ������(1 vs n)
    CT_INT32 DoEatSomeElm(const CT_INT32 iSrc, const CT_INT32 arrayDst[], const CT_INT32 nCount = ELM_EAT_COUNT);
    // Ԫ��һ������ (1 vs all)
    CT_INT32 DoEatAllElm(const CT_INT32 arrayLock[], const CT_INT32 nCount);

    // ����Ԫ������
    CT_VOID FixElmAttr(void);
};
#endif