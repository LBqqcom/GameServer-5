#ifndef CVIP_H_
#define CVIP_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

class CCUserVip
{
	friend class CCUser;
public:
	CCUserVip();
	~CCUserVip();
private:
	CCUser	*m_pUser;
	CCMutex	*m_pMutex;
	CT_INT32 m_VipGrade;		//vip等级
	CT_INT64 m_llExpireTime;  //vip到期时间
	CT_INT32 m_Gift;		//礼包是否领取
public:
	CT_VOID Init(CCUser *pUser,CCMutex *pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_VipGrade=0;
		m_llExpireTime=0;
		m_Gift=0;
	}
	//清理用户数据
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_VipGrade=0;
		m_llExpireTime=0;
		m_Gift=0;
	}
public:
	//vip续费
	CT_VOID VipRenewal(CT_INT32 iGrade,CT_INT32 iTime,CT_INT32 iPropID,CT_INT32 iDiamod);
	//vip到期
	CT_VOID VipTimeMonitor();
	//获取vip等级
	CT_INT32 VipGradeGet();
	//获取vip礼包状态
	CT_INT32 VipGiftStatusGet();
	//领取vip礼包
	CT_VOID VipGiftSend();
	//判断用户是否有该特权
	CT_BOOL VipHasPrivilege(int iPriID);    
    //判断用户是否有该类型特权
    CT_BOOL VipHasPrivilegeType(int iPriType);
	//设置vip信息
	CT_VOID	VipGetDBInfo(CT_INT32 iGrade,CT_INT32 iTime,CT_INT32 iGift);
	//发送vip信息
	CT_VOID VipSendInfo();
	//获取vip额外的特权次数
	CT_INT32 VipPriTimeGet(CT_INT32 iPriType);   
	//获取vip属性加成
	CT_BOOL	VipRoleAddGet(G_VipRoleAdd *vipRoleAdd);	
	//VIP礼包推送
	CT_VOID VipGiftPush();
	//获得VIP到期时间
	CT_INT64 GetVIPExpireTime(){_AUTOMUTEX(m_pMutex);return m_llExpireTime;};
public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iGrade);
};

#endif