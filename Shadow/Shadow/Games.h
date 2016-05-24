#pragma once
#include "Flash.h"

// CGames 对话框

class CGames : public CDialogEx
{
	DECLARE_DYNAMIC(CGames)

public:
	CGames(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CGames();

	BOOL OnStartGame(CString Path);
	static UINT StartGame(LPVOID pParam);
	CFlash m_Flash;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAME_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	CWinThread *m_hOperate;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
};
