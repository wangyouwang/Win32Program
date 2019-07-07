#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stack>
using namespace std;

#define MAX_NUM 50001
int main() { 
	int i = 0;
    char str[MAX_NUM];
    stack<char> sbkt;
    int result = 1;

    cin>>str;
      
    while (str[i] != '\0') {
        if (str[i] == '(') {
            sbkt.push(str[i]);
        } else if (str[i] == ')') {
            if( sbkt.empty() ) {
                result = 0;
                break;
            } else {
                sbkt.pop();
            }
        } else {
            result = 0;
            break;
        }
        i++;
    }
    if( sbkt.empty() && result == 1) {
        result = 1;
    } else {
        result = 0;
    }
    cout<<result;

	return 0;
}