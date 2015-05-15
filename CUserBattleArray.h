/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    �û�������
Author:			wangchen
Create Date:	2012-10-19
History:        �û��� 

******************************************************************/
#ifndef CCUSERBATTLEARRAY_H_
#define CCUSERBATTLEARRAY_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserBase.h"

////����Ϣ
//struct MSC_DB_BattleArray
//{
//	CT_DWORD	dwUserID;				//�û�ID
//	CT_INT32	iPosition[POST_COUNT/2];//λ��
//};

class CCUser;

class CCUserBattleArray
{
    friend class CCUser;
private:
	CT_INT32	m_iPosition[POST_COUNT/2];		//��λ��

private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;

public:
	CCUserBattleArray(void);
	~CCUserBattleArray(void);

public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();
public:
	//������
	CT_VOID InsertBufferData(CT_INT32 iPosition[POST_COUNT/2]);

public:
	//��ȡ��
	CT_VOID GetBattleArray(CT_INT32 iPosition[POST_COUNT/2]);
	CT_BOOL GetBattleArray(CT_CHAR *pBuf,CT_INT32 &iLen);

public:
	//������
	CT_VOID	UpDataBattleArray(CT_INT32 iPosition[POST_COUNT/2]);

    // �����󷨼ӳ�����
    CT_VOID CalcArrayStatus(CT_INT32 iPosition);
	
	//�жϱ����Ƿ�����
	CT_BOOL CheckArmyArray(CT_INT32 iArmyType);

public:
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, CT_INT32 iClass, CT_INT32 *arrayBattle, CT_INT32 nCount);
};

#endif
