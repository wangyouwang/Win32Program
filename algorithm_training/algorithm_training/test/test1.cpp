#include "stdafx.h"
#include "test1.h"
#include "function1.h"

listnode *pListA;
listnode *pListB;

int dataArray1[8] = { 0,1,2,3,4,5,6,7 };
int dataArray2[8] = { 10,11,12,13,14,15,16,17 };
void listMergeTest()
{
    // 初始化 pListA 存放 dataArBray1
    listnode* p1 = NULL;
    listnode* p2 = NULL;
    for (int i = 0; i < 8; i++)
    {
        p1 = new listnode;
        p1->data = dataArray1[i];
        p1->next = NULL;
        if (0 == i)
            pListA = p1;
        if (p2 != NULL)
            p2->next = p1;
        p2 = p1;
    }

    // 初始化 ListB 存放 dataArray2
    p1 = NULL;
    p2 = NULL;
    for (int i = 0; i < 8; i++)
    {
        p1 = new listnode;
        p1->data = dataArray2[i];
        p1->next = NULL;
        if (0 == i)
            pListB = p1;
        if (p2 != NULL)
            p2->next = p1;
        p2 = p1;

    }

    // 打印 listA， listB
    printf("listA: ");
    p1 = pListA;
    while (p1)
    {
        printf(" %d ", p1->data);
        p1 = p1->next;
    }
    printf(" end\n");

    printf("listB: ");
    p1 = pListB;
    while (p1)
    {
        printf(" %d ", p1->data);
        p1 = p1->next;
    }
    printf(" end\n");

#if 0
    // 用递归函数  归并 pListA, pListB
    bool bRet = listMerge(pListA, pListB);

    if (bRet)
    {
        printf("listB: ");
        p1 = pListB;
        while (p1)
        {
            printf(" %d ", p1->data);
            p1 = p1->next;
        }
        printf(" end\n");
    }
    else
    {
        printf("listA: ");
        p1 = pListA;
        while (p1)
        {
            printf(" %d ", p1->data);
            p1 = p1->next;
        }
        printf(" end\n");
    }
#endif

#if 1
    listMerge2(pListA, pListB);
    printf("listA: ");
    p1 = pListA;
    while (p1)
    {
        printf(" %d ", p1->data);
        p1 = p1->next;
    }
    printf(" end\n");
#endif
}

#define Num CUBE_MAX
void printCubeTest()
{
    int a[Num][Num], n=10;
    printf("printCubeTest: input n(n<10):\n");
    scanf_s("%d", &n);
    printCube(a, n);
    return;
}

void absTest()
{
    int i;
    int ret;

    i = -1;
    ret = _abs(i);
    printf("_abs(%d) : %d\n", i, ret);

    i = -2;
    ret = _abs(i);
    printf("_abs(%d) : %d\n", i, ret);
    return;
}

void trainStackOutTest()
{
    int ret;
    ret = trainStackOut(5, "21534");
    printf("21534 nRet=%d\n", ret);
}

void intReverseTest()
{
    int ii = -1024;

    if (!ii)
    {// not here
        printf("!%d is true\n", ii);
    }
    else
    {// here
        printf("!%d is false\n", ii);
    }
    return;
}

void floatTest()
{
    int x = 100;
    float f = 0.25;
    double d = 0.0625;

    if (x == (int)(float)x)
    {
        printf("x(%d) x == (int)(float)x \n", x);
    }
    if (f == (float)(double)f)
    {
        printf("f(%f) f == (float)(double)f\n", f);
    }
    if (d == (double)(float)d)
    {
        printf("d(%f) d == (double)(float)d\n", d);
    }
    if (f == -(-f))
    {
        printf("f(%f) f == -(-f)\n", f);
    }
    if (2 / 3 == 2 / 3.0)
    {
        printf("2 / 3 == 2 / 3.0\n");
    }
    /*
        d<0.0 == >((d * 2)<0.0)
        d > f == > -f > -d
        d*d >= 0.0
        (d + f) - d == f
    */
}

void getenvTest()
{
#if 0
    char *s;
    s = getenv("COMSPEC");       /* get the comspec environment parameter */
    printf("Command processor: %s\n", s);   /* display comspec parameter */
#endif
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "COMSPEC") == 0 && buf != nullptr)
    {
        printf("EnvVarName = %s\n", buf);
        free(buf);
    }
}

void functionAfterMain()
{
    time_t t;
    struct tm * lt = new struct tm;
    time(&t);//获取Unix时间戳。

    // lt = localtime(&t);
    localtime_s(lt, &t); //转为时间结构。

    char time_str[256] = "\0";
    sprintf_s(time_str, "%d/%d/%d %d:%d:%d", \
        lt->tm_year + 1900, lt->tm_mon, lt->tm_mday, \
        lt->tm_hour, lt->tm_min, lt->tm_sec);//格式化
    delete lt;
    printf("TIME:%s FUNC:%s, LINE:%d\n", time_str, __FUNCTION__, __LINE__);
}

#define TreeNodeNum 5
void treeDepthGetTeset()
{
    int relations[(TreeNodeNum - 1) * 2] = {
        1, 2,
        1, 3,
        2, 4,
        4, 5
    };
    int ret = 0;
    ret = treeDepthGet(TreeNodeNum, relations);
    printf("tree depth: %d \n", ret);
}