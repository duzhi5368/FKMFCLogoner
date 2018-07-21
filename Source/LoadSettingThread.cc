// LoadSettingThread.cpp : 实现文件
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
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CLoadSettingThread::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CLoadSettingThread, CWinThread)
	ON_THREAD_MESSAGE(MSG_LOAD_SETTING, OnLoadSetting)
END_MESSAGE_MAP()


// CLoadSettingThread 消息处理程序
void CLoadSettingThread::OnLoadSetting(WPARAM wParam, LPARAM lParam)
{

	if ( CSetting::Instance().initialize((int) wParam) == true)
	{
		m_pMainWnd->PostMessage(MSG_INIT_SETTING, 0, 0);
	}
	
}

