/********************************************************************
	created:	2012/10/13
	created:	13:10:2012   11:22
	file base:	CItemScriptSetting
	file ext:	h
	author:		xingweizhen
	
	purpose:	道具解析脚本
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
    // 获取物品基本属性
     CT_BOOL GetItemBase(int iID, G_ItemBase &item);
    // 获得宝石属性
     CT_BOOL GetItemGem(int iID, G_ItemGem &item);
    // 获得制作书属性
     CT_BOOL GetItemDesign(int iID, G_ItemDesign &item);
    // 获得增益道具属性
     CT_BOOL GetItemBuff(int iID, G_ItemBuff &item);
    // 获得碎片属性
     CT_BOOL GetItemChip(int iID, G_ItemChip &item);
    // 获得装备属性
     CT_BOOL GetEquipment(int iID, G_Equipment &item);
    // 获得资源道具属性
     CT_BOOL GetItemResource(int iID, G_ItemResource &item);
     // 获得宝箱道具属性
     CT_BOOL GetItemChest(int iID, G_ItemChest &item);
};


#endif // CITEMSCRIPTSETTING_H