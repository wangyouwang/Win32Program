// TestDll.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "TestDll.h"
// �ࡢ���������� ��������
#if 0 // �ࡢ���������� ��������
// ���ǵ���������һ��ʾ��
TESTDLL_API int nTestDll=0;

// ���ǵ���������һ��ʾ����
TESTDLL_API int fnTestDll(void)
{
	return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� TestDll.h
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