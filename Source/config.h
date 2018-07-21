#ifndef ___CONFIG_H_INCL___
#define ___CONFIG_H_INCL___

enum
{
	MSG_CHECK_SPEED = WM_USER + 1001,
	MSG_CHECK_FINISH,
	MSG_INIT_SETTING,
	MSG_LOAD_SETTING,
	MSG_GET_IPADDRESS,
	MSG_SHOW_LASTUI,
	MSG_SEND_RESULT,
	MSG_TAP1,
	MSG_TAP2,
	MSG_TAP3,
	MSG_TAP4,
};

// 这个值已经和后台约定好的
#define MAX_SPEED		99999
#define DEFAULT_SPEED	MAX_SPEED + 1

#if defined(A01_CONFIG)

#define _COPYRIGHT_STRING		_T("Copyright freeknight personal")
#define _MAINWIN_STRING			_T("360测试登录器")
#define	_TEXT_COLOR1			RGB(250, 250, 250)
#define	_TEXT_COLOR2			RGB(201, 36, 40)
#define	_TEXT_COLOR3			RGB(21, 128, 214)
#define _COPYRIGHT_COLOR		RGB(0xa8, 0xab, 0xad)

#define _VERSION_CONFIG_URL1	_T("http://www.baidu.com/versionConfig1.html")
#define _VERSION_CONFIG_URL2	_T("http://www.baidu.com/versionConfig2.html")
#define _VERSION_CONFIG_URL3	_T("http://www.baidu.com/versionConfig3.html")

#define _DOMAIN_CONFIG_URL1		_T("http://www.baidu.com/domainConfig1.html")
#define _DOMAIN_CONFIG_URL2		_T("http://www.baidu.com/domainConfig2.html")
#define _DOMAIN_CONFIG_URL3		_T("http://www.baidu.com/domainConfig3.html")

#define _UPDATE_PACK_URL1		_T("http://www.baidu.com/Launcher1.zip")
#define _UPDATE_PACK_URL2		_T("http://www.baidu.com/Launcher2.zip")
#define _UPDATE_PACK_URL3		_T("http://www.baidu.com/Launcher3.zip")

#define FK_IP_MODE 1

#define APP_ID					_T("gi3g7ad5hrmimarqari3deft5x7qv59r")
#define APP_KEY					_T("d3dft5x7qv5n96l2mt5x7qvmari3dft5")
#define DEBUG_GATE_URL			_T("test.gateway.com")
#define RELEASE_GATE_URL		_T("online.gateway.com")
#define APP_TOKEN_SUFFIX		_T("app/getAppToken")
#define GET_CDN_SUFFIX			_T("app/getCDNHostDns")
#define UPDATE_VERSION_SUFFIX	_T("app/version")
#define INSTALL_FILE_NAME		_T("360测试登录器安装包")


#endif

#endif // ___CONFIG_H_INCL___
