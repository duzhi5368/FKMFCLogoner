#include "stdafx.h"
#include "Base/CusStatic.h"

IMPLEMENT_DYNAMIC(CusStatic, CStatic)

CusStatic::CusStatic() {
	image = NULL;
	textRect = NULL;
	textFont = NULL;
	text = "";
	textCoulor = RGB(250, 250, 250);
	maxWidth = 0;
}

CusStatic::~CusStatic() {
	if (image) {
		image->DeleteObject();
		image = NULL;
	}
}

BEGIN_MESSAGE_MAP(CusStatic, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CusStatic::OnPaint() {
	CPaintDC dc(this);
	CPaintDC* pDC = &dc;
	CRect rect;
	GetClientRect(&rect);
	CDC memDC;
	if (!memDC.m_hDC) {
		CBitmap memMap;
		memDC.CreateCompatibleDC(pDC);
		memMap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
		memDC.SelectObject(&memMap);
		memMap.DeleteObject();
		memDC.SetBkMode(TRANSPARENT);
	}

	paintOnMemDC(rect, &memDC);

	paintOnMemDCText(rect, &memDC);

	pDC->SetBkMode(TRANSPARENT);
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();
}

void CusStatic::SetText(CString t, int x, int y, int w, int h) {
	text = t;
	if (w > 0 && h > 0) {
		if (!textRect) {
			textRect = new CRect();
		}
		textRect->SetRect(x, y, x+w, y+h);
	} else if(w < 0 && h < 0 && textRect){
		delete textRect;
		textRect = NULL;
	}

	RedrawWindow(textRect);
}

void CusStatic::SetTextColor(COLORREF clr) {
	textCoulor = clr;
}

// 设置字体大小
void CusStatic::SetTextSize(int size) {
	textFont = new CFont();
	textFont->CreateFont(
		size,                        // nHeight
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

	SetFont(textFont);
}

void CusStatic::paintOnMemDCText(CRect rect, CDC* mDC) {
	if (text == "") {
		return;
	}

	if (textRect != NULL) { rect = textRect; }

	if (textFont) {
		mDC->SelectObject(textFont);
	}
	mDC->SetTextColor(textCoulor);
	mDC->DrawText(text, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void CusStatic::paintOnMemDC(CRect rect, CDC* mDC) {
	if (!image) {
		HBITMAP b = GetBitmap();
		image = new CBitmap();
		image->Attach(b);
		b = NULL;
	}

	CDC dc;
	dc.CreateCompatibleDC(mDC);
	CBitmap* pcb = dc.SelectObject(image);

	BITMAP bmp;
	image->GetBitmap(&bmp);

	int cw = bmp.bmWidth;
	int ch = bmp.bmHeight;
	int h = rect.Height(); // 绘制高度
	int maxW = rect.Width();

	double ratio = double(h) / double(ch);
	int w = int(cw * ratio); // 绘制宽度
	
	
	int x = 0;
	while (x < maxW) {
		if (x + w > maxW) {
			w = maxW - x;
			cw = int(w / ratio);
		}
		TransparentBlt(mDC->GetSafeHdc(), x, 0, w, h, dc.GetSafeHdc(), 0, 0, cw, ch, RGB(255, 0, 255));
		x += w;
	}

	dc.SelectObject(pcb);
}

void CusStatic::SetBitmap(LPCTSTR id) {
	HBITMAP hBmp = (HBITMAP)LoadImage(NULL, id,IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (hBmp) {
		if (!image) {
			image = new CBitmap;
		} else {
			image->DeleteObject();
		}
		image->Attach(hBmp);
	}
}

void CusStatic::SetBitmap(UINT id) {
	if (!image) {
		image = new CBitmap;
	} else {
		image->DeleteObject();
	}

	if (!image->LoadBitmap(id)) {
		delete image;
		image = NULL;
	}

}

void CusStatic::SetWidth(UINT w) {
	if (!image) { return;  }
	if (maxWidth > 0 && w > maxWidth) { return; }

	CRect rect;
	GetClientRect(&rect);

	BITMAP bmp;
	image->GetBitmap(&bmp);

	if (rect.Height() != bmp.bmHeight) {
		double ratio = double(rect.Height()) / double(bmp.bmHeight);
		w = w * ratio;
	}

	SetWindowPos(NULL, 0, 0, w, rect.Height(), SWP_NOMOVE);
	RedrawWindow(rect);
}

void CusStatic::SetMaxWidth(UINT w) {
	maxWidth = w;
}

UINT CusStatic::GetMaxWidth() {
	return maxWidth;
}