/******************************************************************
Copyright (C),  2002-2015,  TECHNOLOGLIES CO., LTD.
File name:		CCRand.h
Version:		1.0
Description:    β�����
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
	//�����
	CT_INT32 rand(void);
	//��������
	CT_VOID srand(CT_UINT32 seed);
};

#endif
