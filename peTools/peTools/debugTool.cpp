#include "debugTool.h"
#include <stdio.h>
#include <Shlwapi.h>
#define BUFFLENGTH 4096
#ifdef _DEBUG
void __cdecl OutputDebugStringF(const TCHAR* format, ...)
{
	va_list vlArgs;
	//char* strBuffer = (char*)GlobalAlloc(GPTR, BUFFLENGTH);
	TCHAR* strBufferT = (TCHAR*)GlobalAlloc(GPTR, BUFFLENGTH);
	va_start(vlArgs, format);
	_vsnwprintf(strBufferT, BUFFLENGTH-2, format, vlArgs);
	//_vsnprintf(strBuffer, BUFFLENGTH-2, format, vlArgs);
	va_end(vlArgs);
	StrCatBuff(strBufferT,TEXT("\n"), BUFFLENGTH);
	//strcat(strBuffer, "\n");
	OutputDebugString(strBufferT);
	GlobalFree(strBufferT);
	return ;
}
#endif