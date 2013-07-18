#include "stdafx.h"
#include "InputHook.h"
HHOOK InputHook::gHook;
UINT VirtualKeyToScanCode(WPARAM wParam, LPARAM lParam)   
{   
	if(HIWORD(lParam) & 0x0F00)    
	{    
		UINT scancode = MapVirtualKey(wParam, 0);    
		return scancode | 0x80;    
	}    
	else    
	{    
		return HIWORD(lParam) & 0x00FF;    
	}    
}
LRESULT CALLBACK InputHook::ChineseHook(int nCode,WPARAM wParam,LPARAM lParam)  
{  
	if (nCode < 0)  
	{  
		return CallNextHookEx(gHook,nCode,wParam,lParam);  
	}  

	MSG * msg = (MSG *) lParam;  

	switch(msg->message)     
	{     
	case WM_CHAR:     
		if (!(GetKeyState(VK_LCONTROL) & 0xF0))  //过滤LControl   
		{  
			CEGUI::utf32 word = (CEGUI::utf32)msg->wParam;  
			//if (word >= 32)  
				CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(word);  //将工程设为Unicode模式,这儿可以直接接收,而网上那个在多字节模式下的貌似有点问题,码和CEGUI的识别对不上~~~   
		}
		break;
// 	case WM_KEYDOWN:
// 		CEGUI::System::getSingleton().injectKeyDown((CEGUI::utf32)(VirtualKeyToScanCode(wParam, lParam)));  
// 		//CEGUI::System::getSingleton().injectKeyDown((CEGUI::utf32)(VirtualKeyToScanCode(wParam, lParam)));  
// 		break;
// 	case WM_KEYUP:
// 		CEGUI::System::getSingleton().injectKeyUp((CEGUI::utf32)(VirtualKeyToScanCode(wParam, lParam)));  
// 		//CEGUI::System::getSingleton().injectKeyUp((CEGUI::utf32)(VirtualKeyToScanCode(wParam, lParam)));  
// 		break;
	} 


	return CallNextHookEx(gHook,nCode,wParam,lParam);     
}
void InputHook::StartHook()
{
	gHook = SetWindowsHookEx(WH_GETMESSAGE,InputHook::ChineseHook,NULL,GetCurrentThreadId());
}