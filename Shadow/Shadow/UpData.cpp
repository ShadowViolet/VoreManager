// UpData.cpp : 实现文件
//

#include "stdafx.h"
#include "Shadow.h"
#include "UpData.h"
#include "afxdialogex.h"
#include <afxinet.h>

#define KB 1024
#define MB (1024*KB)
#define GB (1024*MB)

// CUpData 对话框

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


// CUpData 消息处理程序


BOOL CUpData::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
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
	SetDlgItemText(IDC_OPERATE_STATIC, (CString)KBin + _T(" / ") + (CString)FileSize + _T(" ( ") + Perc + _T(" % )") + _T(" 速度: ") + (CString)KBsec + " /s");          //文件大小+ 已下载大小+ 下载速度
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

	int pos = 0;                     // 当前进度条的位置
	int nperc = 0, kbrecv = 0;       // 进度条的百分比,获取到的数据大小（Kbs为单位）
	double secs, kbsec;	             // 记录秒数, 速度（KB/秒）

	try
	{
		/*GetPrivateProfileString(_T("Config"), _T("File"), NULL, Path.GetBuffer(MAX_PATH), MAX_PATH, _T("./UpData.ini"));
		Path.ReleaseBuffer();*/

		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("正在校验下载地址..."));
		fTargFile = netSession.OpenURL(_T("D://Program Files//VoreManager//ShadowUpdata.7z"), 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
		Sleep(300);

		if (fTargFile == NULL)
		{
			AfxMessageBox(_T("无法下载更新数据包，请检查网络连接."));
			return FALSE;
		}

		COleDateTime dlStart = COleDateTime::GetCurrentTime();
		int filesize = (int)fTargFile->SeekToEnd();
		fTargFile->SeekToBegin();
		pWnd->FileSize = pWnd->GetSize(filesize);           // 远程文件大小
		Name = fTargFile->GetFileName();

		// 在下载目录创建新的目标文件
		CFile fDestFile(Name, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
		int byteswrite;		// 写入文件的字节数

		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("正在下载更新数据..."));
		Sleep(300);

		pWnd->m_Progress.SetRange(0, filesize);
		pWnd->SetTimer(1, 100, NULL);
		while (byteswrite = fTargFile->Read(filebuf, 512))	// 读取文件
		{
			if (pWnd->nTerminate)					// 如果点击取消下载
			{
				fDestFile.Close();					// 关闭我们的目标文件
				fTargFile->Close();					// 关闭远程文件
				delete fTargFile;					// 删除CStdioFile对象，以防止泄漏
				DeleteFile(Name);                   // 删除更新文件
				DeleteFileA("UpData.ini");          // 删除配置文件

				AfxEndThread(0);					// 结束下载线程
				break;
			}

			// 根据开始时间与当前时间比较，获取秒数
			COleDateTimeSpan dlElapsed = COleDateTime::GetCurrentTime() - dlStart;
			secs = dlElapsed.GetTotalSeconds();
			pos = pos + byteswrite;	              // 设置新的进度条位置
			fDestFile.Write(filebuf, byteswrite);	  // 将实际数据写入文件
			pWnd->m_Progress.SetPos(pos);             // 进度条

			nperc = pos * 100 / filesize;            // 进度百分比
			kbrecv = pos;       		              // 获取收到的数据
			kbsec = kbrecv / secs;                   // 获取每秒下载多少（KB）

			pWnd->Perc.Format(_T("%d"), nperc);       // 格式化进度百分比
			pWnd->KBin = pWnd->GetSize(kbrecv);      // 格式化已下载数据大小（KB）
			pWnd->KBsec = pWnd->GetSize((int)kbsec);  // 格式化下载速度（KB/秒）
		}

		// 下载完成，关闭文件
		fDestFile.Close();
		delete fTargFile;
		pWnd->KillTimer(1);
		pWnd->nDownloaded = TRUE;
	}

	catch (CInternetException *IE)
	{
		CString strerror;
		TCHAR error[255];

		IE->GetErrorMessage(error, 255); // 获取错误消息
		strerror = error;

		pWnd->KillTimer(1);
		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, strerror);
		pWnd->nDownloaded = FALSE;
		delete fTargFile;

		DeleteFileA("UpData.ini");      // 删除配置文件
		DeleteFile(Name);               // 删除更新文件
		IE->Delete();					// 删除异常对象，以防止泄漏
	}

	if (pWnd->nDownloaded)
	{
		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("下载完成，更新数据中..."));
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
		pWnd->SetDlgItemText(IDC_OPERATE_STATIC, _T("数据更新完成，更新结束"));
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
