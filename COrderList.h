/********************************************************************
	created:	2013/01/30
	created:	30:1:2013   17:39
	filename: 	COrderList.h
	file base:	COrderList
	file ext:	h
	author:		xingweizhen
	
	purpose:	
*********************************************************************/
#ifndef COrderList_H_
#define COrderList_H_

#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CMD_Game.h"
#include <list>
#include "CMutex.h"

struct OrderInfo {
    CT_CHAR szSign[EXORDER_NO_LEN + 1];
    CT_DWORD dwUserID;
    CT_INT32 iPID;
	CT_INT32 iChannel;
	CT_INT32 iOrderSuc;	//1.订单已生成成功 0.等待充值服务器的创建结果
};

class CCOrderList: public CCSingleton<CCOrderList>
{
private:
	CCMutex   m_mutex;
public:
    CCOrderList();
    ~CCOrderList();
public:
    typedef std::list<OrderInfo> OrderInfoList;
    OrderInfoList m_listOrderSigned;
    CT_BOOL InsertSignedInfo(const CT_CHAR *szSign, const CT_DWORD dwUserID, const CT_INT32 iPID, const CT_INT32 iChannel);
    CT_BOOL PickSignedInfo(const CT_CHAR *szSign, OrderInfo &order, CT_BOOL bRemove = CT_TRUE);
	CT_VOID ReSendCharge();
};


#endif /* COrderList_H_ */