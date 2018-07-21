#ifndef __LOGONER_UTILS_H__
#define __LOGONER_UTILS_H__
#include "resource.h"
#include <vector>
typedef void UtilsCallFunc(void* obj, long p, long m);

extern CStringW AnsiToUnicode(CStringA strA);
extern CStringA UnicodeToAnsi(CStringW strW);

extern INT DownFile(const CString url, const CString path, UtilsCallFunc* func, void* obj);

extern BOOL Execute(CString strCmd, BOOL show=false, CString cmd=L"cmd.exe");
extern BOOL Execute2(CString strCmd, BOOL show = false, CString cmd = L"cmd.exe", CString strCaption = _T(""));
extern BOOL Clearing(UtilsCallFunc* func, void* obj);

extern BOOL UPDNS(UtilsCallFunc* func, void* obj);

extern CString GetRegisterValue(HKEY hkey, LPCTSTR rgspath, LPCTSTR valueName, DWORD type);

extern BOOL CheckChrome();
extern CString GetSystemName();
extern BOOL ParseStringToVector(std::vector<CString> &list, CString);
#endif // __LOGONER_UTILS_H__