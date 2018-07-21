#pragma once

#include "afxwin.h"
/************************************************************************/
/*��GET��POST���͵Ļ��࣬���ڹ涨�����������ʵ�ֵĽӿ�                                                                      */
/************************************************************************/

struct ConfigInfo {
	CString strPlatform;
	CString strAppID;
	CString strAppKey;

	CString strGateUrl;
	CString strAppTokenSuffix;

	bool bIsUseSSL;
	int nPort;

private:
	CString strTimestamp;
	CString strLocalIP;
	CString strSignSrc;
	CString strSign;
	CString strPostUrl;
	CString strPostData;
	CString strPostHeader;

	CString strAppToken;

public:
	ConfigInfo() {
		strTimestamp = GetTimestamp();
		strLocalIP = GetLocalIP();
		strPlatform = _T("wap");
		nPort = 80;
	}
	// ��ʼ���ṹ����
	void Init();
	void InitVersion();
	
	const CString GetPostUrl() { return strPostUrl; };
	const CString GetPostData() { return strPostData; };
	const CString GetPostHeader() { return strPostHeader; };
	const CString GetAppToken() { return strAppToken; };
	void SetAppToken(CString AppToken) { strAppToken = AppToken; };
private:
	// ��ȡ��ǰTimeStamp
	CString GetTimestamp();
	// ��ȡ����IP
	CString GetLocalIP();
	// ����ǩ��
	void CalculateSign();
};

class BaseClient
{
public:
	BaseClient(void);
	~BaseClient(void);

public:
	virtual CString GetAppTokenFromGateServer(ConfigInfo info) = 0;
	virtual CString GetIPFromGateServer(ConfigInfo info) = 0;
	virtual CString GetIPFromPureDNS(CString url) = 0;
};