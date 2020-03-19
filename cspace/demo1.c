#include<stdio.h>

int max(int x,int y)
{
	if(x > y) return x;
	return y;
}

int main(void)
{
	int (*g_fun)(int,int);
	g_fun = max;
	printf("%d\n",g_fun(1,2));
}
