/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		PVPFightData.h
Version:		1.0
Description:    ��CUserPvpInfo.h�з��룬��ֹѭ������
Author:			pengwen
Create Date:	2013-11-7
History:       

******************************************************************/


#ifndef PVPINFO_H_
#define PVPINFO_H_

#include "CTType.h"

struct PVPInfo
{
	CT_DWORD		dwUserID;								//�û�Id
	CT_DWORD		dwChallengUserID[PVP_CHALLENGE_COUNT];	//��ս�û�
	CT_BYTE			bChallengUser[PVP_CHALLENGE_COUNT];		//��ս�û�״̬
	CT_BYTE			cbChestStatus;							//����״̬
	CT_INT32		iFightingCount;							//����ս����
	CT_INT64		lRefreshTime;							//ˢ��ʱ��
	CT_INT64		lBeginTime;								//��ʼʱ��
	CT_INT32		iPVPDegree;								//������
	CT_INT32		iPVPChestNum;							//��ȡ�������
};



#endif