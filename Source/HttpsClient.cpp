#include "HttpsClient.h"
#include <WS2tcpip.h>
#include <WinInet.h>
#include <afxinet.h>
#include <stdio.h>
#include "Utils.h"

#include "curl/curl.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"../../Source/curl/lib/libcurl.lib")
#pragma comment(lib,"../../Source/curl/lib/libeay32.lib")
#pragma comment(lib,"../../Source/curl/lib/ssleay32.lib")

HttpsClient::HttpsClient() 
{
}
HttpsClient::~HttpsClient()
{

}

struct MemoryStruct {
	char *memory;
	size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

CString HttpsClient::GetAppTokenFromGateServer(ConfigInfo info)
{
	CString ret = _T("");
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	struct MemoryStruct memoryChunk;
	memoryChunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	memoryChunk.size = 0;    /* no data at this point */

	curl = curl_easy_init();
	if (curl) {
		struct curl_slist *chunk = NULL;
		//chunk = curl_slist_append(chunk, "Host: gateway.fastgoapi.com");
		//chunk = curl_slist_append(chunk, "ip-address: 10.71.12.113");
		//chunk = curl_slist_append(chunk, "Content-Type: application/x-www-form-urlencoded");
		CString postheader = info.GetPostHeader();
		chunk = curl_slist_append(chunk, UnicodeToAnsi(postheader).GetBuffer(0));
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		CString posturl = info.GetPostUrl();
		curl_easy_setopt(curl, CURLOPT_URL, UnicodeToAnsi(posturl).GetBuffer(0));
		//curl_easy_setopt(curl, CURLOPT_URL, "https://gateway.fastgoapi.com/auths/appToken");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		CString postdata = info.GetPostData();
		CStringA utf8data = UnicodeToAnsi(postdata);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, utf8data.GetBuffer(0));
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "app_id=1eghw8c67ekmbtu1givepkwynusixhl7&platform=wap&timestamp=1506678767000&signature=4a4c22dda0650ec2280c3f2af581d600");

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&memoryChunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		else {
			printf("%lu bytes retrieved\n", (long)memoryChunk.size);
			ret = memoryChunk.memory;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		free(memoryChunk.memory);
	}

	curl_global_cleanup();
	return ret;
}
CString HttpsClient::GetIPFromGateServer(ConfigInfo info)
{
	CString ret = _T("");
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	struct MemoryStruct memoryChunk;
	memoryChunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	memoryChunk.size = 0;    /* no data at this point */

	curl = curl_easy_init();
	if (curl) {
		struct curl_slist *chunk = NULL;
		CString postheader = info.GetPostHeader();
		chunk = curl_slist_append(chunk, UnicodeToAnsi(postheader).GetBuffer(0));
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		CString posturl = info.GetPostUrl();
		curl_easy_setopt(curl, CURLOPT_URL, UnicodeToAnsi(posturl).GetBuffer(0));
		//curl_easy_setopt(curl, CURLOPT_URL, "https://gateway.fastgoapi.com/auths/appToken");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		CString postdata = _T("");
		postdata.Format(_T("app_token=%s"), info.GetAppToken());
		CStringA utf8data = UnicodeToAnsi(postdata);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, utf8data.GetBuffer(0));
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "app_id=1eghw8c67ekmbtu1givepkwynusixhl7&platform=wap&timestamp=1506678767000&signature=4a4c22dda0650ec2280c3f2af581d600");

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&memoryChunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		else {
			printf("%lu bytes retrieved\n", (long)memoryChunk.size);
			ret = memoryChunk.memory;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		free(memoryChunk.memory);
	}

	curl_global_cleanup();
	return ret;
}

CString HttpsClient::GetVersionsFromGateServer(ConfigInfo info)
{
	CString ret = _T("");
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	struct MemoryStruct memoryChunk;
	memoryChunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	memoryChunk.size = 0;    /* no data at this point */

	curl = curl_easy_init();
	if (curl) {
		struct curl_slist *chunk = NULL;
		CString postheader = info.GetPostHeader();
		chunk = curl_slist_append(chunk, UnicodeToAnsi(postheader).GetBuffer(0));
		res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

		CString posturl = info.GetPostUrl();
		curl_easy_setopt(curl, CURLOPT_URL, UnicodeToAnsi(posturl).GetBuffer(0));
		//curl_easy_setopt(curl, CURLOPT_URL, "https://gateway.fastgoapi.com/auths/appToken");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		CString postdata = info.GetPostData();
		CStringA utf8data = UnicodeToAnsi(postdata);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, utf8data.GetBuffer(0));
		//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "app_id=1eghw8c67ekmbtu1givepkwynusixhl7&platform=wap&timestamp=1506678767000&signature=4a4c22dda0650ec2280c3f2af581d600");

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

		/* we pass our 'chunk' struct to the callback function */
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&memoryChunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		else {
			printf("%lu bytes retrieved\n", (long)memoryChunk.size);
			ret = memoryChunk.memory;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		free(memoryChunk.memory);
	}

	curl_global_cleanup();
	return ret;
}

CString HttpsClient::GetIPFromPureDNS(CString url)
{
	CString ret = _T("");
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	struct MemoryStruct memoryChunk;
	memoryChunk.memory = (char*)malloc(1);  /* will be grown as needed by the realloc above */
	memoryChunk.size = 0;    /* no data at this point */

	curl = curl_easy_init();
	if (curl) {
		struct curl_slist *chunk = NULL;

		curl_easy_setopt(curl, CURLOPT_VERBOSE, true);
		std::string sUrl = UnicodeToAnsi(url).GetBuffer(0);
		curl_easy_setopt(curl, CURLOPT_URL, sUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, "2");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);

		std::string sslCert = "./cert.pem";
		std::string sslKey = "./key.pem";

		curl_easy_setopt(curl, CURLOPT_SSLCERT, sslCert.c_str());
		curl_easy_setopt(curl, CURLOPT_SSLKEY, sslKey.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&memoryChunk);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		}
		else {
			printf("%lu bytes retrieved\n", (long)memoryChunk.size);
			ret = memoryChunk.memory;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
		free(memoryChunk.memory);
	}

	curl_global_cleanup();
	return ret;
}

void HttpsClient::Clear()
{

}