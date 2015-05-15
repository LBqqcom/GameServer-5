#include "CMasterEvent.h"
#include "CMD_Game.h"
#include "CMD_Charge.h"
#include "CUserManager.h"
#include "CMasterScriptSetting.h"
#include "CItemScriptSetting.h"
#include "CGameConfig.h"
#include "CThreadMes.h"
#include "CGlobeData.h"
#include "CGlobeFunc.h"
#include "CDBMasterEvent.h"
#include "COrderList.h"
#include "CMD5.h"
#include "CLogMeg.h"
#include "CActive.h"
#include "CTitle.h"

CCMasterEvent::CCMasterEvent(CCGameMesManager* pMesManager, CT_DWORD id) : CCGameMesEvent(pMesManager, id)
{    
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_UPGRADE);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_LEARN_TALENT);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_RANK_UPGRADE);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_LEARN_SKILL);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_EQUIP_SKILL);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_RESET_SKILL);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_RANK_REQ_SALARY);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE);
    RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_EX);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_VG);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_APPLE);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_GOOGLE);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_ANOTHER_INFO);
	RegistMessage(MAIN_G_MASTER_EVENT_CITY, SUB_G_TITLE_USE);

    RegistMessage(MAIN_G_ELEMENT_POWER, SUB_G_BATTLE_FLAG_UPGRADE);

    RegistMessage(MAIN_CH_CHARGE, SUB_CH_TIMEOUT);
    RegistMessage(MAIN_CH_CHARGE, SUB_CH_CHAGERSUC);
    RegistMessage(MAIN_CH_CHARGE, SUB_CH_SHEET_GOT); 
	RegistMessage(MAIN_CH_CHARGE, SUB_CH_CHAGERFAIL);

	RegistMessage(MAIN_CH_CHARGE, SUB_CH_RECEIPT_GOT);

	RegistMessage(MSC_MAIN_MASTER , MSC_SUB_CHECK_OTHER_FINISH);
	RegistMessage(MSC_MAIN_MASTER , MSC_SUB_CHARGE_RESEND);
	RegistMessage(MSC_MAIN_MASTER, MSC_SUB_TITLELIST_GET);
	RegistMessage(MSC_MAIN_MASTER, MSC_SUB_TITLELIST_UPDATE);

    if (m_dwThread == GAME_THREAD_CITY)
    {
        // 定时器
        G_ValueRange vr = {0};
        //vr.fMaxVal = 10.0f;
        //RegistTimer(IDI_MASTER_COOLDOWN_TIMER);
        //SetTimer(IDI_MASTER_COOLDOWN_TIMER, (CT_INT32)(vr.fMaxVal*1000));

        // 定时器
        //RegistMessage(IDI_RANK_SALARY_CLOCK);
        //SetClock(IDI_RANK_SALARY_CLOCK, 0, 0, 0);

        // 体力恢复定时器
        CCMasterScriptSetting::getSingleton().GetValueRange(VALUE_TYPE_STAMINAINTERVAL, vr);
        RegistTimer(IDI_MASTER_STAMINA_RECOVER);
        SetTimer(IDI_MASTER_STAMINA_RECOVER, (CT_INT32)(vr.fMaxVal * 1000));

		RegistTimer(IDI_CHARGE_RECONNECT_TIMER);
    }   
}

CCMasterEvent::~CCMasterEvent(void)
{
}

//处理用户消息
CT_VOID CCMasterEvent::OnUserEvent(CT_DWORD mainID,	CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len, CT_LONGLONG reserve)
{
	switch (mainID)
	{
	case MAIN_G_MASTER_EVENT_CITY:
		DisposeMasterEvent(subID,  pBuf, len);
		break;
    case MAIN_CH_CHARGE:
        DisposeChargeEvent(subID, pBuf, len);        
        break;
    case MAIN_G_ELEMENT_POWER:
        DisposeElmPowerEvent(subID, pBuf, len);   
        break;
	case MSC_MAIN_MASTER:
	{
	    if(subID == MSC_SUB_CHECK_OTHER_FINISH)
		{
			DisposeCheckOtherFinish(pBuf, len);
		}
		if (subID == MSC_SUB_CHARGE_RESEND)
		{
			SetTimer(IDI_CHARGE_RECONNECT_TIMER, 1000*10);
		}
		if (subID == MSC_SUB_TITLELIST_GET)
		{
			DisposeTitleListGet(pBuf, len);
		}
		if (subID == MSC_SUB_TITLELIST_UPDATE)
		{
			CCUserManager::getSingleton().UpdateTitleList();
		}
		break;
	}
	default:
		break;
	}
}

 //用户定时器处理
CT_VOID CCMasterEvent::OnTime(CT_DWORD timeID)
{
    switch (timeID)
    {
    case IDI_MASTER_COOLDOWN_TIMER:
        CCUserManager::getSingleton().UpdateCooldown();
        break;
    case IDI_MASTER_STAMINA_RECOVER:
        CCUserManager::getSingleton().RecoverStamina();
        break;
	case IDI_CHARGE_RECONNECT_TIMER:
		KillTimer(IDI_CHARGE_RECONNECT_TIMER);
		CCOrderList::getSingleton().ReSendCharge();
		break;
	default:
		break;
    }
}

//用户时钟处理
CT_VOID CCMasterEvent::OnClock(CT_DWORD timeID)
{
    switch (timeID)
    {
    case IDI_RANK_SALARY_CLOCK:
        // 俸禄是主动领取。
        // 发放俸禄
        // CCUserManager::getSingleton().PushRankSalary();
        break;
	default:
		break;
    }
}

//处理领主事件
CT_VOID CCMasterEvent::DisposeMasterEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
	switch (subID)
	{
	case  SUB_G_MASTER_UPGRADE:
		if (sizeof(CMD_G_LevelUpgrade) == len) DisposeMasterUpgrade(*(CMD_G_LevelUpgrade*)pBuf);
		break;
    case  SUB_G_LEARN_TALENT:
        if (sizeof(CMD_G_LearnTalent) == len) DisposeLearnTalent(*(CMD_G_LearnTalent *)pBuf);            
        break;
    case SUB_G_RANK_UPGRADE:
        if (sizeof(CMD_G_LevelUpgrade) == len) DisposeRankUpgrade(*(CMD_G_LevelUpgrade *)pBuf);
        break;
    case SUB_G_LEARN_SKILL:
        if (sizeof(CMD_G_LearnSkill) == len) DisposeLearnSkill(*(CMD_G_LearnSkill *)pBuf);
        break;
    case SUB_G_EQUIP_SKILL:
        if (sizeof(CMD_G_EquipSkill) == len) DisposeEquipSkill(*(CMD_G_EquipSkill *)pBuf);
        break;
    case SUB_G_RESET_SKILL:
        if (sizeof(CMD_G_ResetSkill) == len) DisposeResetSkill(*(CMD_G_ResetSkill *)pBuf);
        break;
    case SUB_G_RANK_REQ_SALARY:
        if (sizeof(CMD_G_RankSalaryReq) == len) DisposeRankSalaryReq(*(CMD_G_RankSalaryReq *)pBuf);
        break;
	case SUB_G_MASTER_CHARGE_VG:
    case SUB_G_MASTER_CHARGE :
    case SUB_G_MASTER_CHARGE_EX :
	case SUB_G_MASTER_CHARGE_APPLE:
	case SUB_G_MASTER_CHARGE_GOOGLE:
        // 数据至少是CMD_G_Charge
        if (sizeof(CMD_G_Charge) <= len) {
            DisposeChargeReq(*(CMD_G_Charge *)pBuf, subID, len);
        }
        break;
	case  SUB_G_MASTER_ANOTHER_INFO:
		{
			DisposeAnotherInfo(pBuf, len);
			break;
		}
	case SUB_G_TITLE_USE:
		{
			DisposeTiTleUse(*(CMD_G_TitleUse*)pBuf);
			break;
		}
	default:
		break;
	}
}

// 处理充值结果
CT_VOID CCMasterEvent::DisposeChargeEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
    CCUser *pUser = NULL;
    switch (subID)
    {
    case SUB_CH_TIMEOUT :
        if (sizeof(CMD_CH_ChargeTimeout) == len) {
            DisposeChargeTimeout(*(CMD_CH_ChargeTimeout *)pBuf);
        }
        break;
    case SUB_CH_CHAGERSUC :
        if (sizeof(CMD_CH_ChargeSuc) <= len) {
            DisposeChargeSuc(pBuf, len);
        }
        break;
    case SUB_CH_SHEET_GOT :
        if (sizeof(CMD_CH_SheetGot) <= len) {
            DisposeOrderSuc(*(CMD_CH_SheetGot *)pBuf, len - sizeof(CMD_CH_SheetGot));
        }
        break;
	case SUB_CH_CHAGERFAIL:
		if (sizeof(CMD_CH_ChargeFail) == len) {
			DisposeChargeFail(*(CMD_CH_ChargeFail *)pBuf);
		}
		break;
	case SUB_CH_RECEIPT_GOT:
		if (sizeof(CMD_CH_ReceiptInfo) == len)
		{
			DisposeReceiptGot(*(CMD_CH_ReceiptInfo *)pBuf);
		}
		break;
    }
    if (pUser != NULL) {
        
    }
}

CT_VOID CCMasterEvent::DisposeElmPowerEvent(CT_DWORD subID, CT_CHAR* pBuf, CT_DWORD len)
{
    switch (subID)
    {
    case SUB_G_BATTLE_FLAG_UPGRADE:
        if (sizeof(CMD_G_BattleFlagUpgrade) == len) DisposeBattleFlagUpgrade(*(CMD_G_BattleFlagUpgrade *)pBuf);
        break;
    }
}

//////////////////////////////////////////////////////////////////////////
//处理领主升级事件
CT_VOID CCMasterEvent::DisposeMasterUpgrade(const CMD_G_LevelUpgrade& upgrade)
{
	CCUser* pUser = 0;
	if(CCUserManager::getSingleton().Find(upgrade.dwUserID, &pUser))
	{
        pUser->m_userBase.MasterUpgrage();
	}
}

// Begin [9/18/2012 John]
// 处理军衔提升事件
CT_VOID CCMasterEvent::DisposeRankUpgrade(const CMD_G_LevelUpgrade& stGrade)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(stGrade.dwUserID, &pUser))
    {
        pUser->m_userBase.RankUpgrage();
    }
}

// 处理学习潜能事件
CT_VOID CCMasterEvent::DisposeLearnTalent(const CMD_G_LearnTalent& learn)
{
    CCUser* pUser = 0;
    //ENUM_LearnTalentFail enFail = ELT_DATA_ERROR;
    if(CCUserManager::getSingleton().Find(learn.dwUserID, &pUser))
    {
        ENUM_LearnTalentFail enFail = (ENUM_LearnTalentFail)pUser->m_userTalent.LearnTalent(learn.iTalentID);
        if (enFail != ELT_SUCCESS) {
            CMD_G_LearnTalentFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_LEARN_TALENT_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);        
        }
    }
}

// 处理技能学习事件
CT_VOID CCMasterEvent::DisposeLearnSkill(const CMD_G_LearnSkill &learn)
{
    CCUser* pUser = 0;    

    if(CCUserManager::getSingleton().Find(learn.dwUserID, &pUser))
    {
        ENUM_LearnSkillFail enFail = (ENUM_LearnSkillFail)pUser->m_userSkill.LearnSkill(learn);
        if (enFail != ELS_SUCCESS) {
            CMD_G_LearnSkillFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_LEARN_SKILL_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);        
        }
    }
}

// 处理装备技能
CT_VOID CCMasterEvent::DisposeEquipSkill(const CMD_G_EquipSkill &equip)
{
    CCUser* pUser = 0;
    ENUM_EquipSkillFail enFail = EES_DATA_ERROR;

    if(CCUserManager::getSingleton().Find(equip.dwUserID, &pUser))
    {
        if (EES_SUCCESS == (enFail = (ENUM_EquipSkillFail)pUser->m_userSkill.EquipSkill(equip.iSkillID)))
        {
            // 通知客户端
            CMD_G_EquipSkillSuc stSuc = {0};
            stSuc.dwUserID = pUser->GetUserID();
            memcpy(stSuc.iSkillID, equip.iSkillID, sizeof(stSuc.iSkillID));
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_EQUIP_SKILL_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);  
            return;
        }

        // 装备失败
        CMD_G_EquipSkillFail stFail = {0};
        stFail.dwUserID = pUser->GetUserID();
        stFail.enFail = enFail;
        pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_EQUIP_SKILL_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);     
    }
}

// 重置技能
CT_VOID CCMasterEvent::DisposeResetSkill(const CMD_G_ResetSkill &reset)
{
    CCUser* pUser = 0;
    ENUM_ResetSkillFail enFail = ERS_DATA_ERROR;

    if(CCUserManager::getSingleton().Find(reset.dwUserID, &pUser))
    {
        SkillData skData = {0};
        if (pUser->m_userSkill.GetData(reset.iSkillID, skData))
        {
            MasterVariableData mvd = {0};
            CT_INT32 AddSoul = -skData.iSkillSoul;
            mvd.stRes.iSkillSoul = skData.iSkillSoul;
            pUser->m_userBase.AddMastData(mvd, SAVETAG_RESET_SKILL);
            skData.iSkillSoul = 0;
            pUser->m_userSkill.InsertData(skData.iID, skData);
            enFail = ERS_SUCCESS;
            
            // 更新技能列表
            MSC_DB_W_SkillData wsd = {0};
            wsd.dwUserID = reset.dwUserID;
            wsd.iSkillID = reset.iSkillID;
            wsd.iGradeInc = AddSoul;        // 技魂归零
            wsd.Used = skData.Used;
            memcpy(wsd.szName, skData.szName, sizeof(wsd.szName));
            CT_INT32 threadID =  GAME_THREAD_DATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
            G_SendThreadInfo(threadID,  MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_W_SKILL_INFO , (CT_CHAR*)&wsd, sizeof(wsd));

            pUser->m_userBase.GetMasterData(mvd);
            CMD_G_ResetSkillSuc stSuc = {0};
            stSuc.dwUserID = pUser->GetUserID();
            stSuc.iSkillID = skData.iID;
            stSuc.iSkillSoul = mvd.stRes.iSkillSoul;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_RESET_SKILL_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);
        }
        else
        {
            enFail = ERS_NO_SKILL;

            CMD_G_ResetSkillFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_RESET_SKILL_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}

// 处理请求军衔俸禄
CT_VOID CCMasterEvent::DisposeRankSalaryReq(const CMD_G_RankSalaryReq &salary)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(salary.dwUserID, &pUser))
    {
        pUser->m_userBase.PushRankSalary();
    }
}

// 处理充值请求
CT_VOID CCMasterEvent::DisposeChargeReq(const CMD_G_Charge &charge, const CT_INT32 sub, CT_INT32 iLen)
{
    CCUser* pUser = 0;
    if(CCUserManager::getSingleton().Find(charge.dwUserID, &pUser))
    {
        G_ChargeData stCharge = {0};
        if (CCMasterScriptSetting::getSingleton().GetChargeData(charge.iPID, stCharge)) {
            // 生成签名[UserID-RMB-DATETIME]
            CT_CHAR szBuff[SIGN_RAW_LEN] = {0};
            CT_CHAR szSign[EXORDER_NO_LEN + 1] = {0};
            sprintf(szBuff, "%s-%d-%d-%d-%u", ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.c_str(), 
                ((CCGameConfig&)CCConfig::getSingleton()).m_gamePort, charge.dwUserID, stCharge.iPID, time(0));

            // MD5
            if(CCMD5::getSingleton().Encrypt((CT_UCHAR *)szSign, sizeof(szSign), (CT_UCHAR*)szBuff, sizeof(szBuff)))
            {
                MasterData md = {0};
                pUser->m_userBase.GetMasterData(md);

                CMD_CH_Charge chData = {0};
                chData.dwUserID = charge.dwUserID;
                chData.iChannel = md.iChannel;
                chData.GamePort = ((CCGameConfig&)CCConfig::getSingleton()).m_gamePort;
                memcpy(chData.cGameServer, ((CCGameConfig&)CCConfig::getSingleton()).m_gameAddr.c_str(), sizeof(chData.cGameServer));
                memcpy(chData.cGameServerName, ((CCGameConfig&)CCConfig::getSingleton()).m_strServerName.c_str(), sizeof(chData.cGameServerName));
                memcpy(chData.cImei, charge.cImei, sizeof(chData.cImei));
                memcpy(chData.cImsi, charge.cImsi, sizeof(chData.cImsi));
                strncpy(chData.cExOrder, szSign, EXORDER_NO_LEN);
                chData.iDiamod = stCharge.iDiamond;
                // 发送到充值服务器
				if (sub == SUB_G_MASTER_CHARGE)
				{					
					g_pPChargeServer->Send(MAIN_CH_CHARGE, SUB_CH_CHAGRE, (CT_CHAR *)&chData, sizeof(chData), false, true);
				}
				else if (sub == SUB_G_MASTER_CHARGE_VG)
				{
                    CMD_CH_ChargeWeiXun wxData;
                    memset(&wxData, 0, sizeof(wxData));
                    *(CMD_CH_Charge *)&wxData = chData;
					wxData.iAmount = stCharge.iRMB * 100;
					g_pPChargeServer->Send(MAIN_CH_CHARGE, SUB_CH_CHAGRE_WEIXUN, (CT_CHAR *)&wxData, sizeof(wxData), false, true);
				}else if (sub == SUB_G_MASTER_CHARGE_APPLE)
				{
				    CMD_G_ChargeApple   *pCharge = (CMD_G_ChargeApple *)&charge;
					CMD_CH_ChargeApple  wxData;
					memset(&wxData, 0,  sizeof(wxData));
					*(CMD_CH_Charge *)&wxData = chData;
					strncpy(wxData.cProductID, stCharge.cProductID, sizeof(wxData.cProductID));
					wxData.iCostMoney  = stCharge.iRMB;
					memcpy(wxData.cReceipt, pCharge->cReceipt, RECEIPT_SIZE);
					g_pPChargeServer->Send(MAIN_CH_CHARGE, SUB_CH_CHAGRE_APPLE, (CT_CHAR *)&wxData, sizeof(wxData), false, true);
				}
                else if (sub == SUB_G_MASTER_CHARGE_EX)
                {
                    CT_CHAR cData[UNICOM_JSON_SIZE + sizeof(CMD_CH_ChargeEx)] = {0};
                    CMD_G_ChargeEx *pCharge = (CMD_G_ChargeEx *)&charge;
                    CMD_CH_ChargeEx *pexData = (CMD_CH_ChargeEx *)cData;
                    //memset(&exData, 0, sizeof(exData));
                    *(CMD_CH_Charge *)pexData = chData;
                    pexData->iAmount = stCharge.iRMB * 100;
                    strncpy(pexData->cSdkName, pCharge->sdkName, NAME_SIZE);

					if (strcmp(pexData->cSdkName, "Unicom") == 0)
					{
                        // 截取订单号，只需要24位。
						memset(pexData->cExOrder + UNICOM_NO_LEN, 0, sizeof(pexData->cExOrder) - UNICOM_NO_LEN);
						memset(szSign + UNICOM_NO_LEN, 0, sizeof(pexData->cExOrder) - UNICOM_NO_LEN);
					}
					else if (strcmp(pexData->cSdkName, "zhuowang") == 0)
					{
						// 卓望订单号16位，高八位时间，低八位uid
						CT_CHAR cExOrder[ZHUOWANG_NO_LEN+1] = {0};
						CT_INT32 iTime = time(0);
	
						sprintf(cExOrder, "%x%d", iTime, pUser->GetUserID());
						memset(cExOrder + strlen(cExOrder), '0', ZHUOWANG_NO_LEN - strlen(cExOrder));
						memset(pexData->cExOrder, 0, sizeof(pexData->cExOrder));
						strncpy(pexData->cExOrder, cExOrder, ZHUOWANG_NO_LEN);
						memset(szSign, 0, sizeof(szSign));
						strncpy(szSign, cExOrder, ZHUOWANG_NO_LEN);
					}

					// 拷贝额外数据
					if (iLen > sizeof(CMD_G_ChargeEx))
					{
						strncpy((CT_CHAR *)(cData + sizeof(CMD_CH_ChargeEx)), (CT_CHAR*)(pCharge + 1), iLen - sizeof(CMD_G_ChargeEx));
					}
					
					g_pPChargeServer->Send(MAIN_CH_CHARGE, SUB_CH_CHAGRE_EX, cData, sizeof(CMD_CH_ChargeEx) + iLen - sizeof(CMD_G_ChargeEx) , false, true);
                }
				else if (sub == SUB_G_MASTER_CHARGE_GOOGLE)
				{
					CMD_G_ChargeApple_EX   *pCharge = (CMD_G_ChargeApple_EX *)&charge;
					CMD_CH_ChargeApple_EX  wxData;
					memset(&wxData, 0,  sizeof(wxData));
					*(CMD_CH_Charge *)&wxData = chData;
					strncpy(wxData.cSdkName, pCharge->cSdkName,sizeof(wxData.cSdkName));
					memcpy(wxData.cReceipt, pCharge->cReceipt, RECEIPT_SIZE);
					g_pPChargeServer->Send(MAIN_CH_CHARGE, SUB_CH_CHAGRE_APPLE_EX, (CT_CHAR *)&wxData, sizeof(wxData), false, true);
				}

                // 记录订单
                CCOrderList::getSingleton().InsertSignedInfo(szSign, pUser->GetUserID(), stCharge.iPID,md.iChannel);
                // 记录订单请求提交到数据库
                MSC_LOGDB_Charge stDb = {0};
                stDb.dwUserID = pUser->GetUserID();

				MasterVariableData mvd = {0};
				pUser->m_userBase.GetMasterData(mvd);
				stDb.iUserGrade = mvd.stGrade.iLevel;
                stDb.iPrice = stCharge.iRMB;
                stDb.iDiamond = stCharge.iDiamond + stCharge.iExtern;
                stDb.iState = CHS_POST;
                strncpy(stDb.szSign, szSign, EXORDER_NO_LEN);
                CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
                G_SendThreadInfo(threadID, MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CHARGE , (CT_CHAR*)&stDb, sizeof(stDb));

                return;
            }
        }
        CMD_G_ChargeFail stFail = {0};
        stFail.dwUserID = pUser->GetUserID();
        stFail.enFail = ECH_DATA_ERROR;
        pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_TIMEOUT, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        
    }
}

// 生成订单成功
CT_VOID CCMasterEvent::DisposeOrderSuc(const CMD_CH_SheetGot &order, const CT_INT32 exLen)
{
    OrderInfo orderInfo = {0};
    if (CCOrderList::getSingleton().PickSignedInfo(order.cExOrder, orderInfo, CT_FALSE)) {
        CCUser* pUser = 0;
        if (CCUserManager::getSingleton().Find(orderInfo.dwUserID, &pUser))
        {
            // 通知客户端订单成功
            CT_CHAR buf[1024] = {0};
            CMD_G_ChargeOrder *pstOrder = (CMD_G_ChargeOrder*)buf;
            pstOrder->dwUserID = pUser->GetUserID();
            pstOrder->iPID = orderInfo.iPID;
            strncpy(pstOrder->cOrder, orderInfo.szSign, EXORDER_NO_LEN);
            if (exLen > 0) {
                memcpy((CT_CHAR *)(pstOrder + 1), &order + 1, exLen);
				pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_ORDER_EX, buf, sizeof(*pstOrder) + exLen, false, true);
			} else {
				pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_ORDER, buf, sizeof(*pstOrder) + exLen, false, true);
			}
            return;
        }
    }
}

// 处理充值超时
CT_VOID CCMasterEvent::DisposeChargeTimeout(const CMD_CH_ChargeTimeout &fail)
{
    CCUser *pUser = NULL;
    if (CCUserManager::getSingleton().Find(fail.dwUserID, &pUser)) {
        OrderInfo order = {0};
        CCOrderList::getSingleton().PickSignedInfo(fail.cExOrder, order);

        // 更新数据库记录
        //MSC_DB_U_Charge stDb = {0};
		MSC_LOGDB_UpdateCharge stDb = {0};
        strncpy(stDb.szSign, fail.cExOrder, EXORDER_NO_LEN);
        stDb.iState = CHS_TIMEOUT;
		CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;       
        G_SendThreadInfo(threadID, MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CHARGEUPDATE , (CT_CHAR*)&stDb, sizeof(stDb));

        CMD_G_ChargeFail stFail = {0};
        stFail.dwUserID = pUser->GetUserID();
        stFail.enFail = ECH_TIMEOUT;
        pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_TIMEOUT, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
    }
}

// 处理充值失败
CT_VOID CCMasterEvent::DisposeChargeFail(const CMD_CH_ChargeFail &fail)
{
	CCUser *pUser = NULL;
	if (CCUserManager::getSingleton().Find(fail.dwUserID, &pUser)) {
		OrderInfo order = {0};
		CCOrderList::getSingleton().PickSignedInfo(fail.cExOrder, order);

		// 更新数据库记录
		MSC_LOGDB_UpdateCharge stDb = {0};
		strncpy(stDb.szSign, fail.cExOrder, EXORDER_NO_LEN);
		stDb.iState = CHS_FAIL;
		CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + pUser->GetUserID() % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;		
		G_SendThreadInfo(threadID, MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CHARGEUPDATE , (CT_CHAR*)&stDb, sizeof(stDb));

		CMD_G_ChargeFailDesc stFail = {0};
        CT_INT32 iErrorNo = atoi(fail.cFailedDesc);
        if (iErrorNo != 0) {
            G_PromptText st = {0};
            if (0 == G_GetPromptText(PROMPT_YEEPAY, iErrorNo, st)) {
                // 默认的错误提示
                G_GetPromptText(PROMPT_YEEPAY, 10000, st);
            }
            memcpy(stFail.cFailDesc, st.cText, sizeof(stFail.cFailDesc));
        } else {
		    memcpy(stFail.cFailDesc, fail.cFailedDesc, FAILED_DESC_LEN);
        }
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
	}
}


// 处理充值成功
CT_VOID CCMasterEvent::DisposeChargeSuc(const CT_CHAR* pBuf, CT_INT32 iLen)
{
	CMD_CH_ChargeSuc_EX suc;
	if (iLen == sizeof(CMD_CH_ChargeSuc))
	{
		CMD_CH_ChargeSuc* pcs = (CMD_CH_ChargeSuc*)pBuf;
		suc.dwUserID = pcs->dwUserID;
		suc.iChargeIndex = pcs->iChargeIndex;
		strncpy(suc.cExOrder, pcs->cExOrder, sizeof(suc.cExOrder));
	}
	else if (iLen == sizeof(CMD_CH_ChargeSuc_EX))
	{
		suc = *(CMD_CH_ChargeSuc_EX*)pBuf;
	}
	else
	{
		CCLogMeg::getSingleton().WriteError("CCMasterEvent::DisposeChargeSuc iLen error.");
		return;
	}

    CCUser *pUser = NULL;
    OrderInfo order = {0};
    if (CCOrderList::getSingleton().PickSignedInfo(suc.cExOrder, order) && order.dwUserID == suc.dwUserID) {
		// 如果是苹果订单，根据苹果返回的计费点，找到PID
		if (iLen == sizeof(CMD_CH_ChargeSuc_EX))
		{
			CT_INT32 iPidList[100] = {0};
			CT_INT32 iPidNum = 0;
			G_ChargeData chdata = {0};
			CT_INT32* pPidList = iPidList;
			iPidNum = G_GetChargeList(&pPidList, sizeof(iPidList));

			for (CT_INT32 ip = 0;ip < iPidNum;++ip)
			{
				if (G_GetChargeData(iPidList[ip], &chdata) && strncmp(chdata.cProductID, suc.cProductID,strlen(suc.cProductID)) == 0)
				{
					order.iPID = iPidList[ip];

					// 将金额更新到数据库
					MSC_LOGDB_UpdateCharge_EX stdbEx = {0};
					strncpy(stdbEx.szSign, suc.cExOrder, EXORDER_NO_LEN);
					stdbEx.iState = CHS_SUC;
					stdbEx.iMoney = chdata.iRMB;
					stdbEx.iDiamond = chdata.iDiamond + chdata.iExtern;
					CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + suc.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
					G_SendThreadInfo(threadID, MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CHARGEUPDATE , (CT_CHAR*)&stdbEx, sizeof(stdbEx));
					break;
				}
			}
		}

        // 更新数据库订单记录
        MSC_LOGDB_UpdateCharge stDb = {0};
        strncpy(stDb.szSign, suc.cExOrder, EXORDER_NO_LEN);
		if (suc.iChargeIndex < 10 || suc.iChargeIndex == NULL)
		{
			stDb.iIndex = suc.iChargeIndex;
		}
        stDb.iState = CHS_SUC;
		CT_INT32 threadID =  GAME_THREAD_LOGDATABASE + suc.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadLogDataBaseNum;
        //CT_INT32 threadID =  GAME_THREAD_DATABASE + suc.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
        G_SendThreadInfo(threadID, MSC_MAIN_LOGDB_DATA, MSC_SUB_LOGDB_CHARGEUPDATE , (CT_CHAR*)&stDb, sizeof(stDb));

        G_ChargeData stCharge = {0};
        if (CCMasterScriptSetting::getSingleton().GetChargeData(order.iPID, stCharge)) {
			// 充值金额比订单金额少时，传回iChargeIndex，值为充值金额*10
			if (suc.iChargeIndex >= 10)
			{
				stCharge.iDiamond = suc.iChargeIndex;
				stCharge.iExtern = 0;
			}
            
            if (CCUserManager::getSingleton().Find(suc.dwUserID, &pUser)) {
                // 用户在线
                MasterVariableData mvd = {0};
                mvd.stRes.iDiamod = stCharge.iDiamond + stCharge.iExtern;
                pUser->m_userBase.AddMastData(mvd, SAVETAG_CHARGE_SUC);
				pUser->m_userBase.SetMastDataEx(stCharge.iDiamond, SAVETAGEX_CHARGEDIAMOD);

				if(pUser->LootItems(SAVETAG_CHARGE_SUC,&stCharge.iItemID,&stCharge.iItemNum,1,ITEM_OP_MODE_CHECK) == LOOT_SUC)
				{
					pUser->LootItems(SAVETAG_CHARGE_SUC,&stCharge.iItemID,&stCharge.iItemNum,1,ITEM_OP_MODE_EXEC);
				}
				else
				{
					if(stCharge.iItemID != 0)
					{
						MSC_DB_RESREWARD rr={0};
						rr.dwUID=suc.dwUserID;
						rr.iPID=stCharge.iItemID;
						rr.iPNum=stCharge.iItemNum;
						CT_DWORD threadid = GAME_THREAD_DATABASE + rr.dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
						G_SendThreadInfo(threadid,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_RES_PRODUCT_REWARD,(CT_CHAR*)&rr,sizeof(rr));
					}
				}

				CCActive::getSingleton().ActiveProcess(ACTIVE_RECHARGE_ADDUP,stCharge.iDiamond,pUser);
				CCActive::getSingleton().ActiveProcess(ACTIVE_RECHARGE_ONCE,stCharge.iDiamond,pUser);
				CCActive::getSingleton().ActiveProcess(ACTIVE_RECHARGE_FIRST,stCharge.iDiamond,pUser);

                // 通知客户端
                pUser->m_userBase.GetMasterData(mvd);
                CMD_G_ChargeSuc stSuc = {0};
                stSuc.dwUserID = pUser->GetUserID();
                stSuc.iDiamond = mvd.stRes.iDiamod;
                pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_SUC, (CT_CHAR *)&stSuc, sizeof(stSuc), false, true);                
            } else {
                // 用户不在线
				CT_INT32 iThreadID =  GAME_THREAD_DATABASE + suc.dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
                MSC_DB_MasterSaveData msd = {0};
                msd.dwUserID = suc.dwUserID;
                msd.mvd.stRes.iDiamod = stCharge.iDiamond + stCharge.iExtern;
                msd.iSaveTag = SAVETAG_CHARGE_SUC;
				G_SaveMastvariableData(msd);

				if(stCharge.iItemID != 0)
				{
					MSC_DB_RESREWARD rr={0};
					rr.dwUID=suc.dwUserID;
					rr.iPID=stCharge.iItemID;
					rr.iPNum=stCharge.iItemNum;
					CT_DWORD threadid = GAME_THREAD_DATABASE + rr.dwUID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
					G_SendThreadInfo(threadid,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_RES_PRODUCT_REWARD,(CT_CHAR*)&rr,sizeof(rr));
				}

				MSC_DB_MasterSaveDataEx msde = {0};
				msde.dwUserID = suc.dwUserID;
				msde.iSaveTagEx = SAVETAGEX_CHARGEDIAMOD;
				msde.iInt64Value = stCharge.iDiamond;
				G_SendDBThreadInfo(suc.dwUserID,MSC_MAIN_DB_SAVE_DATA, MSC_SUB_DB_MASTER_DATA_EX, (CT_CHAR*)&msde, sizeof(msde));

				
				//活动
				MSC_DB_ActiveEventStatus dbActiveEventStatus = {0};
				dbActiveEventStatus.dwUserID = suc.dwUserID;
				dbActiveEventStatus.iQuantity = stCharge.iDiamond;
				CT_INT32 iCount = 0;
				CT_INT32 AcviteList[100] = {0};

				iCount = CCActive::getSingleton().GetActiveByType(ACTIVE_RECHARGE_ADDUP, AcviteList);
				dbActiveEventStatus.iStatus = 0;	//累计
				for (CT_INT32 i=0; i<iCount; ++i)
				{
					dbActiveEventStatus.iAID = AcviteList[i];
					G_SendThreadInfo(iThreadID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&dbActiveEventStatus,sizeof(MSC_DB_ActiveEventStatus));
				}
				iCount = CCActive::getSingleton().GetActiveByType(ACTIVE_RECHARGE_ONCE, AcviteList);
				dbActiveEventStatus.iStatus = -1;	//取单比最大
				for (CT_INT32 i=0; i<iCount; ++i)
				{
					dbActiveEventStatus.iAID = AcviteList[i];
					G_SendThreadInfo(iThreadID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&dbActiveEventStatus,sizeof(MSC_DB_ActiveEventStatus));
				}
				iCount = CCActive::getSingleton().GetActiveByType(ACTIVE_RECHARGE_FIRST, AcviteList);
				dbActiveEventStatus.iStatus = ACTIVE_COMPLETED; //直接完成
				for (CT_INT32 i=0; i<iCount; ++i)
				{
					dbActiveEventStatus.iAID = AcviteList[i];
					G_SendThreadInfo(iThreadID,MSC_MAIN_DB_ACTIVE_DATA,MSC_SUB_ACTIVE_STATUS_UPDATE,(CT_CHAR*)&dbActiveEventStatus,sizeof(MSC_DB_ActiveEventStatus));
				}
		
            }
        }
    } else {
        // 订单找不到记录, 记录异常处理
        CCLogMeg::getSingleton().WriteError("\033[31mOrder[%s] for USER[%d] success! But miss the order info.\033[0m", suc.cExOrder, suc.dwUserID);
//         if (pUser != null) {
//             CMD_G_ChargeFail stFail = {0};
//             stFail.dwUserID = pUser->GetUserID();
//             stFail.enFail = ECH_DATA_ERROR;
//             pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
//         }
    }
}

CT_VOID CCMasterEvent::DisposeAnotherInfo(CT_CHAR* pBuf, CT_DWORD len)
{
	if(len != sizeof(CMD_G_ANOTHER_ARG))
	{
		CCLogMeg::getSingleton().WriteError("CCMasterEvent::DisposeAnotherInfo: len != sizeof(CMD_G_ANOTHER_ARG)");
		return;
	}

	CMD_G_ANOTHER_ARG *pAnotherArg=(CMD_G_ANOTHER_ARG*)pBuf;
	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pAnotherArg->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCDBUserEnterEvent::GetAnother,can't find user %d",pAnotherArg->dwUserID);
		return;
	}

	CCUser *pAUser=0;
	if (CCUserManager::getSingleton().Find(pAnotherArg->dwAnotherID,&pAUser))
	{
		PTRLinkUserData linkUserData = {0};
		UserFightingStatus fightingStatus = {0};
		LookUserInfo lookUserInfo = {0};
		pAUser->GetPTRLinkUserData(linkUserData);
		G_GetLookUserInfo(linkUserData,fightingStatus,lookUserInfo);
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_MASTER_ANOTHER_INFO, (CT_CHAR *)&lookUserInfo, sizeof(LookUserInfo));
		return ;
	}
	
	CT_DWORD dbThreadID = GAME_THREAD_DATABASE + pAnotherArg->dwUserID % ((CCGameConfig&)CCConfig::getSingleton()).m_gameThreadDataBaseNum;
	G_SendThreadInfo(dbThreadID, MSC_MAIN_DB_LOGIN, MSC_SUB_DB_ANOTHER, pBuf, len);
}

// 战旗升级
CT_VOID CCMasterEvent::DisposeBattleFlagUpgrade( const CMD_G_BattleFlagUpgrade &bfu )
{
    CCUser *pUser = NULL;
    if (CCUserManager::getSingleton().Find(bfu.dwUserID, &pUser)) {
        ENUM_UseItemFail enFail = (ENUM_UseItemFail)pUser->m_userBattleFlag.UpgradeFlag(bfu.iType);
        if (enFail != EUI_SUCCESS) {
            CMD_G_BattleFlagUpgradeFail stFail = {0};
            stFail.dwUserID = pUser->GetUserID();
            stFail.enFail = enFail;
            pUser->GetNetInterface()->Send(MAIN_G_ELEMENT_POWER, SUB_G_BATTLE_FLAG_UPGRADE_FAIL, (CT_CHAR *)&stFail, sizeof(stFail), false, true);
        }
    }
}



//查看别人的信息
CT_VOID CCMasterEvent::DisposeCheckOtherFinish(CT_CHAR* pBuf, CT_DWORD len)
{
	if (sizeof(MSC_PVP_OtherInfo ) != len)
	{
		return;
	}

	MSC_PVP_OtherInfo* pOtherInfo = (MSC_PVP_OtherInfo*)pBuf;

	CCUser *pUser=0;
	if (!CCUserManager::getSingleton().Find(pOtherInfo->dwUserID,&pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCMasterEvent::DisposeCheckOtherFinish,can't find user %d",pOtherInfo->dwUserID);
		return ;
	}
	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY,SUB_G_MASTER_ANOTHER_INFO, (CT_CHAR *)&pOtherInfo->otherUserInfo, sizeof(LookUserInfo));

}

//使用头衔
CT_VOID CCMasterEvent::DisposeTiTleUse(const CMD_G_TitleUse &gtu)
{
	CCUser *pUser = NULL;
	if (!CCUserManager::getSingleton().Find(gtu.dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCMasterEvent::DisposeTiTleUse can't find user:%d", gtu.dwUserID);
		return;
	}

	ENUM_UseTitleFail enFail = (ENUM_UseTitleFail)pUser->m_userTitle.UseTitle(gtu.iTitleID);
	if (enFail != EUT_USETITLE_SUC)
	{
		CMD_G_TitleUseFail tuf = {0};
		tuf.dwUserID = pUser->GetUserID();
		tuf.enFail = enFail;
		tuf.iTitleID = gtu.iTitleID;
		pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_TITLE_USE_FAIL, (CT_CHAR*)&tuf, sizeof(tuf), false, true);
	}
}

// 处理receipt信息返回（苹果一类）
CT_VOID CCMasterEvent::DisposeReceiptGot(const CMD_CH_ReceiptInfo &ri)
{
	CCUser *pUser = NULL;
	if (!CCUserManager::getSingleton().Find(ri.dwUserID, &pUser))
	{
		CCLogMeg::getSingleton().WriteError("CCMasterEvent::DisposeReceiptGot can't find user:%d", ri.dwUserID);
		return;
	}

	CMD_G_ReceiptInfo gri = {0};
	gri.iStutas = ri.iStutas;
	strncpy(gri.cOrderID, ri.cOrderID, sizeof(gri.cOrderID));
	pUser->GetNetInterface()->Send(MAIN_G_MASTER_EVENT_CITY, SUB_G_MASTER_CHARGE_INFO_GET, (CT_CHAR*)&gri, sizeof(gri));
}

//起服时得到头衔列表
CT_VOID CCMasterEvent::DisposeTitleListGet(CT_CHAR* pBuf, CT_INT32 iLen)
{
	if (iLen % sizeof(int) != 0)
	{
		return;
	}

	CT_INT32* pTitleID = (CT_INT32*)pBuf;
	CT_INT32 iNum = iLen / sizeof(int);
	CCTitle::getSingleton().DeleteTitleList();
	for (CT_INT32 i = 0;i < iNum;++i)
	{
		CCTitle::getSingleton().InsertTitle(pTitleID[i]);
	}
}