#include "StdAfx.h"
#include "resource.h"
#include "Base/BackDialogBase.h"
#include "config.h"
#include "Setting.h"

IMPLEMENT_DYNAMIC(CBackDialogBase, CDialog)

BEGIN_MESSAGE_MAP(CBackDialogBase, CDialog)
    ON_WM_ERASEBKGND()
    ON_WM_MOVE()
    ON_WM_SIZE()
    ON_WM_CREATE()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CBackDialogBase::CBackDialogBase(UINT nIDTemplate, UINT nImgID, CWnd* pParent /*=NULL*/)
    : CDialog(nIDTemplate, pParent)
    , pForeDialog(NULL){
    HMODULE hInstance = (HMODULE)::AfxGetResourceHandle();
    HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(nImgID), _T("PNG")); 
	//HRSRC hRsrc = ::FindResource(hInstance, MAKEINTRESOURCE(nImgID), RT_BITMAP);
    ATLASSERT(hRsrc != NULL);

    DWORD dwSize = ::SizeofResource(hInstance, hRsrc);
    LPBYTE lpRsrc = (LPBYTE)::LoadResource(hInstance, hRsrc);
    ATLASSERT(lpRsrc != NULL);

    HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
    LPBYTE pMem = (LPBYTE)::GlobalLock(hMem);
    memcpy(pMem, lpRsrc, dwSize);
    IStream* pStream = NULL;
    ::CreateStreamOnHGlobal( hMem, FALSE, &pStream);

    pImage = Gdiplus::Image::FromStream(pStream);

    pStream->Release();
    ::FreeResource(lpRsrc);
}

CBackDialogBase::~CBackDialogBase(){
    if (pForeDialog != NULL){
        ::delete pForeDialog;
        pForeDialog = NULL;
    }

    if (pImage != NULL){
        delete pImage;
        pImage = NULL;
    }
}

BOOL CBackDialogBase::OnInitDialog(){
    CDialog::OnInitDialog();
	
	//SetIcon(m_hIcon, TRUE);			// 设置大图标
	//SetIcon(m_hIcon, FALSE);		// 设置小图标

	//CImageManager::Instance().Initialize();
	CString sVersion = CSetting::Instance().GetVersion();
	CString sWinText;
	sWinText.Format(_T("%s %s"), _MAINWIN_STRING, sVersion);
	SetWindowText(sWinText);

	ModifyStyle(WS_CAPTION | WS_CHILD, WS_MINIMIZEBOX);
    ModifyStyleEx(0, WS_EX_LAYERED | WS_OVERLAPPED);

    ::SetWindowPos(pForeDialog->GetSafeHwnd(), NULL, 0, 0, pImage->GetWidth(), pImage->GetHeight(), SWP_NOZORDER | SWP_NOMOVE);
    CenterWindow();

    UpdateView();

    return TRUE;
}

BOOL CBackDialogBase::OnEraseBkgnd(CDC* pDC){
    return TRUE;
}

void CBackDialogBase::OnMove(int x, int y){
    CDialog::OnMove(x, y);

    if (pForeDialog != NULL){
        pForeDialog->AdjustRectByParent();
    }
}

void CBackDialogBase::OnSize(UINT nType, int cx, int cy){
    CDialog::OnSize(nType, cx, cy);

    if (pForeDialog != NULL){
        pForeDialog->AdjustRectByParent();
    }
}

int CBackDialogBase::OnCreate(LPCREATESTRUCT lpCreateStruct){
    if (CDialog::OnCreate(lpCreateStruct) == -1){
        return -1;
    }

    pForeDialog = CreateForeDialog();
    if (pForeDialog != NULL){
        pForeDialog->ShowDialog();
    }

    return 0;
}

void CBackDialogBase::UpdateView(){
    HDC hDC = ::GetDC(m_hWnd);
    HDC hdcMemory = CreateCompatibleDC(hDC);

    SIZE sizeWindow = {pImage->GetWidth(), pImage->GetHeight()};
    HBITMAP hBitMap = CreateCompatibleBitmap(hDC, sizeWindow.cx, sizeWindow.cy);
    ::SelectObject(hdcMemory, hBitMap);

    BITMAPINFOHEADER stBmpInfoHeader = { 0 };   
    int nBytesPerLine = ((sizeWindow.cx * 32 + 31) & (~31)) >> 3;
    stBmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);   
    stBmpInfoHeader.biWidth = sizeWindow.cx;   
    stBmpInfoHeader.biHeight = sizeWindow.cy;   
    stBmpInfoHeader.biPlanes = 1;   
    stBmpInfoHeader.biBitCount = 32;   
    stBmpInfoHeader.biCompression = BI_RGB;   
    stBmpInfoHeader.biClrUsed = 0;   
    stBmpInfoHeader.biSizeImage = nBytesPerLine * sizeWindow.cy;   

    PVOID pvBits = NULL;   
    HBITMAP hbmpMem = ::CreateDIBSection(NULL, (PBITMAPINFO)&stBmpInfoHeader, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (hbmpMem == NULL){
        ::DeleteDC(hdcMemory);
        ::ReleaseDC(m_hWnd, hDC);

        return;
    }
    memset( pvBits, 0, sizeWindow.cx * 4 * sizeWindow.cy);

    HGDIOBJ hbmpOld = ::SelectObject( hdcMemory, hbmpMem);

    Gdiplus::Graphics graph(hdcMemory);
    graph.SetSmoothingMode(Gdiplus::SmoothingModeNone);
    graph.DrawImage(pImage, 0, 0, sizeWindow.cx, sizeWindow.cy);

	// 绘制版本和版权信息
	CString sVersion, sCopyRight;
	sVersion.Format(_T("版本: %s"), CSetting::Instance().GetVersion());
	sCopyRight.Format(_T("%s   "), _COPYRIGHT_STRING);

	Gdiplus::FontFamily fontFamily(L"微软雅黑");
	Gdiplus::Font font(&fontFamily, Gdiplus::REAL(12), Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	Gdiplus::StringFormat sformat;
	sformat.SetAlignment(Gdiplus::StringAlignmentNear);
	sformat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	Gdiplus::SolidBrush sb(Gdiplus::Color(255, 230, 230, 230));
	UINT y = sizeWindow.cy - 45;
	graph.DrawString(sVersion, -1, &font, Gdiplus::RectF(40, y, 300, 50), &sformat, &sb);
	sformat.SetAlignment(Gdiplus::StringAlignmentFar);
	graph.DrawString(sCopyRight, -1, &font, Gdiplus::RectF(sizeWindow.cx - 340, y, 300, 50), &sformat, &sb);

    RECT rcWindow;
    GetWindowRect(&rcWindow);
    POINT ptWinPos = {rcWindow.left, rcWindow.top};
    POINT ptSrc = { 0, 0};
    BLENDFUNCTION stBlend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    ::UpdateLayeredWindow(m_hWnd, hDC, &ptWinPos, &sizeWindow, hdcMemory, &ptSrc, 0, &stBlend, ULW_ALPHA);

    graph.ReleaseHDC(hdcMemory);
    ::SelectObject( hdcMemory, hbmpOld);
    ::DeleteObject(hbmpMem); 

    ::DeleteDC(hdcMemory);
    ::ReleaseDC(m_hWnd, hDC);
}

void CBackDialogBase::OnLButtonDown(UINT nFlags, CPoint point){
    ::SendMessage( GetSafeHwnd(), WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);

    CDialog::OnLButtonDown(nFlags, point);
}