/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCUserEnterEvent.h
Version:		1.0
Description:    �������
Author:			pengwen
Create Date:	2012-8-16
History:        ��������¼�   

******************************************************************/
#ifndef  CCUSERENTER_H_
#define  CCUSERENTER_H_
#include "CGameMesEvent.h"

class  CCNetInterface;
struct CMD_G_EnterGame;
class CCUserEnterEvent : public CCGameMesEvent
{
public:
	CCUserEnterEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCUserEnterEvent(void);

private:
	//�����û���Ϣ  ����2������Ϊ����
	virtual CT_VOID OnUserEvent(CT_DWORD mainID, CT_DWORD subID,
		                        CT_CHAR* pBuf,   CT_DWORD len, 
								CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID)
	{
    
	}
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{

	}
private:
	//�����½
	CT_VOID DisposeEnter( CCNetInterface* pNetHandle, CT_DWORD subID,  CT_CHAR* pBuf,  CT_DWORD len);
	//������¼��Ϣ
	CT_VOID UserEnter(CMD_G_EnterGame* pGeg, CCNetInterface* pNetHandle);


private:
	//���ݿ��¼��Ϣ
	CT_VOID DisposeDBLogin(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);

private:
	//��¼��֤��
	CT_VOID DisposeValidate(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);
	
private:
	//�����û���Ϣ
	CT_VOID SendUserData(CT_DWORD userID, CCNetInterface* pNetHandle);
	//
	CT_VOID Decompress(CT_INT32 *InBuffer,CT_INT32 iInLen,CT_INT32 *OutBuffer);

	//��ͨ�����
	CT_VOID DisposeActiveAdd(CT_CHAR* pBuf,  CT_DWORD len);
	//ת�̻���
	CT_VOID DisposeTrunTableAdd(CT_CHAR* pBuf,  CT_DWORD len);
	//�������
	CT_VOID DisposeWorshipAdd(CT_CHAR* pBuf,  CT_DWORD len);
};

#endif