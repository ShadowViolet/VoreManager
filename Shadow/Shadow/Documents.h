#pragma once


// CDocuments �Ի���

class CDocuments : public CDialogEx
{
	DECLARE_DYNAMIC(CDocuments)

public:
	CDocuments(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDocuments();

	void OnShowText(CString Path);

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DOCUMENT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
