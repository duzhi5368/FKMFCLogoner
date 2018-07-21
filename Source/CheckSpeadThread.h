#ifndef __CHECK_SPEED_THREAD_H_INCL__
#define __CHECK_SPEED_THREAD_H_INCL__

#include <afxinet.h>
#include <string>
// CCheckSpeadThread

class CCheckSpeadThread : public CWinThread
{
	DECLARE_DYNCREATE(CCheckSpeadThread)

protected:
	CCheckSpeadThread();           // 动态创建所使用的受保护的构造函数
	virtual ~CCheckSpeadThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void	SetDomainID(CString id)
	{
		CString StrTmp;
		StrTmp.Format(_T("***********threadid=%d, id=%s******************\n"), GetCurrentThreadId(), id.GetBuffer());
		TRACE(StrTmp);
		m_domainID = id;
	}

	CString GetDomainID()
	{
		return m_domainID;
	}

protected:

	afx_msg void OnCheckSpeed(WPARAM wParam, LPARAM lParam);

	afx_msg void OnSendResult(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

private:
	CString	m_domainID;
};

#endif // __CHECK_SPEED_THREAD_H_INCL__
