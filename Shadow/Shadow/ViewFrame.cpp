#include "stdafx.h"
#include "ViewFrame.h"

#include <comdef.h>
#include <shlwapi.h>
#include <cassert>


std::auto_ptr<CImageInfo> CImageInfo::sm_inst;


CImageInfo::CImageInfo(void)
{
}


CImageInfo::~CImageInfo(void)
{
	// Release�������е��ã���ΪGdiplusShutdown��ԭ�򼺵��õ�ԭ��ᵼ���ڴ����
	// Release();
}


CImageInfo * CImageInfo::Instance()
{
	if (sm_inst.get() == 0)
		sm_inst = auto_ptr<CImageInfo>(new CImageInfo);
	return sm_inst.get();

	/* FOLLOWING CODE WORKS ONLY IN VC7
	if(sm_inst.get() == 0)
	sm_inst.reset(new CMachine);
	return sm_inst.get();
	*/
}


Image* CImageInfo::ImageFromResource(UINT nFile)
{
	Image* pImage = NULL;
	if (m_mpImage.find(nFile) != m_mpImage.end())
	{
		return m_mpImage[nFile];
	}

	pImage = FromResource(nFile);
	if (pImage != NULL)
	{
		m_mpImage[nFile] = pImage;
	}

	return pImage;
}


void CImageInfo::Release()
{
	for (map<UINT, Image*>::iterator ciIter = m_mpImage.begin();
		ciIter != m_mpImage.end(); ciIter++)
	{
		delete ciIter->second;
	}
	m_mpImage.clear();
}


Image* CImageInfo::FromResource(UINT uId)
{
	HMODULE hModule = AfxGetResourceHandle();
	HRSRC hRes = ::FindResource(hModule, MAKEINTRESOURCE(uId), _T("PNG"));
	if (hRes == NULL)
	{
		assert(FALSE);
		return NULL;
	}

	DWORD dwSize = ::SizeofResource(hModule, hRes);
	if (dwSize == 0)
	{
		assert(FALSE);
		return NULL;
	}

	HGLOBAL hGlobal = ::LoadResource(hModule, hRes);
	if (hGlobal == NULL)
	{
		assert(FALSE);
		return NULL;
	}

	LPVOID pBuffer = ::LockResource(hGlobal);
	if (pBuffer == NULL)
	{
		assert(FALSE);
		::FreeResource(hGlobal);
		return NULL;
	}


	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (hMem == NULL)
	{
		assert(FALSE);
		::FreeResource(hGlobal);
		return NULL;
	}

	void* pData = GlobalLock(hMem);
	memcpy(pData, (void *)hGlobal, dwSize);
	GlobalUnlock(hMem);

	Image *pRtImg = NULL;
	IStream* pStream = NULL;
	if (CreateStreamOnHGlobal(hMem, TRUE, &pStream) == S_OK)
	{
		pRtImg = Image::FromStream(pStream, FALSE);
		pStream->Release();
	}

	::FreeResource(hGlobal);

	return pRtImg;
}


// ���ư�ť
void CXButton::DrawButton(Gdiplus::Graphics&  graphics)
{
	// ��ȡ��ťͼƬ��Ϣ
	Image* pImage = CImageInfo::Instance()->ImageFromResource(m_nImageContorl);
	UINT iCount = m_nImageCount;
	if (m_bCheck && m_nAltImageContorl != 0)
	{
		pImage = CImageInfo::Instance()->ImageFromResource(m_nAltImageContorl);;
		iCount = m_nAltImageCount;
	}

	// ��ȡ��ť״̬��Ϣ
	int	iButtonIndex = 0;
	if (m_bDisabled && iCount >= 4) iButtonIndex = 3;
	else if (m_bPressed && iCount >= 3)iButtonIndex = 2;
	else if (m_bHovering && iCount >= 2)iButtonIndex = 1;
	else iButtonIndex = 0;

	// ��ָ��λ�û��ư�ť
	int iWidth = pImage->GetWidth() / iCount;
	int iHeight = pImage->GetHeight();
	RectF grect;
	grect.X = (Gdiplus::REAL)m_rcRect.left, grect.Y = (Gdiplus::REAL)m_rcRect.top; grect.Width = (Gdiplus::REAL)m_rcRect.Width(); grect.Height = (Gdiplus::REAL)m_rcRect.Height();
	graphics.DrawImage(pImage, grect, (Gdiplus::REAL)iWidth*iButtonIndex, 0, (Gdiplus::REAL)iWidth, (Gdiplus::REAL)iHeight, UnitPixel);
}


// ������ť
void CXButton::CreateButton(CWnd* pParent, LPRECT lpRect, UINT nImage, UINT nCount /*= 4*/, UINT nAltImage /*= _T("")*/, UINT nAltCount /*= 0*/, UINT nID/* = 0*/)
{
	m_pParent = pParent;
	m_rcRect = lpRect;
	m_nImageContorl = nImage;
	m_nImageCount = nCount;
	m_nAltImageContorl = nAltImage;
	m_nAltImageCount = nAltCount;
	m_nID = nID;
}


// �л���ť��С����
#define BTN_WIDTH  170
#define BTN_HEIGHT 63


// �Խ���ťId����
#define IDC_CLOSE		4000
#define IDC_BTN_1		4001
#define IDC_BTN_2		4002
#define IDC_BTN_3		4003
#define IDC_BTN_4		4004
#define IDC_BTN_5		4005
#define IDC_BTN_6		4006
#define IDC_BTN_7		4007


// ͼƬ�ƶ���ʱ�����
#define TIMER_MOVE		1


// ��׼���캯��
CViewFrame::CViewFrame()
{
	m_iHovering = -1;
	m_iSelected = -1;

	m_iXButtonHovering = -1;

	m_bHovering = FALSE;
	m_bTracking = FALSE;

	m_bAutoMove = FALSE;
	m_bMouseDown = FALSE;
	m_iMouseMoveWidth = 0;

	m_iMoveWidthPerSecond = 0;
}


// ��׼��������
CViewFrame::~CViewFrame()
{
}


// ������Ϣӳ���
BEGIN_MESSAGE_MAP(CViewFrame, CDialog)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDC_CLOSE, &CViewFrame::OnBtnClose)
	ON_MESSAGE(WM_MOUSELEAVE, &CViewFrame::OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, &CViewFrame::OnMouseHover)
END_MESSAGE_MAP()


// WM_ERASEBKGND��Ϣ������
BOOL CViewFrame::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}


// WM_CREATE��Ϣ������
int  CViewFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetWindowText(_T("��Դ����"));

	// ��ҳ��ǰ����Դ
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDB_FRAME);

	// ��ҳ�汳����Դ
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDB_BACK_FRAME);

	// ���������ڴ�С��λ��
	CRect cClientRect;
	GetClientRect(&cClientRect);
	cClientRect.right = pFrameImage->GetWidth();
	cClientRect.bottom = pFrameImage->GetHeight();
	MoveWindow(&cClientRect);
	CenterWindow(GetParent());

	// �������ڱ�ΪԲ�Ǵ���
	CRgn hRgnClient;
	hRgnClient.CreateRoundRectRgn(0, pFrameImage->GetHeight() - pBackFree->GetHeight(), pFrameImage->GetWidth() + 1, pFrameImage->GetHeight() + 1, 7, 7);
	SetWindowRgn(hRgnClient,FALSE);

	// �����ҳ����ʾ��Ϣ
	AddButton(_T("����"),    IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("����"),    IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("����"),    IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("��Ů"),    IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("P.O.V"),     IDB_GAME_ITEM, IDB_GAME_LIST);
	AddButton(_T("S.T.P"),     IDB_GAME_ITEM, IDB_GAME_LIST);

	// ���õ�ǰ��ʾ��1����ҳ��
	m_iSelected = 0;

	CRect rtButton;

	// �����رհ�ť
	rtButton.top = pFrameImage->GetHeight() - pBackFree->GetHeight();
	rtButton.bottom = rtButton.top + 22;
	rtButton.right = cClientRect.Width() - 4;
	rtButton.left = rtButton.right - 47;
	//dcControl.CreateButton(this, rtButton, IDR_BTN_CLOSE, 4, NULL, 0, IDC_CLOSE);
	//m_XButtonList.push_back(dcControl);
	//m_XButtonRectList.push_back(rtButton);


	// ������ҳ��1�жԻ���
	rtButton.left = 374;
	rtButton.top =202;
	rtButton.bottom = rtButton.top + 52;
	rtButton.right = rtButton.left + 158;


	// Manager
	m_Manager = new CManager;
	if (m_Manager->Create(IDD_MANAGER_DIALOG, this))
	{
		m_Manager->SetWindowPos(NULL, 770, 8, 242, 481, SWP_DRAWFRAME);
		m_Manager->ShowWindow(SW_SHOW);
	}
	else
	{
		m_Manager = NULL;
		AfxMessageBox(_T("�޷�������Դ����Ի���"));
	}

	// Picture View
	m_Pictures = new CPictures;
	if (m_Pictures->Create(IDD_PICTURE_DIALOG, this))
	{
		m_Pictures->SetWindowPos(NULL, 7, 8, 750, 481, SWP_DRAWFRAME);
		m_Pictures->ShowWindow(SW_SHOW);
	}
	else
	{
		m_Pictures = NULL;
		AfxMessageBox(_T("�޷�����ͼƬ��ͼ�Ի���"));
	}

	// Video View
	m_Videos = new CVideos;
	if (m_Videos->Create(IDD_VIDEO_DIALOG, this))
	{
		m_Videos->SetWindowPos(NULL, 7, 8, 750, 481, SWP_DRAWFRAME);
		m_Videos->ShowWindow(SW_HIDE);
	}
	else
	{
		m_Videos = NULL;
		AfxMessageBox(_T("�޷�������Ƶ��ͼ�Ի���"));
	}

	// Audio View
	m_Audios = new CAudios;
	if (m_Audios->Create(IDD_AUDIO_DIALOG, this))
	{
		m_Audios->SetWindowPos(NULL, 7, 8, 750, 481, SWP_DRAWFRAME);
		m_Audios->ShowWindow(SW_HIDE);
	}
	else
	{
		m_Audios = NULL;
		AfxMessageBox(_T("�޷�������Ƶ��ͼ�Ի���"));
	}

	// Game View
	m_Games = new CGames;
	if (m_Games->Create(IDD_GAME_DIALOG, this))
	{
		m_Games->SetWindowPos(NULL, 7, 8, 750, 481, SWP_DRAWFRAME);
		m_Games->ShowWindow(SW_HIDE);
	}
	else
	{
		m_Games = NULL;
		AfxMessageBox(_T("�޷�������Ϸ��ͼ�Ի���"));;
	}

	// Documents View
	m_Documents = new CDocuments;
	if (m_Documents->Create(IDD_DOCUMENT_DIALOG, this))
	{
		m_Documents->SetWindowPos(NULL, 7, 8, 750, 481, SWP_DRAWFRAME);
		m_Documents->ShowWindow(SW_HIDE);
	}
	else
	{
		m_Documents = NULL;
		AfxMessageBox(_T("�޷������ĵ���ͼ�Ի���"));;
	}

	// ChatRoom
	m_ChatRoom = new CChatRoom;
	if (m_ChatRoom->Create(IDD_CHATROOM_DIALOG, this))
	{
		m_ChatRoom->SetWindowPos(NULL, 7, 8, 750, 481, SWP_DRAWFRAME);
		m_ChatRoom->ShowWindow(SW_HIDE);
	}
	else
	{
		m_ChatRoom = NULL;
		AfxMessageBox(_T("�޷�����Vore�����ҶԻ���"));;
	}


	//// ������ҳ��2�а�ť
	//rtButton.left = pFrameImage->GetWidth() * 1 + 385;
	//rtButton.top = 202;
	//rtButton.bottom = rtButton.top + 52;
	//rtButton.right = rtButton.left + 158;
	//dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_2);
	//m_XButtonList.push_back(dcControl);
	//m_XButtonRectList.push_back(rtButton);


	//// ������ҳ��3�а�ť
	//rtButton.left = pFrameImage->GetWidth() * 2 + 385;
	//rtButton.top = 202;
	//rtButton.bottom = rtButton.top + 52;
	//rtButton.right = rtButton.left + 158;
	//dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_3);
	//m_XButtonList.push_back(dcControl);
	//m_XButtonRectList.push_back(rtButton);


	//// ������ҳ��4�а�ť
	//rtButton.left = pFrameImage->GetWidth() * 3 + 385;
	//rtButton.top = 202;
	//rtButton.bottom = rtButton.top + 52;
	//rtButton.right = rtButton.left + 158;
	//dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_4);
	//m_XButtonList.push_back(dcControl);
	//m_XButtonRectList.push_back(rtButton);


	//// ������ҳ��5�а�ť
	//rtButton.left = pFrameImage->GetWidth() * 4 + 385;
	//rtButton.top = 202;
	//rtButton.bottom = rtButton.top + 52;
	//rtButton.right = rtButton.left + 158;
	//dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_5);
	//m_XButtonList.push_back(dcControl);
	//m_XButtonRectList.push_back(rtButton);


	//// ������ҳ��6�� ��һ����ť
	//rtButton.left = pFrameImage->GetWidth() * 5 + 385;
	//rtButton.top = 202;
	//rtButton.bottom = rtButton.top + 52;
	//rtButton.right = rtButton.left + 158;
	//dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_6);
	//m_XButtonList.push_back(dcControl);
	//m_XButtonRectList.push_back(rtButton);


	//// ������ҳ��6�� �ڶ�����ť
	//rtButton.left = pFrameImage->GetWidth() * 5 + 385;
	//rtButton.top = 382;
	//rtButton.bottom = rtButton.top + 52;
	//rtButton.right = rtButton.left + 158;
	//dcControl.CreateButton(this, rtButton, IDB_BTN_GREEN, 3, NULL, 0, IDC_BTN_7);
	//m_XButtonList.push_back(dcControl);
	//m_XButtonRectList.push_back(rtButton);

	return 0;
}


// WM_PAINT��Ϣ������
void CViewFrame::OnPaint()
{
	// ʹ��UNICODE�ַ�ת��
	USES_CONVERSION;

	CPaintDC dc(this);
	CMemoryDC dcMem(&dc);

	// ��ȡ���ھ���λ��
	CRect rtClient;
	GetClientRect(rtClient);

	// ��ȡ�ػ����λ��
	CRect rcPaint;
	dcMem.GetClipBox(&rcPaint);

	Gdiplus::Graphics graphics(dcMem.m_hDC);

	// ��ҳ��ǰ����Դ
	Image* pFrameImage = CImageInfo::Instance()->ImageFromResource(IDB_FRAME);

	// ��ť�Ϸ���ͷ��Դ
	Image *pArrowImage = CImageInfo::Instance()->ImageFromResource(IDB_ARROW);

	// ��ҳ�汳����Դ
	Image* pBackFree = CImageInfo::Instance()->ImageFromResource(IDB_BACK_FRAME);

	// �������Ϸ��°����԰�ť�ı���
	RectF gdiRect;
	gdiRect.X = 19, gdiRect.Y = 0, gdiRect.Width = 120, gdiRect.Height = 5;
	graphics.DrawImage(pBackFree, gdiRect, 0, 0, 120, 5,UnitPixel);

	// ��������ܱ���ͼƬ
	for(int i = -1; i <= (int)m_ButtonList.size(); i++)
	{
		// ���㵱ǰͼƬ��ʾλ���Ƿ�����������ʾ��Χ
		int iCurrentX = (i*(int)pFrameImage->GetWidth() - (m_iSelected * (int)pFrameImage->GetWidth())+m_iMouseMoveWidth);
		if(iCurrentX > rtClient.Width() ||  iCurrentX + rtClient.Width() < 0)
		{
			continue;
		}

		Image* pBackImage = NULL;
		if(i == -1 || i == (int)m_ButtonList.size())
		{
			pBackImage = pBackFree;
		}
		else
		{
			pBackImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nBack);
		}

		gdiRect.X = (Gdiplus::REAL)iCurrentX;
		gdiRect.Y = (Gdiplus::REAL)pFrameImage->GetHeight() - (Gdiplus::REAL)pBackImage->GetHeight();
		gdiRect.Height = (Gdiplus::REAL)pBackImage->GetHeight();
		gdiRect.Width = (Gdiplus::REAL)pBackImage->GetWidth();
		graphics.DrawImage(pBackImage, gdiRect, 0, 0, (Gdiplus::REAL)pBackImage->GetWidth(), (Gdiplus::REAL)pBackImage->GetHeight(), UnitPixel);
	}
  
	// ���������ǰ��ͼƬ
	graphics.DrawImage(pFrameImage, 0, 0, pFrameImage->GetWidth(), pFrameImage->GetHeight());

	// ���ƽ����л���ť�Ϸ��ļ�ͷ
	int iCurrent = ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth)*BTN_WIDTH/(int)pFrameImage->GetWidth();
	if(iCurrent >= 0)
	{
		gdiRect.X = 0, gdiRect.Y = 490; gdiRect.Width = (Gdiplus::REAL)iCurrent; gdiRect.Height = (Gdiplus::REAL)pArrowImage->GetHeight();
		graphics.DrawImage(pArrowImage, gdiRect, 0, 0, 1, (Gdiplus::REAL)pArrowImage->GetHeight(), UnitPixel);
	}

	gdiRect.X = (Gdiplus::REAL)iCurrent, gdiRect.Y = 490; gdiRect.Width = (Gdiplus::REAL)pArrowImage->GetWidth(); gdiRect.Height = (Gdiplus::REAL)pArrowImage->GetHeight();
	graphics.DrawImage(pArrowImage, gdiRect, 0, 0, (Gdiplus::REAL)pArrowImage->GetWidth(), (Gdiplus::REAL)pArrowImage->GetHeight(), UnitPixel);

	if( iCurrent + (int)pArrowImage->GetWidth() < (int)pFrameImage->GetWidth())
	{
		gdiRect.X = (Gdiplus::REAL)iCurrent + (int)pArrowImage->GetWidth(), gdiRect.Y = 490; gdiRect.Width = (Gdiplus::REAL)pFrameImage->GetWidth() - (iCurrent + pArrowImage->GetWidth()); gdiRect.Height = (Gdiplus::REAL)pArrowImage->GetHeight();
		graphics.DrawImage(pArrowImage, gdiRect, 0, 0, 1, (Gdiplus::REAL)pArrowImage->GetHeight(), UnitPixel);
	}

	// ������չʾ����ͼƬ�л���ť
	StringFormat stringFormat;
	stringFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	stringFormat.SetAlignment(StringAlignmentNear);
	stringFormat.SetLineAlignment(StringAlignmentNear);

	LOGFONT lfFont;
	memset(&lfFont, 0, sizeof(lfFont));
	lfFont.lfHeight = -15;
	lfFont.lfWeight |= FW_BOLD;
	lstrcpy(lfFont.lfFaceName, _T("����"));
	Font font(dcMem.GetSafeHdc(), &lfFont);
	Image *pBackHover = CImageInfo::Instance()->ImageFromResource(IDB_HOVER);
	for(int i = 0; i < (int)m_ButtonList.size(); i++)
	{
		CRect rcButton;
		GetButtonRect( i, rcButton );
		Point point(rcButton.left, rcButton.top);
		// �жϰ�ť�Ƿ���Ҫˢ��
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcButton, rcPaint))
		{
			continue;
		}

		// ��ȡ��ť״̬
		int iOffset = 0;
		if(m_iSelected == i)iOffset = 2;
		else if(m_iHovering == i)iOffset = 1;
		else iOffset = 0;

		// ���ư�ť����
		RectF gdiRect;
		gdiRect.X = (Gdiplus::REAL)point.X, gdiRect.Y = (Gdiplus::REAL)point.Y + 192; gdiRect.Width = (Gdiplus::REAL)pBackHover->GetWidth() / 3; gdiRect.Height = (Gdiplus::REAL)pBackHover->GetHeight();
		graphics.DrawImage(pBackHover, gdiRect, (Gdiplus::REAL)pBackHover->GetWidth() / 3 * iOffset, 0, (Gdiplus::REAL)pBackHover->GetWidth() / 3, (Gdiplus::REAL)pBackHover->GetHeight(), UnitPixel);

		// ���ư�ťͼ��
		Image* pIconImage = CImageInfo::Instance()->ImageFromResource(m_ButtonList[i].nIcon);
		gdiRect.X = (Gdiplus::REAL)point.X + 10, gdiRect.Y = (Gdiplus::REAL)point.Y + 198; gdiRect.Width = (Gdiplus::REAL)pIconImage->GetWidth(); gdiRect.Height = (Gdiplus::REAL)pIconImage->GetHeight();
		graphics.DrawImage(pIconImage, gdiRect);

		// ���ư�ť����
		CString sName = m_ButtonList[i].sName;
		CStringW wName = T2W(sName.GetBuffer());
		SolidBrush brush((ARGB)Color::White);
		gdiRect.X = (Gdiplus::REAL)point.X + 49, gdiRect.Y = (Gdiplus::REAL)point.Y + 207; gdiRect.Width = (Gdiplus::REAL)rcButton.Width() - 52; gdiRect.Height = (Gdiplus::REAL)rcButton.Height() - 15;
		graphics.DrawString(wName, wName.GetLength(), &font, gdiRect,&stringFormat, &brush);
	}

	// ���ƽ���رհ�ť��������ť
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		CRect rcControl;
		if(i == 0)
		{
			// ��ǰ��ť�Ǵ��ڹرհ�ť
			dcControl.GetRect(&rcControl);
		}
		else
		{
			// ��ǰ��ť����ҳ���еĹ��ܰ�ť
			rcControl = m_XButtonRectList[i];
			rcControl.left -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			rcControl.right -= ((m_iSelected * (int)pFrameImage->GetWidth())-m_iMouseMoveWidth);
			dcControl.SetRect(&rcControl);
		}
		// �жϵ�ǰ��ť�Ƿ���Ҫ�ػ�
		if(!rcPaint.IsRectEmpty() && !CRect().IntersectRect(&rcControl, rcPaint))
		{
			continue;
		}

		dcControl.DrawButton(graphics);
	}
}


// WM_LBUTTONDOWN��Ϣ������
void CViewFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_bMouseDown)
		return;

	// ��������Ƿ����л���ť��
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}

		// ����������ͼƬ�Զ��ƶ�����ת��ѡ��ͼƬ
		CRect rtClient;
		GetClientRect(&rtClient);
		m_iMouseMoveWidth = (iButton-m_iSelected)*rtClient.Width();
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;

		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;

		m_iSelected = iButton;
		return;
	}

	// ��������Ƿ���������ť��
	iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		if(m_bAutoMove)
		{
			return;
		}
		m_XButtonList[iButton].LButtonDown();
		return;
	}

	// ��괦�ڱ���λ��
	if(point.y < 28)
	{
		//PostMessage(WM_NCLBUTTONDOWN,HTCAPTION,MAKELPARAM(point.x,point.y));
		return;
	}

	// ��괦�ڽ���ͼƬ֮��
	if(m_bAutoMove)
	{
		// ��ǰͼƬ�����Զ��ƶ�
		KillTimer(TIMER_MOVE);
		m_bAutoMove = FALSE;
		m_iMouseDownX = point.x - m_iMouseMoveWidth;
	}
	else
	{
		m_iMouseDownX = point.x;
	}
	m_bMouseDown = TRUE;
	SetCapture();
}


// WM_MOUSEMOVE��Ϣ������
void CViewFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	// ������뿪�¼����и���
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE|TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	if(m_bAutoMove)
	{
		return;
	}

	// ����Ѿ���������ͼƬ����
	if(m_bMouseDown)
	{
		CRect rtClient;
		GetClientRect(&rtClient);
		m_iMouseMoveWidth = point.x - m_iMouseDownX;
		if(m_iSelected == 0 && m_iMouseMoveWidth > rtClient.Width())
		{
			m_iMouseMoveWidth = rtClient.Width();
		}
		if(m_iSelected == (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width())
		{
			m_iMouseMoveWidth = -rtClient.Width();
		}
		Invalidate();
		return;
	}

	// ��������Ƿ����л���ť��
	CRect rtButton;
	int iButton = HitTest(point, rtButton);
	if(iButton != m_iHovering)
	{
		/*if(m_iHovering != -1)
		{
			CRect oldRect;
			GetButtonRect(m_iHovering, oldRect);
			m_iHovering = -1;
			InvalidateRect(oldRect);
		}
		if(iButton != -1)
		{
			m_iHovering = iButton;
			InvalidateRect(rtButton);
		};*/
	}

	// ��������Ƿ���������ť��
	iButton = XButtonHitTest(point, rtButton);
	if(iButton != m_iXButtonHovering)
	{
		if(m_iXButtonHovering != -1)
		{
			m_XButtonList[m_iXButtonHovering].MouseLeave();
			m_iXButtonHovering = -1;
		}
		if(iButton != -1)
		{
			m_iXButtonHovering = iButton;
			m_XButtonList[m_iXButtonHovering].MouseHover();
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}


// WM_LBUTTONUP��Ϣ������
void CViewFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	// ����Ѿ���������ͼƬ����
	if(m_bMouseDown)
	{
		m_bMouseDown = FALSE;
		ReleaseCapture();

		CRect rtClient;
		GetClientRect(&rtClient);
		// ��������϶���
		if(m_iMouseMoveWidth > 0)
		{
			if(m_iSelected > 0 && m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth -= rtClient.Width();
				m_iSelected--;
			}
		}
		// ��������϶���
		else
		{
			if(m_iSelected < (int)m_ButtonList.size()-1 && -m_iMouseMoveWidth > rtClient.Width()/4)
			{
				m_iMouseMoveWidth += rtClient.Width();
				m_iSelected++;
			}
		}
		// ����������ͼƬ�Զ��ƶ����ָ���ǰͼƬ������ת��ѡ��ͼƬ
		m_iMoveWidthPerSecond = (m_iMouseMoveWidth>0?m_iMouseMoveWidth:-m_iMouseMoveWidth)/10;
		if(m_iMoveWidthPerSecond < 20)m_iMoveWidthPerSecond = 20;
		SetTimer(TIMER_MOVE, 30, NULL);
		m_bAutoMove = TRUE;
		return;
	}

	// ��������Ƿ���������ť��
	CRect rtButton;
	int iButton = XButtonHitTest(point, rtButton);
	if(iButton != -1)
	{
		m_XButtonList[iButton].LButtonUp();
	}
}


// WM_LBUTTONDBLCLK��Ϣ������
void CViewFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}


// WM_TIMER��Ϣ������
void CViewFrame::OnTimer(UINT_PTR nIDEvent)
{
	if(!m_bAutoMove)
		return;

	// ����Ѿ������϶������Ѿ�ѡ����ͼƬ
	if(m_iMouseMoveWidth > 0)
	{
		if(m_iMouseMoveWidth - m_iMoveWidthPerSecond <= 0)
		{
			m_iMouseMoveWidth = 0;
			Invalidate();
			KillTimer(TIMER_MOVE);
			m_bAutoMove = FALSE;
		}
		else
		{
			m_iMouseMoveWidth-= m_iMoveWidthPerSecond;
			Invalidate();
		}
	}
	// ����Ѿ������϶������Ѿ�ѡ���ҷ�ͼƬ
	else
	{
		if(m_iMouseMoveWidth + m_iMoveWidthPerSecond >= 0)
		{
			m_iMouseMoveWidth = 0;
			Invalidate();
			KillTimer(TIMER_MOVE);
			m_bAutoMove = FALSE;
		}
		else
		{
			m_iMouseMoveWidth+= m_iMoveWidthPerSecond;
			Invalidate();
		}
	}

	CWnd::OnTimer(nIDEvent);
}


// �ڴ������ٺ����
void CViewFrame::PostNcDestroy()
{
	CWnd::PostNcDestroy();
	
	if (m_Manager != NULL)
	{
		m_Manager->DestroyWindow();
		delete m_Manager;
	}
	
	if (m_Pictures != NULL)
	{
		m_Pictures->DestroyWindow();
		delete m_Pictures;
	}

	if (m_Videos != NULL)
	{
		m_Videos->DestroyWindow();
		delete m_Videos;
	}

	if (m_Audios != NULL)
	{
		m_Audios->DestroyWindow();
		delete m_Audios;
	}

	if (m_Games != NULL)
	{
		m_Games->DestroyWindow();
		delete m_Games;
	}

	if (m_Documents != NULL)
	{
		m_Documents->DestroyWindow();
		delete m_Documents;
	}

	if (m_ChatRoom != NULL)
	{
		m_ChatRoom->DestroyWindow();
		delete m_ChatRoom;
	}

	
	// �Զ��ͷŴ�����Դ
	delete this;
}


// ���ڹرհ�ť�¼�
void CViewFrame::OnBtnClose()
{
	SendMessage( WM_SYSCOMMAND,SC_CLOSE);
}


// ���ͼƬҳ�溯��
int  CViewFrame::AddButton(LPCTSTR szName, UINT nIcon, UINT nBack)
{
	ButtonInfo buttonInfo;
	buttonInfo.sName = szName;
	buttonInfo.nIcon = nIcon;
	buttonInfo.nBack = nBack;
	m_ButtonList.push_back(buttonInfo);
	return (int)m_ButtonList.size()-1;
}


// ������������л���ťλ��
int  CViewFrame::HitTest(CPoint point, CRect& rtButton)
{
	if(point.y < 519 || point.y > 554)
	{
		return -1;
	}

	if (point.x % BTN_WIDTH < 6 || point.x % BTN_WIDTH > 162)
	{
		return -1;
	}

	if(!GetButtonRect(point.x/BTN_WIDTH, rtButton))
	{
		return -1;
	}

	return point.x / BTN_WIDTH;
}


// ��ȡָ���л���ť����λ��
BOOL CViewFrame::GetButtonRect(int iButton, CRect& rtButton)
{
	if(iButton >= (int)m_ButtonList.size())
	{
		return FALSE;
	}

	rtButton.top = 317;
	rtButton.bottom = 362;
	rtButton.left = 1+ iButton * BTN_WIDTH + 6;
	rtButton.right =  1+ iButton * BTN_WIDTH + 163;

	return TRUE;
}
 

// �����������������ťλ��
int  CViewFrame::XButtonHitTest(CPoint point, CRect& rtButton)
{
	for(size_t i = 0; i< m_XButtonList.size(); i++)
	{
		CXButton& dcControl = m_XButtonList[i];
		if(dcControl.PtInButton(point))
		{
			m_XButtonList[i].GetRect(&rtButton);
			return (int)i;
		}
	}
	return -1;
}


// �����봰����Ϣ֪ͨ
LRESULT CViewFrame::OnMouseHover(WPARAM wparam, LPARAM lparam)
{
	m_bHovering = TRUE;

	return 0;
}


// ����뿪������Ϣ֪ͨ
LRESULT CViewFrame::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bTracking = FALSE;
	m_bHovering = FALSE;
	if(m_iHovering != -1)
	{
		CRect oldRect;
		GetButtonRect(m_iHovering, oldRect);
		m_iHovering = -1;
		InvalidateRect(oldRect);
	}
	if(m_iXButtonHovering != -1)
	{
		m_XButtonList[m_iXButtonHovering].MouseLeave();
		m_iXButtonHovering = -1;
	}
	return 0;
}
