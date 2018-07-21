#pragma once
#include "Base/BackDialogBase.h"

class CLogonerBackDialog : public CBackDialogBase{
	DECLARE_DYNAMIC(CLogonerBackDialog)

public:
	CLogonerBackDialog(UINT nImgID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogonerBackDialog();

	enum { IDD = IDD_LOGONER_BACKDIALOG };

	virtual CForeDialogBase* CreateForeDialog();

	void UpdateView();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
