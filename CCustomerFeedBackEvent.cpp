#include "CCustomerFeedBackEvent.h"
#include "CMD_Game.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "ProtocolGame.h"
#include "CGameConfig.h"
#include "CUserEnterManager.h"
#include "CThreadMes.h"
#include "CUserManager.h"

CCCustomerFeedBackEvent::CCCustomerFeedBackEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_CUSTOMER_FEEDBACK,SUB_G_CUSTOMER_FEEDBACK);
}

CCCustomerFeedBackEvent::~CCCustomerFeedBackEvent(void)
{
}

//处理用户消息  最后的2个参数为备用
CT_VOID CCCustomerFeedBackEvent::OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
							CT_CHAR* pBuf,   CT_DWORD len, 
							CT_LONGLONG reserve)
{
	if(mainID != MAIN_G_CUSTOMER_FEEDBACK)
	{
		CCLogMeg::getSingleton().WriteError("CCCustomerFeedBackEvent::OnUserEvent mainID != MAIN_G_CUSTOMER_FEEDBACK");
		return;
	}

	switch(subID)
	{
	case SUB_G_CUSTOMER_FEEDBACK:
		{
			DisposeFeedBack(pBuf,len);
			break;
		}
	}
}

//信息反馈MSC_MAIN_FEEDBACK
CT_VOID CCCustomerFeedBackEvent::DisposeFeedBack(CT_CHAR *pBuf,const CT_INT32 iLen)
{
	MSC_DB_FeedBack FeedBack={0};
	CT_INT32 iSize=0;
	memcpy(&FeedBack.dwUserID,pBuf,sizeof(FeedBack.dwUserID));
	iSize+=sizeof(FeedBack.dwUserID);
	memcpy(FeedBack.cMobile,pBuf+iSize,sizeof(FeedBack.cMobile));
	iSize+=sizeof(FeedBack.cMobile);
	memcpy(FeedBack.cEmailContent,pBuf+iSize,(iLen-iSize >= FEEDBACK_CONTENT_SIZE)?FEEDBACK_CONTENT_SIZE-1:iLen-iSize);

	CT_DWORD threadid = GAME_THREAD_LOGDATABASE + FeedBack.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
	G_SendThreadInfo(threadid,MSC_MAIN_LOGDB_DATA,MSC_SUB_LOGDB_FEEDBACK,(CT_CHAR*)&FeedBack,sizeof(FeedBack));

	CCUser *pUser=0;
	if(CCUserManager::getSingleton().Find(FeedBack.dwUserID,&pUser))
	{
		pUser->GetNetInterface()->Send(MAIN_G_CUSTOMER_FEEDBACK,SUB_G_CUSTOMER_FEEDBACK_SUC);
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCCustomerFeedBackEvent::DisposeFeedBack UserID:%d",FeedBack.dwUserID);
	}
	return;
}
