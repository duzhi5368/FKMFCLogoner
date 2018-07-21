// CheckSpeadThread.cpp : ʵ���ļ�
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
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CCheckSpeadThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CCheckSpeadThread, CWinThread)
	ON_THREAD_MESSAGE(MSG_CHECK_SPEED, OnCheckSpeed)
	ON_THREAD_MESSAGE(MSG_SEND_RESULT, OnSendResult)
END_MESSAGE_MAP()


// CCheckSpeadThread ��Ϣ�������
void CCheckSpeadThread::OnCheckSpeed(WPARAM wParam, LPARAM lParam){
	int speed = CSetting::Instance().CheckSpeed(m_domainID);

	m_pMainWnd->PostMessage(MSG_CHECK_FINISH, 0, 0);

//	CSetting::Instance().SendCheckSpeedResult(m_domainID, speed);
}

void CCheckSpeadThread::OnSendResult(WPARAM wParam, LPARAM lParam)
{
	CSetting::Instance().SendCheckSpeedResult(m_domainID);
}