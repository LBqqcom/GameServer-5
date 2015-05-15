/********************************************************************
	created:	2012/10/13
	created:	13:10:2012   11:22
	file base:	CItemScriptSetting
	file ext:	h
	author:		xingweizhen
	
	purpose:	���߽����ű�
*********************************************************************/
#ifndef CITEMSCRIPTSETTING_H
#define CITEMSCRIPTSETTING_H

#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"

class CCItemScriptSetting: public CCSingleton<CCItemScriptSetting>
{
public:
    CCItemScriptSetting(void);
    ~CCItemScriptSetting(void);
public:
    // ��ȡ��Ʒ��������
     CT_BOOL GetItemBase(int iID, G_ItemBase &item);
    // ��ñ�ʯ����
     CT_BOOL GetItemGem(int iID, G_ItemGem &item);
    // �������������
     CT_BOOL GetItemDesign(int iID, G_ItemDesign &item);
    // ��������������
     CT_BOOL GetItemBuff(int iID, G_ItemBuff &item);
    // �����Ƭ����
     CT_BOOL GetItemChip(int iID, G_ItemChip &item);
    // ���װ������
     CT_BOOL GetEquipment(int iID, G_Equipment &item);
    // �����Դ��������
     CT_BOOL GetItemResource(int iID, G_ItemResource &item);
     // ��ñ����������
     CT_BOOL GetItemChest(int iID, G_ItemChest &item);
};


#endif // CITEMSCRIPTSETTING_H