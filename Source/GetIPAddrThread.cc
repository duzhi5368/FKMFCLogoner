// GetIPAddrThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GetIPAddrThread.h"
#include "config.h"
#include "Setting.h"
// CGetIPAddrThread

IMPLEMENT_DYNCREATE(CGetIPAddrThread, CWinThread)

CGetIPAddrThread::CGetIPAddrThread()
{
}

CGetIPAddrThread::~CGetIPAddrThread()
{
}

BOOL CGetIPAddrThread::InitInstance()
{
	// TODO: �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CGetIPAddrThread::ExitInstance()
{
	// TODO: �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGetIPAddrThread, CWinThread)
	ON_THREAD_MESSAGE(MSG_GET_IPADDRESS, OnMsgGetIPAddress)
END_MESSAGE_MAP()


// CGetIPAddrThread ��Ϣ�������
void CGetIPAddrThread::OnMsgGetIPAddress(WPARAM wParam, LPARAM lParam)
{
	CSetting::Instance().GetLocalIpAddress();
}