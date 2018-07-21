#pragma once

#include "BaseClient.h"

class HttpsClient : public BaseClient
{
public:
	HttpsClient();
	~HttpsClient();

public:
	CString GetAppTokenFromGateServer(ConfigInfo info);
	CString GetIPFromGateServer(ConfigInfo info);
	CString GetVersionsFromGateServer(ConfigInfo info);
	CString GetIPFromPureDNS(CString url);
	void	Clear();

public:
	CString			cstrServerUrl;                // ·þÎñÆ÷µØÖ·
};