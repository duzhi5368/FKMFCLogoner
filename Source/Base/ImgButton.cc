// ButtonEx.cpp : implementation file
//

#include "stdafx.h"
#include "ImgButton.h"
#include <windowsx.h>

#define IDC_HAND            MAKEINTRESOURCE(32649)
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ImgButton

ImgButton::ImgButton() {
	m_bOver = m_bSelected = m_bTracking = m_bFocus = FALSE;
	m_textCoulor = UI_COLOR_TEXT;
	//m_textCoulor = RGB(0x08, 0x34, 0x56);
	m_bIsEx = FALSE;
	m_bHasEx = FALSE;
	offsetX = 0;
	offsetY = 0;

	m_bDrawNoZoom = false;

	m_pFont = NULL;
	m_pBigFont = NULL;
}

ImgButton::~ImgButton(){
	// [ShenChenggui*2005-07-29*] Must destroy resource
	NormalBitmap.DeleteObject();
	SelectBitmap.DeleteObject();
	FocusBitmap.DeleteObject();
	DisableBitmap.DeleteObject();

	if(m_bHasEx){
		NormalBitmap1.DeleteObject();
		SelectBitmap1.DeleteObject();
		FocusBitmap1.DeleteObject();
		DisableBitmap1.DeleteObject();
	}

	if (m_pFont){
		delete m_pFont;
	}
	if(m_pBigFont){
		delete m_pBigFont;
		m_pBigFont = NULL;
	}
}


BEGIN_MESSAGE_MAP(ImgButton, CButton)
	//{{AFX_MSG_MAP(ImgButton)
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ImgButton message handlers

// 设置Button默认字体
void ImgButton::SetDefaultFont(){
	if (m_pFont){
		delete m_pFont;
		m_pFont = NULL;
	}

	m_pFont = new CFont();
	m_pFont->Attach(GetStockObject(DEFAULT_GUI_FONT));
	SetFont(m_pFont);
}

// 设置大字体
void ImgButton::SetBigFont(){
	LOGFONT logFont;
	GetFont()->GetLogFont(& logFont);
	m_pBigFont = new CFont;
	m_pBigFont->CreateFont(
		26,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_BOLD, //FW_BOLD,//FW_NORMAL,                   // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		DEFAULT_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		PROOF_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("微软雅黑"));
	
	SetFont(m_pBigFont);
}

void ImgButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
	// TODO: Add your code to draw the specified item
	CDC* pDC;
	pDC= new CDC();
	pDC->Attach(lpDrawItemStruct->hDC);
	CRect rect;
	GetClientRect(&rect);
	
    UINT state = lpDrawItemStruct->itemState ;
	UINT action=lpDrawItemStruct->itemAction;
    bool IsDisable = false;	

	if (state & ODS_FOCUS){
		m_bFocus = TRUE;
		m_bSelected = TRUE;
	}else{
		m_bFocus = FALSE;
		m_bSelected = FALSE;
	}	
	
	if (state & ODS_SELECTED || state & ODS_DEFAULT){
		m_bFocus = TRUE;
	}
	if(!m_bIsEx){
		CBitmap* pBitmap = NULL;
		if (m_bFocus && !m_bOver && FocusBitmap.m_hObject != NULL){
			pBitmap = &FocusBitmap;
		} else {
			if (m_bOver)
				pBitmap = &SelectBitmap;
			else
				pBitmap = &NormalBitmap;
		}
		//	if (m_bOver)
		//		DrawBitmap(pDC, &SelectBitmap, &rect);
		//	else
		//		DrawBitmap(pDC, &NormalBitmap, &rect);
		if (state & ODS_SELECTED){
			rect.OffsetRect( 1, 1 );
			if (DownBitmap.m_hObject != NULL){
				pBitmap = &DownBitmap;
			} else {
				pBitmap = &DisableBitmap;
			}
		}

		if( state & ODS_DISABLED ){
			IsDisable = TRUE;
			pBitmap = &DisableBitmap;
		}

		if (pBitmap && pBitmap->m_hObject == NULL){
			pBitmap = &NormalBitmap; // 如果都没有就绘制普通的
		}

		DrawBitmap(pDC, pBitmap, &rect);
	} else {
		CBitmap* pBitmap = NULL;
		if (m_bFocus && !m_bOver && FocusBitmap.m_hObject != NULL) {
			pBitmap = &FocusBitmap1;
		} else {
			if (m_bOver)
				pBitmap = &SelectBitmap1;
			else
				pBitmap = &NormalBitmap1;
		}
		//	if (m_bOver)
		//		DrawBitmap(pDC, &SelectBitmap, &rect);
		//	else
		//		DrawBitmap(pDC, &NormalBitmap, &rect);
		if (state & ODS_SELECTED) {
			rect.OffsetRect( 1, 1 );
			if (DownBitmap.m_hObject != NULL) {
				pBitmap = &DownBitmap1;
			} else {
				pBitmap = &DisableBitmap1;
			}
		}

		if( state & ODS_DISABLED ){
			IsDisable = TRUE;
			pBitmap = &DisableBitmap1;
		}

		if (pBitmap && pBitmap->m_hObject == NULL){
			pBitmap = &NormalBitmap1; // 如果都没有就绘制普通的
		}

		DrawBitmap(pDC, pBitmap, &rect);
	}

    int imode = pDC->SetBkMode(TRANSPARENT);
    COLORREF oldColor;

    if(IsDisable)
      oldColor = pDC->SetTextColor( GetSysColor(COLOR_GRAYTEXT) );
    else
      oldColor = pDC->SetTextColor(m_textCoulor);

    CString txt;
    GetWindowText(txt);
	rect.left += offsetX;
	rect.top += offsetY;
    pDC->DrawText(txt, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    pDC->SetTextColor( oldColor );
    pDC->SetBkMode(imode );

	// [Shenchenggui*2005-07-29*] Must detach 
	pDC->Detach ();
	delete pDC;
	
}

void ImgButton::SetTextOffsetX(int x) {
	offsetX = x;
}

void ImgButton::SetTextOffsetY(int y) {
	offsetY = y;
}

void ImgButton::MoveToY(int y){
	if (::IsWindow(m_hWnd)){
		CRect	rc;
		GetWindowRect(rc);
		GetParent()->ScreenToClient(rc);
		rc.MoveToY(y);
		MoveWindow(rc);
	}
}

void ImgButton::MoveToX(int x){
	if (::IsWindow(m_hWnd)){
		CRect	rc;
		GetWindowRect(rc);
		GetParent()->ScreenToClient(rc);
		rc.MoveToX(x);
		MoveWindow(rc);
	}
}

void ImgButton::MoveTo(int x, int y){
	if (::IsWindow(m_hWnd)){
		CRect	rc;
		GetWindowRect(rc);
		GetParent()->ScreenToClient(rc);
		rc.MoveToXY(x, y);
		MoveWindow(rc);
	}
}

void ImgButton::DrawBitmap(CDC *pDC, CBitmap *bitmap, CRect *rect){
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	if (bitmap && bitmap->GetSafeHandle()){
		CBitmap* pOldBitmap = dc.SelectObject(bitmap);

		int bmw, bmh ;
		BITMAP bmap;
		bitmap->GetBitmap(&bmap);
		bmw = bmap.bmWidth;
		bmh = bmap.bmHeight;
		int xo=0, yo=0;

		if (!m_bDrawNoZoom){
			// 可以缩放的绘制
			TransparentBlt(pDC->GetSafeHdc(), xo, yo, rect->Width(),rect->Height(), dc.GetSafeHdc(),
				0, 0,bmw,bmh, RGB(255,0,255));
		}else{
			// 禁用缩放的绘制
			// 左边
			int iHeight = min(bmh, rect->Height());
			TransparentBlt(pDC->GetSafeHdc(), xo, yo, 10, iHeight, dc.GetSafeHdc(),
				0, 0, 10, iHeight, RGB(255,0,255));
			xo = 10;
			// 中间
			while (xo < rect->Width() - 10){
				int right = xo + bmw - (2 * 10);
				if (right > rect->Width() - 10){
					right = rect->Width() - 10;
				}
				TransparentBlt(pDC->GetSafeHdc(), xo, yo, right - xo, iHeight, dc.GetSafeHdc(),
					10, 0, right - xo, iHeight, RGB(255,0,255));
				xo = right;
			}
			// 右边
			TransparentBlt(pDC->GetSafeHdc(), rect->Width() - 10, yo, 10, iHeight, dc.GetSafeHdc(),
				bmw - 10, 0, 10, iHeight, RGB(255,0,255));
		}

		dc.SelectObject(pOldBitmap);
	}else{
		COLORREF clrBk = dc.SetBkColor(RGB(143, 190, 241));
		int nMode = dc.SetBkMode(TRANSPARENT);
		dc.ExtTextOut(0, 0, ETO_OPAQUE, rect, NULL, 0, NULL);  
		dc.SetBkColor(clrBk);
	}
}

BOOL ImgButton::OnEraseBkgnd(CDC* pDC){
	return	TRUE;
}

void ImgButton::SetDrawStyle(bool bDrawNoZoom){
	m_bDrawNoZoom = bDrawNoZoom;
}

bool ImgButton::SetBitmaps(LPCTSTR pszNormal /* = 0 */, LPCTSTR pszSelect /* = 0 */, LPCTSTR pszDisabled /* = 0 */, LPCTSTR pszFocus, LPCTSTR pszDown){	

	HBITMAP hBmp = (HBITMAP )LoadImage(NULL, pszNormal ? pszNormal : L"Button_up.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		NormalBitmap.DeleteObject();		
		NormalBitmap.Attach(hBmp);
		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszSelect ? pszSelect : L"Button_ Select.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp){
		SelectBitmap.DeleteObject();
		SelectBitmap.Attach(hBmp);

		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszDisabled ? pszDisabled : L"Button_Disable.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp){
		DisableBitmap.DeleteObject();
		DisableBitmap.Attach(hBmp);
		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszFocus ? pszFocus : L"Button_Focus.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		FocusBitmap.DeleteObject();
		FocusBitmap.Attach(hBmp);
		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszDown,
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		DownBitmap.DeleteObject();
		DownBitmap.Attach(hBmp);
		hBmp = 0;
	}

	// 调整 Button 尺寸与图相同
	if(NormalBitmap.GetSafeHandle() && IsWindow(m_hWnd)){
		BITMAP bmpInfo;
		NormalBitmap.GetBitmap(&bmpInfo);
		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(rc);
		if (m_bDrawNoZoom){
			// 不缩放的情况下不用改变窗口宽度
			MoveWindow(rc.left, rc.top, rc.Width(), bmpInfo.bmHeight);
		} else {
			MoveWindow(rc.left, rc.top, bmpInfo.bmWidth, bmpInfo.bmHeight);
		}

		m_size.SetSize(bmpInfo.bmWidth, bmpInfo.bmHeight);
	}
	
//	SetButtonStyle(BS_OWNERDRAW);
	return true;
}

bool ImgButton::SetBitmapsEx(LPCTSTR pszNormal /* = 0 */, LPCTSTR pszSelect /* = 0 */, LPCTSTR pszDisabled /* = 0 */, LPCTSTR pszFocus, LPCTSTR pszDown){	
	m_bHasEx = TRUE;
	CString strpath;

	HBITMAP hBmp = (HBITMAP )LoadImage(NULL, pszNormal ? pszNormal : L"Button_up.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		NormalBitmap1.DeleteObject();		
		NormalBitmap1.Attach(hBmp);
		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszSelect ? pszSelect : L"Button_ Select.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		SelectBitmap1.DeleteObject();
		SelectBitmap1.Attach(hBmp);

		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszDisabled ? pszDisabled : L"Button_Disable.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		DisableBitmap1.DeleteObject();
		DisableBitmap1.Attach(hBmp);
		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszFocus ? pszFocus : L"Button_Focus.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp){
		FocusBitmap1.DeleteObject();
		FocusBitmap1.Attach(hBmp);
		hBmp = 0;
	}

	hBmp = (HBITMAP )LoadImage(NULL, pszDown,
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		DownBitmap1.DeleteObject();
		DownBitmap1.Attach(hBmp);
		hBmp = 0;
	}


	//调整button尺寸与图相同
/*	if(NormalBitmap1.GetSafeHandle() && IsWindow(m_hWnd))
	{
		BITMAP bmpInfo;
		NormalBitmap1.GetBitmap(&bmpInfo);
		CRect rc;
		GetWindowRect(&rc);
		GetParent()->ScreenToClient(rc);
		MoveWindow(rc.left, rc.top, bmpInfo.bmWidth, bmpInfo.bmHeight);
	}


	SetButtonStyle(BS_OWNERDRAW);
	//DeleteObject(hBmp);*/
	return true;
}

bool ImgButton::LoadBitmaps(UINT uNormalResID,UINT uHoverResID,UINT uDownResID, UINT uFocusResID,UINT uDisableResID){
	NormalBitmap.DeleteObject();
	SelectBitmap.DeleteObject();
	DisableBitmap.DeleteObject();
	FocusBitmap.DeleteObject();
	DownBitmap.DeleteObject();

	NormalBitmap.LoadBitmap(uNormalResID);
	SelectBitmap.LoadBitmap(uHoverResID);
	if(uDisableResID != 0)
		DisableBitmap.LoadBitmap(uDisableResID);
	else
		DisableBitmap.LoadBitmap(uDownResID);
	FocusBitmap.LoadBitmap(uFocusResID);
	DownBitmap.LoadBitmap(uDownResID);

	SetButtonStyle(BS_OWNERDRAW);
	return true;
}

bool ImgButton::LoadPNG(UINT uNormalResID, UINT uHoverResID, UINT uDownResID, UINT uFocusResID, UINT uDisableResID) {
	NormalBitmap.DeleteObject();
	SelectBitmap.DeleteObject();
	DisableBitmap.DeleteObject();
	FocusBitmap.DeleteObject();
	DownBitmap.DeleteObject();

	CPngImage image;
	//HINSTANCE hs = AfxGetInstanceHandle();
	image.Load(uNormalResID);
	NormalBitmap.Attach(image.Detach());

	image.Load(uHoverResID);
	SelectBitmap.Attach(image.Detach());

	if (uDisableResID != 0) {
		image.Load(uDisableResID);
		DisableBitmap.Attach(image.Detach());
	} else {
		image.Load(uDownResID);
		DisableBitmap.Attach(image.Detach());
	}
	image.Load(uFocusResID);
	FocusBitmap.Attach(image.Detach());

	image.Load(uDownResID);
	DownBitmap.Attach(image.Detach());

	SetButtonStyle(BS_OWNERDRAW);
	return true;
}

void ImgButton::OnMouseMove(UINT nFlags, CPoint point) {
	if (!m_bTracking){
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme) == TRUE;
	}
	
	CButton::OnMouseMove(nFlags, point);
}

BOOL ImgButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	// TODO: Add your message handler code here and/or call default
	HCURSOR sor=AfxGetApp()->LoadStandardCursor(IDC_HAND);
	::SetCursor(sor);
	return true;
	
}

int ImgButton::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (GetWindowTextLength() > 0){
		SetDefaultFont();
	}
	return 0;
}

void ImgButton::SetToolTip(CString strTip){
	if(!IsWindow(m_ToolTip.m_hWnd))
		m_ToolTip.Create( this,TTS_ALWAYSTIP ) ;
	CRect rect;
	GetWindowRect(&rect);
	m_ToolTip.AddTool(this,strTip);
	m_ToolTip.Activate(true);
}

LRESULT ImgButton::OnMouseLeave(WPARAM wParam, LPARAM lParam){
	m_bOver = FALSE;
	m_bTracking = FALSE;
	InvalidateRect(NULL, FALSE);
	return 0;
}

LRESULT ImgButton::OnMouseHover(WPARAM wParam, LPARAM lParam){
	m_bOver = TRUE;
	InvalidateRect(NULL, FALSE);
	return 0;
}

void ImgButton::PreSubclassWindow() {
	// TODO: Add your specialized code here and/or call the base class
	CButton::PreSubclassWindow();
	ModifyStyle(0, BS_OWNERDRAW);
}

bool ImgButton::SetBitmap(CString FileName, int State){
	HBITMAP hBmp = (HBITMAP )LoadImage(NULL, FileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {	
		switch (State){
		case 1:
			NormalBitmap.DeleteObject();
			NormalBitmap.Attach(hBmp);
			break;
		case 2:
			SelectBitmap.DeleteObject();
			SelectBitmap.Attach(hBmp);
			break;
		case 3:
			FocusBitmap.DeleteObject();
			FocusBitmap.Attach(hBmp);
			break;
		case 4:
			DisableBitmap.DeleteObject();
			DisableBitmap.Attach(hBmp);
			break;
		default:
			DeleteObject(hBmp);			
		}

		hBmp = 0;
	}
	
	return true;
}

void ImgButton::SetTextColor(COLORREF clr){
	m_textCoulor = clr; 
}

BOOL ImgButton::PreTranslateMessage(MSG* pMsg) {
	// TODO: Add your specialized code here and/or call the base class
	if(m_ToolTip.m_hWnd!=NULL)
		m_ToolTip.RelayEvent(pMsg);
	return CButton::PreTranslateMessage(pMsg);
}

void ImgButton::SetBitmapStatus(BOOL bOver){
	m_bIsEx = bOver;
	InvalidateRect(NULL, FALSE);
}

BOOL ImgButton::GetBitmapStatus(){
	return	m_bIsEx;
}

CBitmap* ImgButton::GetBitmap(UINT idBmp){
	switch(idBmp){
		case BMP_SELECTED: return &SelectBitmap;
		case BMP_FOCUS:    return &FocusBitmap;
		case BMP_DISABLED: return &DisableBitmap;
		case BMP_NORMAL:
		default:           return &NormalBitmap;
	}
}
