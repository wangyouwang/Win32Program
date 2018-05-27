// TestSubProject.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <psapi.h>
DWORD GetModuleSize(TCHAR* szModuleName);
void PrintProcessNameAndID( DWORD processID );
void TestListProcessAndModules();
void TestShowPeFileStruct();
int _tmain(int argc, _TCHAR* argv[])
{
	//TestListProcessAndModules();
	TestShowPeFileStruct();
	return 0;
}
// ��һ���� �г����̺ͽ���������ģ��
void TestListProcessAndModules()
{
	// Get the list of process identifiers.
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;
	if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
	{
		return ;
	}

	// Calculate how many process identifiers were returned.
	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.
	for ( i = 0; i < cProcesses; i++ )
	{
		if( aProcesses[i] != 0 )
		{
			PrintProcessNameAndID( aProcesses[i] );
		}
	}
}

// ע image size ��Ҫ��ȡ·��ָ���ļ� ���߿�ѡpeͷ��ȡ
void PrintProcessNameAndID( DWORD processID )
{
	TCHAR szProcessName[MAX_PATH] = TEXT("<system Idle Process>");
	int i;
	// Get a handle to the process.

	HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
		PROCESS_VM_READ,
		FALSE, processID ); //PROCESS_ALL_ACCESS

	// Get the process name.
	if (NULL != hProcess )
	{
		HMODULE hMods[1024];
		HMODULE hMod;
		DWORD cbNeeded;

		if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), 
			&cbNeeded) )
		{
			//GetModuleBaseName( hProcess, NULL/*hMods[0]*/, szProcessName, 
			//	sizeof(szProcessName)/sizeof(TCHAR) );
			//Print the process name and identifier.
			//_tprintf( TEXT("%s  (PID: %u)\n"), szProcessName, processID );
			// Printf all the modules in this process.
			for ( i = 0; i < 1; i++ )
			{
				 TCHAR szModName[MAX_PATH];
				  // Get the full path to the module's file.
				 if ( GetModuleFileNameEx( hProcess, hMod, szModName,
					 sizeof(szModName) / sizeof(TCHAR)))
				 {/*IMAGE_DOS_HEADER*/
						DWORD imageSize = GetModuleSize(szModName);
						if(i==0)
						{
							// Print the Process name and handle value.
							_tprintf( TEXT("<process>: %s pid: %d imageBase:0x%08X imagesize:0x%08x\n"),
								szModName, processID, hMod, imageSize );
						}
						else
						{
							// Print the module name and handle value.
							_tprintf( TEXT("\t<module>: %s imageBase:0x%08X imagesize:0x%08x\n"), 
								szModName, hMods[i], imageSize );
						}					 
				 }
			}
		}
		else
		{
			//only Print the process name and identifier.
			_tprintf( TEXT("%s  (PID: %u)\n"), szProcessName, processID );
		}
	}
	// Release the handle to the process.
	CloseHandle( hProcess );
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


// �ڶ����� ��ʾPe�ļ��ṹ
TCHAR inFileName[1024] = TEXT("D:\\win32Program\\Projects\\github_Win32Program\\peTools\\Debug\\peTools_dbg.exe");
TCHAR logFileName[1024] = TEXT("D:\\win32Program\\Projects\\github_Win32Program\\peTools\\Debug\\peTools_show.log");
void TestShowPeFileStruct()
{
	// ��������
	int nameLength = 0;
	FILE* inFile = NULL;
	errno_t err = 0;
	DWORD inFileSize = 0;
	BYTE* fileBuffer = NULL;
	DWORD nRet;
	IMAGE_DOS_HEADER* pDosHeader = NULL;
	IMAGE_NT_HEADERS* pImageNtHeader = NULL;
	IMAGE_FILE_HEADER* pImageFileHeader = NULL;
	IMAGE_OPTIONAL_HEADER32* pImageOptionHeader32 = NULL;
	IMAGE_OPTIONAL_HEADER64* pImageOptionHeader64 = NULL;


	// ���ļ� ��ȡ��С ����ռ� ���ص��ڴ�
#if 1
	{
		err = _tfopen_s(&inFile, inFileName, TEXT("rb"));
		if( err!=0 )
		{
			_tprintf_s(TEXT("err = %d\n"),err);
		}
		assert(inFile!=NULL);
		err = fseek(inFile, 0, SEEK_END); assert(err==0);
		inFileSize = ftell(inFile); assert(inFileSize!=0);

		fileBuffer = new BYTE[inFileSize];assert(fileBuffer!=NULL);	
		err = fseek(inFile, 0, SEEK_SET);assert(err==0);
		nRet = fread(fileBuffer, sizeof(BYTE), inFileSize, inFile);
		if(nRet != inFileSize)
		{
			_tprintf_s(TEXT("read file error!\n"));
		}
		else
		{
			_tprintf_s(TEXT("read file ok! read 0x%x bytes"), inFileSize);
		}
		fclose(inFile);
		inFile = NULL;
	}
#endif
	//��ʼ������ͷ��ָ��  pDosHeader pImageNtHeader pImageFileHeader ? pImageOptionHeader32 : pImageOptionHeader64
	assert(inFileSize > sizeof(IMAGE_DOS_HEADER));
	pDosHeader = (IMAGE_DOS_HEADER*)fileBuffer;
	pImageNtHeader =  (IMAGE_NT_HEADERS*)(fileBuffer + pDosHeader->e_lfanew);
	pImageFileHeader = &(pImageNtHeader->FileHeader);
	if( pImageFileHeader->SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER32) )
	{
		pImageOptionHeader32 = &(pImageNtHeader->OptionalHeader);
	}
	else if( pImageFileHeader->SizeOfOptionalHeader == sizeof(IMAGE_OPTIONAL_HEADER64) )
	{
		pImageOptionHeader64 = (IMAGE_OPTIONAL_HEADER64*)&(pImageNtHeader->OptionalHeader);
	}
	else
	{
		_tprintf_s(TEXT("read file wasn't PE format!\n"));
		return ;
	}

	delete[] fileBuffer;
}