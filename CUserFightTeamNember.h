/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CUserFightTeamNember.h
Version:		1.0
Description:    �û�ս������
Author:			wangchen
Create Date:	2012-10-19
History:        ս��    

******************************************************************/
#ifndef CCUSERFIGHTTRAMNEMBER_H_
#define CCUSERFIGHTTRAMNEMBER_H_
#include <map>
#include "CGDefine.h"
#include "CMutex.h"
#include "CMD_Game.h"
#include "CUserData.h"

//��Ա�Ƽ�
struct NemberScience
{
	CT_DWORD	dwUserID;						//�û�ID
	CT_INT32	iScienceID;						//�Ƽ�ID
	CT_INT32	iScienceGrade;					//�Ƽ��ȼ�
};

typedef std::map<CT_INT32,NemberScience>	ScienceInfoMap;		//�Ƽ���Ϣ

class CCUser;
struct MasterCombatStatus;
class CCUserFightTeamNember:public CCUserData<CT_INT32, NemberScience>
{
public:
	CCUserFightTeamNember(void);
	~CCUserFightTeamNember(void);

protected:
	virtual CT_INT32 transData(const NemberScience &src, CT_CHAR *pDst) const;
public:
	virtual CT_VOID SyncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE){};

public:
	//��ʼ��
	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();

public:
	//�����Ա�Ƽ�
	CT_VOID	InsertMemberScience(NemberScience & Ftns);	
	//�������ʱ��
	CT_VOID InsertLeaveTeamTime(CT_INT64 lTime);

public:
	//��ȡ��Ա�Ƽ�
	CT_BOOL	GetNemberScience(CT_CHAR *pBuf,CT_INT32 &iLen);	
	//��ȡ�Ƽ���Ϣ
	CT_BOOL GetScienceInfo(CT_INT32 iScienceID,NemberScience &ScienceInfo);
	//��ȡ�ӳ�Ч��
	CT_VOID GetAddAffect(CT_DWORD dwFTID,CT_INT32 iOccupation,MasterCombatStatus &mcs);

private:
	//���ͼӳ�
	CT_VOID AddFightingType(CT_INT32 iType,MasterCombatStatus &mcs,G_FightTeamBless &ftb);

public:
	//�Ƽ�����
	CT_INT32 ScienceUpGrade(CT_INT32 iScienceID,CT_INT32 iItemCount);
	//�Ƽ�����
	CT_VOID ScienceDownGrade();

public:
	//�Ƽ������ӳ�Ч��
	CT_INT32 ScienceBulidAdd(const CT_DWORD dwTeamID,const CT_INT32 iProduct,const CT_INT32 iBuildType);
    //ս�����ӳ�
    static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs,CT_DWORD dwTeamID, CT_DWORD dwUserID,const CT_INT32 arrayTargets[], const CT_INT32 nTarget, const ScienceInfoMap &scienceMap); 
};

#endif
