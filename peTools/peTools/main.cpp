

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
DWORD GetModuleSize(TCHAR* szModuleName);
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
	int iItem=0,iSubItem=0;
	 TCHAR szModName[MAX_PATH];
	//����windows ���н��̣� �õ����н�����Ϣ
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	DWORD i;
	TCHAR szProcessName_system[MAX_PATH] = TEXT("<system Idle Process>");
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
	{
		dbgPrintf(TEXT("EnumProcesses error! \n"));
		return ;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for( i=0; i<cProcesses; i++ )
	{
		DWORD processID = aProcesses[i];
		if( processID != NULL)
		{
			HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |	PROCESS_VM_READ,
				FALSE, processID ); //PROCESS_ALL_ACCESS
			if( hProcess != NULL )
			{
				HMODULE hMod;
				DWORD cbNeeded;
				if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
					&cbNeeded) )
				{
					if( GetModuleFileNameEx( hProcess, hMod, szModName,
						sizeof(szModName) / sizeof(TCHAR)))
					{
						DWORD imageSize = GetModuleSize(szModName);

						// now we got the process, pid, imagebase, imagesize
						// they are: szModName, processID, hMod, imageSize
						iSubItem=0;
						memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
						memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT;
						_stprintf(tcharBuff, TEXT("%s"), szModName);
						lvItem.pszText = tcharBuff;
						lvItem.iItem = iItem;
						lvItem.iSubItem = iSubItem++;
						ListView_InsertItem(hwnd, &lvItem);
						//SendMessage(hwnd, LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

						memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
						memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT;
						_stprintf(tcharBuff, TEXT("%d"), processID);
						lvItem.pszText = tcharBuff;
						lvItem.iItem = iItem;
						lvItem.iSubItem = iSubItem++;
						ListView_SetItem(hwnd, &lvItem);
						//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

						memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
						memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT;
						_stprintf(tcharBuff, TEXT("0x%08X"), (DWORD)hMod);
						lvItem.pszText = tcharBuff;
						lvItem.iItem = iItem;
						lvItem.iSubItem = iSubItem++;
						ListView_SetItem(hwnd, &lvItem);
						//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

						memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
						memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT;
						_stprintf(tcharBuff, TEXT("0x%08X"), imageSize);
						lvItem.pszText = tcharBuff;
						lvItem.iItem = iItem;
						lvItem.iSubItem = iSubItem++;
						ListView_SetItem(hwnd, &lvItem);
						//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));
						iItem++;
					}
				}
				else  // <system Idle Process>
				{
					DWORD imageSize = 0;

					// now get the process, pid, imagebase, imagesize
					// they are: szModName, processID, hMod, imageSize
					iSubItem=0;
					memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
					memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT;
					_stprintf(tcharBuff, TEXT("%s"), szProcessName_system);
					lvItem.pszText = tcharBuff;
					lvItem.iItem = iItem;
					lvItem.iSubItem = iSubItem++;
					ListView_InsertItem(hwnd, &lvItem);
					//SendMessage(hwnd, LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

					memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
					memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT;
					_stprintf(tcharBuff, TEXT("%d"), processID);
					lvItem.pszText = tcharBuff;
					lvItem.iItem = iItem;
					lvItem.iSubItem = iSubItem++;
					ListView_SetItem(hwnd, &lvItem);
					//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

					memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
					memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT;
					_stprintf(tcharBuff, TEXT("0x%08X"), (DWORD)0);
					lvItem.pszText = tcharBuff;
					lvItem.iItem = iItem;
					lvItem.iSubItem = iSubItem++;
					ListView_SetItem(hwnd, &lvItem);
					//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

					memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
					memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT;
					_stprintf(tcharBuff, TEXT("0x%08X"), (DWORD)0);
					lvItem.pszText = tcharBuff;
					lvItem.iItem = iItem;
					lvItem.iSubItem = iSubItem++;
					ListView_SetItem(hwnd, &lvItem);
					//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));
					iItem++;
				}	
			}
			CloseHandle(hProcess);
			hProcess = NULL;

		}
	}

	delete[] tcharBuff;
	tcharBuff = NULL;
}

DWORD GetModuleSize(TCHAR* szModuleName)
{
#define DOSHEADERSIZE  sizeof(IMAGE_DOS_HEADER)
#define NTHEADERSIZE sizeof(IMAGE_NT_HEADERS)
	IMAGE_DOS_HEADER* pDOSHeader = NULL;
	IMAGE_NT_HEADERS* pNTHeaders = NULL;
	FILE* file = NULL;
	BYTE fileBufferDos[DOSHEADERSIZE];
	BYTE fileBufferNt[NTHEADERSIZE];
	DWORD readNumber = 0;
	errno_t err;
	err = _tfopen_s(&file, szModuleName, TEXT("rb"));
	if(err != NULL)
		return 0;
	// ��ȡDOSͷ �ҵ�e_lfanew
	readNumber = fread_s(fileBufferDos, DOSHEADERSIZE, DOSHEADERSIZE, 1, file);
	if(readNumber != 1)
		return 0;
	pDOSHeader = (IMAGE_DOS_HEADER*)fileBufferDos;
	// ����e_lfanew �ҵ�NTͷλ��
	fseek(file, pDOSHeader->e_lfanew, SEEK_SET);
	// ��ȡNTͷ
	readNumber = fread_s(fileBufferNt, NTHEADERSIZE, NTHEADERSIZE, 1, file);
	if(readNumber != 1)
		return 0;
	pNTHeaders = (IMAGE_NT_HEADERS*)fileBufferNt;
	// ����NTͷ��OptionalHeader.SizeofImage
	return pNTHeaders->OptionalHeader.SizeOfImage;
}