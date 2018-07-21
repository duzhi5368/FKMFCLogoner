#ifndef __SETTING_H_INCL__
#define __SETTING_H_INCL__

#include <afxmt.h>
#include <map>
#include <vector>
 
#include <algorithm>
#include "CheckSpeadThread.h"

struct DOMAIN_DATA
{
	CString	domainId;
	CString domain;
	int		speed;
	bool	sended;
};

typedef std::map<CString, DOMAIN_DATA *>	T_Domains;
typedef std::map<CString, int>				T_Speeds;
typedef std::map<CString, CString>			T_DomainIPs;
typedef std::vector<CCheckSpeadThread *>	T_CheckSpeedThreads;

class TSafeCriticalSection
{
public:
	TSafeCriticalSection(CCriticalSection & lock) 
	{
		m_pLock = &lock; 
		lock.Lock();
	}

	~TSafeCriticalSection(){
		if(m_pLock)
		{
			m_pLock->Unlock();
		}
	}

private:
	CCriticalSection * m_pLock;
};

class CSetting
{
public:
	static CSetting& Instance();

protected:
	CSetting();
public:
	~CSetting();

public:
	bool	IsInitialize() {return m_bInit;}

	bool	initialize(int idx);

	bool	ClearBestDomainId();

	CString	GetLocalIpAddress();

	CString GetVersion();

	void	CreateCheckSpeedThreads(CWnd* pWnd);

	void	StartCheckSpeed();

	int		CheckSpeed(CString id);

	void	DestroyCheckSpeedThreads();

	CString	GetDomainByDomainID(CString id);

	void	AddDomainIPs(CString domain, CString ip);

	CString GetRandomDomain(int nIndex);

	CString GetIPByDomain(CString domain);

	int		GetDomainSpeed(CString id);
	
	void	SetDomainSpeed(CString id, int speed);

	int		GetFastSpeed() {return m_fastSpeed;}

	int		GetNormalSpeed() {return m_normalSpeed;}

	void	SendCheckSpeedResult(CString id);
	
	void	ReadyToStart() {m_bToStart = true;}

	bool	IsCheckFinished();

	CString	GetBestDomainID(int idx);

	void	SetToSendResult(bool bSend) {m_bToSendResult = bSend;}

	std::vector<CString> & GetFlashPlayerURLList() {
		 return m_filesPlayerURL; 
	}
	CString GetChromeURL() { return m_chromeURL; }

private:
	bool	_initVersionConfig(CString sURL, CString sUpdateUrl);

	bool	_initDomainConfig(CString sURL);

	bool	CheckAppVertion(TCHAR * sWinName);

	BOOL	UpdateClient(LPCTSTR sWinName);

	BOOL	ReleaseRes(TCHAR* szFileName, WORD wResID, TCHAR* szFileType);

	int		_CheckSpeed(CString id);

	void	_UpdateBestDomain(CString id, int speed);

	void	ResetAllSpeed();

	CString	_GetIPAddress1();

	CString	_GetIPAddress2();

private:
	bool		m_bInit;
	bool		m_bToStart;
	bool		m_bToSendResult;

	CString		m_versionConfigURL[3];
	CString		m_domainConfigURL[3];
	CString		m_updatePackageURL[3];
	CString		m_bestDomainID[3];

	CString		m_sIpAddress;
	CString		m_sCollectResURL;
	CString		m_updateURL;
	CString     m_strVersion;
	CString		m_strUpdateVersion;
	std::vector<CString>  m_filesPlayerURL;
	CString     m_chromeURL;
	int			m_fastSpeed;
	int			m_normalSpeed;

	T_Domains	m_domains;
	T_DomainIPs m_ips;

	CCriticalSection m_domainsLock;
	CCriticalSection m_domainIPsLock;
	CCriticalSection m_UrlsLock;
	
	T_CheckSpeedThreads m_pThreads;
	T_CheckSpeedThreads	m_pSendThreads;
	int			m_threadsCount;
	
};

#endif // __SETTING_H_INCL__
