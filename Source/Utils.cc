#include "stdafx.h"
#include "Utils.h"
#include "afxinet.h"
#include <vector>
CStringW AnsiToUnicode(CStringA strA){
	CStringW strW;
	WCHAR *pwcString;

	pwcString = new WCHAR[strA.GetLength() + 1];
	MultiByteToWideChar(CP_ACP, 0, strA.GetBuffer(), -1, pwcString, strA.GetLength() + 1);
	strW = pwcString;
	delete pwcString;

	return strW;
}

CStringA UnicodeToAnsi(CStringW strW){
	CStringA strA;
	char *pcString;

	pcString = new char[2 * strW.GetLength() + 1];
	WideCharToMultiByte(CP_ACP, 0, strW.GetBuffer(), -1, pcString, 2 * strW.GetLength() + 1, NULL, NULL);
	strA = pcString;
	delete pcString;

	return strA;
}

INT DownFile(const CString url, const CString path, UtilsCallFunc* func, void* obj) {
	if (url.IsEmpty()) { return -5; }
	if (path.IsEmpty()) { return -6; }

	unsigned short port; // 目标HTTP服务端口
	CString server; // 目标服务地址
	CString object; // 文件对象名

	DWORD serviceType; // 服务类型
	DWORD ret;       // 请求返回状态号

					 // 解析URL，获得服务信息
	if (!AfxParseURL(url, serviceType, server, object, port)) {
		if (func) { func(obj, -1, 1); }
		return -1; // 解析URL失败
	}

	CInternetSession intsess;
	CHttpFile* httpFile = NULL;
	CHttpConnection* httpCon = intsess.GetHttpConnection(server, port);
	if (httpCon == NULL) {
		intsess.Close();
		if (func) { func(obj, -2, 1); }
		return -2; // 建立网络连接失败
	}

	if (url[4] == 's') {
		httpFile = httpCon->OpenRequest(CHttpConnection::HTTP_VERB_GET, object, NULL, 1, NULL, NULL, INTERNET_FLAG_SECURE);
	}
	else {
		httpFile = httpCon->OpenRequest(CHttpConnection::HTTP_VERB_GET, object);
	}
	if (httpFile == NULL) {
		intsess.Close();
		delete httpCon;
		httpCon = NULL;
		if (func) { func(obj, -3, 1); }
		return -3; // GET请求失败
	}
	TRY{
		if (!httpFile->SendRequest()) {
			intsess.Close();
			delete httpCon;
			httpCon = NULL;
			if (func) { func(obj, -4, 1); }
			return -4; // GET请求失败
		}
	}
	CATCH(CException,e)
	{
		e->ReportError();
		intsess.Close();
		delete httpCon;
		httpCon = NULL;
		if (func) { func(obj, -2, 1); }
		return -2; // GET请求失败
	}
	END_CATCH

	CString fileSizeBuff;
	httpFile->QueryInfo(HTTP_QUERY_CONTENT_LENGTH, fileSizeBuff);
	DWORD fileSize = _ttoi(fileSizeBuff) + 0;
	httpFile->QueryInfoStatusCode(ret);
	if (ret != HTTP_STATUS_OK) {
		intsess.Close();
		delete httpCon;
		httpCon = NULL;
		delete httpFile;
		httpFile = NULL;
		if (func) { func(obj, -6, 1); }
		return -6; // 请求服务失败
	}

	UINT packSize = fileSize < 10240 ? fileSize:10240; //(UINT)httpFile->GetLength();
	DWORD read = 1; // 用于标记读了多少
	CHAR* buffer = new CHAR[packSize + 1]; // 缓冲区

	DWORD readSize = 0;
	TRY{
		CFile picFile(path, CFile::modeCreate | CFile::modeWrite | CFile::shareExclusive);
		while (read > 0) {
			memset(buffer, 0, (size_t)(packSize + 1));
			read = httpFile->Read(buffer, packSize);
			picFile.Write(buffer, read);
			readSize += read;

			if (func && readSize != fileSize) { func(obj, readSize, fileSize); }
		}
		picFile.Close();
		delete[]buffer;
		delete httpFile;
		delete httpCon;
		intsess.Close();
	} CATCH(CFileException, e) {
		e->ReportError();
		delete[]buffer;
		delete httpFile;
		delete httpCon;
		intsess.Close();

		if (func) { func(obj, -7, 0); }
		return -7; // 读写文件异常
	}
	END_CATCH

	if (func) { func(obj, 1, 1); }
	return 0;
}
// 执行命令
extern BOOL Execute(CString strCmd, BOOL show, CString cmd) {
	//strCmd.Format(L"/c %s", strCmd);

	SHELLEXECUTEINFO si = { sizeof(SHELLEXECUTEINFO) };
	si.lpFile = cmd;
	si.fMask = SEE_MASK_NOCLOSEPROCESS;
	si.hwnd = NULL;
	si.lpVerb = L"open";
	si.nShow = show ? SW_SHOW : SW_HIDE;
	si.lpParameters = strCmd;
	ShellExecuteEx(&si);

	WaitForSingleObject(si.hProcess, INFINITE);
	return TRUE;
}

static bool _bSeted = false;
BOOL CALLBACK  EnumFunc(HWND hwnd, LPARAM param)
{
	if (NULL == hwnd)
		return FALSE;
	LPTSTR lpStr = (LPTSTR)param;

	CString strClass;
	RealGetWindowClass(hwnd, strClass.GetBufferSetLength(50), 50);
	strClass.ReleaseBuffer();
	if (strClass.Find(_T("32770")) >= 0)
	{
		strClass.Empty();
		::GetWindowText(hwnd, strClass.GetBufferSetLength(50), 50);
		strClass.ReleaseBuffer();

		//::OutputDebugString(strClass);
		if (!strClass.IsEmpty() && strClass.Find(_T("Flash")) >= 0)
		{
			::SetWindowText(hwnd, lpStr);
			_bSeted = true;
		}
	}
	return TRUE;
}

bool  SetThreadMainWndCaption(int id , CString & strCaption)
{
	_bSeted = false;
	EnumThreadWindows(id, EnumFunc, (LPARAM)strCaption.GetBuffer());
	return _bSeted;
}

extern BOOL Execute2(CString strCmd, BOOL show, CString cmd, CString strCaption) {
	
	STARTUPINFO   Si = { 0 };
	PROCESS_INFORMATION   Pi = { 0 };

	Si.cb = sizeof(STARTUPINFO);
	Si.dwFlags = STARTF_USESHOWWINDOW | CREATE_SUSPENDED;
	Si.wShowWindow = SW_HIDE;
	// Si.dw
 
	CreateProcessW(strCmd, (LPWSTR)NULL, NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi);

	if (!strCaption.IsEmpty())
	while (1)
	{
		::Sleep(50);
		 
		bool bseted = SetThreadMainWndCaption(Pi.dwThreadId ,strCaption);
		if (bseted)
			break;
	}

	WaitForSingleObject(Pi.hProcess, INFINITE);
	return TRUE;
}

// 清理垃圾
BOOL Clearing(UtilsCallFunc* func, void* obj) {
	long p = 0;
	long m = 15;

	Execute(L"/c del /f/s/q %systemdrive%\\*.tmp");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %systemdrive%\\*._mp");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %systemdrive%\\*.log");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %systemdrive%\\*.gid");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %systemdrive%\\*.chk");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %systemdrive%\\*.old");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %systemdrive%\\recycled\\*.*");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %windir%\\*.bak");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del /f/s/q %windir%\\prefetch\\*.*");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c rd/s/q %windir%\\temp & md %windir%\\temp");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del/f/q \"%userprofile%\\cookies\\*.*\"");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del/f/q \"%userprofile%\\recent\\*.*\"");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del/f/s/q \"%userprofile%\\Local Settings\\Temporary Internet Files\\*.*\"");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del/f/s/q \"%userprofile%\\Local Settings\\Temp\\*.*\"");
	if (func) { func(obj, ++p, m); }
	Execute(L"/c del/f/s/q \"%userprofile%\\recent\\*.*\"");
	if (func) { func(obj, ++p, m); }

	return true;
}

// 修改DNS
BOOL UPDNS(UtilsCallFunc* func, void* obj) {
	long p = 0;
	long m = 6;

	HINSTANCE hInst = LoadLibrary(L"iphlpapi.dll");
	if (!hInst) {
		if (func) { func(obj, -1, m); }
		return FALSE;
	}
	typedef DWORD(CALLBACK* PGAINFO)(PIP_ADAPTER_INFO, PULONG);
	PGAINFO pGAInfo;
	pGAInfo = (PGAINFO)GetProcAddress(hInst, "GetAdaptersInfo");
	if (pGAInfo == 0) {
		FreeLibrary(hInst);
		if (func) { func(obj, -2, m); }
		return FALSE;
	}

	PIP_ADAPTER_INFO pInfo = NULL, pInfoTemp = NULL;
	ULONG ulSize = 0;
	DWORD dwRet;
	dwRet = pGAInfo(pInfo, &ulSize);
	if (dwRet != ERROR_BUFFER_OVERFLOW) {
		FreeLibrary(hInst);
		if (func) { func(obj, -3, m); }
		return FALSE;
	}

	pInfoTemp = pInfo = (PIP_ADAPTER_INFO) new (char[ulSize]);
	if (pInfo == NULL) {
		FreeLibrary(hInst);
		if (func) { func(obj, -4, m); }
		return FALSE;
	}

	dwRet = pGAInfo(pInfo, &ulSize);
	if (dwRet != ERROR_SUCCESS) {
		delete[] pInfo;
		FreeLibrary(hInst);
		if (func) { func(obj, -5, m); }
		return FALSE;
	}

	while (pInfo) {
		if (pInfo->Type == MIB_IF_TYPE_ETHERNET) {
			m += 2;
		}
		pInfo = pInfo->Next;
	}
	pInfo = pInfoTemp;

	if (func) { func(obj, ++p, m); }
	while(pInfo) {
		if (pInfo->Type == MIB_IF_TYPE_ETHERNET) {
			CString s;
			s.Format(L"/c netsh interface ip set dns %d static 114.114.114.114", pInfo->Index);
			Execute(s);
			if (func) { func(obj, ++p, m); }
			s.Format(L"/c netsh interface ip add dns %d 223.5.5.5", pInfo->Index);
			Execute(s);
			if (func) { func(obj, ++p, m); }
		}
		pInfo = pInfo->Next;
	}

	FreeLibrary(hInst);
	delete pInfoTemp;
	pInfoTemp = NULL;

	// 刷新DNS缓存
	Execute(L"/c del/f/s/q \"%userprofile%\\recent\\*.*\"");
	if (func) { func(obj, ++p, m); }
	// 查看当前所有网卡设置的DNS
	Execute(L"/c del/f/s/q \"%userprofile%\\recent\\*.*\"");
	if (func) { func(obj, ++p, m); }
	// IE浏览器缓存清理
	Execute(L"/c del/f/s/q \"%userprofile%\\recent\\*.*\"");
	if (func) { func(obj, ++p, m); }
	// FireFox浏览器缓存清理
	Execute(L"/c del/f/s/q \"%userprofile%\\recent\\*.*\"");
	if (func) { func(obj, ++p, m); }
	// Chrome浏览器缓存清理
	Execute(L"/c del/f/s/q \"%userprofile%\\recent\\*.*\"");
	if (func) { func(obj, ++p, m); }

	return TRUE;
}


CString GetRegisterValue(HKEY hkey, LPCTSTR rgspath, LPCTSTR valueName, DWORD type) {
	CString str = L"";
	unsigned char path[255];
	HKEY hKEY;
	LONG ret = RegOpenKeyEx(hkey, rgspath, 0, KEY_READ, &hKEY);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(hKEY);
		AfxMessageBox(_T("Error"));
		return str;
	}

	DWORD dwInfoSize = 255;
	ret = RegQueryValueEx(hKEY, valueName, NULL, &type, (BYTE*)path, &dwInfoSize);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(hKEY);
		return str;
	}

	unsigned char path_t[255];
	int i = 0;
	while (path[i] != '\0') {
		path_t[i / 2] = path[i];
		i += 2;
	}

	path_t[i / 2] = '\0';
	str.Format(L"%S", path_t);

	return str;
}

BOOL CheckChrome() {
	TCHAR tcProgramFile[MAX_PATH];
	SHGetSpecialFolderPath(NULL, tcProgramFile, CSIDL_PROGRAM_FILES, 0);
	
	CString chromepath;
	chromepath.Format(L"%s\\Google\\Chrome\\Application\\chrome.exe", tcProgramFile);

	if (PathFileExists(chromepath)) {
		return TRUE;
	}

	return FALSE;
}

BOOL CheckFireFox()
{
	return FALSE;
}

DWORD GetMajorVersion()
{
	DWORD dwVersion = MAKELONG(0, 0);
	WCHAR szDLLName[MAX_PATH] = { 0 };
	HRESULT hr = SHGetFolderPathW(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szDLLName);
	if (SUCCEEDED(hr) && PathAppendW(szDLLName, L"kernel32.dll"))
	{
		DWORD dwVerInfoSize = GetFileVersionInfoSizeW(szDLLName, NULL);
		if (dwVerInfoSize > 0) {
			LPVOID pvVerInfoData = (LPVOID)new BYTE[dwVerInfoSize];
			if (GetFileVersionInfoW(szDLLName, 0, dwVerInfoSize, pvVerInfoData)) {
				UINT ulLength = 0;
				VS_FIXEDFILEINFO *pvffi = NULL;
				if (VerQueryValueW(pvVerInfoData, L"\\", (LPVOID *)&pvffi, &ulLength)) {
					dwVersion = pvffi->dwFileVersionMS;
				}
			}
			delete[] pvVerInfoData;
		}
	}
	return dwVersion;
	 
}

CString GetSystemName()
{
	DWORD dwVersion = GetMajorVersion();
	int  major = HIWORD(dwVersion);
	int  sub = LOWORD(dwVersion);

	switch (major)
	{
	case 6:
		if (sub == 1)
			return _T("Win7");
		if (sub > 1)
			return _T("Win10");
		if (sub < 1)
			return _T(""); //not support
		break;
	case 10:
		return _T("Win10");
	default:
		return _T("");
	}
	 
	return _T(""); 
}

BOOL ParseStringToVector(std::vector<CString> &list, CString strString)
{
	if (strString.IsEmpty())
		return FALSE;

	int i = 0;
	CString strURL;
	while (AfxExtractSubString(strURL, strString, i,','))
	{
		i++;
		if (!strString.IsEmpty())
			list.push_back(strURL);
	}

	return TRUE;
}

/*
BOOL DoExec(CString strFile, CString strCaption)
{
	STARTUPINFO   Si = { 0 };
	PROCESS_INFORMATION   Pi = { 0 };


	Si.cb = sizeof(STARTUPINFO);
	Si.dwFlags = STARTF_USESHOWWINDOW | CREATE_SUSPENDED;
	Si.wShowWindow = SW_HIDE;
	// Si.dw

	HWND hWnd = AfxGetMainWnd()->m_hWnd;

	CreateProcessW(_T("D:\\flashplayer25ax_df_install.exe"), (LPWSTR)hWnd, NULL, NULL, FALSE, 0, NULL, NULL, &Si, &Pi);

	::Sleep(3000);
	HWND hwnd = ::FindWindow(_T("#32770"), _T("Adobe Flash Player 安装程序"));  //Adobe Flash Player 安装程序

	::SetWindowText(hwnd, _T("strCaption"));
	::ShowWindow(hwnd, SW_SHOW);

	::WaitForSingleObject(Pi.hThread, INFINITE);
	return TRUE;
}*/