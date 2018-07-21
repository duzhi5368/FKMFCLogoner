#include "stdafx.h"
#include "Logoner.h"
#include "LogonerBackDialog.h"
#include "LogonerForeDialog.h"


IMPLEMENT_DYNAMIC(CLogonerBackDialog, CBackDialogBase)

CLogonerBackDialog::CLogonerBackDialog(UINT nImgID, CWnd* pParent /*=NULL*/)
	: CBackDialogBase(CLogonerBackDialog::IDD, nImgID, pParent){
}

CLogonerBackDialog::~CLogonerBackDialog(){
}

void CLogonerBackDialog::DoDataExchange(CDataExchange* pDX){
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogonerBackDialog, CBackDialogBase)
END_MESSAGE_MAP()

CForeDialogBase* CLogonerBackDialog::CreateForeDialog(){
	return ::new CLogonerForeDialog(this);
}

// CLogonerBackDialog message handlers
