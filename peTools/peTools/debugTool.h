#pragma once
#include "stdafx.h"

void __cdecl OutputDebugStringF(const TCHAR* format, ...);

#ifdef _DEBUG
#define dbgPrintf OutputDebugStringF
#else
#define dbgPrintf
#endif


#if 0
#ifdef UNICODE
#define _stprintf swprintf
#else
#define _stprintf sprintf
#endif
#endif