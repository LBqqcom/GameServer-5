#include "CFightingRecord.h"
#include "CUser.h"

template<> CCFightingRecord* CCSingleton<CCFightingRecord>::ms_Singleton=0;

CCFightingRecord::CCFightingRecord(void)
{
	memset(m_Record,0,sizeof(m_Record));
	memset(m_FightingInfo,0,sizeof(m_FightingInfo));
}

CCFightingRecord::~CCFightingRecord(void)
{
}

//���ü�¼
CT_VOID	CCFightingRecord::SetFightingRector(const MSC_Fighting_Info &FightingInfo,CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iDifficulty)
{
	_AUTOMUTEX(&m_Mutex);

	if(WORLD_SCENE_COUNT <= iWorldScene || SCENE_COUNT <= iScene || iDifficulty >= DIFFICULTY)
		return;

	m_FightingInfo[iWorldScene][iScene][iDifficulty]=FightingInfo;
 
#ifdef a
	//Ψһ�ж�
	for(CT_INT32 i=0;i<RECORD;++i)
	{
		if(FightingInfo.dwUserID == m_FightingInfo[iWorldScene][iScene][i][0].dwUserID)
			return;
	}

	//����ɼ�(Ԥ�����)
	MSC_Fighting_Info FightingInfoEx[POST_COUNT]={0};
	FightingInfoEx[0]=FightingInfo;
	SortFightingRector(iWorldScene,iScene,FightingInfoEx);
#endif
	return;
}
//��ȡ��¼
CT_BOOL CCFightingRecord::GetFightingRector(MSC_Fighting_Info &FightingInfo,CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iDifficulty)
{
	_AUTOMUTEX(&m_Mutex); 

	if(WORLD_SCENE_COUNT <= iWorldScene || SCENE_COUNT <= iScene || DIFFICULTY <= iDifficulty)
		return CT_FALSE;

	if(m_FightingInfo[iWorldScene][iScene][iDifficulty].iSeed != 0)
	{
		FightingInfo=m_FightingInfo[iWorldScene][iScene][iDifficulty];
		return CT_TRUE;
	}

	return CT_FALSE;
}

//�����¼
CT_VOID CCFightingRecord::InsertRector(const MSC_Fighting_Info &FightingInfo,CT_INT32 iWorldScene,CT_INT32 iScene,CT_INT32 iDifficulty)
{
	_AUTOMUTEX(&m_Mutex); 
	CT_INT32 iCount=m_Record[iWorldScene][iScene]++;
	if(WORLD_SCENE_COUNT <= iWorldScene || SCENE_COUNT <= iScene || iDifficulty >= DIFFICULTY)
		return;

	m_FightingInfo[iWorldScene][iScene][iDifficulty]=FightingInfo;

	return;
}

//��¼����
CT_VOID CCFightingRecord::SortFightingRector(CT_INT32 iWorldScene,CT_INT32 iScene,const MSC_Fighting_Info &FightingInfo)
{
	if(WORLD_SCENE_COUNT <= iWorldScene || SCENE_COUNT <= iScene)
		return;

	for(CT_INT32 i=1;i<RECORD;++i)
	{
		m_FightingInfo[iWorldScene][iScene][i-1]=m_FightingInfo[iWorldScene][iScene][i];
	}

	m_FightingInfo[iWorldScene][iScene][RECORD-1]=FightingInfo;
	return;
}
