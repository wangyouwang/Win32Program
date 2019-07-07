// algorithm_training.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "test1.h"
#include <stdlib.h>

void test1()
{
    listMergeTest();
}

void test2()
{
	//printMatrix(8);  // mine

	printCubeTest();   // reference
}

void test3()
{
    absTest();
}

void test4()
{
    intReverseTest();
}

void test5()
{
    floatTest();
}

void test6()
{
    getenvTest();
}

void test7()
{
    trainStackOutTest();
}

void test8()
{
    treeDepthGetTeset();
}

int main()
{
    system("chcp 65001");
	atexit(functionAfterMain);
	char c = '\0';
    char getBuff = EOF;

	{
        //test1();
        //test2();
        //test3();
        //test4();
        //test5();
        //test6();
        //test7();
        //test8();

        system("pause");
	}	

    return 0;
}

