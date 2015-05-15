/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CTeamDuplicate.h
Version:		1.0
Description:    组队副本
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

//房间信息
struct TeamDuplicateRoom
{
	CT_INT32		iRoomID;		//房间id
	CT_DWORD		dwUserID;		//用户id
	CT_INT32		iCount;			//人数
	CT_INT32		iSceneID;		//副本id
	CT_INT32		iDifficulty;	//难度
	CT_CHAR			cName[NAME_SIZE];//房主名称
};

//房间信息
struct TeamDuplicateRoomInfo
{
	CT_INT32			iRoomID;					//房间id
	CT_DWORD			dwUserID;					//房主ID
	CT_INT32			iSceneID;					//场景ID
	CT_INT32			iDifficulty;				//难度
	CT_INT32			iStatus[TEAM_NUM];			//玩家状态
	CT_INT32			iFightingTime[TEAM_NUM];	//战斗次数
	CT_INT32			iAutoMatch[TEAM_NUM];		//自动匹配
	UserFightingStatus	FightingData[TEAM_NUM];		//战斗数据
	LookUserInfo			AutoMatchUser[TEAM_NUM];	//自动匹配用户信息
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
	RoomIDList				m_RoomIDList;			//房间ID
	RoomMap					m_RoomMap;				//房间列表
	RoomInfoMap				m_RoomInfoMap;			//房间信息
	UserRoomMap				m_UserRoomMap;			//用户所在房间信息(不记录自动匹配用户)
	SceneMap				m_SceneMap;				//场景房间ID
public:
	CCTeamDuplicate(void);
	~CCTeamDuplicate(void);


public:
	//创建房间
	CT_INT32 CreateRoom(TeamDuplicateRoomInfo &RoomInfo);
	//加入房间
	CT_INT32 AddRoom(CT_INT32 iRoomID,CT_INT32 iFightingTime,UserFightingStatus &UserData);
	//快速加入
	CT_INT32 AddFastRoom(CT_INT32 iFightingTime,UserFightingStatus &UserData,CT_INT32 iSceneID,CT_INT32 iDifficutly);
	//阵位调整
	CT_INT32 ChangePos(CT_INT32 iRoomID,CT_DWORD dwUserID,CT_DWORD dwPosID[TEAM_NUM]);
	//房间信息调整
	CT_INT32 ChangeRoomInfo(CT_INT32 iRoomID,CT_DWORD dwUserID,CT_INT32 iSceneID,CT_INT32 iDifficutly);
	//玩家离开
	CT_INT32 LeaveRoom(CT_INT32 iRoomID,CT_DWORD dwUserID);
	//设置状态
	CT_INT32 SetUserStatus(CT_INT32 iRoomID,CT_DWORD dwUserID);
	//清空状态
	CT_VOID ClearUserStatus(CT_INT32 iRoomID);
	//自动匹配
	CT_INT32 AutoMatch(CT_INT32 iRoomID,UserFightingStatus &FightingData,LookUserInfo &Userdata);
	//房间解散
	CT_VOID ClearRoom(CT_INT32 iRoomID);
	//删除BOSS
	CT_VOID DeleteBoss(const CT_INT32 iRoomID);

public:
	//向房间每个人发消息
	CT_VOID SendRoomMessage(CT_INT32 iRoomID,CT_INT32 iMainID,CT_INT32 iSubID,CT_CHAR *pBuf,CT_INT32 iLen);
	//向房间每个人发消息
	CT_VOID SendRoomMessage(CT_INT32 iRoomID,CT_INT32 iMainID,CT_INT32 iSubID);
	//发送房间列表
	CT_VOID SendRoomList();

public:
	//获取房间信息 任何人不得改动这个函数的实现（死锁相关） 彭文
	CT_BOOL GetRoomInfo(CT_INT32 iRoomID,TeamDuplicateRoomInfo &RoomInfo);
	//获取房间信息 任何人不得改动这个函数的实现（死锁相关） 彭文
	CT_BOOL GetRoomInfo(CT_DWORD dwUserID,TeamDuplicateRoomInfo &RoomInfo);
	//获取房间列表
	CT_VOID GetRoomList(CT_CHAR *pBuf,CT_INT32 &iLen);
};

#endif
