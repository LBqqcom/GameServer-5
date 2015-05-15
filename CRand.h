/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCRand.h
Version:		1.0
Description:    尾随机数
Author:			wangchen
Create Date:	2012-9-29
History:            

******************************************************************/
#ifndef CCRAND_H_
#define CCRAND_H_

#include "CTType.h"

class CCRand
{
private:
	CT_UINT32 m_iNext;
public:
	CCRand(void);
	~CCRand(void);

private:
	CT_INT32 rand_r(CT_UINT32 *seed);
public:
	//随机数
	CT_INT32 rand(void);
	//设置种子
	CT_VOID srand(CT_UINT32 seed);
};

#endif
