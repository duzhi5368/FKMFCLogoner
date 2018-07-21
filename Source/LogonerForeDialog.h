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

	// ����ͨ�ð�ť��ʽ1
	void SetButStyle1(ImgButton& but, CString text, int x, int y, int w, int h);
	// ����ͨ�ð�ť��ʽ2
	void SetButStyle2(ImgButton& but, CString text, int x, int y, int w, int h);

	// ���ý���������
	void SetProcess(CusStatic& c, int process);
	void DownloadFlashFiles();
	//void DoDownloadFile(void *); 
	// ������ʾͼ����ʽ
	void SetIconStyle(CusStatic& c, int mode=0, BOOL show=false);

	void Match();   

	void MatchUI(int mode);

	UINT GetProcessID(int Speed, int& s);
#ifdef FK_IP_MODE
private:
	// ���һ���ļ��Ƿ����
	BOOL	IsFileExist(const CString& csFile);
	CString GetBrowserFileName();
	// ֪ͨ�Զ���IE�������һ��ָ��IP������
	void	OpenDomain(CString & addr, CString & IP);
	// ��GateServer��ȡһ��AppToken
	CString	GetAppTokenFromGateServer(bool bDebug = false);
	// ��GateServer��ȡ������IP����ӳ���
	bool	UpdateDomainIPListFromGateServer(CString strAppToken, bool bDebug = false);
	// ��GateServer��ȡ�����б�
	bool	GetUpdateListFromGateServer(CString strAppToken, bool bDebug = false);
	// ��PureDNS��ȡIP��Ϣ
	CString GetIPByDomain(CString addr, bool bDebug = false);
	// ����Ƿ���Ҫ�����Ը���
	bool	NeedToUpdateSelf();
	// ���Ҹ���
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

	CStatic   imgText1;       // ���ֱ�ǩ
	CStatic   imgLine1;       // �ָ���

	CStatic   imgMatch21;       // ƥ������ʾ����
	CusStatic imgMatch22;       // ƥ������ʾ����

	ImgButton btnMatch;      // ƥ�䰴ť

	ImgButton btnCleanUP;    // �����水ť
	ImgButton btnModifyDNS;  // �޸�DNS��ť
	ImgButton btnDownFlash;  // ����Flash Player ��ť
	ImgButton btnDownChrome; // ����Google�������ť
	ImgButton btnOpen1;      // ��¼��ť1
	ImgButton btnOpen2;      // ��¼��ť1
	ImgButton btnOpen3;      // ��¼��ť1

	//ImgButton btnExecAll;    // ִ��ȫ����ť

	CusStatic imgProcess1;   // ������1
	CusStatic imgProcess2;   // ������1
	CusStatic imgProcess3;   // ������1

	int  matchProcess;      // ƥ�����

	int  cleanUPProcess;    // �������
	int  modifyDNSProcess;  // �޸�DNS����
	//int  downFlashProcess;  // ����Flash����
	int  downChromeProcess; // ����Chrome����

	CString chromePath; // Chrome ��װ�ļ������ַ

	CGetIPAddrThread* pThreadIpAddr;
	CLoadSettingThread* threadLoad[3];
	CString	bestDomainID[3];
	public:
		int  downFlashProcess;  // ����Flash����
		std::vector<CString> falsePath; // Flash ������ر����ַ
};
