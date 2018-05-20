#include "debugTool.h"
#include <stdio.h>
#include <Shlwapi.h>
#define BUFFLENGTH 4096
#ifdef _DEBUG
void __cdecl OutputDebugStringF(const TCHAR* format, ...)
{
	va_list vlArgs;
	//char* strBuffer = (char*)GlobalAlloc(GPTR, BUFFLENGTH);
	TCHAR* strBufferW = (TCHAR*)GlobalAlloc(GPTR, BUFFLENGTH);
	va_start(vlArgs, format);
	_vsnwprintf(strBufferW, BUFFLENGTH-2, format, vlArgs);
	//_vsnprintf(strBuffer, BUFFLENGTH-2, format, vlArgs);
	va_end(vlArgs);
	StrCatBuff(strBufferW,TEXT("\n"), BUFFLENGTH);
	//strcat(strBuffer, "\n");
	OutputDebugString(strBufferW);
	GlobalFree(strBufferW);
	return ;
}
#endif