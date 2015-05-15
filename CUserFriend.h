#ifndef CFriend_H_
#define CFriend_H_

#include <map>
#include <vector>
#include "CUser.h"
#include "CMD_Game.h"
#include "GlobeScriptInterface.h"

// struct FriendData
// {
// 	CT_DWORD	dwUserID;					// id
// 	CT_DWORD	dwF_UID;					// ����ID
// 	CT_CHAR		cF_NickName[NAME_SIZE];		// �ǳ�
// 	CT_INT32	iF_Occupation;				// ְҵ
// 	CT_INT32	iF_Grade;					// �ȼ�
// 	CT_INT32	iF_Status;					// ����״̬
// 	CT_INT32	iF_FightingPower;			// ս����(����״̬������ս����)
// 	CT_INT64	iLastExitTime;				// �������ʱ��
// };

typedef CMD_G_FriendData FriendData;

enum
{
	FRIEND_REQ			=	(1),		//��������
	FRIEND_DATA			=	(2),		//����
	FRIEND_BLACKNAME	=	(3),		//������
	FRIEND_BLACKREV		=	(4),		//���������
};
 


class CCUserFriend
{
private:
	CCMutex*    m_pMutex;
	CCUser*     m_pUser;
	typedef std::map<CT_DWORD, FriendData>					FriendDataMap;			// ������������
	typedef std::map<CT_DWORD, FriendData>::iterator		FD_IT;
	FriendDataMap											m_FriendDataMap;		// �����б�
	FriendDataMap											m_FriendReqMap;			// ���������б�
	FriendDataMap											m_BlackNameDataMap;		// �������б�
	std::set<CT_INT32>										m_BlackNameRev;			// ���������	�Լ��������û��������б�
	CT_BOOL													m_bEnable;

	std::map<CT_DWORD, CMD_G_Friend_RSP>					m_FriendRSP;
	CT_INT32												m_RSPCount;
public:
	CCUserFriend();
	~CCUserFriend(void);

	CT_VOID Init(CCUser* pUser, CCMutex* pMutex);
	//�����û�����
	CT_VOID Clear();

	CT_BOOL IsEnable() {_AUTOMUTEX(m_pMutex);   return m_bEnable;}
	
	CT_BOOL GetFriendData(CT_CHAR *pBuf, CT_INT32 &bufLen);			// �õ�������Ϣ
	
	CT_VOID InsertFriendData(const CMD_G_FriendData &data);			// �����������
	
	CT_BOOL GetFriendReq(CT_CHAR *pBuf, CT_INT32 &bufLen);			// �õ�������������
	
	CT_VOID InsertFriendReq(const CMD_G_FriendData &data);			// ���������������

	CT_BOOL GetBlackName(CT_CHAR *pBuf, CT_INT32 &bufLen);			// �õ�����������
	
	CT_VOID InsertBlackName(const CMD_G_FriendData &data);			// �������������

	CT_VOID InsertBlackRev(const CT_INT32 iUID);					// ���뷴�������

	CT_VOID DeleteFriend(CT_INT32 iF_UID, CT_INT32 iType);			// ɾ������/����/������(�ڴ�)

	CT_BOOL CheckBlackList(CT_INT32 iF_UID);						// ����û��������б�

	CT_BOOL CheckIsFriend(CT_INT32 iF_UID);							// ����Ƿ��Ѿ����Լ��ĺ���

	CT_BOOL GetFriendList(CT_DWORD *dwFList, CT_INT32 *iGradeList, CT_INT64 *iExitTimeList, CT_INT32 &iCount);		// 

	CT_BOOL CheckFriendLimit(CT_INT32 iType);						// ������/����������

	CT_BOOL GetFriendReq(CT_DWORD iF_UID, CMD_G_FriendData **pFriendData);			// �õ������������� by FUID

	CT_BOOL GetFriendRSP(CT_CHAR *pBuf, CT_DWORD &bufLen);
	CT_VOID InsertFriendRSP(const CT_DWORD dwSubID, const CMD_G_Friend_RSP &data);

	CT_VOID UserExitEvent(CT_INT64 iExitTime = 0);					//�Լ��˳�
	CT_VOID UserExitEvent(CT_DWORD dwFriendID, CT_INT64 iExitTime = 0, CT_INT32 iFType = FRIEND_DATA);		//�����˳�

	CT_VOID Broadcast(CT_DWORD mainID, CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len);	//���ѹ㲥
	CT_VOID SetRSPCount(CT_INT32 iCount);
};

#endif