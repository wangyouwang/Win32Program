

#include "stdafx.h"
#include "debugTool.h"
#include <CommCtrl.h>
#pragma  comment(lib, "comctl32.lib")

#define BUFFLENGTHMAX 1024
#define MAXPROCESS 1024
#define MAXMODULES  1024


//全局变量 imageBase
#if 1
HINSTANCE hAppInstance = NULL;
HWND hwndMainDlg = NULL;
HWND hwndPeFileDlg = NULL;
TCHAR peFileName[MAX_PATH]=TEXT("\0");
BYTE* fileBuffer = NULL;
DWORD inFileSize = 0;
DWORD gDirectoryFlags = 0;

IMAGE_DOS_HEADER* pDosHeader = NULL;
IMAGE_NT_HEADERS* pImageNtHeader = NULL;
IMAGE_FILE_HEADER* pImageFileHeader = NULL;
IMAGE_OPTIONAL_HEADER32* pImageOptionHeader32 = NULL;
IMAGE_OPTIONAL_HEADER64* pImageOptionHeader64 = NULL;
IMAGE_SECTION_HEADER* pImageSectionHeaders=NULL;
// WORD dNumberOfSections = 0;  //  pImageFileHeader->NumberOfSections
// #define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16
#endif

//函数声明 
#if 1
VOID InitProcessListView(HWND hwnd);
VOID InitModuleListView(HWND hwnd);
VOID InitSectionsListView(HWND hwnd);
VOID InitDirectorys(HWND hwnd);
void initDialogDirectoryInfo(HWND hDialg);
VOID EnumProcess(HWND hwnd);
VOID EnumModules(HWND hListProcess, WPARAM wparam, LPARAM lparam);
VOID ShowModules(HWND hListModule, DWORD processId);
DWORD GetModuleSize(TCHAR* szModuleName);
void destroyPeFileStruct();
BOOL initPeFileStruct();
DWORD FOA2RVA(IMAGE_SECTION_HEADER* pImageSectionHeader, DWORD numOfSections, DWORD dFOA);
DWORD RVA2FOA(IMAGE_SECTION_HEADER* pImageSectionHeader, DWORD numOfSections, DWORD dRVA);
BOOL DlgGetPeFileName(HWND hwnd, TCHAR* fileName, DWORD NameBuffSize);
void fillBufferForExportDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, IMAGE_EXPORT_DIRECTORY* pImgExportDirectory);
void fillBufferForImportDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, IMAGE_IMPORT_DESCRIPTOR* pImgImportDirectory);
void fillBufferForIatDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, IMAGE_IMPORT_DESCRIPTOR* pImgIatDirectory);
void fillBuffForBoundImportDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, IMAGE_BOUND_IMPORT_DESCRIPTOR* pImgBoundImportDirectory);
void fillBufferForBaseRelocDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, IMAGE_BASE_RELOCATION* pImgBaseRelocDirectory);
void fillBufferForResourceDirectory(TCHAR*tcharBuff, DWORD dSizeInTchar, IMAGE_RESOURCE_DIRECTORY* pImgResourceDirectory);
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_PeFile(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_Sections(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  DialogProc_Directorys(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK  DialogProc_ShowInfo(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
// 入口函数，回调函数::C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include\winbase.h
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, 
				__in LPSTR lpCmdLine, __in int nShowCmd )
{
	//保存全局变量  imageBase
	hAppInstance = hInstance;

#if 1  // ListControl need 
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
		nRet = TRUE;
		break;
	case WM_CLOSE:  // 右上角的x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
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
		hwndMainDlg = hwnd;
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
#if 1
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_OPENPE:
			dbgPrintf(TEXT("WM_COMMAN: IDC_BUTTON_OPENPE wParam=%x lParam=%x\n"),wParam, lParam);
			// GetOpenFileName 得到文件路径
			if(TRUE == DlgGetPeFileName(hwnd, peFileName, MAX_PATH-1))
			{
				dbgPrintf(TEXT("select filepath: %s\n"), peFileName);
			// 显示对话窗，输出pe头信息
				//创建窗口
				DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_FILE), NULL, DialogProc_PeFile);
			}
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
			DestroyWindow(hwnd);
			nRet = TRUE;
			break;
		default:
			break;
		}
#endif
		break;
	case WM_SYSCOMMAND:
#if 1
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
#endif
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
	//DWORD ThreadId;
	//HANDLE hThread;
	//初始化
	memset(&lv, 0, sizeof(lv));
	//获取IDC_LIST_PROCESS句柄
	hListProcess = GetDlgItem(hwnd, IDC_LIST_PROCESS);
	//设置整行选中
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
#if 1
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
#endif
	//hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EnumProcess, hListProcess, 0, &ThreadId);
	//WaitForSingleObject(hThread, INFINITE);
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
#if 1
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
#endif
					}
				}
				else  // <system Idle Process>
				{
					DWORD imageSize = 0;
#if 1
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
#endif
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
#if 1
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
			_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), (DWORD)hMods[i]);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = iItem;
			lvItem.iSubItem = iSubItem++;
			lvItem.pszText = tcharBuff;
			ListView_SetItem(hListModule, &lvItem);

			memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
			memset(&lvItem, 0, sizeof(LV_ITEM));
			_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), imageSize);
			lvItem.mask = LVIF_TEXT;
			lvItem.iItem = iItem;
			lvItem.iSubItem = iSubItem++;
			lvItem.pszText = tcharBuff;
			ListView_SetItem(hListModule, &lvItem);
#endif
		}
	}
	CloseHandle(hProcess);
	hProcess = NULL;
}

BOOL DlgGetPeFileName(HWND hwnd, TCHAR* fileName, DWORD NameBuffSize)
{
	BOOL bRet = FALSE;
	OPENFILENAME ofn; 
	memset(&ofn, 0, sizeof(OPENFILENAME));


	ofn.hwndOwner = hwnd;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST; 
	ofn.lpstrFilter = TEXT("*.exe;*.dll;*.csr;*.drv;*.sys");
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = NameBuffSize;
	bRet = GetOpenFileName(&ofn);
	if(bRet != FALSE)
		bRet = TRUE;

	return bRet;
}

// DIALOG_FILE 消息回调函数  处理过的消息返回TRUE 否则返回FALSE
BOOL CALLBACK DialogProc_PeFile(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL nRet = FALSE;
	HWND hEditWnd = NULL;
	TCHAR tcharBuff[BUFFLENGTHMAX];
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		destroyPeFileStruct();
		nRet = TRUE;
		break;
	case WM_CLOSE:  // 右上角的x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
		nRet = TRUE;
		break;
	case WM_INITDIALOG:
		hwndPeFileDlg = hwnd;
		SetWindowPos(hwnd, NULL, 100, 100, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		// init PE file buffer and image buffer 
		if(TRUE == initPeFileStruct())
		{
#if 1
			if(pImageOptionHeader32 != NULL)
			{
				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->AddressOfEntryPoint);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT1);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->ImageBase);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT2);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->SizeOfImage);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT3);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->BaseOfCode);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT4);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->BaseOfData);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT5);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->SectionAlignment);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT6);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->FileAlignment);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT7);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->LoaderFlags);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT8);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->Subsystem);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT9);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageFileHeader->NumberOfSections);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT10);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageFileHeader->TimeDateStamp);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT11);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->SizeOfHeaders);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT12);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%04X"), pImageFileHeader->Characteristics);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT13);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader32->CheckSum);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT14);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%04X"), pImageFileHeader->SizeOfOptionalHeader);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT15);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%04X"), pImageOptionHeader32->NumberOfRvaAndSizes);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT16);
				Edit_SetText(hEditWnd, tcharBuff);
			}
			else if(pImageOptionHeader64 != NULL)
			{
				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->AddressOfEntryPoint);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT1);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%I64x"), pImageOptionHeader64->ImageBase);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT2);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->SizeOfImage);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT3);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->BaseOfCode);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT4);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("%x"), 0);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT5);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->SectionAlignment);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT6);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->FileAlignment);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT7);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->LoaderFlags);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT8);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->Subsystem);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT9);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageFileHeader->NumberOfSections);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT10);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageFileHeader->TimeDateStamp);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT11);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->SizeOfHeaders);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT12);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%04X"), pImageFileHeader->Characteristics);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT13);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%08X"), pImageOptionHeader64->CheckSum);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT14);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%04X"), pImageFileHeader->SizeOfOptionalHeader);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT15);
				Edit_SetText(hEditWnd, tcharBuff);

				_stprintf_s(tcharBuff, BUFFLENGTHMAX, 
					TEXT("0x%04X"), pImageOptionHeader64->NumberOfRvaAndSizes);
				hEditWnd = GetDlgItem(hwnd, IDC_EDIT16);
				Edit_SetText(hEditWnd, tcharBuff);
			}
#endif
		}
		nRet = TRUE;
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_PEDLG_CLOSE:
			DestroyWindow(hwnd);
			nRet = TRUE;
			break;
		case IDC_BUTTON_SECTIONS:
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_SECTIONS), NULL, DialogProc_Sections);
			nRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY:
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_PE_DIRECTORYS), NULL, DialogProc_Directorys);
			nRet = TRUE;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return nRet;
}


BOOL initPeFileStruct()
{
	FILE* inFile = NULL;
	errno_t err = 0;
	DWORD nRet;
	BOOL bRet = FALSE;
	// 打开文件 获取大小 申请空间 加载到内存
#if 1
	{
		err = _tfopen_s(&inFile, peFileName, TEXT("rb"));
		if( err!=0 )
		{
			dbgPrintf(TEXT("err = %d\n"),err);
			return bRet;
		}
		assert(inFile!=NULL);
		err = fseek(inFile, 0, SEEK_END); assert(err==0);
		inFileSize = ftell(inFile); assert(inFileSize!=0);

		fileBuffer = new BYTE[inFileSize];assert(fileBuffer!=NULL);	
		err = fseek(inFile, 0, SEEK_SET);assert(err==0);
		nRet = fread(fileBuffer, sizeof(BYTE), inFileSize, inFile);
		if(nRet != inFileSize)
		{
			dbgPrintf(TEXT("read file error!\n"));
			return bRet;
		}
		else
		{
			dbgPrintf(TEXT("read file ok! read 0x%x bytes"), inFileSize);
		}
		fclose(inFile);
		inFile = NULL;
	}
#endif

	//初始化几个头部指针  pDosHeader pImageNtHeader pImageFileHeader ? pImageOptionHeader32 : pImageOptionHeader64
	assert(inFileSize > sizeof(IMAGE_DOS_HEADER) );
	pDosHeader = (IMAGE_DOS_HEADER*)fileBuffer;
	pImageNtHeader =  (IMAGE_NT_HEADERS*)(fileBuffer + pDosHeader->e_lfanew);
	pImageFileHeader = &(pImageNtHeader->FileHeader);
	if( pImageFileHeader->SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER32) )
	{
		pImageOptionHeader32 = &(pImageNtHeader->OptionalHeader);
		dbgPrintf(TEXT("read file PE in x32 format!\n"));
		bRet = TRUE;
	}
	else if( pImageFileHeader->SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER64) )
	{
		pImageOptionHeader64 = (IMAGE_OPTIONAL_HEADER64*)&(pImageNtHeader->OptionalHeader);
		dbgPrintf(TEXT("read file PE in x64 format!\n"));
		bRet = TRUE;
	}
	else
	{// IMAGE_FIRST_SECTION doesn't need 32/64 versions since the file header is the same either way.
		dbgPrintf(TEXT("read file wasn't PE format!\n"));
		return bRet;
	}
	pImageSectionHeaders = IMAGE_FIRST_SECTION(pImageNtHeader);
	return bRet;
}

void destroyPeFileStruct()
{
	inFileSize = 0;
	if(fileBuffer)
	{
		delete[] fileBuffer;
		fileBuffer = NULL;
	}
	pDosHeader = NULL;
	pImageNtHeader =  NULL;
	pImageFileHeader = NULL;
	pImageOptionHeader32 = NULL;
	pImageOptionHeader64 = NULL;
}

//
// DIALOG_SECTIONS 消息回调函数  处理过的消息返回TRUE 否则返回FALSE
BOOL CALLBACK DialogProc_Sections(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL nRet = FALSE;
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		nRet = TRUE;
		break;
	case WM_CLOSE:  // 右上角的x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
		nRet = TRUE;
		break;
	case WM_INITDIALOG:
		SetWindowPos(hwnd, NULL, 100, 100, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		InitSectionsListView(hwnd);
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_SECTIONS_EXIT:
			DestroyWindow(hwnd);
			nRet = TRUE;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	return nRet;
}

VOID InitSectionsListView(HWND hwnd)
{
	LV_COLUMN lv;
	LV_ITEM lvItem;	
	TCHAR tcharBuff[BUFFLENGTHMAX];
	BYTE Name[IMAGE_SIZEOF_SHORT_NAME+1];
	HWND hListSections;
	TCHAR listSectionsNames[6][MAX_PATH]={
		TEXT("名称"),TEXT("虚拟大小"),TEXT("虚拟偏移"),
		TEXT("RAW大小"),TEXT("RAW偏移"),TEXT("特征值") };
	DWORD iSubitem = 0;
	WORD dNumberOfSections = pImageFileHeader->NumberOfSections;
	//初始化
	memset(Name, 0, IMAGE_SIZEOF_SHORT_NAME+1);
	memset(&lv, 0, sizeof(lv));
	//获取IDC_LIST_PROCESS句柄
	hListSections = GetDlgItem(hwnd, IDC_LIST_SECTIONS);
	//设置整行选中
	SendMessage(hListSections, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

	for(int i=0; i<6; i++)
	{
		lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		lv.pszText = listSectionsNames[i];
		lv.cx = 90;
		lv.iSubItem = i;	
		SendMessage(hListSections, LVM_INSERTCOLUMN, i, (DWORD)&lv); 
		// ListView_InsertColumn(hListSections, 0, (DWORD)&lv)
	}

	if( pImageSectionHeaders==NULL )
		return;

	//显示 Sections 到 hListSections
 	for(int i=0; i<dNumberOfSections; i++)
 	{
#if 1
 		int j = 0; //TEXT("名称"),
 		memset(tcharBuff, 0, sizeof(tcharBuff));
 		memset(&lvItem, 0, sizeof(lvItem));
 		memcpy_s(Name, sizeof(Name), pImageSectionHeaders[i].Name, IMAGE_SIZEOF_SHORT_NAME);
 		CharToTchar((const char*)Name, tcharBuff);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_InsertItem(hListSections, &lvItem);
 
 		j++; // TEXT("虚拟大小"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0%08X"), pImageSectionHeaders[i].Misc.VirtualSize);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 
 		j++; // TEXT("虚拟偏移"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), pImageSectionHeaders[i].VirtualAddress);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 		
 
 		j++; // TEXT("RAW大小"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), pImageSectionHeaders[i].SizeOfRawData);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 
 		j++; // TEXT("RAW偏移"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), pImageSectionHeaders[i].PointerToRawData);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 
 		j++; // TEXT("特征值")
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), pImageSectionHeaders[i].Characteristics);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
#endif
 	}
}

//#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16
DWORD IDC_EDIT_RVA_DIRECTORYS[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]= \
	{ IDC_EDIT_DIRECTORY_1, IDC_EDIT_DIRECTORY_3, IDC_EDIT_DIRECTORY_5, IDC_EDIT_DIRECTORY_7,
	  IDC_EDIT_DIRECTORY_9, IDC_EDIT_DIRECTORY_11, IDC_EDIT_DIRECTORY_13, IDC_EDIT_DIRECTORY_15,
	  IDC_EDIT_DIRECTORY_17, IDC_EDIT_DIRECTORY_19, IDC_EDIT_DIRECTORY_21, IDC_EDIT_DIRECTORY_23,
	  IDC_EDIT_DIRECTORY_25, IDC_EDIT_DIRECTORY_27, IDC_EDIT_DIRECTORY_29, IDC_EDIT_DIRECTORY_31 };
DWORD IDC_EDIT_SIZE_DIRECTORYS[IMAGE_NUMBEROF_DIRECTORY_ENTRIES]= \
	{ IDC_EDIT_DIRECTORY_2, IDC_EDIT_DIRECTORY_4, IDC_EDIT_DIRECTORY_6, IDC_EDIT_DIRECTORY_8,
	  IDC_EDIT_DIRECTORY_10, IDC_EDIT_DIRECTORY_12, IDC_EDIT_DIRECTORY_14, IDC_EDIT_DIRECTORY_16,
	  IDC_EDIT_DIRECTORY_18, IDC_EDIT_DIRECTORY_20, IDC_EDIT_DIRECTORY_22, IDC_EDIT_DIRECTORY_24,
	  IDC_EDIT_DIRECTORY_26, IDC_EDIT_DIRECTORY_28, IDC_EDIT_DIRECTORY_30, IDC_EDIT_DIRECTORY_32 };

// DIALOG_DIRECTORY 消息回调函数  处理过的消息返回TRUE 否则返回FALSE
BOOL CALLBACK  DialogProc_Directorys(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		bRet = TRUE;
		break;
	case WM_CLOSE:  // 右上角的x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
		bRet = TRUE;
		break;
	case WM_INITDIALOG:
		SetWindowPos(hwnd, NULL, 100, 100, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		InitDirectorys(hwnd);
		bRet = TRUE;
		break;
	case WM_COMMAND:
		switch( LOWORD(wParam) )
		{
		case IDC_BUTTON_DIRECTORY_EXPORT:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_EXPORT;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
#if 1
		case IDC_BUTTON_DIRECTORY_IMPORT:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_IMPORT;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_RESOURCE:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_RESOURCE;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_EXCEPTION:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_EXCEPTION;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_BASERELOC:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_BASERELOC;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_DEBUG:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_DEBUG;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_TLS:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_TLS;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_LOAD_CONFIG:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_LOAD_CONFIG;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_BOUND_IMPORT:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_BOUND_IMPORT;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
		case IDC_BUTTON_DIRECTORY_IAT:
			gDirectoryFlags = IDC_BUTTON_DIRECTORY_IAT;
			DialogBox(hAppInstance, MAKEINTRESOURCE(IDD_DIALOG_DIRECTORY_INFO), NULL, DialogProc_ShowInfo);
			bRet = TRUE;
			break;
#endif
		default:
			break;
		}
		break;
	default:
		break;
	}
	return bRet;
}

VOID InitDirectorys(HWND hwnd)
{
	TCHAR tcharBuff[BUFFLENGTHMAX];
	DWORD iRva=0, iSize=0;
	IMAGE_DATA_DIRECTORY* ptrDataDirectory=NULL;
	DWORD number_directorys = IMAGE_NUMBEROF_DIRECTORY_ENTRIES;
	HWND hEditWnd=NULL;
	if( pImageOptionHeader32 != NULL )
	{
		number_directorys = pImageOptionHeader32->NumberOfRvaAndSizes;
		ptrDataDirectory = pImageOptionHeader32->DataDirectory;
	}
	else if( pImageOptionHeader64 != NULL )
	{
		number_directorys = pImageOptionHeader64->NumberOfRvaAndSizes;
		ptrDataDirectory = pImageOptionHeader64->DataDirectory;
	}
	else
		return;

	dbgPrintf(TEXT("number_directorys=%d\n"), number_directorys);

	for(unsigned int i=0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
	{
		if(i < number_directorys)
		{
			iRva = ptrDataDirectory[i].VirtualAddress;
			iSize = ptrDataDirectory[i].Size;
		}
		else
		{
			iRva=0; 
			iSize=0;
		}

		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), iRva);
		hEditWnd = GetDlgItem(hwnd, IDC_EDIT_RVA_DIRECTORYS[i]);
		Edit_SetText(hEditWnd, tcharBuff);

		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), iSize);
		hEditWnd = GetDlgItem(hwnd, IDC_EDIT_SIZE_DIRECTORYS[i]);
		Edit_SetText(hEditWnd, tcharBuff);
	}
}


// DIALOG_DIRECTORY 消息回调函数  处理过的消息返回TRUE 否则返回FALSE
BOOL CALLBACK  DialogProc_ShowInfo(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		bRet = TRUE;
		break;
	case WM_CLOSE:  // 右上角的x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
		bRet = TRUE;
		break;
	case WM_INITDIALOG:
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		SetWindowPos(hwnd, NULL, 100, 100, 0, 0, SWP_SHOWWINDOW|SWP_NOSIZE);
		initDialogDirectoryInfo(hwnd);
		bRet = TRUE;
		break;
	default:
		break;
	}
	return bRet;
}

void initDialogDirectoryInfo(HWND hDialg)
{
	IMAGE_DATA_DIRECTORY* pImageDataDirectory = NULL;
	if( pImageOptionHeader32!=NULL )
		pImageDataDirectory = pImageOptionHeader32->DataDirectory;
	else if( pImageOptionHeader64 != NULL )
		pImageDataDirectory = pImageOptionHeader64->DataDirectory;
	assert(pImageDataDirectory);
	DWORD dFoa = 0;
	DWORD dRva = 0;
	//导出表
	dRva = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress; // 0
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRva);
	IMAGE_EXPORT_DIRECTORY* pImgExportDirectory = (IMAGE_EXPORT_DIRECTORY*)((BYTE*)pDosHeader + dFoa);
	//导入表
	dRva = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress; // 1
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRva);
	IMAGE_IMPORT_DESCRIPTOR* pImgImportDirectory = (IMAGE_IMPORT_DESCRIPTOR*)((BYTE*)pDosHeader + dFoa);

	//IAT表  Import Address Tables  必须用 ImportDirectory才能知道 IAT有多少个模块。。。。 
	//dRva = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress; // 12
	//dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRva);
	// 是32 还是 64位 暂时先不确定，在具体函数中再进行判断，否则还得定义不同的函数
	//IMAGE_THUNK_DATA* pImgIatDirectory = (IMAGE_THUNK_DATA*)((BYTE*)pDosHeader + dFoa);

	// 绑定导入表 Bound Import Table
	dRva = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT].VirtualAddress; // 11
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRva);
	IMAGE_BOUND_IMPORT_DESCRIPTOR* pImgBoundImportDirectory = (IMAGE_BOUND_IMPORT_DESCRIPTOR*)((BYTE*)pDosHeader + dFoa);

	// 重定位表 Base Relocation Table //IMAGE_DIRECTORY_ENTRY_BASERELOC
	dRva = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress; // 5
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRva);
	IMAGE_BASE_RELOCATION* pImgBaseRelocDirectory = (IMAGE_BASE_RELOCATION*)((BYTE*)pDosHeader + dFoa);

	// 资源表 IMAGE_RESOURCE_DIRECTORY  IMAGE_RESOURCE_DIRECTORY_ENTRY  IMAGE_RESOURCE_DIR_STRING_U IMAGE_RESOURCE_DATA_ENTRY
	dRva = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress; //2
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRva);
	IMAGE_RESOURCE_DIRECTORY* pImgResourceDirectory=(IMAGE_RESOURCE_DIRECTORY*)((BYTE*)pDosHeader + dFoa);

	HWND hStaticTitle = GetDlgItem(hDialg, IDC_STATIC_DIRECTORY_TITLE);
	HWND hEditContent = GetDlgItem(hDialg, IDC_EDIT_DIRECTORY_CONTENT);
	TCHAR tcharBuff[BUFFLENGTHMAX*100] = TEXT("\0");  //超长了怎么办？？？？

	switch (gDirectoryFlags)
	{
	case IDC_BUTTON_DIRECTORY_EXPORT:
		Static_SetText(hStaticTitle, TEXT("Export Table Info:"));
		fillBufferForExportDirectory(tcharBuff, sizeof(tcharBuff)/sizeof(TCHAR), pImgExportDirectory);
		Edit_SetText(hEditContent, tcharBuff);
		break;
	case IDC_BUTTON_DIRECTORY_IMPORT:
		Static_SetText(hStaticTitle, TEXT("Import Table Info:"));
		fillBufferForImportDirectory(tcharBuff, sizeof(tcharBuff)/sizeof(TCHAR), pImgImportDirectory);
		Edit_SetText(hEditContent, tcharBuff);
		break;
	case IDC_BUTTON_DIRECTORY_RESOURCE:
		Static_SetText(hStaticTitle, TEXT("Resource Table Info:"));
		fillBufferForResourceDirectory(tcharBuff, sizeof(tcharBuff)/sizeof(TCHAR), pImgResourceDirectory);
		Edit_SetText(hEditContent, tcharBuff);
		break;
#if 1
	case IDC_BUTTON_DIRECTORY_BASERELOC:
		Static_SetText(hStaticTitle, TEXT("Base Relocation Table Info:"));
		fillBufferForBaseRelocDirectory(tcharBuff, sizeof(tcharBuff)/sizeof(TCHAR), pImgBaseRelocDirectory);
		Edit_SetText(hEditContent, tcharBuff);
		break;
	case IDC_BUTTON_DIRECTORY_BOUND_IMPORT:
		Static_SetText(hStaticTitle, TEXT("Bound Import Table Info:"));
		fillBuffForBoundImportDirectory(tcharBuff, sizeof(tcharBuff)/sizeof(TCHAR), pImgBoundImportDirectory);
		Edit_SetText(hEditContent, tcharBuff);
		break;
	case IDC_BUTTON_DIRECTORY_IAT:
		Static_SetText(hStaticTitle, TEXT("IAT Table Info:"));
		fillBufferForIatDirectory(tcharBuff, sizeof(tcharBuff)/sizeof(TCHAR), pImgImportDirectory);
		Edit_SetText(hEditContent, tcharBuff);
		break;
#endif
	default:
		break;
	}
	return ;
}
// 预定义的资源类型  
TCHAR resourceID2Name[24][32]={
	TEXT("RT_CURSOR"),TEXT("RT_BITMAP"),TEXT("RT_ICON"),TEXT("RT_MENU"),
	TEXT("RT_DIALOG"),TEXT("RT_STRING"),TEXT("RT_FONTDIR"),TEXT("RT_FONT"),
	TEXT("RT_ACCELERATOR"),TEXT("RT_RCDATA"),TEXT("RT_MESSAGETABLE"),TEXT("RT_GROUP_CURSOR"),
	TEXT("13"),TEXT("RT_GROUP_ICON"),TEXT("15"),TEXT("RT_VERSION"),
	TEXT("RT_DLGINCLUDE"),TEXT("18"),TEXT("19"),TEXT("20"),
	TEXT("21"),TEXT("22"),TEXT("23"),TEXT("RT_MANIFEST")
};



// 打印 资源表信息 到 tcharBuff
void fillBufferForResourceDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, IMAGE_RESOURCE_DIRECTORY* pImgResourceDirectory)
{
	// 变量
	DWORD dRVA=0,dFOA=0;
	TCHAR* pTchar = 0;
	BYTE* pByte = 0;
	WORD* pWord = 0;

	TCHAR _tcharBuff[BUFFLENGTHMAX];
	IMAGE_RESOURCE_DIRECTORY* ptrDirectoryLv1 = NULL;
	IMAGE_RESOURCE_DIRECTORY* ptrDirectoryLv2 = NULL;
	IMAGE_RESOURCE_DIRECTORY* ptrDirectoryLv3 = NULL;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* ptrDirectoryEntrysLv1 = NULL;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* ptrDirectoryEntrysLv2 = NULL;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* ptrDirectoryEntrysLv3 = NULL;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* ptrDirectoryEntryLv1=NULL;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* ptrDirectoryEntryLv2=NULL;
	IMAGE_RESOURCE_DIRECTORY_ENTRY* ptrDirectoryEntryLv3=NULL;
	IMAGE_RESOURCE_DIR_STRING_U* ptrStringU = NULL;
	IMAGE_RESOURCE_DATA_ENTRY* ptrDataEntry = NULL;
#if 1 
	if((BYTE*)pImgResourceDirectory == (BYTE*)pDosHeader)
	{
		_tcscat_s(tcharBuff, dSizeInTchar, TEXT("Resource Table is Empty!!\r\n"));
		return ;
	}
#endif

//一级目录 打印目录头
	ptrDirectoryLv1 = pImgResourceDirectory;
	_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|Characteristics:%08X\tTimeDateStamp:%08X\tMajorVersion:%04X\tMinorVersion:%04X\t")
		TEXT("NumberOfNamedEntries:%d\tNumberOfIdEntries:%d\r\n"), 
		ptrDirectoryLv1->Characteristics,ptrDirectoryLv1->TimeDateStamp,ptrDirectoryLv1->MajorVersion,
		ptrDirectoryLv1->MinorVersion,ptrDirectoryLv1->NumberOfNamedEntries,ptrDirectoryLv1->NumberOfIdEntries);
	_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);

	ptrDirectoryEntrysLv1 = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)ptrDirectoryLv1 + sizeof(IMAGE_RESOURCE_DIRECTORY));
	for( int i = 0; i < ptrDirectoryLv1->NumberOfNamedEntries + ptrDirectoryLv1->NumberOfIdEntries; i++ )
	{
		//一级目录 打印目录项
		ptrDirectoryEntryLv1 = &(ptrDirectoryEntrysLv1[i]);
#if 1
		//DUMMYUNIONNAME
		if( ptrDirectoryEntryLv1->NameIsString ) //
		{
			ptrStringU = (IMAGE_RESOURCE_DIR_STRING_U*)((BYTE*)pImgResourceDirectory + ptrDirectoryEntryLv1->NameOffset);
			pTchar = new TCHAR[ ptrStringU->Length + 1 ];
			memcpy_s( pTchar, ptrStringU->Length, ptrStringU->NameString, ptrStringU->Length);
			pTchar[ ptrStringU->Length ] = TCHAR(0);
			_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|Name:%s{%08X}\r\n"),  pTchar, ptrDirectoryEntryLv1->Name);
			delete[] pTchar;
			pTchar = NULL;
		}
		else
		{
			if( ptrDirectoryEntryLv1->Id <= 24 )
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|Id:%d{%s}\r\n"), ptrDirectoryEntryLv1->Id, 
				resourceID2Name[ptrDirectoryEntryLv1->Id-1] );
			else
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|Id:%d{-}\r\n"), ptrDirectoryEntryLv1->Id );
		}
		_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);
		//DUMMYUNIONNAME2
		if( ptrDirectoryEntryLv1->DataIsDirectory )
		{
			_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|DataIsDirectory:%d, OffsetToDirectory:%08X\r\n"), 
				ptrDirectoryEntryLv1->DataIsDirectory, ptrDirectoryEntryLv1->OffsetToDirectory );
		}
		else
		{	// should not in here for level 1 directory
			_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|OffsetToData:%08X\r\n"), 
				ptrDirectoryEntryLv1->OffsetToData);
		}
		_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);
#endif
//二级目录 打印目录头
		ptrDirectoryLv2 = (IMAGE_RESOURCE_DIRECTORY*)((BYTE*)pImgResourceDirectory + ptrDirectoryEntryLv1->OffsetToDirectory);
		_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|Characteristics:%08X\tTimeDateStamp:%08X\tMajorVersion:%04X\tMinorVersion:%04X\t")
			TEXT("NumberOfNamedEntries:%d\tNumberOfIdEntries:%d\r\n"), 
			ptrDirectoryLv2->Characteristics,ptrDirectoryLv2->TimeDateStamp,ptrDirectoryLv2->MajorVersion,
			ptrDirectoryLv2->MinorVersion,ptrDirectoryLv2->NumberOfNamedEntries,ptrDirectoryLv2->NumberOfIdEntries);
		_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);

		ptrDirectoryEntrysLv2 = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)ptrDirectoryLv2 + sizeof(IMAGE_RESOURCE_DIRECTORY));
		for( int j = 0; j < ptrDirectoryLv2->NumberOfNamedEntries + ptrDirectoryLv2->NumberOfIdEntries; j++ )
		{//二级目录 打印目录项
			ptrDirectoryEntryLv2 = &(ptrDirectoryEntrysLv2[j]);
#if 1
			//DUMMYUNIONNAME
			if( ptrDirectoryEntryLv2->NameIsString )
			{
				ptrStringU =  (IMAGE_RESOURCE_DIR_STRING_U*)((BYTE*)pImgResourceDirectory + ptrDirectoryEntryLv2->NameOffset);
				pTchar = new TCHAR[ ptrStringU->Length + 1 ];
				memcpy_s( pTchar, ptrStringU->Length, ptrStringU->NameString, ptrStringU->Length);
				pTchar[ ptrStringU->Length ] = TCHAR(0);
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|Name:%s{%08X}\r\n"),  pTchar, ptrDirectoryEntryLv2->Name);
				delete[] pTchar;
				pTchar = NULL;
			}
			else
			{
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|Id:%d\r\n"), ptrDirectoryEntryLv2->Id );
			}
			_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);
			//DUMMYUNIONNAME2
			if( ptrDirectoryEntryLv2->DataIsDirectory )
			{
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|DataIsDirectory:%d, OffsetToDirectory:%08X\r\n"), 
					ptrDirectoryEntryLv2->DataIsDirectory, ptrDirectoryEntryLv2->OffsetToDirectory );
			}
			else
			{	// should not in here for level 2 directory
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|OffsetToData:%08X\r\n"), 
					ptrDirectoryEntryLv1->OffsetToData);
			}
			_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);
#endif
//三级目录 打印目录头
			ptrDirectoryLv3 = (IMAGE_RESOURCE_DIRECTORY*)((BYTE*)pImgResourceDirectory + ptrDirectoryEntryLv2->OffsetToDirectory);
			_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|\t|Characteristics:%08X\tTimeDateStamp:%08X\tMajorVersion:%04X\tMinorVersion:%04X\t")
				TEXT("NumberOfNamedEntries:%d\tNumberOfIdEntries:%d\r\n"), 
				ptrDirectoryLv3->Characteristics,ptrDirectoryLv3->TimeDateStamp,ptrDirectoryLv3->MajorVersion,
				ptrDirectoryLv3->MinorVersion,ptrDirectoryLv3->NumberOfNamedEntries,ptrDirectoryLv3->NumberOfIdEntries);
			_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);

			ptrDirectoryEntrysLv3 = (IMAGE_RESOURCE_DIRECTORY_ENTRY*)((BYTE*)ptrDirectoryLv3 + sizeof(IMAGE_RESOURCE_DIRECTORY));
			for( int k = 0; k < ptrDirectoryLv3->NumberOfNamedEntries + ptrDirectoryLv3->NumberOfIdEntries; k++ )
			{//三级目录 打印目录项
				ptrDirectoryEntryLv3 = &(ptrDirectoryEntrysLv3[k]);
#if 1
				//DUMMYUNIONNAME
				if( ptrDirectoryEntryLv3->NameIsString )
				{
					ptrStringU =  (IMAGE_RESOURCE_DIR_STRING_U*)((BYTE*)pImgResourceDirectory + ptrDirectoryEntryLv3->NameOffset);
					pTchar = new TCHAR[ ptrStringU->Length +1 ];
					memcpy_s(pTchar, ptrStringU->Length, ptrStringU->NameString, ptrStringU->Length);
					pTchar[ ptrStringU->Length ] = TCHAR(0);
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|\t|Name:%s{%08X}\r\n"), pTchar, ptrDirectoryEntryLv3->Name);
					delete[] pTchar;
					pTchar = NULL;
				}
				else
				{
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|\t|Id:%d\r\n"), ptrDirectoryEntryLv3->Id );
				}
				_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);

				//DUMMYUNIONNAME2
				if( ptrDirectoryEntryLv3->DataIsDirectory )
				{// should not in here for level 3 directory
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|\t|DataIsDirectory:%d, OffsetToDirectory:%08X\r\n"), 
						ptrDirectoryEntryLv3->DataIsDirectory, ptrDirectoryEntryLv3->OffsetToDirectory );
				}
				else
				{	
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("|\t|\t|OffsetToData:%08X\r\n"), 
						ptrDirectoryEntryLv3->OffsetToData);
				}
				_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);
#endif
				//三级目录指向的数据结构 IMAGE_RESOURCE_DATA_ENTRY				
				ptrDataEntry = (IMAGE_RESOURCE_DATA_ENTRY*)((BYTE*)pImgResourceDirectory + ptrDirectoryEntryLv3->OffsetToData);
				//数据项  ptrDataEntry->OffsetToData 是RVA....
				dRVA = ptrDataEntry->OffsetToData;
				dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, 
					TEXT("|\t|\t|\t| FOA:%08X OffsetToData(RVA):%08X Size:%08X CodePage:%08X Reserved:%08X\r\n"),
					dFOA, ptrDataEntry->OffsetToData, ptrDataEntry->Size, ptrDataEntry->CodePage, ptrDataEntry->Reserved);
				_tcscat_s(tcharBuff, dSizeInTchar, _tcharBuff);
			}
		}

		_tcscat_s(tcharBuff, dSizeInTchar, TEXT("\r\n"));
	}
	return ;
}

// 打印 重定位表信息 到 tcharBuff
void fillBufferForBaseRelocDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, IMAGE_BASE_RELOCATION* pImgBaseRelocDirectory)
{	
	// 变量
	//DWORD dRVA=0,dFOA=0;
	BYTE* pByte = 0;
	WORD* pWord = 0;
	TCHAR _tcharBuff[BUFFLENGTHMAX];
	IMAGE_BASE_RELOCATION* pTmpBaseRelocDirectory = pImgBaseRelocDirectory;

	if((BYTE*)pTmpBaseRelocDirectory == (BYTE*)pDosHeader)
	{
		_tcscat_s(tcharBuff, dSizeInTchar, TEXT("Base Relocation Table is Empty!!\r\n"));
		return ;
	}
	TCHAR BASETYPE_MASK[11][16]={	TEXT("ABSOLUTE:0"),TEXT("HIGH:1"),TEXT("LOW:2"),TEXT("HIGHLOW:3"),TEXT("HIGHADJ:4"),
									TEXT("MIPS_JMPADDR:5"),TEXT("-6"),TEXT("-7"),TEXT("-8"),TEXT("MIPS16/IA64:9"),
									TEXT("DIR64")};

	// 外循环
	while(1)
	{
		if(pImgBaseRelocDirectory->VirtualAddress == 0)
			break;

		// 打印 IMAGE_BASE_RELOCATION 信息
		//pImgBaseRelocDirectory
		int NumBlock = (pImgBaseRelocDirectory->SizeOfBlock - 8) / 2;
		_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("VirtualAddress: %08X\r\nSizeOfBlock:%08X\tNumBlock:%dD\r\n"), 
			pImgBaseRelocDirectory->VirtualAddress, pImgBaseRelocDirectory->SizeOfBlock, NumBlock);
		_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );

		// 内循环
		
		pWord = (WORD*) (((BYTE*)pImgBaseRelocDirectory) + sizeof(IMAGE_BASE_RELOCATION));
		for(int i=0; i <NumBlock; i++)
		{
			 //pWord[i]  打印高4位，低12位
			BYTE BASETYPE = (BYTE)(pWord[i]>>12);
			WORD offset = pWord[i] & 0X0FFF;
			_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("BASETYPE: %s\toffset:%08X\r\n"), 
				BASETYPE_MASK[BASETYPE], offset);
			_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );
		}
		pImgBaseRelocDirectory = (IMAGE_BASE_RELOCATION*)((BYTE*)pImgBaseRelocDirectory + pImgBaseRelocDirectory->SizeOfBlock);
	}
}


// 打印 绑定导入表信息 到tcharBuff
void fillBuffForBoundImportDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar, 
	IMAGE_BOUND_IMPORT_DESCRIPTOR* pImgBoundImportDirectory)
{
	// 变量
	//DWORD dRVA=0,dFOA=0;
	BYTE* pByte=0;
	TCHAR _tcharBuff[BUFFLENGTHMAX];
	IMAGE_BOUND_IMPORT_DESCRIPTOR* ptrBoundImport = pImgBoundImportDirectory;
	IMAGE_BOUND_IMPORT_DESCRIPTOR empty;
	memset(&empty,0,sizeof(empty));

	if( (BYTE*)pImgBoundImportDirectory == (BYTE*)pDosHeader )
	{
		_tcscat_s(tcharBuff, dSizeInTchar, TEXT("Bound Import Table is Empty!!\r\n"));
		return ;
	}

	// 外循环 
	while(1)
	{//		IMAGE_BOUND_IMPORT_DESCRIPTOR
			if( memcmp(ptrBoundImport, &empty, sizeof(IMAGE_BOUND_IMPORT_DESCRIPTOR) ) == 0 ) // 结尾
				break;

			//打印 ptrBoundImport 信息
			_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("TimeDataStamp:%08X\r\nOffsetModuleName:%04X("),
				ptrBoundImport->TimeDateStamp, ptrBoundImport->OffsetModuleName);
			_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );
			pByte = (BYTE*)pImgBoundImportDirectory + ptrBoundImport->OffsetModuleName;
			CharToTchar( (const char*)pByte, _tcharBuff );
			_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );  // DLL name
			_tcscat_s( tcharBuff, dSizeInTchar, TEXT(")\r\n") );
			_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("NumberOfModuleForwarderRefs:%d\r\n"), ptrBoundImport->NumberOfModuleForwarderRefs);
			_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );

			// 内循环
			for( int i=0; i<ptrBoundImport->NumberOfModuleForwarderRefs; i++)
			{
				ptrBoundImport++;

				//打印 ptrBoundImport 信息
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("TimeDataStamp:%08X\r\nOffsetModuleName:%04X("),
					ptrBoundImport->TimeDateStamp, ptrBoundImport->OffsetModuleName);
				_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );
				pByte = (BYTE*)pImgBoundImportDirectory + ptrBoundImport->OffsetModuleName;
				CharToTchar( (const char*)pByte, _tcharBuff );
				_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );  // DLL name
				_tcscat_s( tcharBuff, dSizeInTchar, TEXT(")\r\n") );
				_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("Reserved:%d\r\n"), ((IMAGE_BOUND_FORWARDER_REF*)ptrBoundImport)->Reserved);
				_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );

			}
			ptrBoundImport++;
	}
	return ;
}

// 打印 导入表信息 到tcharBuff
void fillBufferForImportDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar,
	IMAGE_IMPORT_DESCRIPTOR* pImgImportDirectory)
{
	// 变量
	DWORD dRVA=0,dFOA=0;
	BYTE* pByte=0;
	//TCHAR* pTchar;
	//BYTE charBuff[BUFFLENGTHMAX];
	TCHAR _tcharBuff[BUFFLENGTHMAX];
	IMAGE_IMPORT_BY_NAME* ptrImageImportByName=NULL;
	IMAGE_THUNK_DATA64* ptrTrunkData64;
	IMAGE_THUNK_DATA32* ptrTrunkData32;

	if((BYTE*)pImgImportDirectory == (BYTE*)pDosHeader)
	{
		_tcscat_s(tcharBuff, dSizeInTchar, TEXT("Import is Empty!!\r\n"));
		return ;
	}

	// 外循环 DLL
	while(1)
	{
		if( pImgImportDirectory->Characteristics == 0 ) // 0 for terminating null import descriptor
			break;		
		// 打印该DLL的名字
		dRVA = pImgImportDirectory->Name;
		dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
		pByte = (BYTE*) ( ((BYTE*)pDosHeader) + dFOA );
		CharToTchar( (const char*)pByte, _tcharBuff );
		_tcscat_s( tcharBuff, dSizeInTchar, TEXT("\r\nthe dll name is: "));
		_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );
		_tcscat_s( tcharBuff, dSizeInTchar, TEXT("\r\n") );

		_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("OriginalFirstTrunk: %08X\r\nFirstTrunk:%08X"), 
			pImgImportDirectory->OriginalFirstThunk, pImgImportDirectory->FirstThunk);
		_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );
		_tcscat_s( tcharBuff, dSizeInTchar, TEXT("\r\n") );
		if( pImageOptionHeader32 != NULL )
		{
			dRVA = pImgImportDirectory->OriginalFirstThunk;
			dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
			ptrTrunkData32 = (IMAGE_THUNK_DATA32*) ( ((BYTE*)pDosHeader) + dFOA );
			// 内循环 function names
			while(1)
			{
				if(ptrTrunkData32->u1.AddressOfData == 0)
					break;

				if( ptrTrunkData32->u1.AddressOfData & IMAGE_ORDINAL_FLAG32 )
				{     //按序号导入
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("ID:%06X"), (ptrTrunkData32->u1.AddressOfData & 0x0fff));
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));
				}
				else  //按名字导入
				{
					dRVA = ptrTrunkData32->u1.AddressOfData;
					dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
					ptrImageImportByName = (IMAGE_IMPORT_BY_NAME*) ( ((BYTE*)pDosHeader) + dFOA );
					// 打印 ptrImageImportByName->Hint(WORD) 和 ptrImageImportByName->Name(BYTE*)
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("Hint:%04X Name:"), ptrImageImportByName->Hint);
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					pByte =  (BYTE*)ptrImageImportByName->Name;
					CharToTchar( (const char*)pByte, _tcharBuff );
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));
				}
				ptrTrunkData32 = ptrTrunkData32 + 1;
				dbgPrintf(TEXT("__tcharBuff length: %x\n"), _tcsnlen(tcharBuff, dSizeInTchar*sizeof(TCHAR)));
			}
		}

#if 1
		else if(pImageOptionHeader64 != NULL)
		{
			dRVA = pImgImportDirectory->OriginalFirstThunk;
			dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
			ptrTrunkData64 = (IMAGE_THUNK_DATA64*) ( ((BYTE*)pDosHeader) + dFOA );
			// 
			while(1)
			{
				if(ptrTrunkData64->u1.AddressOfData == 0)
					break;
				if( ptrTrunkData64->u1.AddressOfData & IMAGE_ORDINAL_FLAG64 )
				{     //按序号导入
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("ID:%06x"), (DWORD)(ptrTrunkData64->u1.AddressOfData & 0x0fff));
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));
				}
				else  //按名字导入
				{
					dRVA = (DWORD)ptrTrunkData64->u1.AddressOfData;
					dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
					ptrImageImportByName = (IMAGE_IMPORT_BY_NAME*) ( ((BYTE*)pDosHeader) + dFOA );
					// 打印 ptrImageImportByName->Hint(WORD) 和 ptrImageImportByName->Name(BYTE*)
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("Hint:%04X Name:"), ptrImageImportByName->Hint);
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					pByte = (BYTE*)ptrImageImportByName->Name;
					CharToTchar( (const char*)pByte, _tcharBuff );
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));		
				}
				ptrTrunkData64 = ptrTrunkData64 + 1;
				dbgPrintf(TEXT("__tcharBuff length: %x\n"), _tcsnlen(tcharBuff, dSizeInTchar*sizeof(TCHAR)));
			}
		}
#endif

		pImgImportDirectory = pImgImportDirectory + 1;
	
	}

	return ;
}


// 打印 IAT表信息 到tcharBuff
void fillBufferForIatDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar,
	IMAGE_IMPORT_DESCRIPTOR* pImgImportDirectory)
{
	// 变量
	DWORD dRVA=0,dFOA=0;
	BYTE* pByte=0;
	//TCHAR* pTchar;
	//BYTE charBuff[BUFFLENGTHMAX];
	TCHAR _tcharBuff[BUFFLENGTHMAX];
	IMAGE_IMPORT_BY_NAME* ptrImageImportByName=NULL;
	IMAGE_THUNK_DATA64* ptrTrunkData64;
	IMAGE_THUNK_DATA32* ptrTrunkData32;

	if((BYTE*)pImgImportDirectory == (BYTE*)pDosHeader)
	{
		_tcscat_s(tcharBuff, dSizeInTchar, TEXT("IAT is Empty!!\r\n"));
		return ;
	}

	// 外循环 DLL
	while(1)
	{
		if( pImgImportDirectory->Characteristics == 0 ) // 0 for terminating null import descriptor
			break;
		// 打印该DLL的名字
		dRVA = pImgImportDirectory->Name;
		dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
		pByte = (BYTE*) ( ((BYTE*)pDosHeader) + dFOA );
		CharToTchar( (const char*)pByte, _tcharBuff );
		_tcscat_s( tcharBuff, dSizeInTchar, TEXT("\r\nthe dll name is: "));
		_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );
		_tcscat_s( tcharBuff, dSizeInTchar, TEXT("\r\n") );

		_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("OriginalFirstTrunk: %08X\r\nFirstTrunk:%08X"), 
			pImgImportDirectory->OriginalFirstThunk, pImgImportDirectory->FirstThunk);
		_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff );
		_tcscat_s( tcharBuff, dSizeInTchar, TEXT("\r\n") );
		if( pImageOptionHeader32 != NULL )
		{
			dRVA = pImgImportDirectory->FirstThunk;
			dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
			ptrTrunkData32 = (IMAGE_THUNK_DATA32*) ( ((BYTE*)pDosHeader) + dFOA );
			// 内循环 function names
			while(1)
			{
				if(ptrTrunkData32->u1.AddressOfData == 0)
					break;

				if( ptrTrunkData32->u1.AddressOfData & IMAGE_ORDINAL_FLAG32 )
				{     //按序号导入
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("ID:%06X"), (ptrTrunkData32->u1.AddressOfData & 0x0fff));
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));
				}
				else  //按名字导入
				{
					dRVA = ptrTrunkData32->u1.AddressOfData;
					dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
					ptrImageImportByName = (IMAGE_IMPORT_BY_NAME*) ( ((BYTE*)pDosHeader) + dFOA );
					// 打印 ptrImageImportByName->Hint(WORD) 和 ptrImageImportByName->Name(BYTE*)
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("Hint:%04X Name:"), ptrImageImportByName->Hint);
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					pByte = (BYTE*)ptrImageImportByName->Name;
					CharToTchar( (const char*)pByte, _tcharBuff );
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));
				}
				ptrTrunkData32 = ptrTrunkData32 + 1;
				dbgPrintf(TEXT("__tcharBuff length: %x\n"), _tcsnlen(tcharBuff, dSizeInTchar*sizeof(TCHAR)));
			}
		}

#if 1
		else if(pImageOptionHeader64 != NULL)
		{
			dRVA = pImgImportDirectory->FirstThunk;
			dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
			ptrTrunkData64 = (IMAGE_THUNK_DATA64*) ( ((BYTE*)pDosHeader) + dFOA );
			// 
			while(1)
			{
				if(ptrTrunkData64->u1.AddressOfData == 0)
					break;
				if( ptrTrunkData64->u1.AddressOfData & IMAGE_ORDINAL_FLAG64 )
				{     //按序号导入
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("ID:%06x"), (DWORD)(ptrTrunkData64->u1.AddressOfData & 0x0fff));
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));
				}
				else  //按名字导入
				{
					dRVA = (DWORD)ptrTrunkData64->u1.AddressOfData;
					dFOA = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, dRVA);
					ptrImageImportByName = (IMAGE_IMPORT_BY_NAME*) ( ((BYTE*)pDosHeader) + dFOA );
					// 打印 ptrImageImportByName->Hint(WORD) 和 ptrImageImportByName->Name(BYTE*)
					_stprintf_s(_tcharBuff, BUFFLENGTHMAX, TEXT("Hint:%04X Name:"), ptrImageImportByName->Hint);
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					pByte = (BYTE*)ptrImageImportByName->Name;
					CharToTchar( (const char*)pByte, _tcharBuff );
					_tcscat_s( tcharBuff, dSizeInTchar, _tcharBuff);
					_tcscat_s( tcharBuff, dSizeInTchar,  TEXT("\r\n"));		
				}
				ptrTrunkData64 = ptrTrunkData64 + 1;
				dbgPrintf(TEXT("__tcharBuff length: %x\n"), _tcsnlen(tcharBuff, dSizeInTchar*sizeof(TCHAR)));
			}
		}
#endif

		pImgImportDirectory = pImgImportDirectory + 1;

	}

	return ;
}

// 打印 导出表信息 到tcharBuff
void fillBufferForExportDirectory(TCHAR* tcharBuff, DWORD dSizeInTchar,
	IMAGE_EXPORT_DIRECTORY* pImgExportDirectory)
{
	TCHAR ptmpbuff[BUFFLENGTHMAX];
	DWORD dFoa = 0;
	BYTE* pStringAddress = 0;
	TCHAR* pTstringAddress = new TCHAR[BUFFLENGTHMAX];
	DWORD* pFunctionAddress = 0;
	DWORD* pNamesAddress = 0;
	WORD* pNamesIndexAddress = 0;

	if((BYTE*)pImgExportDirectory == (BYTE*)pDosHeader)
	{
		_tcscat_s(tcharBuff, dSizeInTchar, TEXT("Export is Empty!!\r\n"));
		delete[] pTstringAddress;
		return ;
	}
	
	assert(pTstringAddress!=NULL);
	//_stprintf_s(ptmp, TEXT("%s:%d"),)
	//_tcscat_s()
	// 打印 IMAGE_EXPORT_DIRECTORY 信息
	_stprintf_s(tcharBuff, dSizeInTchar, 
		TEXT(" Characteristics: %X\r\n TimeDateStamp:%X\r\n MajorVersion: %X\r\n MinorVersion: %X\r\n")\
		TEXT(" Name:%X\r\n Base:%X\r\n NumberOfFunctions:%d\r\n NumberOfNames:%d\r\n AddressOfFunctions:%X\r\n")\
		TEXT(" AddressOfNameOrdinals:%X\r\n AddressOfNames:%X\r\n"),
		pImgExportDirectory->Characteristics, pImgExportDirectory->TimeDateStamp,
		pImgExportDirectory->MajorVersion, pImgExportDirectory->MinorVersion,
		pImgExportDirectory->Name, pImgExportDirectory->Base,
		pImgExportDirectory->NumberOfFunctions, pImgExportDirectory->NumberOfNames, 
		pImgExportDirectory->AddressOfFunctions, pImgExportDirectory->AddressOfNameOrdinals,
		pImgExportDirectory->AddressOfNames);
	// 打印 DLL NAME
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, pImgExportDirectory->Name);
	pStringAddress = ((BYTE*)pDosHeader + dFoa);
	CharToTchar((const char*)pStringAddress, pTstringAddress);
	_stprintf_s(ptmpbuff, BUFFLENGTHMAX, TEXT("DllName: %s\r\n"), pTstringAddress);
	_tcscat_s(tcharBuff, dSizeInTchar, ptmpbuff);

#if 1
	// 打印函数地址 RVA， 一共有 %d 个(NumberOfFunctions)
	_stprintf_s(ptmpbuff, BUFFLENGTHMAX, TEXT("Export Function RVA: \r\n") );
	_tcscat_s(tcharBuff, dSizeInTchar, ptmpbuff);
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, pImgExportDirectory->AddressOfFunctions);
	pFunctionAddress = (DWORD*)((BYTE*)pDosHeader + dFoa);
	for( unsigned int i=0; i<pImgExportDirectory->NumberOfFunctions; i++ )
	{
		DWORD funRva = *(pFunctionAddress+i);
		if( funRva != 0)
		{
			_stprintf_s(ptmpbuff, BUFFLENGTHMAX, TEXT("FunctionRVA%d: (id=%d) %08X\r\n"), i, (i+pImgExportDirectory->Base), funRva );
			_tcscat_s(tcharBuff, dSizeInTchar, ptmpbuff);
		}
	}
#endif

#if 1
	// 打印函数名字 ， 一共有 %d 个（NumberOfNames）
	_stprintf_s(ptmpbuff, BUFFLENGTHMAX, TEXT("Export Function Names: \r\n") );
	_tcscat_s(tcharBuff, dSizeInTchar, ptmpbuff);
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, pImgExportDirectory->AddressOfNames);
	pNamesAddress = (DWORD*)((BYTE*)pDosHeader + dFoa);
	for( unsigned int i=0; i<pImgExportDirectory->NumberOfNames; i++ )
	{
		// pNameAddress的值为 RVA
		DWORD nameRva = pNamesAddress[i];
		DWORD nameFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, nameRva);
		pStringAddress = ((BYTE*)pDosHeader + nameFoa);
		CharToTchar((const char*)pStringAddress, pTstringAddress);
		_stprintf_s(ptmpbuff, BUFFLENGTHMAX, TEXT("FunctionName%d: %s\r\n"), i, pTstringAddress);
		_tcscat_s(tcharBuff, dSizeInTchar, ptmpbuff);
	}
#endif

#if 1
	// 打印函数名字序号  ！！！2字节！！！
	_stprintf_s(ptmpbuff, BUFFLENGTHMAX, TEXT("Export Function Ordinals: \r\n") );
	_tcscat_s(tcharBuff, dSizeInTchar, ptmpbuff);
	dFoa = RVA2FOA(pImageSectionHeaders, pImageFileHeader->NumberOfSections, pImgExportDirectory->AddressOfNameOrdinals);
	pNamesIndexAddress = (WORD*)((BYTE*)pDosHeader + dFoa);
	for( unsigned int i=0; i<pImgExportDirectory->NumberOfNames; i++ )
	{
		_stprintf_s(ptmpbuff, BUFFLENGTHMAX, TEXT("FunctionOrdinals%d %04X\r\n"), i, *(pNamesIndexAddress+i) );
		_tcscat_s(tcharBuff, dSizeInTchar, ptmpbuff);
	}
#endif

	delete[] pTstringAddress;
}

/************************************************************************/
/*
功能:虚拟内存相对地址dFOA和文件偏移RVA的转换
*/
/************************************************************************/
DWORD FOA2RVA(IMAGE_SECTION_HEADER* pImageSectionHeader, DWORD numOfSections, DWORD dFOA)
{
	DWORD dRVA = 0;
	int iSectionBegin = 0;
	int iSectionEnd = numOfSections;
	int i = 0;
	// 找到命中节 break;   // 0 1 2 3 4 5 6  rva=2.3  rva=7
	while(1)
	{
		i = (iSectionBegin + iSectionEnd)/2;   // i = 3   // i = 1  // i = 2 ///i=3 ///i=4  ///i=5 ///i=5
		if( pImageSectionHeader[i].PointerToRawData <= dFOA 
			&& dFOA < (pImageSectionHeader[i].PointerToRawData + pImageSectionHeader[i].SizeOfRawData) )
		{
			break;
		}
		else if( pImageSectionHeader[i].PointerToRawData > dFOA )
		{
			iSectionEnd = i - 1;    // 0 1 2 3
		}
		else if( dFOA >= (pImageSectionHeader[i].PointerToRawData + pImageSectionHeader[i].SizeOfRawData) )
		{
			iSectionBegin = i + 1;  // 1 2 3  /// 3 4 5 6 ///4 5 6 ///5 6
		}
		
		if( iSectionEnd < iSectionBegin || iSectionEnd < 0)
		{
			i = -1;  // 没找到
			break;
		}
	}
	if(i >= 0)
	{
		dRVA =  dFOA - pImageSectionHeader[i].PointerToRawData + pImageSectionHeader[i].VirtualAddress;
	}
	else if( dFOA < pImageSectionHeader[0].PointerToRawData ) // 在文件头
	{
		dRVA = dFOA;
	}
	return dRVA;
}
/************************************************************************/
/*
功能:虚拟内存相对地址dRVA和文件偏移FOA的转换
*/
/************************************************************************/
DWORD RVA2FOA(IMAGE_SECTION_HEADER* pImageSectionHeader, DWORD numOfSections, DWORD dRVA)
{
	DWORD dFOA = 0;
	int iSectionBegin = 0;
	int iSectionEnd = numOfSections;
	int i = 0;
	// 找到命中节 break;   // 0 1 2 3 4 5 6  rva=2.3  rva=7
	while(1)
	{
		i = (iSectionBegin + iSectionEnd)/2;   // i = 3   // i = 1  // i = 2 ///i=3 ///i=4  ///i=5 ///i=5
		if( pImageSectionHeader[i].VirtualAddress <= dRVA 
			&& dRVA < (pImageSectionHeader[i].VirtualAddress + pImageSectionHeader[i].Misc.VirtualSize) )
		{
			break;
		}
		else if( pImageSectionHeader[i].VirtualAddress > dRVA )
		{
			iSectionEnd = i - 1;    // 0 1 2 3
		}
		else if( dRVA >= (pImageSectionHeader[i].VirtualAddress + pImageSectionHeader[i].Misc.VirtualSize) )
		{
			iSectionBegin = i + 1;  // 1 2 3  /// 3 4 5 6 ///4 5 6 ///5 6
		}
		
		if( iSectionEnd < iSectionBegin || iSectionEnd < 0 )
		{
			i = -1; // 没找到
			break;
		}
	}
	if(i >= 0)
	{
		dFOA =  dRVA - pImageSectionHeader[i].VirtualAddress + pImageSectionHeader[i].PointerToRawData;
	}
	else if( dRVA < pImageSectionHeader[0].VirtualAddress ) // 在文件头
	{
		dFOA = dRVA;
	}	
	return dFOA;
}