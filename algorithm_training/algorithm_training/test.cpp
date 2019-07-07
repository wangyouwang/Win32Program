#include "test.h"
#include "stdafx.h"

// 20180628
/*
	不使用额外空间将A、B两链表元素交叉归并
	方法是采用递归从链表尾端进行归并，实际从汇编来看，递归调用，参数压栈，耗费的空间更多
	通常需要预先知道哪个链表长，这里通过返回值进行传递，true表示listB长，false表示listA长
*/
bool listMerge(listnode* listA, listnode* listB)
{
	if (listA->next == NULL || listB->next == NULL)
	{
		if (listA->next == NULL)
		{
			listA->next = listB->next;
			listB->next = listA;
			return true;
		}
		else //listB->next == NULL
		{
			listB->next = listA->next;
			listA->next = listB;
			return false;
		}
	}
	else
	{
		if (listMerge(listA->next, listB->next))
		{
			listA->next = listB->next;
			listB->next = listA;
			return true;
		}
		else
		{
			listB->next = listA->next;
			listA->next = listB;
			return false;
		}
	}
}



// 非递归的实现A、B两个链表的交叉归并
// 只能算是没有需要额外的空间存放 list->data而已
// 最终listA为 list头
void listMerge2(listnode* listA, listnode* listB)
{
	listnode *pBCurrent = listB;
	listnode *pBm_next = NULL;
	listnode *pACurrent = listA;
	listnode *pAm_next = NULL;

	while (pBCurrent != NULL && pACurrent != NULL)
	{
		pBm_next = pBCurrent->next;//记录下一个待插元素
		pAm_next = pACurrent->next;//记录插入位置

		if( NULL != pACurrent->next)
			pBCurrent->next = pACurrent->next;
		pACurrent->next = pBCurrent;

		pBCurrent = pBm_next;
		pACurrent = pAm_next;
	}
}

// 20180629
// 
// 输入一个n ，然后在屏幕上打印出NxN 的矩阵！
// 例如，输入一个3，则
// 1 2 3
// 8 9 4
// 7 6 5
// 输入一个4，则
// 01 02 03 04
// 12 13 14 05
// 11 16 15 06
// 10 09 08 07
//

void printMatrix(int n)
{
	int *Matrix = new int[n*n];
	memset(Matrix, 0, n*n*sizeof(int));

	enum direct {
		dir_right, dir_down, dir_left, dir_up
	};
	//direct dirs[4] = { dir_right, dir_down, dir_left, dir_up };

	direct dd = dir_right; //dirs[0]


	printf("START\t\t\t\t\tSTART\n");
	for (int x = 0; x < n; x++)
	{
		for (int y = 0; y < n; y++)
		{
			printf("%2d ", Matrix[x*n + y]);
		}
		printf("\n");
	}
	int c = 1;
	int i = 0; 
	int j = 0;
	while (c <= n*n)
	{
		if (Matrix[ i * n + j ] == 0 && i < n && j < n)
		{
			printf("步骤：填写 %d\n", c);
			Matrix[i * n + j] = c++;
			for (int x = 0; x < n; x++)
			{
				for (int y = 0; y < n; y++)
				{
					printf("%2d ", Matrix[x*n + y]);
				}
				printf("\n");
			}			

			switch (dd)
			{
			case dir_right:
				j++;
				break;
			case dir_down:
				i++;
				break;
			case dir_left:
				j--;
				break;
			case dir_up:
				i--;
				break;
			}
		}
		else
		{
			switch (dd)
			{
			case dir_right:
				dd = dir_down;
				j--;
				i++;
				break;
			case dir_down:
				dd = dir_left;
				i--;
				j--;
				break;
			case dir_left:
				dd = dir_up;
				j++;
				i--;
				break;
			case dir_up:
				dd = dir_right;
				i++;
				j++;
				break;
			}
		}
	}

	printf("END\t\t\t\t\t\tEND\n");
	for (int x = 0; x < n; x++)
	{
		for (int y = 0; y < n; y++)
		{
			printf("%2d ", Matrix[x*n+y]);
		}
		printf("\n");
	}
	
	delete[] Matrix;
	Matrix = NULL;
}

#define N 10 
void printCube(int a[][N], int n)
{
	int i, j, round = 1;
	int m = 1;
	for (i = 0; i < n; i++)
		a[0][i] = m++;
	for (i = n - 1; i >= n / 2; i--)
	{
		for (j = round; j <= i; j++)  // down
			a[j][i] = m++;
		for (j = i; j >= round; j--)  // left
			a[i][j - 1] = m++;
		for (j = i; j > round; j--)   // up
			a[j - 1][round - 1] = m++;
		for (j = round; j < i; j++)   // right
			a[round][j] = m++;
		round++;
	}
	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++)
			printf("%3d", a[i][j]);
		printf("\n");
	}
}
void printCubeTest()
{
	int a[N][N], n;
	printf("input n:\n");
	scanf_s("%d", &n);
	printCube(&a[0], n);
	getchar();
}


//There are two int variables: a and b, don’t use “if”, “? :”, “switch” 
//or other judgement statements, find out the biggest one of the two numbers. 

#define MAX(X,Y) ( (abs(X-Y)+(X+Y))/2 )

int __cdecl _abs(signed int i)
{
	int l, m, n;
	l = (i >> 31);  // 有符号整数移位，用符号填充。。。。
	m = l ^ i;
	n = m - l;
	// n = ((i>>31)^i) - (i>>31);
	return  n;
}
/* 如果 i>0;  i>>31 = 0; (i>>31) ^ i = i;  i-0=i;
 * 如果 i<0;  i>>31 = 0xffffffff; (i>>31) ^ i = (0xffffffff-i); 
 * ((i>>31)^i) - (i>>31)= ((0xffffffff-i) - 0xffffffff) = -i;
*/



/* 火车堆栈问题 */
/* 入栈 1 2 3 4 5 出栈 3 2 1 5 4 */
int train_stack_out(int maxNum, const char* trainSequence)
{
	std::vector<int> trainSeq;
	int currentTrainId = 0;
	for (int i = 0; i < maxNum; i++)
	{
		int tmpId;
		trainSeq.clear();
		trainSeq.reserve(maxNum);
		currentTrainId = int(trainSequence[i] - '0');
		for (int j = i + 1; j < maxNum; j++)
		{
			tmpId = int(trainSequence[j] - '0');
			if (tmpId < currentTrainId)
			{
				trainSeq.push_back(tmpId);

				if (trainSeq.size() > 1 && trainSeq[trainSeq.size() - 1] > trainSeq[trainSeq.size() - 2])
				{
					return 0;
				}
			}
		}
	}
	return 1;
}