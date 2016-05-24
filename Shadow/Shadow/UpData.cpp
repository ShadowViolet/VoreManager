// UpData.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Shadow.h"
#include "UpData.h"
#include "afxdialogex.h"
#include <afxinet.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

// CUpData �Ի���

IMPLEMENT_DYNAMIC(CUpData, CDialogEx)

CUpData::CUpData(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_UPDATA_DIALOG, pParent)
{
	bStart = nTerminate = nDownloaded = 0;
}

CUpData::~CUpData()
{
}

void CUpData::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UPDATA_PROGRESS, m_Progress);
}


BEGIN_MESSAGE_MAP(CUpData, CDialogEx)
	ON_BN_CLICKED(IDOK, &CUpData::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CUpData::OnCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CUpData ��Ϣ�������


BOOL CUpData::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CUpData::OnOK()
{
	GetDlgItem(IDOK)->EnableWindow(FALSE);
	m_Downloader = AfxBeginThread(DownloadFile, this);
	bStart = TRUE;
}


void  CUpData::OnFinished()
{
	EndDialog(FALSE);
	theApp.IsUpdata = TRUE;
	::SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_CLOSE, 0, 0);
}


void CUpData::OnTimer(UINT_PTR nIDEvent)
{
	CRect rect;
	GetDlgItem(IDC_OPERATE_STATIC)->GetWindowRect(&rect);
	ScreenToClient(&rect);
	SetDlgItemText(IDC_OPERATE_STATIC, (CString)KBin + _T(" / ") + (CString)FileSize + _T(" ( ") + Perc + _T(" % )") + _T(" �ٶ�: ") + (CString)KBsec + " /s");          //�ļ���С+ �����ش�С+ �����ٶ�
	InvalidateRect(&rect);

	CDialogEx::OnTimer(nIDEvent);
}


UINT CUpData::DownloadFile(LPVOID pParam)
{
	CUpData* pWnd = (CUpData*)pParam;
	char			  filebuf[512];
	CInternetSession  netSession;
	CStdioFile		  *fTargFile = NULL;
	CString           Name, Path;

	int pos = 0;                     // ��ǰ��������λ��
	int nperc = 0, kbrecv = 0;       // �������İٷֱ�,��ȡ�������ݴ�С��KbsΪ��λ��
	double secs, kbsec;	             // ��¼����, �ٶȣ�KB/�룩

	try
	{
		/*GetPrivateProfileString(_T("Config"), _T("File"), NULL, Path.GetBuffer(MAX_PATH), MAX_PATH, _T("./UpData.ini"));
		Path.ReleaseBuffer();*/

		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("����У�����ص�ַ..."));
		fTargFile = netSession.OpenURL(_T("D://Program Files//VoreManager//ShadowUpdata.7z"), 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		Sleep(300);

		if (fTargFile == NULL)
		{
			AfxMessageBox(_T("�޷����ظ������ݰ���������������."));
			return FALSE;
		}

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = (int)fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		pWnd->FileSize = pWnd->GetSize(filesize);           // Զ���ļ���С
		Name = fTargFile->GetFileName();

		// ������Ŀ¼�����µ�Ŀ���ļ�
		CFile fDestFile(Name, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// д���ļ����ֽ���

		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("�������ظ�������..."));
		Sleep(300);

		pWnd->m_Progress.SetRange(0, filesize);
		pWnd->SetTimer(1, 100, NULL);
		while (byteswrite = fTargFile->Read(filebuf, 512))	// ��ȡ�ļ�
		{
			if (pWnd->nTerminate)					// ������ȡ������
			{
				fDestFile.Close();					// �ر����ǵ�Ŀ���ļ�
				fTargFile->Close();					// �ر�Զ���ļ�
				delete fTargFile;					// ɾ��CStdioFile�����Է�ֹй©
				DeleteFile(Name);                   // ɾ�������ļ�
				DeleteFileA("UpData.ini");          // ɾ�������ļ�

				AfxEndThread(0);					// ���������߳�
				break;
			}

			// ���ݿ�ʼʱ���뵱ǰʱ��Ƚϣ���ȡ����
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;	              // �����µĽ�����λ��
			fDestFile.Write(filebuf, byteswrite);	  // ��ʵ������д���ļ�
			pWnd->m_Progress.SetPos(pos);             // ������

			nperc = pos * 100 / filesize;            // ���Ȱٷֱ�
			kbrecv = pos;       		              // ��ȡ�յ�������
			kbsec = kbrecv / secs;                   // ��ȡÿ�����ض��٣�KB��

			pWnd->Perc.Format(_T("%d"), nperc);       // ��ʽ�����Ȱٷֱ�
			pWnd->KBin = pWnd->GetSize(kbrecv);      // ��ʽ�����������ݴ�С��KB��
			pWnd->KBsec = pWnd->GetSize((int)kbsec);  // ��ʽ�������ٶȣ�KB/�룩
		}

		// ������ɣ��ر��ļ�
		fDestFile.Close();
		delete fTargFile;
		pWnd->KillTimer(1);
		pWnd->nDownloaded = TRUE;
	}

	catch (CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error, 255); // ��ȡ������Ϣ
		strerror = error;

		pWnd->KillTimer(1);
		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, strerror);
		pWnd->nDownloaded = FALSE;
		delete fTargFile;

		DeleteFileA("UpData.ini");      // ɾ�������ļ�
		DeleteFile(Name);               // ɾ�������ļ�
		IE->Delete();					// ɾ���쳣�����Է�ֹй©
	}

	if (pWnd->nDownloaded)
	{
		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("������ɣ�����������..."));
		Sleep(500);
		/*SHELLEXECUTEINFO ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.lpFile = _T("Application\\7-Zip\\7z.exe");
		ShExecInfo.lpParameters = _T("x -t7z -pShadow -y " + Name);
		ShExecInfo.nShow = SW_HIDE;
		ShellExecuteEx(&ShExecInfo);

		AfxGetApp()->BeginWaitCursor();
		pWnd->m_Progress.SetRange(0, 100);
		for (int i = 0; i < 99; i++)
		{
			pWnd->m_Progress.SetPos(i);
			Sleep(30);
		}

		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		AfxGetApp()->EndWaitCursor();

		pWnd->m_Progress.SetPos(100);
		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("���ݸ�����ɣ����½���"));
		Sleep(500);

		DeleteFileA("UpData.ini");
		DeleteFile(Name);*/

		pWnd->OnFinished();
	}

	return TRUE;
}


CString CUpData::GetSize(int nSize)
{
	CString Size;
	if (nSize > GB)
	{
		Size.Format(_T("%0.2f GB"), (long double)nSize / GB);
	}
	else if (nSize > MB)
	{
		Size.Format(_T("%0.2f MB"), (double)nSize / MB);
	}
	else if (nSize > KB)
	{
		Size.Format(_T("%0.2f KB"), (double)nSize / KB);
	}
	else
	{
		Size.Format(_T("%d B"), nSize);
	}

	return Size;
}


void CUpData::OnCancel()
{
	if (m_Downloader != NULL)
	{
		DWORD code;
		GetExitCodeThread(m_Downloader->m_hThread, &code);
		if (code == STILL_ACTIVE)
		{
			nTerminate = 1;
			Sleep(500);
		}
	}

	CDialogEx::OnCancel();
}
