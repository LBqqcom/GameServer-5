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
	CT_INT32 m_VipGrade;		//vip�ȼ�
	CT_INT64 m_llExpireTime;  //vip����ʱ��
	CT_INT32 m_Gift;		//����Ƿ���ȡ
public:
	CT_VOID Init(CCUser *pUser,CCMutex *pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_VipGrade=0;
		m_llExpireTime=0;
		m_Gift=0;
	}
	//�����û�����
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_VipGrade=0;
		m_llExpireTime=0;
		m_Gift=0;
	}
public:
	//vip����
	CT_VOID VipRenewal(CT_INT32 iGrade,CT_INT32 iTime,CT_INT32 iPropID,CT_INT32 iDiamod);
	//vip����
	CT_VOID VipTimeMonitor();
	//��ȡvip�ȼ�
	CT_INT32 VipGradeGet();
	//��ȡvip���״̬
	CT_INT32 VipGiftStatusGet();
	//��ȡvip���
	CT_VOID VipGiftSend();
	//�ж��û��Ƿ��и���Ȩ
	CT_BOOL VipHasPrivilege(int iPriID);    
    //�ж��û��Ƿ��и�������Ȩ
    CT_BOOL VipHasPrivilegeType(int iPriType);
	//����vip��Ϣ
	CT_VOID	VipGetDBInfo(CT_INT32 iGrade,CT_INT32 iTime,CT_INT32 iGift);
	//����vip��Ϣ
	CT_VOID VipSendInfo();
	//��ȡvip�������Ȩ����
	CT_INT32 VipPriTimeGet(CT_INT32 iPriType);   
	//��ȡvip���Լӳ�
	CT_BOOL	VipRoleAddGet(G_VipRoleAdd *vipRoleAdd);	
	//VIP�������
	CT_VOID VipGiftPush();
	//���VIP����ʱ��
	CT_INT64 GetVIPExpireTime(){_AUTOMUTEX(m_pMutex);return m_llExpireTime;};
public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CT_INT32 iGrade);
};

#endif