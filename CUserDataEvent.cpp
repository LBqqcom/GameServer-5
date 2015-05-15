#include "CUserDataEvent.h"
#include "CLogMeg.h"
#include "CThreadMes.h"
#include "CFightingData.h"

CCUserDataEvent::CCUserDataEvent(CCGameMesManager* pMesManager, CT_DWORD id): CCGameMesEvent(pMesManager, id)
{
	RegistMessage(MAIN_G_USER_DATA,MSC_SUB_USER_DATA_INSERT);
	RegistMessage(MAIN_G_USER_DATA,MSC_SUB_INSERT_SEEKED_PLAYERINFO);
}

CCUserDataEvent::~CCUserDataEvent(void)
{
}

//处理用户消息
CT_VOID CCUserDataEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	if(mainID != MAIN_G_USER_DATA)
	{
		CCLogMeg::getSingleton().WriteError("CCUserDataEvent::OnUserEvent mainID != MAIN_G_USER_DATA mainID:%d",mainID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_USER_DATA_INSERT:
		{
			DisposeInsertData(pBuf,len);
			break;
		}
	case MSC_SUB_INSERT_SEEKED_PLAYERINFO:
		{
			DisposeSeekedInfo(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCUserDataEvent::OnUserEvent mainID:%d subID:%d",mainID,subID);
			return;
		}
	}

	return;
}

//插入数据
CT_VOID CCUserDataEvent::DisposeInsertData(CT_CHAR *pBuf,CT_DWORD len)
{
	if(sizeof(MSC_SUB_UserdataInsert) != len)
	{
		CCLogMeg::getSingleton().WriteError("CCUserDataEvent::DisposeInsertData sizeof(MSC_SUB_UserdataInsert) != len");
		return;
	}

	CCFightingData::getSingleton().InsertData(pBuf,len);
	return;
}

//插入被访者信息
CT_VOID CCUserDataEvent::DisposeSeekedInfo(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen != sizeof(MSC_DB_Seeked_Info))
	{
		CCLogMeg::getSingleton().WriteError("CCInsertUserInfo::DisposeSeekedInfo iLen != sizeof(MSC_DB_Seeked_Info)");
		return;
	}

	MSC_DB_Seeked_Info* pInfo = (MSC_DB_Seeked_Info*)pBuf;

	CCFightingData::getSingleton().InsertSeekedInfo(pInfo);
}