/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CDBFightTeam.h
Version:		1.0
Description:    战队数据库
Author:			zhoujun
Create Date:	2013-05-05
History:        处理DB战队事件   

******************************************************************/
#ifndef CCDBFIGHTTEAM_H_
#define CCDBFIGHTTEAM_H_

#include "CGameMesEvent.h"
class CCDBConnector;

class CCDBFightTeam:public CCGameMesEvent
{
private:
	CCDBConnector& m_dbConnector;
public:
	CCDBFightTeam(CCGameMesManager* pMesManager, CT_DWORD id, CCDBConnector& dbConnector);
	~CCDBFightTeam(void);

private:
	//处理用户消息
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,	CT_DWORD subID,
		CT_CHAR* pBuf,		CT_DWORD len,
		CT_LONGLONG reserve);

	//用户定时器处理
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//用户时钟处理
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}

private:
	CT_VOID DisposeFightTeamUpdate(CT_CHAR* pBuf,CT_DWORD len);	//保存战队

	CT_VOID DisposeFightTeamDelete(CT_CHAR* pBuf,CT_DWORD len);	//解散战队

	CT_VOID DisposeFightTeamFind(CT_CHAR* pBuf,CT_DWORD len);	//查找战队

	CT_VOID DisposeFightTeamMemberUpdate(CT_CHAR* pBuf,CT_DWORD len);	//保存战队成员

	CT_VOID DisposeFightTeamMemberDelete(CT_CHAR* pBuf,CT_DWORD len);	//删除战队成员

	CT_VOID DisposeFightTeamMemberFind(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeFightTeamRecruitUpdate(CT_CHAR* pBuf,CT_DWORD len);	//招募

	CT_VOID DisposeFightTeamRecruitDelete(CT_CHAR* pBuf,CT_DWORD len);	//删除招募

	CT_VOID DisposeFightTeamEventSave(CT_CHAR* pBuf,CT_DWORD len);

	CT_VOID DisposeFightTeamScienceUpGrade(CT_CHAR* pBuf,CT_DWORD len); //科技升级

	CT_VOID DisposeFightTeamSummonInfo(CT_CHAR *pBuf,CT_DWORD len);		//召唤信息

	CT_VOID DisposeFightTeamSummonItem(CT_CHAR *pBuf,CT_DWORD len);		//召唤道具

	CT_VOID DisposeFightTeamDownGrade(CT_CHAR *pBuf,CT_DWORD len);		//战队科技惩罚
	//威能升级
	CT_VOID DisposeCSInsporeUp(CT_CHAR *pBuf,CT_INT32 iLen);
	//读取威能
	CT_VOID DisopseCSReadInspore(CT_CHAR *pBuf,CT_INT32 iLen);
	//清理时限boss
	CT_VOID DisposeClearTimeBoss(CT_CHAR *pBuf,CT_INT32 iLen);
	//时限boss信息
	CT_VOID DisposeTimeBossInfo(CT_CHAR *pBuf,CT_INT32 iLen);
	//时限boss成员
	CT_VOID DisposeTimeBossMumber(CT_CHAR *pBuf,CT_INT32 iLen);

	CT_VOID GetFightTeamDB();		// 程序启动20s后从数据读入初始化数据
};
#endif