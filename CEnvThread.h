/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CEnvThread.h
Version:		1.0
Description:    �¼�����
Author:			pengwen
Create Date:	2012-8-8
History:        �����û��¼�     

******************************************************************/

#ifndef CENVTREAD_H_
#define CENVTREAD_H_
#include "CGateThread.h"
#include "CGameMesEvent.h"

struct CMD_G_EnterGame;
struct MSC_DB_R_UserInfo;


//�¼�����
class CCEnvThread :public CCGateThread
{
private:

public:
	CCEnvThread(CT_VOID);
	~CCEnvThread(CT_VOID);

public:
	//��
	virtual CT_VOID OnOpen(); 
	
	//�����Ϣ
public:
	//��ʱ��
	virtual CT_VOID OnTimer(CT_DWORD timeID); 

	//�������ӽ���
	virtual CT_VOID OnNetEventOpen(CCNetInterface* pNetHandle) ;
	//�������ݵ���
	virtual CT_VOID OnNetEventRead(CCNetInterface* pNetHandle, CT_DWORD mainID, CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len) ;
	//����ر�
	virtual CT_VOID OnNetEventClose(CCNetInterface* pNetHandle) ;
	//�û��Զ�����������
	virtual CT_VOID OnNetEventReadRow(CCNetInterface* pNetHandle, CT_CHAR* pBuf,  CT_DWORD len);


};
#endif