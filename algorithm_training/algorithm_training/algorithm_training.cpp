﻿// algorithm_training.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "test1.h"

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

int main()
{
    system("chcp 65001");
	atexit(functionAfterMain);
	char c = '\0';
    char getBuff;
	while (c != 'q')
	{
        test1();
        test2();
        test3();
        test4();
        test5();
        test6();
        test7();
		printf("结束：enter q to exit:");
        //fflush(stdin); /*清除输入缓冲区*/
        while (getBuff = getchar() != '\n' && getBuff != EOF);
        scanf_s("%c", &c, 1);
        printf("the input char:%c\n", c);
	}	

    return 0;
}

