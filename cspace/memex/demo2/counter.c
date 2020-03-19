#include <sys/time.h>
#include <stdio.h>

/**
 * 获取时间，us
 */
long long timeum(){

	// 表示时间的结构体，包含秒数和微秒数
    struct timeval tim; 
    gettimeofday(&tim , NULL);
    return (long long)tim.tv_sec*1000000+tim.tv_usec;
}


/**
 * 主函数，记录每次循环的时间
 */
int main()
{
    int i;
    long long start,tmp;
    start = timeum();
    for(i = 0; i < 60; ++i){
        printf("My Counter: %d\n", i);
        sleep(1);
        tmp = timeum();
        printf("Time Interval: %lld\n",tmp-start);
        start = tmp;
    }
    return 0;
}

