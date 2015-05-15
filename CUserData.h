/********************************************************************
	created:	2013/08/15
	created:	15:8:2013   11:26
	filename: 	CUserData.h
	file base:	CUserData
	file ext:	h
	author:		John
	
	purpose:	用户数据基类
*********************************************************************/
#ifndef CUSERDATA_H_
#define CUSERDATA_H_

#include <map>
#include "CMutex.h"
#include "CGlobeFunc.h"

class CCUser;

template <typename TKey, typename TData>
class CCUserData
{
    friend class CCUser;
public:
    typedef std::map<TKey, TData> DataMap;
    typedef typename std::map<TKey, TData>::iterator DataMap_IT;
    typedef typename std::map<TKey, TData>::const_iterator DataMap_CONSTIT;

protected:
    CCUser*  m_pUser;
    CCMutex* m_pMutex;
    DataMap m_DataMap;
    CT_INT64 m_lastSyncTime;         // 上次同步数据的时间

    /* 把服务端记录的数据转换为与客户端通讯的协议数据
     * 子类必须实现此接口
     * @src     服务端记录的数据
     * @pDst    转换结构存放的缓冲区地址
     * @return  转换后的结构大小
     */
    virtual CT_INT32 transData(const TData &src, CT_CHAR *pDst) const = 0;
public:
    CCUserData(void) {}
    ~CCUserData(void) {}
    /* 把服务器内存中的数据同步到数据库
     * @currTime    进行同步的时间，当前
     * @bForce      是否强制同步
     */
    virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE) { _AUTOMUTEX(m_pMutex); };
public:
    //初始化
    inline CT_VOID Init(CCUser* pUser, CCMutex* pMutex) { 
        m_pUser = pUser; 
        m_pMutex = pMutex; 
        m_DataMap.clear(); 
        m_lastSyncTime= time(0);
    }
    //清理用户数据
    inline CT_VOID Clear() { 
        _AUTOMUTEX(m_pMutex); 
        m_DataMap.clear(); 
        m_lastSyncTime= 0;
    }   
    // 插入数据
    inline CT_VOID InsertData(const TKey key, const TData &data);
    // 移除数据
    inline CT_VOID RemoveData(const TKey key);    
    inline CT_VOID RemoveData(const DataMap_IT it);
    // 判断是否有某个数据
    inline CT_BOOL HasData(const TKey key) const;
    // 获得某个数据 - 服务器结构
    inline CT_BOOL GetData(const TKey key, TData &data) const;  
    // 获得所有数据 - 客户端结构
    inline CT_BOOL GetAllData(CT_CHAR *pBuf, CT_INT32 &bufLen) const; 
    // 设置某个数据
    inline CT_VOID SetData(const DataMap_IT it, const TData &data);
    // 发送客户端数据
    inline CT_VOID SendClientData(const CT_DWORD mainID, const CT_DWORD subID, CT_CHAR *buf, CT_INT32 bufLen) const;
};

/* 
 * 模板类的方法实现要放到头文件内，不然编译器会找不到函数实现 
 */

template <typename TKey, typename TData>
CT_VOID CCUserData<TKey, TData>::InsertData( const TKey key, const TData &data )
{
    _AUTOMUTEX(m_pMutex);
    if (key != 0) {
        m_DataMap[key] = data;
    } else {
        //CCLogMeg::getSingleton().WriteError("InsertData::user %d has invaild KEY: key=%d", m_pUser->GetUserID(), key);
    }
}

template <typename TKey, typename TData>
CT_VOID CCUserData<TKey, TData>::RemoveData( const TKey key )
{
    _AUTOMUTEX(m_pMutex);
    m_DataMap.erase(key);
}

template <typename TKey, typename TData>
CT_VOID CCUserData<TKey, TData>::RemoveData( const DataMap_IT it )
{
    _AUTOMUTEX(m_pMutex);
    m_DataMap.erase(it);
}

template <typename TKey, typename TData>
CT_BOOL CCUserData<TKey, TData>::HasData( const TKey key ) const
{
    _AUTOMUTEX(m_pMutex);
    return m_DataMap.find(key) != m_DataMap.end();
}

template <typename TKey, typename TData>
CT_BOOL CCUserData<TKey, TData>::GetData( const TKey key, TData &data ) const
{
    _AUTOMUTEX(m_pMutex);
    DataMap_CONSTIT it = m_DataMap.find(key);
    if (it != m_DataMap.end())
    {
        data = it->second;
        return CT_TRUE;
    } 
    else
    {
        return CT_FALSE;
    }
}

template <typename TKey, typename TData>
CT_BOOL CCUserData<TKey, TData>::GetAllData( CT_CHAR *pBuf, CT_INT32 &bufLen ) const
{
    _AUTOMUTEX(m_pMutex);

    CT_CHAR *ptr = pBuf;
    for (DataMap_CONSTIT it = m_DataMap.begin(); it != m_DataMap.end(); ++it)
    {
        ptr += transData(it->second, ptr);
        if (ptr > pBuf + bufLen)
        {
            //CCLogMeg::getSingleton().WriteError("GetAllData::user %d uncompleted data type: %s", m_pUser->GetUserID(), typeid(TData).name());
            break;
        }
    }
    bufLen = ptr - pBuf;

    return CT_TRUE;
}

template <typename TKey, typename TData>
CT_VOID CCUserData<TKey, TData>::SetData( const DataMap_IT it, const TData &data )
{
    _AUTOMUTEX(m_pMutex);
    if (it != m_DataMap.end())
    {
        it->second = data;
    }
    else
    {
        //CCLogMeg::getSingleton().WriteError("SetData:: user %d has invaild iterator", m_pUser->GetUserID());
    }
}
#if 1
template <typename TKey, typename TData>
CT_VOID CCUserData<TKey, TData>::SendClientData( const CT_DWORD mainID, const CT_DWORD subID, CT_CHAR *buf, CT_INT32 bufLen ) const
{
    _AUTOMUTEX(m_pMutex);
    memset(buf, 0, bufLen);
    if (GetAllData(buf, bufLen))
    {
        G_SendClientData(*m_pUser, mainID, subID, buf, bufLen);
        //m_pUser->GetNetInterface()->Send(mainID, subID, buf, bufLen);
    }
}
#endif
#endif