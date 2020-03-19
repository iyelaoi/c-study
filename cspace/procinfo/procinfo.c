#include<stdio.h>

int main(int argc,char *argv[])
{
	
	char c1,c2;

	if(argc>1)
	{
		sscanf(argv[1],"%c%c",&c1,&c2);
		if(c1 != '-'){
			fprintf(stderr,"usage:kinfo[-i int -d dur]\n");
			exit(1);
		}
		if(c2 == 'i'){
			interval = atoi(argv[2]);	
		}
	}
}
