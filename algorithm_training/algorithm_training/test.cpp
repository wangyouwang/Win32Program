#include "test.h"
#include "stdafx.h"


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

