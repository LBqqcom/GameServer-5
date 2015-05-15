/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		PVPFightData.h
Version:		1.0
Description:    从CUserPvpInfo.h中分离，防止循环依赖
Author:			pengwen
Create Date:	2013-11-7
History:       

******************************************************************/


#ifndef PVPINFO_H_
#define PVPINFO_H_

#include "CTType.h"

struct PVPInfo
{
	CT_DWORD		dwUserID;								//用户Id
	CT_DWORD		dwChallengUserID[PVP_CHALLENGE_COUNT];	//挑战用户
	CT_BYTE			bChallengUser[PVP_CHALLENGE_COUNT];		//挑战用户状态
	CT_BYTE			cbChestStatus;							//宝箱状态
	CT_INT32		iFightingCount;							//可挑战次数
	CT_INT64		lRefreshTime;							//刷新时间
	CT_INT64		lBeginTime;								//起始时间
	CT_INT32		iPVPDegree;								//竞技度
	CT_INT32		iPVPChestNum;							//领取宝箱次数
};



#endif