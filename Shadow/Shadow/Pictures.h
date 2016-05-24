#pragma once


// CPictures 对话框

class CPictures : public CDialogEx
{
	DECLARE_DYNAMIC(CPictures)

public:
	CPictures(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPictures();

	void ShowPictures(CString Path), OnRelease();

	Image *image;
	UINT count;
	WCHAR strGuid[39];
	UINT frameCount;
	PropertyItem* pItem;
	UINT fcount;
	GUID Guid;
	long lPause;
	int size;
	GUID *pDimensionIDs;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PICTURE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnPaint();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
