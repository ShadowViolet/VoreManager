// Audios.cpp : 实现文件
//

#include "stdafx.h"
#include "Shadow.h"
#include "Audios.h"
#include "afxdialogex.h"

#include <vfw.h>
#pragma comment(lib,"vfw32.lib")

// CAudios 对话框

IMPLEMENT_DYNAMIC(CAudios, CDialogEx)

CAudios::CAudios(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_AUDIO_DIALOG, pParent)
{
	SetTimer(1, 500, NULL);
	fInited = FALSE;
}


CAudios::~CAudios()
{
	KillTimer(1);
}


void CAudios::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAudios, CDialogEx)
	ON_BN_CLICKED(IDOK, &CAudios::OnOK)
	ON_BN_CLICKED(IDCANCEL, &CAudios::OnCancel)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CAudios 消息处理程序


BOOL CAudios::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	InitStar();
	fInited = TRUE;
	m_nTotalLines = 0;

	LOGFONT logFont = { sizeof(logFont) };

	logFont.lfCharSet = 134;
	logFont.lfWidth = 6;
	logFont.lfHeight = 12;
	logFont.lfWeight = 10;
	lstrcpy(logFont.lfFaceName, _T("宋体"));
	m_font.CreateFontIndirect(&logFont);

	fHideMain = FALSE;

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAudios::OnOK()
{
}


void CAudios::OnPlayMusic(CString Path)
{
	//AfxBeginThread(PlayMusic, (LPVOID)(LPCTSTR)Path);
	if (!OpenLyric(Path, 0, Path))
		OpenLyric(0, Path);
}


void CAudios::OnCancel()
{
}


void CAudios::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CDialogEx::OnPaint()

	CDC memDC;
	CBitmap memBitmap;
	CBitmap* oldBitmap;

	// to avoid flicker, establish a memory dc, draw to it 
	// and then BitBlt it to the client
	memDC.CreateCompatibleDC(&dc);
	memBitmap.CreateCompatibleBitmap(&dc, 283, 374);
	oldBitmap = (CBitmap *)memDC.SelectObject(&memBitmap);

	if (memDC.GetSafeHdc() != NULL)
	{
		// first drop the bitmap on the memory dc
		memDC.BitBlt(0, 0, 283, 374, &m_MainDC, 0, 0, SRCCOPY);
		// finally send the result to the display
		dc.BitBlt(0, 0, 283, 374, &memDC, 0, 0, SRCCOPY);
	}
	memDC.SelectObject(oldBitmap);
}


HBRUSH CAudios::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性
	HBRUSH hBrush = ::CreateSolidBrush(RGB(0, 0, 0));
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}


void CAudios::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 1)
		InvalidateCtrl();

	CDialogEx::OnTimer(nIDEvent);
}


void CAudios::CleanScreen()
{
	CRect rect;
	GetClientRect(&rect);

	CDC*pDC = GetDC();

	pDC->FillSolidRect(&rect, RGB(0, 0, 0));

}


void CAudios::NewStar(int nIndex)
{
	CRect rect;
	GetClientRect(&rect);
	m_star[nIndex].x = rand() % rect.Width();
	m_star[nIndex].y = 0;
	m_star[nIndex].speed = rand() % 50 + 2;
	m_star[nIndex].size = rand() % 4;
}


void CAudios::DrawStar(CDC *pDC, int nIndex)
{
	CPen pen(PS_SOLID, m_star[nIndex].size, RGB(255, 255, 255));
	CPen*pOldPen;
	pOldPen = m_MainDC.SelectObject(&pen);
	m_MainDC.MoveTo(m_star[nIndex].x, m_star[nIndex].y);
	m_MainDC.LineTo(m_star[nIndex].x, m_star[nIndex].y);
	m_MainDC.SelectObject(pOldPen);
}


void CAudios::InitStar()
{
	srand((unsigned)time(0));

	CRect rect;
	GetClientRect(&rect);
	for (int i = 0;i < MAXCOUNT;i++) 
	{
		m_star[i].x = rand() % rect.Width();
		m_star[i].y = rand() % rect.Height();
		m_star[i].speed = rand() % 50 + 2;
		m_star[i].size = rand() % 4;
	}
}


void CAudios::DrawText(CDC *pDC)
{
	if (m_nTotalLines == 0)
		return;

	CDC memDC;
	CBitmap memBitmap;
	memDC.CreateCompatibleDC(pDC);
	memBitmap.CreateCompatibleBitmap(pDC, 283, 374);
	memDC.SelectObject(&memBitmap);
	memDC.SetBkColor(RGB(0, 0, 0));
	memDC.SetTextColor(RGB(0, 255, 0));
	memDC.SetBkMode(TRANSPARENT);

	nCounter -= nStep;

	CFont *pOldFont;

	pOldFont = memDC.SelectObject(&m_font);

	memDC.BitBlt(0, 0, 283, 374, NULL, 0, 0, BLACKNESS);

	CRect rect;
	GetClientRect(&rect);
	rect.top = nCounter;
	rect.bottom = rect.top + 26;
	memDC.SetBkColor(RGB(0, 0, 0));

	for (int i = 0;i < m_nTotalLines;i++) {
		if (rect.top <= 182 && rect.top >= 155)
			memDC.SetTextColor(RGB(255, 255, 0));
		else if (rect.top < 20)
			memDC.SetTextColor(RGB(0, 100, 0));
		else if (rect.top < 156)
			memDC.SetTextColor(RGB(0, 150, 0));
		else
			memDC.SetTextColor(RGB(0, 250, 0));

		memDC.DrawText(m_lyric[i].strLyric, &rect, DT_CENTER);
		rect.top += 26;
		rect.bottom = rect.top + 26;
	}
	m_MainDC.BitBlt(0, 0, 283, 374, &memDC, 0, 0, SRCPAINT);
	memDC.SelectObject(pOldFont);

}


void CAudios::GetLyric(CString strFileName)
{

	ifstream infile(strFileName, ios::in);
	if (!infile)
		return;

	TCHAR szTemp[1000];
	int nLength;
	while (infile.getline((char*)szTemp, 1000)) 
	{
		nLength = lstrlen(szTemp);
		if (nLength > 0 && szTemp[nLength - 1] != _T(']')) 
		{
			AnalyseLine(szTemp);
		}
	}

	if (m_nTotalLines > 0)
		SortLyric();

	infile.close();
}


BOOL CAudios::OpenLyric(int nIndex, CString LrcPath)
{
	/*if (nIndex < 0 || nIndex >= m_strPlayListArray.GetSize())
		return FALSE;*/

	//CString strFileName = m_strPlayListArray.GetAt(nIndex);
	CString strFileName = LrcPath;
	CString strLyricName = strFileName.Left(strFileName.GetLength() - 3);
	strLyricName += _T("lrc");

	m_nTotalLines = 0;
	m_nCurrentLine = 0;
	nStep = 0;
	nCounter = 195;

	/*CSkinButton*pButton = new CSkinButton;
	pButton = (CSkinButton*)m_pParent->GetDlgItem(IDC_BTN_LYRIC);*/

	if (IsExistFile(strLyricName)) {
		GetLyric(strLyricName);
		if (m_nTotalLines > 0) 
		{
			nStep = 15 / m_lyric[0].time;
			/*if (!IsWindowVisible() && m_pParent->IsWindowVisible()) 
			{
				pButton->SetBitmap(IDB_LYRIC_HIDE_NORMAL, IDB_LYRIC_HIDE_OVER);
				pButton->SetToolTipText("关闭歌词");
				ShowWindow(SW_SHOW);
			}*/
			return TRUE;
		}
		else 
		{
			/*if (IsWindowVisible() && m_pParent->IsWindowVisible()) 
			{
				pButton->SetBitmap(IDB_LYRIC_SHOW_NORMAL, IDB_LYRIC_SHOW_OVER);
				pButton->SetToolTipText("打开歌词");
				ShowWindow(SW_HIDE);
			}*/
		}
	}
	else 
	{
		/*if (IsWindowVisible() && m_pParent->IsWindowVisible()) 
		{
			pButton->SetBitmap(IDB_LYRIC_SHOW_NORMAL, IDB_LYRIC_SHOW_OVER);
			pButton->SetToolTipText("打开歌词");
			ShowWindow(SW_HIDE);
		}*/
	}

	return FALSE;
}


BOOL CAudios::IsExistFile(CString strFileName)
{
	HANDLE hFind;
	WIN32_FIND_DATA wfd;
	hFind = FindFirstFile(strFileName, &wfd);
	if (hFind == INVALID_HANDLE_VALUE)
		return FALSE;

	FindClose(hFind);
	return TRUE;
}


void CAudios::AnalyseLine(CString strLine)
{
	CString strLyricLine;
	CString strTimeLine;
	int nLength = strLine.GetLength();
	int nTime;
	int nPos = -1;
	int nOldPos = -1;

	while ((nPos = strLine.Find(_T("]"), nPos + 1)) > 0)
		nOldPos = nPos;

	if (nOldPos == -1)
		return;
	strLyricLine = strLine.Right(nLength - nOldPos - 1);

	nPos = -1;
	int nTimePos;
	while ((nPos = strLine.Find(_T("["), nPos + 1)) >= 0)
	{
		strTimeLine = strLine.Right(nLength - nPos - 1);
		nTimePos = strTimeLine.Find(_T("]"), 0);
		strTimeLine = strTimeLine.Left(nTimePos);
		nTime = GetTime(strTimeLine);
		if (nTime > 0) 
		{
			m_lyric[m_nTotalLines].strLyric = strLyricLine;
			m_lyric[m_nTotalLines].time = nTime;
			m_nTotalLines++;
		}
	}
}


int CAudios::GetTime(CString strTime)
{
	int nPos = 0;
	int nTime;
	int nMinute;
	int nSecond;
	TCHAR szMinute[10];
	TCHAR szSecond[10];
	lstrcpy(szMinute, _T(""));
	lstrcpy(szSecond, _T(""));
	CString strGet;

	nPos = strTime.Find(_T(":"), 0);
	if (nPos < 0)
		return -1;

	if (nPos == 0)
	{
		nMinute = 0;
	}
	else 
	{
		strGet = strTime.Left(nPos);
		lstrcpy(szMinute, strGet);
		nMinute = atoi((CStringA)szMinute);
	}

	strGet = strTime.Right(strTime.GetLength() - nPos - 1);
	if (strGet.GetLength() == 1)
		strGet = strGet.Left(1);
	else if (strGet.GetLength() >= 2)
		strGet = strGet.Left(2);
	else
		return -1;

	lstrcpy(szSecond, strGet);

	nSecond = atoi((CStringA)szSecond);

	nTime = nMinute * 60 + nSecond;

	return nTime;
}


void CAudios::SortLyric()
{
	LYRIC lyricTemp;

	for (int i = m_nTotalLines - 1;i > 0;i--)
		for (int j = 0;j < i;j++) {
			if (m_lyric[j].time > m_lyric[j + 1].time) {
				lyricTemp = m_lyric[j];
				m_lyric[j] = m_lyric[j + 1];
				m_lyric[j + 1] = lyricTemp;
			}
		}

}


void CAudios::CountStep()
{
	/*if (hAudio == NULL)
		return;*/

	//int nTime = MCIWndGetPosition(hAudio) / MCIWndGetSpeed(hAudio);

	int nTime = 0;
	if (nTime < m_lyric[0].time)
		return;

	if (nTime == m_lyric[m_nCurrentLine].time)
		nCounter = 208 - (m_nCurrentLine + 1) * 26;

	int space = m_lyric[m_nCurrentLine + 1].time - m_lyric[m_nCurrentLine].time;
	if (space > 0)
		nStep = 15 / space;

	if (m_nCurrentLine + 1 == m_nTotalLines - 1) 
	{
		//int ntotal = MCIWndGetLength(hAudio) / MCIWndGetSpeed(hAudio);
		nStep = 3;
		return;
	}
	if (nTime >= m_lyric[m_nCurrentLine + 1].time) 
	{
		m_nCurrentLine += 1;
	}
}


void CAudios::SlideClick()
{
	/*if (hAudio == NULL)
		return;*/

	//int nTime = MCIWndGetPosition(hAudio) / MCIWndGetSpeed(hAudio);

	int nTime = 0;
	for (int i = 0;i<m_nTotalLines - 1;i++) 
	{
		if (m_lyric[i].time <= nTime && m_lyric[i + 1].time > nTime) 
		{
			m_nCurrentLine = i;
			nCounter = 208 - (m_nCurrentLine + 1) * 26;
		}
	}
}


void CAudios::InvalidateCtrl()
{
	CPaintDC dc(this); // device context for painting

					   // TODO: Add your message handler code here
	if (dc == NULL)
		return;


	if (m_MainDC.GetSafeHdc() == NULL)
	{
		m_MainDC.CreateCompatibleDC(&dc);
		m_MainBitmap.CreateCompatibleBitmap(&dc, 283, 374);
		m_pOldBitmap = m_MainDC.SelectObject(&m_MainBitmap);
	}

	m_MainDC.SetBkColor(RGB(0, 0, 0));
	m_MainDC.SetTextColor(RGB(255, 255, 255));
	m_MainDC.FillSolidRect(CRect(0, 0, 283, 374), RGB(0, 0, 0));

	CRect rect;
	GetClientRect(&rect);

	for (int i = 0;i < MAXCOUNT;i++) {
		DrawStar(&dc, i);
		m_star[i].y += m_star[i].speed;
		if (m_star[i].y >= rect.Height())
			NewStar(i);
	}
	if (m_nTotalLines > 0) {
		CountStep();
		DrawText(&dc);
	}

	Invalidate();

}


BOOL CAudios::OpenLyric(CString strLyricPath, int nIndex, CString LrcName)
{
	/*if (nIndex < 0 || nIndex >= m_strPlayListArray.GetSize())
		return FALSE;*/

	//CString strFileName = m_strPlayListArray.GetAt(nIndex);
	CString strFileName = LrcName;
	CString strLyricName;

	int nPos = -1;
	int nOldPos = -1;
	while ((nPos = strFileName.Find(_T("\\"), nPos + 1))>0)
		nOldPos = nPos;

	strFileName = strFileName.Right(strFileName.GetLength() - nOldPos - 1);
	strFileName = strFileName.Left(strFileName.GetLength() - 3);
	strFileName += _T("lrc");
	strLyricName.Format(_T("%s\\%s"), strLyricPath, strFileName);


	m_nTotalLines = 0;
	m_nCurrentLine = 0;
	nStep = 0;
	nCounter = 195;

	/*CSkinButton*pButton = new CSkinButton;
	pButton = (CSkinButton*)m_pParent->GetDlgItem(IDC_BTN_LYRIC);*/

	if (IsExistFile(strLyricName)) {
		GetLyric(strLyricName);
		if (m_nTotalLines > 0) 
		{
			nStep = 15 / m_lyric[0].time;
			/*if (!IsWindowVisible() && m_pParent->IsWindowVisible()) 
			{
				pButton->SetBitmap(IDB_LYRIC_HIDE_NORMAL, IDB_LYRIC_HIDE_OVER);
				pButton->SetToolTipText("关闭歌词");
				ShowWindow(SW_SHOW);
			}*/
			return TRUE;
		}
		else 
		{
			/*if (IsWindowVisible() && m_pParent->IsWindowVisible())
			{
				pButton->SetBitmap(IDB_LYRIC_SHOW_NORMAL, IDB_LYRIC_SHOW_OVER);
				pButton->SetToolTipText("打开歌词");
				ShowWindow(SW_HIDE);
			}*/
		}
	}
	else 
	{
		/*if (IsWindowVisible() && m_pParent->IsWindowVisible()) 
		{
			pButton->SetBitmap(IDB_LYRIC_SHOW_NORMAL, IDB_LYRIC_SHOW_OVER);
			pButton->SetToolTipText("打开歌词");
			ShowWindow(SW_HIDE);
		}*/
	}

	return FALSE;
}
