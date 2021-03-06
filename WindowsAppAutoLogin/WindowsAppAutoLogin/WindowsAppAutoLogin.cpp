// WindowsAppAutoLogin.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#define QQ_PATH TEXT("D:\\Program Files (x86)\\Tencent\\QQ\\Bin\\QQ.exe")
#define QQ_CLASS_NAME TEXT("TXGuiFoundation")
#define QQ_WINDOWS_NAME TEXT("QQ")
#define QQ_EDIT_TITLE TEXT("QQEdit")
#define QQ_EDIT_CLASS TEXT("Edit")
//  QQEdit

char USER_ID[10] = ("867669661");
char PASS_ID[] = ("wyw@qq.com");

//  0  1  2  3  4   5  6  7  8  9
DWORD TABLE_NUMBER_TO_VIRTUAL_CODE[10] = {
	48,49,50,51,52,53,54,55,56,57 };
// a ~ z
DWORD TABLE_AZ_TO_VIRTUAL_CODE[26] = {
//  a   b  c  d  e  f  g  h  i  j
	65,66,67,68,69,70,71,72,73,74,
//  k  l  m  n  o  p  q  r  s  t
	75,76,77,78,79,80,81,82,83,84,
//  u  v  w  x  y  z
	85,86,87,88,89,90 };
// tab shift enter 2(@) .
DWORD TABLE_SPECIAL_TO_VIRTUAL_CODE[] = {
	VK_TAB,VK_SHIFT,VK_RETURN, 50, VK_DECIMAL };

//867669661
DWORD USER_ID_KEY_CODE[] = {
	56,54,55,54,54,57,54,54,49
};
// wyw@qq.com
DWORD PASS_ID_KEY_CODE[] = {
	87, 89, 87, VK_SHIFT, 50, 81, 81, VK_DECIMAL, 67, 79, 77
};

BOOL CALLBACK EnumChildWindow(HWND hwnd, LPARAM lParam);
int main(int argc, TCHAR* argv[])
{
	DWORD dwTabs = 0;
	if (argc > 1)
	{
		_stscanf_s(argv[1], TEXT("%d"), &dwTabs);
		_tprintf(TEXT("tabs :%d\r\n"), dwTabs);
	}

	STARTUPINFOW si = { 0 };
	si.cb = sizeof(si);
	PROCESS_INFORMATION pi = { 0 };
	BOOL bRet = CreateProcess(QQ_PATH, NULL, NULL, NULL, NULL,
		NULL, NULL, NULL, &si, &pi);
	if(!bRet)
	{
		_tprintf(TEXT("CreateProcess 失败！\r\n"));
		return -1;
	}

	Sleep(10 * 1000);

	HWND hQQwnd = FindWindow(QQ_CLASS_NAME, QQ_WINDOWS_NAME);

	if (hQQwnd == NULL)
	{
		_tprintf(TEXT("窗口不存在！\r\n"));
		return -1;
	}
	typedef void (WINAPI *PSWITCHTOTHISWINDOW)(HWND, BOOL);
	PSWITCHTOTHISWINDOW switchToThisWindow = NULL;
	HMODULE hUser32 = LoadLibrary(TEXT("user32.dll"));
	switchToThisWindow = (PSWITCHTOTHISWINDOW)GetProcAddress(hUser32, "SwitchToThisWindow");
	if (!switchToThisWindow)
	{
		_tprintf(TEXT("switchToThisWindow 没找到！\r\n"));
		return -1;
	}
	switchToThisWindow(hQQwnd, TRUE);

	// 遍历 子窗口， 确认 Edit 控件： 账号， 密码，确认按钮： 登录
	//::EnumChildWindows(hQQwnd, EnumChildWindow, 0);

// 	HWND hEdit = NULL;
// 	hEdit = FindWindowEx(hQQwnd, NULL, NULL, NULL);
// 	if (!hEdit)
// 	{
// 		_tprintf(TEXT("hEdit 没找到！\r\n"));
// 		return -1;
// 	}
// 	RECT rect;
// 	bRet = GetWindowRect(hQQwnd, &rect);
// 	if (bRet)
// 	{
// 		_tprintf(TEXT("GetWindowRect 失败！\r\n"));
// 		return -1;
// 	}
// 	_tprintf(TEXT("rect {%d %d %d %d} "), rect.right, rect.top, rect.left, rect.bottom);

	// 无法获取 子窗口 无法获取 窗口Rect
	// 直接通过键盘输入 账号、tab、密码

	for( int i=0; i<dwTabs; i++)
	{
		_tprintf(TEXT("tab 切到 账号区！\r\n"));
		// tab 切到 账号区
		keybd_event(VK_TAB, 0, 0, 0);
		keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);
	}

	_tprintf(TEXT("模拟键盘输入账号！\r\n"));
	// 模拟键盘输入账号
	for (int i = 0; i < sizeof(USER_ID_KEY_CODE) / sizeof(DWORD); i++)
	{
		keybd_event(USER_ID_KEY_CODE[i], 0, 0, 0);
		keybd_event(USER_ID_KEY_CODE[i], 0, KEYEVENTF_KEYUP, 0);
	}
	_tprintf(TEXT("tab 切到 密码区！\r\n"));
	// tab 切到 密码区
	keybd_event(VK_TAB, 0, 0, 0);
	keybd_event(VK_TAB, 0, KEYEVENTF_KEYUP, 0);

	_tprintf(TEXT("模拟键盘输入密码！\r\n"));
	// 模拟键盘输入密码
	for (int i = 0; i < sizeof(PASS_ID_KEY_CODE) / sizeof(DWORD); i++)
	{
		if (PASS_ID_KEY_CODE[i] == VK_SHIFT)  // shift 按键
		{
			keybd_event(PASS_ID_KEY_CODE[i], 0, 0, 0);
			keybd_event(PASS_ID_KEY_CODE[i+1], 0, 0, 0);
			keybd_event(PASS_ID_KEY_CODE[i], 0, KEYEVENTF_KEYUP, 0);
			keybd_event(PASS_ID_KEY_CODE[i+1], 0, KEYEVENTF_KEYUP, 0);
			i++;
		}
		else
		{
			keybd_event(PASS_ID_KEY_CODE[i], 0, 0, 0);
			keybd_event(PASS_ID_KEY_CODE[i], 0, KEYEVENTF_KEYUP, 0);
		}
	}
	_tprintf(TEXT("模拟键盘输入Enter！\r\n"));
	// 模拟键盘输入Enter
	keybd_event(VK_RETURN, 0, 0, 0);
	keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);

	_tprintf(TEXT("wait a Enter！\r\n"));
	getchar();
    return 0;
}


BOOL CALLBACK EnumChildWindow(HWND hwnd, LPARAM lParam)
{
	static DWORD counter = 0;
	TCHAR szTitle[MAX_PATH] = { 0 };
	::GetWindowText(hwnd, szTitle, MAX_PATH);
	_tprintf(TEXT("[子窗口 %d] hwnd=%d， szTitle:%s\r\n"), counter++, (DWORD)hwnd, szTitle);

	return TRUE;
}

