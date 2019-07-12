#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

char points_id[26] = { 'A','B','C','D','E','F','G','H',
                       'I','J', 'K','L','M','N','O','P','Q',
                       'R', 'S','T','U','V','W','X','Y','Z' };
int points_x[26] = { 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 
                       5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 0 };
int points_y[26] = { 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 
                        3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 0 };

int get_dis_4_2(int p, int c_x, int c_y)
{
    return (points_x[p] * 4 - c_x)*(points_x[p] * 4 - c_x) + (points_y[p] * 4 - c_y)*(points_y[p] * 4 - c_y);
}

/*
ax ay
bx by
cx cy
dx dy
*/
int is_rect(int a, int b, int c, int d)
{
    int *x=points_x;
    int *y=points_y;
    int id[4] = {a, b, c, d};
    int da,db,dc,dd;
    int center_x_4 = (x[a] + x[b] + x[c] + x[d]); /* 如果不是整数怎么办？ */
    int center_y_4 = (y[a] + y[b] + y[c] + y[d]); /* 这里不除以4 */
    
    da = get_dis_4_2(a, center_x_4, center_y_4);  /* 获得的距离平方是没有除以4的 */
    db = get_dis_4_2(b, center_x_4, center_y_4);  /* 获得的是距离*4的平方 */
    dc = get_dis_4_2(c, center_x_4, center_y_4);
    dd = get_dis_4_2(d, center_x_4, center_y_4);

    if( (da == db) && (da == dc) && (da == dd) &&  
        (x[a] - x[b]) * (x[a] - x[c]) * (x[a] - x[d]) == 0 &&
        (y[a] - y[b]) * (y[a] - y[c]) * (y[a] - y[d]) == 0 ) {
        return 1;
    } else {
        return 0;
    }
}

int main() {
	int n = 26;
	int i;
    int result = 0;
    int a,b,c,d;
#if 1
#ifdef _WIN32     
    scanf_s("%d", &n);
#else
    scanf("%d", &n);
#endif    
    //printf("\nn = %d\n", n);

    for(i=0; i<n; i++) {
        //printf("\ni=%d\n", i);
#ifdef _WIN32
        scanf_s("\n%c %d %d", &points_id[i], 1, &points_x[i], &points_y[i]);
#else
        scanf("\n%c %d %d", &points_id[i], &points_x[i], &points_y[i]);
#endif
        //printf("\ninput[%c %d %d]\n", points_id[i], points_x[i], points_y[i]);
    }
#endif
    for(a=0; a<n; a++) {
        for(b=a+1; b<n; b++) {
            for(c=b+1;c<n;c++) {
                for(d=c+1;d<n;d++) {
                    if( is_rect(a,b,c,d) ) {
                        //printf("a,b,c,d={%c %c %c %c}\r\n", 'A' + a, 'A' + b, 'A' + c, 'A' + d);
                        result ++;
                    }
                }
            }
        }
    }

    printf("%d", result);
	return 0;
}

/*
有两个队伍在玩捉迷藏游戏，分别为A队和B队，每队有4个人。现在轮到A队藏，B队找。现在有N个洞，他们被映射到了二维平面上。每个洞有座标以及名字（用一个大写字母表示），现在4个人要找4个不同的洞藏起来，A队想要这4个点所形成的多边形是长方形，并且四边要与座标轴平行。他们想知道有多少种不同的藏法。
输入

单组测试数据。
第一行输入一个整数N(1<=N<=26)。
接下来T行,每行一个点，先输入点的名字（一个大写字母），然后两个整数，表示座标(x,y)。(0<=x,y<=50)
每个点的名字是唯一的。

输出

输出一个整数表示答案。

输入样例

样例输入1
7
A 1 1
B 2 1
C 3 1
D 2 3
E 3 3
F 1 4
G 3 4

输出样例

样例输出1
2

*/