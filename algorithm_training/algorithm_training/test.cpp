#include "test.h"
#include "stdafx.h"


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

