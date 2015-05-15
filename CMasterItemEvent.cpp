#include "CMasterItemEvent.h"
#include "CMD_Game.h"
#include "CMasterScriptSetting.h"
#include "CItemScriptSetting.h"
#include "CGameConfig.h"
#include "CThreadMes.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CDBMasterEvent.h"
#include "CLogMeg.h"
#include "CFightTeamManager.h"

CCMasterItemEvent::CCMasterItemEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{    
    // 注册消息
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MOVE_ITEM);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_INC_BAGSLOT);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_SORT_BAG);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_SMITHING_EQUIP);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_FORGE_EQUIP);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_FORGE_EQUIP_CONFIRM);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_PUNCH_EQUIP);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_EMBED_EQUIP);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_INHERIT_EQUIP);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_SELL_ITEM);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_CLEAR_CD);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_REWARD);

    RegistMessage(MAIN_G_ELEMENT_POWER, SUB_G_EAT_ELM);
    RegistMessage(MAIN_G_ELEMENT_POWER, SUB_G_EAT_SOMEELM);
    RegistMessage(MAIN_G_ELEMENT_POWER, SUB_G_EAT_ALLELM);
    RegistMessage(MAIN_G_ELEMENT_POWER, SUB_G_EQUIP_ELM);
    RegistMessage(MAIN_G_ELEMENT_POWER, SUB_G_INC_ELMSLOT);
	RegistMessage(MAIN_G_ELEMENT_POWER, SUB_G_SORT_BAG);

    RegistMessage(MSC_MAIN_USR_ITEM_UPDATE, MSC_SUB_USR_ADD_ITEM);

	//Begin[10/23/2012 wangchen]
	RegistMessage(MSC_MAIN_USR_ITEM_UPDATE, MSC_SUB_G_DELETE_ITEM);
	//End[10/23/2012 wangchen]
	RegistMessage(MSC_MAIN_USR_ITEM_UPDATE, MSC_SUB_DB_W_MASTER_NAME);

    if (m_dwThread == GAME_THREAD_CITY) {
        // 定时器
        G_ValueRange vr = {0};
        G_GetValueRange(VALUE_TYPE_BUFF_INTERVAL, &vr);
        CT_INT32 interval = (CT_INT32)vr.fMaxVal;
        if (interval == 0) interval = 10;
        RegistTimer(IDI_MASTER_BUFF_TIMER);
        SetTimer(IDI_MASTER_BUFF_TIMER, interval * 1000);

        G_GetValueRange(VALUE_TYPE_ITEM_SYNC_INTERVAL, &vr);
        interval = (CT_INT32)vr.fMaxVal;
        if (interval == 0) interval = 60;
        RegistTimer(IDI_MASTER_ITEM_SYNC_TIMER);
        SetTimer(IDI_MASTER_ITEM_SYNC_TIMER, interval * 1000);

		G_ValueRange vrt = {0};
		G_GetValueRange(VALUE_TYPE_TITLE_INTERVAL, &vrt);
		interval = (CT_INT32)vrt.fMaxVal;
		if (interval == 0) interval = 10;
		RegistTimer(IDI_MASTER_TITLE_TIMER);
		SetTimer(IDI_MASTER_TITLE_TIMER, interval * 1000);
	}
}

CCMasterItemEvent::~CCMasterItemEvent(void)
{
}

//处理用户消息
CT_VOID CCMasterItemEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
    switch (mainID)
    {
    case MAIN_G_MASTER_EVENT_CITY:
        DisposeItemEvent(subID,  pBuf, len);
        break;
    case MSC_MAIN_USR_ITEM_UPDATE :
        DisposeUsrItemUpdate(subID, pBuf, len);
        break;
    case MAIN_G_ELEMENT_POWER:
        DisposeElmPowerEvent(subID, pBuf, len);
        break;
    }
}


//用户定时器处理
CT_VOID CCMasterItemEvent::OnTime(CT_DWORD timeID)
{
    switch (timeID)
    {
    case IDI_MASTER_BUFF_TIMER:
		{
			CCUserManager::getSingleton().UpdateItemBuff();
		}
        break;
    case IDI_MASTER_ITEM_SYNC_TIMER :
        CCUserManager::getSingleton().SyncItemData();
        break;
	case IDI_MASTER_TITLE_TIMER:
		{
			CCUserManager::getSingleton().UpdataTitleStatus();
			break;
		}
    }
}

//////////////////////////////////////////////////////////////////////////
//处理道具事件
CT_VOID CCMasterItemEvent::DisposeItemEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
    switch (subID)
    {
    case SUB_G_USE_ITEM :
        if (sizeof(CMD_G_UseItem) == len) DisposeUseItem(*(CMD_G_UseItem *)pBuf);
        return;
    case SUB_G_MOVE_ITEM :
        if (sizeof(CMD_G_MoveItem) == len) DisposeMoveItem(*(CMD_G_MoveItem *)pBuf);
        return;
    case SUB_G_INC_BAGSLOT :
        if (sizeof(CMD_G_IncBagSlot) == len) DisposeIncBagSlot(*(CMD_G_IncBagSlot *)pBuf);
        return;
    case SUB_G_SORT_BAG :
        if (sizeof(CMD_G_SortBag) == len) DisposeSortBag(*(CMD_G_SortBag *)pBuf);
        return;
    case SUB_G_SMITHING_EQUIP :
        if (sizeof(CMD_G_SmithingEquip) == len) DisposeSmithingEquip(*(CMD_G_SmithingEquip *)pBuf);
        return;
    case SUB_G_FORGE_EQUIP :
        if (sizeof(CMD_G_ForgeEquip) == len) DisposeForgeEquip(*(CMD_G_ForgeEquip *)pBuf);
        return;
	case SUB_G_FORGE_EQUIP_CONFIRM :
		if (sizeof(CMD_G_ForgeEquipConfirm) == len) DisposeForgeEquipConfirm(*(CMD_G_ForgeEquipConfirm *)pBuf);
		return;
    case SUB_G_PUNCH_EQUIP :
        if (sizeof(CMD_G_PunchEquip) == len) DisposePunchEquip(*(CMD_G_PunchEquip *)pBuf);
        return;
    case SUB_G_EMBED_EQUIP :
        if (sizeof(CMD_G_EmbedEquip) == len) DisposeEmbedEquip(*(CMD_G_EmbedEquip *)pBuf);
        return;
    case SUB_G_INHERIT_EQUIP :
        if (sizeof(CMD_G_InheritEquip) == len) DisposeInheritEquip(*(CMD_G_InheritEquip *)pBuf);
        return;
    case SUB_G_SELL_ITEM :
        if (sizeof(CMD_G_SellItem) == len) DisposeSellItem(*(CMD_G_SellItem *)pBuf);
        return;
    case SUB_G_CLEAR_CD :
        if (sizeof(CMD_G_ClearCD) == len) DisposeClearCD(*(CMD_G_ClearCD *)pBuf);
        return;
    case SUB_G_ENCHANT:
        if (sizeof(CMD_G_Enchant) == len) DisposeEnchant(*(CMD_G_Enchant *)pBuf);
		return;
	case SUB_G_ENCHANT_REWARD:
		if (sizeof(CMD_G_EnchantReward) == len) DisposeEnchantReward(*(CMD_G_EnchantReward*)pBuf);
        return;
    }


    CCLogMeg::getSingleton().WriteError("CCMasterItemEvent::OnUserEvent : subID=%i", subID);
}

CT_VOID CCMasterItemEvent::DisposeElmPowerEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
    switch (subID)
    {
    case SUB_G_EAT_ELM:
        if (sizeof(CMD_G_EatElm) == len) DisposeEatElm(*(CMD_G_EatElm *)pBuf);
        return;
    case SUB_G_EAT_SOMEELM:
        if (sizeof(CMD_G_EatSomeElm) == len) DisposeEatSomeElm(*(CMD_G_EatSomeElm *)pBuf);        
        return;
    case SUB_G_EAT_ALLELM:
        if (sizeof(CMD_G_EatAllElm) == len) DisposeEatAllElm(*(CMD_G_EatAllElm *)pBuf);
        return;
    case SUB_G_EQUIP_ELM:
        if (sizeof(CMD_G_EquipElm) == len) DisposeEquipElm(*(CMD_G_EquipElm *)pBuf);
        return;
    case SUB_G_INC_ELMSLOT:
        if (sizeof(CMD_G_IncBagSlot) == len) DisposeIncElmSlot(*(CMD_G_IncBagSlot *)pBuf);
        return;
	case SUB_G_SORT_BAG:
		if (sizeof(CMD_G_SortBag) == len) DisposeElmBag(*(CMD_G_SortBag *)pBuf);
    }


    CCLogMeg::getSingleton().WriteError("CCMasterItemEvent::OnUserEvent : subID=%i", subID);
}

CT_VOID CCMasterItemEvent::DisposeUsrItemUpdate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
    switch (subID)
    {
    case MSC_SUB_USR_ADD_ITEM :
        if (sizeof(MSC_USR_AddItem) == len) DisposeAddItem(*(MSC_USR_AddItem *)pBuf);
        break;
	case MSC_SUB_DB_W_MASTER_NAME:
		{
			DisposeChangeMasterName(pBuf,len);
			break;
		}
    }
}

//////////////////////////////////////////////////////////////////////////

// 处理使用道具事件（装备/卸下/合成/使用）
CT_VOID CCMasterItemEvent::DisposeUseItem(const CMD_G_UseItem &useItem)
{
    CCUser* pUser = 0;
    ENUM_UseItemFail enFail = EUI_DATA_ERROR;

    if(CCUserManager::getSingleton().Find(useItem.dwUserID, &pUser))
    {
    	if (useItem.iCount <= 0) goto FAIL;
        // 从列表中查找使用的道具
        ItemData item = {0};
        if (!pUser->m_userItem.GetData(useItem.iIndex, item))
        {
            enFail = EUI_DIFF_EXIST;
            goto FAIL;
        }

        G_ItemBase itBase = {0};
        if (!CCItemScriptSetting::getSingleton().GetItemBase(item.iID, itBase)) goto FAIL;   // 配置数据有误
        if (!pUser->m_userItem.CheckItemUsable(itBase))
        {
            enFail = EUI_CANNOT_USE;
            goto FAIL;
        }

        ItemData useIt = {0};
        pUser->m_userItem.GetData(useItem.iIndex, useIt);
        useIt.iCount = useItem.iCount;

        switch (itBase.iTID)
        {
        case ITEM_TYPE_DESIGN:
            enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemDesign(useIt);
            break;
        case ITEM_TYPE_BUFF:
            enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemBuff(useIt);
            break;
		case ITEM_TYPE_PET_SKILL:
        case ITEM_TYPE_CHIP:
        case ITEM_TYPE_GEM :
            enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemChip(useIt);                   
            break;
        case ITEM_TYPE_EQUIPMENT:
		case ITEM_TYPE_PET_EQUIPMENT:
            enFail = (ENUM_UseItemFail)pUser->m_userItem.UseEquipment(useIt);                   
            break;
        case ITEM_TYPE_CHEST:
            enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemChest(useIt);
            break;
        case ITEM_TYPE_RESOURCE:
            enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemResource(useIt);
            break;
		/*case ITEM_TYPE_ENCHANTITEM:
			enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemEnchant(useIt);
			break;*/
        case ITEM_TYPE_BROADCAST:
            enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemBroadCast(useIt);
            break;
		case ITEM_TYPE_PET_EGG:
			enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemPetEgg(useIt);
			break;
		case ITEM_TYPE_TITLE:
			enFail = (ENUM_UseItemFail)pUser->m_userItem.UserItemTitle(useIt);
			break;
		case ITEM_TYPE_CHANGEMASTERNAME_CARD:
			{
				if(strlen(useItem.cName) == 0)
				{
					enFail=EUI_NAME_SAME;
					break;
				}
				else if(pUser->m_userItem.AutoExpendItem(SAVETAG_ACTIVE_TITLE, useIt.iID, 1, ITEM_OP_MODE_CHECK))
				{
					MSC_DB_MasterName mn={0};
					mn.dwUserID=pUser->GetUserID();
					mn.iPropID=useIt.iID;
					memcpy(mn.cName,useItem.cName,NAME_SIZE);
					CT_INT32 threadID =  GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
					G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA,MSC_SUB_DB_W_MASTER_NAME , (CT_CHAR*)&mn, sizeof(mn));

					CT_DWORD dwFTID=CCFightTeamManager::getSingleton().GetFightTeamIDByUser(pUser->GetUserID());
					if(dwFTID != 0)
					{
						CCFightTeam *pFt=NULL;
						if(CCFightTeamManager::getSingleton().Find(dwFTID,&pFt))
						{
							if(pFt->GetFightTeamBossID() == pUser->GetUserID())
							{
								pFt->SetFightTeamBossName((CT_CHAR*)useItem.cName);
							}
						}
					}

					return;
				}
				enFail=EUI_NO_COUNT;
				break;
			}
		case ITEM_TYPE_CHANGEFTNAME_CARD:
			{
				if(strlen(useItem.cName) == 0)
				{
					enFail=EUI_FTNAME_SAME;
					break;
				}
				if(pUser->m_userItem.AutoExpendItem(SAVETAG_ACTIVE_TITLE, useIt.iID, 1, ITEM_OP_MODE_CHECK))
				{
					MSC_DB_FTName ftn={0};
					ftn.dwUserID=pUser->GetUserID();
					ftn.iPropID=useIt.iID;
					memcpy(ftn.cName,useItem.cName,NAME_SIZE);
					CT_INT32 threadID =  GAME_THREAD_CITY + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
					G_SendThreadInfo(threadID,  MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_CHANGE_FTNAME , (CT_CHAR*)&ftn, sizeof(ftn));

					return;
				}
				enFail=EUI_NO_COUNT;
				break;
			}
		case ITEM_TYPE_BOSS_CALL:
			{
				enFail = (ENUM_UseItemFail)pUser->m_userItem.UserItemBoss(useIt);
				break;
			}
		case ITEM_TYPE_ELM_DESIGN:
			{
				enFail = (ENUM_UseItemFail)pUser->m_userElmPower.UseItemElmPowerDesign(useIt);
				break;
			}
		case ITEM_TYPE_ATTRIBUTE_ADD:
			{
				enFail = (ENUM_UseItemFail)pUser->m_userItem.UseItemAttributeAdd(useIt);
				break;
			}
        default:
            enFail = EUI_UNUSABLE;
            goto FAIL;
        }
        if (enFail != EUI_SUCCESS)
        {
            goto FAIL;
        }

		//记录日志
		MSC_LOGDB_UserItem LogUserItem={0};
		LogUserItem.dwUserID=pUser->GetUserID();
		LogUserItem.iItemID=useIt.iID;
		LogUserItem.iItemNum=useIt.iCount;
		CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
		G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_USESITEM,(CT_CHAR*)&LogUserItem,sizeof(LogUserItem));					

#if 0
		// 道具使用成功 - 为了解决1.037版客户端没有提示道具获得信息
        if (itBase.iTID != ITEM_TYPE_CHEST) {
            CT_INT32 type = 0;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_CHEST_ITEM, 
                (CT_CHAR*)&type, sizeof(type), false, true);
        }
#endif

		//add by caihan
		//使用道具，判断道具任务
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_USE;
		taskEventInfo.iSubType = useIt.iID;
		pUser->m_userTask.DisposeTask(taskEventInfo);
		//end

        return;
    }

FAIL:
    CMD_G_UseItemFail stFail = {0};
    stFail.dwUserID = pUser ? pUser->GetUserID() : 0;
    stFail.enFail = enFail;
    if (pUser)
    {
		G_SendErrorInfo(pUser,ERE_ITEM,enFail);
        //pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
    }
}

// 处理道具移动
CT_VOID CCMasterItemEvent::DisposeMoveItem(const CMD_G_MoveItem &mvItem)
{
    CCUser* pUser = 0;    

    if(CCUserManager::getSingleton().Find(mvItem.dwUserID, &pUser))
    {
        ENUM_UseItemFail enFail = (ENUM_UseItemFail)pUser->m_userItem.SwapItemData(mvItem.iSlotFrom, mvItem.iSlotTo);
        if (enFail != EUI_SUCCESS)
        {
            CMD_G_MoveItemFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MOVE_ITEM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

// 处理扩展背包数量
CT_VOID CCMasterItemEvent::DisposeIncBagSlot(const CMD_G_IncBagSlot &bagSlot)
{
    CCUser* pUser = 0;
    ENUM_IncBagSlotFail enFail = EIB_DATA_ERROR;
    if(CCUserManager::getSingleton().Find(bagSlot.dwUserID, &pUser) && bagSlot.iCount == EACH_BAG_SLOT)
    {
        MasterData md = {0};
        pUser->m_userBase.GetMasterData(md);      

        CT_INT32 addSlot = bagSlot.iCount;
        G_ValueRange stRange = {0};
        if (CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_BAGSLOTS, stRange))
        {
            if (md.mvd.stRes.iBagSlots == (CT_INT32)stRange.fMaxVal)
            {
                enFail = EIB_NO_MORE;
                goto FAIL;
            }
            else if (md.mvd.stRes.iBagSlots + addSlot > (CT_INT32)stRange.fMaxVal)
            {
                addSlot = (CT_INT32)stRange.fMaxVal - md.mvd.stRes.iBagSlots;
            }

            {
                // 计算消耗
                G_ValueRange vr = {0};
                G_GetValueRange(VALUE_TYPE_INC_BAG_DIAMOND, &vr);
                CT_INT32 iDiamond = G_CalcBagExpandCost((int)vr.fMinVal, (md.mvd.stRes.iBagSlots - (CT_INT32)stRange.fMinVal) / EACH_BAG_SLOT + 1);;
                if (iDiamond < (CT_INT32)vr.fMaxVal) iDiamond = (CT_INT32)vr.fMaxVal;
                if (iDiamond + md.mvd.stRes.iDiamod < 0) {
                    enFail = EIB_NO_DIAMOND;
                    goto FAIL;
                }

                // to: 内存       
                //pUser->m_userBase.UpdateBagSlot(addSlot);
                //md.mvd.stRes.iBagSlots += addSlot; 
                MasterVariableData mvd = {0};
                mvd.stRes.iBagSlots = addSlot;
                mvd.stRes.iDiamod = iDiamond;
                pUser->m_userBase.AddMastData(mvd, SAVETAG_INC_BAGSLOT);

				//数据记录
				MSC_LOGDB_BagSlot LogBagSlot={0};
				LogBagSlot.dwUserID=pUser->GetUserID();
				LogBagSlot.nSlot=addSlot;
				LogBagSlot.iDiamond=iDiamond;
				LogBagSlot.iUserGrade = mvd.stGrade.iLevel;
                pUser->SaveLogData(MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_BAGSLOTUPDATA, (CT_CHAR*)&LogBagSlot,sizeof(LogBagSlot));

                // to: 客户端
                pUser->m_userBase.GetMasterData(md);
                CMD_G_IncBagSlotSuc stSuc ={0};
                stSuc.dwUserID = pUser->GetUserID();
                stSuc.nSlot = md.mvd.stRes.iBagSlots;
                stSuc.iDiamond = md.mvd.stRes.iDiamod;
                pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_INC_BAGSLOT_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

                //pUser->UpdateAndSendMasterData(EMI_RESOURCE);

				//add by caihan
				//扩充背包，判断背包扩充任务
				CMD_G_TaskEventInfo taskEventInfo = {0};
				taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
				taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_ENLARGE;
				pUser->m_userTask.DisposeTask(taskEventInfo);
				//end
            }
             return;
        }

FAIL:
        CMD_G_IncBagSlotFail stFail ={0};
        stFail.dwUserID = pUser->GetUserID();
        stFail.enFail = enFail;
        pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_INC_BAGSLOT_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
    }
}
// 处理整理背包
CT_VOID CCMasterItemEvent::DisposeSortBag(const CMD_G_SortBag &sortBag)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(sortBag.dwUserID, &pUser))
    {
        pUser->m_userItem.SortItemData();
    }
}

//处理纹章背包
CT_VOID CCMasterItemEvent::DisposeElmBag(const CMD_G_SortBag &elmBag)
{
	CCUser* pUser = 0;
	if (CCUserManager::getSingleton().Find(elmBag.dwUserID, &pUser))
	{
		pUser->m_userElmPower.SortItemData();
	}
}

// 处理装备强化
CT_VOID CCMasterItemEvent::DisposeSmithingEquip(const CMD_G_SmithingEquip &smithingEquip)
{    
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(smithingEquip.dwUserID, &pUser))
    {
        ENUM_SmithingEquipFail enFail = ESE_DATA_ERROR;
        ItemData itData = {0};
        EquipmentData eqData = {0};        
        if (pUser->m_userItem.GetData(smithingEquip.index, itData))
        {
            eqData.key = itData.key;
            enFail = (ENUM_SmithingEquipFail)pUser->m_userEquip.SmithingEquip(itData, eqData);
        }
        if (enFail == ESE_SUCCESS)
        {
            // 强化冷却时间更新
            CooldownData cdData = {0};
            CT_INT32 iCoolTime = G_CalcSmithingCooldown(eqData.iLevel - 1);
			//vip add 强化冷却时间取消
            if (pUser->m_userVip.VipHasPrivilegeType(VIP_PRIVILEGE_CLODDOWN)) {
                iCoolTime = 0;
            }            
            pUser->m_userBase.UpdateCooldown(SMITHINGEQUIP_COOL, iCoolTime);

            // 发送至客户端
            CMD_G_SmithingEquipSuc stSuc = {0};
            stSuc.dwUserID = pUser->GetUserID();
            stSuc.key = eqData.key;
            stSuc.iLevel = eqData.iLevel;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_SMITHING_EQUIP_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

            pUser->m_userDaily.UpdateQuestData(DAILY_SMITHING_EQUIPMENT, 1);

            if (itData.iIndex < pUser->m_userItem.getSlotIdx())
            {
                pUser->UpdateAndSendMasterData();
            }
            else
            {
                pUser->UpdateAndSendMasterData(EMI_RESOURCE);
            }
        }
        else
        {
            CMD_G_SmithingEquipFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_SMITHING_EQUIP_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

// 处理装备洗炼 - 仅消耗属性未更改
CT_VOID CCMasterItemEvent::DisposeForgeEquip(const CMD_G_ForgeEquip &forgeEquip)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(forgeEquip.dwUserID, &pUser))
    {
        ENUM_ForgeEquipFail enFail = EFE_DATA_ERROR;
        ItemData itData = {0};
        EquipmentData eqData = {0};
        if (pUser->m_userItem.GetData(forgeEquip.index, itData))
        {            
            eqData.key = itData.key;
            enFail = (ENUM_ForgeEquipFail)pUser->m_userEquip.ForgeEquip(itData, forgeEquip, eqData);
        }
        if (enFail == EFE_SUCCESS)
        {
            // 发送至客户端
            CMD_G_ForgeEquipSuc stSuc = {0};
            stSuc.dwUserID = pUser->GetUserID();
            stSuc.key = eqData.key;
            memcpy(&stSuc.Forge, &eqData.Forge, sizeof(stSuc.Forge));
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_FORGE_EQUIP_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

            pUser->m_userDaily.UpdateQuestData(DAILY_FORGE_EQUIPMENT, 1);

            pUser->UpdateAndSendMasterData(EMI_RESOURCE);
        }
        else
        {
            CMD_G_ForgeEquipFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_FORGE_EQUIP_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

//add start by Victor 2013-1-8
//处理装备洗练属性替换 - 无消耗，属性更改
CT_VOID CCMasterItemEvent::DisposeForgeEquipConfirm(const CMD_G_ForgeEquipConfirm &forgeEquipConfirm)
{
	CCUser* pUser = 0;
	if(CCUserManager::getSingleton().Find(forgeEquipConfirm.dwUserID, &pUser))
	{
		ENUM_ForgeEquipFail enFail = EFE_DATA_ERROR;
		ItemData itData = {0};
		EquipmentData eqData = {0};
		if (pUser->m_userItem.GetData(forgeEquipConfirm.index, itData))
		{            
			eqData.key = itData.key;
			enFail = (ENUM_ForgeEquipFail)pUser->m_userEquip.ForgeEquipConfirm(itData, eqData, forgeEquipConfirm.confirmFlag);
		}
		if (enFail == EFE_SUCCESS)
		{
			// 发送至客户端
			CMD_G_ForgeEquipSuc stSuc = {0};
			stSuc.dwUserID = pUser->GetUserID();
			stSuc.key = eqData.key;
			memcpy(&stSuc.Forge, &eqData.Forge, sizeof(stSuc.Forge));
			stSuc.confirmFlag = forgeEquipConfirm.confirmFlag;
			pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_FORGE_EQUIP_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

			if (itData.iIndex < pUser->m_userItem.getSlotIdx())
			{
				pUser->UpdateAndSendMasterData(EMI_BATLLE);
			}
		}
		else
		{
			CMD_G_ForgeEquipFail stFail = {0};
			stFail.dwUserID = pUser->GetUserID();
			stFail.enFail = enFail;
			pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_FORGE_EQUIP_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
		}
	}
}
//add end by Victor 2013-1-8

// 处理装备打孔
CT_VOID CCMasterItemEvent::DisposePunchEquip(const CMD_G_PunchEquip &punchEquip)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(punchEquip.dwUserID, &pUser))
    {
        ENUM_PunchEquipFail enFail = EPE_DATA_ERROR;
        ItemData itData = {0};
        EquipmentData eqData = {0};
        if (pUser->m_userItem.GetData(punchEquip.index, itData))
        {            
            eqData.key = itData.key;
            enFail = (ENUM_PunchEquipFail)pUser->m_userEquip.PunchEquip(itData, eqData);
        }
        if (enFail == EPE_SUCCESS)
        {
            // 发送至客户端
            CMD_G_PunchEquipSuc stSuc = {0};
            stSuc.dwUserID = pUser->GetUserID();
            stSuc.key = eqData.key;
            stSuc.nSlot = eqData.nSlot;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_PUNCH_EQUIP_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

            //pUser->m_userDaily.UpdateQuestData(4, 1);
            pUser->UpdateAndSendMasterData(EMI_RESOURCE);
        }
        else
        {
            CMD_G_PunchEquipFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_PUNCH_EQUIP_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

// 处理装备镶嵌
CT_VOID CCMasterItemEvent::DisposeEmbedEquip(const CMD_G_EmbedEquip &embedEquip)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(embedEquip.dwUserID, &pUser))
    {
        ENUM_EmbedEquipFail enFail = EEE_DATA_ERROR;
        ItemData itData = {0};
        EquipmentData eqData = {0};
        if (pUser->m_userItem.GetData(embedEquip.index, itData))
        {            
            eqData.key = itData.key;
            enFail = (ENUM_EmbedEquipFail)pUser->m_userEquip.EmbedEquip(itData, embedEquip, eqData);
        }
        if (enFail == ESE_SUCCESS)
        {
            // 发送至客户端
            CMD_G_EmbedEquipSuc stSuc = {0};
            stSuc.dwUserID = pUser->GetUserID();
            stSuc.key = eqData.key;
            stSuc.iSlot = embedEquip.iSlot;
            stSuc.iGemID = embedEquip.iGemID;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_EMBED_EQUIP_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

			MSC_LOGDB_EmbedEquip logdbEmbedEquip = {0};
			logdbEmbedEquip.dwUserID = pUser->GetUserID();
			logdbEmbedEquip.key = eqData.key;
			logdbEmbedEquip.iGemID = embedEquip.iGemID;
			logdbEmbedEquip.iSlot = embedEquip.iSlot;
			MasterVariableData mvd = {0};
			pUser->m_userBase.GetMasterData(mvd);
			logdbEmbedEquip.iGrade = mvd.stGrade.iLevel;
			CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbEmbedEquip.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_EMBEDEQUIP,(CT_CHAR*)&logdbEmbedEquip,sizeof(MSC_LOGDB_EmbedEquip));

            //pUser->m_userDaily.UpdateQuestData(5, 1);

            if (itData.iIndex < pUser->m_userItem.getSlotIdx())
            {
                pUser->UpdateAndSendMasterData();
            }
        }
        else
        {
            CMD_G_EmbedEquipFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_EMBED_EQUIP_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

// 处理装备继承
CT_VOID CCMasterItemEvent::DisposeInheritEquip(const CMD_G_InheritEquip &inheritEquip)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(inheritEquip.dwUserID, &pUser))
    {
        ENUM_InheritEquipFail enFail = EIE_DATA_ERROR;
        ItemData itDataSrc = {0};
        ItemData itDataDst = {0};
        EquipmentData eqDataSrc = {0};
        EquipmentData eqDataDst = {0};
        if (pUser->m_userItem.GetData(inheritEquip.idxSrc, itDataSrc)
            && pUser->m_userItem.GetData(inheritEquip.idxDst, itDataDst))
        {            
            if (itDataSrc.key != itDataDst.key) {
                eqDataSrc.key = itDataSrc.key;
                eqDataDst.key = itDataDst.key;            
                enFail = (ENUM_InheritEquipFail)pUser->m_userEquip.InheritEquip(itDataSrc, itDataDst, eqDataSrc, eqDataDst);
            } else {
                enFail = EIE_NO_DIFF;
            }
        }
        if (enFail == EIE_SUCCESS)
        {
            // 发送至客户端
            CMD_G_InheritEquipSuc stSuc = {0};
            stSuc.dwUserID = pUser->GetUserID();
            stSuc.keySrc = eqDataSrc.key;
            stSuc.iLvSrc = eqDataSrc.iLevel;
            stSuc.keyDst = eqDataDst.key;
            stSuc.iLvDst = eqDataDst.iLevel;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_INHERIT_EQUIP_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

            if (itDataSrc.iIndex < pUser->m_userItem.getSlotIdx() || itDataDst.iIndex < pUser->m_userItem.getSlotIdx())
            {
                pUser->UpdateAndSendMasterData();
            }
            else
            {
                pUser->UpdateAndSendMasterData(EMI_RESOURCE);
            }
        }
        else
        {
            CMD_G_InheritEquipFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_INHERIT_EQUIP_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}
// 处理出售道具
CT_VOID CCMasterItemEvent::DisposeSellItem(const CMD_G_SellItem &sellItem)
{
    CCUser* pUser = 0;
    ENUM_UseItemFail enFail = EUI_DATA_ERROR;

    if(CCUserManager::getSingleton().Find(sellItem.dwUserID, &pUser))
    {		
        ItemData itData = {0};
        G_ItemBase itBase = {0};
        CCUserItem *pUserItem = NULL;

    	if (sellItem.iCount <= 0) goto FAIL;
        if (sellItem.iSlot < ITEM_SLOT_BAG)
        {
            enFail = EUI_NO_INBAG;
            goto FAIL;
        } else if (sellItem.iSlot > ITEM_SLOT_ELM) {
            pUserItem = &pUser->m_userElmPower;
        } else {
            pUserItem = &pUser->m_userItem;
        }
        pUserItem->GetData(sellItem.iSlot, itData);
        if (CCItemScriptSetting::getSingleton().GetItemBase(itData.iID, itBase))
        {
            if (itBase.iSellPrice < 0) {
                enFail = EUI_NO_SELL;
                goto FAIL;
            }           

            if (pUserItem->AutoExpendItem(SAVETAG_SELL_ITEM, itData.iID, sellItem.iCount, ITEM_OP_MODE_CHECK))
            {
                // 先计算出售价格
                EquipmentData equipData = {0};
                G_Equipment itEquip = {0};                
                CT_INT32 finalPrice = itBase.iSellPrice;
                if (pUserItem == &pUser->m_userItem) {
                    pUser->m_userEquip.GetData(itData.key, equipData);
                    CCItemScriptSetting::getSingleton().GetEquipment(itData.iID, itEquip);
                    if (itBase.iTID == ITEM_TYPE_EQUIPMENT || itBase.iTID == ITEM_TYPE_PET_EQUIPMENT) {
						{
							finalPrice = G_GetItemSell(itBase.iSellPrice, equipData.iLevel, itEquip.fGoldCoefficient1, itEquip.fGoldCoefficient2);
							if(itEquip.iGoods != 0)
							{
								CT_INT32 iCount=abs(itEquip.iCount)*equipData.iLevel;
								if(!pUserItem->AutoAddItemData(SAVETAG_SELL_ITEM,&itEquip.iGoods,&iCount,1,ITEM_OP_MODE_CHECK))
								{
									enFail = EUI_NO_SELL;
									goto FAIL;
								}
								else
									pUserItem->AutoAddItemData(SAVETAG_SELL_ITEM,&itEquip.iGoods,&iCount,1,ITEM_OP_MODE_EXEC);
							}
						}
                    }
                }

                CT_INT32 count = sellItem.iCount;
                if (!pUserItem->ExpendItem(SAVETAG_SELL_ITEM, sellItem.iSlot, count))
                {
                    // 继续出售
                    pUserItem->AutoExpendItem(SAVETAG_SELL_ITEM, itData.iID, count, ITEM_OP_MODE_EXEC);
                }

                MasterVariableData mvd = {0};              
                mvd.stRes.iGold = finalPrice * sellItem.iCount;
                pUser->m_userBase.AddMastData(mvd, SAVETAG_SELL_ITEM);

				//记录日志数据库
				MSC_LOGDB_ItemSell	dbIS = {0};
				dbIS.dwUserID = pUser->GetUserID();
				dbIS.iGlod = finalPrice * sellItem.iCount;
				dbIS.iPropID = itData.iID;
				dbIS.iPropNum = sellItem.iCount;
				CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
				G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ITEMSELL,(CT_CHAR*)&dbIS,sizeof(dbIS));
				
                CMD_G_SellItemSuc stSuc = {0};
                pUser->m_userBase.GetMasterData(mvd);
                stSuc.dwUserID = pUser->GetUserID();
                stSuc.iGold = mvd.stRes.iGold;
                pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_SELL_ITEM_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);
                return;
            }
            else
            {
                enFail = EUI_NO_COUNT;
            }
        }
        else
        {
            enFail = EUI_DIFF_ID;
        }
        
FAIL:
        CMD_G_SellItemFail stFail = {0};
        stFail.dwUserID = pUser->GetUserID();
        stFail.enFail = enFail;
        pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_SELL_ITEM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
    }
}

// 处理消除冷却消息
CT_VOID CCMasterItemEvent::DisposeClearCD(const CMD_G_ClearCD &clearCD)
{
    CCUser* pUser = 0;
    ENUM_ClearCDFail enFail = ECC_SUCCESS;
    if(CCUserManager::getSingleton().Find(clearCD.dwUserID, &pUser)) {
        CooldownData cdData = {0};
        if (pUser->m_userBase.GetCooldownData(clearCD.iType, cdData)) {           
            G_ValueRange vr = {0};
            G_GetValueRange(VALUE_TYPE_PARAM_CLEAR_SMITHING_CD, &vr);
            MasterVariableData mvd = {0}, mvdAdd = {0};
            pUser->m_userBase.GetMasterData(mvd);
            
            // 计算消耗
            mvdAdd.stRes.iDiamod = G_CalcClearCDCost(vr.fMinVal, vr.fMaxVal, (CT_INT32)(cdData.leftTime - (time(0) - cdData.toggleTime)));
            if (mvd.stRes.iDiamod + mvdAdd.stRes.iDiamod < 0) {
                enFail = ECC_NO_DIAMOND;
            }

            if (enFail == ECC_SUCCESS) {
                // 消耗
                pUser->m_userBase.AddMastData(mvdAdd, SAVETAG_CLEAR_CD);
                // 消除
                pUser->m_userBase.UpdateCooldown(clearCD.iType, -cdData.leftTime);
                
                pUser->m_userBase.GetMasterData(mvd);
                CMD_G_ClearCDSuc stSuc = {0};
                stSuc.dwUserID = pUser->GetUserID();
                stSuc.iType = cdData.iType;
                stSuc.iDiamond = mvd.stRes.iDiamod;
                pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_CLEAR_CD_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

			    //记录数据
			    MSC_LOGDB_ClearCD LogdbClearCD={0};
			    LogdbClearCD.dwUserID=pUser->GetUserID();
			    LogdbClearCD.iDiamod=mvd.stRes.iDiamod;
			    LogdbClearCD.iUserGrade = mvd.stGrade.iLevel;

			    CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
			    G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_CLEARCD,(CT_CHAR*)&LogdbClearCD,sizeof(LogdbClearCD));
            } else {
                CMD_G_ClearCDFail stFail = {0};
                stFail.dwUserID = pUser->GetUserID();
                stFail.enFail = enFail;
                pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_CLEAR_CD_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
            }
        }
    }
}


// 处理附魔请求
CT_VOID CCMasterItemEvent::DisposeEnchant(const CMD_G_Enchant &enchant)
{
    CCUser* pUser = 0;

    if(CCUserManager::getSingleton().Find(enchant.dwUserID, &pUser))
    {
        ENUM_EnchantFail enFail = (ENUM_EnchantFail)pUser->m_userEnchant.ActiveEnchant(enchant.iBID, enchant.iIDX, enchant.iItemID);
        if (enFail != EEN_SUCCESS) {
            CMD_G_EnchantFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
			stFail.iBID = enchant.iBID;
			stFail.iIDX = enchant.iIDX;
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}
// 处理附魔奖励
CT_VOID CCMasterItemEvent::DisposeEnchantReward(const CMD_G_EnchantReward &enchantReward)
{
	CCUser* pUser = 0;
	ENUM_UseItemFail itemFail = EUI_SUCCESS;

	if (CCUserManager::getSingleton().Find(enchantReward.dwUserID, &pUser))
	{
		ENUM_EnchantRewardFail enFail = pUser->m_userEnchant.EnchantReward(enchantReward.iIndex, itemFail);
		if (enFail != EER_SUCCESS)
		{
			CMD_G_UseItemFail stFail = {0};
			stFail.dwUserID = pUser->GetUserID();
			stFail.enFail = itemFail;
			if (itemFail == EUI_NO_ELM_SLOT)
			{
				pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
			}else if (itemFail == EUI_NO_SLOT)
			{
				pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
			}
			else
			{
				CMD_G_EnchantReward_Fail err = {0};
				err.dwUserID = pUser->GetUserID();
				err.iIndex = enchantReward.iIndex;
				err.enFail = enFail;
				pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_ENCHANT_REWARD_FAIL, (CT_CHAR *)&err, sizeof(err), false, true);
			}
		}
	}
}
// 元素吞噬
CT_VOID CCMasterItemEvent::DisposeEatElm( const CMD_G_EatElm &eatElm )
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(eatElm.dwUserID, &pUser)) {
        CT_INT32 nCrit = 0;
        ENUM_UseItemFail enFail = (ENUM_UseItemFail)pUser->m_userElmAttr.DoEatElm(eatElm.iIdxSrc, eatElm.iIdxDst, nCrit);
        if (enFail != EUI_SUCCESS) {
            CMD_G_EquipElmFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_EAT_ELM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}
// 吞噬多个元素
CT_VOID CCMasterItemEvent::DisposeEatSomeElm( const CMD_G_EatSomeElm &eatSome )
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(eatSome.dwUserID, &pUser)) {
        ENUM_UseItemFail enFail = (ENUM_UseItemFail)pUser->m_userElmAttr.DoEatSomeElm(eatSome.iIdxSrc, eatSome.arrayDst, ELM_EAT_COUNT);
        if (enFail != EUI_SUCCESS) {
            CMD_G_EquipElmFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_EAT_ELM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}
// 元素一键吞噬
CT_VOID CCMasterItemEvent::DisposeEatAllElm( const CMD_G_EatAllElm &eatAll )
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(eatAll.dwUserID, &pUser)) {
        ENUM_UseItemFail enFail = (ENUM_UseItemFail)pUser->m_userElmAttr.DoEatAllElm(eatAll.arrayLock, ELM_LOCK_COUNT);
        if (enFail != EUI_SUCCESS) {
            CMD_G_EquipElmFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_EAT_ELM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}
// 装备元素(移动)
CT_VOID CCMasterItemEvent::DisposeEquipElm( const CMD_G_EquipElm &equipElm )
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(equipElm.dwUserID, &pUser)) {
        ENUM_UseItemFail enFail = (ENUM_UseItemFail)pUser->m_userBattleFlag.DoEquipElm(equipElm.iType, equipElm.iSlotFrom, equipElm.iSlotTo);
        if (enFail != EUI_SUCCESS) {
            CMD_G_EquipElmFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_EQUIP_ELM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

// 元素背包扩展
CT_VOID CCMasterItemEvent::DisposeIncElmSlot( const CMD_G_IncBagSlot &incSlot )
{
    CCUser* pUser = 0;
    ENUM_IncBagSlotFail enFail = EIB_DATA_ERROR;
    if(CCUserManager::getSingleton().Find(incSlot.dwUserID, &pUser) && incSlot.iCount == EACH_ELM_SLOT)
    {
        MasterData md = {0};
        pUser->m_userBase.GetMasterData(md);      

        CT_INT32 addSlot = incSlot.iCount;
        G_ValueRange stRange = {0};
        if (CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_ELMSLOTS, stRange))
        {
            if (md.mvd.stRes.iElmSlots == (CT_INT32)stRange.fMaxVal)
            {
                enFail = EIB_NO_MORE;
                goto FAIL;
            }
            else if (md.mvd.stRes.iElmSlots + addSlot > (CT_INT32)stRange.fMaxVal)
            {
                addSlot = (CT_INT32)stRange.fMaxVal - md.mvd.stRes.iBagSlots;
            }

            {
                // 计算消耗
                G_ValueRange vr = {0};
                G_GetValueRange(VALUE_TYPE_INC_ELM_DIAMOND, &vr);
                CT_INT32 iDiamond = G_CalcBagExpandCost((int)vr.fMinVal, (md.mvd.stRes.iElmSlots - (CT_INT32)stRange.fMinVal) / EACH_BAG_SLOT + 1);;
                if (iDiamond < (CT_INT32)vr.fMaxVal) iDiamond = (CT_INT32)vr.fMaxVal;
                if (iDiamond + md.mvd.stRes.iDiamod < 0) {
                    enFail = EIB_NO_DIAMOND;
                    goto FAIL;
                }

                // to: 内存                
                MasterVariableData mvd = {0};
                mvd.stRes.iElmSlots = addSlot; 
                mvd.stRes.iDiamod = iDiamond;
                pUser->m_userBase.AddMastData(mvd, SAVETAG_INC_BAGSLOT);
#if 0
                //数据记录
                MSC_LOGDB_BagSlot LogBagSlot={0};
                LogBagSlot.dwUserID=pUser->GetUserID();
                LogBagSlot.nSlot=addSlot;
                LogBagSlot.iDiamond=iDiamond;
                LogBagSlot.iUserGrade = mvd.stGrade.iLevel;
                CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
                G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_BAGSLOTUPDATA,(CT_CHAR*)&LogBagSlot,sizeof(LogBagSlot));
#endif

                // to: 客户端
                pUser->m_userBase.GetMasterData(md); 
                CMD_G_IncBagSlotSuc stSuc ={0};
                stSuc.dwUserID = pUser->GetUserID();
                stSuc.nSlot = md.mvd.stRes.iElmSlots;
                stSuc.iDiamond = md.mvd.stRes.iDiamod;
                pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_INC_ELMSLOT_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

#if 0
                //add by caihan
                //扩充背包，判断背包扩充任务
                CMD_G_TaskEventInfo taskEventInfo = {0};
                taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
                taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_ENLARGE;
                pUser->m_userTask.DisposeTask(taskEventInfo);
                //end
#endif
            }
            return;
        }

FAIL:
        CMD_G_IncBagSlotFail stFail ={0};
        stFail.dwUserID = pUser->GetUserID();
        stFail.enFail = enFail;
        pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_INC_ELMSLOT_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
    }
}

//////////////////////////////////////////////////////////////////////////
// 添加道具
CT_VOID CCMasterItemEvent::DisposeAddItem(const MSC_USR_AddItem &addItem)
{
    CCUser* pUser = 0;

    if(CCUserManager::getSingleton().Find(addItem.dwUserID, &pUser))
    {
        CT_INT32 iCount = addItem.iCount;
        pUser->m_userItem.AutoAddItemData(m_dwThread, addItem.iItemID, iCount);
    }
}

//修改领主名称
CT_VOID CCMasterItemEvent::DisposeChangeMasterName(CT_CHAR *pBuf,CT_INT32 len)
{
	if(len != sizeof(MSC_USR_ChangeMasterName))
	{
		CCLogMeg::getSingleton().WriteError("CCMasterItemEvent::DisposeChangeMasterName len != sizeof(MSC_USR_ChangeMasterName)");
		return;
	}

	MSC_USR_ChangeMasterName *p=(MSC_USR_ChangeMasterName*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(p->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCMasterItemEvent::DisposeChangeMasterName No Find UserID:%d",p->dwUserID);
		return;
	}

	CMD_G_UseItemFail stFail = {0};
	if(p->iStatus != EUI_SUCCESS || strlen(p->cName) == 0)
	{
		stFail.enFail = EUI_NAME_SAME;
	}
	else if(!pUser->m_userItem.AutoExpendItem(SAVATAG_CHANGE_MASTERNAME,p->iPropID,1,ITEM_OP_MODE_CHECK))
	{
		stFail.enFail=EUI_NO_EXIST;
	}

	stFail.dwUserID =pUser->GetUserID();
	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);

	if(stFail.enFail != 0)
	{
		return;
	}

	pUser->m_userItem.AutoExpendItem(SAVATAG_CHANGE_MASTERNAME,p->iPropID,1,ITEM_OP_MODE_CKEXE);
	pUser->m_userBase.ChangeMasterName(p->cName);

	//记录日志
	MSC_LOGDB_UserItem LogUserItem={0};
	LogUserItem.dwUserID=pUser->GetUserID();
	LogUserItem.iItemID=p->iPropID;
	LogUserItem.iItemNum=1;
	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_USESITEM,(CT_CHAR*)&LogUserItem,sizeof(LogUserItem));					


	//add by caihan
	//使用道具，判断道具任务
	CMD_G_TaskEventInfo taskEventInfo = {0};
	taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
	taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_USE;
	taskEventInfo.iSubType = p->iPropID;
	pUser->m_userTask.DisposeTask(taskEventInfo);

	return;
}