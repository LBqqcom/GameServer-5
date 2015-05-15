#include "CItemScriptSetting.h"
#include "CUser.h"

template<> CCItemScriptSetting* CCSingleton<CCItemScriptSetting>::ms_Singleton=0;

CCItemScriptSetting::CCItemScriptSetting(void)
{
}

CCItemScriptSetting::~CCItemScriptSetting(void)
{
}

// 获取物品基本属性
CT_BOOL CCItemScriptSetting::GetItemBase(int iID, G_ItemBase &item)
{
    return 0 != G_GetItemBase(iID, &item);
}
// 获得宝石属性
CT_BOOL CCItemScriptSetting::GetItemGem(int iID, G_ItemGem &item)
{
    return 0 != G_GetItemGem(iID, &item);
}
// 获得制作书属性
CT_BOOL CCItemScriptSetting::GetItemDesign(int iID, G_ItemDesign &item)
{
    return 0 != G_GetItemDesign(iID, &item);
}
// 获得增益道具属性
CT_BOOL CCItemScriptSetting::GetItemBuff(int iID, G_ItemBuff &item)
{
    return 0 != G_GetItemBuff(iID, &item);
}
// 获得碎片属性
CT_BOOL CCItemScriptSetting::GetItemChip(int iID, G_ItemChip &item)
{
    return 0 != G_GetItemChip(iID, &item);
}
// 获得装备属性
CT_BOOL CCItemScriptSetting::GetEquipment(int iID, G_Equipment &item)
{
    return 0 != G_GetEquipment(iID, &item);
}
// 获得资源道具属性
CT_BOOL CCItemScriptSetting::GetItemResource(int iID, G_ItemResource &item)
{
    return 0 != G_GetItemResource(iID, &item);
}
// 获得宝箱道具属性
CT_BOOL CCItemScriptSetting::GetItemChest(int iID, G_ItemChest &item)
{
    return 0 != G_GetItemChest(iID, &item);
}