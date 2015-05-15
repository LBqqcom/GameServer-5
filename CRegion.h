#ifndef CREGION_H_
#define CREGION_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"

class CCUserRegion
{
	friend class CCUser;
private:
	CCUser*             m_pUser; 
	CCMutex*			m_pMutex;

	typedef std::vector<CT_INT32>						UnlockRegionVector;		//�ѽ�������
	typedef	std::vector<CT_INT32>::iterator				UR_IT;

	UnlockRegionVector	m_regionStatusVec;		//�ѽ�������

public:
	CCUserRegion();
	~CCUserRegion();
public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_regionStatusVec.clear();
		m_pUser = pUser;
		m_pMutex = pMutex;
	}

	//�����û�����
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_regionStatusVec.clear();
	}
public:
	//�鿴�����Ƿ��ѽ���
	CT_BOOL	GetRegionStatus(CT_INT32 rid);
	//�鿴�����Ƿ����
	CT_BOOL	GetRegionExist(CT_INT32 rid,CT_INT32 iRegionList[]);
	//�鿴�����ڽ������Ƿ����
	CT_BOOL	GetRegionLink(CT_INT32 iRegionLink[]);
	//�鿴�����ȼ��Ƿ�ﵽ�����������
	CT_BOOL	GetRegionMasterLevel(CT_INT32 rid);	
	//���ӽ�������
	CT_VOID InsertRegionUnlock(const CT_INT32 &rid){_AUTOMUTEX(m_pMutex); m_regionStatusVec.push_back(rid);}
	//��ȡ�ѽ�������
	CT_BOOL GetUnlockRegionData(CT_CHAR *pBuf,CT_INT32 &bufLen);
};

#endif