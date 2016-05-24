#pragma once


#include <fstream>
using   namespace   std;

#define MAXCOUNT  100

typedef  struct _star
{
	int x;
	int y;
	int speed;
	int size;
} STAR, *PSTAR;

typedef struct _lyric
{
	int time;
	CString strLyric;
}LYRIC, *PLYRIC;



// CAudios 对话框

class CAudios : public CDialogEx
{
	DECLARE_DYNAMIC(CAudios)

public:
	CAudios(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAudios();

	void OnPlayMusic(CString Path);
	static UINT PlayMusic(LPVOID pParam);

	BOOL fHideMain;
	BOOL OpenLyric(CString strLyricPath, int nIndex, CString LrcName);
	void InvalidateCtrl();
	CBitmap* m_pOldBitmap;
	CBitmap m_MainBitmap;
	CDC m_MainDC;
	CFont m_font;
	void SlideClick();
	int m_nCurrentLine;
	void CountStep();
	int nStep;
	void SortLyric();
	int nCounter;
	int GetTime(CString strTime);
	void AnalyseLine(CString strLine);
	int m_nTotalLines;
	LYRIC m_lyric[1000];
	BOOL IsExistFile(CString strFileName);
	BOOL OpenLyric(int nIndex, CString LrcPath);
	BOOL fInited;
	void GetLyric(CString strFileName);
	void DrawText(CDC*pDC);
	void InitStar();
	void DrawStar(CDC*pDC, int nIndex);
	void NewStar(int nIndex);
	STAR m_star[MAXCOUNT];
	void CleanScreen();
	int m_nID;

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AUDIO_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
