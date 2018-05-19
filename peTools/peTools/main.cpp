

#include "stdafx.h"

LRESULT CALLBACK WindowProc(
	IN HWND hwnd,
	IN UINT uMsg,
	IN WPARAM wParam,
	IN LPARAM lParam
	);

// ��ں������ص�����::C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include\winbase.h
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, 
				__in LPSTR lpCmdLine, __in int nShowCmd )
{
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
		300,						//���ڸ߶�
		NULL,						//�����ھ��
		NULL,						//�˵����
		hInstance,					//��ǰӦ�ó���ľ��
		NULL						//��������
		);
	if(hwnd == NULL)					//����ʧ��?
		return 0;

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
	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

