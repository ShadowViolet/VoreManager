// Shadow.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "Shadow.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为函数中的第一个语句
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CShadowApp

BEGIN_MESSAGE_MAP(CShadowApp, CWinApp)
END_MESSAGE_MAP()


// CShadowApp 构造

CShadowApp::CShadowApp()
{
}


// 唯一的一个 CShadowApp 对象

CShadowApp theApp;


// CShadowApp 初始化

BOOL CShadowApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	//阴影效果初始化
	CWndShadow::Initialize(AfxGetInstanceHandle());

	IsUpdata = FALSE;

	return TRUE;
}


int CShadowApp::ExitInstance()
{
	_CrtDumpMemoryLeaks();                 // 内存泄露检测

	return CWinApp::ExitInstance();
}


extern "C" __declspec(dllexport) BOOL ShowMainWnd()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CoInitialize(NULL);
	AfxEnableControlContainer();
	AfxInitRichEdit();

	CMainDlg dlg;
	theApp.m_pMainWnd = &dlg;

	//GDI+
	ULONG_PTR gdiplusToken;
	GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	if (dlg.DoModal() == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
		return FALSE;
	}

	//关闭gdiplus的环境
	Gdiplus::GdiplusShutdown(gdiplusToken);

	if (theApp.IsUpdata)
	{
		return -1;
	}
	
	return TRUE;
}
