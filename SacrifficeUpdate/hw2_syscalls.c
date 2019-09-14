
#include <linux/sched.h>

int get_remaining_timeslice(pid_t pid){
	task_t* p = find_task_by_pid(pid);
	if(!p)
		return ESRCH;
	if(p->policy == SCHED_FIFO)
		return EINVAL;
	/*
		if(p->policy == SCHED_POOL){
			return pool_time;
		}
	
	*/
	if (p->state == TASK_ZOMBIE) 
		return 0; 
	return p->time_slice;
}

int get_total_proccecor_usage(pid_t pid){
	task_t* p = find_task_by_pid(pid);
	if(!p){
		return ESRCH;
	}
	/*
		if(p->policy == SCHED_POOL){
			return pool_time;
		}
	
	*/
	return p->proc_usage;
}

int get_total_time_in_runqueue(pid_t pid){
	task_t* p = find_task_by_pid(pid);
	if(!p)
		return ESRCH;
	
	/*
		if(p->policy == SCHED_POOL){
			return pool_time;
		}
	
	*/
	return p->total_activation_time;
}

int sacrifice_timeslice(pid_t pid){
	task_t* p = find_task_by_pid(pid);
	if(!p)
		return ESRCH;
	if(p->pid == current->pid || p->policy == SCHED_FIFO)
		return EINVAL;
	if(current->policy == SCHED_FIFO)
		return EPERM;
	p->time_slice += current->time_slice;
	int tmp = current->time_slice;
	spin_lock(&rq->lock);
	current->time_slice = 0;
	/*
		if(p->policy == SCHED_POOL){
			return pool_time;
		}
	
	*/
	if(current->policy == SCHED_OTHER){
		dequeue_task(current, rq->active);
		set_tsk_need_resched(current);
		current->prio = effective_prio(current);
		current->first_time_slice = 0;
		current->time_slice = TASK_TIMESLICE(current);

		if (!TASK_INTERACTIVE(current) || EXPIRED_STARVING(rq)) {
			if (!rq->expired_timestamp)
				rq->expired_timestamp = jiffies;
			enqueue_task(current, rq->expired);
		} else
			enqueue_task(current, rq->active);
	}
	if(current->policy == SCHED_RR){
		current->time_slice = TASK_TIMESLICE(current);
		current->first_time_slice = 0;
		set_tsk_need_resched(current);

		/* put it at the end of the queue: */
		dequeue_task(current, rq->active);
		enqueue_task(current, rq->active);
	}
	spin_unlock(&rq->lock);
	schedule();
	return tmp;
}


