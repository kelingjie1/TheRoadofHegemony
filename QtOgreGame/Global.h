#include "stdafx.h"
namespace Global
{
	extern float Gravity;
	extern float DiceSpeed;
	extern float MoveSpeed;
	extern float CardWidthScale;
	extern float CardChooseHeight;
	void LoadGlobleSetting();
}
inline std::wstring ANSI_to_UNICODE(const char *str)
{
	int size=MultiByteToWideChar(CP_ACP, 0, str, -1, 0, 0);
	std::wstring wstr;
	wstr.resize(size);
	MultiByteToWideChar(CP_ACP, 0, str, -1, const_cast<WCHAR*>(wstr.data()), size);
	return wstr;
}
inline std::string UNICODE_to_UTF8(const WCHAR *wstr)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, 0, 0, 0, 0);
	std::string str;
	str.resize(size);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, const_cast<char*>(str.data()), size, 0, 0);
	return str;
}
inline std::string ANSI_to_UTF8(const char *str)
{
	return UNICODE_to_UTF8(ANSI_to_UNICODE(str).c_str());
}
inline CEGUI::String CEGUIText(const WCHAR *str)
{
	return (CEGUI::utf8*)UNICODE_to_UTF8(str).c_str();
}
inline CEGUI::String CEGUIText(const char *str)
{
	return (CEGUI::utf8*)ANSI_to_UTF8(str).c_str();
}
