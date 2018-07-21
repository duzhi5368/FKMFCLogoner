// CheckSpeadThread.cpp : 实现文件
//

#include "stdafx.h"
#include "CheckSpeadThread.h"
#include "config.h"
#include "Setting.h"
// CCheckSpeadThread

IMPLEMENT_DYNCREATE(CCheckSpeadThread, CWinThread)

CCheckSpeadThread::CCheckSpeadThread()
{
}

CCheckSpeadThread::~CCheckSpeadThread()
{
}

BOOL CCheckSpeadThread::InitInstance()
{
	// TODO:    在此执行任意逐线程初始化
	return TRUE;
}

int CCheckSpeadThread::ExitInstance()
{
	// TODO:    在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCheckSpeadThread, CWinThread)
	ON_THREAD_MESSAGE(MSG_CHECK_SPEED, OnCheckSpeed)
	ON_THREAD_MESSAGE(MSG_SEND_RESULT, OnSendResult)
END_MESSAGE_MAP()


// CCheckSpeadThread 消息处理程序
void CCheckSpeadThread::OnCheckSpeed(WPARAM wParam, LPARAM lParam){
	int speed = CSetting::Instance().CheckSpeed(m_domainID);

	m_pMainWnd->PostMessage(MSG_CHECK_FINISH, 0, 0);

//	CSetting::Instance().SendCheckSpeedResult(m_domainID, speed);
}

void CCheckSpeadThread::OnSendResult(WPARAM wParam, LPARAM lParam)
{
	CSetting::Instance().SendCheckSpeedResult(m_domainID);
}