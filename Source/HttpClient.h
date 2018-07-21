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
	BOOL				bIsPost;				// 是否是Post请求
	CString				strServerUrl;			// 请求服务器的Url地址
	CInternetSession	*pSession;				// HTTP会话
	CHttpConnection		*pConnection;			// 管理到HTTP服务器的连接
	CHttpFile			*pHttpFile;				// 读写HTTP服务器
};