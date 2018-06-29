// algorithm_training.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"


listnode *pListA;
listnode *pListB;

int dataArray1[8] = { 0,1,2,3,4,5,6,7 };
int dataArray2[8] = { 10,11,12,13,14,15,16,17 };

void test1()
{
	// 初始化 pListA 存放 dataArray1
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


void test2()
{
	printMatrix(8);
}
void test3();

int main()
{
	int ii = 0;
	while (ii != 'q')
	{
		//test1();
		//test2();
		test3();


		printf("enter q to exit:");
		ii = getchar();
	}
    return 0;
}

