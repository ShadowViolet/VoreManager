
// VoreManager.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMyApp: 
// �йش����ʵ�֣������ VoreManager.cpp
//

class CMyApp : public CWinApp
{
public:
	CMyApp();

// ��д
public:
	virtual BOOL InitInstance();
	BOOL Repair(LPCTSTR lpName, LPCTSTR lpType, CString FilePath, CString FileName);

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMyApp theApp;