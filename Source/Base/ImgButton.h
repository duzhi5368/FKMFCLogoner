#if !defined(AFX_BUTTONEX_H__3D653D07_0D29_4878_B9D2_AA8C7C71A89C__INCLUDED_)
#define AFX_BUTTONEX_H__3D653D07_0D29_4878_B9D2_AA8C7C71A89C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonEx.h : header file
/*
文件名：ButtonEx.h
摘要：自按钮类

	  注：当开发环境为VC6及以下版本时，需加上:
		    #include<windgi.h>
			#pragma comment(lib,"msimg32.lib")
		  开发环境为VC++.net 上一版本时不用加如上包含
版本：1.00
完成日期：2007年8 月17日

修改记录：
*/
#define BMP_NORMAL		0
#define BMP_SELECTED	1
#define BMP_FOCUS		2	
#define BMP_DISABLED	3
// 字体颜色
#define UI_COLOR_TEXT				RGB(72, 93, 122)


/////////////////////////////////////////////////////////////////////////////
// ImgButton window
enum
{
	DOWN,
	UP,
	HOVER,
};

class ImgButton : public CButton {
// Construction
public:
	ImgButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ImgButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetTextColor(COLORREF clr);
	bool SetBitmap(CString FileName,int State);
	void SetToolTip(CString strTip);
	CRect m_BtRect;
	bool SetBitmaps(LPCTSTR pszNormal = 0, LPCTSTR pszSelect = 0, LPCTSTR pszDisabled = 0, LPCTSTR pszFocus = 0, LPCTSTR pszDown = NULL);
	bool SetBitmapsEx(LPCTSTR pszNormal = 0, LPCTSTR pszSelect = 0, LPCTSTR pszDisabled = 0, LPCTSTR pszFocus = 0, LPCTSTR pszDown = NULL);
	bool LoadBitmaps(UINT uNormalResID,UINT uHoverResID,UINT uDownResID, UINT uFocusResID,UINT uDisableResID=0);
	bool LoadPNG(UINT uNormalResID, UINT uHoverResID, UINT uDownResID, UINT uFocusResID, UINT uDisableResID = 0);
	void DrawBitmap(CDC *pDC, CBitmap *bitmap, CRect *rect);
	void SetBitmapStatus(BOOL bOver);
	BOOL GetBitmapStatus();
	CBitmap * GetNormalBitmap(){ return & NormalBitmap;}
    CBitmap * GetBitmap(UINT idBmp);
	CSize GetButtonSize(){return m_size;}

	void MoveToY(int y);
	void MoveToX(int x);
	void MoveTo(int x, int y);

	void SetDrawStyle(bool bDrawNoZoom);

	void SetDefaultFont(); // 设置Button默认字体
	void SetBigFont();     // 设置大字体

	void SetTextOffsetX(int x);
	void SetTextOffsetY(int y);

	virtual ~ImgButton();

	// Generated message map functions
protected:
	COLORREF m_textCoulor;
	//{{AFX_MSG(ImgButton)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CBitmap NormalBitmap;
    CBitmap SelectBitmap;
    CBitmap FocusBitmap;
    CBitmap DisableBitmap;
	CBitmap DownBitmap;

	CBitmap NormalBitmap1;
	CBitmap SelectBitmap1;
	CBitmap FocusBitmap1;
	CBitmap DisableBitmap1;
	CBitmap DownBitmap1;

	CFont *	m_pFont;	// 字体
	CFont * m_pBigFont; //大字体
	CSize   m_size;

	bool	m_bDrawNoZoom;	// 绘制时是否禁用缩放
	BOOL	m_bOver;
	BOOL	m_bFocus;
	BOOL	m_bSelected;
	BOOL	m_bTracking;
	BOOL	m_bIsEx;
	BOOL	m_bHasEx;

	int     offsetX;
	int     offsetY;
private:
	CToolTipCtrl m_ToolTip;

	
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONEX_H__3D653D07_0D29_4878_B9D2_AA8C7C71A89C__INCLUDED_)
