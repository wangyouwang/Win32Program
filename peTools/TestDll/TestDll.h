// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� TESTDLL_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// TESTDLL_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef TESTDLL_EXPORTS
#define TESTDLL_API extern "C" __declspec(dllexport)
#else
#define TESTDLL_API extern "C" __declspec(dllimport)
#endif

// �ࡢ���������� ��������
#if 0 // �ࡢ���������� ��������
// �����Ǵ� TestDll.dll ������
class TESTDLL_API CTestDll {
public:
	CTestDll(void);
	// TODO: �ڴ�������ķ�����
};

extern TESTDLL_API int nTestDll;

TESTDLL_API int fnTestDll(void);
#endif


TESTDLL_API int __stdcall funcAdd(int ia, int ib);

TESTDLL_API int __stdcall funcSub(int ia, int ib);

TESTDLL_API int __stdcall funcMulti(int ia, int ib);

TESTDLL_API int __stdcall funcDiv(int ia, int ib);
