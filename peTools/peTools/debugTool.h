#pragma once
#include "stdafx.h"

void __cdecl OutputDebugStringF(const char* format, ...);

#ifdef _DEBUG
#define dgbPrintf OutputDebugStringF
#else
#define dgbPrintf
#endif