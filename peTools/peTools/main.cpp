

#include "stdafx.h"
#include "debugTool.h"

//ȫ�ֱ��� imageBase
HINSTANCE hAppInstance;

LRESULT CALLBACK WindowProc(IN HWND hwnd, IN UINT uMsg, IN WPARAM wParam, IN LPARAM lParam);
void createButtons(HWND hwndmain);
// ��ں������ص�����::C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include\winbase.h
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, 
				__in LPSTR lpCmdLine, __in int nShowCmd )
{
	//����ȫ�ֱ���  imageBase
	hAppInstance = hInstance;
	dbgPrintf(TEXT("hAppInstance=%x\nlpCmdLine=%x\nnShowCmd=%x\n"), hAppInstance, lpCmdLine, nShowCmd);
	//��ƴ�����
	TCHAR className[] = TEXT("My First Window");  //��������
	WNDCLASS wndclass={0}; //����������Ķ��� //�����ʼ��
	wndclass.hbrBackground = (HBRUSH)COLOR_MENU;
	wndclass.lpfnWndProc = WindowProc; //��Ϣ��������WindowProc�������ʵ��
	wndclass.lpszClassName = className;
	wndclass.hInstance = hInstance;

	//ע�ᴰ����
	RegisterClass(&wndclass);

	//��������
	HWND hwnd = CreateWindow(
		className,					// ��һ����ע��Ĵ����������
		TEXT("�ҵĵ�һ������"),		//���ڱ���
		WS_OVERLAPPEDWINDOW,		//���������ʽ
		60,							//����ڸ����ڵ�X����
		60,							//����ڸ����ڵ�Y����
		600,						//���ڿ��
		600,						//���ڸ߶�
		NULL,						//�����ھ��
		NULL,						//�˵����
		hInstance,					//��ǰӦ�ó���ľ��
		NULL						//��������
		);
	if(hwnd == NULL)					//����ʧ��?
		return 0;
	createButtons(hwnd);
	//��ʾ����
	ShowWindow(hwnd, SW_SHOW);

	//��Ϣѭ��
	MSG msg;
	// lpMsg[out] ָ��һ�����ڴ��߳���Ϣ���н���MSG��Ϣ�Ľṹ��
	// hWnd[in] ָ����ȡ��Ϣ����Դ��ΪNULL��ʾ��ȡ��ǰ�߳��µ����д�����Ϣ�����ָ�����ھ�����ͱ�ʾֻ���߳���ĳһ�����ڵ���Ϣ��
	// ����ֵ�� ����ֵ��ʾ��ǰ��ȡ���Ƿ�WM_QUIT��Ϣ��Ϊ���ʾ��ȡ����WM_QUIT��Ϣ�����߱�ʾlpMsgָ�����һ����Ч��ֵ��
	while( GetMessage(&msg, NULL, 0, 0) )
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//ʵ�ֻص����� // WindowProc; //��Ϣ������
	/*
	������Ϣ����������, ���ڻص�����:
	1. ���ڻص��������������Ϣ,����return 0;
	2. ���ڻص��������������Ϣ,��DefWindowProc
	*/
LRESULT CALLBACK WindowProc(
	IN HWND hwnd,
	IN UINT uMsg,
	IN WPARAM wParam,
	IN LPARAM lParam
	)
{
	//dbgPrintf("uMsg=%x\n", uMsg);
	switch(uMsg)
	{
	case WM_QUIT:
		// ������WM_QUIT�� GetMessage ��ȡ�� WM_QUIT �᷵�� 0
		dbgPrintf(TEXT("WM_QUIT uMsg=%x wParam=%x lParam=%x\n"), uMsg, wParam, lParam);
		return 0;
	case WM_CREATE:
		dbgPrintf(TEXT("WM_CREATE uMsg=%x wParam=%x lParam=%x\n"), uMsg, wParam, lParam);
		break;
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY uMsg=%x wParam=%x lParam=%x\n"), uMsg, wParam, lParam);
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
		dbgPrintf(TEXT("WM_LBUTTONDOWN uMsg=%x wParam=%x lParam=%x\n"), uMsg, wParam, lParam);
		break;
	case WM_COMMAND:  // ���Խ����Ӵ��� ���� button ����Ϣ
		dbgPrintf(TEXT("WM_COMMAND uMsg=%x wParam=%x lParam=%x\n"), uMsg, wParam, lParam);
		switch( LOWORD(wParam) )
		{
		case 1010:
			MessageBox(hwnd, TEXT("1010 button!"), TEXT("Demo"), MB_OK);
			break;
		case 1011:
			MessageBox(hwnd, TEXT("1011 button!"), TEXT("Demo"), MB_OK);
			break;
		case 1012:
			MessageBox(hwnd, TEXT("1012 button!"), TEXT("Demo"), MB_OK);
			break;
		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
		break;
	default:
		dbgPrintf(TEXT("Other uMsg=%x wParam=%x lParam=%x\n"), uMsg, wParam, lParam);
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

// ��ӽ���button
void createButtons(HWND hwndmain)
{
	DWORD start_x,start_y, width, hight, interval_x, interval_y;
	start_x = 20;
	start_y = 40;

	width = 100;
	hight = 25;

	interval_x = 10;
	interval_y = 10;

	HWND button1 = CreateWindow(
		TEXT("BUTTON"),
		TEXT("Button1"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_MULTILINE | BS_PUSHBUTTON ,
		start_x,start_y,
		width, hight,
		hwndmain,HMENU(1010),
		hAppInstance,NULL);

	start_x += width + interval_x;
	//start_y += hight + interval_y;
	HWND button1_1 = CreateWindow(
		TEXT("BUTTON"),
		TEXT("Button1_1"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_RADIOBUTTON,
		start_x,start_y,
		width, hight,
		hwndmain,HMENU(1011),
		hAppInstance,NULL);
	
	start_x += width + interval_x;
	//start_y += hight + interval_y;
	HWND button1_2 = CreateWindow(
		TEXT("BUTTON"),
		TEXT("Button1_2"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_CHECKBOX | BS_AUTOCHECKBOX,
		start_x,start_y,
		width, hight,
		hwndmain,HMENU(1012),
		hAppInstance,NULL);
#if 0	
	start_x = 20;
	//start_x += width + interval_x;
	start_y += hight + interval_y;
	HWND button2 = CreateWindow(
		TEXT("COMBOBOX"),
		TEXT("Button2"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_MULTILINE | BS_PUSHBUTTON ,
		start_x,start_y,
		width, hight,
		hwndmain,HMENU(102),
		hAppInstance,NULL);

	//start_x += width + interval_x;
	start_y += hight + interval_y;
	HWND button3 = CreateWindow(
		TEXT("EDIT"),
		TEXT("Button3"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_MULTILINE | BS_PUSHBUTTON ,
		start_x,start_y,
		width, hight,
		hwndmain,HMENU(103),
		hAppInstance,NULL);

	//start_x += width + interval_x;
	start_y += hight + interval_y;
	HWND button4 = CreateWindow(
		TEXT("LISTBOX"),
		TEXT("Button4"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_MULTILINE | BS_PUSHBUTTON ,
		start_x,start_y,
		width, hight,
		hwndmain,HMENU(104),
		hAppInstance,NULL);

	//start_x += width + interval_x;
	start_y += hight + interval_y;
	HWND button5 = CreateWindow(
		TEXT("STATIC"),
		TEXT("Button5"),
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | BS_MULTILINE | BS_PUSHBUTTON ,
		start_x,start_y,
		width, hight,
		hwndmain,HMENU(105),
		hAppInstance,NULL);
#endif
	TCHAR szClassName[100];
	WNDCLASS wndclass_get;
	int ll = sizeof(szClassName);
	memset(szClassName, 0, ll);
	GetClassName(button1, szClassName, ll-1);
	GetClassInfo(hAppInstance, szClassName, &wndclass_get);

	dbgPrintf(TEXT("wndclass_get.style=%x\n"), wndclass_get.style);
	dbgPrintf(TEXT("wndclass_get.lpszClassName=%s\n"), wndclass_get.lpszClassName);
	dbgPrintf(TEXT("wndclass_get.lpfnWndProc=%x\n"), wndclass_get.lpfnWndProc);

	return ;
}