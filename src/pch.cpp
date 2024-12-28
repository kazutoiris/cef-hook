// pch.cpp: 与预编译标头对应的源文件

#include "pch.h"

// 当使用预编译的头时，需要使用此源文件，编译才能成功。

bool endsWith(const TCHAR* szCurName, const TCHAR* szAppName) {
	size_t lenCur = _tcslen(szCurName);
	size_t lenApp = _tcslen(szAppName);

	if (lenCur < lenApp) {
		return false;
	}

	return StrCmpI(szCurName + lenCur - lenApp, szAppName) == 0;
}

string_t IntToString(int value) {
#ifdef UNICODE
	return std::to_wstring(value);
#elif
	return std::to_string(value);
#endif
}
