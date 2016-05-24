#pragma once


// CDocuments 对话框

class CDocuments : public CDialogEx
{
	DECLARE_DYNAMIC(CDocuments)

public:
	CDocuments(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDocuments();

	void OnShowText(CString Path);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DOCUMENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
