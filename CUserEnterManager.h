/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		UserEnterInfo.h
Version:		1.0
Description:    �û��������
Author:			pengwen
Create Date:	2012-8-20
History:        �������ķ������������û���¼��Ϣ     

******************************************************************/

#ifndef CCUSERENTERMANGER_H_
#define CCUSERENTERMANGER_H_
#include <map>
#include "CSingleton.h"
#include "CTType.h"
#include "CMutex.h"

//�û����������Ϣ
struct UserEnterInfo
{
	CT_DWORD            iID;				//�û�id
	CT_BYTE				cbValidate[VALIDATE];	 //��֤��Ϣ
	CT_LONGLONG         llEnterTime;        //�û���¼ʱ��
	CT_BOOL				bTalk;				//����
	CT_INT64			lExitTime;        //�û��˳�ʱ��
};

class CCUserEnterManager : public CCSingleton<CCUserEnterManager>
{
	CCMutex  m_Mutex;
public:
	typedef std::map<CT_DWORD, UserEnterInfo>			UserEnterInfoMap;
	typedef std::map<CT_DWORD, UserEnterInfo>::iterator UEIM_IT;
private:
	//�û�������Ϣ��,��Щ��Ϣ�Ǵ����ķ�����������
	UserEnterInfoMap	m_userEnterInfoMap;	

	//
	CT_INT32			m_iTime;

public:
	CCUserEnterManager(CT_VOID);
	~CCUserEnterManager(CT_VOID);

public:
	//��������
	CT_VOID Insert(const UserEnterInfo& us);
	//��������
	CT_BOOL Find(CT_DWORD id);
	CT_BOOL	Find(CT_DWORD id,UserEnterInfo& us);
	//�������
	CT_VOID Erase(CT_DWORD id);
	//���
	CT_VOID Clear()
	{
		m_userEnterInfoMap.clear();
	}
	//��֤����,�Ƿ�����ڶ�����
	CT_BOOL Validate(const CMD_G_EnterGame& eg);
	//ɾ����֤
	CT_VOID DeleteValidate();
	//�����˳�ʱ��
	CT_VOID SetExitTime(const CT_DWORD dwUserID);
	//�˳���ʱɾ����֤
	CT_VOID ExitDeleteValidate();


};
#endif