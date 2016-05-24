// Shadow.h : Shadow DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "sqlite3\sqlite3.h"
using namespace SqliteSpace;

#define WM_CHILDMESSAGE  WM_USER + 100 //�Զ����Ӵ�����Ϣ

// CShadowApp
// �йش���ʵ�ֵ���Ϣ������� Shadow.cpp
//

class CShadowApp : public CWinApp
{
public:
	CShadowApp();

	sqlite3 *db;
	sqlite3_stmt * stmt;
	char * errMsg;

	int ChildType;
	CString Class, Path, Name, Type, Fever, Parameters, User, Password;
	BOOL IsUpdata;

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CShadowApp theApp;