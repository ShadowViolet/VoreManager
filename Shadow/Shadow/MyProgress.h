#if !defined(AFX_MYPROGRESS_H__A269A27F_8DDF_450A_91DD_40B9B8EE0970__INCLUDED_)
#define AFX_MYPROGRESS_H__A269A27F_8DDF_450A_91DD_40B9B8EE0970__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyProgress.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyProgress window

class CMyProgress : public CProgressCtrl
{
// Construction
public:
	CMyProgress();

// Attributes
public:
	int         SetPos(int nPos);
    int         StepIt();
    void        SetRange(int nLower, int nUpper);
    int         OffsetPos(int nPos);
    int         SetStep(int nStep);
    void        SetForeColour(COLORREF col);
    void        SetBkColour(COLORREF col);
    void        SetTextForeColour(COLORREF col);
    void        SetTextBkColour(COLORREF col);
    COLORREF    GetForeColour();
    COLORREF    GetBkColour();
    COLORREF    GetTextForeColour();
    COLORREF    GetTextBkColour();

    void        SetShowText(BOOL bShow);

public:
	virtual ~CMyProgress();

protected:

	int         m_nPos, 
                m_nStepSize, 
                m_nMax, 
                m_nMin;
    CString     m_strText;
    BOOL        m_bShowText;
    int         m_nBarWidth;
    COLORREF    m_colFore,
                m_colBk,
                m_colTextFore,
                m_colTextBk;

    //{{AFX_MSG(CTextProgressCtrl)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
	afx_msg LRESULT OnSetText(WPARAM szText, LPARAM);
	afx_msg LRESULT OnGetText(WPARAM szText, LPARAM cchTextMax);

	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYPROGRESS_H__A269A27F_8DDF_450A_91DD_40B9B8EE0970__INCLUDED_)
