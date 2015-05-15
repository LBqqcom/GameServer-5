/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CFightingRecord.h
Version:		1.0
Description:    ս����¼
Author:			wangchen
Create Date:	2012-10-25
History:             

******************************************************************/

#ifndef CCFIGHTINGRECORD_H_
#define CCFIGHTINGRECORD_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CGDefine.h"
#include "CThreadMes.h"

class CCUser;
class CCMutex;
class CCFightingRecord : public CCSingleton<CCFightingRecord>
{
private:
	CCMutex					m_Mutex;
	MSC_Fighting_Info		m_FightingInfo[WORLD_SCENE_COUNT][SCENE_COUNT][DIFFICULTY];
	CT_INT32				m_Record[WORLD_SCENE_COUNT][SCENE_COUNT];
public:
	CCFightingRecord(void);
	~CCFightingRecord(void);

public:
	//���ü�¼
	CT_VOID	SetFightingRector(const MSC_Fighting_Info &FightingInfo,CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iDifficulty);
	//��ȡ��¼
	CT_BOOL GetFightingRector(MSC_Fighting_Info &FightingInfo,CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iDifficulty);
	//�����¼
	CT_VOID InsertRector(const MSC_Fighting_Info &FightingInfo,CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iDifficulty);

private:
	//��¼����
	CT_VOID SortFightingRector(CT_INT32 iWorldScene,CT_INT32 iScene,const MSC_Fighting_Info &FightingInfo);
};
#endif