#include "stdafx.h"
#include "HttpClient.h"

HttpClient::HttpClient() 
	: pConnection(NULL)
	, pHttpFile(NULL)
{
	// 创建会话，并设置会话选项
	pSession = new CInternetSession(_T("session"), 0, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_DONT_CACHE);
	pSession->SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 20);
	pSession->SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
	pSession->SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
}

HttpClient::~HttpClient() 
{
	if (NULL != pConnection)
	{
		pConnection->Close();
		delete pConnection;
	}

	if (NULL != pSession)
	{
		pSession->Close();
		delete pSession;
	}
}


CString HttpClient::GetAppTokenFromGateServer(ConfigInfo info)
{
	CString result = _T("");
	// STEP1: 连接服务器
	try
	{
		CString str = info.strGateUrl;
		pConnection = pSession->GetHttpConnection(str.GetBuffer(0), 0, info.nPort, NULL, NULL);
	}
	catch (CInternetException* e)
	{
		if (pConnection)
		{
			pConnection->Close();
			pConnection = NULL;
		}

		e->m_dwContext;
		e->Delete();

		return _T("");
	}

	// 连接发送
	try
	{
		CString strPostSiffix;
		strPostSiffix.Format(_T("/%s"), info.strAppTokenSuffix);
		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strPostSiffix.GetBuffer(0));

		// 请求头
		pHttpFile->AddRequestHeaders(info.GetPostHeader());

		CString strPostData = info.GetPostData();
		CT2A asciiPostData(strPostData);
		pHttpFile->SendRequest(NULL, 0, (LPVOID)asciiPostData.m_psz, strlen(asciiPostData.m_psz));

		DWORD nReturnStatus;
		pHttpFile->QueryInfoStatusCode(nReturnStatus);

		DWORD dwStatus;
		DWORD dwBuffLen = sizeof(dwStatus);
		BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);
		if (bSuccess && HTTP_STATUS_OK == nReturnStatus)
		{
			char szContent[10240] = { 0 };
			memset(szContent, 0, sizeof(szContent));
			if (pHttpFile->Read(szContent, sizeof(szContent) - 1) <= 0)
			{
				Clear();
				return _T("");
			}
			result = szContent;
		}
		else
		{
			Clear();
			return _T("");
		}
	}
	catch (CInternetException * e)
	{
		e->m_dwContext;
		e->Delete();
		Clear();
		return _T("");
	}

	Clear();
	return result;
}

CString HttpClient::GetIPFromGateServer(ConfigInfo info)
{
	CString result = _T("");
	// STEP1: 连接服务器
	try
	{
		CString str = info.strGateUrl;
		pConnection = pSession->GetHttpConnection(str.GetBuffer(0), 0, info.nPort, NULL, NULL);
	}
	catch (CInternetException* e)
	{
		if (pConnection)
		{
			pConnection->Close();
			pConnection = NULL;
		}

		e->m_dwContext;
		e->Delete();

		return _T("");
	}

	// 连接发送
	try
	{
		CString strPostSiffix;
		strPostSiffix.Format(_T("/%s"), info.strAppTokenSuffix);
		pHttpFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strPostSiffix.GetBuffer(0));

		// 请求头
		pHttpFile->AddRequestHeaders(info.GetPostHeader());

		CString strPostData = _T("");
		strPostData.Format(_T("app_token=%s"), info.GetAppToken());
		CT2A asciiPostData(strPostData);
		pHttpFile->SendRequest(NULL, 0, (LPVOID)asciiPostData.m_psz, strlen(asciiPostData.m_psz));

		DWORD nReturnStatus;
		pHttpFile->QueryInfoStatusCode(nReturnStatus);

		DWORD dwStatus;
		DWORD dwBuffLen = sizeof(dwStatus);
		BOOL bSuccess = pHttpFile->QueryInfo(HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);
		if (bSuccess && HTTP_STATUS_OK == nReturnStatus)
		{
			char szContent[10240] = { 0 };
			memset(szContent, 0, sizeof(szContent));
			if (pHttpFile->Read(szContent, sizeof(szContent) - 1) <= 0)
			{
				Clear();
				return _T("");
			}
			result = szContent;
		}
		else
		{
			Clear();
			return _T("");
		}
	}
	catch (CInternetException * e)
	{
		e->m_dwContext;
		e->Delete();
		Clear();
		return _T("");
	}

	Clear();
	return result;
}

CString HttpClient::GetVersionsFromGateServer(ConfigInfo info)
{
	return _T("");
}

CString HttpClient::GetIPFromPureDNS(CString url)
{
	/*
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://example.com");

		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);


		res = curl_easy_perform(curl);

		if (res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));

		curl_easy_cleanup(curl);
	}
	*/
	return _T("");
}

void HttpClient::Clear() 
{
	if (NULL != pHttpFile)
	{
		pHttpFile->Close();
		pHttpFile = NULL;
	}

	if (NULL != pConnection)
	{
		pConnection->Close();
		pConnection = NULL;
	}

	if (NULL != pSession)
	{
		pSession->Close();
		pSession = NULL;
	}
}