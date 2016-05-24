// Pictures.cpp : 实现文件
//

#include "stdafx.h"
#include "Shadow.h"
#include "Pictures.h"
#include "afxdialogex.h"


// CPictures 对话框

IMPLEMENT_DYNAMIC(CPictures, CDialogEx)

CPictures::CPictures(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PICTURE_DIALOG, pParent)
{
}

CPictures::~CPictures()
{
	OnRelease();
}

void CPictures::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPictures, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CPictures::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CPictures::OnCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPictures 消息处理程序


BOOL CPictures::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CPictures::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CPictures::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}


void CPictures::ShowPictures(CString Path)
{
	KillTimer(1);

	CDC* pDC = this->GetDC();
	CRect rcclient;
	this->GetClientRect(&rcclient);
	Graphics imgraphics(pDC->m_hDC);

	CString Ext = PathFindExtension(Path);
	if (Ext.MakeLower() != _T(".gif"))
	{
	NOT_GIF:
		Image image(Path);
		imgraphics.DrawImage(&image, rcclient.left, rcclient.top, rcclient.Width(), rcclient.Height());
	}
	else
	{
		OnRelease();

		image = new Image(Path);
		count = 0;
		count = image->GetFrameDimensionsCount();
		pDimensionIDs = (GUID*)new GUID[count];
		image->GetFrameDimensionsList(pDimensionIDs, count);
		StringFromGUID2(pDimensionIDs[0], strGuid, 39);
		frameCount = image->GetFrameCount(&pDimensionIDs[0]);
		delete[]pDimensionIDs;
		size = image->GetPropertyItemSize(PropertyTagFrameDelay);

		if (!frameCount)
			goto NOT_GIF;

		pItem = (PropertyItem*)new PropertyItem[size];
		image->GetPropertyItem(PropertyTagFrameDelay, size, pItem);
		fcount = 0;
		Guid = FrameDimensionTime;

		ReleaseDC(pDC);
		SetTimer(1, 0, NULL);
	}
}


void CPictures::OnRelease()
{
	KillTimer(1);
	delete image;
	delete[] pItem;
}


void CPictures::OnOK()
{
}


void CPictures::OnTimer(UINT_PTR nIDEvent)
{
	CRect rcclient;
	this->GetClientRect(&rcclient);
	CDC* hdc = this->GetDC();
	Graphics  graphics(hdc->m_hDC);
	graphics.DrawImage(image, rcclient.left, rcclient.top, rcclient.Width(), rcclient.Height());

	image->SelectActiveFrame(&Guid, fcount++);
	if (fcount == frameCount)
		fcount = 0;

	lPause = ((long*)pItem->value)[fcount] * 10;
	ReleaseDC(hdc);

	KillTimer(1);
	SetTimer(1, lPause, NULL);

	CDialogEx::OnTimer(nIDEvent);
}


void CPictures::OnCancel()
{
}
