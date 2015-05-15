#include "CMD_Game.h"
#include "CMD_Center.h"
#include "CDatabaseMes.h"
#include "CUserManager.h"
#include "CGlobeFunc.h"
#include "CLogMeg.h"
#include "CGlobeData.h"
#include "CGameConfig.h"
#include "GlobeScriptInterface.h"
#include "CPetEvent.h"
#include <time.h>
#include "CUser.h"
#include "CBroadcastScriptSetting.h"

CCPetEvent::CCPetEvent(CCGameMesManager *pMesManager,CT_WORD id):CCGameMesEvent(pMesManager,id)
{
	RegistMessage(MSC_MAIN_EVENT_PET,MSC_SUB_USE_EGG);
	RegistMessage(MAIN_G_PET_EVENT_CITY,SUB_G_PET_UP_GRADE);
	RegistMessage(MAIN_G_PET_EVENT_CITY,SUB_G_PET_SKILL_EQUIP );
	RegistMessage(MAIN_G_PET_EVENT_CITY,SUB_G_PET_CHANGE);
	RegistMessage(MAIN_G_PET_EVENT_CITY,SUB_G_PET_SKILL_UPGRADE);
}

CCPetEvent::~CCPetEvent()
{

}

CT_VOID CCPetEvent::OnTime(CT_DWORD timeID)
{
}

CT_VOID CCPetEvent::OnUserEvent(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len,CT_LONGLONG reserve)
{
	switch(mainID)
	{
	case MSC_MAIN_EVENT_PET:
		{
			DisposeMecMessage(mainID,subID,pBuf,len);
			break;
		}
	case MAIN_G_PET_EVENT_CITY:
		{
			DisposeNetMessage(mainID,subID,pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCPetEvent::OnUserEvent mainID != MSC_MAIN_EVENT_PET or mainID !=MAIN_G_PET_EVENT_CITY mainID:%d",mainID);
			return;
		}
	}
}

//线程消息
CT_VOID CCPetEvent::DisposeMecMessage(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	if(MSC_MAIN_EVENT_PET != mainID)
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeMecMessage mainid %d",mainID);
		return;
	}

	switch(subID)
	{
	case MSC_SUB_USE_EGG:
		{
			DisposeUseEgg(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeMecMessage main:%d subid:%d",mainID,subID);
			return;
		}
	}
	return;
}

//网络消息
CT_VOID CCPetEvent::DisposeNetMessage(CT_DWORD mainID,CT_DWORD subID,CT_CHAR* pBuf,CT_DWORD len)
{
	if(MAIN_G_PET_EVENT_CITY != mainID)
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeNetMessage mainid %d",mainID);
		return;
	}

	switch(subID)
	{
	case SUB_G_PET_UP_GRADE:
		{
			DisposePetUpGrade(pBuf,len);
			break;
		}
	case SUB_G_PET_SKILL_EQUIP:
		{
			DisposePetSkill(pBuf,len);
			break;
		}
	case SUB_G_PET_CHANGE:
		{
			DisposePetChange(pBuf,len);
			break;
		}
	case SUB_G_PET_SKILL_UPGRADE:
		{
			DisposePetSkillUpGrade(pBuf,len);
			break;
		}
	default:
		{
			CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeNetMessage mainid:%d subid:%d",mainID,subID);
			break;
		}
	}
	return;
}

//使用宠物蛋
CT_VOID CCPetEvent::DisposeUseEgg(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(MSC_UsePetEgg) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeUseEgg sizeof(MSC_UsePetEgg) != iLen");
		return;
	}

	MSC_UsePetEgg *pUpr=(MSC_UsePetEgg*)pBuf;
	G_ItemBase ib={0};
	G_PetAddition pa={0};
	PetInfo pi={0};
	CCUser *pUser=0;
	if(!G_GetItemBase(pUpr->iItemID,&ib))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeUseEgg G_GetItemBase item:%d",pUpr->iItemID);
		return;
	}
	if(!G_GetPetAddition(ib.iEquipSkill,&pa))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeUseEgg G_GetPetAddition iEquipSkill:%d",ib.iEquipSkill);
		return;
	}
	if(!CCUserManager::getSingleton().Find(pUpr->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposeUseEgg No Find UserID:%d",pUpr->dwUserID);
		return;
	}

	PetChange pc={0};
	pc.iChangeID=ib.iEquipSkill;
	pc.iSkillID=pa.iInitiativeSkill;
	if(!pUser->m_userPet.GetPetInfo(pi))
	{
		//添加宠物
		pi.iPetID=PET_ID;
		pi.iGrade=1;
		pi.iChangeID=ib.iEquipSkill;
		pi.iSkillID[0]=pa.iInitiativeSkill;
		pi.iSkillID[1]=pa.iPassivitySkill;
		pUser->m_userPet.InsertPetBase(pi,CT_TRUE);
		pUser->m_userPet.InsertPetChange(pc,CT_TRUE);

		pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_INFO,(CT_CHAR*)&pi,sizeof(pi));

	}
	else
	{
		//增加幻化
		pUser->m_userPet.InsertPetChange(pc,CT_TRUE);

		CMD_G_PetChangeInfo pci={0};
		pci.iPetChangeID=ib.iEquipSkill;
		pci.iPetSkillID=pa.iInitiativeSkill;
		pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_CHANGE_INFO,(CT_CHAR*)&pci,sizeof(pci));

		//播放公告
		if(pci.iPetChangeID == 1)
		{
			MasterData masterData = {0};
			RollBroadcast rb={0};
			pUser->m_userBase.GetMasterData(masterData);
			rb.SetValue(pUser->GetUserID(), masterData.cNickName);
			G_SendRollBroadecast(95,4,1,(CT_CHAR*)&rb,sizeof(rb));
		}
	}

	pUser->UpdateAndSendMasterData(EMI_BATLLE);
	return;
}

//宠物升级
CT_VOID CCPetEvent::DisposePetUpGrade(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_PetUpGrade) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetRequest sizeof(CMD_G_PetUpGrade) != iLen");
		return;
	}

	CMD_G_PetUpGrade *pPug=(CMD_G_PetUpGrade*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pPug->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetUpGrade No Find UserID:%d",pPug->dwUserID);
		return;
	}

	CMD_G_PetUpGradeFail pugf={0};
	pugf.iFailInfo=ValidateUpGrade(pUser,*pPug);
	if(pugf.iFailInfo != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_UP_GRADE_FAIL,(CT_CHAR*)&pugf,sizeof(pugf));
		return;
	}

	PetInfo pt={0};
	pUser->m_userPet.GetPetInfo(pt);

	CMD_G_PetUpGradeReturn pugr={0};
	pugr.iExp=pt.iExp;
	pugr.iGrade=pt.iGrade;
	pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_UP_GRADE_RETURN,(CT_CHAR*)&pugr,sizeof(pugr));
	return;
}

//宠物技能
CT_VOID CCPetEvent::DisposePetSkill(CT_CHAR *pBuf,CT_INT32 iLen)
{
	if(sizeof(CMD_G_PetSkillEquip) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetRequest sizeof(CMD_G_PetSkillEquip) != iLen");
		return;
	}
	CMD_G_PetSkillEquip *pPse=(CMD_G_PetSkillEquip*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pPse->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetSkill No Find UserID:%d",pPse->dwUserID);
		return;
	}

	CMD_G_PetSkillEquipFail psef={0};
	psef.iFailInfo=ValidateSkill(pUser,*pPse);
	if(psef.iFailInfo != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_SKILL_EQUIP_FAIL,(CT_CHAR*)&psef,sizeof(psef));
		return;
	}


	G_ItemBase ib={0};
	G_EquipSkill es={0};
	if(!G_GetItemBase(pPse->iItemID,&ib))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetSkill G_GetItemBase item:%d",pPse->iItemID);
	}
	if(!G_GetEquipSkill(ib.iEquipSkill,pUser->m_userPet.GetPetSkillGrade(ib.iEquipSkill),&es.iSkill))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetSkill G_GetEquipSkill iEquipSkill:%d",ib.iEquipSkill);
	}

	pUser->m_userPet.LearnSkill(es.iSkill,pPse->iSkillBar);

	CMD_G_PetSkillEquipReturn pser={0};
	PetInfo pi={0};
	pUser->m_userPet.GetPetInfo(pi);
	pser.iSkillID=pi.iSkillID[pPse->iSkillBar];
	pser.iSkillBar=pPse->iSkillBar;
	pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_SKILL_EQUIP_RETURN,(CT_CHAR*)&pser,sizeof(pser));

	return;
}

//宠物幻化
CT_VOID CCPetEvent::DisposePetChange(CT_CHAR* pBuf,CT_DWORD iLen)
{
	if(sizeof(CMD_G_PetChange) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetRequest sizeof(CMD_G_PetChange) != iLen");
		return;
	}
	CMD_G_PetChange *pPc=(CMD_G_PetChange*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pPc->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetChange No Find UserID:%d",pPc->dwUserID);
		return;
	}

	CMD_G_PetChangeFail pcf={0};
	pcf.iFailInfo=ValidateChange(pUser,*pPc);
	if(pcf.iFailInfo != 0)
	{
		pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_CHANGE_FAIL,(CT_CHAR*)&pcf,sizeof(pcf));
		return;
	}

	pUser->m_userPet.SetChangeID(pPc->iChangeID);

	pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_CHANGE_RETURN,pBuf,iLen);

	pUser->UpdateAndSendMasterData(EMI_BATLLE);

	return;
}

//宠物技能升级
CT_VOID CCPetEvent::DisposePetSkillUpGrade(CT_CHAR* pBuf,CT_DWORD iLen)
{
	if(sizeof(CMD_G_PetSkillUpGrade) != iLen)
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetSkillUpGrade sizeof(CMD_G_PetSkillUpGrade) != iLen");
		return;
	}
	CMD_G_PetSkillUpGrade *pPsug=(CMD_G_PetSkillUpGrade*)pBuf;

	CCUser *pUser=0;
	if(!CCUserManager::getSingleton().Find(pPsug->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::DisposePetSkillUpGrade No Find UserID:%d",pPsug->dwUserID);
		return;
	}

	RewardCost rc={0};
	CT_INT32 iFail=ValidateSkillUpGrade(pUser,*pPsug,rc);
	if(iFail != 0)
	{
		CMD_G_PetSkillUpGradeFail psugf={0};
		psugf.iFailID=iFail;
		pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_SKILL_UPGRADE_FAIL,(CT_CHAR*)&psugf,sizeof(psugf));
		return;
	}

	PetInfo pi={0};
	pUser->m_userPet.GetPetInfo(pi);
	CMD_G_PetSkillUpGradeReturn psugr={0};
	psugr.iPetChangeID=pi.iChangeID;
	psugr.iSkillID=pUser->m_userPet.GetPetSkill(pi.iChangeID);
	memcpy(psugr.iResType,rc.iUseResType,sizeof(rc.iUseResType));
	memcpy(psugr.iResCount,rc.iCurrentResCount,sizeof(rc.iCurrentResCount));
	pUser->GetNetInterface()->Send(MAIN_G_PET_EVENT_CITY,SUB_G_PET_SKILL_UPGRADE_RETURN,(CT_CHAR*)&psugr,sizeof(psugr));
	return;
}

//升级验证
CT_INT32 CCPetEvent::ValidateUpGrade(CCUser *pUser,CMD_G_PetUpGrade &pug)
{
	PetInfo pt={0};
	G_PetDevour pd={0};
	G_PetUpgrade pu={0};
	CT_INT32 iNum=(pug.iNum > 0?-pug.iNum:pug.iNum);
	pUser->m_userPet.GetPetInfo(pt);
	if(!G_GetPetDevour(pt.iPetID,pug.iItemID,&pd))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::ValidateUpGrade G_GetPetDevour iPetID:%d,iItemID:%d",pt.iPetID,pug.iItemID);
		return UPGRADE_ITEM_DATA;
	}

	if(pt.iGrade > pd.iMaxGrade || pt.iGrade < pd.iMinGrade)
	{
		return UPGRADE_ITEM_ERR;
	}

	if(pUser->LootItems(SAVETAG_PET_UP_GRADE,&pug.iItemID,&iNum,1,ITEM_OP_MODE_CHECK) != LOOT_SUC)
	{
		return UPGRADE_ITEM_NUM;
	}

	if(!G_GetPetUpgrade(pt.iGrade+1,&pu))
	{
		return UPGRADE_ITEM_DATA;
	}

	//扣除资源
	pUser->LootItems(SAVETAG_PET_UP_GRADE,&pug.iItemID,&iNum,1,ITEM_OP_MODE_EXEC);

	pUser->m_userPet.AddPetExp(pd.iExp*(-iNum));

	return 0;
}

//技能验证
CT_INT32 CCPetEvent::ValidateSkill(CCUser *pUser,CMD_G_PetSkillEquip &pse)
{
	PetInfo pi={0};
	pUser->m_userPet.GetPetInfo(pi);

	G_ItemBase ib={0};
	if(!G_GetItemBase(pse.iItemID,&ib))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::ValidateSkill G_GetItemBase id:%d",pse.iItemID);
		return UPGRADE_ITEM_DATA;
	}


	//数量验证
	CT_INT32 iNum=-1;
	if(pUser->LootItems(SAVETAG_PET_SKILL,&pse.iItemID,&iNum,1,ITEM_OP_MODE_CHECK) != LOOT_SUC)
	{
		return UPGRADE_ITEM_NUM;
	}

	//道具段验证
	if(ib.iTID != ITEM_TYPE_PET_SKILL)
	{
		return SKILL_ITEM;
	}



	
	G_EquipSkill es={0};
	if(!G_GetEquipSkill(ib.iEquipSkill,pUser->m_userPet.GetPetSkillGrade(ib.iEquipSkill),&es.iSkill))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::ValidateSkill G_GetEquipSkill iEquipSkill:%d",ib.iEquipSkill);
		return SKILL_DATA;
	}
	
	CT_INT32 iSkillCount=PET_SKILL_COUNT-1+pUser->m_userVip.VipPriTimeGet(VIP_PRIVILEGE_PETSKILL);
	iSkillCount=(iSkillCount > PET_SKILL_COUNT?PET_SKILL_COUNT:iSkillCount);

	//同类型技能验证
	for(CT_INT32 i=2;i<iSkillCount;++i)
	{
		if(i == pse.iSkillBar)continue;
		if(pUser->m_userPet.GetPetSkillType(pi.iSkillID[i]) == pUser->m_userPet.GetPetSkillType(es.iSkill))
			return SKILL_SAME;
	}

	//相同技能验证
	for(CT_INT32 i=2;i<iSkillCount;++i)
	{
		if(pUser->m_userPet.GetPetSkillGrade(pi.iSkillID[i]) >= pUser->m_userPet.GetPetSkillGrade(es.iSkill) && 
			pUser->m_userPet.GetPetSkillType(pi.iSkillID[i]) == pUser->m_userPet.GetPetSkillType(es.iSkill))
		{
			return SKILL_GRADE;
		}
	}

	//消耗道具
	pUser->LootItems(SAVETAG_PET_SKILL,&pse.iItemID,&iNum,1,ITEM_OP_MODE_EXEC);

	return 0;
}
//幻化验证
CT_INT32 CCPetEvent::ValidateChange(CCUser *pUser,CMD_G_PetChange &pc)
{
	if(!pUser->m_userPet.GetChangeID(pc.iChangeID))
	{
		return CHANGE_ID;
	}
	return 0;
}

//主动技能升级验证
CT_INT32 CCPetEvent::ValidateSkillUpGrade(CCUser *pUser,CMD_G_PetSkillUpGrade &psug,RewardCost &rc)
{
	PetInfo pi={0};
	G_PetSkillUpGrade pg={0};
	pUser->m_userPet.GetPetInfo(pi);
	if(!G_GetPetSkillUpGradeEx(pi.iSkillID[0],&pg))
	{
		CCLogMeg::getSingleton().WriteError("CCPetEvent::ValidateSkillUpGrade G_GetPetSkillUpGradeEx skillID:%d",pi.iSkillID[0]);
		return PET_SKILL_DATA;
	}

	if(pg.iNextSkillID == 0)
	{
		return PET_SKILL_MAX;
	}

	if(pg.iGrade > pi.iGrade)
	{
		return PET_SKILL_GRADE;
	}
	
	FakeProBase fb={0};	
	if(G_GetCostReward(pg.iCostID,pUser,ITEM_OP_MODE_CHECK,0,rc,fb) != 0)
		return PET_SKILL_COST;

	memset(&rc,0,sizeof(rc));
	G_GetCostReward(pg.iCostID,pUser,ITEM_OP_MODE_EXEC,0,rc,fb);
	pUser->m_userPet.SetSkillID();

	MSC_LOGDB_PETUPSKILL ps={0};
	ps.dwUserID=pUser->GetUserID();
	ps.iItemID=rc.iUseItem[0];
	ps.iNum=rc.iUseItemCount[0];
	ps.iSkillID=pi.iSkillID[0];
	ps.iGold=rc.iUseResCount[0];

	G_SendLogDBThreadInfo(pUser->GetUserID(),MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_PETUPSKILL,(CT_CHAR*)&ps,sizeof(ps));

	return 0;
}