#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define max_num 10000

long long select_pos(int n, int *arrays)
{
    long long short_distance = 0x7fffffffffffffffLL;
    long long distance;
    int i;
    int j;

    for (i = 0; i < n; i++) {
        distance = 0;
        for( j=0; j<n; j++ ) {
            distance += (long long) abs( arrays[j] - arrays[i] );
        }
        if(distance <= short_distance){
            short_distance = distance; 
        }        
    }
    return short_distance;
}


int main() {
	//int n;
	//scanf("%d",&n);
	//printf("%d",n);
	//主要程序内容写在这里
    int i;
    int n;
    int arrays[max_num];
    long long distance = 0x7fffffffffffffffLL; 
#if 1
    scanf_s("%d", &n);
    //printf("n=%d\n", n);
    for (i=0; i<n; i++) {
        scanf_s("%d", &(arrays[i]));
        //printf("arrays[%d]=%d\n", i, arrays[i]);
    }
#endif
    distance = select_pos(n, arrays);
    printf("%lld\n", distance);

	return 0;
}