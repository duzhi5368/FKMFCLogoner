#ifndef __GET_IPADDRESS_THREAD_H_INCL__
#define __GET_IPADDRESS_THREAD_H_INCL__

#include <afxinet.h>

// CGetIPAddrThread

class CGetIPAddrThread : public CWinThread
{
	DECLARE_DYNCREATE(CGetIPAddrThread)

protected:
	CGetIPAddrThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CGetIPAddrThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMsgGetIPAddress(WPARAM wParam, LPARAM lParam);

};


#endif // __GET_IPADDRESS_THREAD_H_INCL__