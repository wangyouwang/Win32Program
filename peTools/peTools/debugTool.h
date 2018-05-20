#pragma once
#include "stdafx.h"

void __cdecl OutputDebugStringF(const char* format, ...);

#ifdef _DEBUG
#define dbgPrintf OutputDebugStringF
#else
#define dbgPrintf
#endif