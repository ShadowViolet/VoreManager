// MyProgress.cpp : implementation file
//

#include "stdafx.h"
#include "MyProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CMyMemDC : public CDC
{
public:

 // constructor sets up the memory DC
	CMyMemDC(CDC* pDC) : CDC()
    {
        ASSERT(pDC != NULL);

        m_pDC = pDC;
        m_pOldBitmap = NULL;
        m_bMemDC = !pDC->IsPrinting();
              
        if (m_bMemDC)    // Create a Memory DC
        {
            pDC->GetClipBox(&m_rect);
    CreateCompatibleDC(pDC);
            m_bitmap.CreateCompatibleBitmap(pDC, m_rect.Width(), m_rect.Height());
            m_pOldBitmap = SelectObject(&m_bitmap);
            SetWindowOrg(m_rect.left, m_rect.top);
        }
        else        // Make a copy of the relevent parts of the current DC for printing
        {
            m_bPrinting = pDC->m_bPrinting;
            m_hDC       = pDC->m_hDC;
            m_hAttribDC = pDC->m_hAttribDC;
        }
    }
 // Destructor copies the contents of the mem DC to the original DC
	~CMyMemDC()
    {
        if (m_bMemDC) 
        {    
            // Copy the offscreen bitmap onto the screen.
            m_pDC->BitBlt(m_rect.left, m_rect.top, m_rect.Width(), m_rect.Height(),
                          this, m_rect.left, m_rect.top, SRCCOPY);

            //Swap back the original bitmap.
            SelectObject(m_pOldBitmap);
         } else {
            // All we need to do is replace the DC with an illegal value,
            // this keeps us from accidently deleting the handles associated with
            // the CDC that was passed to the constructor.
            m_hDC = m_hAttribDC = NULL;
        }
    }

    // Allow usage as a pointer
	CMyMemDC* operator->() { return this; }
 // Allow usage as a pointer
	operator CMyMemDC*() { return this; }

private:
    CBitmap  m_bitmap;      // Offscreen bitmap
    CBitmap* m_pOldBitmap;  // bitmap originally found in CMemDC
    CDC*     m_pDC;         // Saves CDC passed in constructor
    CRect    m_rect;        // Rectangle of drawing area.
    BOOL     m_bMemDC;      // TRUE if CDC really is a Memory DC.
};

/////////////////////////////////////////////////////////////////////////////
// CMyProgress

CMyProgress::CMyProgress()
{
	m_nPos            = 0;
    m_nStepSize        = 1;
    m_nMax            = 100;
    m_nMin            = 0;
    m_bShowText        = TRUE;
    m_strText.Empty();
    m_colFore        = ::GetSysColor(COLOR_HIGHLIGHT);
    m_colBk            = ::GetSysColor(COLOR_WINDOW);
    m_colTextFore    = ::GetSysColor(COLOR_HIGHLIGHT);
    m_colTextBk        = ::GetSysColor(COLOR_WINDOW);
    m_nBarWidth = -1;
}

CMyProgress::~CMyProgress()
{
}

BEGIN_MESSAGE_MAP(CMyProgress, CProgressCtrl)
	//{{AFX_MSG_MAP(CMyProgress)
	ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
    ON_MESSAGE(WM_GETTEXT, OnGetText)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyProgress message handlers

LRESULT CMyProgress::OnSetText(WPARAM szText, LPARAM)
{
    LRESULT result = Default();

    if ( (!szText && m_strText.GetLength()) ||
         (szText && (m_strText != szText))   )
    {
		m_strText = (LPCTSTR)szText;
        Invalidate();
    }

    return result;
}

LRESULT CMyProgress::OnGetText(WPARAM szText, LPARAM cchTextMax)
{
	if (!_tcsncpy_s((LPTSTR)szText, NULL, m_strText, cchTextMax))
        return 0;
    else 
        return min(cchTextMax, m_strText.GetLength());
}

BOOL CMyProgress::OnEraseBkgnd(CDC* /*pDC*/) 
{    
     return TRUE;
}

void CMyProgress::OnSize(UINT nType, int cx, int cy) 
{
    CProgressCtrl::OnSize(nType, cx, cy);
    
    m_nBarWidth    = -1;   // Force update if SetPos called
}

void CMyProgress::OnPaint() 
{
    if (m_nMin >= m_nMax) 
        return;

    CRect LeftRect, RightRect, ClientRect;
    GetClientRect(ClientRect);

    double Fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));

    CPaintDC PaintDC(this); // device context for painting
    CMyMemDC dc(&PaintDC);

    LeftRect = RightRect = ClientRect;

    LeftRect.right = LeftRect.left + (int)((LeftRect.right - LeftRect.left)*Fraction);
    dc.FillSolidRect(LeftRect, m_colFore);

    RightRect.left = LeftRect.right;
    dc.FillSolidRect(RightRect, m_colBk);

    if (m_bShowText)
    {
        CString str;
        if (m_strText.GetLength())
            str = m_strText;
        else
            str.Format(_T("%d%%"), (int)(Fraction*100.0));

        dc.SetBkMode(TRANSPARENT);

        CRgn rgn;
        rgn.CreateRectRgn(LeftRect.left, LeftRect.top, LeftRect.right, LeftRect.bottom);
        dc.SelectClipRgn(&rgn);
        dc.SetTextColor(m_colTextBk);

        dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        rgn.DeleteObject();
        rgn.CreateRectRgn(RightRect.left, RightRect.top, RightRect.right, RightRect.bottom);
        dc.SelectClipRgn(&rgn);
        dc.SetTextColor(m_colTextFore);

        dc.DrawText(str, ClientRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
}

void CMyProgress::SetForeColour(COLORREF col)
{
    m_colFore = col;
}

void CMyProgress::SetBkColour(COLORREF col)
{
    m_colBk = col;
}

void CMyProgress::SetTextForeColour(COLORREF col)
{
    m_colTextFore = col;
}

void CMyProgress::SetTextBkColour(COLORREF col)
{
    m_colTextBk = col;
}

COLORREF CMyProgress::GetForeColour()
{
    return m_colFore;
}

COLORREF CMyProgress::GetBkColour()
{
    return m_colBk;
}

COLORREF CMyProgress::GetTextForeColour()
{
    return m_colTextFore;
}

COLORREF CMyProgress::GetTextBkColour()
{
    return m_colTextBk;
}

/////////////////////////////////////////////////////////////////////////////
// CMyProgress message handlers

void CMyProgress::SetShowText(BOOL bShow)
{ 
    if (::IsWindow(m_hWnd) && m_bShowText != bShow)
        Invalidate();

    m_bShowText = bShow;
}


void CMyProgress::SetRange(int nLower, int nUpper)
{
    m_nMax = nUpper;
    m_nMin = nLower;
}

int CMyProgress::SetPos(int nPos) 
{    
    if (!::IsWindow(m_hWnd))
        return -1;

    int nOldPos = m_nPos;
    m_nPos = nPos;

    CRect rect;
    GetClientRect(rect);

    double Fraction = (double)(m_nPos - m_nMin) / ((double)(m_nMax - m_nMin));
    int nBarWidth = (int) (Fraction * rect.Width());

    if (nBarWidth != m_nBarWidth)
    {
        m_nBarWidth = nBarWidth;
        RedrawWindow();
    }

    return nOldPos;
}

int CMyProgress::StepIt() 
{
   return SetPos(m_nPos + m_nStepSize);
}

int CMyProgress::OffsetPos(int nPos)
{
    return SetPos(m_nPos + nPos);
}

int CMyProgress::SetStep(int nStep)
{
    int nOldStep = nStep;
    m_nStepSize = nStep;
    return nOldStep;
}
