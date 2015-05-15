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

	typedef std::vector<CT_INT32>						UnlockRegionVector;		//已解锁区域
	typedef	std::vector<CT_INT32>::iterator				UR_IT;

	UnlockRegionVector	m_regionStatusVec;		//已解锁区域

public:
	CCUserRegion();
	~CCUserRegion();
public:
	//初始化
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex)
	{
		m_regionStatusVec.clear();
		m_pUser = pUser;
		m_pMutex = pMutex;
	}

	//清理用户数据
	CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_regionStatusVec.clear();
	}
public:
	//查看区域是否已解锁
	CT_BOOL	GetRegionStatus(CT_INT32 rid);
	//查看区域是否存在
	CT_BOOL	GetRegionExist(CT_INT32 rid,CT_INT32 iRegionList[]);
	//查看区域邻接区域是否解锁
	CT_BOOL	GetRegionLink(CT_INT32 iRegionLink[]);
	//查看领主等级是否达到区域解锁条件
	CT_BOOL	GetRegionMasterLevel(CT_INT32 rid);	
	//增加解锁区域
	CT_VOID InsertRegionUnlock(const CT_INT32 &rid){_AUTOMUTEX(m_pMutex); m_regionStatusVec.push_back(rid);}
	//获取已解锁区域
	CT_BOOL GetUnlockRegionData(CT_CHAR *pBuf,CT_INT32 &bufLen);
};

#endif