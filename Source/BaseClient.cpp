#include "stdafx.h"
#include "BaseClient.h"
#include <WS2tcpip.h>
#include "Setting.h"
#include "MD5.h"


// ��ȡ��ǰTimeStamp
CString ConfigInfo::GetTimestamp()
{
	CTime time = CTime::GetCurrentTime();
	CString strTimeStamp = _T("");
	strTimeStamp.Format(L"%lld", time.GetTime() * 1000);
	return strTimeStamp;
}

// ��ȡ����IP
CString ConfigInfo::GetLocalIP()
{
	CString strIP = _T("");

	WORD wVersion = MAKEWORD(2, 2);
	WSADATA WSAData;
	WSAStartup(wVersion, &WSAData);

	addrinfo hints;
	addrinfo *res = NULL;
	addrinfo *cur = NULL;
	char szHostName[128] = "";
	if (::gethostname(szHostName, sizeof(szHostName)))
	{
		return _T("");
	}
	struct sockaddr_in *addr = NULL;
	char ipAddr[16] = "";
	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(szHostName, NULL, &hints, &res) != 0) {
		return _T("");
	}
	for (cur = res; cur != NULL; cur = cur->ai_next) {
		addr = (struct sockaddr_in *)cur->ai_addr;
		sprintf(ipAddr, "%d.%d.%d.%d",
			(*addr).sin_addr.S_un.S_un_b.s_b1,
			(*addr).sin_addr.S_un.S_un_b.s_b2,
			(*addr).sin_addr.S_un.S_un_b.s_b3,
			(*addr).sin_addr.S_un.S_un_b.s_b4);
		strIP = ipAddr;
	}
	freeaddrinfo(res);
	WSACleanup();

	return strIP;
}

// ��ʼ���ṹ����
void ConfigInfo::Init() {
	// ����ǩ��
	CalculateSign();

	// ����Url
	if (bIsUseSSL)
		strPostUrl.Format(_T("https://%s/%s"), strGateUrl, strAppTokenSuffix);
	else
		strPostUrl.Format(_T("http://%s/%s"), strGateUrl, strAppTokenSuffix);
	// ����header
	strPostHeader.Format(_T("Host:%s\r\nip-address:%s\r\nContent-Type: application/x-www-form-urlencoded"),
		strGateUrl, strLocalIP);
	// ����data
	strPostData.Format(_T("app_id=%s&platform=%s&timestamp=%s&signature=%s"),
		strAppID, strPlatform, strTimestamp, strSign);
}

// ��ʼ����ȡ�汾��ʱ�Ľṹ
void ConfigInfo::InitVersion() 
{
	strSignSrc.Format(_T("app_token=%s&version=%s"), strAppToken, CSetting::Instance().GetVersion());
	CT2A ascii(strSignSrc);
	strSign = CString(MD5(ascii.m_psz).toStr().c_str());

	// ����Url
	if (bIsUseSSL)
		strPostUrl.Format(_T("https://%s/%s"), strGateUrl, strAppTokenSuffix);
	else
		strPostUrl.Format(_T("http://%s/%s"), strGateUrl, strAppTokenSuffix);

	// ����header
	strPostHeader.Format(_T("Host:%s\r\nContent-Type: application/x-www-form-urlencoded"),
		strGateUrl);

	// ����data
	strPostData.Format(_T("app_key=%s&app_token=%s&signature=%s&version=%s"),
		strAppKey, strAppToken, strSign, CSetting::Instance().GetVersion());
}

// ����ǩ��
void ConfigInfo::CalculateSign() {
	strSignSrc.Format(_T("app_id=%s&platform=%s&timestamp=%s&app_key=%s"),
		strAppID, strPlatform, strTimestamp, strAppKey);
	//strSignSrc = "app_id=3m7jcugnkkomnwsplm9ydokx33ch9v99&platform=wap&timestamp=1506509809000&app_key=6w3mgqan5gwqdoxqvk7hpij5ed8gs2a8";
	CT2A ascii(strSignSrc);
	strSign = CString(MD5(ascii.m_psz).toStr().c_str());
}

BaseClient::BaseClient(void) {

}
BaseClient::~BaseClient(void) {

}