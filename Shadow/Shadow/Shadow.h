// Shadow.h : Shadow DLL 的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "sqlite3\sqlite3.h"
using namespace SqliteSpace;

#define WM_CHILDMESSAGE  WM_USER + 100 //自定义子窗口消息

// CShadowApp
// 有关此类实现的信息，请参阅 Shadow.cpp
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

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CShadowApp theApp;