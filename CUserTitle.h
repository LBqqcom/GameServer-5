/* 头衔功能实现   
 * 主要功能：头衔激活，头衔使用，定时更新头衔状态
 *         by zhouhuan
 */

#ifndef CUSERTITLE_H_
#define CUSERTITLE_H_

#include "GlobeScriptInterface.h"
#include "CUserBase.h"
#include "CUserData.h"
#include "CMutex.h"

// 保存用户使用的头衔信息
struct TitleData 
{
	CT_INT32 iID;				//头衔ID
	CT_INT32 iActiveTime;		//激活时间
	CT_INT32 iEffectiveTime;	//头衔有效时间
	CT_BOOL bIsUse;			//头衔是否使用
};

class CCUser;

class CCUserTitle : public CCUserData<CT_INT32, TitleData>
{
public:
	inline CT_VOID init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_DataMap.clear();
		m_lastSyncTime = time(0);
	}
	inline CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_DataMap.clear();
		m_lastSyncTime = 0;
	}

protected:
	/* 功能：内存数据与客户端协议数据的转换
	   @src: 内存数据结构，TitleData
	   @pDst：转换后的客户端协议数据，CMD_G_TitleData
	   返回值：pDst的长度
	 */
	virtual CT_INT32 transData(const TitleData& src, CT_CHAR* pDst)const;

public:
	virtual CT_VOID syncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE){}

	/* 功能：激活头衔（用户在线）
	 * @iTitleItemID: 激活头衔所需的道具ID
	 * 返回值：失败码
	 */
	CT_INT32 ActiveTitle(CT_INT32 iTitleItemID);	

	/* 功能：使用头衔
	 * @iTitleID：使用的头衔ID
	 * 返回值：失败码
	 */
	CT_INT32 UseTitle(CT_INT32 iTitleID);

	// 功能：更新头衔时限
	CT_VOID UpdataTitleStatus();

	/* 功能：得到使用中的头衔
	 * 返回值：头衔ID，没有返回0
	 */
	CT_INT32 GetUseTitleID();

	// 得到头衔列表
	CT_INT32 GetTitleList(CT_CHAR* pBuf, CT_INT32& iLen);
	// 更新头衔列表
	CT_VOID UpdataTitleList();

public:
	// 更新战斗力
	static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserTitle::DataMap &titleMap);
};

#endif