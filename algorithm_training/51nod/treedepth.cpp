#include <cmath>
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

int main() { 
	int n;
	int node;
	int parent;
	int depth = 0;
    int depth_max = 0;
    int i;
	cin>>n;
	vector<int> tree_node(n+1, 0);
	for (i=2; i <= n; i++) {
	    cin>>parent>>node;
	    if( parent>n || node>n ) {
	        return -1;
	    }
	    tree_node[node] = parent;
	}
	for (i = n; i > 0; i--) {
        node = i;
        parent = node;
        depth = 0;
        while (parent != 0) {
            depth++;
            parent = tree_node[parent];
        }
        if (depth > depth_max) {
            depth_max = depth;
        }
    }
    cout<<depth_max;
	return 0;
}