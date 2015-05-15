/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicate.h
Version:		1.0
Description:    ��Ӹ���
Author:			wangchen
Create Date:	2013-5-9
History:             

******************************************************************/

#ifndef CCDUPLICATEROOM_H_
#define CCDUPLICATEROOM_H_
#include "CSingleton.h"
#include "CTType.h"
#include "GlobeScriptInterface.h"
#include "CGDefine.h"
#include "CThreadMes.h"

//������Ϣ
struct TeamDuplicateRoom
{
	CT_INT32		iRoomID;		//����id
	CT_DWORD		dwUserID;		//�û�id
	CT_INT32		iCount;			//����
	CT_INT32		iSceneID;		//����id
	CT_INT32		iDifficulty;	//�Ѷ�
	CT_CHAR			cName[NAME_SIZE];//��������
};

//������Ϣ
struct TeamDuplicateRoomInfo
{
	CT_INT32			iRoomID;					//����id
	CT_DWORD			dwUserID;					//����ID
	CT_INT32			iSceneID;					//����ID
	CT_INT32			iDifficulty;				//�Ѷ�
	CT_INT32			iStatus[TEAM_NUM];			//���״̬
	CT_INT32			iFightingTime[TEAM_NUM];	//ս������
	CT_INT32			iAutoMatch[TEAM_NUM];		//�Զ�ƥ��
	UserFightingStatus	FightingData[TEAM_NUM];		//ս������
	LookUserInfo			AutoMatchUser[TEAM_NUM];	//�Զ�ƥ���û���Ϣ
};

#define MAX_BUFFER			1024*128

class CCMutex;
class CCTeamDuplicate : public CCSingleton<CCTeamDuplicate>
{
private:
	typedef std::list<CT_INT32>									RoomIDList;
	typedef std::list<CT_INT32>::iterator						RID_IT;
	typedef std::map<CT_INT32,TeamDuplicateRoom>				RoomMap;
	typedef std::map<CT_INT32,TeamDuplicateRoom>::iterator		RM_IT;
	typedef std::map<CT_INT32,TeamDuplicateRoomInfo>			RoomInfoMap;
	typedef std::map<CT_INT32,TeamDuplicateRoomInfo>::iterator	RI_IT;
	typedef std::map<CT_DWORD,CT_INT32>							UserRoomMap;
	typedef std::map<CT_DWORD,CT_INT32>::iterator				UR_IT;
	typedef std::map<CT_INT32,RoomIDList>						SceneMap;
	typedef std::map<CT_INT32,RoomIDList>::iterator				SM_IT;

private: 
	CCMutex					m_Mutex;
private:
	RoomIDList				m_RoomIDList;			//����ID
	RoomMap					m_RoomMap;				//�����б�
	RoomInfoMap				m_RoomInfoMap;			//������Ϣ
	UserRoomMap				m_UserRoomMap;			//�û����ڷ�����Ϣ(����¼�Զ�ƥ���û�)
	SceneMap				m_SceneMap;				//��������ID
public:
	CCTeamDuplicate(void);
	~CCTeamDuplicate(void);


public:
	//��������
	CT_INT32 CreateRoom(TeamDuplicateRoomInfo &RoomInfo);
	//���뷿��
	CT_INT32 AddRoom(CT_INT32 iRoomID,CT_INT32 iFightingTime,UserFightingStatus &UserData);
	//���ټ���
	CT_INT32 AddFastRoom(CT_INT32 iFightingTime,UserFightingStatus &UserData,CT_INT32 iSceneID,CT_INT32 iDifficutly);
	//��λ����
	CT_INT32 ChangePos(CT_INT32 iRoomID,CT_DWORD dwUserID,CT_DWORD dwPosID[TEAM_NUM]);
	//������Ϣ����
	CT_INT32 ChangeRoomInfo(CT_INT32 iRoomID,CT_DWORD dwUserID,CT_INT32 iSceneID,CT_INT32 iDifficutly);
	//����뿪
	CT_INT32 LeaveRoom(CT_INT32 iRoomID,CT_DWORD dwUserID);
	//����״̬
	CT_INT32 SetUserStatus(CT_INT32 iRoomID,CT_DWORD dwUserID);
	//���״̬
	CT_VOID ClearUserStatus(CT_INT32 iRoomID);
	//�Զ�ƥ��
	CT_INT32 AutoMatch(CT_INT32 iRoomID,UserFightingStatus &FightingData,LookUserInfo &Userdata);
	//�����ɢ
	CT_VOID ClearRoom(CT_INT32 iRoomID);
	//ɾ��BOSS
	CT_VOID DeleteBoss(const CT_INT32 iRoomID);

public:
	//�򷿼�ÿ���˷���Ϣ
	CT_VOID SendRoomMessage(CT_INT32 iRoomID,CT_INT32 iMainID,CT_INT32 iSubID,CT_CHAR *pBuf,CT_INT32 iLen);
	//�򷿼�ÿ���˷���Ϣ
	CT_VOID SendRoomMessage(CT_INT32 iRoomID,CT_INT32 iMainID,CT_INT32 iSubID);
	//���ͷ����б�
	CT_VOID SendRoomList();

public:
	//��ȡ������Ϣ �κ��˲��øĶ����������ʵ�֣�������أ� ����
	CT_BOOL GetRoomInfo(CT_INT32 iRoomID,TeamDuplicateRoomInfo &RoomInfo);
	//��ȡ������Ϣ �κ��˲��øĶ����������ʵ�֣�������أ� ����
	CT_BOOL GetRoomInfo(CT_DWORD dwUserID,TeamDuplicateRoomInfo &RoomInfo);
	//��ȡ�����б�
	CT_VOID GetRoomList(CT_CHAR *pBuf,CT_INT32 &iLen);
};

#endif
