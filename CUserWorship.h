#ifndef CUserWorship_H_
#define CUserWorship_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

struct WorshipStatus
{
	CT_INT32	iXP;			//�Ѽ��뾭��
	CT_INT32	iStatus;		//�Ƿ����
};

class CCUserWorship
{
	friend class CCUser;
private:
	CCUser	*m_pUser;
	CCMutex *m_pMutex;

	typedef  std::map<CT_INT32,WorshipStatus>			WorshipStatusMap;
	typedef  std::map<CT_INT32,WorshipStatus>::iterator			WorshipStatusMap_IT;

	WorshipStatusMap		m_WorshipStatusMap;
public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_WorshipStatusMap.clear();
	}

	//�����û�����
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_WorshipStatusMap.clear();
	}
public:
	CCUserWorship();
	~CCUserWorship();
public:
	CT_BOOL			CheckAbilityToWorship(CT_INT32 iType);     //���ĳ���͵ļ����Ƿ���Խ���
	CT_VOID			UpdateWorshipStatus(CT_INT32 iType,CT_INT32 iXp);  //����׼������Ӿ���
	CT_BOOL			CheckWorshipComplete(CT_INT32 iType);		//�������Ƿ����
	CT_VOID			WorshipGetReward(CT_INT32 iType);			//�����콱
	CT_VOID			GetWorshipByMasterLevel(CT_INT32 iLevel);	//���������ȼ�����ȡ�����ļ���
	CT_VOID			GetWorshipStatus(CT_INT32 iType,CT_INT32 iStatus,CT_INT32 iXp);  //�û���¼��ȡ������Ϣ
	CT_VOID			SendUserWorshipInfo();		//�����û�������Ϣ
	CT_BOOL			CheckWorshipCount(CT_INT32 iType);	//zhoujun	�����ʯ�������
	CT_INT32		GetWorshipCount(CT_INT32 iType);
	CT_VOID			WorshipCount(CT_INT32 iType);
	CT_VOID			UpdateWorshipData();
};

#endif