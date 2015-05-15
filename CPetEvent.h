#ifndef CPetEvent_H_
#define CPetEvent_H_

#include "CGameMesEvent.h"
#include "CMD_Game.h"
#include "CGlobeData.h"

class CCPetEvent:public CCGameMesEvent
{
public:
	CCPetEvent(CCGameMesManager *pMesManager,CT_WORD id);
	~CCPetEvent();
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
	//�߳���Ϣ
	CT_VOID DisposeMecMessage(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);
	//������Ϣ
	CT_VOID DisposeNetMessage(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len);

private:
	//ʹ�ó��ﵰ
	CT_VOID DisposeUseEgg(CT_CHAR *pBuf,CT_INT32 iLen);

private:
	//��������
	CT_VOID DisposePetUpGrade(CT_CHAR *pBuf,CT_INT32 iLen);
	//���＼��
	CT_VOID DisposePetSkill(CT_CHAR *pBuf,CT_INT32 iLen);
	//����û�
	CT_VOID DisposePetChange(CT_CHAR* pBuf,CT_DWORD iLen);
	//���＼������
	CT_VOID DisposePetSkillUpGrade(CT_CHAR* pBuf,CT_DWORD iLen);

private:
	//������֤
	CT_INT32 ValidateUpGrade(CCUser *pUser,CMD_G_PetUpGrade &pug);
	//������֤
	CT_INT32 ValidateSkill(CCUser *pUser,CMD_G_PetSkillEquip &pse);
	//�û���֤
	CT_INT32 ValidateChange(CCUser *pUser,CMD_G_PetChange &pc);
	//��������������֤
	CT_INT32 ValidateSkillUpGrade(CCUser *pUser,CMD_G_PetSkillUpGrade &psug,RewardCost &rc);
};

#endif