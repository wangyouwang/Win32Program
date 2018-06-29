#include "test.h"
#include "stdafx.h"

// 20180628
/*
	��ʹ�ö���ռ佫A��B������Ԫ�ؽ���鲢
	�����ǲ��õݹ������β�˽��й鲢��ʵ�ʴӻ���������ݹ���ã�����ѹջ���ķѵĿռ����
	ͨ����ҪԤ��֪���ĸ�����������ͨ������ֵ���д��ݣ�true��ʾlistB����false��ʾlistA��
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



// �ǵݹ��ʵ��A��B��������Ľ���鲢
// ֻ������û����Ҫ����Ŀռ��� list->data����
// ����listAΪ listͷ
void listMerge2(listnode* listA, listnode* listB)
{
	listnode *pBCurrent = listB;
	listnode *pBm_next = NULL;
	listnode *pACurrent = listA;
	listnode *pAm_next = NULL;

	while (pBCurrent != NULL && pACurrent != NULL)
	{
		pBm_next = pBCurrent->next;//��¼��һ������Ԫ��
		pAm_next = pACurrent->next;//��¼����λ��

		if( NULL != pACurrent->next)
			pBCurrent->next = pACurrent->next;
		pACurrent->next = pBCurrent;

		pBCurrent = pBm_next;
		pACurrent = pAm_next;
	}
}

// 20180629
// 
// ����һ��n ��Ȼ������Ļ�ϴ�ӡ��NxN �ľ���
// ���磬����һ��3����
// 1 2 3
// 8 9 4
// 7 6 5
// ����һ��4����
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
			printf("���裺��д %d\n", c);
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
void test3()
{
	int a[N][N], n;
	printf("input n:\n");
	scanf_s("%d", &n);
	printCube(&a[0], n);
	getchar();
}
