/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qmfcapp.h"
#include "qwinwidget.h"

#include <QMessageBox>
#include <QHBoxLayout>
#include <windows.h>
#include "mainWidget.h"
HINSTANCE g_instance;
#include  "../include/easyhook/easyhook.h"
#if _WIN64
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook64.lib")
#else
#pragma comment(lib, "..\\lib\\easyHook\\EasyHook32.lib")
#endif
#include <iostream>
using namespace std;
DWORD gFreqOffset = 0;

extern "C" __declspec(dllexport) bool showDialog(HWND parent)
{
	QWinWidget win(parent);
	win.showCentered();
	//QMessageBox::about(&win, "About QtMfc", "QtMfc Version 1.0\nCopyright (C) 2003");
	QHBoxLayout hbox(&win);
	MainWidget *widget = new MainWidget(&win);
	widget->setWindowFlags(Qt::Window);
	hbox.addWidget(widget);
	widget->show();
	win.show();
	
	qApp->exec();
	return TRUE;
}
//extern "C" __declspec(dllexport) bool hideDialog(HWND parent)
//{
//	QWinWidget win(parent);
//	win.showCentered();
//	QMessageBox::about(&win, "About QtMfc", "QtMfc Version 1.0\nCopyright (C) 2003");
//
//	return TRUE;
//}
HHOOK hkGame;
LRESULT CALLBACK KeyboardProc(
	int code,       // hook code
	WPARAM wParam,  // virtual-key code
	LPARAM lParam   // keystroke-message information
)
{
	MessageBox(0, "F10被按下", 0, 0);
	if (code < 0)
		return CallNextHookEx(hkGame, code, wParam, lParam);
	if (wParam == VK_HOME && lParam & 0x40000000)
	{
		//MessageBox(0, "F10被按下", 0, 0);
		showDialog(nullptr);
		//DialogBox(GetModuleHandle("GameDll.dll"), MAKEINTRESOURCE(IDD_DIALOG1), FindWindow(NULL, "MyGame"), DlgProc); //第一个参数也可以写作：(HMODULE)LoadLibrar("GameDll.dll")
		return 1;
	}
	return 0;
}



BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpvReserved*/ )
{
    static bool ownApplication = FALSE;

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		g_instance = hInstance;
		ownApplication = QMfcApp::pluginInstance(hInstance);
//		showDialog(nullptr);

	}
	if (dwReason == DLL_PROCESS_DETACH && ownApplication)
	{
		delete qApp;
	}
    return TRUE;
}


static void __stdcall startHook()
{
	HWND g_gameHwnd = (HWND)gFreqOffset;
	DWORD TID = GetWindowThreadProcessId(g_gameHwnd, nullptr);
	hkGame = SetWindowsHookEx(WH_KEYBOARD, HOOKPROC(&KeyboardProc), g_instance, 0);
	if (!hkGame)
		MessageBox(0, "HOOK失败！", 0, MB_OK);
	else
		MessageBox(0, "HOOK成功！", 0, MB_OK);
}


extern "C" void __declspec(dllexport) __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo);

void __stdcall NativeInjectionEntryPoint(REMOTE_ENTRY_INFO* inRemoteInfo)
{
	if (inRemoteInfo->UserDataSize == sizeof(DWORD))
	{
		gFreqOffset = *reinterpret_cast<DWORD *>(inRemoteInfo->UserData);
	}
	//HWND g_gameHwnd = (HWND)gFreqOffset;
	//DWORD TID = GetWindowThreadProcessId(g_gameHwnd, nullptr);
	//hkGame = SetWindowsHookEx(WH_KEYBOARD, HOOKPROC(&KeyboardProc), g_instance, 0);
	//if (!hkGame)
	//	MessageBox(0, "HOOK失败！", 0, MB_OK);
	//else
	//	MessageBox(0, "HOOK成功！", 0, MB_OK);
	startHook();
//	showDialog(g_gameHwnd);
	//// Perform hooking
	//HOOK_TRACE_INFO hHook = { NULL }; // keep track of our hook
	//HOOK_TRACE_INFO hHookMessageBoxW = { NULL }; // keep track of our hook
	//HOOK_TRACE_INFO hHookSend = { NULL }; // keep track of our hook

	//std::cout << "\n";
	//std::cout << "NativeInjectionEntryPoint: Win32 Beep found at address: " << GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Beep") << "\n";




	//// If the threadId in the ACL is set to 0,
	//// then internally EasyHook uses GetCurrentThreadId()
	//ULONG ACLEntries[1] = { 0 };

	//// Disable the hook for the provided threadIds, enable for all others
	//LhSetExclusiveACL(ACLEntries, 1, &hHook);
	//pSendDataFile = 
	return;
}