#pragma once
#include "MyProgress.h"


// CUpData 对话框

class CUpData : public CDialogEx
{
	DECLARE_DYNAMIC(CUpData)

public:
	CUpData(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUpData();

	static UINT DownloadFile(LPVOID pParam);
	void OnFinished();

	BOOL bStart, nTerminate, nDownloaded;
	CString GetSize(int nSize), CurrentVersion, ServerVersion, FileSize, KBin, KBsec, Perc;
	
	CWinThread* m_Downloader;
	CMyProgress m_Progress;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UPDATA_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
