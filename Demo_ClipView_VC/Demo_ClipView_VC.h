
// Demo_ClipView_VC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDemo_ClipView_VCApp:
// �йش����ʵ�֣������ Demo_ClipView_VC.cpp
//

class CDemo_ClipView_VCApp : public CWinApp
{
public:
	CDemo_ClipView_VCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDemo_ClipView_VCApp theApp;