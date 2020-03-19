#include<stdio.h>
#include<linux/unistd.h>

int main()
{
	int i = getuid();
	printf("Hello World! This is my uid: %d\n",i);
}
