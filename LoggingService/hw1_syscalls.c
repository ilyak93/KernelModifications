#include <linux/sched.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include "../arch/i386/kernel/hw1_logging.c"


int sys_enable_syscalls_logging(pid_t pid, int size){
	if (pid<0){
		return -ESRCH;
	}

	task_t* p = find_task_by_pid(pid);
	if (p == NULL){
		return -ESRCH;
	}

	if (size<0){
		return -EINVAL;
	}

	if (p->status == ALLOW_LOG){
		return -EINVAL;
	}

	p->log = kmalloc(sizeof(log_record)*size,GFP_KERNEL);
	if (p->log == NULL){
		return -ENOMEM;
	}
	p->status = ALLOW_LOG;
	p->size = size;

	return 0;
}


int sys_disable_syscalls_logging(pid_t pid){
	if (pid<0){
		return -ESRCH;
	}

	task_t* p = find_task_by_pid(pid);
	if(p == NULL){
		return -ESRCH;
	}

	if(p->status  != ALLOW_LOG){
		return -EINVAL;
	}

	kfree(p->log);
	p->status = BLOCK_LOG;
	p->log = NULL;
	p->size = 0;
	p->first = 0;
	p->last = 0;
	p->current_size=0;

	return 0;
}

int sys_get_syscalls_log(pid_t pid, int size, struct syscall_log_info* user_mem){
	if (pid<0){
		return -ESRCH;
	}

	task_t* p = find_task_by_pid(pid);
	if(p == NULL){
		return -ESRCH;
	}

	if (size>p->current_size){
		return -EINVAL;
	}

	if (size<0){
		return -EINVAL;
	}

	if(p->status == BLOCK_LOG){
		return -EINVAL;
	}

	log_record* temp_arr = kmalloc(sizeof(log_record)*size, GFP_KERNEL);
	if(temp_arr == NULL){
		return -ENOMEM;
	}

	int j=0;
	int i=p->first;
	int c=size;
	for (;c>0 ; i=(++i)%(p->size)){
		temp_arr[j] =((log_record*)(p->log))[i];
		j++;
		c--;
	}

	p->current_size -= size;
	p->first = (p->first + size) % (p->size);


	int copy_res = copy_to_user(user_mem, temp_arr, size*sizeof(log_record));
	if(copy_res!=0){
		return -ENOMEM;
	}
	kfree(temp_arr);

	return 0;
}
