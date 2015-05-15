/* ͷ�ι���ʵ��   
 * ��Ҫ���ܣ�ͷ�μ��ͷ��ʹ�ã���ʱ����ͷ��״̬
 *         by zhouhuan
 */

#ifndef CUSERTITLE_H_
#define CUSERTITLE_H_

#include "GlobeScriptInterface.h"
#include "CUserBase.h"
#include "CUserData.h"
#include "CMutex.h"

// �����û�ʹ�õ�ͷ����Ϣ
struct TitleData 
{
	CT_INT32 iID;				//ͷ��ID
	CT_INT32 iActiveTime;		//����ʱ��
	CT_INT32 iEffectiveTime;	//ͷ����Чʱ��
	CT_BOOL bIsUse;			//ͷ���Ƿ�ʹ��
};

class CCUser;

class CCUserTitle : public CCUserData<CT_INT32, TitleData>
{
public:
	inline CT_VOID init(CCUser* pUser, CCMutex* pMutex)
	{
		m_pUser = pUser;
		m_pMutex = pMutex;
		m_DataMap.clear();
		m_lastSyncTime = time(0);
	}
	inline CT_VOID Clear()
	{
		_AUTOMUTEX(m_pMutex);
		m_DataMap.clear();
		m_lastSyncTime = 0;
	}

protected:
	/* ���ܣ��ڴ�������ͻ���Э�����ݵ�ת��
	   @src: �ڴ����ݽṹ��TitleData
	   @pDst��ת����Ŀͻ���Э�����ݣ�CMD_G_TitleData
	   ����ֵ��pDst�ĳ���
	 */
	virtual CT_INT32 transData(const TitleData& src, CT_CHAR* pDst)const;

public:
	virtual CT_VOID syncData(CT_INT64 currTime, CT_BOOL bForce = CT_FALSE){}

	/* ���ܣ�����ͷ�Σ��û����ߣ�
	 * @iTitleItemID: ����ͷ������ĵ���ID
	 * ����ֵ��ʧ����
	 */
	CT_INT32 ActiveTitle(CT_INT32 iTitleItemID);	

	/* ���ܣ�ʹ��ͷ��
	 * @iTitleID��ʹ�õ�ͷ��ID
	 * ����ֵ��ʧ����
	 */
	CT_INT32 UseTitle(CT_INT32 iTitleID);

	// ���ܣ�����ͷ��ʱ��
	CT_VOID UpdataTitleStatus();

	/* ���ܣ��õ�ʹ���е�ͷ��
	 * ����ֵ��ͷ��ID��û�з���0
	 */
	CT_INT32 GetUseTitleID();

	// �õ�ͷ���б�
	CT_INT32 GetTitleList(CT_CHAR* pBuf, CT_INT32& iLen);
	// ����ͷ���б�
	CT_VOID UpdataTitleList();

public:
	// ����ս����
	static CT_VOID UpdateBattleStatus(MasterCombatStatus &mcs, const CCUserTitle::DataMap &titleMap);
};

#endif