
// ciratefi.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CciratefiApp:
// �аѾ\��@�����O�� ciratefi.cpp
//

class CiratefiApp : public CWinAppEx
{
public:
	CiratefiApp();

// �мg
	public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CiratefiApp theApp;