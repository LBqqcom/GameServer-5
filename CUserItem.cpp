#include <time.h>
#include "CUserItem.h"
#include "CMD_Game.h"
#include "CUser.h"
#include "CItemScriptSetting.h"
#include "CDatabaseMes.h"
#include "CNetInterface.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "CGlobeFunc.h"
#include "CRand.h"
#include "CUserManager.h"
#include "CBroadcastScriptSetting.h"
#include "CUserSeek.h"

#define CHEST_GOODS_MAX			50		//���俪����Ʒ��Ŀ

CT_VOID CCUserItem::Init(CCUser* pUser, CCMutex* pMutex, CT_INT32 slotIdx)
{
    m_pUser = pUser;
    m_pMutex = pMutex;
	m_DataMap.clear();
    m_SlotIdx = slotIdx;
    m_lastSyncTime = time(0);
}

//�����û�����
CT_VOID CCUserItem::Clear()
{
	_AUTOMUTEX(m_pMutex);
	m_DataMap.clear();
	m_lastSyncTime =0;
}

CT_INT32 CCUserItem::transData(const ItemData &src, CT_CHAR *pDst) const
{
    CMD_G_ItemData *pst = (CMD_G_ItemData *)pDst;
    pst->key = src.key;
    pst->iIndex = src.iIndex;
    pst->iID = src.iID;
    pst->iCount = src.iCount;
    return sizeof(*pst);
}

CT_VOID CCUserItem::SyncData(CT_INT64 currTime, CT_BOOL bForce  /* = CT_FALSE*/)
{
    _AUTOMUTEX(m_pMutex);

    G_ValueRange vr = {0};
    G_GetValueRange(VALUE_TYPE_ITEM_SYNC_THRESHOLDS, &vr);
    CT_INT32 interval = (CT_INT32)vr.fMaxVal;
    if (interval == 0) interval = 600;
    if (bForce || (currTime - m_lastSyncTime) > interval) {
        MSC_DB_SyncItemData stDbData = {0};
        stDbData.dwUserID = m_pUser->GetUserID();
        DataMap_IT it = m_DataMap.begin();
        for (CT_INT32 i = 0; it != m_DataMap.end(); ++it, ++i) 
		{
			if(i < ITEM_SLOT_COUNT)
				stDbData.stItems[i] = it->second;
        }
        if (m_SlotIdx == ITEM_SLOT_BAG) {
            m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ITEM_SYNC, (CT_CHAR*)&stDbData, sizeof(stDbData));
        } else if (m_SlotIdx == ITEM_SLOT_ELM) {
            m_pUser->SaveData(MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_ELM_SYNC, (CT_CHAR*)&stDbData, sizeof(stDbData));
        }
        m_lastSyncTime = currTime;
    }
}

// ���ӵ�������
CT_VOID CCUserItem::AddItemCount(CT_DWORD saveTag, DataMap_IT it, CT_INT32 addValue, CT_BOOL bSendData /*= CT_TRUE*/)
{
    _AUTOMUTEX(m_pMutex);

    it->second.iCount += addValue;    
    if (it->second.iCount < 0) {
        it->second.iCount = 0;
    }

	MSC_LOGDB_AddItem logdbAddItem = {0};
	logdbAddItem.dwUserID = m_pUser->GetUserID();
    logdbAddItem.saveTag = saveTag;
	logdbAddItem.iItemID = it->second.iID;
	logdbAddItem.iItemNum = addValue;
	CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbAddItem.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ADDSITEM,(CT_CHAR*)&logdbAddItem,sizeof(MSC_LOGDB_AddItem));

	//add by caihan
	//��Ʒ���ģ�������Ʒ��������,��Ʒ���Ӳ�������ӿ�
	if (addValue<0) {
		CMD_G_TaskEventInfo taskEventInfo = {0};
		taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_OWN;
		taskEventInfo.iSubType = it->second.iID;
		m_pUser->m_userTask.DisposeTask(taskEventInfo,addValue);
		taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_SUBMIT;
		m_pUser->m_userTask.DisposeTask(taskEventInfo,addValue);
	}
	//end

    if (bSendData) {
        SendItemData(it->second);
        if (it->second.iCount == 0) {
            if (m_SlotIdx == ITEM_SLOT_BAG) {
                m_pUser->m_userEquip.RemoveData(it->second.key);
            } else if (m_SlotIdx == ITEM_SLOT_ELM) {
                m_pUser->m_userElmAttr.RemoveData(it->second.key);
            }
            m_DataMap.erase(it);
        }
    }
}
// ��������λ��
CT_VOID CCUserItem::SwapItem(DataMap_IT itSrc, DataMap_IT itDst, CT_BOOL bSendData)
{
    _AUTOMUTEX(m_pMutex);

    ItemData data = itSrc->second;    
    itSrc->second = itDst->second;
    itDst->second = data;

    itSrc->second.iIndex += itDst->second.iIndex;
    itDst->second.iIndex = itSrc->second.iIndex - itDst->second.iIndex;
    itSrc->second.iIndex = itSrc->second.iIndex - itDst->second.iIndex;
    
    if (bSendData) {
        CMD_G_MoveItemSuc stSuc = {0};
        stSuc.dwUserID = m_pUser->GetUserID();
        stSuc.iIdxSrc = itSrc->second.iIndex;
        stSuc.iIdxDst = itDst->second.iIndex;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MOVE_ITEM_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

        if (stSuc.iIdxSrc < ITEM_SLOT_BAG || stSuc.iIdxDst < ITEM_SLOT_BAG) {
            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
        }
    }
}
// �ƶ�����λ��
CT_VOID CCUserItem::MoveItem(DataMap_IT it, CT_INT32 slotTo, CT_BOOL bSendData)
{
    _AUTOMUTEX(m_pMutex);

    ItemData data = it->second;
    CT_INT32 slotFrom = data.iIndex;
    data.iIndex = slotTo;
    m_DataMap.insert(DataMap::value_type(data.iIndex, data));
    m_DataMap.erase(it);

    if (bSendData) {
        CMD_G_MoveItemSuc stSuc = {0};
        stSuc.dwUserID = m_pUser->GetUserID();
        stSuc.iIdxSrc = slotFrom;
        stSuc.iIdxDst = slotTo;
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MOVE_ITEM_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);

        if (stSuc.iIdxSrc < ITEM_SLOT_BAG || stSuc.iIdxDst < ITEM_SLOT_BAG) {
            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
        }
    }
}

// ���������Ϣ
CT_VOID CCUserItem::TrimItem(void)
{
    DataMap_IT itNext = m_DataMap.begin();
    while (itNext != m_DataMap.end())
    {
        DataMap_IT it = itNext;
        ++itNext;
        if (it->second.iCount == 0 || it->second.iID == 0)
        {
            m_DataMap.erase(it);
        }
    }
}
// ���ɵ���ΨһKey
CT_UINT32 CCUserItem::GenerateKey(void)
{
    CT_DWORD key = 0;
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it) {
        if (it->second.key > key) {
            key = it->second.key;
        }
    }
    return key + 1;
}

// ���㵱ǰ��������ĳ�ֵ�����Ҫ���ٸ��ո���
CT_INT32 CCUserItem::CalcItemSlot(CT_INT32 iItemID, CT_INT32 iCount)
{
    DataMap_IT its[ITEM_SLOT_COUNT];
    CT_INT32 num = ITEM_SLOT_COUNT;
    CT_INT32 nCostSlot = 0;
    G_ItemBase itBase = {0};

    if (CCItemScriptSetting::getSingleton().GetItemBase(iItemID, itBase)) {
        if (SearchItemData(iItemID, its, num) > 0) {
            for (int i = 0; i < num && iCount > 0; ++i) {
                ItemData data = its[i]->second;
                iCount -= itBase.iStackNum - data.iCount;
            }
        }
        if (iCount > 0) {
            nCostSlot = iCount / itBase.iStackNum;
            if (iCount % itBase.iStackNum > 0) {
                nCostSlot += 1;
            }
        }
    }
    return nCostSlot;
}

// �õ���װ�������б�
CT_BOOL CCUserItem::GetEquipedItemData(CT_CHAR *pBuf, CT_INT32 &bufLen)
{
    _AUTOMUTEX(m_pMutex);

    CT_INT32 len = EQUIPED_ITEM_COUNT * sizeof(CMD_G_ItemData);
    if (len > bufLen)
    {
        return CT_FALSE;
    }

    DataMap_IT it = m_DataMap.begin();
    int index = 0;
    bufLen = 0;
    for (int i = 0; it != m_DataMap.end(); ++it, ++i)
    {
        if ((*it).second.iIndex > ITEM_SLOT_EQUIPMENT && (*it).second.iIndex < (ITEM_SLOT_EQUIPMENT + EQUIPED_ITEM_COUNT + 1)) // ��װ����
        {
            CMD_G_ItemData itemData = {0};
            itemData.key = (*it).second.key;
            itemData.iID = (*it).second.iID;
            itemData.iCount = (*it).second.iCount;
            itemData.iIndex = (*it).second.iIndex;
            memcpy(pBuf + sizeof(CMD_G_ItemData) * (index++), &itemData, sizeof(itemData));
            bufLen += sizeof(CMD_G_ItemData);
        }
    }

    return CT_TRUE;
}

// ��ÿղ�����
CT_INT32 CCUserItem::CalcEmptySlot(CT_INT32 slotType /* = 0 */)
{
    _AUTOMUTEX(m_pMutex);
    
    CT_INT32 maxIdx = 0;
    CT_INT32 slots = 0;

    if (slotType == 0) slotType = m_SlotIdx;
    switch (slotType)
    {
    case ITEM_SLOT_BAG:
    case ITEM_SLOT_ELM:        
        maxIdx = slotType + 1 + m_pUser->m_userBase.GetSlotCount(slotType);
		slotType += 1;
        break;
    case ITEM_SLOT_CLOTH:
    case ITEM_SLOT_MEDAL:
    case ITEM_SLOT_NECK:
    case ITEM_SLOT_RING:
    case ITEM_SLOT_WEAPON:
    case ITEM_SLOT_SHOE:
        maxIdx = slotType + 1;
        break;
    default:
        break;
    }

    for (int i = slotType; i < maxIdx; ++i)
    {
        ItemData data = {0};
        if (!GetData(i, data))
        {
            slots++;
        }
    }
    return slots;
}

// ���ҿյĲ�
CT_INT32 CCUserItem::SearchSlot(CT_INT32 slotType)
{
    _AUTOMUTEX(m_pMutex);

    CT_INT32 maxIdx = 0;
    if (slotType == 0) slotType = m_SlotIdx;
    switch (slotType)
    {
    case ITEM_SLOT_BAG:
    case ITEM_SLOT_ELM:        
        maxIdx = slotType + 1 + m_pUser->m_userBase.GetSlotCount(slotType);
		slotType += 1;
        break;
    case ITEM_SLOT_CLOTH:
    case ITEM_SLOT_MEDAL:
    case ITEM_SLOT_NECK:
    case ITEM_SLOT_RING:
    case ITEM_SLOT_WEAPON:
    case ITEM_SLOT_SHOE:
        maxIdx = slotType + 1;
        break;
    default:
        break;
    }

    for (int i = slotType; i < maxIdx; ++i)
    {
        ItemData data = {0};
        if (!GetData(i, data))
        {
            return i;
        }
    }
    return -1;
}

// ���ҵ�������
CT_BOOL CCUserItem::GetClientData(CT_INT32 index, CMD_G_ItemData &data) 
{
    ItemData itData = {0};
    CT_BOOL ret = GetData(index, itData);
    memset(&data, 0, sizeof(data));
    if (ret) 
    {
        transData(itData, (CT_CHAR *)&data);
    }
    else
    {
        data.iIndex = index;
    }
    return ret;
}

CT_INT32 CCUserItem::SearchItemData(CT_INT32 iID, DataMap_IT its[], CT_INT32 &slotCount)
{
    _AUTOMUTEX(m_pMutex);
    CT_INT32 num = 0;
    CT_INT32 itemCount = 0;
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        if ((*it).second.iID == iID)
        {
            if (num < slotCount)
            {
                its[num] = it;
                num++;
                itemCount += it->second.iCount;
            }
        }
    }
    slotCount = num;
    return itemCount;
}
CT_BOOL CCUserItem::SearchItemData(const EquipmentData &equip, ItemData &data)          // ĳ��װ������Ϣ
{
    _AUTOMUTEX(m_pMutex);
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        if (it->second.key == equip.key)
        {
            data = it->second;
            return CT_TRUE;
        }
    }
    return CT_FALSE;
}

// ����ĳ�ֵ��ߵ�������
CT_INT32 CCUserItem::CalcItemCount(CT_INT32 iItemID)
{
    _AUTOMUTEX(m_pMutex);
    CT_INT32 num = 0;
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        if ((*it).second.iID == iItemID)
        {
            num += it->second.iCount;
        }
    }
    return num;
}

// ����һ��������ĵ���
CT_BOOL CCUserItem::ExpendItem(const CT_DWORD saveTag, const CT_INT32 iSlot, CT_INT32 &Count, ITEM_OP_MODE enMode)
{
    _AUTOMUTEX(m_pMutex);
    DataMap_IT itData = m_DataMap.find(iSlot);
    CT_BOOL bRet = CT_FALSE;

    if (itData != m_DataMap.end())
    {
        ItemData data = itData->second;
        CT_INT32 count = data.iCount;
        if (data.iCount < Count)
        {
            data.iCount = -data.iCount;
            Count += data.iCount;
        }
        else
        {
            data.iCount = -Count;
            Count = 0;
            bRet = CT_TRUE;
        }
        if (enMode & ITEM_OP_MODE_EXEC)
        {
            AddItemCount(saveTag, itData, data.iCount);
        }
    }
    return bRet;
}

// ����һ�ֵ���
CT_BOOL CCUserItem::AutoExpendItem(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 Count, ITEM_OP_MODE enMode)
{
	_AUTOMUTEX(m_pMutex);
    DataMap_IT its[ITEM_SLOT_COUNT];
    CT_INT32 num = ITEM_SLOT_COUNT;
    
    if (enMode == ITEM_OP_MODE_CHECK)
    {
        // ���������
        if (CalcItemCount(iItemID) < Count) return CT_FALSE;
    }
    else
    {
        if (SearchItemData(iItemID, its, num) < Count) return CT_FALSE;
    }

    if (enMode & ITEM_OP_MODE_EXEC)
    {
        // ���ĵ���
        for (int i = 0; i < num && Count > 0; ++i)
        {	
            ItemData data = its[i]->second;
            if (Count > data.iCount)
            {
                Count -= data.iCount;
                data.iCount = -data.iCount;
            }
            else
            {
                data.iCount = -Count;
                Count = 0;
            }
            AddItemCount(saveTag, its[i], data.iCount);

            if (Count == 0)
            {
                break;
            }
        }
    }
    return CT_TRUE;
}

// ���Ķ��ֵ���
CT_BOOL CCUserItem::AutoExpendItem(const CT_DWORD dwThread, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 itemTypes, ITEM_OP_MODE enMode)
{
    CT_BOOL bRet = CT_TRUE;
    for (int i = 0; i < itemTypes; ++i)
    {
        if (!AutoExpendItem(dwThread, iItemIDs[i], Counts[i], enMode))
        {
            bRet = CT_FALSE;
            break;
        }
    }
    return bRet;
}

// ���һ�ֵ���
CT_INT32 CCUserItem::AutoAddItemData(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 &iCount, ITEM_OP_MODE enMode)
{
    CT_INT32 iSlot = 0;
    return AddItemData(saveTag, iItemID, iCount, iSlot, enMode);
}

CT_INT32 CCUserItem::AddItemData(const CT_DWORD saveTag, const CT_INT32 iItemID, CT_INT32 &iCount, CT_INT32 &iSlot, ITEM_OP_MODE enMode)
{
    _AUTOMUTEX(m_pMutex);
	CT_INT32 iOCount = iCount;
        
    //ItemDataMap newDataList;   // ��ʱ����б�
    G_ItemBase itBase = {0};    // ��������
    ItemData itemData = {0};    // Ҫ��ӵĵ���
    itemData.iID = iItemID;
    CT_INT32 nLeft = iCount;    // ʣ������
    iSlot = 0;

    if (CCItemScriptSetting::getSingleton().GetItemBase(iItemID, itBase))
    {
        memcpy(itemData.szName, itBase.szName, sizeof(itemData.szName));       
        DataMap_IT its[ITEM_SLOT_COUNT];        
        CT_INT32 num = ITEM_SLOT_COUNT;         // ���еĸ���Ʒռ�еĸ�����
        
        // ���ռ�
        CT_INT32 count = 0;
        CT_INT32 nEmpty = CalcEmptySlot(m_SlotIdx);
        if (SearchItemData(iItemID, its, num)) {            
            if (enMode & ITEM_OP_MODE_CHECK) {                               
                for (int i = 0; i < num; ++i) {
                    ItemData data = its[i]->second;
                    count += itBase.iStackNum - data.iCount;
                }
                count += itBase.iStackNum * nEmpty;                
            } else {
                count = iCount;
            }
        } else {
            count = nEmpty * itBase.iStackNum;
        }
        if (count < iCount) return iCount - count;
       
        if (enMode & ITEM_OP_MODE_EXEC) {
            itemData.iCount = iCount;
            iCount = 0;     // ��ʼ��¼�õ��ߵ���������
            // ��ʼ�ѵ�
            for (int i = 0; i < num && itemData.iCount > 0; ++i) {                
                ItemData data = its[i]->second;
                CT_INT32 nLeftSlot = itBase.iStackNum - data.iCount;
                iCount += data.iCount;
                if (nLeftSlot > 0) {
                    if (nLeftSlot < itemData.iCount) {
                        // ʣ��ѵ�λ�ò���                            
                        data.iCount = nLeftSlot;
                        itemData.iCount -= data.iCount;
                    } else {
                        data.iCount = itemData.iCount;
                        itemData.iCount = 0;
                    }
                    //newDataList.insert(ItemDataMap::value_type(data.iIndex, data));
                    iCount += data.iCount;
                    AddItemCount(saveTag, its[i], data.iCount);
                    iSlot = its[i]->second.iIndex;
                }                
            }

            nLeft = itemData.iCount;
            while (nLeft > 0) {
                // �����ڿղ�
                itemData.iIndex = SearchSlot(m_SlotIdx);                
                if (itBase.iStackNum < nLeft) {                    
                    itemData.iCount = itBase.iStackNum;
                    nLeft -= itBase.iStackNum;
                } else {
                    itemData.iCount = nLeft;
                    nLeft = 0;
                }
                itemData.key = GenerateKey(); // �����µ�keyֵ�����²۵ĵ���
                InsertData(itemData.iIndex, itemData);
                iSlot = itemData.iIndex;
				MSC_LOGDB_AddItem logdbAddItem = {0};
				logdbAddItem.dwUserID = m_pUser->GetUserID();
                logdbAddItem.saveTag = saveTag;
				logdbAddItem.iItemID = itemData.iID;
				logdbAddItem.iItemNum = itemData.iCount;
				CT_DWORD logThreadid = GAME_THREAD_LOGDATABASE + logdbAddItem.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
				G_SendThreadInfo(logThreadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_ADDSITEM,(CT_CHAR*)&logdbAddItem,sizeof(MSC_LOGDB_AddItem));
                SendItemData(itemData);
                // �����Ԫ�ؾ������������������
                if (itBase.iTID == ITEM_TYPE_ELMMARROW) {
                    m_pUser->m_userElmAttr.GenerateAttr(itemData);
                }
                iCount += itemData.iCount;
            }
        } else {
			nLeft -= count;
			if (nLeft < 0) nLeft = 0;
		}
    }

	//add by caihan
	if (enMode!=ITEM_OP_MODE_CHECK)
	{
		//���߻�ã����ӶԵ���������ж�,�ύ��������Ҳ�ж�
		//nleftΪ0������ȫ����ȡ�ɹ����������0�����enmode�ж�
		if (nLeft==0)
		{
			CMD_G_TaskEventInfo taskEventInfo = {0};
			taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_OWN;
			taskEventInfo.iSubType = iItemID;
			m_pUser->m_userTask.DisposeTask(taskEventInfo,iOCount);
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_SUBMIT;
			m_pUser->m_userTask.DisposeTask(taskEventInfo,iOCount);
		}
		else if (nLeft>0&&enMode==ITEM_OP_MODE_EXEC)
		{
			CMD_G_TaskEventInfo taskEventInfo = {0};
			taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_ITEM;
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_OWN;
			taskEventInfo.iSubType = iItemID;
			m_pUser->m_userTask.DisposeTask(taskEventInfo,iOCount);
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_ITEM_SUBMIT;
			m_pUser->m_userTask.DisposeTask(taskEventInfo,iOCount-nLeft);
		}

		if (itBase.iTID == ITEM_TYPE_ELMMARROW)
		{
			//���Ԫ�سɹ����жϻ��Ԫ������
			G_ItemBase elmBase = {0};
			G_ElmMarrow elmGet = {0};
			ElmMarrowAttr elmAttr = {0};
			G_GetItemBase(itemData.iID, &elmBase);
			G_GetElmMarrow(itemData.iID,elmGet);
			m_pUser->m_userElmAttr.GetData(itemData.key,elmAttr);
			CMD_G_TaskSystemInfo taskSysInfo = {0};
			taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
			taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_GETELM;
			taskSysInfo.arrayParam[0] = elmGet.iType;            //����0������
			taskSysInfo.arrayParam[1] = elmBase.iQuality;        //����1��Ʒ��
			taskSysInfo.arrayParam[2] = elmAttr.iLevel;          //����2���ȼ�
			taskSysInfo.iProgress = iOCount - nLeft;
			m_pUser->m_userTask.DisposeTask(taskSysInfo);
		}
		/*else if (itBase.iTID == ITEM_TYPE_ELMRUBBLE)
		{
			CMD_G_TaskSystemInfo taskSysInfo = {0};
			taskSysInfo.iProgress = iOCount - nLeft;
			m_pUser->m_userTask.DisposeTask(taskSysInfo);
		}*/
	}
	//end
    
    return nLeft;
}

// ��Ӷ��ֵ���
CT_BOOL CCUserItem::AutoAddItemData(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 itemTypes, ITEM_OP_MODE enMode)
{
    return AddItemData(saveTag, iItemIDs, Counts, NULL, itemTypes, enMode);
}
CT_BOOL CCUserItem::AddItemData(const CT_DWORD saveTag, const CT_INT32 iItemIDs[], CT_INT32 Counts[], CT_INT32 Slots[], CT_INT32 itemTypes, ITEM_OP_MODE enMode)
{
    CT_BOOL bRet = CT_TRUE;
    if (enMode & ITEM_OP_MODE_CHECK) {
        CT_INT32 nNeedSlots = 0;
        for (int i = 0; i < itemTypes; ++i) {
            nNeedSlots += CalcItemSlot(iItemIDs[i], Counts[i]);
        }
        if (nNeedSlots > CalcEmptySlot(m_SlotIdx)) {
            return CT_FALSE;
        }
    }
    if (enMode & ITEM_OP_MODE_EXEC) {
        for (int i = 0; i < itemTypes; ++i) {
            CT_INT32 slot = 0;
            CT_INT32 &iSlot = Slots ? Slots[i] : slot;
            AddItemData(saveTag, iItemIDs[i], Counts[i], iSlot, ITEM_OP_MODE_EXEC);
        }
    }
    return CT_TRUE;
}

// �����������ߵ�λ��
CT_INT32 CCUserItem::SwapItemData(const CT_INT32 iSrcSlot, const CT_INT32 iDstSlot)
{
    _AUTOMUTEX(m_pMutex);

    struct MoveItemInfo 
    {
        DataMap_IT itData;
        G_ItemBase itBase;
        G_Equipment itEquip;
    };
	MoveItemInfo mvSrc, mvDst;

	memset(&mvSrc.itBase, 0, sizeof(mvSrc.itBase));
	memset(&mvSrc.itEquip, 0, sizeof(mvSrc.itEquip));
	memset(&mvDst.itBase, 0, sizeof(mvDst.itBase));	
	memset(&mvDst.itEquip, 0, sizeof(mvDst.itEquip));
    if (!CheckSlotValid(iSrcSlot) || !CheckSlotValid(iDstSlot))
    {
        return EUI_DATA_ERROR;
    }

    if ((mvSrc.itData = m_DataMap.find(iSrcSlot)) != m_DataMap.end())
    {
        CCItemScriptSetting::getSingleton().GetItemBase(mvSrc.itData->second.iID, mvSrc.itBase);
        if (mvSrc.itBase.iTID == ITEM_TYPE_EQUIPMENT)
        {
            CCItemScriptSetting::getSingleton().GetEquipment(mvSrc.itBase.iID, mvSrc.itEquip);
        }
    } else 
    {
        CCLogMeg::getSingleton().WriteError("Wrong Operate %d", iSrcSlot);
        return EUI_DATA_ERROR;
    }
    if ((mvDst.itData = m_DataMap.find(iDstSlot)) != m_DataMap.end())
    {
        CCItemScriptSetting::getSingleton().GetItemBase(mvDst.itData->second.iID, mvDst.itBase);
        if (mvDst.itBase.iTID == ITEM_TYPE_EQUIPMENT)
        {
            CCItemScriptSetting::getSingleton().GetEquipment(mvDst.itBase.iID, mvDst.itEquip);
        }
    }

    MoveItemInfo *pToEquip = NULL;
    CT_BOOL bUnEquip = CT_FALSE;
    if (iDstSlot < m_SlotIdx)
    {
        if (mvSrc.itData == m_DataMap.end() || mvSrc.itData->second.iID == 0)
        {
            // Դ���ǿյģ������ݴ���
            return EUI_DATA_ERROR;
        }
        if (mvSrc.itEquip.iEID == iDstSlot)
        {
            // Ŀ�����װ���ۣ�����װ��Դ����Ʒ
            pToEquip = &mvSrc;
        }
        else
        {
            // Դ����Ʒ�����Ͳ��ԣ��޷�װ��
            return EUI_NO_TYPE;
        }
    }
    else if (iSrcSlot < m_SlotIdx)
    {
        if (mvDst.itData == m_DataMap.end() || mvDst.itData->second.iID == 0)
        {
            // Դ����װ������Ŀ����ǿյģ���ʶж��װ��
            bUnEquip = CT_TRUE;
        }            
        else if (mvDst.itEquip.iEID == iSrcSlot)
        {
            // Դ����װ������Ŀ�����������ƥ��ĵ��ߣ�����װ��Ŀ�����Ʒ
            pToEquip = &mvDst;               
        }
        else 
        {
            // Ŀ�����Ʒ�����Ͳ��ԣ��޷�װ��
            return EUI_NO_TYPE;
        }
    }

    if (pToEquip != NULL)
    {
        if (CheckItemUsable(pToEquip->itBase))
        {
            return UseEquipment(pToEquip->itData->second);
        }
        else
        {
            return EUI_CANNOT_USE;
        }
    }
    else
    {
        if (mvSrc.itData->second.iID == 0)
        {
            return EUI_DATA_ERROR;
        }
        else
        {
            if (mvSrc.itBase.iID == mvDst.itBase.iID 
                && mvSrc.itData->second.iCount < mvSrc.itBase.iStackNum
                && mvDst.itData->second.iCount < mvDst.itBase.iStackNum)
            {
                // �����ͬһID����Ʒ�����е���
                StackItem(mvSrc.itData, mvDst.itData);
            }
            else
            {
                // ����λ��                
                if (mvDst.itData == m_DataMap.end())
                {              
                    MoveItem(mvSrc.itData, iDstSlot);
                }
                else
                {                    
                    SwapItem(mvSrc.itData, mvDst.itData);
                }
            }
        }
    }

    return EUI_SUCCESS;
}

// ���ߵ���
CT_VOID CCUserItem::StackItem(DataMap_IT itSrc, DataMap_IT itDst, CT_BOOL bSendData)
{
    _AUTOMUTEX(m_pMutex);

    G_ItemBase itBase = {0};
    CT_INT32 nSrc = itSrc->second.iCount;
    CT_INT32 nDst = itDst->second.iCount;
    CT_INT32 iTotal = nSrc + nDst;

    G_GetItemBase(itSrc->second.iID, &itBase);

    if (itSrc->second.iID != itDst->second.iID) return;
    if (nDst == itBase.iStackNum) return;    
    
    if (iTotal > itBase.iStackNum) {
        AddItemCount(0, itDst, itBase.iStackNum - nDst, bSendData);
        AddItemCount(0, itSrc, iTotal - itDst->second.iCount - nSrc, bSendData);
    } else {
        AddItemCount(0, itDst, iTotal - nDst, bSendData);
        AddItemCount(0, itSrc, -nSrc, bSendData);
    }

    /** Example:
     * status   dst     src
     * --------------------
     * before   5       19
     * after    20      4
     */
}

CT_BOOL CCUserItem::IsItemSwap(const DataMap_IT &it, const DataMap_IT &itNext)
{
	// ��������ǰ
	// IDС������ǰ
	// ID��ͬ��������ķ���ǰ
	return (it == m_DataMap.end() || (*it).second.iID > (*itNext).second.iID ||
		((*it).second.iID == (*itNext).second.iID && (*it).second.iCount < (*itNext).second.iCount));
}

CT_BOOL CCUserItem::IsElmSwap(const DataMap_IT &it, const DataMap_IT &itNext)
{
	if (it == m_DataMap.end())
	{
		return CT_TRUE;
	}
	G_ItemBase elmIt = {0};
	G_ItemBase elmItnext = {0};
	G_GetItemBase((*it).second.iID, &elmIt);
	G_GetItemBase((*itNext).second.iID, &elmItnext);
	ElmMarrowAttr ema = {0};
	ElmMarrowAttr emaNext = {0};
	m_pUser->m_userElmAttr.GetData((it->second).key, ema);
	m_pUser->m_userElmAttr.GetData((itNext->second).key, emaNext);

	// �������ڲ���ǰ
	// ͬ�����ͣ�ID�����ǰ
	// ͬ��ID����Ʒ�ʵ���ǰ
	// ͬ��Ʒ�ʣ��ȼ��ߵ���ǰ
    // ͬ�ֵȼ�����ֵ�����ǰ
	if ((elmIt.iTID == ITEM_TYPE_ELMRUBBLE && elmItnext.iTID == ITEM_TYPE_ELMMARROW) ||
		(elmIt.iTID == elmItnext.iTID && 
		((*it).second.iID < (*itNext).second.iID ||
		((*it).second.iID == (*itNext).second.iID && 
		(elmIt.iQuality < elmItnext.iQuality ||
		(elmIt.iQuality == elmItnext.iQuality && ema.iLevel < emaNext.iLevel))))))
	{
		return CT_TRUE;
	}
	else if (elmIt.iTID == elmItnext.iTID && (*it).second.iID == (*itNext).second.iID && elmIt.iQuality == elmItnext.iQuality && ema.iLevel == emaNext.iLevel)
	{
		for (CT_INT32 i = 0;i < sizeof(ema.attrs) / sizeof(ema.attrs[0]);++i)
		{
			if (ema.attrs[i] < emaNext.attrs[i])
			{
				return CT_TRUE;
			}
		}
	}
	return CT_FALSE;
}

// ��������
CT_VOID CCUserItem::SortItemData()
{
    _AUTOMUTEX(m_pMutex);

    // �ȵ���
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it) {        
        DataMap_IT itNext = it;
        itNext++;
        for (; itNext != m_DataMap.end(); ++itNext) {
            if (it->first > m_SlotIdx && itNext->first > m_SlotIdx) {
                ItemData &itDst = (*it).second;
                ItemData &itSrc = (*itNext).second;

                StackItem(itNext, it, CT_FALSE);
            }
        }
    }

    // ����
    TrimItem();

    // ������
    CT_INT32 nEntries = m_DataMap.size();
    for (CT_INT32 i = m_SlotIdx; i < m_SlotIdx + nEntries; ++i)
    {
        DataMap_IT it = m_DataMap.find(i + 1);
        DataMap_IT itCurr = it;

		if (m_SlotIdx == ITEM_SLOT_BAG)
		{
			for (DataMap_IT itNext = m_DataMap.begin(); itNext != m_DataMap.end(); ++itNext)
			{
				// �ų�װ����/�ų����źõ�
				if ((*itNext).first < m_SlotIdx || (*itNext).second.iIndex <= (CT_INT32)(i + 1)) continue;

				if (IsItemSwap(it,itNext))
				{
					it = itNext;
				}
			}	
		}else if (m_SlotIdx == ITEM_SLOT_ELM)
		{
			for (DataMap_IT itNext = m_DataMap.begin(); itNext != m_DataMap.end(); ++itNext)
			{
				//�ų�װ�������ų����źõ�
				if ((*itNext).first > ITEM_SLOT_BATTLEFLAG || (*itNext).second.iIndex <= (i + 1)) continue;

				if (IsElmSwap(it,itNext))
				{
					it = itNext;
				}
			}	
		}  
        
        if (it != m_DataMap.end()) {
            if (itCurr == m_DataMap.end() || itCurr->second.iCount == 0) {
                // ��ǰλ���Ǹ��ո��� �Ѻ���������������Ƶ���ǰλ��
                MoveItem(it, i + 1, CT_FALSE);
            } else if (it != itCurr) {
                // ��ǰλ�ò�Ϊ�գ��Ѻ�������������ĺ͵�ǰ���滻
                SwapItem(it, itCurr, CT_FALSE);
            }
        }
    }

    // ֪ͨ�ͻ���
    CT_INT32 len = m_DataMap.size() * sizeof(CMD_G_ItemData);
    CT_CHAR *buff = new CT_CHAR[len];
    GetAllData(buff, len);
	if (m_SlotIdx == ITEM_SLOT_BAG)
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_SORT_BAG_SUC, buff, len, false, true);
	}
	if (m_SlotIdx == ITEM_SLOT_ELM)
	{
		m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_ELMPOWER_INFO, buff, len, false, true);
	}
    delete [] buff;
}

//���俪��
CT_VOID CCUserItem::OpenChest(const CT_INT32 iID,CT_INT32 iGoods[],CT_INT32 iCount[],CT_INT32 &iNum,ResAmount Chest[],FakeProBase &fb)
{
	CT_INT32 iGoods1[GOODS_COUNT*2]={0};
	CT_INT32 iCounts1[GOODS_COUNT*2]={0};
	G_ItemChest stItem = {0};    
	//MasterVariableData mvd={0};
	//m_pUser->m_userBase.GetMasterData(mvd);
	if (CCItemScriptSetting::getSingleton().GetItemChest(iID, stItem))
	{
		//��Դ�ۼ�
		Chest[iNum*2].iID+=stItem.stRes[0].iID;
		Chest[iNum*2].iCount+=stItem.stRes[0].iCount;
		Chest[iNum*2+1].iID+=stItem.stRes[1].iID;
		Chest[iNum*2+1].iCount+=stItem.stRes[1].iCount;

		//�Զ�����
		if(stItem.iType != 0)
		{
			if(stItem.iGoods1 != 0)G_GetGoodsGroupInfo(stItem.iGoods1,iGoods1,iCounts1,fb);
			if(stItem.iGoods2 != 0)G_GetGoodsGroupInfo(stItem.iGoods2,iGoods1+GOODS_COUNT,iCounts1+GOODS_COUNT,fb);
			for(CT_INT32 i=0;i<GOODS_COUNT*2&&iNum <CHEST_GOODS_MAX;++i)
			{
				if(iGoods1[i] == 0 || iCounts1[i] == 0)continue;
				G_ItemChest stItem = {0};    
				if (CCItemScriptSetting::getSingleton().GetItemChest(iGoods1[i], stItem))
				{
					if(stItem.iType != 0)
					{
						OpenChest(iGoods1[i],iGoods,iCount,iNum,Chest,fb);
					}
					else
					{
						iGoods[iNum]=iGoods1[i];
						iCount[iNum]=iCounts1[i];
						iNum++;
					}
				}
				else
				{
					iGoods[iNum]=iGoods1[i];
					iCount[iNum]=iCounts1[i];
					iNum++;
				}
			}
		}
		else
		{
			if(stItem.iGoods1 != 0)G_GetGoodsGroupInfo(stItem.iGoods1,iGoods1,iCounts1,fb);
			if(stItem.iGoods2 != 0)G_GetGoodsGroupInfo(stItem.iGoods2,iGoods1+GOODS_COUNT,iCounts1+GOODS_COUNT,fb);
			for(CT_INT32 i=0;i<GOODS_COUNT*2&&iNum <CHEST_GOODS_MAX;++i)
			{
				if(iGoods1[i] == 0 || iCounts1[i] == 0)continue;
				G_ItemChest stItem = {0};    
				if (CCItemScriptSetting::getSingleton().GetItemChest(iGoods1[i], stItem))
				{
					if(stItem.iType != 0)
					{
						OpenChest(iGoods1[i],iGoods,iCount,iNum,Chest,fb);
					}
					else
					{
						iGoods[iNum]=iGoods1[i];
						iCount[iNum]=iCounts1[i];
						iNum++;
					}
				}
				else
				{
					iGoods[iNum]=iGoods1[i];
					iCount[iNum]=iCounts1[i];
					iNum++;
				}
			}
		}
	}
	return;
}
//////////////////////////////////////////////////////////////////////////
// ʹ��������ϳɵ���
CT_INT32 CCUserItem::UseItemDesign(const ItemData &useItem)
{
    _AUTOMUTEX(m_pMutex);

    ENUM_UseItemFail enFail = EUI_SUCCESS;

    CT_INT32 iCount = 1;
    G_ItemDesign stItem = {0};
    if (CCItemScriptSetting::getSingleton().GetItemDesign(useItem.iID, stItem))
    {
        CT_INT32 MIDs[MATERIAL_TYPES] = {0};
        CT_INT32 CNTs[MATERIAL_TYPES] = {0};
        CT_INT32 iSmithingCost = 0; // ԭ���������װ������¼��ǿ�����ѡ�

        // ����Ǯ�Ƿ��㹻
        MasterVariableData mvd = {0};
        m_pUser->m_userBase.GetMasterData(mvd);
        if (mvd.stRes.iGold + stItem.iCostGold < 0) {
            enFail = EUI_NO_GOLD;
        } else if (CalcEmptySlot(m_SlotIdx) < iCount) {
            enFail = EUI_NO_SLOT;
        } else {
            // �������Ƿ��㹻
            for (int i = 0; i < MATERIAL_TYPES; ++i) {
                MIDs[i] = stItem.Materail[i].iMID;
                CNTs[i] = stItem.Materail[i].iCount;                
            }
            if (!AutoExpendItem(SAVETAG_USE_ITEM_FINISH, MIDs, CNTs, sizeof(MIDs) /  sizeof(*MIDs), ITEM_OP_MODE_CHECK)) {
                enFail = EUI_NO_COUNT;
            }
            for (int i = 0; i < MATERIAL_TYPES; ++i) {
                // ��������а���װ������¼��ǿ������
                G_ItemBase itBase = {0};
                CCItemScriptSetting::getSingleton().GetItemBase(MIDs[i], itBase);
                if (itBase.iTID == ITEM_TYPE_EQUIPMENT) {
                    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it) {
                        if (it->second.iID == MIDs[i]) {
                            EquipmentData eqData = {0};
                            m_pUser->m_userEquip.GetData(it->second.key, eqData);
                            G_Equipment itEquip = {0};
                            if (CCItemScriptSetting::getSingleton().GetEquipment(it->second.iID, itEquip))
                            {
                                // ǿ�����ĵĽ�Ǯ����������ļ۸������һ����������0��
                                iSmithingCost = G_GetItemSell(0, eqData.iLevel, itEquip.fGoldCoefficient1, itEquip.fGoldCoefficient2);
                            }
                            break;
                        }
                    }
                    break; // �����������һ��װ��
                }
            }
        }

        // �ϳɵ���
        if (enFail == EUI_SUCCESS)
        {
			//add by caihan
			//�жϺϳ�����
			CMD_G_TaskEventInfo taskEventInfo = {0};
			taskEventInfo.iTaskMainType = TASK_MAIN_TYPE_SMITHY;
			taskEventInfo.iTaskSubType = TASK_SUB_TYPE_SMITHY_MIX;
			m_pUser->m_userTask.DisposeTask(taskEventInfo);
			//end                        
            
            // ���ĵ���
            AutoExpendItem(SAVETAG_USE_ITEM_FINISH, MIDs, CNTs, sizeof(MIDs) /  sizeof(*MIDs), ITEM_OP_MODE_EXEC);

            // ��õ���
            CT_INT32 iSlot = 0;
            AddItemData(SAVETAG_USE_ITEM_FINISH, stItem.iItemID, iCount, iSlot, ITEM_OP_MODE_EXEC);

            // ����ϳ���һ��װ�����Ҳ�������װ�����̳���ǿ������
            G_ItemBase itBase = {0};
            CCItemScriptSetting::getSingleton().GetItemBase(stItem.iItemID, itBase);
            if (itBase.iTID == ITEM_TYPE_EQUIPMENT && iSmithingCost > 0) {
                ItemData itData = {0};
                EquipmentData eqData = {0};
                G_Equipment equip = {0};
                if (GetData(iSlot, itData) && CCItemScriptSetting::getSingleton().GetEquipment(itData.iID, equip)) {
                    eqData.key = itData.key;                
                    // ǿ���ȼ����ܳ��������ȼ�
                    while (eqData.iLevel < mvd.stGrade.iLevel && iSmithingCost > 0)
                    {
                        iSmithingCost += G_GetItemStrengthenGold(eqData.iLevel++, equip.fGoldCoefficient1, equip.fGoldCoefficient2);
                    }
                    m_pUser->m_userEquip.InsertData(eqData.key, eqData);
                    CMD_G_SmithingEquipSuc stSuc = {0};
                    stSuc.dwUserID = m_pUser->GetUserID();
                    stSuc.key = eqData.key;
                    stSuc.iLevel = eqData.iLevel;
                    m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_SMITHING_EQUIP_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);
                }
            }
            
			if (G_BroadcastFilter(7, stItem.iItemID))
			{
				MasterData masterData = {0};
				RollBroadcast rb[2]={0};
				m_pUser->m_userBase.GetMasterData(masterData);
				rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
				rb[1].SetValue(stItem.iItemID,"");
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{24}<2:%d>{25}",m_pUser->GetUserID(), masterData.cNickName,stItem.iItemID);
				G_SendRollBroadecast(24,3,1,(CT_CHAR*)rb,sizeof(rb));
			}

            // ���Ľ�Ǯ
            MasterVariableData mvd = {0};
            mvd.stRes.iGold = stItem.iCostGold;
            m_pUser->m_userBase.AddMastData(mvd, SAVETAG_USE_ITEM_DESIGN);

            m_pUser->UpdateAndSendMasterData(EMI_RESOURCE);
        }
    } else {
        enFail = EUI_DATA_ERROR;
    }
    return enFail;
}
// ���ĵ��߻������
CT_INT32 CCUserItem::UseItemBuff(const ItemData &useItem)
{
    _AUTOMUTEX(m_pMutex);

    ENUM_UseItemFail enFail = EUI_SUCCESS;

    G_ItemBuff stItem = {0};
    if (CCItemScriptSetting::getSingleton().GetItemBuff(useItem.iID, stItem))
    {
        // ����ܷ�ʹ��
        BuffData buffData = {0};
        CT_INT32 duration = 0;
        CT_INT64 currTime = time(0);
        CT_INT64 toggleTime = currTime;
        if (m_pUser->m_userBuff.SearchBuffData(stItem, buffData))
        {
            G_ItemBuff itBuff = {0};
            CCItemScriptSetting::getSingleton().GetItemBuff(buffData.iID, itBuff);
            if (stItem.iPriority < itBuff.iPriority) {
                // ���ȼ��������ܾ�ʹ��
                enFail = EUI_NO_PRIORITY;
            } else if (useItem.iID == buffData.iID) {
                // ͬһ�����棬�̳г���ʱ��
                duration = buffData.iDuration;
                toggleTime = buffData.toggleTime;
            }
        }

        if (enFail == EUI_SUCCESS) {
            // ��������
            CT_INT32 iCount = 1;//useItem.iCount;
            ExpendItem(SAVETAG_USE_ITEM_FINISH, useItem.iIndex, iCount);     

            // �������
            G_ItemBuff itBuff = {0};
            BuffData bfData = {0};
            CCItemScriptSetting::getSingleton().GetItemBuff(useItem.iID, itBuff);
            bfData.key = useItem.key;
            bfData.iID = useItem.iID;
            bfData.iGroup = itBuff.iGroup;
            bfData.iDuration = duration + itBuff.iDuration;
            bfData.toggleTime = toggleTime;
            m_pUser->m_userBuff.InsertData(bfData.iGroup, bfData);            
            m_pUser->UpdateFactorData();

            // ֪ͨ�ͻ���
            CMD_G_UserItemBuff buff = {0};
            buff.dwUserID = m_pUser->GetUserID();
            buff.iGroup = bfData.iGroup;
            buff.iID = bfData.iID;
            buff.leftTime = bfData.iDuration - (currTime - bfData.toggleTime);
            m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_BUFF, (CT_CHAR *)&buff, sizeof(buff), false, true);

            m_pUser->UpdateAndSendMasterData(EMI_BATLLE);
        }
    }
    else
    {
        enFail = EUI_DATA_ERROR;
    }

    return enFail;
}
// ʹ����Ƭ�ϳɵ���
CT_INT32 CCUserItem::UseItemChip(const ItemData &useItem)
{
    _AUTOMUTEX(m_pMutex);

    ENUM_UseItemFail enFail = EUI_SUCCESS;
    G_ItemChip stItem = {0};

    if (CCItemScriptSetting::getSingleton().GetItemChip(useItem.iID, stItem))
    {
        // ����Ǯ�Ƿ��㹻
        MasterVariableData mvd = {0};
        m_pUser->m_userBase.GetMasterData(mvd);
        if (mvd.stRes.iGold + stItem.iCostGold < 0) {
            enFail = EUI_NO_GOLD;
        } else if (SearchSlot(m_SlotIdx) == -1) {
            enFail = EUI_NO_SLOT;
        } else if (CalcItemCount(stItem.iID) < stItem.iCount) {
            enFail = EUI_NO_COUNT;
        }

        if (enFail == EUI_SUCCESS) {
            // ������Ʒ
            AutoExpendItem(SAVETAG_USE_ITEM_CHIP, stItem.iID, stItem.iCount, ITEM_OP_MODE_EXEC);

            // ������Ʒ
            CT_INT32 iCount = 1;            
            AutoAddItemData(SAVETAG_USE_ITEM_CHIP, stItem.iItemID, iCount, ITEM_OP_MODE_EXEC);

			if (G_BroadcastFilter(7, stItem.iItemID))
			{
				MasterData masterData = {0};
				RollBroadcast rb[2]={0};
				m_pUser->m_userBase.GetMasterData(masterData);
				rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
				rb[1].SetValue(stItem.iItemID,"");
				//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{24}<2:%d>{25}",m_pUser->GetUserID(), masterData.cNickName,stItem.iItemID);
				G_SendRollBroadecast(24,3,1,(CT_CHAR*)rb,sizeof(rb));
			}

            // ���Ľ�Ǯ
            MasterVariableData mvd = {0};
            mvd.stRes.iGold = stItem.iCostGold;
            m_pUser->m_userBase.AddMastData(mvd, SAVETAG_USE_ITEM_CHIP);

            m_pUser->UpdateAndSendMasterData(EMI_RESOURCE);
        }
    } else {
        enFail = EUI_DATA_ERROR;
    }
    return enFail;
}

// װ��/ж��
CT_INT32 CCUserItem::UseEquipment(const ItemData &useItem)
{
    _AUTOMUTEX(m_pMutex);

    ENUM_UseItemFail enFail = EUI_SUCCESS;
    G_Equipment stItem = {0};
	EquipmentData equipData = {0};
	m_pUser->m_userEquip.GetData(useItem.key, equipData);

    if (CCItemScriptSetting::getSingleton().GetEquipment(useItem.iID, stItem))
    {
        DataMap_IT itData = m_DataMap.find(useItem.iIndex);
        ItemData itTar = {0};

        if (useItem.iIndex > m_SlotIdx)     // �Ǳ�����
        {
            // װ������
            DataMap_IT itEquiped = m_DataMap.find(stItem.iEID);

            if (itEquiped != m_DataMap.end())
            {                    
                // װ������װ��������                                        
                SwapItem(itEquiped, itData);                                        
            }
            else
            {
                // װ������װ��
                MoveItem(itData, stItem.iEID);       
            }

			// �жϴ���ָ����װ��n��,����0��װ���ȼ�������1��װ��Ʒ�ʣ�����2�����λ�ã�����3��װ��ID
			CT_INT32 iCount = 0;
			CMD_G_TaskSystemInfo taskSysInfo = {0};	
			G_ItemBase itemBase = {0};
			G_GetItemBase(stItem.iID, &itemBase);
			taskSysInfo.iTaskMainType = TASK_MAIN_TYPE_SYS;
			taskSysInfo.iTaskSubType = TASK_SUB_TYPE_SYSTEM_WEAR_EQUIPMENT;
			taskSysInfo.arrayParam[0] = equipData.iLevel;
			taskSysInfo.arrayParam[1] = itemBase.iQuality;
            taskSysInfo.arrayParam[2] = stItem.iSlot;
			taskSysInfo.arrayParam[3] = stItem.iID;
			taskSysInfo.iProgress = 1;
			m_pUser->m_userTask.DisposeTask(taskSysInfo);
        }
        else
        {
            // ж�µ���
            CT_INT32 idx = SearchSlot(m_SlotIdx);
            if (idx > -1)
            {
                // �ƶ�����
                MoveItem(itData, idx);
            }
            else
            {
                enFail = EUI_NO_SLOT;
            }
        }
    }
    else
    {
        enFail = EUI_DATA_ERROR;
    }
    return enFail;
}

// �򿪱���    
CT_INT32 CCUserItem::UseItemChest(const ItemData &useItem)
{
    _AUTOMUTEX(m_pMutex);

    ENUM_UseItemFail enFail = EUI_SUCCESS;

	G_ItemChest stItem = {0};    
	if (CCItemScriptSetting::getSingleton().GetItemChest(useItem.iID, stItem))
	{
        struct ST_AniItem {
            CT_INT32 type;
            CT_INT32 itemID;
        };
        ST_AniItem aniItemMagicBox[] = {
            {ITEM_MAGIC_BOX, 12010},    // ħ��
            {ITEM_MAGIC_BOX, 18108},    // ��ħ��
            {ITEM_MAGIC_BOX, 18109},    // ��ħ��
            {ITEM_MAGIC_BOX, 18110},    // ��ħ��
            {ITEM_MAGIC_BOX, 18111},    // ��ħ��
            {ITEM_MAGIC_BOX, 18112},    // ��ħ��
            {ITEM_RUNE_BAG, 12013},     // ���Ĵ�1
            {ITEM_RUNE_BAG, 12014},     // ���Ĵ�2
		{ITEM_PATCAVE_CHEST1, 19301},     // ��Ѩ����
		{ITEM_PATCAVE_CHEST2, 19302},     // ��Ѩ����
        };
  //      // ����ħ�б��뱣֤���������ϵĿո���
  //      for (CT_INT32 i = 0; i < sizeof(aniItemMagicBox) / sizeof(*aniItemMagicBox); ++i) 
		//{
  //          if ((useItem.iID == aniItemMagicBox[i].itemID && ITEM_MAGIC_BOX == aniItemMagicBox[i].type && CalcEmptySlot() < 2) ||
		//(useItem.iID == aniItemMagicBox[i].itemID && CalcEmptySlot() < 1) || CalcEmptySlot() < 1) {
  //              return EUI_MORE_BAG_FOR_MAGIC_BOX;
  //          }
  //      }
		//������Ը��ӵ���֤
		if(CalcEmptySlot() < stItem.iMaxGoods && stItem.iMaxGoods != 0)
			return EUI_MORE_BAG_FOR_MAGIC_BOX;

		//������
		CT_INT32 iGoods[CHEST_GOODS_MAX]={0};
		CT_INT32 iCounts[CHEST_GOODS_MAX]={0};
		ResAmount Check[CHEST_GOODS_MAX*2]={0};
		CT_INT32 iNum=0;
		FakeProBase fb={0};
		if(ITEM_MAGIC_BOX_TRUE == useItem.iID)//���츣��ָͨ��12010��ħ�д���ͳ��
			G_GetFakeProDta(m_pUser,SAVETAGEEX_OPEN_CHEST,fb);
		OpenChest(useItem.iID,iGoods,iCounts,iNum,Check,fb);
		G_ClearFakeProDat(m_pUser,SAVETAGEEX_OPEN_CHEST,fb);

		CT_INT32 iType=0;
		for(CT_INT32 i=0;i<CHEST_GOODS_MAX-1;++i)
		{
			if(iGoods[i] == 0 || iCounts[i] == 0)continue;
			for(CT_INT32 j=i+1;j<CHEST_GOODS_MAX;++j)
			{
				if(iGoods[j] == 0 || iCounts[j] == 0)continue;
				if(iGoods[i] == iGoods[j])
				{
					iCounts[i]+=iCounts[j];
					iCounts[j]=0;
					iGoods[j]=0;
				}
			}
			iType++;
		}


        // ���㿪������õ��ĵ��ߺ���Դ
        struct ST_GameRes {
            CT_INT32 iType;
            CT_INT32 iID;
            CT_INT32 iCount;
        };
        struct ST_ChestInfo {
            CT_INT32 type;
            ST_GameRes reses[CHEST_GOODS_MAX * 3];
        };
        ST_ChestInfo chestInfo = {0};
        //ST_GameRes chestInfo.reses[CHEST_GOODS_MAX * 3] = {0};
        CT_INT32 n = 0;
        for (CT_INT32 i = 0; i < iType; ++i) {
            if (iGoods[i] != 0 && iCounts[i] != 0) {
                chestInfo.reses[n].iType = CLIENT_OBJ_ITEM;
                chestInfo.reses[n].iID = iGoods[i];
                chestInfo.reses[n].iCount = iCounts[i];
                n++;
            }
        }
        for (CT_INT32 i = 0; i < CHEST_GOODS_MAX * 2; ++i)
        {
            if (Check[i].iCount != 0) {
                chestInfo.reses[n].iType = CLIENT_OBJ_RES;
                chestInfo.reses[n].iID = Check[i].iID;
                chestInfo.reses[n].iCount = Check[i].iCount;
                n++;
            }
        }

        ENUM_LootItem enLoot = m_pUser->LootItems(SAVETAG_USE_ITEM_FINISH, iGoods, iCounts, iType);

		if (LOOT_SUC == enLoot) {
            MasterVariableData mvd = {0};
			for (int i = 0; i < CHEST_GOODS_MAX*2; ++i)
			{	
                if (Check[i].iCount == 0) continue;
			    switch (Check[i].iID)
			    {
			    case VALUE_TYPE_GOLD :      mvd.stRes.iGold += Check[i].iCount;break;
			    case VALUE_TYPE_DIAMOND :   mvd.stRes.iDiamod += Check[i].iCount;break;
			    case VALUE_TYPE_FOOD :      mvd.stRes.iFood += Check[i].iCount;break;
			    case VALUE_TYPE_TALENTNUM : mvd.stRes.iTalentNum += Check[i].iCount;break;
			    case VALUE_TYPE_MASTERXP :  mvd.stGrade.iCurrentExp += Check[i].iCount;break;
			    case VALUE_TYPE_RANKXP :    mvd.stRank.iCurrentExp += Check[i].iCount;break;
			    case VALUE_TYPE_CIVILIXP :  mvd.stCivili.iCurrentExp += Check[i].iCount;break;
				case VALUE_TYPE_THROWTIMES : m_pUser->m_userIsland.AddThrowTimes(Check[i].iCount);break;
				case VALUE_TYPE_SEEK_SPITE_TIMES: 
					{
						if (!m_pUser->m_userSeek.AddSpiteTimes(Check[i].iCount))
						{
							return EUI_UNUSABLE;
						}
					}
					break;
			    default : continue;
			    }
			}
            m_pUser->m_userBase.AddMastData(mvd, SAVETAG_USE_ITEM_FINISH);
            m_pUser->UpdateAndSendMasterData(EMI_RESOURCE);
            
            // �����ħ��
            for (CT_INT32 i = 0; i < sizeof(aniItemMagicBox) / sizeof(*aniItemMagicBox); ++i)
            {
                if (useItem.iID == aniItemMagicBox[i].itemID) {
                    chestInfo.type = aniItemMagicBox[i].type;
                    m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_CHEST_ITEM, 
                        (CT_CHAR*)&chestInfo, sizeof(chestInfo.type) + sizeof(*chestInfo.reses) * n, false, true);
                    break;
                }
            }

            if (chestInfo.type == 0) {
                m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_CHEST_ITEM, 
                    (CT_CHAR*)&chestInfo, sizeof(chestInfo.type), false, true);
            }

			CT_INT32 iCount = 1;
			ExpendItem(SAVETAG_USE_ITEM_FINISH, useItem.iIndex, iCount);

			for (CT_INT32 i=0;i<CHEST_GOODS_MAX;++i)
			{
				if (G_BroadcastFilter(6, iGoods[i]))
				{
					MasterData masterData = {0};
					RollBroadcast rb[3]={0};
					m_pUser->m_userBase.GetMasterData(masterData);
					rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
					rb[1].SetValue(useItem.iID,"");
					rb[2].SetValue(iGoods[i], "");
					//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{22}<2:%d>{23}<2:%d>",m_pUser->GetUserID(), masterData.cNickName,useItem.iID,iGoods[i]);
					G_SendRollBroadecast(22,3,1,(CT_CHAR*)rb,sizeof(rb));
				}
			}
		} 
		else 
		{
            switch (enLoot){
                case LOOT_BAG_FULL: enFail = EUI_NO_SLOT; break;
                case LOOT_ELM_FULL : enFail = EUI_NO_ELM_SLOT; break;
                default: enFail = EUI_DATA_ERROR;
            }
		}
	}
    return enFail;
}

// ʹ�ûָ���Դ�ĵ���
CT_INT32 CCUserItem::UseItemResource(const ItemData &useItem)
{
    _AUTOMUTEX(m_pMutex);

    ENUM_UseItemFail enFail = EUI_SUCCESS;
    G_ItemResource stItem = {0};    
    if (CCItemScriptSetting::getSingleton().GetItemResource(useItem.iID, stItem))
    {
        MasterData master = {0};
        m_pUser->m_userBase.GetMasterData(master);
        if (stItem.iRID == VALUE_TYPE_HEALTH && master.mvd.stRes.iCurrentHP >= master.mcs.iMaxHP)
        {
            enFail = EUI_MAX_HP;
        }
        if (stItem.iRID == VALUE_TYPE_MANA && master.mvd.stRes.iCurrentMP >= master.mcs.iMaxMP)
        {
            enFail = EUI_MAX_MP;
        }
		if (useItem.iCount <= 0) {
			enFail = EUI_DATA_ERROR;
		}

        if (enFail == EUI_SUCCESS)
        {
            DataMap_IT its[ITEM_SLOT_COUNT];
            CT_INT32 num = ITEM_SLOT_COUNT;

            if (SearchItemData(useItem.iID, its, num) >= useItem.iCount)
            {
                CT_INT32 itCount = useItem.iCount;
                // ���ĵ��� - ������ָ�����ӵĵ���
                CT_INT32 theIndex = -1;
                for (int i = 0; i < num; ++i) {
                    ItemData data = its[i]->second;
                    if (data.iIndex == useItem.iIndex) {
                        if (itCount > data.iCount) {
                            itCount -= data.iCount; // ������ӵ����������ˡ�
                        } else {
                            data.iCount = itCount;  // ���������������Ϊ itCount �ĵ���
                            itCount = 0;
                        }
                        AddItemCount(SAVETAG_USE_ITEM_FINISH, its[i], -data.iCount);
                        theIndex = i;   // ��¼���ĵĸ���
                        break;
                    }
                }
                // ���ĵ��� - �����������ӵ�ͬ�ֵ���
                for (int i = 0; i < num; ++i)
                {
                    if (itCount == 0) {
                        // ʹ�ý���
                        MasterVariableData addValue = {0};

                        switch (stItem.iRID)
                        {
                        case VALUE_TYPE_GOLD :
                            addValue.stRes.iGold = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_DIAMOND :
                            addValue.stRes.iDiamod = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_FOOD :
                            addValue.stRes.iFood = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_STAMINA :
                            addValue.stRes.iStamina = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_HEALTH :
                            addValue.stRes.iCurrentHP = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_MANA :
                            addValue.stRes.iCurrentMP = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_TALENTNUM :
                            addValue.stRes.iTalentNum = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_RANKXP :
                            addValue.stRank.iCurrentExp = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_CIVILIXP :
                            addValue.stCivili.iCurrentExp = stItem.iValue * useItem.iCount;
                            break;
                        case VALUE_TYPE_MASTERXP :
                            addValue.stGrade.iCurrentExp = stItem.iValue * useItem.iCount;
                            break;
                        default:
                            break;
                        }

                        // ����������Դ
                        m_pUser->m_userBase.AddMastData(addValue, SAVETAG_USE_ITEM_FINISH);

                        m_pUser->UpdateAndSendMasterData(EMI_RESOURCE);
                        break;
                    }   // if (itCount == 0)

                    if (i == theIndex) continue; // �������Ĺ��ĸ���

                    ItemData data = its[i]->second;
                    if (itCount > data.iCount){
                        itCount -= data.iCount; // data�����������ˡ�
                    } else {
                        data.iCount = itCount; // data��ʹ����Ϊ itCount
                        itCount = 0;
                    }
                    AddItemCount(SAVETAG_USE_ITEM_FINISH, its[i], -data.iCount);
                    
                } // (int i = 0; i < num; ++i)
            }   // (SearchItemData(useItem.iID, data, num))
            else
            {
                enFail = EUI_NO_COUNT;
            }
        }   // (enFail == EUI_SUCCESS)        
    } // (0 != G_GetItemResource(useItem.iID, &stItem))
    else
    {
        enFail = EUI_DATA_ERROR;
    }
    return enFail;
}

ENUM_UseItemFail EnchantOne(CT_INT32 iBID, CT_INT32 iIDX, CT_INT32 iLimit,CCUser *pUser)
{
	EnchantData data = {0};
    G_ValueRange vrGrade;
	G_ValueRange vrLevel; 
	G_GetValueRange(VALUE_TYPE_ENCHANT_GRADE, &vrGrade);
	G_GetValueRange(VALUE_TYPE_ENCHANT_LEVEL, &vrLevel);
	CT_INT32 iMaxLever = (CT_INT32)vrGrade.fMaxVal * (CT_INT32)vrLevel.fMaxVal;

	pUser->m_userEnchant.GetData(G_Combine64(iBID, iIDX), data);
	if (data.iGrade == -1 || data.iGrade >= iMaxLever)
	{
		return EUI_NOT_ACTIVE;
	}
	else if (data.iGrade < iLimit)
	{
		return EUI_INVALID_USE;
	}
	else
	{
		pUser->m_userEnchant.EnchantSuc(iBID, iIDX, 1);
		return EUI_SUCCESS;
	}
}

// ʹ��ħ������
CT_INT32 CCUserItem::UseItemEnchant(const ItemData &useItem)
{
	_AUTOMUTEX(m_pMutex);

	ENUM_UseItemFail enFile = EUI_SUCCESS;
	CT_INT32 iBID = 0;
	CT_INT32 iIDX = 0;
	CT_INT32 iCount = 0;
	G_ItemEffect itemEffect = {0};

	if (!G_GetItemEffect(useItem.iID, itemEffect))
	{
		return EUI_DATA_ERROR;
	}

	iBID = itemEffect.iObject / 10;                  //����IDǰ��λΪ��λ������λΪ���
	iIDX = itemEffect.iObject % 10;

	if (iBID != 0 && iIDX != 0)                      //ָ����λ��ָ�����ħ��
	{   
		enFile = EnchantOne(iBID, iIDX, itemEffect.iLimit, m_pUser);
	} 
	else if (iBID == 0 && iIDX == 0)                 //����λ��ħ��
	{
		for (CT_INT32 i = ITEM_SLOT_CLOTH;i <= ITEM_SLOT_SHOE;++i)
		{
			for(CT_INT32 j = 1;j <= ENCHANT_ATTR_CNT;++j)
			{
				if (EnchantOne(i, j, itemEffect.iLimit, m_pUser) == EUI_SUCCESS)
				{
					++iCount;
				}
			}
		}
		if (iCount == 0)                             //���в�λ����������Ч����
		{
			enFile = EUI_USE_NO_EFFECT;
		}
	}
	else                                         
	{
		iCount = 0;
		if (iBID == 0)                               //ָ�����λħ��
		{
			for (CT_INT32 i = ITEM_SLOT_CLOTH;i <= ITEM_SLOT_SHOE;++i)
			{
				if (EnchantOne(i, iIDX, itemEffect.iLimit, m_pUser) == EUI_SUCCESS)
				{
					++iCount;
				}
			}
			if (iCount == 0)                         //ָ�����λ����������Ч����
			{
				enFile = EUI_USE_NO_EFFECT;
			}
		}
		else if (iIDX == 0)                          //ָ����λħ��
		{
			for (CT_INT32 i = 1;i <= ENCHANT_ATTR_CNT;++i)
			{
				if (EnchantOne(iBID, i, itemEffect.iLimit, m_pUser) == EUI_SUCCESS)
				{
					++iCount;
				}
			}
			if (iCount == 0)                         //ָ����λ����������Ч����
			{
				enFile = EUI_USE_NO_EFFECT;
			}
		}
	}

	if (enFile == EUI_SUCCESS)
	{
		CT_INT32 iNum = 1;
		ExpendItem(SAVETAG_USE_ITEM_FINISH, useItem.iIndex, iNum);
	}

	return enFile;
}

CT_INT32 CCUserItem::UseItemBroadCast(const ItemData &useItem)
{
    _AUTOMUTEX(m_pMutex);

    CT_INT32 iCount = 1;//useItem.iCount;// �õ���ÿ��ֻ��ʹ��һ��
    if (m_pUser->m_userItem.ExpendItem(SAVETAG_USE_ITEM_FINISH, useItem.iIndex, iCount)) {

        G_ItemBase itBase = {0};
        CCItemScriptSetting::getSingleton().GetItemBase(useItem.iID, itBase);

        struct stItemBc {
            CT_DWORD dwPlayerID;
            CT_INT32 iItemID;
        } itemBc;
        itemBc.dwPlayerID = m_pUser->GetUserID();
        itemBc.iItemID = useItem.iID;

        if (itBase.iQuality > ITEM_QUALITY_LEVEL_2) {
            const CT_INT32 BROAD_ITEM_AID = 3401; // ACTIVE_NEW_YEAR
            // ��Ʒ�ʣ�Ҫ���㲥
            MasterData md = {0};
			RollBroadcast rb[4]={0};
            m_pUser->m_userBase.GetMasterData(md);
            CT_INT32 nTotalTimes = m_pUser->m_userActive.GetActiveQuanity(BROAD_ITEM_AID) + 1;
			rb[0].SetValue(m_pUser->GetUserID(), md.cNickName);
			rb[1].SetValue(useItem.iID, "");
			rb[2].SetValue(m_pUser->GetUserID(), md.cNickName);
			rb[3].SetValue(nTotalTimes, "");
            //sprintf(broadMes.cMes, "{94}<1:%d:%s>{95}<2:%d>{96}<1:%d:%s>\xe7\xac\xac%d{97}", 
                //m_pUser->GetUserID(), md.cNickName, useItem.iID, m_pUser->GetUserID(), md.cNickName, nTotalTimes);
			G_SendRollBroadecast(94,1,1,(CT_CHAR*)rb,sizeof(rb));

            CCUserManager::getSingleton().BroadCastMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_BROADCAST, (CT_CHAR *)&itemBc, sizeof(itemBc));
            
            // ʹ���ۻ����Ի��ʽ��
            MSC_DB_ActiveEventStatus stDb = {0};
            stDb.dwUserID = m_pUser->GetUserID();
            stDb.iStatus = ACTIVE_UNCOMPLETE;
            stDb.iAID = BROAD_ITEM_AID;
            stDb.iQuantity = 1;
            m_pUser->m_userActive.SetActiveStatus(stDb.iAID, stDb.iStatus, nTotalTimes);
            m_pUser->SaveData(MSC_MAIN_DB_ACTIVE_DATA, MSC_SUB_ACTIVE_STATUS_UPDATE, (CT_CHAR *)&stDb, sizeof(stDb));
        } else {
            // ��Ʒ�ʣ���֪ͨ�û��Լ�
            m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_BROADCAST, (CT_CHAR *)&itemBc, sizeof(itemBc));
        }
        return EUI_SUCCESS;
    }
    return EUI_NO_COUNT;
}

//ʹ�ó��ﵰ
CT_INT32 CCUserItem::UseItemPetEgg(const ItemData &useItem)
{
	_AUTOMUTEX(m_pMutex);
	CT_INT32 iCount = 1;//useItem.iCount;// �õ���ÿ��ֻ��ʹ��һ��
	if (m_pUser->m_userItem.ExpendItem(SAVETAG_USE_ITEM_FINISH, useItem.iIndex, iCount)) 
	{
		MSC_UsePetEgg upe={0};
		upe.dwUserID=m_pUser->GetUserID();
		upe.iItemID=useItem.iID;
		CT_DWORD dwThreadID=GAME_THREAD_CITY+(m_pUser->GetUserID()%((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum);
		G_SendThreadInfo(dwThreadID,MSC_MAIN_EVENT_PET,MSC_SUB_USE_EGG,(CT_CHAR*)&upe,sizeof(upe));
		return EUI_SUCCESS;
	}
	return EUI_NO_COUNT;
}

// �����Ʒʹ������
CT_BOOL CCUserItem::CheckItemUsable(const G_ItemBase &itSuc)
{
    _AUTOMUTEX(m_pMutex);

    MasterData master = {0};
    m_pUser->m_userBase.GetMasterData(master);
    // �ȼ�����
    if (itSuc.iLevel > master.mvd.stGrade.iLevel) return CT_FALSE;
    // ְҵ��ƥ��
    if (itSuc.iClass != 0 && itSuc.iClass != master.iOccupation) return CT_FALSE;
    return CT_TRUE;
}
// �����Ƿ���Ч
CT_BOOL CCUserItem::CheckSlotValid(const CT_INT32 iIndex)
{
    if (iIndex < ITEM_SLOT_EQUIPMENT 
        || (iIndex > ITEM_SLOT_SHOE && iIndex < m_SlotIdx)
        || (iIndex > m_SlotIdx + m_pUser->m_userBase.GetSlotCount(m_SlotIdx)))
    {
        return CT_FALSE;
    }
    return CT_TRUE;
}

// �õ���������
CT_INT32 CCUserItem::GetItemCount(const CT_INT32 iItemID)
{
    _AUTOMUTEX(m_pMutex);
    CT_INT32 num = 0;
    for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        if ((*it).second.iID == iItemID)
        {
            num += (*it).second.iCount;                
        }
    }
    return num;
}

//////////////////////////////////////////////////////////////////////////
// ��õ�������
CT_CHAR *CCUserItem::GetItemName(const CT_INT32 iID, CT_CHAR *szBuff, CT_INT32 nLen)
{
	_AUTOMUTEX(m_pMutex);
    G_ItemBase itBase = {0};
    CCItemScriptSetting::getSingleton().GetItemBase(iID, itBase);
    memcpy(szBuff, itBase.szName, nLen);
    return szBuff;
}

// ���͵�����Ϣ���ͻ���
CT_VOID CCUserItem::SendItemData(const ItemData &itemData)
{
	_AUTOMUTEX(m_pMutex);
    CMD_G_UseItemSuc itSuc = {0};
    itSuc.dwUserID = m_pUser->GetUserID();
    itSuc.key = itemData.key;
    itSuc.iIndex = itemData.iIndex;
    itSuc.iItemID = itemData.iID;
    itSuc.iCount = itemData.iCount;
    if (m_SlotIdx == ITEM_SLOT_BAG) {
        m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_USE_ITEM_SUC, (CT_CHAR *)&itSuc, sizeof(itSuc), false, true);
    } else if (m_SlotIdx == ITEM_SLOT_ELM) {
        m_pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_USE_ITEM_SUC, (CT_CHAR *)&itSuc, sizeof(itSuc), false, true);
    }

    if (itemData.iIndex <= m_SlotIdx) {
        CCLogMeg::getSingleton().WriteError("Item Index <= %d", m_SlotIdx);
    }    
}

CT_INT32 CCUserItem::UserOwnItemGet(const CT_INT32 iID,const CT_INT32 iLevel)
{
	_AUTOMUTEX(m_pMutex);
	DataMap_IT it = m_DataMap.begin();
	CT_INT32 iOwnNum=0;
	while(it!=m_DataMap.end())
	{
		if (it->second.iID==iID)
		{
			if (iLevel>0)
			{
				//����key��ȡװ��ǿ���ȼ�
				EquipmentData data = {0};
				if (m_pUser->m_userEquip.GetData(it->second.key, data))
				{
					if (data.iLevel>=iLevel)
					{
						iOwnNum+=it->second.iCount;
					}
				}
			}
			else
			{
				iOwnNum+=it->second.iCount;
			}
		}
		it++;
	}
	return iOwnNum;
}

CT_INT32 CCUserItem::UserItemTitle(const ItemData &userItem)
{
	_AUTOMUTEX(m_pMutex);
	G_ItemTitle git = {0};
	if (EUI_SUCCESS == m_pUser->m_userTitle.ActiveTitle(userItem.iID))
	{
		AutoExpendItem(SAVETAG_ACTIVE_TITLE, userItem.iID, 1, ITEM_OP_MODE_CKEXE);
	}

	return EUI_SUCCESS;
}

// ʹ��BOSS����
CT_INT32 CCUserItem::UserItemBoss(const ItemData &userItem)
{
	_AUTOMUTEX(m_pMutex);
	AutoExpendItem(SAVATAG_ITEM_BOSS, userItem.iID, 1, ITEM_OP_MODE_CKEXE);

	G_ItemBase ib={0};
	G_GetItemBase(userItem.iID,&ib);
	m_pUser->m_userArmy.InsertBufferData(ib.iEquipSkill,ib.iLevel);
	// ���;�����Ϣ
	m_pUser->m_userArmy.SendArmyInfo(MAIN_G_ENTER_ENV); 
	return EUI_SUCCESS;
}

// ʹ��ս�Ӹ�������
CT_INT32 CCUserItem::UserItemChangeFTName(const ItemData &userItem,CT_CHAR *pName)
{
	_AUTOMUTEX(m_pMutex);
	if(AutoExpendItem(SAVATAG_CHANGE_FTNAME, userItem.iID, 1, ITEM_OP_MODE_CHECK))
	{
		AutoExpendItem(SAVATAG_CHANGE_FTNAME, userItem.iID, 1, ITEM_OP_MODE_CKEXE);

		MSC_ChangeFTName cftn={0};
		cftn.dwUserID=m_pUser->GetUserID();
		memcpy(cftn.cName,pName,NAME_SIZE);
		CT_DWORD threadID = GAME_THREAD_CITY + m_pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadCityNum;
		G_SendThreadInfo(threadID,MSC_MAIN_FIGHTTEAM_EVENT,MSC_SUB_DB_FIGHTTEAMMEMBER_FIND,(CT_CHAR*)&cftn,sizeof(cftn));
		return EUI_SUCCESS;
	}
	return EUI_NO_COUNT;
}

// ʹ��������ϳɵ���
CT_INT32 CCUserItem::UseItemElmPowerDesign(const ItemData &useItem)
{
	_AUTOMUTEX(m_pMutex);
	ENUM_UseItemFail enFail = EUI_SUCCESS;

	CT_INT32 iCount = 1;
	G_ItemDesign stItem = {0};
	if (CCItemScriptSetting::getSingleton().GetItemDesign(useItem.iID, stItem))
	{
		CT_INT32 MIDs[MATERIAL_TYPES] = {0};
		CT_INT32 CNTs[MATERIAL_TYPES] = {0};
		CT_INT32 iMaxGrade = 1; // ������ߵȼ�
		CT_INT32 iExpendSlot[MATERIAL_TYPES]={0};//���²�λ

		// ����Ǯ�Ƿ��㹻
		MasterVariableData mvd = {0};
		m_pUser->m_userBase.GetMasterData(mvd);
		if (mvd.stRes.iGold + stItem.iCostGold < 0) {
			enFail = EUI_NO_GOLD;
		} else if (CalcEmptySlot(m_SlotIdx) < iCount) {
			enFail = EUI_NO_SLOT;
		} else 
		{
			// �������Ƿ��㹻
			for (int i = 0; i < MATERIAL_TYPES; ++i) {
				MIDs[i] = stItem.Materail[i].iMID;
				CNTs[i] = -stItem.Materail[i].iCount;                
			}
			if (m_pUser->LootItems(SAVETAG_USE_ITEM_FINISH, MIDs, CNTs, sizeof(MIDs) /  sizeof(*MIDs), ITEM_OP_MODE_CHECK)) {
				enFail = EUI_NO_COUNT;
			}
			for (int i = 0; i < MATERIAL_TYPES; ++i) 
			{
				// ��¼������ߵȼ�
				G_ItemBase itBase = {0};
				
				CCItemScriptSetting::getSingleton().GetItemBase(MIDs[i], itBase);
				if (itBase.iTID == ITEM_TYPE_ELMMARROW) 
				{
					for (DataMap_IT it = m_DataMap.begin(); it != m_DataMap.end(); ++it) 
					{
						if (it->second.iID == MIDs[i]) {
							ElmMarrowAttr eqData = {0};;

							m_pUser->m_userElmAttr.GetData(it->second.key, eqData);

							if(m_pUser->m_userElmAttr.isMarrowFullLevel((*it).second))
							{
								iExpendSlot[i]=(*it).second.iIndex;
								MIDs[i]=0;
								iMaxGrade = (iMaxGrade < eqData.iLevel?eqData.iLevel:iMaxGrade);
								break;
							}
							
						}
					}
				}
			}
		}

		//���µȼ�����
		if(iMaxGrade < 10)
			enFail = EUI_ELM_NO_GRADE;

		// �ϳɵ���
		if (enFail == EUI_SUCCESS)
		{
                       
			// ���ĵ���
			m_pUser->LootItems(SAVETAG_USE_ITEM_FINISH, MIDs, CNTs, sizeof(MIDs) /  sizeof(*MIDs), ITEM_OP_MODE_EXEC);

			for(CT_INT32 i=0;i<MATERIAL_TYPES;++i)
			{
				if(iExpendSlot[i] != 0) 
				{
					CT_INT32 iTempCount=abs(CNTs[i]);
					ExpendItem(SAVETAG_USE_ITEM_FINISH,iExpendSlot[i],iTempCount,ITEM_OP_MODE_EXEC);
				}
			}

			// ��õ���
			CT_INT32 iSlot = 0;
			AddItemData(SAVETAG_USE_ITEM_FINISH, stItem.iItemID, iCount, iSlot, ITEM_OP_MODE_EXEC);

			// ����ϳ���һ��װ�����Ҳ�������װ�����̳���ǿ������
			G_ItemBase itBase = {0};
			CCItemScriptSetting::getSingleton().GetItemBase(stItem.iItemID, itBase);
			if (itBase.iTID == ITEM_TYPE_ELMMARROW && iMaxGrade > 1) {
				ItemData itData = {0};
				ElmMarrowAttr eqData = {0};
				if (GetData(iSlot, itData) && m_pUser->m_userElmAttr.GetData(itData.key,eqData)) 
				{  
					eqData.iLevel=iMaxGrade;
					//m_pUser->m_userElmAttr.GenerateAttr(itData,iMaxGrade);
					m_pUser->m_userElmAttr.InsertData(eqData.key, eqData);   // ��������������
				}
			}

			//if (G_BroadcastFilter(7, stItem.iItemID))
			//{
			//	MasterData masterData = {0};
			//	RollBroadcast rb[2]={0};
			//	m_pUser->m_userBase.GetMasterData(masterData);
			//	rb[0].SetValue(m_pUser->GetUserID(), masterData.cNickName);
			//	rb[1].SetValue(stItem.iItemID,"");
			//	//sprintf(cBroadcastMes+sizeof(iBroadcastType)+sizeof(iBroadcastTime), "<1:%d:%s>{24}<2:%d>{25}",m_pUser->GetUserID(), masterData.cNickName,stItem.iItemID);
			//	G_SendRollBroadecast(24,3,1,(CT_CHAR*)rb,sizeof(rb));
			//}

			// ���Ľ�Ǯ
			MasterVariableData mvd = {0};
			mvd.stRes.iGold = stItem.iCostGold;
			m_pUser->m_userBase.AddMastData(mvd, SAVETAG_USE_ITEM_DESIGN);

			m_pUser->UpdateAndSendMasterData(EMI_RESOURCE);
		}
	} else {
		enFail = EUI_DATA_ERROR;
	}
	return enFail;
}

// ʹ����������
CT_INT32 CCUserItem::UseItemAttributeAdd(const ItemData &useItem)
{
	_AUTOMUTEX(m_pMutex);
	G_ItemTitle git = {0};
	if(AutoExpendItem(SAVATAG_ITEM_FIGHTADD, useItem.iID, useItem.iCount, ITEM_OP_MODE_CHECK))
	{
		G_ItemAttributeAddition iaa={0};
		if(!G_GetItemAttributeAddition(useItem.iID,&iaa))
		{
			CCLogMeg::getSingleton().WriteWaring("CCUserItem::UseItemAttributeAdd G_GetItemAttributeAddition itemid:%d",useItem.iID);
			return EUI_ERR_INFO;
		}

		MasterCombatStatus mcs={0};
		MasterFightAdd mfa={0};
		m_pUser->m_userBase.GetMasterFightAdd(iaa.iOccupation,mfa);
		CT_INT32 iAddValue=G_AttributeAdd(iaa.iAddBase,iaa.fCoefficient)*useItem.iCount;
		if(G_GetMasterCombatStatusValue(iaa.iType,mfa.mcs)+iAddValue > iaa.iMaxAdd)
		{
			return EUI_ADD_MAX;
		}

		G_AddFightAttribute(mcs,iaa.iType,iAddValue);

		m_pUser->m_userBase.AddMastFightData(iaa.iOccupation,mcs,SAVATAG_ITEM_FIGHTADD);
		AutoExpendItem(SAVATAG_ITEM_FIGHTADD, useItem.iID, useItem.iCount, ITEM_OP_MODE_CKEXE);

		CT_CHAR  buf[MAX_MES_MAX_BUFF_LEN]={0};
		CT_INT32 bufLen = MAX_MES_MAX_BUFF_LEN;
		if(m_pUser->m_userBase.GetMasterFightAdd(buf,bufLen))
		{
			m_pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_MASTER_FIGHTADD,(CT_CHAR*)buf, bufLen);
		}

		return EUI_SUCCESS;
	}

	return EUI_NO_COUNT;
}