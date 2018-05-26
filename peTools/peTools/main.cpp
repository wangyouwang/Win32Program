

#include "stdafx.h"
#include "debugTool.h"


#include <CommCtrl.h>
#pragma  comment(lib, "comctl32.lib")
#define BUFFLENGTHMAX 1024
#define MAXPROCESS 1024
#define MAXMODULES  1024

//全局变量 imageBase
HINSTANCE hAppInstance;
VOID InitProcessListView(HWND hwnd);
VOID InitModuleListView(HWND hwnd);
VOID EnumProcess(HWND hwnd);
VOID EnumModules(HWND hListProcess, WPARAM wparam, LPARAM lparam);
VOID ShowModules(HWND hListModule, DWORD processId);
DWORD GetModuleSize(TCHAR* szModuleName);
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
// 入口函数，回调函数::C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include\winbase.h
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, 
				__in LPSTR lpCmdLine, __in int nShowCmd )
{
	//保存全局变量  imageBase
	hAppInstance = hInstance;

#if 1  // VC6 need 
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icex);
#endif

	//创建窗口
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DialogProc);

	return 0;
}

// 消息回调函数  处理过的消息返回TRUE 否则返回FALSE
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	NMHDR* nmhdr = NULL;
	HICON hicon = NULL;
	BOOL nRet = FALSE;
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		PostQuitMessage(0);
		nRet = TRUE;
		break;
	case WM_CLOSE:  // 右上角的x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		PostQuitMessage(0);
		nRet = TRUE;
		break;
	case WM_NOTIFY:
		{
			dbgPrintf(TEXT("wParam=0x%x\n"),wParam);
			nmhdr = (NMHDR*) lParam;
			HWND hListProcess = GetDlgItem(hwnd, IDC_LIST_PROCESS);
			dbgPrintf(TEXT("lParam:nmhdr.hwndFrom = %x, hListProcess:IDC_LIST_PROCESS = %x"), nmhdr->hwndFrom, hListProcess );
			if(wParam == IDC_LIST_PROCESS && nmhdr->code == NM_CLICK)
			{
				EnumModules(hListProcess, wParam, lParam);
				nRet = TRUE;
			}
		}
		break;
	case WM_INITDIALOG:   //WM_CREATE:
		hicon = LoadIcon(hAppInstance, MAKEINTRESOURCE(IDI_ICON_XPS) );
		//设置图标
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (DWORD)hicon);
		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (DWORD)hicon);
		SendMessage(hwnd, WM_SETICON, ICON_SMALL2, (DWORD)hicon);

		SetWindowPos(hwnd, NULL, 100, 100, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
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
			nRet = FALSE;  //返回 FALSE 交由默认处理函数
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

	//初始化
	memset(&lv, 0, sizeof(lv));
	//获取IDC_LIST_PROCESS句柄
	hListProcess = GetDlgItem(hwnd, IDC_LIST_PROCESS);
	//设置整行选中
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//第1列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("进程路径");
	lv.cx = 620;
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

	//第2列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("PID");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 1, (DWORD)&lv);

	//第3列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("镜像基址");
	lv.cx = 100;
	lv.iSubItem = 2;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 2, (DWORD)&lv);

	//第4列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM ;
	lv.pszText = TEXT("镜像大小");
	lv.cx = 100;
	lv.iSubItem = 3;
	SendMessage(hListProcess, LVM_INSERTCOLUMN, 3, (DWORD)&lv);

	EnumProcess(hListProcess);
}


VOID InitModuleListView(HWND hwnd)
{
	LV_COLUMN lv;
	HWND hListModule;

	//初始化
	memset(&lv, 0, sizeof(lv));
	//获取IDC_LIST_PROCESS句柄
	hListModule = GetDlgItem(hwnd, IDC_LIST_MODULE);
	//设置整行选中
	SendMessage(hListModule, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	//第1列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("模块路径");
	lv.cx = 720;
	lv.iSubItem = 0;
	SendMessage(hListModule, LVM_INSERTCOLUMN, 0, (DWORD)&lv);

	//第2列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("镜像基址");
	lv.cx = 100;
	lv.iSubItem = 1;
	SendMessage(hListModule, LVM_INSERTCOLUMN, 1, (DWORD)&lv);

	//第3列
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM ;
	lv.pszText = TEXT("镜像大小");
	lv.cx = 100;
	lv.iSubItem = 2;
	SendMessage(hListModule, LVM_INSERTCOLUMN, 2, (DWORD)&lv);
}

VOID EnumProcess(HWND hwnd)
{
	LV_ITEM lvItem;
	TCHAR* tcharBuff = new TCHAR[BUFFLENGTHMAX];
	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
	int iItem=0,iSubItem=0;
	 TCHAR szModName[MAX_PATH];
	//遍历windows 所有进程， 得到所有进程信息
	DWORD aProcesses[MAXPROCESS], cbNeeded, cProcesses;
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
						_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("%s"), szModName);
						lvItem.pszText = tcharBuff;
						lvItem.iItem = iItem;
						lvItem.iSubItem = iSubItem++;
						ListView_InsertItem(hwnd, &lvItem);
						//SendMessage(hwnd, LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

						memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
						memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT;
						_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("%d"), processID);
						lvItem.pszText = tcharBuff;
						lvItem.iItem = iItem;
						lvItem.iSubItem = iSubItem++;
						ListView_SetItem(hwnd, &lvItem);
						//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

						memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
						memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT;
						_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), (DWORD)hMod);
						lvItem.pszText = tcharBuff;
						lvItem.iItem = iItem;
						lvItem.iSubItem = iSubItem++;
						ListView_SetItem(hwnd, &lvItem);
						//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

						memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
						memset(&lvItem, 0, sizeof(LV_ITEM));
						lvItem.mask = LVIF_TEXT;
						_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), imageSize);
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
					_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("%s"), szProcessName_system);
					lvItem.pszText = tcharBuff;
					lvItem.iItem = iItem;
					lvItem.iSubItem = iSubItem++;
					ListView_InsertItem(hwnd, &lvItem);
					//SendMessage(hwnd, LVM_INSERTITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

					memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
					memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT;
					_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("%d"), processID);
					lvItem.pszText = tcharBuff;
					lvItem.iItem = iItem;
					lvItem.iSubItem = iSubItem++;
					ListView_SetItem(hwnd, &lvItem);
					//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

					memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
					memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT;
					_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), (DWORD)0);
					lvItem.pszText = tcharBuff;
					lvItem.iItem = iItem;
					lvItem.iSubItem = iSubItem++;
					ListView_SetItem(hwnd, &lvItem);
					//SendMessage(hwnd, LVM_SETITEM, 0, (LPARAM)(const LV_ITEM *)(&lvItem));

					memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
					memset(&lvItem, 0, sizeof(LV_ITEM));
					lvItem.mask = LVIF_TEXT;
					_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), (DWORD)0);
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
	// 读取DOS头 找到e_lfanew
	readNumber = fread_s(fileBufferDos, DOSHEADERSIZE, DOSHEADERSIZE, 1, file);
	if(readNumber != 1)
		return 0;
	pDOSHeader = (IMAGE_DOS_HEADER*)fileBufferDos;
	// 根据e_lfanew 找到NT头位置
	fseek(file, pDOSHeader->e_lfanew, SEEK_SET);
	// 读取NT头
	readNumber = fread_s(fileBufferNt, NTHEADERSIZE, NTHEADERSIZE, 1, file);
	if(readNumber != 1)
		return 0;
	pNTHeaders = (IMAGE_NT_HEADERS*)fileBufferNt;
	fclose(file);
	// 返回NT头中OptionalHeader.SizeofImage
	return pNTHeaders->OptionalHeader.SizeOfImage;
}

VOID EnumModules(HWND hListProcess, WPARAM wparam, LPARAM lparam)
{
	DWORD dwRowId;
	LV_ITEM lvItem;
	BOOL bRet = FALSE;
	DWORD processID = 0;
	HWND hMainDlg;
	HWND hListModule;

	TCHAR* tcharBuff = new TCHAR[BUFFLENGTHMAX];
	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);

	// 获取选中行
	//SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	dwRowId = ListView_GetNextItem(hListProcess, -1, LVNI_SELECTED);
	dbgPrintf(TEXT("dwRowId=%d\n"), dwRowId);
	if(dwRowId == -1)
	{
		dbgPrintf(TEXT("Error!\n"));
	}
	//获取选中行的PID
	lvItem.iItem = dwRowId; // the row 
	lvItem.iSubItem = 1; // the column: pid
	lvItem.mask =  LVIF_TEXT;
	lvItem.pszText = tcharBuff;
	lvItem.cchTextMax = BUFFLENGTHMAX;
	//(BOOL)SendMessage((hwnd), LVM_GETITEM, 0, (LPARAM)(LV_ITEM *)(pitem))
	bRet = ListView_GetItem(hListProcess, &lvItem);
	if(!bRet)
	{
		dbgPrintf(TEXT("get pid failed\n"));
		return ;
	}
	dbgPrintf(TEXT("Get PID: %s\n"), lvItem.pszText);

	_stscanf_s(lvItem.pszText,TEXT("%d"), &processID);

	hMainDlg = GetParent(hListProcess);
	hListModule = GetDlgItem(hMainDlg, IDC_LIST_MODULE);
	ListView_DeleteAllItems(hListModule);
	// 在 hListModule 显示 processID 对应的 modules
	ShowModules(hListModule, processID);

	delete[] tcharBuff;
	return ;
}

VOID ShowModules(HWND hListModule, DWORD processId)
{
	LV_ITEM lvItem;
	DWORD iItem=0, iSubItem=0;
	HMODULE hMods[MAXMODULES];
	DWORD cbNeeded;
	DWORD i,iMax;
	TCHAR szModName[MAX_PATH];
	TCHAR tcharBuff[BUFFLENGTHMAX];

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
		FALSE, processId ); //PROCESS_ALL_ACCESS
	
	if (NULL == hProcess || FALSE == EnumProcessModules( hProcess, hMods, sizeof(hMods), &cbNeeded) )
	{
		dbgPrintf(TEXT("Error EnumProcessModules\n"));
		return ;
	}
	iMax = cbNeeded/sizeof(DWORD);
	for( i=0; i<iMax; i++)
	{
		memset(szModName, 0, sizeof(szModName));
		if( GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName)/sizeof(TCHAR)) )
		{
			iSubItem = 0;
			DWORD imageSize = GetModuleSize(szModName);
			// now the informations: szModName, hMods[i], imageSize; 
						
			memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
			memset(&lvItem, 0, sizeof(LV_ITEM));
			_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("%s"), szModName);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = iItem;
			lvItem.iSubItem = iSubItem++;
			lvItem.pszText = tcharBuff;
			ListView_InsertItem(hListModule, &lvItem);

			memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
			memset(&lvItem, 0, sizeof(LV_ITEM));
			_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08x"), (DWORD)hMods[i]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = iItem;
			lvItem.iSubItem = iSubItem++;
			lvItem.pszText = tcharBuff;
			ListView_SetItem(hListModule, &lvItem);

			memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
			memset(&lvItem, 0, sizeof(LV_ITEM));
			_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08x"), imageSize);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = iItem;
			lvItem.iSubItem = iSubItem++;
			lvItem.pszText = tcharBuff;
			ListView_SetItem(hListModule, &lvItem);
		}
	}
	CloseHandle(hProcess);
	hProcess = NULL;
}