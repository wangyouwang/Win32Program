#pragma once

#include <vector>
#include <stack>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <iostream>

/*
There are two int variables: a and b, don’t use “if”, “? :”, “switch” 
or other judgement statements, find out the biggest one of the two numbers. 
*/
#define MAX(X,Y) ( (abs(X-Y)+(X+Y))/2 )
#define CUBE_MAX 10
typedef struct listnodeTag {
    int data;
    struct listnodeTag* next;
}listnode;

bool listMerge(listnode* listA, listnode* listB);
void listMerge2(listnode* listA, listnode* listB);
void printMatrix(int n);
void printCube(int a[][CUBE_MAX], int n);
int __cdecl _abs(signed int i);
int trainStackOut(int maxNum, const char* trainSequence);