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
    int center_x_4 = (x[a] + x[b] + x[c] + x[d]); /* �������������ô�죿 */
    int center_y_4 = (y[a] + y[b] + y[c] + y[d]); /* ���ﲻ����4 */
    
    da = get_dis_4_2(a, center_x_4, center_y_4);  /* ��õľ���ƽ����û�г���4�� */
    db = get_dis_4_2(b, center_x_4, center_y_4);  /* ��õ��Ǿ���*4��ƽ�� */
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
��������������׽�Բ���Ϸ���ֱ�ΪA�Ӻ�B�ӣ�ÿ����4���ˡ������ֵ�A�Ӳأ�B���ҡ�������N���������Ǳ�ӳ�䵽�˶�άƽ���ϡ�ÿ�����������Լ����֣���һ����д��ĸ��ʾ��������4����Ҫ��4����ͬ�Ķ���������A����Ҫ��4�������γɵĶ�����ǳ����Σ������ı�Ҫ��������ƽ�С�������֪���ж����ֲ�ͬ�Ĳط���
����

����������ݡ�
��һ������һ������N(1<=N<=26)��
������T��,ÿ��һ���㣬�����������֣�һ����д��ĸ����Ȼ��������������ʾ����(x,y)��(0<=x,y<=50)
ÿ�����������Ψһ�ġ�

���

���һ��������ʾ�𰸡�

��������

��������1
7
A 1 1
B 2 1
C 3 1
D 2 3
E 3 3
F 1 4
G 3 4

�������

�������1
2

*/