#pragma once

#include "afxwin.h"

class CusStatic :public CStatic {
	DECLARE_DYNAMIC(CusStatic)
public:
	CusStatic();
	virtual ~CusStatic();

	void SetBitmap(LPCTSTR id);

	void SetBitmap(UINT id);

	void SetMaxWidth(UINT w);
	UINT GetMaxWidth();

	void SetWidth(UINT w);

	void SetText(CString t, int x = 0, int y = 0, int w = 0, int h = 0);

	void SetTextColor(COLORREF c);
	void SetTextSize(int size);

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { return TRUE; }
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
private :
	void paintOnMemDC(CRect rect, CDC* mDC);
	void paintOnMemDCText(CRect rect, CDC* mDC);

	CBitmap* image;

	CString   text;
	CRect*    textRect;
	CFont*    textFont;
	COLORREF  textCoulor;

	UINT maxWidth;
};