#pragma once

#include "BaseClient.h"
#include <WS2tcpip.h>
#include <WinInet.h>
#include <afxinet.h>

class HttpClient : public BaseClient 
{
public:
	HttpClient();
	~HttpClient();
public:
	CString GetAppTokenFromGateServer(ConfigInfo info);
	CString GetIPFromGateServer(ConfigInfo info);
	CString GetVersionsFromGateServer(ConfigInfo info);
	CString GetIPFromPureDNS(CString url);
	void	Clear();
public:
	BOOL				bIsPost;				// �Ƿ���Post����
	CString				strServerUrl;			// �����������Url��ַ
	CInternetSession	*pSession;				// HTTP�Ự
	CHttpConnection		*pConnection;			// ����HTTP������������
	CHttpFile			*pHttpFile;				// ��дHTTP������
};