#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

typedef unsigned int uint32;

uint32 calc_gcd(uint32 a, uint32 b)
{
    uint32 tmp;
    while ( (tmp = a%b) != 0) {
        a = b;
        b = tmp;
    }
    return b;
}


int main() {
	//int n;
	//scanf("%d",&n);
	//printf("%d",n);
	//主要程序内容写在这里
    uint32 a, b;
    uint32 gcd = 1;
    
    scanf("%u %u", &a, &b);
    //printf("\n\n%u %u \n\n", a, b);

    gcd = calc_gcd(a, b);
    printf("%u\n", gcd);

	return 0;
}
