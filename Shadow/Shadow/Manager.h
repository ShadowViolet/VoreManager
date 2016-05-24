#pragma once
#include "SortListCtrl.h"


// CManager 对话框

class CManager : public CDialogEx
{
	DECLARE_DYNAMIC(CManager)

public:
	CManager(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CManager();

	void OnSetImg(CString ImgPath);
	void OnRefresh(), OnSearch();
	BOOL OnAddData(CString Name, CString Path);

	void OnAddFloder(CString Path);
	int CountFile(CString Path), ParameterType;


	CImageList m_Imagelist;
	CSortListCtrl m_Resource_List;

	BOOL IsFind, IsRunning;
	static UINT FullScreen(LPVOID pParam), Operate(LPVOID pParam);

	HDROP hDropInfo;
	CWinThread *m_hOperate;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MANAGER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnAddResource();
	afx_msg void OnModifyResource();
	afx_msg void OnDeleteResource();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnLvnItemchangedResourceList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickResourceList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickResourceList(NMHDR *pNMHDR, LRESULT *pResult);
};
