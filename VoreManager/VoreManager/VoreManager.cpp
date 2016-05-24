
// VoreManager.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "VoreManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyApp

BEGIN_MESSAGE_MAP(CMyApp, CWinApp)
END_MESSAGE_MAP()


// CMyApp 构造

CMyApp::CMyApp()
{
}


// 唯一的一个 CMyApp 对象

CMyApp theApp;


// CMyApp 初始化

BOOL CMyApp::InitInstance()
{
	CWinApp::InitInstance();

	CFileFind Finder;               // 搜索动态链接库
	BOOL Status = Finder.FindFile(_T("Shadow.dll"));
	if (!Status)
		if(!Repair(MAKEINTRESOURCE(IDR_MAIN_LIBRARY), _T("Library"), NULL, _T("Shadow.dll")))
			AfxMessageBox(_T("无法自动修复缺失的核心链接库，请重新下载后再试！"));

	Status = Finder.FindFile(_T("avcodec-54.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_AVCODEC_54_LIBRARY), _T("Library"), NULL, _T("avcodec-54.dll")))
			AfxMessageBox(_T("无法自动修复缺失的视频解码链接库，请重新下载后再试！"));

	Status = Finder.FindFile(_T("avformat-54.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_AVFORMAT_54_LIBRARY), _T("Library"), NULL, _T("avformat-54.dll")))
			AfxMessageBox(_T("无法自动修复缺失的视频格式链接库，请重新下载后再试！"));

	Status = Finder.FindFile(_T("avutil-52.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_AVUTIL_52_LIBRARY), _T("Library"), NULL, _T("avutil-52.dll")))
			AfxMessageBox(_T("无法自动修复缺失的视频解析链接库，请重新下载后再试！"));

	Status = Finder.FindFile(_T("SDL.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_SDL_LIBRARY), _T("Library"), NULL, _T("SDL.dll")))
			AfxMessageBox(_T("无法自动修复缺失的音视频解码链接库，请重新下载后再试！"));

	Status = Finder.FindFile(_T("swscale-2.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_SWSCALE_2_LIBRARY), _T("Library"), NULL, _T("swscale-2.dll")))
			AfxMessageBox(_T("无法自动修复缺失的视频依赖链接库，请重新下载后再试！"));

	Main:
	HMODULE hMod = ::LoadLibrary(_T("Shadow.dll"));
	if (hMod != NULL)
	{
		typedef BOOL(WINAPI * ShowWnd)();
		ShowWnd ShowMainWnd = (ShowWnd)GetProcAddress(hMod, "ShowMainWnd");

		if (ShowMainWnd != NULL)
		{
			BOOL Flag = ShowMainWnd();
			if (Flag == FALSE)
				AfxMessageBox(_T("无法创建主窗口"));
			else if (Flag == -1)
			{
				::FreeLibrary(hMod);

				SHELLEXECUTEINFO ShExecInfo = { 0 };
				ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
				ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
				ShExecInfo.lpDirectory = NULL;
				ShExecInfo.lpFile = _T("Application\\7-Zip\\7z.exe");
				ShExecInfo.lpParameters = _T("x -t7z -pShadow -y ShadowUpdata.7z");
				ShExecInfo.nShow = SW_HIDE;
				ShellExecuteEx(&ShExecInfo);

				AfxGetApp()->BeginWaitCursor();
				WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
				AfxGetApp()->EndWaitCursor();

				DeleteFileA("ShadowUpdata.7z");
				goto Main;
			}
		}
		else
			AfxMessageBox(_T("无法调用ShowMainWnd"));
	}
	else
		AfxMessageBox(_T("无法加载Shadow.dll"));

	::FreeLibrary(hMod);

	_CrtDumpMemoryLeaks();                 // 内存泄露检测
	return FALSE;
}


BOOL CMyApp::Repair(LPCTSTR lpName, LPCTSTR lpType, CString FilePath, CString FileName)
{
	HRSRC hRes = ::FindResource(NULL, lpName, lpType);
	HGLOBAL gl = ::LoadResource(NULL, hRes);
	LPVOID lp = ::LockResource(gl);

	if (!FilePath.IsEmpty() && FilePath != "")
	{
		DWORD Path = GetFileAttributes(FilePath);
		if (Path == 0xFFFFFFFF)     //文件夹不存在
		{
			CreateDirectory(FilePath, NULL);
		}

		if (FilePath.Right(1) != "\\")
			FilePath += "\\";
	}

	HANDLE fp = ::CreateFile(FilePath + FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);

	if (fp == INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD a;
	if (!::WriteFile(fp, lp, SizeofResource(NULL, hRes), &a, NULL))
		return FALSE;

	CloseHandle(fp);
	FreeResource(gl);
	return TRUE;
}
