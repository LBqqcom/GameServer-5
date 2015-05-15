/********************************************************************
	created:	2013/01/30
	created:	30:1:2013   17:38
	filename: 	COrderList.cpp
	file base:	COrderList
	file ext:	cpp
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#include "CMD_Charge.h"
#include "CGameConfig.h"
#include "CMasterScriptSetting.h"
#include "CNetInterface.h"
#include "CGlobeFunc.h"
#include "COrderList.h"


template<> CCOrderList* CCSingleton<CCOrderList>::ms_Singleton=0;

CCOrderList::CCOrderList()
{

}

CCOrderList::~CCOrderList()
{

}

CT_BOOL CCOrderList::InsertSignedInfo(const CT_CHAR *szSign, const CT_DWORD dwUserID, const CT_INT32 iPID, const CT_INT32 iChannel)
{
	_AUTOMUTEX(&m_mutex);
    OrderInfo info = {0};
    if (strlen(szSign) <= EXORDER_NO_LEN) {
        strncpy(info.szSign, szSign, EXORDER_NO_LEN);
        info.dwUserID = dwUserID;
        info.iPID = iPID;
		info.iChannel = iChannel;

        m_listOrderSigned.push_back(info);
        return CT_TRUE;
    } else {
        return CT_FALSE;
    }
}

CT_BOOL CCOrderList::PickSignedInfo(const CT_CHAR *szSign, OrderInfo &order, CT_BOOL bRemove)
{
	_AUTOMUTEX(&m_mutex);
    OrderInfoList::iterator it = m_listOrderSigned.end();
    for (it = m_listOrderSigned.begin(); it != m_listOrderSigned.end(); ++it)
    {
        if (strcmp(it->szSign, szSign) == 0)
        {
            break;
        }
    }
    if (it != m_listOrderSigned.end()) {
        strncpy(order.szSign, it->szSign, EXORDER_NO_LEN);
        order.dwUserID = it->dwUserID;
        order.iPID = it->iPID;
        if (bRemove) 
		{
            m_listOrderSigned.erase(it);
        }
		else
		{
			if (0 == it->iOrderSuc)
			{
				it->iOrderSuc = 1;
			}
			else
			{
				return CT_FALSE;
			}
		}
        return CT_TRUE;
    }
    return CT_FALSE;
}

CT_VOID CCOrderList::ReSendCharge()
{
	_AUTOMUTEX(&m_mutex);
	G_ChargeData stCharge = {0};
	CMD_CH_Charge chData = {0};
	OrderInfoList::iterator it = m_listOrderSigned.end();
	for (it = m_listOrderSigned.begin(); it != m_listOrderSigned.end(); ++it)
	{
		chData.dwUserID = it->dwUserID;
		chData.iChannel = it->iChannel;
		chData.GamePort = ((CCGameConfig&)CCConfig::getSingleton()).m_gamePort;
		memcpy(chData.cGameServer, ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.c_str(), sizeof(chData.cGameServer));
		memcpy(chData.cGameServerName, ((CCGameConfig&)CCConfig::getSingleton()).m_strServerName.c_str(), sizeof(chData.cGameServerName));
		//memcpy(chData.cImei, it->cImei, sizeof(chData.cImei));
		//memcpy(chData.cImsi, it->cImsi, sizeof(chData.cImsi));
		strncpy(chData.cExOrder, it->szSign, EXORDER_NO_LEN);
		CCMasterScriptSetting::getSingleton().GetChargeData(it->iPID, stCharge);
		chData.iDiamod = stCharge.iDiamond;
		g_pPChargeServer->Send(MAIN_CH_CHARGE, SUB_CH_CHAGRE, (CT_CHAR *)&chData, sizeof(chData), false, true);
	}
}

