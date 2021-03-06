#pragma once

// 加载图片
BOOL LoadImage(const CString strPathFile, CBitmap &bitmap, CSize &size);
// 读取图片
BOOL LoadImage(UINT nID, CBitmap &bitmap, CSize &size, CString strType);
// 加载图片
BOOL ImageFromIDResource(UINT nID, CString strType, Image * & pImg);
// 取得图片平均颜色
BOOL GetAverageColor(CDC *pDC, CBitmap &bitmap, const CSize &sizeImage, COLORREF &clrImage);
// 取得图片大小
BOOL GetSize(CBitmap &bitmap, CSize &size);
// 取得字体大小
Size GetTextBounds(const Font& font,const StringFormat& strFormat,const CString& strText);
// 取得字体大小
Size GetTextBounds(const Font& font,const CString& strText);
// 取得位置
CPoint GetOriginPoint(int nWidth, int nHeight, int nChildWidth, int nChildHeight, UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER);
// 取得位置
CPoint GetOriginPoint(CRect rc, int nChildWidth, int nChildHeight, UINT uAlignment = DT_CENTER, UINT uVAlignment = DT_VCENTER);
// 转换数字
CString DecimalFormat(int nNumber);


// 绘画函数

// 画垂直过渡
int DrawVerticalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent = 0, int nEndTransparent = 100);
// 画水平过渡
int DrawHorizontalTransition(CDC &dcDes, CDC &dcSrc, const CRect &rcDes, const CRect &rcSrc, int nBeginTransparent = 0, int nEndTransparent = 100);
// 画右下角过渡
void DrawRightBottomTransition(CDC &dc, CDC &dcTemp, CRect rc, const int nOverRegio, const COLORREF clrBackground);
// 画图片边框
void DrawImageFrame(Graphics &graphics, Image *pIamge, const CRect &rcControl, int nX, int nY, int nW, int nH, int nFrameSide = 4);
// 画过渡圆角矩形
void DrawRectangle(CDC &dcDes, const CRect &rcDes, BOOL bUp = TRUE, int nBeginTransparent = 60, int nEndTransparent = 90);
