#pragma once
#include "stdafx.h"
#include <vector>
#include <stack>
struct listnode {
	int data;
	listnode* next;
};

bool listMerge(listnode* listA, listnode* listB);
void listMerge2(listnode* listA, listnode* listB);
void printMatrix(int n);
void printCubeTest();
int __cdecl _abs(signed int i);
int train_stack_out(int maxNum, const char* trainSequence);