// GetIPAddrThread.cpp : 实现文件
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
	// TODO: 在此执行任意逐线程初始化
	return TRUE;
}

int CGetIPAddrThread::ExitInstance()
{
	// TODO: 在此执行任意逐线程清理
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CGetIPAddrThread, CWinThread)
	ON_THREAD_MESSAGE(MSG_GET_IPADDRESS, OnMsgGetIPAddress)
END_MESSAGE_MAP()


// CGetIPAddrThread 消息处理程序
void CGetIPAddrThread::OnMsgGetIPAddress(WPARAM wParam, LPARAM lParam)
{
	CSetting::Instance().GetLocalIpAddress();
}