#include <sys/ptrace.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
#include <sys/user.h> 
#include <stdio.h> 

int main(int argc, char *argv[]) 
{ 
	pid_t traced_process; // 被跟踪进程pid 
	pid_t sub_process_id;
	struct user_regs_struct regs; 
	long ins; 
	if(argc != 2) 
	{ 
		printf("PID?"); 
		return 1; 
	} 
	traced_process = atoi(argv[1]);  // 将字符串转换成一个整数
	printf("ready attach to : %d\n",traced_process);
	ins = ptrace(PTRACE_ATTACH, traced_process, NULL, NULL);  // attach到目标进程
	printf("attach over, return :%lx\n",ins);
	sleep(2);
	printf("sleep(2) over, now wait(NULL)\n");
	sub_process_id = wait(NULL); // 等待子进程回收，返回子进程id
	printf("wait(NULL) over,return sub_process_id: %d\n",sub_process_id);
	sleep(2);
	ptrace(PTRACE_GETREGS, traced_process, NULL, &regs); // PTRACE_GETREGS i386特有的，读取寄存器
	
	// PTRACE_PEEKTEXT, PTRACE_PEEKDATA    
	//从子进程内存空间 addr 指向的位置读取一个字，并作为调用的结果返回
	ins = ptrace(PTRACE_PEEKTEXT, traced_process, regs.rip, NULL);
	printf("regs.rip address: %lx , a DWORD content: %lx \n", regs.rip, ins); 
	ptrace(PTRACE_DETACH, traced_process, NULL, NULL); // detach
	return 0; 
}

