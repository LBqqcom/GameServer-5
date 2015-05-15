#ifndef CCGMORDER_H_
#define CCGMORDER_H_
#include <vector>
#include <string>
#include "CTType.h"
#include "../sdk/json/include/json/json.h"

class CCNetInterface;
class CCGmOrder
{
public:
	//typedef std::vector<std::string> OrderInfo;

public:
	CCGmOrder(void);
	~CCGmOrder(void);
public:
	CT_VOID DisposeGMOrder(CT_DWORD mainID, CT_DWORD subID,
		                   const Json::Value& info, CCNetInterface* pNetface);


private:
	CT_VOID DisposeGMChat(CT_DWORD subID, const Json::Value& info, CCNetInterface* pNetface);
	CT_VOID DisposeGMExit(CT_DWORD subID, const Json::Value& info, CCNetInterface* pNetface);
};

#endif