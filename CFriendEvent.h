#ifndef CCFRIENDEVNET_H_
#define CCFRIENDEVNET_H_
#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCFriendEvent:public CCGameMesEvent
{
public:
	CCFriendEvent(CCGameMesManager* pMesManager, CT_DWORD id);
	~CCFriendEvent(void);

private:
	//�����û���Ϣ
	virtual CT_VOID OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve);
	//�û���ʱ������
	virtual CT_VOID OnTime(CT_DWORD timeID);
	//�û�ʱ�Ӵ���
	virtual CT_VOID OnClock(CT_DWORD timeID)
	{
	}
	
private:
	CT_VOID DisposeFriendRequestBat(CT_CHAR* pBuf,CT_DWORD len);	//һ����������(����)
	CT_VOID DisposeFriendRequest(CT_CHAR* pBuf,CT_DWORD len);		//��������
	CT_VOID DisposeFriendRequest_S(CT_CHAR* pBuf,CT_DWORD len);		//�������� �Զ�
	CT_VOID DisposeFriendFind(CT_CHAR* pBuf,CT_DWORD len);			//�����ݿ���ҶԷ���Ϣ,by F_NickName

	CT_VOID DisposeFriendAcceptBat(CT_CHAR* pBuf,CT_DWORD len);		//һ�����ܺ�������(����)
	CT_VOID DisposeFriendAccept(CT_CHAR* pBuf,CT_DWORD len);		//���ܺ�������
	CT_VOID DisposeFriendAccept_S(CT_CHAR* pBuf,CT_DWORD len);		//���ܺ������� �Զ�
	CT_VOID DisposeFriendAcceptEX(CT_CHAR* pBuf,CT_DWORD len);		//���ܺ���������ӶԷ�Ϊ����

	CT_VOID DisposeFriendRejectBat(CT_CHAR* pBuf,CT_DWORD len);		//һ���ܾ���������(����)
	CT_VOID DisposeFriendReject(CT_CHAR* pBuf,CT_DWORD len);		//�ܾ���������
	CT_VOID DisposeFriendReject_S(CT_CHAR* pBuf,CT_DWORD len);		//�ܾ��������� �Զ�

	CT_VOID DisposeFriendDelete(CT_CHAR* pBuf,CT_DWORD len);		//ɾ������
	CT_VOID DisposeFriendDelete_S(CT_CHAR* pBuf,CT_DWORD len);		//ɾ������ �Զ�

	CT_VOID DisposeFriendBlack(CT_CHAR* pBuf,CT_DWORD len);			//���������
	CT_VOID DisposeFriendBlack_S(CT_CHAR* pBuf,CT_DWORD len);		//��������� �Զ�
	CT_VOID DisposeFriendFindBlack(CT_CHAR* pBuf,CT_DWORD len);		//�����ݿ���ҶԷ���Ϣ,by F_NickName

	CT_VOID DisposeFriendBlackDelete(CT_CHAR* pBuf,CT_DWORD len);	//ɾ��������
//	CT_VOID DisposeFriendBlackDelete_S(CT_CHAR* pBuf,CT_DWORD len);	//ɾ�������� �Զ�

	CT_VOID DisposeFriendPush(CT_CHAR* pBuf,CT_DWORD len);			//���ͺ���
	CT_VOID DisposeFriendLine(CT_CHAR* pBuf, CT_DWORD len);			//��ȡ����״̬

	CT_VOID DisposeFriendNoLinkSend(CT_CHAR* pBuf, CT_DWORD len);	//�򿪽���ʱ�����Ͳ����ߺ��ѵ���Ϣ

	CT_VOID DisposeFriendExit(CT_CHAR* pBuf, CT_DWORD len);	
};
#endif