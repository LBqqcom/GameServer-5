/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserDuplicate.h
Version:		1.0
Description:    �û���Ӹ���
Author:			wangchen
Create Date:	2012-10-19
History:        �û�����    

******************************************************************/
#ifndef CCUSERTEAMDUPLICATE_H_
#define CCUSERTEAMDUPLICATE_H_
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include<map>

class CCUser;
class CCMutex;

//��Ӹ���
struct TeamDuplicateScene
{
	CT_INT32		iSceneID;
	CT_INT32		iDifficulty[TEAMDUPLICATE_DIFFICULTY];
	CT_INT32		iWin[TEAMDUPLICATE_DIFFICULTY];	
};

class CCUserTeamDuplicate
{
private:
	typedef std::map<CT_INT32,TeamDuplicateScene>				TeamDuplicateMap;
	typedef std::map<CT_INT32,TeamDuplicateScene>::iterator		TD_IT;

private: 
	CT_INT32			m_iFightingTime;			//ս������
	TeamDuplicateMap	m_TeamDuplicateMap;			//��Ӹ�������

private:
	CCUser*  m_pUser;
	CCMutex* m_pMutex;
public:
	CCUserTeamDuplicate(void);
	~CCUserTeamDuplicate(void);

public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();

public: 
	//��������
	CT_VOID InsertSceneData(const CT_INT32 iSceneID,const CT_INT32 iDifficulty,CT_INT32 iWin);


public:
	//���ó���
	CT_BOOL SetSceneData(CT_INT32 iSceneID,CT_INT32 iDifficulty);

public:
	//��ȡ������Ϣ
	CT_VOID GetSceneData(CT_CHAR *pBuf,CT_INT32 &iLen);

public:
	//��֤�ؿ�
	CT_BOOL VaildateScene(CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_BOOL bUpdata=CT_FALSE);
	//���³�����Ϣ
	CT_VOID UpdataScene(CT_INT32 iSceneID,CT_INT32 iDifficulty,CT_INT32 iWin);
	//���½�������
	CT_VOID UpdataFightingReward();
};

#endif