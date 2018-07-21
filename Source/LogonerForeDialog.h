#pragma once
#include "Base/ForeDialogBase.h"
#include "Base/ImgButton.h"
#include "Explorer.h"
#include "afxwin.h"
#include "LoadSettingThread.h"
#include "GetIPAddrThread.h"
#include "Base/CusStatic.h"
#include "MD5.h"
#include "config.h"

#include <map>

#ifdef FK_IP_MODE

struct SVersionUpdateInfo {
public:
	CString version_num;
	CString version_num_required;
	CString src;
	bool is_update;
	bool is_remaind;

	SVersionUpdateInfo() { Clear(); }
	void Clear() {
		version_num = version_num_required = src = _T("");
		is_update = is_remaind = false;
	}
	void DebugFill(){
		version_num = _T("3.0.4");
		version_num_required = _T("3.0.0");
		src = _T("http://a04phone.bdhsea.com/downloader/A04/comm/D88_Launcher.zip");
		is_update = true;
		is_remaind = true;
	}
};
#endif

class CLogonerForeDialog : public CForeDialogBase {
	DECLARE_DYNAMIC(CLogonerForeDialog)

public:
	CLogonerForeDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLogonerForeDialog();

	enum { IDD = IDD_LOGONER_FOREDIALOG };

	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void PaintBkgnd(CDC* pDC);

	void OpenDomain(CString & addr);

	CString GetCurrTimeString();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();

	afx_msg LRESULT OnFinishCheckSpeed(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInitSettingFinished(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgShowLastUI(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT MsgTap1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgTap2(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgTap3(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT MsgTap4(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButExit();
	afx_msg void OnBnClickedBtnMinisize();
	afx_msg void OnBnClickedButMatch();
	afx_msg void OnBnClickedButOpen1();
	afx_msg void OnBnClickedButOpen2();
	afx_msg void OnBnClickedButOpen3();

	afx_msg void OnBnClickedButCleanUP();
	afx_msg void OnBnClickedButModifyDNS();
	afx_msg void OnBnClickedButDownFlash();
	afx_msg void OnBnClickedButDownChrome();
	afx_msg void OnBnClickedButExecAll();

	// 设置通用按钮样式1
	void SetButStyle1(ImgButton& but, CString text, int x, int y, int w, int h);
	// 设置通用按钮样式2
	void SetButStyle2(ImgButton& but, CString text, int x, int y, int w, int h);

	// 设置进度条进度
	void SetProcess(CusStatic& c, int process);
	void DownloadFlashFiles();
	//void DoDownloadFile(void *); 
	// 设置提示图标样式
	void SetIconStyle(CusStatic& c, int mode=0, BOOL show=false);

	void Match();   

	void MatchUI(int mode);

	UINT GetProcessID(int Speed, int& s);
#ifdef FK_IP_MODE
private:
	// 检查一个文件是否存在
	BOOL	IsFileExist(const CString& csFile);
	CString GetBrowserFileName();
	// 通知自定义IE浏览器打开一个指定IP的域名
	void	OpenDomain(CString & addr, CString & IP);
	// 从GateServer获取一个AppToken
	CString	GetAppTokenFromGateServer(bool bDebug = false);
	// 从GateServer获取并更新IP域名映射表
	bool	UpdateDomainIPListFromGateServer(CString strAppToken, bool bDebug = false);
	// 从GateServer获取更新列表
	bool	GetUpdateListFromGateServer(CString strAppToken, bool bDebug = false);
	// 从PureDNS获取IP信息
	CString GetIPByDomain(CString addr, bool bDebug = false);
	// 检查是否需要进行自更新
	bool	NeedToUpdateSelf();
	// 自我更新
	void	UpdateSelf();
	SVersionUpdateInfo m_info;
#endif
private:
	ImgButton btnClose;
	ImgButton btnMinisize;

	CusStatic imgTap1;
	CusStatic imgTap2;
	CusStatic imgTap3;
	CusStatic imgTap4;

	CusStatic imgIcon1;
	CusStatic imgIcon2;
	CusStatic imgIcon3;
	CusStatic imgIcon4;

	CStatic   imgText1;       // 文字标签
	CStatic   imgLine1;       // 分隔线

	CStatic   imgMatch21;       // 匹配中显示背景
	CusStatic imgMatch22;       // 匹配中显示进度

	ImgButton btnMatch;      // 匹配按钮

	ImgButton btnCleanUP;    // 清理缓存按钮
	ImgButton btnModifyDNS;  // 修改DNS按钮
	ImgButton btnDownFlash;  // 下载Flash Player 按钮
	ImgButton btnDownChrome; // 下载Google浏览器按钮
	ImgButton btnOpen1;      // 登录按钮1
	ImgButton btnOpen2;      // 登录按钮1
	ImgButton btnOpen3;      // 登录按钮1

	//ImgButton btnExecAll;    // 执行全部按钮

	CusStatic imgProcess1;   // 进度条1
	CusStatic imgProcess2;   // 进度条1
	CusStatic imgProcess3;   // 进度条1

	int  matchProcess;      // 匹配进度

	int  cleanUPProcess;    // 清理进度
	int  modifyDNSProcess;  // 修改DNS进度
	//int  downFlashProcess;  // 下载Flash进度
	int  downChromeProcess; // 下载Chrome进度

	CString chromePath; // Chrome 安装文件保存地址

	CGetIPAddrThread* pThreadIpAddr;
	CLoadSettingThread* threadLoad[3];
	CString	bestDomainID[3];
	public:
		int  downFlashProcess;  // 下载Flash进度
		std::vector<CString> falsePath; // Flash 插件下载保存地址
};
