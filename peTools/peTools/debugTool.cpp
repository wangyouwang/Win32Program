#include "debugTool.h"
#include <stdio.h>
#define BUFFLENGTH 4096
void __cdecl OutputDebugStringF(const char* format, ...)
{
	va_list vlArgs;
	char* strBuffer = (char*)GlobalAlloc(GPTR, BUFFLENGTH);
	va_start(vlArgs, format);
	_vsnprintf(strBuffer, BUFFLENGTH-2, format, vlArgs);
	va_end(vlArgs);

	strcat(strBuffer, "\n");
	OutputDebugStringA(strBuffer);
	return ;
}