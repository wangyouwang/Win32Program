#pragma once
#include "stdafx.h"

void __cdecl OutputDebugStringF(const TCHAR* format, ...);

#ifdef _DEBUG
#define dbgPrintf OutputDebugStringF
#else
#define dbgPrintf
#endif