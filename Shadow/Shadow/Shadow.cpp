// Shadow.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "Shadow.h"
#include "MainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO:  ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����: 
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ��  ����ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//

// CShadowApp

BEGIN_MESSAGE_MAP(CShadowApp, CWinApp)
END_MESSAGE_MAP()


// CShadowApp ����

CShadowApp::CShadowApp()
{
}


// Ψһ��һ�� CShadowApp ����

CShadowApp theApp;


// CShadowApp ��ʼ��

BOOL CShadowApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}


	//��ӰЧ����ʼ��
	CWndShadow::Initialize(AfxGetInstanceHandle());

	IsUpdata = FALSE;

	return TRUE;
}


int CShadowApp::ExitInstance()
{
	_CrtDumpMemoryLeaks();                 // �ڴ�й¶���

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
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
		return FALSE;
	}

	//�ر�gdiplus�Ļ���
	Gdiplus::GdiplusShutdown(gdiplusToken);

	if (theApp.IsUpdata)
	{
		return -1;
	}
	
	return TRUE;
}
