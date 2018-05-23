

#include "stdafx.h"
#include "debugTool.h"

#if 1 // VC6 need 
#include <CommCtrl.h>
#pragma  comment(lib, "comctl32.lib")
#endif

//ȫ�ֱ��� imageBase
HINSTANCE hAppInstance;
VOID InitProcessListView(HWND hwnd);
VOID InitModuleListView(HWND hwnd);
VOID EnumProcess(HWND hwnd);
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// ��ں������ص�����::C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include\winbase.h
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, 
				__in LPSTR lpCmdLine, __in int nShowCmd )
{
	//����ȫ�ֱ���  imageBase
	hAppInstance = hInstance;

#if 1  // VC6 need 
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);
#endif

	//��������
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);

	return 0;
}

// ��Ϣ�ص�����  ���������Ϣ����TRUE ���򷵻�FALSE
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HICON hicon = NULL;
	BOOL nRet = FALSE;
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		PostQuitMessage(0);
		nRet = TRUE;
		break;
	case WM_CLOSE:  // ���Ͻǵ�x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		PostQuitMessage(0);
		nRet = TRUE;
		break;
	case WM_INITDIALOG:   //WM_CREATE:
		hicon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_ICON_XPS) );
		//����ͼ��
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (DWORD)hicon);
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (DWORD)hicon);
		SendMessage(hwnd, WM_SETICON, ICON_SMALL2, (DWORD)hicon);
		InitProcessListView(hwnd);
		InitModuleListView(hwnd);
		nRet = TRUE;
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_OPENPE:
			dbgPrintf(TEXT("WM_COMMAN: IDC_BUTTON_OPENPE wParam=%x lParam=%x\n"),wParam, lParam);
			nRet = TRUE;
			break;
		case IDC_BUTTON_PROTECT:
			dbgPrintf(TEXT("WM_COMMAN: IDC_BUTTON_PROTECT wParam=%x lParam=%x\n"),wParam, lParam);
			nRet = TRUE;
			break;
		case IDC_BUTTON_DAMAGE:
			dbgPrintf(TEXT("WM_COMMAN: IDC_BUTTON_DAMAGE wParam=%x lParam=%x\n"),wParam, lParam);
			nRet = TRUE;
			break;
		case IDC_BUTTON_ABOUT:
			dbgPrintf(TEXT("WM_COMMAN: IDC_BUTTON_ABOUT wParam=%x lParam=%x\n"),wParam, lParam);
			nRet = TRUE;
			break;
		case IDC_BUTTON_EXIT:
			dbgPrintf(TEXT("WM_COMMAN: IDC_BUTTON_EXIT wParam=%x lParam=%x\n"),wParam, lParam);
			PostQuitMessage(0);
			nRet = TRUE;
			break;
		default:
			break;
		}		
		break;
	case WM_SYSCOMMAND:
		switch( (DWORD)wParam )
		{
		case SC_CLOSE:
			dbgPrintf(TEXT("WM_SYSCOMMAND: SC_CLOSE wParam=%x lParam=%x\n"),wParam, lParam);
			PostQuitMessage(0);
			nRet = TRUE;
			break;
		case SC_MOVE:
			dbgPrintf(TEXT("WM_SYSCOMMAND: SC_MOVE wParam=%x lParam=%x\n"),wParam, lParam);
			nRet = FALSE;  //���� FALSE ����Ĭ�ϴ�����
			break;
		case SC_SIZE:
			dbgPrintf(TEXT("WM_SYSCOMMAND: SC_SIZE wParam=%x lParam=%x\n"),wParam, lParam);
			nRet = TRUE;
			break;
		}
		break;
	default:
		break;
	}

	return nRet; 
}


VOID InitProcessListView(HWND hwnd)
{
	LV_COLUMN lv;
	HWND hListProcess;

	//��ʼ��
	memset(&lv, 0, sizeof(lv));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hwnd, IDC_LIST_PROCESS);
	//��������ѡ��
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//��1��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("����·��");
	lv.cx = 420;
	lv.iSubItem = 0;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

	//��2��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);

	//��3��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("�����ַ");
	lv.cx = 100;
	lv.iSubItem = 2;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 2, (DWORD)&lv);

	//��4��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM ;
	lv.pszText = TEXT("�����С");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 3, (DWORD)&lv);

	EnumProcess(hListProcess);
}


VOID InitModuleListView(HWND hwnd)
{
	LV_COLUMN lv;
	HWND hListModule;

	//��ʼ��
	memset(&lv, 0, sizeof(lv));
	//��ȡIDC_LIST_PROCESS���
	hListModule = GetDlgItem(hwnd, IDC_LIST_MODULE);
	//��������ѡ��
	SendMessage(hListModule, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//��1��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("ģ��·��");
	lv.cx = 520;
	lv.iSubItem = 0;
	SendMessage(hListModule, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

	//��2��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("�����ַ");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListModule, LVM_INSERTCOLUMN, 1, (DWORD)&lv);

	//��3��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM ;
	lv.pszText = TEXT("�����С");
	lv.cx = 100;
	lv.iSubItem = 2;
	SendMessage(hListModule, LVM_INSERTCOLUMN, 2, (DWORD)&lv);
}

// 


VOID EnumProcess(HWND hwnd)
{
#define BUFFLENGTHMAX 1024
	LV_ITEM lvItem;
	TCHAR* tcharBuff = new TCHAR[BUFFLENGTHMAX];
	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
	int iItem,iSubItem;

	//����windows ���н��̣� �õ����н�����Ϣ

	//������Ϣ ���浽ListItem 
	
	//ListItem ��ӵ�ListView
	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
	memset(&lvItem, 0, sizeof(LV_ITEM));
	lvItem.mask = LVIF_TEXT;
	_stprintf(tcharBuff, TEXT("%s"), TEXT("C://PATH//COMMAND.exe"));
	lvItem.pszText = tcharBuff;
	lvItem.iItem = 0;
	lvItem.iSubItem = 0;
	ListView_InsertItem(hwnd, &lvItem);
	//SendMessage(hwnd, LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
	memset(&lvItem, 0, sizeof(LV_ITEM));
	lvItem.mask = LVIF_TEXT;
	_stprintf(tcharBuff, TEXT("%s"), TEXT("448"));
	lvItem.pszText = tcharBuff;
	lvItem.iItem = 0;
	lvItem.iSubItem = 1;
	ListView_SetItem(hwnd, &lvItem);
	//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
	memset(&lvItem, 0, sizeof(LV_ITEM));
	lvItem.mask = LVIF_TEXT;
	_stprintf(tcharBuff, TEXT("%s"), TEXT("0x400000"));
	lvItem.pszText = tcharBuff;
	lvItem.iItem = 0;
	lvItem.iSubItem = 2;
	ListView_SetItem(hwnd, &lvItem);
	//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
	memset(&lvItem, 0, sizeof(LV_ITEM));
	lvItem.mask = LVIF_TEXT;
	_stprintf(tcharBuff, TEXT("%s"), TEXT("0x250000"));
	lvItem.pszText = tcharBuff;
	lvItem.iItem = 0;
	lvItem.iSubItem = 3;
	ListView_SetItem(hwnd, &lvItem);
	//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

	delete[] tcharBuff;
	tcharBuff = NULL;
}