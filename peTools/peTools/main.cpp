

#include "stdafx.h"
#include "debugTool.h"
#include <CommCtrl.h>
#pragma  comment(lib, "comctl32.lib")

#define BUFFLENGTHMAX 1024
#define MAXPROCESS 1024
#define MAXMODULES  1024


//ȫ�ֱ��� imageBase
#if 1
HINSTANCE hAppInstance = NULL;
HWND hwndMainDlg = NULL;
HWND hwndPeFileDlg = NULL;
TCHAR peFileName[MAX_PATH]=TEXT("\0");
BYTE* fileBuffer = NULL;
DWORD inFileSize = 0;

IMAGE_DOS_HEADER* pDosHeader = NULL;
IMAGE_NT_HEADERS* pImageNtHeader = NULL;
IMAGE_FILE_HEADER* pImageFileHeader = NULL;
IMAGE_OPTIONAL_HEADER32* pImageOptionHeader32 = NULL;
IMAGE_OPTIONAL_HEADER64* pImageOptionHeader64 = NULL;
IMAGE_SECTION_HEADER* pImageSectionHeaders=NULL;
// WORD dNumberOfSections = 0;  //  pImageFileHeader->NumberOfSections
// #define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16
#endif

//�������� 
#if 1
VOID InitProcessListView(HWND hwnd);
VOID InitModuleListView(HWND hwnd);
VOID InitSectionsListView(HWND hwnd);
VOID InitDirectorys(HWND hwnd);
VOID EnumProcess(HWND hwnd);
VOID EnumModules(HWND hListProcess, WPARAM wparam, LPARAM lparam);
VOID ShowModules(HWND hListModule, DWORD processId);
DWORD GetModuleSize(TCHAR* szModuleName);
BOOL DlgGetPeFileName(HWND hwnd, TCHAR* fileName, DWORD NameBuffSize);
BOOL CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_PeFile(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DialogProc_Sections(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void destroyPeFileStruct();
BOOL initPeFileStruct();
BOOL CALLBACK  DialogProc_Directorys(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif
// ��ں������ص�����::C:\Program Files (x86)\Microsoft SDKs\Windows\v7.0A\include\winbase.h
int CALLBACK WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, 
				__in LPSTR lpCmdLine, __in int nShowCmd )
{
	//����ȫ�ֱ���  imageBase
	hAppInstance = hInstance;

#if 1  // ListControl need 
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
	NMHDR* nmhdr = NULL;
	HICON hicon = NULL;
	BOOL nRet = FALSE;
	
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		nRet = TRUE;
		break;
	case WM_CLOSE:  // ���Ͻǵ�x
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
		//����ͼ��
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
			// GetOpenFileName �õ��ļ�·��
			if(TRUE == DlgGetPeFileName(hwnd, peFileName, MAX_PATH-1))
			{
				dbgPrintf(TEXT("select filepath: %s\n"), peFileName);
			// ��ʾ�Ի��������peͷ��Ϣ
				//��������
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
			nRet = FALSE;  //���� FALSE ����Ĭ�ϴ�����
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
	//��ʼ��
	memset(&lv, 0, sizeof(lv));
	//��ȡIDC_LIST_PROCESS���
	hListProcess = GetDlgItem(hwnd, IDC_LIST_PROCESS);
	//��������ѡ��
	SendMessage(hListProcess, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
#if 1
	//��1��
	lv.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lv.pszText = TEXT("����·��");
	lv.cx = 620;
	lv.iSubItem = 0;
	//ListView_InsertColumn(hListProcess, 0, &lv);
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
#endif
	//hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EnumProcess, hListProcess, 0, &ThreadId);
	//WaitForSingleObject(hThread, INFINITE);
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
	lv.cx = 720;
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

VOID EnumProcess(HWND hwnd)
{
	LV_ITEM lvItem;
	TCHAR* tcharBuff = new TCHAR[BUFFLENGTHMAX];
	memset(tcharBuff, 0, sizeof(TCHAR) * BUFFLENGTHMAX);
	int iItem=0,iSubItem=0;
	 TCHAR szModName[MAX_PATH];
	//����windows ���н��̣� �õ����н�����Ϣ
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
	fclose(file);
	// ����NTͷ��OptionalHeader.SizeofImage
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

	// ��ȡѡ����
	//SendMessage(hListProcess, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
	dwRowId = ListView_GetNextItem(hListProcess, -1, LVNI_SELECTED);
	dbgPrintf(TEXT("dwRowId=%d\n"), dwRowId);
	if(dwRowId == -1)
	{
		dbgPrintf(TEXT("Error!\n"));
	}
	//��ȡѡ���е�PID
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
	// �� hListModule ��ʾ processID ��Ӧ�� modules
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

// DIALOG_FILE ��Ϣ�ص�����  ���������Ϣ����TRUE ���򷵻�FALSE
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
	case WM_CLOSE:  // ���Ͻǵ�x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
		nRet = TRUE;
		break;
	case WM_INITDIALOG:
		hwndPeFileDlg = hwnd;
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
	// ���ļ� ��ȡ��С ����ռ� ���ص��ڴ�
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

	//��ʼ������ͷ��ָ��  pDosHeader pImageNtHeader pImageFileHeader ? pImageOptionHeader32 : pImageOptionHeader64
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
// DIALOG_SECTIONS ��Ϣ�ص�����  ���������Ϣ����TRUE ���򷵻�FALSE
BOOL CALLBACK DialogProc_Sections(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL nRet = FALSE;
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		nRet = TRUE;
		break;
	case WM_CLOSE:  // ���Ͻǵ�x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
		nRet = TRUE;
		break;
	case WM_INITDIALOG:
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
		TEXT("����"),TEXT("�����С"),TEXT("����ƫ��"),
		TEXT("RAW��С"),TEXT("RAWƫ��"),TEXT("����ֵ") };
	DWORD iSubitem = 0;
	WORD dNumberOfSections = pImageFileHeader->NumberOfSections;
	//��ʼ��
	memset(Name, 0, IMAGE_SIZEOF_SHORT_NAME+1);
	memset(&lv, 0, sizeof(lv));
	//��ȡIDC_LIST_PROCESS���
	hListSections = GetDlgItem(hwnd, IDC_LIST_SECTIONS);
	//��������ѡ��
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

	//��ʾ Sections �� hListSections
 	for(int i=0; i<dNumberOfSections; i++)
 	{
#if 1
 		int j = 0; //TEXT("����"),
 		memset(tcharBuff, 0, sizeof(tcharBuff));
 		memset(&lvItem, 0, sizeof(lvItem));
 		memcpy_s(Name, sizeof(Name), pImageSectionHeaders[i].Name, IMAGE_SIZEOF_SHORT_NAME);
 		CharToTchar((const char*)Name, tcharBuff);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_InsertItem(hListSections, &lvItem);
 
 		j++; // TEXT("�����С"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0%08X"), pImageSectionHeaders[i].Misc.VirtualSize);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 
 		j++; // TEXT("����ƫ��"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), pImageSectionHeaders[i].VirtualAddress);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 		
 
 		j++; // TEXT("RAW��С"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), pImageSectionHeaders[i].SizeOfRawData);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 
 		j++; // TEXT("RAWƫ��"),
 		_stprintf_s(tcharBuff, BUFFLENGTHMAX, TEXT("0x%08X"), pImageSectionHeaders[i].PointerToRawData);
 		lvItem.mask = LVIF_TEXT;
 		lvItem.pszText = tcharBuff;
 		lvItem.iItem = i;
 		lvItem.iSubItem = j;
 		ListView_SetItem(hListSections, &lvItem);
 
 		j++; // TEXT("����ֵ")
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

// DIALOG_DIRECTORY ��Ϣ�ص�����  ���������Ϣ����TRUE ���򷵻�FALSE
BOOL CALLBACK  DialogProc_Directorys(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bRet = FALSE;
	switch(uMsg)
	{
	case WM_DESTROY:
		dbgPrintf(TEXT("WM_DESTROY: wParam=%x lParam=%x\n"),wParam, lParam);
		bRet = TRUE;
		break;
	case WM_CLOSE:  // ���Ͻǵ�x
		dbgPrintf(TEXT("WM_CLOSE: wParam=%x lParam=%x\n"),wParam, lParam);
		DestroyWindow(hwnd);
		bRet = TRUE;
		break;
	case WM_INITDIALOG:
		InitDirectorys(hwnd);
		bRet = TRUE;
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

	for(int i=0; i < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; i++)
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