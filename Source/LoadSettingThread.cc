// LoadSettingThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
//#include "PublishAddress.h"
#include "LoadSettingThread.h"
#include "config.h"

// CLoadSettingThread

IMPLEMENT_DYNCREATE(CLoadSettingThread, CWinThread)

CLoadSettingThread::CLoadSettingThread()
{

}

CLoadSettingThread::~CLoadSettingThread()
{
}

BOOL CLoadSettingThread::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CLoadSettingThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLoadSettingThread, CWinThread)
	ON_THREAD_MESSAGE(MSG_LOAD_SETTING, OnLoadSetting)
END_MESSAGE_MAP()


// CLoadSettingThread ��Ϣ�������
void CLoadSettingThread::OnLoadSetting(WPARAM wParam, LPARAM lParam)
{

	if ( CSetting::Instance().initialize((int) wParam) == true)
	{
		m_pMainWnd->PostMessage(MSG_INIT_SETTING, 0, 0);
	}
	
}

