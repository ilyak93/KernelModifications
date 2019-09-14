
#include <linux/sched.h>


typedef struct syscall_log_info {
	int syscall_num;
	int syscall_res;
	int time;
} log_record;


int log_info(void){
int sys_call_num, ret_val;
asm("\t movl %%eax,%0" :"=r"(ret_val));
asm("\t movl %%edx,%0" :"=r"(sys_call_num));
int pid = getpid();
task_t* p = find_task_by_pid();
if (p == NULL){
		return -ESRCH;
	}

if(p->status==ALLOW_LOG){
	((log_record*)(p->log))[p->last].syscall_res = ret_val;
	((log_record*)(p->log))[p->last].syscall_num = sys_call_num;
	((log_record*)(p->log))[p->last].time = jiffies;
	p->last = (p->last+1)%(p->size);
	if (p->current_size == p->size){
		p->first = (p->first + 1)%(p->size);
		printk("\n the current_size is : %d \n", p->current_size);
	} else {
		p->current_size++;
		printk("\n the current_size is : %d \n", p->current_size);
	}

}

return 0;
}

