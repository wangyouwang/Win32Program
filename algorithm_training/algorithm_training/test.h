#pragma once
#include "stdafx.h"

struct listnode {
	int data;
	listnode* next;
};

bool listMerge(listnode* listA, listnode* listB);
void listMerge2(listnode* listA, listnode* listB);
