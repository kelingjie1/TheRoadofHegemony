#pragma once;
#include<Windows.h>
namespace InputHook
{
	extern HHOOK gHook;
	LRESULT CALLBACK ChineseHook(int nCode,WPARAM wParam,LPARAM lParam);
	void StartHook();
};