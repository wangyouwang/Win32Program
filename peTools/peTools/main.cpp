

#include "stdafx.h"

LRESULT CALLBACK WindowProc(
	IN HWND hwnd,
	IN UINT uMsg,
	IN WPARAM wParam,
	IN LPARAM lParam
	);

// 入口函数，回调函数::C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include\winbase.h
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, 
				__in LPSTR lpCmdLine, __in int nShowCmd )
{
	//设计窗口类
	TCHAR className[] = TEXT("My First Window");  //窗口类名
	WNDCLASS wndclass={0}; //创建窗口类的对象 //必须初始化
	wndclass.hbrBackground = (HBRUSH)COLOR_MENU;
	wndclass.lpfnWndProc = WindowProc; //消息处理函数，WindowProc，需后续实现
	wndclass.lpszClassName = className;
	wndclass.hInstance = hInstance;

	//注册窗口类
	RegisterClass(&wndclass);

	//创建窗口
	HWND hwnd = CreateWindow(
		className,					// 上一步中注册的窗口类的类名
		TEXT("我的第一个窗口"),		//窗口标题
		WS_OVERLAPPEDWINDOW,		//窗口外观样式
		60,							//相对于父窗口的X坐标
		60,							//相对于父窗口的Y坐标
		600,						//窗口宽度
		300,						//窗口高度
		NULL,						//父窗口句柄
		NULL,						//菜单句柄
		hInstance,					//当前应用程序的句柄
		NULL						//附加数据
		);
	if(hwnd == NULL)					//创建失败?
		return 0;

	//显示窗口
	ShowWindow(hwnd, SW_SHOW);

	//消息循环
	MSG msg;
	// lpMsg[out] 指向一个用于从线程消息队列接收MSG消息的结构体
	// hWnd[in] 指定获取消息的来源；为NULL表示获取当前线程下的所有窗口消息；如果指定窗口句柄，就表示只获线程下某一个窗口的消息；
	// 返回值： 非零值表示当前获取的是非WM_QUIT消息，为零表示获取的是WM_QUIT消息，或者表示lpMsg指向的是一个无效的值；
	while( GetMessage(&msg, NULL, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//实现回调函数 // WindowProc; //消息处理函数
	/*
	窗口消息处理函数程序, 窗口回调函数:
	1. 窗口回调函数处理过的消息,必须return 0;
	2. 窗口回调函数不处理的消息,由DefWindowProc
	*/
LRESULT CALLBACK WindowProc(
	IN HWND hwnd,
	IN UINT uMsg,
	IN WPARAM wParam,
	IN LPARAM lParam
	)
{
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

