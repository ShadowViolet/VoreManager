// ChatRoom.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Shadow.h"
#include "ChatRoom.h"
#include "afxdialogex.h"


// CChatRoom �Ի���

IMPLEMENT_DYNAMIC(CChatRoom, CDialogEx)

CChatRoom::CChatRoom(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHATROOM_DIALOG, pParent)
{
	m_connectsock = INVALID_SOCKET;
	m_connecttread = NULL;
	IsShutdown = false;

	Port = _T("53520");
	Addr = _T("115.28.79.163");
	Name = _T("��Ӱ��1");
}


CChatRoom::~CChatRoom()
{
	StopClient();
}


void CChatRoom::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CChatRoom, CDialogEx)
END_MESSAGE_MAP()


// CChatRoom ��Ϣ�������


BOOL CChatRoom::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	OnConnect();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


BOOL CChatRoom::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN: 
			if ((pMsg->wParam == VK_RETURN) && (GetAsyncKeyState(VK_CONTROL) < 0))
				;
			else
			{
				if (GetDlgItem(IDC_TEXT_EDIT)->GetFocus())
					OnSendinfo();

				return TRUE;

			}break;

		case VK_ESCAPE:
			return TRUE;
			break;

		default:
			break;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


DWORD WINAPI CChatRoom::ClientThreadFunc(LPVOID lpParameter)
{
	CString msg;
	CChatRoom *	pChatRoom = (CChatRoom *)lpParameter;
	ASSERT(pChatRoom != NULL);
	pChatRoom->m_connectsock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (pChatRoom->m_connectsock == INVALID_SOCKET)
	{
		AfxMessageBox(_T("�½�Socketʧ�ܣ�"));
		return FALSE;
	}

	int iPort = atoi((CStringA)pChatRoom->Port);
	if (iPort <= 0 || iPort > 65535)
	{
		AfxMessageBox(_T("��������ʵĶ˿ڣ�1 - 65535"));
		goto __Error_End;
	}

	sockaddr_in sever;
	sever.sin_family = AF_INET;
	sever.sin_addr.s_addr = inet_addr((CStringA)pChatRoom->Addr);
	sever.sin_port = htons(iPort);

	if (connect(pChatRoom->m_connectsock, (sockaddr*)&sever, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		//AfxMessageBox(_T("����ʧ�ܣ�"));
		pChatRoom->ShowMsg(_T("ϵͳ��Ϣ�����ӷ�����ʧ�ܣ���������."));
		goto __Error_End;
	}
	else
		pChatRoom->ShowMsg(_T("ϵͳ��Ϣ�����ӷ������ɹ�."));

	while (!(pChatRoom->IsShutdown))
	{
		if (pChatRoom->SOCKET_Select(pChatRoom->m_connectsock, 100, TRUE))
		{
			char buf[1024] = { 0 };
			int irec = recv(pChatRoom->m_connectsock, (char*)buf, 1024, 0);
			if (irec > 0)
			{
				msg = buf;
				pChatRoom->ShowMsg(msg);
			}
			else
			{
				pChatRoom->ShowMsg(_T("��������ֹͣ������������."));
				break;
			}

			Sleep(500);
		}
	}

__Error_End:
	closesocket(pChatRoom->m_connectsock);
	return true;
}


BOOL  CChatRoom::SOCKET_Select(SOCKET hSocket, int nTimeOut, BOOL bRead)
{
	fd_set fdset;
	timeval tv;
	FD_ZERO(&fdset);
	FD_SET(hSocket, &fdset);
	nTimeOut = nTimeOut > 1000 ? 1000 : nTimeOut;
	tv.tv_sec = 0;
	tv.tv_usec = nTimeOut;

	int iRet = 0;
	if (bRead)
	{
		iRet = select(0, &fdset, NULL, NULL, &tv);
	}
	else
	{
		iRet = select(0, NULL, &fdset, NULL, &tv);
	}

	if (iRet <= 0)
	{
		return FALSE;
	}
	else if (FD_ISSET(hSocket, &fdset))
	{
		return TRUE;
	}
	return FALSE;
}


void CChatRoom::OnConnect()
{
	m_connecttread = CreateThread(NULL, 0, ClientThreadFunc, this, 0, NULL);
}


void CChatRoom::ShowMsg(CString Msg)
{
	((CEdit*)GetDlgItem(IDC_MSG_LIST))->SetSel(-1, -1);
	((CEdit*)GetDlgItem(IDC_MSG_LIST))->ReplaceSel(Msg + _T("\r\n"));
}


void CChatRoom::OnSendinfo()
{
	CString msg;
	GetDlgItemText(IDC_TEXT_EDIT, msg);

	if (msg.IsEmpty())
		return;

	int isend = send(m_connectsock, (CStringA)msg, _tcslen(msg) * sizeof(char) * 2, 0);
	msg = Name + _T(" : ") + msg;
	ShowMsg(msg);

	SetDlgItemText(IDC_TEXT_EDIT, _T(""));
}


void CChatRoom::StopClient()//������Դ����
{
	IsShutdown = true;
	DWORD dw = WaitForSingleObject(m_connecttread, 1000);
	if (dw != WAIT_OBJECT_0)
	{
		TerminateThread(m_connecttread, -1);
		closesocket(m_connectsock);
	}

	m_connecttread = NULL;
	m_connectsock = INVALID_SOCKET;
	
	IsShutdown = false;
}
