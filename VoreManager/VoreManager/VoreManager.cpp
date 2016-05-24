
// VoreManager.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "VoreManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyApp

BEGIN_MESSAGE_MAP(CMyApp, CWinApp)
END_MESSAGE_MAP()


// CMyApp ����

CMyApp::CMyApp()
{
}


// Ψһ��һ�� CMyApp ����

CMyApp theApp;


// CMyApp ��ʼ��

BOOL CMyApp::InitInstance()
{
	CWinApp::InitInstance();

	CFileFind Finder;               // ������̬���ӿ�
	BOOL Status = Finder.FindFile(_T("Shadow.dll"));
	if (!Status)
		if(!Repair(MAKEINTRESOURCE(IDR_MAIN_LIBRARY), _T("Library"), NULL, _T("Shadow.dll")))
			AfxMessageBox(_T("�޷��Զ��޸�ȱʧ�ĺ������ӿ⣬���������غ����ԣ�"));

	Status = Finder.FindFile(_T("avcodec-54.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_AVCODEC_54_LIBRARY), _T("Library"), NULL, _T("avcodec-54.dll")))
			AfxMessageBox(_T("�޷��Զ��޸�ȱʧ����Ƶ�������ӿ⣬���������غ����ԣ�"));

	Status = Finder.FindFile(_T("avformat-54.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_AVFORMAT_54_LIBRARY), _T("Library"), NULL, _T("avformat-54.dll")))
			AfxMessageBox(_T("�޷��Զ��޸�ȱʧ����Ƶ��ʽ���ӿ⣬���������غ����ԣ�"));

	Status = Finder.FindFile(_T("avutil-52.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_AVUTIL_52_LIBRARY), _T("Library"), NULL, _T("avutil-52.dll")))
			AfxMessageBox(_T("�޷��Զ��޸�ȱʧ����Ƶ�������ӿ⣬���������غ����ԣ�"));

	Status = Finder.FindFile(_T("SDL.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_SDL_LIBRARY), _T("Library"), NULL, _T("SDL.dll")))
			AfxMessageBox(_T("�޷��Զ��޸�ȱʧ������Ƶ�������ӿ⣬���������غ����ԣ�"));

	Status = Finder.FindFile(_T("swscale-2.dll"));
	if (!Status)
		if (!Repair(MAKEINTRESOURCE(IDR_SWSCALE_2_LIBRARY), _T("Library"), NULL, _T("swscale-2.dll")))
			AfxMessageBox(_T("�޷��Զ��޸�ȱʧ����Ƶ�������ӿ⣬���������غ����ԣ�"));

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
				AfxMessageBox(_T("�޷�����������"));
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
			AfxMessageBox(_T("�޷�����ShowMainWnd"));
	}
	else
		AfxMessageBox(_T("�޷�����Shadow.dll"));

	::FreeLibrary(hMod);

	_CrtDumpMemoryLeaks();                 // �ڴ�й¶���
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
		if (Path == 0xFFFFFFFF)     //�ļ��в�����
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
