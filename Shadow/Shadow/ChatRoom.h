#pragma once


// CChatRoom 对话框

class CChatRoom : public CDialogEx
{
	DECLARE_DYNAMIC(CChatRoom)

public:
	CChatRoom(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChatRoom();

	void ShowMsg(CString Msg);
	void StopClient();

	CString Name, Port, Addr;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATROOM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	SOCKET m_connectsock;
	HANDLE m_connecttread;
	bool IsShutdown;
	BOOL SOCKET_Select(SOCKET hSocket, int nTimeOut, BOOL bRead);
	static DWORD WINAPI ClientThreadFunc(LPVOID lpParameter);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSeverop();
	afx_msg void OnSetnet();
	afx_msg void OnStartsever();
	afx_msg void OnClientop();
	afx_msg void OnSendinfo();
	afx_msg void OnConnect();
	afx_msg void OnChangeInputinfo();
	afx_msg void OnClientstop();
	afx_msg void OnSeverstop();
	afx_msg void OnClosewin();
	afx_msg void OnMore();
};
