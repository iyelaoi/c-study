#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <stdio.h>
#include <string.h>

const int long_size = sizeof(long);

/**
 * 从某个进程的某个地址读取数据
 * param：child 进程id
 * param：addr 目标地址
 */
void getdata(pid_t child, long addr, char *str, int len)
{
    char *laddr;
    int i,j;
    union u{
        long val;
        char chars[long_size];
    }data;

    i = 0;
    j = len / long_size;
    laddr = str;

    while(i < j){
    	// 读取地址对应的数据
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i*4, NULL);
        memcpy(laddr, data.chars, long_size);
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0){
        data.val = ptrace(PTRACE_PEEKDATA, child, addr + i*4, NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = ' ';
}

void putdata(pid_t child, long addr, char *str, int len)
{
    char *laddr;
    int i,j;
    union u{
        long val;
        char chars[long_size];
    }data;

    long rst; 

    i = 0;
    j = len / long_size;
    laddr = str;
    while(i < j){
        memcpy(data.chars, laddr, long_size);
        rst = ptrace(PTRACE_POKEDATA, child, addr + i*4, data.val);
        if (rst < 0) printf("Putdata Failed! \n");
        ++i;
        laddr += long_size;
    }
    j = len % long_size;
    if(j != 0){
        memcpy(data.chars, laddr, j);
        rst = ptrace(PTRACE_POKEDATA, child, addr + i*4, data.val);
        if (rst < 0) printf("Putdata Failed! \n");
    }
}

void prints(char s[], int len){
	int i = 0;
	for(i=0; i < len; i++)
    	printf("%x",s[i]);
    printf("\n");
}

int main(int argc, char *argv[])
{
    pid_t traced_process;
    struct user_regs_struct regs, newregs;
    /* int 0x80, int 3 */
    // int 0x80 : 启动系统调用
    // int 3 ： 软件中断，断点
    char code[] = {0xcd,0x80,0xcc,0};
    //char code[] = {0,0,0,0}; //TEST
    
    char backup[4]; // 原指令备份
    if(argc != 2) {
        printf("PID?\n");
        return 1;
    }
    traced_process = atoi(argv[1]); // 将进程ID转为整数
    ptrace(PTRACE_ATTACH, traced_process, NULL, NULL); // attach到目标进程
    int pid = wait(NULL); // 等待
    printf("Attach Pid: %d\n",pid);
    sleep(5);
    
    // 读取寄存器值，pid表示被跟踪的子进程，data为用户变量地址用于返回读到的数据。
    // 此功能将读取所有17个基本寄存器的值。
    ptrace(PTRACE_GETREGS, traced_process, NULL, &regs); 
    
    
    printf("source regs.rip = %x\n",regs.rip);
    /* Copy instructions into a backup variable */
    // rip/eip 为cpu将要读取的指令地址，读取原指令备份
    getdata(traced_process, regs.rip, backup, 3);
    printf("backup = ");
    prints(backup, 4);
    
    /* Put the breakpoint */
    // 往cpu将要读取的指令地址上写入自定义代码
    putdata(traced_process, regs.rip, code, 3);
    printf("写入自定义指令完成\n");
    
    /* Let the process continue and execute the int 3 instruction */
    // 写完自定义代码后，让目标进程继续执行，则会执行自定义代码
    ptrace(PTRACE_CONT, traced_process, NULL, NULL);
    printf("目标进程继续执行\n");
    wait(NULL);

    sleep(5);

    /*Segmentation fault (core dumped)
    printf("The process stopped, putting back "
            "the original instructions ");
    printf("Press <enter> to continue ");
    getchar();*/
	
	printf("now regs.rip = %x\n",regs.rip);
	// 向目标地址中写入备份，恢复原指令
    putdata(traced_process, regs.rip, backup, 3);
    printf("恢复原指令\n");
    //putdata(traced_process, regs.eip, backup, 3);  //TEST

    /* Setting the eip back to the original 
        instruction to let the process continue */
    // 恢复寄存器，使目标进程恢复至原状态
    ptrace(PTRACE_SETREGS, traced_process, NULL, &regs);
    printf("恢复寄存器\n");
    ptrace(PTRACE_GETREGS, traced_process, NULL, &regs);
    printf("end regs.rip = %x\n",regs.rip);
    // detach
    ptrace(PTRACE_DETACH, traced_process, NULL, NULL);
}
