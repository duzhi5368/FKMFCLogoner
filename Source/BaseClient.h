#pragma once

#include "afxwin.h"
/************************************************************************/
/*各GET和POST类型的基类，用于规定各派生类必须实现的接口                                                                      */
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
	// 初始化结构数据
	void Init();
	void InitVersion();
	
	const CString GetPostUrl() { return strPostUrl; };
	const CString GetPostData() { return strPostData; };
	const CString GetPostHeader() { return strPostHeader; };
	const CString GetAppToken() { return strAppToken; };
	void SetAppToken(CString AppToken) { strAppToken = AppToken; };
private:
	// 获取当前TimeStamp
	CString GetTimestamp();
	// 获取本地IP
	CString GetLocalIP();
	// 计算签名
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