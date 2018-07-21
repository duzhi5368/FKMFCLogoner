#include "stdafx.h"
#include "Logoner.h"
#include "LogonerForeDialog.h"
#include "afxdialogex.h"
#include "config.h"
#include "Setting.h"
#include "Utils.h"
#include <thread>
#include "Resource.h"

#ifdef FK_IP_MODE
#include "Json/json.h"
#include "BaseClient.h"
#include "HttpClient.h"
#include "HttpsClient.h"
#endif

#define TIME_EVENT_ID_CHECK	1001

IMPLEMENT_DYNAMIC(CLogonerForeDialog, CForeDialogBase)

static void GetTempFileList(std::vector<CString> &urls, std::vector<CString> &tmpPaths)
{
	if (urls.size() <= 0)
		return;

	for (int i = 0; i < urls.size(); i++)
	{
		CString  cURL = urls[i];
		CString ext = cURL.Right(cURL.GetLength() - cURL.ReverseFind('.') - 1);

		TCHAR tmpPath[MAX_PATH];
		DWORD len = GetTempPath(MAX_PATH, tmpPath);
		if (len > MAX_PATH || len == 0) {
			return;
		}

		::PathStripPath(cURL.GetBuffer());
		cURL.ReleaseBuffer();

		CString cTmpPath;
		cTmpPath.Format(L"%s%s", tmpPath, cURL);
		if (::PathFileExists(cTmpPath))
		{
			::DeleteFile(cTmpPath);
		}
		
		tmpPaths.push_back(cTmpPath);
	}

}
 
static void DoDownloadFile(void *pthis)
{
	//std::vector<CString> tempFilePaths;
	CLogonerForeDialog *obj = (CLogonerForeDialog *)pthis;
	std::vector<CString> urls = CSetting::Instance().GetFlashPlayerURLList();
	GetTempFileList(urls, obj->falsePath);

	//static double p = 0.0;
	static double preProgress = 0.0;
	static double curProgress = 0.0;
	static double curStage = 0.0;
	static double stageCount = 0.0;
	static double stage = (double)100 / ((double)obj->falsePath.size() * 100);

	int m = obj->falsePath.size();
	for (int i = 0; i <obj->falsePath.size(); i++)
	{

		preProgress = 0.0;
		curStage = 0.0;
		
		DownFile(urls[i], obj->falsePath[i], [](void* o, long p, long m ) {
			CLogonerForeDialog* obj = (CLogonerForeDialog*)o;
			if (p < 0)
			{
				obj->downFlashProcess = p;
				obj->PostMessageW(MSG_TAP4, 0, 0);
			}
			else
			{
				//obj->downFlashProcess +=  stage * ((double)p / m) * 100.0;
				 
				if (preProgress == 0.0)
					curStage = preProgress = stage * ((double)p / m);
				else
				{
					curStage = (stage * ((double)p / m) - preProgress);
					if (curStage < 0.0) curStage = 0.0;
					preProgress += curStage;
					curProgress += curStage;
					stageCount += curStage;
				}

				//int tcp = obj->downFlashProcess;
				obj->downFlashProcess = curProgress * 100.0;
				obj->PostMessageW(MSG_TAP4, 0, 0);
				if (stageCount * 100 > 1)
				{
					Sleep(20);
					stageCount = 0.0;
				}
			}
		}, pthis);

		if (obj->downFlashProcess < 0)
			return;

		int p2 =(int)((double)stage * (i + 1) * 100);
		p2 = p2 >= 100 ? 99 : p2;
		if (p2 != obj->downFlashProcess) {
			obj->downFlashProcess = p2;
			obj->PostMessageW(MSG_TAP4, 0, 0);
		}
	}

	 
	::Sleep(2000);
	obj->downFlashProcess = 100;
	obj->PostMessageW(MSG_TAP4, 0, 0);
	
}

CLogonerForeDialog::CLogonerForeDialog(CWnd* pParent /*=NULL*/)
	: CForeDialogBase(CLogonerForeDialog::IDD, pParent){

	matchProcess = 0;

	bestDomainID[0] = _T("");
	bestDomainID[1] = _T("");
	bestDomainID[2] = _T("");
	//falsePath = L"";
	chromePath = L"";
	//bit1 = new Gdiplus::Bitmap(IDB_BMP_EXIT1);
}

CLogonerForeDialog::~CLogonerForeDialog(){
}


void CLogonerForeDialog::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_CLOSE,      btnClose);
	DDX_Control(pDX, IDC_BTN_MINISIZE,   btnMinisize);
	DDX_Control(pDX, IDC_BTN_MATCH,      btnMatch);
	DDX_Control(pDX, IDC_BTN_CLEANUP,    btnCleanUP);
	DDX_Control(pDX, IDC_BTN_MODIFYDNS,  btnModifyDNS);
	DDX_Control(pDX, IDC_BTN_DOWNFLASH,  btnDownFlash);
	DDX_Control(pDX, IDC_BTN_DOWNCHROME, btnDownChrome);
	//DDX_Control(pDX, IDC_BTN_EXECALL,    btnExecAll);
	DDX_Control(pDX, IDC_BTN_OPEN1,      btnOpen1);
	DDX_Control(pDX, IDC_BTN_OPEN2,      btnOpen2);
	DDX_Control(pDX, IDC_BTN_OPEN3,      btnOpen3);
	DDX_Control(pDX, IDC_IMG_PROGRESSBAR1, imgProcess1);
	DDX_Control(pDX, IDC_IMG_PROGRESSBAR2, imgProcess2);
	DDX_Control(pDX, IDC_IMG_PROGRESSBAR3, imgProcess3);
	DDX_Control(pDX, IDC_IMG_TEXT1, imgText1);
	DDX_Control(pDX, IDC_IMG_LINE1, imgLine1);
	DDX_Control(pDX, IDC_IMG_BUTMATCHBG21, imgMatch21);
	DDX_Control(pDX, IDC_IMG_BUTMATCHBG22, imgMatch22);
	DDX_Control(pDX, IDC_IMG_TAP1, imgTap1);
	DDX_Control(pDX, IDC_IMG_TAP2, imgTap2);
	DDX_Control(pDX, IDC_IMG_TAP3, imgTap3);
	DDX_Control(pDX, IDC_IMG_TAP4, imgTap4);
	DDX_Control(pDX, IDC_IMG_TAPICON1, imgIcon1);
	DDX_Control(pDX, IDC_IMG_TAPICON2, imgIcon2);
	DDX_Control(pDX, IDC_IMG_TAPICON3, imgIcon3);
	DDX_Control(pDX, IDC_IMG_TAPICON4, imgIcon4);
}

void CLogonerForeDialog::SetButStyle1(ImgButton& but, CString text, int x, int y, int w, int h) {
	but.SetDrawStyle(true);
	but.LoadBitmaps(IDB_BMP_BUTBG1, IDB_BMP_BUTBG1, IDB_BMP_BUTBG1, IDB_BMP_BUTBG1);
	but.SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER | SWP_FRAMECHANGED);
	but.SetTextOffsetY(-5);
	but.SetWindowText(text);
	but.SetTextColor(_TEXT_COLOR1);
}

void CLogonerForeDialog::SetButStyle2(ImgButton& but, CString text, int x, int y, int w, int h) {
	but.SetDrawStyle(true);
	but.LoadBitmaps(IDB_BMP_BUTBG2, IDB_BMP_BUTBG2, IDB_BMP_BUTBG2, IDB_BMP_BUTBG2);
	but.SetWindowPos(NULL, x, y, w, h, SWP_NOZORDER | SWP_FRAMECHANGED);
	but.SetTextOffsetY(-5);
	but.SetWindowText(text);
	but.SetTextColor(_TEXT_COLOR2);
}

void CLogonerForeDialog::SetProcess(CusStatic& c, int process) {
	UINT w = c.GetMaxWidth();
	if (w <= 0) { return; }
	w = w * process / 100;
	c.SetWidth(w);
}

void CLogonerForeDialog::SetIconStyle(CusStatic& c, int mode, BOOL show) {
	CRect rect;
	c.GetWindowRect(&rect);
	ScreenToClient(&rect);
	if (mode == 1) {
		c.SetBitmap(IDB_BMP_ERROR);
		c.SetWindowPos(NULL, 799, rect.top, 0, 0,SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}else {
		c.SetBitmap(IDB_BMP_SUCCESS);
		c.SetWindowPos(NULL, 837, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
	

	c.ShowWindow(show ? SW_SHOW : SW_HIDE);
}


BOOL CLogonerForeDialog::OnInitDialog(){
	CForeDialogBase::OnInitDialog();

	btnClose.SetDrawStyle(true);
	btnClose.LoadBitmaps(IDB_BMP_EXIT1, IDB_BMP_EXIT2, IDB_BMP_EXIT2, IDB_BMP_EXIT1);
	btnClose.SetWindowPos(NULL, 1020, 8, 28, 28, SWP_NOZORDER | SWP_FRAMECHANGED);

	btnMinisize.SetDrawStyle(true);
	btnMinisize.LoadBitmaps(IDB_BMP_MINISIZE1, IDB_BMP_MINISIZE2, IDB_BMP_MINISIZE2, IDB_BMP_MINISIZE1);
	btnMinisize.SetWindowPos(NULL, 990, 8, 28, 28, SWP_NOZORDER | SWP_FRAMECHANGED);

	btnOpen1.LoadBitmaps(IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG);
	btnOpen1.SetWindowPos(NULL, 532, 120, 82, 30, SWP_NOZORDER | SWP_FRAMECHANGED);
	btnOpen1.SetWindowText(L"");

	btnOpen2.LoadBitmaps(IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG);
	btnOpen2.SetWindowPos(NULL, 532, 157, 82, 30, SWP_NOZORDER | SWP_FRAMECHANGED);
	btnOpen2.SetWindowText(L"");

	btnOpen3.LoadBitmaps(IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG, IDB_BMP_BUTLOGINBG);
	btnOpen3.SetWindowPos(NULL, 532, 194, 82, 30, SWP_NOZORDER | SWP_FRAMECHANGED);
	btnOpen3.SetWindowText(L"");

	imgText1.SetWindowPos(NULL, 28, 110, 451, 131, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgLine1.SetWindowPos(NULL, 630, 125, 16, 97, SWP_NOZORDER | SWP_FRAMECHANGED);

	imgMatch21.SetWindowPos(NULL, 662, 150, 159, 50, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgMatch22.SetWindowPos(NULL, 663, 151, 80, 46, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgMatch22.SetMaxWidth(156);
	imgMatch22.SetText(L"0", 7, 4, 36, 36);
	imgMatch22.SetTextSize(28);

	btnMatch.SetDrawStyle(true);
	btnMatch.LoadBitmaps(IDB_BMP_BUTMATCHBG11, IDB_BMP_BUTMATCHBG12, IDB_BMP_BUTMATCHBG12, IDB_BMP_BUTMATCHBG11);
	btnMatch.SetWindowPos(NULL, 663, 150, 156, 51, SWP_NOZORDER | SWP_FRAMECHANGED);
	btnMatch.SetTextOffsetY(-5);
	btnMatch.SetWindowText(L"开始匹配");
	btnMatch.SetBigFont();
	btnMatch.SetTextColor(_TEXT_COLOR3);

	imgProcess1.SetWindowPos(NULL, 102, 124, 305, 17, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgProcess1.SetMaxWidth(305);

	imgProcess2.SetWindowPos(NULL, 102, 163, 305, 17, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgProcess2.SetMaxWidth(305);

	imgProcess3.SetWindowPos(NULL, 102, 200, 305, 17, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgProcess3.SetMaxWidth(305);

	SetButStyle1(btnCleanUP,    L"一键完成", 910, 339, 102, 35);
	imgIcon1.ShowWindow(SW_HIDE);
	imgIcon1.SetWindowPos(NULL, 837, 335, 38, 41, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap1.ShowWindow(SW_HIDE);
	imgTap1.SetWindowPos(NULL, 828, 338, 186, 35, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap1.SetText(L"0%", 10, 4, 28, 27);
	imgTap1.SetTextSize(16);
	imgTap1.SetTextColor(_TEXT_COLOR3);

	SetButStyle1(btnModifyDNS,  L"一键完成", 910, 428, 102, 35);
	imgIcon2.ShowWindow(SW_HIDE);
	imgIcon2.SetWindowPos(NULL, 837, 423, 38, 41, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap2.ShowWindow(SW_HIDE);
	imgTap2.SetWindowPos(NULL, 828, 427, 186, 35, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap2.SetText(L"0%", 10, 4, 28, 27);
	imgTap2.SetTextSize(16);
	imgTap2.SetTextColor(_TEXT_COLOR3);

	SetButStyle1(btnDownChrome, L"下载并安装", 910, 517, 102, 35);
	imgIcon3.ShowWindow(SW_HIDE);
	imgIcon3.SetWindowPos(NULL, 837, 512, 38, 41, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap3.ShowWindow(SW_HIDE);
	imgTap3.SetWindowPos(NULL, 828, 516, 186, 35, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap3.SetText(L"0%", 10, 4, 28, 27);
	imgTap3.SetTextSize(16);
	imgTap3.SetTextColor(_TEXT_COLOR3);

	SetButStyle1(btnDownFlash,  L"一键完成下载及安装", 820, 605, 192, 35);
	
	btnDownFlash.SetDrawStyle(true);
	btnDownFlash.LoadBitmaps(IDB_BMP_TAPBG4, IDB_BMP_TAPBG5, IDB_BMP_TAPBG5, IDB_BMP_TAPBG4);
	btnDownFlash.SetWindowPos(NULL, 820, 605, 192, 36, SWP_NOZORDER | SWP_FRAMECHANGED);
	btnDownFlash.SetWindowText(L"");

	imgIcon4.ShowWindow(SW_HIDE);
	imgIcon4.SetWindowPos(NULL, 837, 600, 38, 41, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap4.ShowWindow(SW_HIDE);
	imgTap4.SetWindowPos(NULL, 828, 604, 186, 35, SWP_NOZORDER | SWP_FRAMECHANGED);
	imgTap4.SetText(L"0%", 10, 4, 28, 27);
	imgTap4.SetTextSize(16);
	imgTap4.SetTextColor(_TEXT_COLOR3);

	

	//SetButStyle1(btnExecAll,    L"一键完成以上 4 项", 900, 700, 135, 35);

	MatchUI(0);

	UpdateData(FALSE);

	pThreadIpAddr = (CGetIPAddrThread *)AfxBeginThread(RUNTIME_CLASS(CGetIPAddrThread));
	pThreadIpAddr->m_pMainWnd = this;

#ifdef _DEBUG
		threadLoad[0] = (CLoadSettingThread *)AfxBeginThread(RUNTIME_CLASS(CLoadSettingThread));
		threadLoad[0]->m_pMainWnd = this;
		threadLoad[0]->PostThreadMessageW(MSG_LOAD_SETTING, 0, 0);
#else
	for (int i = 0; i < 3; i++) {
		threadLoad[i] = (CLoadSettingThread *)AfxBeginThread(RUNTIME_CLASS(CLoadSettingThread));
		threadLoad[i]->m_pMainWnd = this;
		threadLoad[i]->PostThreadMessageW(MSG_LOAD_SETTING, i, 0);
	}
#endif

	if (CheckChrome()) {
		downChromeProcess = 200;
		PostMessageW(MSG_TAP3, 0, 0);
	}
	

#ifdef FK_IP_MODE
	// 尝试更新
	if (NeedToUpdateSelf()) {
		UpdateSelf();
		::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
		return TRUE;
	}
#endif

	// 强制提前执行匹配
	Match();

	return TRUE;
}
#ifdef FK_IP_MODE
bool CLogonerForeDialog::NeedToUpdateSelf() 
{

	// 首先尝试连接服务器获取AppToken
	CString strAppToken = GetAppTokenFromGateServer(false);
	if (strAppToken.IsEmpty()) {
		return false;
	}

	// 根据AppToken去获取更新列表信息
	bool bGetUpdateList = GetUpdateListFromGateServer(strAppToken, false);
	if (!bGetUpdateList){
		return false;
	}

	// DEBUG 填充
	//m_info.DebugFill();
	
	// 逻辑检查是否需要更新
	if (m_info.src.IsEmpty()) { 
		return false; 
	}
	if ((!m_info.is_update) && (!m_info.is_remaind)) { 
		return false; 
	 }
	
	// 强制更新
	if (m_info.is_update) {
		return true;
	}

	// 选择更新
	CString strInfo;
	strInfo.Format(_T("是否升级新版本至 %s ?"), m_info.version_num);
	//
	if (MessageBox(strInfo, _T("提示"),
		MB_YESNO | MB_ICONQUESTION | MB_SYSTEMMODAL) == IDYES){
		return true;
	}

	return false;


}
// 从GateServer获取更新列表
bool CLogonerForeDialog::GetUpdateListFromGateServer(CString strAppToken, bool bDebug)
{
	m_info.Clear();

	ConfigInfo info;
	info.strAppID = APP_ID;
	info.strAppKey = APP_KEY;
	info.strGateUrl = RELEASE_GATE_URL;
	info.strAppTokenSuffix = UPDATE_VERSION_SUFFIX;
	info.bIsUseSSL = true;
	info.SetAppToken(strAppToken);

	if (bDebug) {
		info.strGateUrl = DEBUG_GATE_URL;
		info.bIsUseSSL = false;
	}


	// 计算内部数据
	info.InitVersion();

	CString strResponse = _T("");
	if (bDebug) {
		HttpClient client;
		strResponse = client.GetVersionsFromGateServer(info);
	}
	else {
		HttpsClient client;
		strResponse = client.GetVersionsFromGateServer(info);
	}

	if (strResponse.IsEmpty())
	{
		return false;
	}

	Json::Reader	reader;
	Json::Value		value;
	CT2A asciiResponse(strResponse);

	if (!reader.parse(asciiResponse.m_psz, value, false))
	{
		return false;
	}
	Json::Value data = value["data"];
	if (!data.isObject())
	{
		return false;
	}
	Json::Value version_num = data["version_num"];
	if (!version_num.isString())
	{
		return false;
	}
	Json::Value version_num_required = data["version_num_required"];
	if (!version_num_required.isString())
	{
		return false;
	}
	Json::Value src = data["src"];
	if (!src.isString())
	{
		return false;
	}
	Json::Value is_update = data["is_update"];
	if (!is_update.isBool())
	{
		return false;
	}
	Json::Value is_remaind = data["is_remaind"];
	if (!is_remaind.isBool())
	{
		return false;
	}

	m_info.version_num = CString(version_num.asCString());
	m_info.version_num_required = CString(version_num_required.asCString());
	m_info.src = CString(src.asCString());
	m_info.is_update = is_update.asBool(); 
	m_info.is_remaind = is_remaind.asBool();
	// 保护
	m_info.src.Replace(_T("https://"), _T("http://"));

	return true;
}

// 从PureDNS获取IP信息
CString CLogonerForeDialog::GetIPByDomain(CString addr, bool bDebug)
{
	//addr = _T("www.google.com");

	CString baseUrl = _T("http://10.71.12.16:85/query.php");
	//if (!bDebug) {
		baseUrl = _T("https://218.103.65.147/query.php");
	//}

	CString config_md5_key = _T("8c2fe1a180322e66c6a4c68735f55740dba4edc5");

	// step1:
	// 域名加密
	bool strip_padding = false;
	CT2CA pszConvertedAnsiString(addr.GetBuffer(0));
	std::string input(pszConvertedAnsiString);
	std::string dmb;
	//static std::string decoded;

	dmb.resize(Base64::EncodedLength(input));
	if (!Base64::Encode(input.c_str(), input.size(), &dmb[0], dmb.size())) {
		return _T("");
	}

	if (strip_padding) 
		Base64::StripPadding(&dmb);
	/*
	decoded.resize(Base64::DecodedLength(encoded));
	if (!Base64::Decode(encoded.c_str(), encoded.size(), &decoded[0], decoded.size())) {
		return _T("");
	}

	if (input != decoded) {
		return _T("");
	}
	*/

	// step2:
	// url 编码
	string dmu = url_encode(dmb);
	CString sdmu(dmu.c_str());

	// step3:
	// 生成签名
	CT2A ascii(config_md5_key);
	std::string tmp = MD5(ascii.m_psz).toStr();
	tmp = tmp.substr(5);
	std::string dmk = MD5(dmb + tmp).toStr();
	CString sdmk(dmk.c_str());

	// 生成Url
	CString url;
	url.Format(_T("%s?v=%s&k=%s"), baseUrl, sdmu, sdmk);

	// 获取response
	CString strResponse;
	//if (bDebug) {
	//	HttpClient client;
	//	strResponse = client.GetIPFromPureDNS(url);
	//}
	//else {
		HttpsClient client;
		strResponse = client.GetIPFromPureDNS(url);
	//}

	// 解析response
	if (strResponse.IsEmpty())
	{
		return _T("");
	}

	Json::Reader	reader;
	Json::Value		value;
	CT2A asciiResponse(strResponse);

	if (!reader.parse(asciiResponse.m_psz, value, false))
	{
		return _T("");
	}
	Json::Value code_num = value["code"];
	if (!code_num.isNumeric())
	{
		return _T("");
	}
	int nResult = code_num.asInt();
	if (nResult != 0) {
		return _T("");
	}
	Json::Value ip_string = value["data"];
	if (!ip_string.isString())
	{
		return _T("");
	}
	CString sIP(ip_string.asCString());
	return sIP;
}

void CLogonerForeDialog::UpdateSelf()
{
	DWORD dwWrite(0);
	HANDLE hFile;
	TCHAR  szTempPath[MAX_PATH];

	GetTempPath(MAX_PATH, szTempPath);
	(_tcsrchr(szTempPath, '\\'))[1] = 0;
	_tcscat(szTempPath, _T("XUpdater.exe"));

	hFile = CreateFile(szTempPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	// 查找资源文件中、加载资源到内存、得到资源大小
	HRSRC	hrsc = FindResource(NULL, MAKEINTRESOURCE((WORD)IDR_UPDATE), _T("EXE"));
	HGLOBAL hG = LoadResource(NULL, hrsc);
	DWORD	dwSize = SizeofResource(NULL, hrsc);

	// 写入文件
	WriteFile(hFile, hG, dwSize, &dwWrite, NULL);
	CloseHandle(hFile);

	// 开始进行自我更新
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR szXML[_MAX_PATH];

	GetModuleFileName(NULL, szPath, MAX_PATH);
	GetTempPath(MAX_PATH, szXML);
	(_tcsrchr(szXML, '\\'))[1] = 0;
	_tcscat(szXML, _T("XUpdater.exe"));

	//m_info.src = _T("http://a04phone.bdhsea.com/downloader/A04/comm/D88_Launcher.zip");
	//m_info.src = _T("http://a04image.csqcy.com/static/A04P/_default/__static/__download/ZL_LauncherInstaller.exe");

	_stprintf(szXML + lstrlen(szXML),
		_T(" <wnd>%s</wnd><app>%s</app><files><file>%s</file></files>"),
		INSTALL_FILE_NAME, szPath, m_info.src.GetBuffer());

	::WinExec(UnicodeToAnsi(szXML), SW_SHOW);
}

#endif

BEGIN_MESSAGE_MAP(CLogonerForeDialog, CForeDialogBase)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CLogonerForeDialog::OnBnClickedButExit)
	ON_BN_CLICKED(IDC_BTN_MINISIZE, &CLogonerForeDialog::OnBnClickedBtnMinisize)
	ON_BN_CLICKED(IDC_BTN_MATCH, &CLogonerForeDialog::OnBnClickedButMatch)

	ON_BN_CLICKED(IDC_BTN_OPEN1, &CLogonerForeDialog::OnBnClickedButOpen1)
	ON_BN_CLICKED(IDC_BTN_OPEN2, &CLogonerForeDialog::OnBnClickedButOpen2)
	ON_BN_CLICKED(IDC_BTN_OPEN3, &CLogonerForeDialog::OnBnClickedButOpen3)

	ON_BN_CLICKED(IDC_BTN_CLEANUP, &OnBnClickedButCleanUP)
	ON_BN_CLICKED(IDC_BTN_MODIFYDNS, &OnBnClickedButModifyDNS)
	ON_BN_CLICKED(IDC_BTN_DOWNFLASH, &OnBnClickedButDownFlash)
	ON_BN_CLICKED(IDC_BTN_DOWNCHROME, &OnBnClickedButDownChrome)
	//ON_BN_CLICKED(IDC_BTN_EXECALL, &OnBnClickedButExecAll)

	ON_MESSAGE(MSG_CHECK_FINISH, OnFinishCheckSpeed)
	ON_MESSAGE(MSG_INIT_SETTING, OnInitSettingFinished)
	ON_MESSAGE(MSG_SHOW_LASTUI, OnMsgShowLastUI)

	ON_MESSAGE(MSG_TAP1, MsgTap1)
	ON_MESSAGE(MSG_TAP2, MsgTap2)
	ON_MESSAGE(MSG_TAP3, MsgTap3)
	ON_MESSAGE(MSG_TAP4, MsgTap4)

	//ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CLogonerForeDialog::OnBnClickedButExit(){
	CForeDialogBase::OnCancel();
}

void CLogonerForeDialog::OnBnClickedBtnMinisize() {
	ShowWindow2(SW_MINIMIZE);
}

void CLogonerForeDialog::OnBnClickedButMatch(){
	Match();
}

void CLogonerForeDialog::OnBnClickedButOpen1() {
	CString addr = CSetting::Instance().GetDomainByDomainID(bestDomainID[0]);
#ifdef FK_IP_MODE
	// 整个查询入口
	CString strAppToken = GetAppTokenFromGateServer(false);
	if (strAppToken.IsEmpty()) {
		OpenDomain(addr);
		return;
	}
	if (!UpdateDomainIPListFromGateServer(strAppToken, false)) {
		OpenDomain(addr);
		return;
	}
	CString newAddr = CSetting::Instance().GetRandomDomain(0);
	if (newAddr.IsEmpty()) {
		OpenDomain(addr);
		return;
	}
	CString ip = GetIPByDomain(newAddr);
	if (ip.IsEmpty()) {
		OpenDomain(newAddr);
		return;
	}

	OpenDomain(newAddr, ip);
#else
	OpenDomain(addr);
#endif
}

void CLogonerForeDialog::OnBnClickedButOpen2() {
	CString addr = CSetting::Instance().GetDomainByDomainID(bestDomainID[1]);
#ifdef FK_IP_MODE
	// 整个查询入口
	CString strAppToken = GetAppTokenFromGateServer(false);
	if (strAppToken.IsEmpty()) {
		OpenDomain(addr);
		return;
	}
	if (!UpdateDomainIPListFromGateServer(strAppToken, false)) {
		OpenDomain(addr);
		return;
	}
	CString newAddr = CSetting::Instance().GetRandomDomain(1);
	if (newAddr.IsEmpty()) {
		OpenDomain(addr);
		return;
	}
	CString ip = GetIPByDomain(newAddr);
	if (ip.IsEmpty()) {
		OpenDomain(newAddr);
		return;
	}

	OpenDomain(newAddr, ip);
#else
	OpenDomain(addr);
#endif
}

void CLogonerForeDialog::OnBnClickedButOpen3() {
	CString addr = CSetting::Instance().GetDomainByDomainID(bestDomainID[2]);
#ifdef FK_IP_MODE
	// 整个查询入口
	CString strAppToken = GetAppTokenFromGateServer(false);
	if (strAppToken.IsEmpty()) {
		OpenDomain(addr);
		return;
	}
	if (!UpdateDomainIPListFromGateServer(strAppToken, false)) {
		OpenDomain(addr);
		return;
	}
	CString newAddr = CSetting::Instance().GetRandomDomain(2);
	if (newAddr.IsEmpty()) {
		OpenDomain(addr);
		return;
	}
	CString ip = GetIPByDomain(newAddr);
	if (ip.IsEmpty()) {
		OpenDomain(newAddr);
		return;
	}

	OpenDomain(newAddr, ip);
#else
	OpenDomain(addr);
#endif
}


void CLogonerForeDialog::OnBnClickedButCleanUP() {
	if (imgIcon1.IsWindowVisible()) {
		return;
	}
	btnCleanUP.ShowWindow(SW_HIDE);
	imgTap1.ShowWindow(SW_SHOW);
	std::thread t(Clearing, [](void* o, long p, long m) {
		CLogonerForeDialog* obj = (CLogonerForeDialog*)o;
		obj->cleanUPProcess = p * 100 / m;
		obj->PostMessageW(MSG_TAP1, 0, 0);
	}, this);
	t.detach();
}

void CLogonerForeDialog::OnBnClickedButModifyDNS() {
	if (imgIcon2.IsWindowVisible()) {
		return;
	}
	btnModifyDNS.ShowWindow(SW_HIDE);
	imgTap2.ShowWindow(SW_SHOW);
	std::thread t(UPDNS, [](void* o, long p, long m) {
		CLogonerForeDialog* obj = (CLogonerForeDialog*)o;
		obj->modifyDNSProcess = p * 100 / m;
		obj->PostMessageW(MSG_TAP2, 0, 0);
	}, this);
	t.detach();
}
 
void CLogonerForeDialog::DownloadFlashFiles()
{
	std::thread t(DoDownloadFile, this);
	t.detach();
}

void CLogonerForeDialog::OnBnClickedButDownFlash() {
	if (imgIcon4.IsWindowVisible()) {
		return;
	}

	btnDownFlash.ShowWindow(SW_HIDE);
	imgTap4.ShowWindow(SW_SHOW);

	std::vector<CString> urls = CSetting::Instance().GetFlashPlayerURLList();
	if (urls.size() <= 0) {
		downFlashProcess = -1;
		PostMessageW(MSG_TAP4, 0, 0);
		return;
	}

	DownloadFlashFiles();
	 
}
void CLogonerForeDialog::OnBnClickedButDownChrome() {
	if (imgIcon3.IsWindowVisible()) {
		return;
	}

	btnDownChrome.ShowWindow(SW_HIDE);
	imgTap3.ShowWindow(SW_SHOW);

	CString url = CSetting::Instance().GetChromeURL();
	if (url == "") {
		downFlashProcess = -1;
		PostMessageW(MSG_TAP3, 0, 0);
		return;
	}

	CString ext = url.Right(url.GetLength() - url.ReverseFind('.') - 1);

	TCHAR tmpPath[MAX_PATH];
	DWORD len = GetTempPath(MAX_PATH, tmpPath);
	if (len > MAX_PATH || len == 0) {
		return;
	}

	chromePath.Format(L"%sChromeInstall.%s", tmpPath, ext);

	std::thread t(DownFile, url, chromePath, [](void* o, long p, long m) {
		CLogonerForeDialog* obj = (CLogonerForeDialog*)o;
		int p2 = p <= 0 ? p : (int)((double)p / m * 100);
		if (p2 != obj->downChromeProcess) {
			obj->downChromeProcess = p2;
			obj->PostMessageW(MSG_TAP3, 0, 0);
		}
	}, this);

	t.detach();
}
void CLogonerForeDialog::OnBnClickedButExecAll() {
	if (btnCleanUP.IsWindowVisible()) {
		OnBnClickedButCleanUP();
	}
	if (btnModifyDNS.IsWindowVisible()) {
		OnBnClickedButModifyDNS();
	}
	if (btnDownFlash.IsWindowVisible()) {
		OnBnClickedButDownFlash();
	}
	if(btnDownChrome.IsWindowVisible()){
		OnBnClickedButDownChrome();
	}
}

LRESULT CLogonerForeDialog::MsgTap1(WPARAM wParam, LPARAM lParam) {
	if (cleanUPProcess >= 100) {
		SetButStyle1(btnCleanUP, L"已完成", 910, 339, 102, 35);
		btnCleanUP.ShowWindow(SW_SHOW);
		imgTap1.ShowWindow(SW_HIDE);
		SetIconStyle(imgIcon1, 0, TRUE);
	} else {
		if (cleanUPProcess < 0) {
			SetIconStyle(imgIcon1, 1, TRUE);
			imgTap1.ShowWindow(SW_HIDE);
			btnCleanUP.ShowWindow(SW_SHOW);
			SetButStyle2(btnCleanUP, L"清理失败，请联系客服", 872, 339, 140, 35);
			return 0;
		}
		CString t;
		t.Format(L"%d%%", cleanUPProcess);
		imgTap1.SetText(t);
	}
	return 0;
}

LRESULT CLogonerForeDialog::MsgTap2(WPARAM wParam, LPARAM lParam) {
	if (modifyDNSProcess >= 100) {
		btnModifyDNS.ShowWindow(SW_SHOW);
		imgTap2.ShowWindow(SW_HIDE);

		SetIconStyle(imgIcon2, 0, TRUE);
		SetButStyle1(btnModifyDNS, L"已完成", 910, 428, 102, 35);
	}else {
		if (modifyDNSProcess < 0) {
			SetIconStyle(imgIcon2, 1, TRUE);
			imgTap2.ShowWindow(SW_HIDE);
			btnModifyDNS.ShowWindow(SW_SHOW);
			SetButStyle2(btnModifyDNS, L"修改失败，请联系客服", 872, 428, 140, 35);
			return 0;
		}
		CString t;
		t.Format(L"%d%%", modifyDNSProcess);
		imgTap2.SetText(t);
	}
	return 0;
}

LRESULT CLogonerForeDialog::MsgTap3(WPARAM wParam, LPARAM lParam) {
	if (downChromeProcess >= 100) {
		if (downChromeProcess > 100) {
			btnDownChrome.ShowWindow(SW_SHOW);
			imgTap3.ShowWindow(SW_HIDE);
			SetIconStyle(imgIcon3, 0, TRUE);
			SetButStyle1(btnDownChrome, L"已完成", 910, 517, 102, 35);
		}
		else {
			std::thread t([&]() {
				// 安装程序
				Execute(L"", TRUE, chromePath);
				if (CheckChrome()) {
					downChromeProcess = 200;
				}
				else {
					downChromeProcess = -100;
				}
				PostMessageW(MSG_TAP3, 0, 0);
			});

			t.detach();

		}
	}
	else {
		if (downChromeProcess < 0) {
			SetIconStyle(imgIcon3, 1, TRUE);
			imgTap3.ShowWindow(SW_HIDE);
			btnDownChrome.ShowWindow(SW_SHOW);
			SetButStyle2(btnDownChrome, downChromeProcess == -100 ? L"安装失败，请联系客服" : L"下载失败，请联系客服", 882, 517, 140, 35);
			return 0;
		}
		CString t;
		t.Format(L"%d%%", (int)(downChromeProcess * 0.99));
		imgTap3.SetText(t);
	}
	return 0;
}

LRESULT CLogonerForeDialog::MsgTap4(WPARAM wParam, LPARAM lParam) {
	if (downFlashProcess >= 100) {
		if (downFlashProcess > 100) {
			btnDownFlash.ShowWindow(SW_SHOW);
			imgTap4.ShowWindow(SW_HIDE);

			SetIconStyle(imgIcon4, 0, TRUE);
			SetButStyle1(btnDownFlash, L"已完成", 910, 605, 102, 35);
		}
		else{
			/*imgTap4.SetBitmap(IDB_BMP_TAPBG5);
			imgTap4.SetWindowPos(NULL, 821, 605, 192, 36, SWP_NOZORDER | SWP_FRAMECHANGED);
			imgTap4.SetText(L"", 0, 0, 192, 36);*/

			// 安装程序
			std::thread t([&]() {
				//downFlashProcess = Execute(L"", TRUE, falsePath) ? 200 : -100;
				for (int i = 0; i < falsePath.size(); i++)
				{
					CString path = falsePath[i];
					if (::PathFileExists(path))
					{
						CString strName = path;
						::PathStripPath(strName.GetBuffer());

						if (strName.Find(_T("ax"), 0) >= 0)
							strName = _T("IE Flash插件");
						else
						if (strName.Find(_T("pp"), 0) >= 0)
							strName = _T("Chromium Flash插件");
						else
							strName = _T("Firefox Flash插件");

						downFlashProcess =  Execute2(path, TRUE, path,_T("正在安装:") + strName) ? 200 : -100;
					}
				}

				//downFlashProcess = 200;
				PostMessageW(MSG_TAP4, 0, 0);
			});

			t.detach();

			//downFlashProcess = Execute(L"", TRUE, falsePath) ? 200 : -100;
			//PostMessageW(MSG_TAP4, 0, 0);
		}
	}
	else {
		if (downFlashProcess < 0) {
			SetIconStyle(imgIcon4, 1, TRUE);
			imgTap4.ShowWindow(SW_HIDE);
			btnDownFlash.ShowWindow(SW_SHOW);
			SetButStyle2(btnDownFlash, downFlashProcess == -100 ? L"安装失败，请联系客服":L"下载失败，请联系客服", 882, 605, 140, 35);
			return 0;
		}
		CString t;
		t.Format(L"%d%%", downFlashProcess);
		imgTap4.SetText(t);
	}
	return 0;
}


#ifdef FK_IP_MODE
// 检查一个文件是否存在
BOOL CLogonerForeDialog::IsFileExist(const CString& csFile)
{
	DWORD dwAttrib = GetFileAttributes(csFile);
	return INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

CString CLogonerForeDialog::GetBrowserFileName()
{
	const CString DEPEND_EXE_NAME = _T("浏览器.exe");

	CString url;
	TCHAR pBuf[MAX_PATH + 1];					// 存放路径的变量
	GetModuleFileName(NULL, pBuf, MAX_PATH);
	(_tcsrchr(pBuf, _T('\\')))[1] = 0;			// 删除文件名，只获得路径
	CString filePath;
	filePath.Format(_T("%s%s"), pBuf, DEPEND_EXE_NAME);

	CString myDataPath, fdPath;
	myDataPath.Format(_T("%s\\*.exe"), pBuf);
	CString strTmp;

	CFileFind find;
	BOOL bf = find.FindFile(myDataPath);
	while (bf)
	{
		bf = find.FindNextFile();
		if (!find.IsDots())
		{
			fdPath = find.GetFilePath();
			if (!find.IsDirectory())
			{
				//如果是文件,判断是否是目标文件
				if (fdPath.Find(DEPEND_EXE_NAME) >= 0) {
					CString ret = find.GetFileName();
					find.Close();
					return ret;
				}
			}
		}
	}
	find.Close();

	return _T("");
}

// 通知自定义IE浏览器打开一个指定IP的域名
void CLogonerForeDialog::OpenDomain(CString & addr, CString & IP) {

	if (addr != _T("")) 
	{		
		CString strBrowserExe = GetBrowserFileName();

		CString url;
		TCHAR pBuf[MAX_PATH + 1];					// 存放路径的变量
		GetModuleFileName(NULL, pBuf, MAX_PATH);
		(_tcsrchr(pBuf, _T('\\')))[1] = 0;			// 删除文件名，只获得路径
		CString filePath;
		filePath.Format(_T("%s%s"), pBuf, strBrowserExe);

		if (!strBrowserExe.IsEmpty())
		{
			SHELLEXECUTEINFO shExecInfo = { 0 };
			shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
			shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
			shExecInfo.hwnd = NULL;
			shExecInfo.lpVerb = _T("open");
			shExecInfo.lpFile = filePath;

			CString domain = addr;
			CString header = IP.Left(4);
			CString tureIP = IP;

			if (header.CompareNoCase(_T("http")) != 0) {
				// 强制IP携带https
				IP.Format(_T("http://%s"), IP);
			}
			
			/*
			if (tureIP.CompareNoCase(_T("255.255.255.255")) == 0) {
				// IP为 https://255.255.255.255
				url.Format(_T("%s https://%s?source=1&info=%s&time=%s %s %s"),
					strBrowserExe, domain, CSetting::Instance().GetVersion(), GetCurrTimeString(), _T("255.255.255.255"), domain);
			}
			else if (tureIP.CompareNoCase(_T("255.255.255.255")) == 0) {
				// IP为 255.255.255.255
				url.Format(_T("%s https://%s?source=1&info=%s&time=%s %s %s"),
					strBrowserExe, domain, CSetting::Instance().GetVersion(), GetCurrTimeString(), _T("255.255.255.255"), domain);
			}
			else if (tureIP.CompareNoCase(_T("0.0.0.0")) == 0)
			{
				// IP为0.0.0.0， 其实是HTTP访问
				url.Format(_T("%s http://%s?source=1&info=%s&time=%s %s %s"),
					strBrowserExe, domain, CSetting::Instance().GetVersion(), GetCurrTimeString(), _T("255.255.255.255"), domain);
			}
			else {
				// 带HTTPS的IP访问
				url.Format(_T("%s %s?source=1&info=%s&time=%s %s %s"),
					strBrowserExe, IP, CSetting::Instance().GetVersion(), GetCurrTimeString(), tureIP, domain);
			}
			*/

			if (tureIP.CompareNoCase(_T("255.255.255.255")) == 0) {
				// IP为 https://255.255.255.255
				url.Format(_T("%s https://%s %s %s"),
					strBrowserExe, domain, _T("255.255.255.255"), domain);
			}
			else if (tureIP.CompareNoCase(_T("255.255.255.255")) == 0) {
				// IP为 255.255.255.255
				url.Format(_T("%s https://%s %s %s"),
					strBrowserExe, domain,  _T("255.255.255.255"), domain);
			}
			else if (tureIP.CompareNoCase(_T("0.0.0.0")) == 0)
			{
				// IP为0.0.0.0， 其实是HTTP访问
				url.Format(_T("%s http://%s %s %s"),
					strBrowserExe, domain,  _T("255.255.255.255"), domain);
			}
			else {
				// 带HTTPS的IP访问
				url.Format(_T("%s %s %s %s"),
					strBrowserExe, IP, tureIP, domain);
			}


			TCHAR szParams[MAX_PATH];
			wcscpy_s(szParams, MAX_PATH, url);
			shExecInfo.lpParameters = szParams;// LPWSTR(url.GetBuffer());
			shExecInfo.nShow = SW_SHOWNORMAL;
			shExecInfo.hInstApp = NULL;
			ShellExecuteEx(&shExecInfo);
		}
		else
		{
			CString domain = addr;
			CString header = addr.Left(4);
			if (header.CompareNoCase(_T("http")) != 0) {
				domain.Format(_T("http://%s"), addr);
			}
			url.Format(_T("%s?source=1&info=%s&time=%s"),
				domain, CSetting::Instance().GetVersion(), GetCurrTimeString());
			ShellExecute(NULL, _T("open"), (LPWSTR)url.GetBuffer(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}
// 从GateServer获取一个AppToken
CString	CLogonerForeDialog::GetAppTokenFromGateServer(bool bDebug)
{
	ConfigInfo info;
	info.strAppID = APP_ID;
	info.strAppKey = APP_KEY;
	info.strGateUrl = RELEASE_GATE_URL;
	info.strAppTokenSuffix = APP_TOKEN_SUFFIX;
	info.bIsUseSSL = true;

	if (bDebug) {
		info.strGateUrl = DEBUG_GATE_URL;
		info.bIsUseSSL = false;
	}

	// 计算内部数据
	info.Init();

	CString strResponse = _T("");
	if (bDebug) {
		HttpClient client;
		strResponse = client.GetAppTokenFromGateServer(info);
	}
	else {
		HttpsClient client;
		strResponse = client.GetAppTokenFromGateServer(info);
	}

	if (strResponse.IsEmpty())
	{
		return _T("");
	}
	Json::Reader	reader;
	Json::Value		value;
	CT2A asciiResponse(strResponse);
	if (!reader.parse(asciiResponse.m_psz, value, false))
	{
		return _T("");
	}

	Json::Value domains = value["data"];
	if (!domains.isObject())
	{
		return _T("");
	}

	Json::Value appToken = domains["appToken"];
	if (!appToken.isString())
	{
		return _T("");
	}

	return AnsiToUnicode(appToken.asCString());
}
// 从GateServer获取并更新IP域名映射表
bool CLogonerForeDialog::UpdateDomainIPListFromGateServer(CString strAppToken, bool bDebug)
{
	ConfigInfo info;
	info.strGateUrl = RELEASE_GATE_URL;
	info.strAppTokenSuffix = GET_CDN_SUFFIX;
	info.bIsUseSSL = true;
	info.SetAppToken(strAppToken);

	if (bDebug) {
		info.strGateUrl = DEBUG_GATE_URL;
		info.bIsUseSSL = false;
	}

	// 计算内部数据
	info.Init();

	CString strResponse = _T("");
	if (bDebug) {
		HttpClient client;
		strResponse = client.GetIPFromGateServer(info);
	}
	else {
		HttpsClient client;
		strResponse = client.GetIPFromGateServer(info);
	}

	if (strResponse.IsEmpty())
	{
		return false;
	}

	Json::Reader	reader;
	Json::Value		value;
	CT2A asciiResponse(strResponse);
	if (!reader.parse(asciiResponse.m_psz, value, false))
	{
		return false;
	}
	Json::Value data = value["data"];
	if (data.isNull())
	{
		return false;
	}
	Json::Value domains = data["dns"];
	if (!domains.isArray())
	{
		return false;
	}
	int nCount = domains.size();
	for (int i = 0; i < nCount; i++)
	{
		Json::Value element = domains[i];
		CString strDomain = CString(element["domain"].asCString());
		bool bIsHttp = false;
		if (strDomain.Find(_T("http://")) != -1)
		{
			bIsHttp = true;
		}
		strDomain.Replace(_T("http://m."), _T("www."));
		strDomain.Replace(_T("https://m."), _T("www."));
		strDomain.Replace(_T("http://"), _T(""));
		strDomain.Replace(_T("https://"), _T(""));
		if (bIsHttp) {
			CSetting::Instance().AddDomainIPs(strDomain, _T("0.0.0.0"));	// http标示
			continue;
		}
		else
		{
			Json::Value iplist = element["ip_list"];
			if (!iplist.isArray()) {
				continue;
			}
			int nIPCount = iplist.size();
			for (int j = 0; j < nIPCount; ++j) {
				CString strIP = CString(iplist[j].asCString());
				CSetting::Instance().AddDomainIPs(strDomain, strIP);
			}
		}
	}
	return true;
}
#endif

void CLogonerForeDialog::OpenDomain(CString & addr) {
	CString domain = addr;
	CString header = addr.Left(4);
	if (header.CompareNoCase(_T("http")) != 0) {
		domain.Format(_T("http://%s"), addr);
	}

	if (addr != _T("")) {
		CString url;

		url.Format(_T("%s?source=1&info=%s&time=%s"),
			domain, CSetting::Instance().GetVersion(), GetCurrTimeString());

		ShellExecute(NULL, _T("open"), (LPWSTR)url.GetBuffer(), NULL, NULL, SW_SHOWNORMAL);
	}
}

CString CLogonerForeDialog::GetCurrTimeString() {
	SYSTEMTIME st;
	GetLocalTime(&st);

	long lTime = (st.wHour * 3600 + st.wMinute * 60 + st.wSecond) * 1000 + st.wMilliseconds;

	char szTime[32];
	memset(szTime, 0, sizeof(szTime));

	time_t tt = time(NULL);

	unsigned __int64 uTime = tt * 1000 + st.wMilliseconds;

	_ui64toa(uTime, szTime, 10);

	return AnsiToUnicode(szTime);
}

void CLogonerForeDialog::OnOK(){
	/*if (GetFocus() == &m_urlEdit){
		UpdateData(TRUE);
	}*/
}

LRESULT CLogonerForeDialog::OnFinishCheckSpeed(WPARAM wParam, LPARAM lParam) {
	if (CSetting::Instance().IsCheckFinished()) {
		CSetting& setting = CSetting::Instance();
		bestDomainID[0] = setting.GetBestDomainID(0);
		bestDomainID[1] = setting.GetBestDomainID(1);
		bestDomainID[2] = setting.GetBestDomainID(2);
	}

	return 0;
}

LRESULT CLogonerForeDialog::OnInitSettingFinished(WPARAM wParam, LPARAM lParam){
	CSetting::Instance().CreateCheckSpeedThreads(this);

	return 0;
}

UINT CLogonerForeDialog::GetProcessID(int Speed, int& s) {
	int fastSpeed = CSetting::Instance().GetFastSpeed();
	int normanSpeed = CSetting::Instance().GetNormalSpeed();

	if (Speed >= 0 && Speed <= fastSpeed){
		s = 100;
		return IDB_BMP_PROGRESSBAR1;
	}else if (Speed > fastSpeed && Speed <= normanSpeed) {
		s = 70;
		return IDB_BMP_PROGRESSBAR2;
	} else if (Speed > normanSpeed && Speed < MAX_SPEED) {
		s = 40;
		return IDB_BMP_PROGRESSBAR3;
	}

	s = 40;
	return IDB_BMP_PROGRESSBAR3;
}

LRESULT CLogonerForeDialog::OnMsgShowLastUI(WPARAM wParam, LPARAM lParam) {
	if(!btnMatch.IsWindowVisible()){
		btnMatch.SetWindowText(L"重新匹配");

		MatchUI(2);

		//int k, s;
		//k = GetProcessID(CSetting::Instance().GetDomainSpeed(bestDomainID[0]), s);
		//imgProcess1.SetBitmap(k);
		//SetProcess(imgProcess1, s);
		imgProcess1.SetBitmap(IDB_BMP_PROGRESSBAR1);
		SetProcess(imgProcess1, 100);

		//k = GetProcessID(CSetting::Instance().GetDomainSpeed(bestDomainID[1]), s);
		//imgProcess2.SetBitmap(k);
		//SetProcess(imgProcess2, s - 10);
		imgProcess2.SetBitmap(IDB_BMP_PROGRESSBAR2);
		SetProcess(imgProcess2, 80);

		//k = GetProcessID(CSetting::Instance().GetDomainSpeed(bestDomainID[2]), s);
		//imgProcess3.SetBitmap(k);
		//SetProcess(imgProcess3, s - 20);
		imgProcess3.SetBitmap(IDB_BMP_PROGRESSBAR3);
		SetProcess(imgProcess3, 60);
		

		pThreadIpAddr->PostThreadMessage(MSG_GET_IPADDRESS, 0, 0);
		CSetting::Instance().SetToSendResult(true);
	}
	
	Invalidate();
	return 0;
}

BOOL CLogonerForeDialog::OnEraseBkgnd(CDC* pDC) { 
	return TRUE;
}

void CLogonerForeDialog::PaintBkgnd(CDC* pDC) {
}

void CLogonerForeDialog::MatchUI(int mode) {
	switch (mode) {
	case 0:
		btnMatch.ShowWindow(SW_SHOW);
		imgText1.ShowWindow(SW_SHOW);

		imgMatch21.ShowWindow(SW_HIDE);
		imgMatch22.ShowWindow(SW_HIDE);
		imgLine1.ShowWindow(SW_HIDE);
		imgProcess1.ShowWindow(SW_HIDE);
		imgProcess2.ShowWindow(SW_HIDE);
		imgProcess3.ShowWindow(SW_HIDE);
		btnOpen1.ShowWindow(SW_HIDE);
		btnOpen2.ShowWindow(SW_HIDE);
		btnOpen3.ShowWindow(SW_HIDE);
		break;
	case 1:
		imgText1.ShowWindow(SW_SHOW);

		imgMatch21.ShowWindow(SW_SHOW);
		imgMatch22.ShowWindow(SW_SHOW);

		btnMatch.ShowWindow(SW_HIDE);
		imgLine1.ShowWindow(SW_HIDE);
		imgProcess1.ShowWindow(SW_HIDE);
		imgProcess2.ShowWindow(SW_HIDE);
		imgProcess3.ShowWindow(SW_HIDE);
		btnOpen1.ShowWindow(SW_HIDE);
		btnOpen2.ShowWindow(SW_HIDE);
		btnOpen3.ShowWindow(SW_HIDE);
		break;
	case 2:
		btnMatch.ShowWindow(SW_SHOW);
		imgLine1.ShowWindow(SW_SHOW);
		imgProcess1.ShowWindow(SW_SHOW);
		imgProcess2.ShowWindow(SW_SHOW);
		imgProcess3.ShowWindow(SW_SHOW);
		btnOpen1.ShowWindow(SW_SHOW);
		btnOpen2.ShowWindow(SW_SHOW);
		btnOpen3.ShowWindow(SW_SHOW);

		imgText1.ShowWindow(SW_HIDE);
		imgMatch21.ShowWindow(SW_HIDE);
		imgMatch22.ShowWindow(SW_HIDE);
		break;
	default: return;
	}
	
}

void CLogonerForeDialog::Match() {
	SetTimer(TIME_EVENT_ID_CHECK, 10, NULL);

	MatchUI(1);

	SetProcess(imgMatch22, 0);
	matchProcess = 5;
	CSetting::Instance().ReadyToStart();

	//Invalidate();
}

void CLogonerForeDialog::OnDestroy(){
	CDialog::OnDestroy();

	CSetting::Instance().DestroyCheckSpeedThreads();

	for (int i = 0; i < 3; i++){
		if (!IsBadReadPtr(threadLoad[i], sizeof(CWinThread *))){
			threadLoad[i]->PostThreadMessage(WM_QUIT, 0, 0);
			Sleep(2);
		}
	}
}

void CLogonerForeDialog::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == TIME_EVENT_ID_CHECK) {
		CRect rect(300, 200, 600, 300);

		do {
			CSetting::Instance().StartCheckSpeed();

			// 到达30之前一直用10这个速度
			if (matchProcess < 30) {
				matchProcess += 2;
				break;
			}

			KillTimer(nIDEvent);

			if (!CSetting::Instance().IsInitialize()) {
				if (matchProcess < 60) {
					matchProcess++;
				}
				SetTimer(TIME_EVENT_ID_CHECK, 50, NULL);
				break;
			}

			if (!CSetting::Instance().IsCheckFinished()) {
				if (matchProcess < 90) {
					matchProcess++;
				}
				SetTimer(TIME_EVENT_ID_CHECK, 20, NULL);
				break;
			}

			if (matchProcess < 99) {
				matchProcess++;
				SetTimer(TIME_EVENT_ID_CHECK, 10, NULL);
			} else {
				PostMessage(MSG_SHOW_LASTUI);
			}

		} while (0);

		SetProcess(imgMatch22, matchProcess);
		CString t;
		t.Format(L"%d", matchProcess);
		imgMatch22.SetText(t);
	}

	CDialog::OnTimer(nIDEvent);
}
