// Games.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Shadow.h"
#include "Games.h"
#include "afxdialogex.h"


// CGames �Ի���

IMPLEMENT_DYNAMIC(CGames, CDialogEx)

CGames::CGames(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GAME_DIALOG, pParent)
{

}

CGames::~CGames()
{
}

void CGames::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_SHOCKWAVEFLASH, m_Flash);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH, m_Flash);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH, m_Flash);
}


BEGIN_MESSAGE_MAP(CGames, CDialogEx)
	ON_BN_CLICKED(IDOK, &CGames::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CGames::OnCancel)
END_MESSAGE_MAP()


// CGames ��Ϣ�������


BOOL CGames::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Flash.MoveWindow(0, 0, 750, 481);
	m_Flash.SetZoomRect(-10, -10, -10, -10);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CGames::OnOK()
{
}


BOOL CGames::OnStartGame(CString Path)
{
	CString Ext = PathFindExtension(Path);
	if (Ext.MakeLower() == _T(".swf"))
	{
		m_Flash.StopPlay();
		m_Flash.LoadMovie(0, Path);
		m_Flash.Play();

		m_Flash.SetZoomRect(100, 0, 0, 0);
	}
	else
	{
		m_hOperate = AfxBeginThread(StartGame, (LPVOID)(LPCTSTR)Path);
	}

	DWORD exitcode = 0;
	GetExitCodeThread(m_hOperate, &exitcode);
	return exitcode;

	return TRUE;
}


//�������̣߳�ִ�����к�ʱ����
UINT CGames::StartGame(LPVOID lpParameter)
{
	//������Ϸ����
	CString Path((LPCTSTR)lpParameter), Name = Path.Right(Path.GetLength() - Path.ReverseFind('\\') - 1),
		Directory = Path.Left(Path.GetLength() - Name.GetLength());

	CString Ext = PathFindExtension(Path);
	if (Ext.MakeLower() == _T(".swf"))
	{
		Directory = _T("Application\\FlashPlayer");
		Name = _T("FlashPlayer.exe");
	}
	else if (Ext.MakeLower() == _T(".blend"))
	{
		Directory = _T("Application\\blender");
		Name = _T("blender.exe");
	}

	//���öԳ���
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.lpDirectory = Directory;
	ShExecInfo.lpFile = Name;
	ShExecInfo.lpParameters = Path;
	ShExecInfo.nShow = SW_SHOW;
	BOOL Ret = ShellExecuteEx(&ShExecInfo);

	//�ȴ�����ر�
	AfxGetApp()->BeginWaitCursor();
	AfxGetApp()->GetMainWnd()->ShowWindow(SW_MINIMIZE);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	AfxGetApp()->GetMainWnd()->ShowWindow(SW_RESTORE);
	AfxGetApp()->EndWaitCursor();

	return Ret;
}


void CGames::OnCancel()
{
}
