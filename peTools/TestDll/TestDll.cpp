// TestDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "TestDll.h"
// 类、变量、函数 导出举例
#if 0 // 类、变量、函数 导出举例
// 这是导出变量的一个示例
TESTDLL_API int nTestDll=0;

// 这是导出函数的一个示例。
TESTDLL_API int fnTestDll(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 TestDll.h
CTestDll::CTestDll()
{
	return;
}
#endif

TESTDLL_API int __stdcall funcAdd(int ia, int ib)
{
	return (ia + ib);
}

TESTDLL_API int __stdcall funcSub(int ia, int ib)
{
	return (ia - ib);
}

TESTDLL_API int __stdcall funcMulti(int ia, int ib)
{
	return (ia * ib);
}

TESTDLL_API int __stdcall funcDiv(int ia, int ib)
{
	return (ia / ib);
}