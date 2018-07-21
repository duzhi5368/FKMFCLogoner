#include "stdafx.h"
#include "Setting.h"
#include <afxinet.h>
#include <winver.h>
#include "Json/json.h"
#include "config.h"
#include "Utils.h"

#define FREE_APTR(x)	if(x){delete [] x;	x = NULL;	}

CSetting& CSetting::Instance()
{
	static CSetting sInst;

	return sInst;
}

CSetting::CSetting()
{
	m_sIpAddress			= _T("");
	m_sCollectResURL		= _T("");
	m_updateURL				= _T("");
	m_strVersion			= _T("");
	m_strUpdateVersion		= _T("");
	//m_filesPlayerURL = _T("");
	m_chromeURL = _T("");
	m_bInit					= false;
	m_bToStart				= false;
	m_bToSendResult			= false;
	m_fastSpeed				= 2000;
	m_normalSpeed			= 10000;
	m_threadsCount			= 0;

	m_versionConfigURL[0]	= _VERSION_CONFIG_URL1;
	m_versionConfigURL[1]	= _VERSION_CONFIG_URL2;
	m_versionConfigURL[2]	= _VERSION_CONFIG_URL3;

	m_domainConfigURL[0]	= _DOMAIN_CONFIG_URL1;
	m_domainConfigURL[1]	= _DOMAIN_CONFIG_URL2;
	m_domainConfigURL[2]	= _DOMAIN_CONFIG_URL3;

	m_updatePackageURL[0]	= _UPDATE_PACK_URL1;
	m_updatePackageURL[1]	= _UPDATE_PACK_URL2;
	m_updatePackageURL[2]	= _UPDATE_PACK_URL3;

	m_bestDomainID[0]		= _T("");
	m_bestDomainID[1]		= _T("");
	m_bestDomainID[2]		= _T("");
}


CSetting::~CSetting()
{

}

bool CSetting::initialize(int idx)
{

	if(m_bInit) 
		return false;

	if ( _initVersionConfig(m_versionConfigURL[idx], m_updatePackageURL[idx]) )
	{

		m_bInit = true;
		return true;

		CString sVersion = GetVersion();
		CString sWinText;
		sWinText.Format(_T("%s %s"), _MAINWIN_STRING, sVersion);

		if( !CheckAppVertion(sWinText.GetBuffer()) )
			return false;

		sVersion.ReleaseBuffer();
		sWinText.ReleaseBuffer();
	}

	m_bInit = true;
	return true;

	m_bInit = _initDomainConfig(m_domainConfigURL[idx]);

#if 0 // 测试用
	DOMAIN_DATA *pDomainData = new DOMAIN_DATA();

	pDomainData->domainId = _T("101");
	pDomainData->domain = _T("www.18btt.com");
	pDomainData->speed = 0;
	m_domains[pDomainData->domainId] = pDomainData;

	pDomainData = new DOMAIN_DATA();

	pDomainData->domainId = _T("102");
	pDomainData->domain = _T("www.118btt.com");
	pDomainData->speed = 0;
	m_domains[pDomainData->domainId] = pDomainData;
	
	pDomainData = new DOMAIN_DATA();

	pDomainData->domainId = _T("103");
	pDomainData->domain = _T("www.01918999.net");
	pDomainData->speed = 0;
	m_domains[pDomainData->domainId] = pDomainData;

	pDomainData = new DOMAIN_DATA();

	pDomainData->domainId = _T("104");
	pDomainData->domain = _T("www.168btt777.net");
	pDomainData->speed = 0;
	m_domains[pDomainData->domainId] = pDomainData;

	Sleep(2000);
	m_bInit = true;
#endif
	return m_bInit;
}

bool	CSetting::_initVersionConfig(CString sURL, CString sUpdateUrl)
{
	bool bRet = false;

	CInternetSession session;
	CHttpFile *pHttpFile = NULL;

	try
	{
		pHttpFile = (CHttpFile*)session.OpenURL(sURL);
	}
	catch (CException* e)
	{
		int n= GetLastError();
		pHttpFile = NULL;
		return false;
	}

	if (!pHttpFile)
	{
		return false;
	}

	char	szContent[10240] = { 0 };
	memset(szContent, 0, sizeof(szContent));

	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

	do 
	{
		if (bSuccess && dwStatus >= 200 && dwStatus < 300)
		{
			if (pHttpFile->Read(szContent, sizeof(szContent) - 1) <= 0)
			{
				break;
			}

			Json::Reader	reader;
			Json::Value		value;
			if( reader.parse( szContent, value, false ) == false )
			{
				break;
			}
			
			{
				//多线程
				TSafeCriticalSection slock(m_UrlsLock);

				m_updateURL = sUpdateUrl;
				m_strUpdateVersion	= AnsiToUnicode(value["updateVersion"].asCString());
				m_sCollectResURL	= AnsiToUnicode(value["collectResultURL"].asCString());

				
				CString strWin = GetSystemName();
				CString nodName = _T("flashPlayer");
				if (!strWin.IsEmpty()) //flashPlayerWin7 /flashPlayerWin10  --支持win7/win10 np,pp
					nodName += strWin;

				if (!value["flashPlayer"].isNull()) { //flashPlayer节点必须要有，存放window的默认安装器

					if (m_filesPlayerURL.empty())
					{
#ifndef _DEBUG	
						CString strURLs = AnsiToUnicode(value["flashPlayer"].asCString());
						ParseStringToVector(m_filesPlayerURL,strURLs);
						
						//m_filesPlayerURL.push_back(AnsiToUnicode(value["flashPlayer"].asCString()));
						if (nodName.CompareNoCase(_T("flashPlayer")) != 0)
						{
							CStringA nodAName = CT2A(nodName).m_psz;
							if (!value[nodAName].isNull())
							{
								CString strURLs = AnsiToUnicode(value[nodAName].asCString());
								ParseStringToVector(m_filesPlayerURL, strURLs);
							}
						}
#else 

						CString strURLs = _T("https://admdownload.adobe.com/bin/live/flashplayer25pp_ga_install.exe,")\
										  _T("https://admdownload.adobe.com/bin/live/flashplayer25_ga_install.exe,")\
										  _T("https://admdownload.adobe.com/bin/live/flashplayer25ax_ha_install.exe" );
						ParseStringToVector(m_filesPlayerURL, strURLs);
#endif
					}

				}

				if (!value["chrome"].isNull()) {
					m_chromeURL = AnsiToUnicode(value["chrome"].asCString());
				}

				Json::Value speed	= value["speed"];
				if(!speed.isNull())
				{
					m_fastSpeed		= atoi(speed["fast"].asCString());
					m_normalSpeed	= atoi(speed["normal"].asCString());
				}

				bRet = true;
			}
		}

	} while (0);
		

	pHttpFile->Close();
	delete pHttpFile;
	session.Close();

	return bRet;
}

bool	CSetting::_initDomainConfig(CString sURL)
{

	bool bRet = false;

	CInternetSession session;
	CHttpFile *pHttpFile = NULL;

	try
	{
		pHttpFile = (CHttpFile*)session.OpenURL(sURL);
	}
	catch (CException* e)
	{
		pHttpFile = NULL;
#ifdef _DEBUG
		TCHAR errMsg[255] = {0};
		e->GetErrorMessage(errMsg, 255);
		AfxMessageBox(errMsg);
#endif
		return false;
	}

	if (!pHttpFile)
	{
		return false;
	}

	char	szContent[10240] = { 0 };
	memset(szContent, 0, sizeof(szContent));

	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);
 
#ifdef _DEBUG
	if (FALSE == bSuccess)
	{
		TCHAR *errMsg = _T("http文件请求失败！");
		AfxMessageBox(errMsg);
	}
#endif
 
	do 
	{
		if (bSuccess && dwStatus >= 200 && dwStatus < 300)
		{
			if (pHttpFile->Read(szContent, sizeof(szContent) - 1) <= 0)
			{
				break;
			}

			Json::Reader	reader;
			Json::Value		value;
			if( reader.parse( szContent, value, false ) == false )
			{
#ifdef _DEBUG 
				CString strErr(_T("域名内容解析失败："));
				strErr+=szContent;
				AfxMessageBox(strErr);
#endif
				break;
			}

			Json::Value domains = value["domains"];

			if(!domains.isArray())
			{
				break;
			}

			{
				TSafeCriticalSection slock(m_domainsLock);

				int nCount = domains.size();
				for(int i = 0; i < nCount; i++)
				{
					DOMAIN_DATA *pDomainData = new DOMAIN_DATA();
					Json::Value element = domains[i];
					int nID = element["domainId"].asInt();
					pDomainData->domainId.Format(_T("%d"), nID);
					pDomainData->domain = AnsiToUnicode(element["domain"].asCString());
					pDomainData->speed = 0;
					if(pDomainData->domainId != _T("") && pDomainData->domain != _T(""))
					{
						m_domains[pDomainData->domainId] = pDomainData;
					}
				}
			}

			bRet = true;
		}

	} while (0);

	pHttpFile->Close();
	delete pHttpFile;
	session.Close();

	{
		m_ips.clear();
	}

	return bRet;
}

CString	CSetting::GetLocalIpAddress()
{
	if(m_sIpAddress != _T(""))
		return m_sIpAddress;

	m_sIpAddress = _GetIPAddress1();
	if(m_sIpAddress != _T(""))
		return m_sIpAddress;
	
	m_sIpAddress = _GetIPAddress2();

	return m_sIpAddress;
}

CString	CSetting::_GetIPAddress1()
{
	CInternetSession session;
	CHttpFile *	pHttpFile	= NULL;
	CString		ipAddress	= _T("");
	TCHAR *		szUrl		= _T("http://www.ip138.com/ip2city.asp");

	try
	{
		pHttpFile = (CHttpFile*)session.OpenURL(szUrl);
	}
	catch (CException* e)
	{
		pHttpFile = NULL;
		return ipAddress;
	}

	if (!pHttpFile)
	{
		return ipAddress;
	}

	char	szIpAddress[32];
	char	szContent[10240] = { 0 };
	memset(szContent, 0, sizeof(szContent));
	memset(szIpAddress, 0, sizeof(szIpAddress));

	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

	do 
	{
		if (bSuccess && dwStatus >= 200 && dwStatus < 300)
		{
			if (pHttpFile->Read(szContent, sizeof(szContent) - 1) <= 0)
			{
				break;
			}

			char *start= strstr(szContent, ("["));
			if(!start)
			{
				break;
			}
			start=start+strlen("[");
			char *temp=strstr(start, ("]"));

			strncpy(szIpAddress, start, temp-start);

			ipAddress = AnsiToUnicode(szIpAddress);
		}

	} while (0);

	if( pHttpFile )
	{
		pHttpFile->Close();

		delete pHttpFile;
		pHttpFile = NULL;
	}
	session.Close();

	return ipAddress;
}

CString	CSetting::_GetIPAddress2()
{
	CInternetSession session;
	CHttpFile *	pHttpFile	= NULL;
	CString		ipAddress	= _T("");
	TCHAR *		szUrl		= _T("http://whereismyip.com");

	try
	{
		pHttpFile = (CHttpFile*)session.OpenURL(szUrl);
	}
	catch (CException* e)
	{
		pHttpFile = NULL;
		return ipAddress;
	}

	if (!pHttpFile)
	{
		return ipAddress;
	}

	char	szIpAddress[32];
	char	szContent[10240] = { 0 };
	memset(szContent, 0, sizeof(szContent));
	memset(szIpAddress, 0, sizeof(szIpAddress));

	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

	do 
	{
		if (bSuccess && dwStatus >= 200 && dwStatus < 300)
		{
			if (pHttpFile->Read(szContent, sizeof(szContent) - 1) <= 0)
			{
				break;
			}
			const char * szTag = "color=\"#000000\" size=\"10\" face=\"verdana\">";
			char *start= strstr(szContent, szTag);
			if(!start)
			{
				break;
			}
			start=start+strlen(szTag);
			char *temp=strstr(start, ("</font>"));

			strncpy(szIpAddress, start, temp-start);

			ipAddress = AnsiToUnicode(szIpAddress);
		}

	} while (0);

	if( pHttpFile )
	{
		pHttpFile->Close();

		delete pHttpFile;
		pHttpFile = NULL;
	}
	session.Close();

	return ipAddress;
}

CString	CSetting::GetDomainByDomainID(CString id)
{
	TSafeCriticalSection slock(m_domainsLock);

	T_Domains::iterator iter = m_domains.find(id);
	if(iter != m_domains.end())
	{
		return iter->second->domain;
	}
	return _T("");
}

void CSetting::AddDomainIPs(CString domain, CString ip) 
{
	TSafeCriticalSection slock(m_domainIPsLock);

	T_DomainIPs::iterator iter = m_ips.find(domain);
	if (iter != m_ips.end())
	{
		return;
	}

	m_ips.insert(std::map<CString, CString> ::value_type(domain, ip));
}

CString CSetting::GetRandomDomain(int nIndex)
{
	TSafeCriticalSection slock(m_domainIPsLock);

	int nCount = m_ips.size();
	if (nCount <= 0)
		return _T("");
	int nRIndex = nIndex % nCount;
	CString strKey = _T("");
	int n = 0;

	T_DomainIPs httpsIPs;
	for (T_DomainIPs::iterator iter = m_ips.begin(); iter != m_ips.end(); iter++) {
		// 移除http
		if ((*iter).second.Find(_T("0.0.0.0")) == -1)
		{
			httpsIPs.insert(std::map<CString, CString> ::value_type((*iter).first, (*iter).second));
		}
	}

	if (httpsIPs.size() <= 0) 
	{
		// 全http
		for (T_DomainIPs::iterator iter = m_ips.begin(); iter != m_ips.end(); iter++)
		{
			if (n == nRIndex) {
				strKey = (*iter).first;
				break;
			}
			n++;
		}
	}
	else {
		nCount = httpsIPs.size();
		nRIndex = nIndex % nCount;
		// 有https
		for (T_DomainIPs::iterator iter = httpsIPs.begin(); iter != httpsIPs.end(); iter++)
		{
			if (n == nRIndex) {
				strKey = (*iter).first;
				break;
			}
			n++;
		}
	}
	
	return strKey;
}

CString CSetting::GetIPByDomain(CString domain) 
{
	TSafeCriticalSection slock(m_domainIPsLock);

	T_DomainIPs::iterator iter = m_ips.find(domain);
	if (iter != m_ips.end())
	{
		return iter->second;
	}
	return _T("255.255.255.255");
}

void	CSetting::SetDomainSpeed(CString id, int speed)
{
	TSafeCriticalSection slock(m_domainsLock);

	T_Domains::iterator iter = m_domains.find(id);
	if(iter != m_domains.end())
	{
		iter->second->speed = speed;
	}
}

int		CSetting::GetDomainSpeed(CString id)
{
	TSafeCriticalSection slock(m_domainsLock);

	T_Domains::iterator iter = m_domains.find(id);
	if(iter != m_domains.end())
	{
		return iter->second->speed;
	}

	return MAX_SPEED;
}

CString CSetting::GetVersion(){
	if(m_strVersion != _T(""))
		return m_strVersion;

	TCHAR szPath[MAX_PATH] = { 0 };
	::GetModuleFileName(NULL, szPath, MAX_PATH);

	DWORD dwLen = 0, dwSize = 0;
	dwLen = GetFileVersionInfoSize(szPath, &dwSize);
	if (dwLen <= 0) return FALSE;

	TCHAR *pBuffer = new TCHAR[dwLen + 2];

	try{
		if (GetFileVersionInfo(szPath, NULL, dwLen, pBuffer)){
			VS_FIXEDFILEINFO *pFileInfo = NULL;
			dwLen = sizeof(VS_FIXEDFILEINFO);

			if (VerQueryValue(pBuffer, _T("\\"), (LPVOID *)&pFileInfo, (PUINT)&dwLen) != 0){
				if (pFileInfo != NULL){
					TCHAR *lpszVer = new TCHAR[dwLen];
					ZeroMemory(lpszVer, dwLen);

					wsprintf(lpszVer, _T("%d.%d.%d.%d"),
						HIWORD(pFileInfo->dwProductVersionMS),
						LOWORD(pFileInfo->dwProductVersionMS),
						HIWORD(pFileInfo->dwProductVersionLS),
						LOWORD(pFileInfo->dwProductVersionLS));

					m_strVersion = lpszVer;

					FREE_APTR(lpszVer);
				}
			}
		}
	}
	catch (...){}
	FREE_APTR(pBuffer);

	return m_strVersion;
}

bool	CSetting::CheckAppVertion(TCHAR * sWinName)
{
	if(m_strVersion != m_strUpdateVersion)
	{
		//if (MessageBox(NULL, _T("是否升级新版本!"), _T("提示"),
		//	MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDYES)
		{
 			if (ReleaseRes(_T("XUpdater.exe"), (WORD)IDR_UPDATE, _T("EXE")))
 			{
 				UpdateClient(sWinName);
 			}
		}
		return false;
	}
	return true;
}

BOOL	CSetting::UpdateClient(LPCTSTR sWinName)
{
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szXML[_MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH);
	GetTempPath(MAX_PATH, szXML);
	(_tcsrchr(szXML, '\\'))[1] = 0;
	_tcscat(szXML, _T("XUpdater.exe"));

	_stprintf(szXML + lstrlen(szXML),
		_T(" <wnd>%s</wnd><app>%s</app><files><file>%s</file></files>"),
		sWinName, szPath, m_updateURL.GetBuffer());

	::WinExec(UnicodeToAnsi(szXML), SW_SHOW);

	m_updateURL.ReleaseBuffer();

	return TRUE;
}


BOOL	CSetting::ReleaseRes(TCHAR* szFileName, WORD wResID, TCHAR* szFileType)
{
	DWORD dwWrite(0);
	HANDLE hFile;
	TCHAR  szTempPath[MAX_PATH];

	GetTempPath(MAX_PATH, szTempPath);
	(_tcsrchr(szTempPath, '\\'))[1] = 0;
	_tcscat(szTempPath, szFileName);

	hFile = CreateFile(szTempPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	// 查找资源文件中、加载资源到内存、得到资源大小
	HRSRC	hrsc	= FindResource(NULL, MAKEINTRESOURCE(wResID), szFileType);
	HGLOBAL hG		= LoadResource(NULL, hrsc);
	DWORD	dwSize	= SizeofResource(NULL, hrsc);

	// 写入文件
	WriteFile(hFile, hG, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);
	return TRUE;
}

void	CSetting::CreateCheckSpeedThreads(CWnd* pWnd)
{
	int index = 0;

	if(m_threadsCount > 0)
		return;

	TSafeCriticalSection slock(m_domainsLock);

	T_Domains::iterator itor = m_domains.begin();

	for (index = 0;itor != m_domains.end(); itor++, index++)
	{
		m_pThreads.push_back((CCheckSpeadThread *)AfxBeginThread(RUNTIME_CLASS(CCheckSpeadThread)));
		m_pThreads[index]->m_pMainWnd = pWnd;
		m_pThreads[index]->SetDomainID(itor->second->domainId);
/*
		m_pSendThreads.push_back((CCheckSpeadThread *)AfxBeginThread(RUNTIME_CLASS(CCheckSpeadThread)));
		m_pSendThreads[index]->m_pMainWnd = pWnd;
		m_pSendThreads[index]->SetDomainID(itor->second->domainId);
*/	
	}

	m_threadsCount = index;
}

void	CSetting::StartCheckSpeed()
{
	if(!m_bInit)
		return;

	if(!m_bToStart)
		return;

	m_bToStart = false;

	ResetAllSpeed();
	
	ClearBestDomainId();

	SetToSendResult(false);

	for (int index = 0; index < m_threadsCount; index++)
	{
		m_pThreads[index]->PostThreadMessageW(MSG_CHECK_SPEED, 0, 0);
	}
}

void	CSetting::ResetAllSpeed()
{
	TSafeCriticalSection slock(m_domainsLock);

	T_Domains::iterator iter = m_domains.begin();
	for(; iter != m_domains.end(); ++iter)
	{
		iter->second->speed  = DEFAULT_SPEED;
		iter->second->sended = false;
	}
}

void	CSetting::DestroyCheckSpeedThreads()
{
	for (int index = 0; index < m_threadsCount; index++)
	{
		if (!IsBadReadPtr(m_pThreads[index], sizeof(CWinThread *)))
		{
			m_pThreads[index]->PostThreadMessage(WM_QUIT, 0, 0);
			Sleep(2);
		}

		//if (!IsBadReadPtr(m_pSendThreads[index], sizeof(CWinThread *)))
		//{
		//	m_pSendThreads[index]->PostThreadMessage(WM_QUIT, 0, 0);
		//	Sleep(2);
		//}
	}
}

int		CSetting::CheckSpeed(CString id)
{
	int speed = _CheckSpeed(id);
	
	SetDomainSpeed(id, speed);
	
	_UpdateBestDomain(id, speed);

	/*
	CString strTmp;
	strTmp.Format(_T("*******************threadid: %lu, id:%s, speed:%d\n"), GetCurrentThreadId(), id.GetBuffer(), speed);

	TRACE( strTmp );

	for (int index = 0; index < m_threadsCount; index++)
	{
		if (m_pSendThreads[index]->GetDomainID() == id)
		{
			m_pSendThreads[index]->PostThreadMessage(MSG_SEND_RESULT, 0, 0);
			break;
		}
	}
	*/
	

	return speed;
}

void	CSetting::_UpdateBestDomain(CString id, int speed)
{
	if(MAX_SPEED == speed || DEFAULT_SPEED == speed)
		return;

	TSafeCriticalSection slock(m_domainsLock);

	if(m_bestDomainID[0] == _T(""))
	{
		m_bestDomainID[0] = id;
		return;
	}

	//比第一个快
	T_Domains::iterator iter = m_domains.find(m_bestDomainID[0]);
	if(iter != m_domains.end())
	{
		DOMAIN_DATA * pData = iter->second;
		if ( pData && pData->speed > speed )
		{
			m_bestDomainID[2] = m_bestDomainID[1];
			m_bestDomainID[1] = m_bestDomainID[0];
			m_bestDomainID[0] = id;
			return;
		}
	}

	//比第二个快
	if(m_bestDomainID[1] == _T(""))
	{
		m_bestDomainID[1] = id;
		return;
	}
	iter = m_domains.find(m_bestDomainID[1]);
	if(iter != m_domains.end())
	{
		if ( iter->second->speed > speed )
		{
			m_bestDomainID[2] = m_bestDomainID[1];
			m_bestDomainID[1] = id;
			return;
		}
	}

	//比第三个快
	if(m_bestDomainID[2] == _T(""))
	{
		m_bestDomainID[2] = id;
		return;
	}
	iter = m_domains.find(m_bestDomainID[2]);
	if(iter != m_domains.end())
	{
		if ( iter->second->speed > speed )
		{
			m_bestDomainID[2] = id;
			return;
		}
	}
}

int		CSetting::_CheckSpeed(CString id)
{
// #define CHECK_TEST_JSP

	CInternetSession	session;
	CHttpFile *			pHttpFile = NULL;

	CString domain	= GetDomainByDomainID(id);
	CString url		= domain;
	 

	CString header = domain.Left(4);
	if (header.CompareNoCase(_T("http")) != 0)
	{
#if defined(Ag8_CONFIG)
		url.Format(_T("https://%s"), domain);
#else
		url.Format(_T("http://%s"), domain);
#endif
	}


#ifdef CHECK_TEST_JSP
	url += _T("/test.jsp");
#elif CHECK_TEST_PHP
	url += _T("/time.php");
#elif CHECK_TEST_TXT
	url += _T("/version.txt");
#endif

	int tick = GetTickCount();

	try{
		pHttpFile = (CHttpFile*)session.OpenURL(url);
	}catch (CException* e){
		pHttpFile = NULL;
#ifdef _DEBUG
		TCHAR errMsg[255] = { 0 };
		e->GetErrorMessage(errMsg, 255);
		AfxMessageBox(errMsg);
#endif
		return MAX_SPEED;
	}

	if (!pHttpFile)
	{
		return MAX_SPEED;
	}

	char szContent[10240] = { 0 };
	memset(szContent, 0, sizeof(szContent));

	DWORD dwStatus;
	DWORD dwBuffLen = sizeof(dwStatus);
	BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

	if (bSuccess && dwStatus >= HTTP_STATUS_OK && dwStatus < HTTP_STATUS_AMBIGUOUS)
	{
		if (pHttpFile->Read(szContent, sizeof(szContent) - 1) <= 0)
		{
			tick = MAX_SPEED;
		}
		else
		{
#ifdef CHECK_TEST_JSP
			char *start= strstr(szContent, ("SUCCEDED"));
			if(!start)
			{
				tick = MAX_SPEED;
			}
			else
#elif	CHECK_TEST_PHP
			char *start = strstr(szContent, "SUCCESS!");
			if(!start)
			{
				tick = MAX_SPEED;
			}
			else
#endif
			{
				tick = GetTickCount() - tick;
			}
		}	
	}
	else if( dwStatus >= HTTP_STATUS_REDIRECT && dwStatus<= HTTP_STATUS_NOT_MODIFIED)
	{
	//	CString StrContent, Strtmp;

	//	if( pHttpFile )
	//	{
	//		while( pHttpFile->ReadString( Strtmp )!= NULL)
	//		{
	//			StrContent += Strtmp;
	//		}
	//	}

	//	AfxMessageBox(StrContent);

	//#if	CHECK_TEST_PHP
	//	if (StrContent.Find(_T("SUCCESS!"))==0)
	//	{
	//		tick = MAX_SPEED;
	//	}
	//	else
	//#endif
		{
			tick = GetTickCount() - tick;
		}
	}
	else
	{
		tick = MAX_SPEED;
	}

	pHttpFile->Close();
	delete pHttpFile;
	session.Close();

	return tick;
}

void	CSetting::SendCheckSpeedResult(CString id)
{
	while(!m_bToSendResult)
	{
		Sleep(100);
	}

	Sleep(10);

	#define NORMAL_CONNECT	INTERNET_FLAG_KEEP_CONNECTION
	#define SECURE_CONNECT	NORMAL_CONNECT | INTERNET_FLAG_SECURE
	#define NORMAL_REQUEST	INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE
	#define SECURE_REQUEST	NORMAL_REQUEST | INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID

	if(m_sCollectResURL == _T(""))
		return;

	CInternetSession	session;
	CHttpFile *			pHttpFile	= NULL;
	CHttpConnection *	pConnection = NULL;

	CString strSever;
	CString	strObject;
	DWORD	dwServiceType;
	INTERNET_PORT	nPort;
	
	AfxParseURL(m_sCollectResURL, dwServiceType, strSever, strObject, nPort);

	if(AFX_INET_SERVICE_HTTP != dwServiceType && AFX_INET_SERVICE_HTTPS != dwServiceType)
	{
		return;
	}

	try
	{
		pConnection = session.GetHttpConnection(strSever, 
			AFX_INET_SERVICE_HTTP == dwServiceType ? NORMAL_CONNECT : SECURE_CONNECT, 
			nPort);

		if(!pConnection)
		{
			return;
		}
		
		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, 
				strObject,
				NULL, 
				1, 
				NULL, 
				NULL,
				AFX_INET_SERVICE_HTTP == dwServiceType ? NORMAL_REQUEST : SECURE_REQUEST);

		if( pHttpFile )
		{
			pHttpFile->AddRequestHeaders(_T("Accept: *,*/*"));
			pHttpFile->AddRequestHeaders(_T("Accept-Language: zh-cn"));
			pHttpFile->AddRequestHeaders(_T("Content-Type: application/x-www-form-urlencoded"));
			pHttpFile->AddRequestHeaders(_T("Accept-Encoding: gzip, deflate"));
			
			CString szFormData;

			{
				TSafeCriticalSection slock(m_domainsLock);

				T_Domains::iterator iter = m_domains.find(id);
				if(iter != m_domains.end())
				{
					CTime time = CTime::GetCurrentTime();
					CString strTime = time.Format(_T("%Y-%m-%d %H:%M:%S"));

					szFormData.Format(_T("area=&clientKey=&clientVersion=%s&costTime=%d&domain=%s&domain_id=%s&ip=%s&pingTime=%s"), 
						GetVersion(), iter->second->speed, iter->second->domain, id, GetLocalIpAddress(), strTime); 
				}			
			}

			CStringA szFormDataA = UnicodeToAnsi(szFormData);

			BOOL bResult = pHttpFile->SendRequest(NULL, 0, (LPVOID) szFormDataA.GetBuffer(), (DWORD)szFormDataA.GetLength());
		}
	}
	catch (CException* e)
	{
		if(pHttpFile)
		{
			pHttpFile->Close();
			delete pHttpFile;
		}
		session.Close();

		return;
	}

	if( pHttpFile )
	{
		pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;
	}
	session.Close();

}

bool	CSetting::IsCheckFinished()
{
	bool bCheckAll = true;
	
	int  iBest = 0;
	
	TSafeCriticalSection slock(m_domainsLock);

	T_Domains::iterator iter = m_domains.begin();
	for(; iter != m_domains.end(); ++iter)
	{
		if ( iter->second->speed == DEFAULT_SPEED)
		{
			bCheckAll = false;
		}
		else if(iter->second->speed != MAX_SPEED)
		{
			iBest++;
		}
	}

	if(m_bInit && m_domains.size() == 0)
	{
		return true;
	}

	if(iBest >= 3 || bCheckAll)
		return true;
	else 
		return false;

}

CString	CSetting::GetBestDomainID(int idx)
{
	if(idx >= 0 && idx <=2)
		return (m_bestDomainID[idx]);

	return _T("");
}

bool CSetting::ClearBestDomainId()
{
	TSafeCriticalSection slock(m_domainsLock);

	m_bestDomainID[0]		= _T("");
	m_bestDomainID[1]		= _T("");
	m_bestDomainID[2]		= _T("");

	return true;
}
