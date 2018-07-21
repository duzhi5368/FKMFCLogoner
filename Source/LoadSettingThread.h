#pragma once

#include "CheckSpeadThread.h"
#include "Setting.h"

// CLoadSettingThread

class CLoadSettingThread : public CWinThread
{
	DECLARE_DYNCREATE(CLoadSettingThread)

protected:
	CLoadSettingThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CLoadSettingThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:

	afx_msg void OnLoadSetting(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};


