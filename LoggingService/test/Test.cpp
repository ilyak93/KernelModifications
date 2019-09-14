
#include "hw1_syscalls.h"
#include "staticQueue.hpp"
#include "testsMacros.h"
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sched.h>

// Global variables. Used to perform various system calls.
int GLOBAL_PID = getpid();
struct sched_param GLOBAL_SCHED_PARAM;

// Performs system call and pushes it's num+res
void intToSystemCall(unsigned int x, staticQueue<struct syscall_log_info>& queue){
	
	struct syscall_log_info info;

	switch(x%6){
		case 0:
			info.syscall_num = 20;
			info.syscall_res = GLOBAL_PID;
			getpid();
			queue.push(info);
			break;
			
		case 1:
			info.syscall_num = 64;
			info.syscall_res = getppid();
			queue.push(info);
			break;
			
		case 2:
			info.syscall_num = 158;
			info.syscall_res = sched_yield();
			queue.push(info);
			break;
		
		case 3:
			info.syscall_num = 43;
			info.syscall_res = clock();
			queue.push(info);
			break;
			
		case 4:
			info.syscall_num = 155;
			info.syscall_res = sched_getparam(GLOBAL_PID, &GLOBAL_SCHED_PARAM);
			queue.push(info);
			break;
			
		case 5:	
			info.syscall_num = 175;
			info.syscall_res = sleep(1);
			queue.push(info);
			
			info.syscall_num = 174;
			queue.push(info);
			
			info.syscall_num = 175;
			queue.push(info);
			
			info.syscall_num = 162;
			queue.push(info);
			break;
			
		default:
			assert(0);
	}
}

/*
		FILE *fp = fopen("file.txt", "r");	// 5
		fclose (fp);						// 6
*/

int stressTest(){
	// Test variables
	int result = 1;
	int pid= getpid();
	int i=0;
	int res;
	struct syscall_log_info info;
	
	const int NUM_OF_OPS = 200;
	
	struct syscall_log_info* pBuff = new struct syscall_log_info[NUM_OF_OPS];
	
	unsigned int ops[NUM_OF_OPS];
	for (i=0 ; i<NUM_OF_OPS ; ++i)
		ops[i] = rand();
	
	staticQueue<struct syscall_log_info> queue(NUM_OF_OPS);
	
	info.syscall_num = 243;
	info.syscall_res = 0;
	queue.push(info);
	
	// Begin logging. Add call #0
	enable_syscalls_logging(pid, NUM_OF_OPS);		// 243
	
	for (i=0 ; i<NUM_OF_OPS ; ++i)
		intToSystemCall(ops[i], queue);
	
	// Try reading calls
	res = get_syscalls_log(pid, NUM_OF_OPS, pBuff);	// 245
	assert(res == 0);
	
	res = disable_syscalls_logging(pid);
	assert(res == 0);
	
	delete[] (pBuff);
	
	return result;
}

int checkSmallBuffer(){
	// Test variables
	int result = 1;
	struct syscall_log_info buff[1];
	int pid= getpid();
	int i=0;
	int res;
	
	// Begin logging. Add call #0
	enable_syscalls_logging(pid, 1);		// 243
		
	// Add calls #0-7	
	for(i=0 ; i<4 ; ++i){	
		sched_yield();						// 158
		getppid();							// 64
	}
	
	// Try reading 12 calls (Should succeed)
	res = get_syscalls_log(pid, 1, buff);	// 245
	assert(res == 0);
	
	res = disable_syscalls_logging(pid);
	assert(res == 0);
	
	return result;
}

int checkReturnValues(){
	// Test variables
	int result = 1;
	struct syscall_log_info buff[9];
	int pid= getpid();
	int i=0;
	int res;
	
	// Begin logging. Add call #0
	enable_syscalls_logging(pid, 8);		// 243
		
	// Add calls #0-7	
	for(i=0 ; i<4 ; ++i){	
		sched_yield();						// 158
		getppid();							// 64
	}
	
	// Try reading 12 calls (Should fail)
	res = get_syscalls_log(pid, 12, buff);	// 245
	assert(res == -1);
	
	assert(buff[0].syscall_num == 245);

	
	res = disable_syscalls_logging(pid);
	assert(res == 0);
	
	return result;
}

int wrapupBufferB(){
	// Test variables
	int result = 1;
	struct syscall_log_info buff[9];
	int pid= getpid();
	int i=0;
	int res;
	
	// Begin logging. Add call #0
	enable_syscalls_logging(pid, 8);		// 243
		
	// Add calls #0-7	
	for(i=0 ; i<4 ; ++i){	
		sched_yield();						// 158
		getppid();							// 64
	}
		
	// Try reading 4 calls (Should succeed)
	res = get_syscalls_log(pid, 4, buff);	// 245
	assert(res == 0);

	// Check log
	for(i=0 ; i<2 ; ++i){	
		assert(buff[2*i].syscall_num == 158); 
		assert(buff[1+2*i].syscall_num == 64);
	}

	// Try reading 4 calls (Should succeed)
	res = get_syscalls_log(pid, 4, buff);	// 245
	assert(res == 0);

	// Check log
	for(i=0 ; i<2 ; ++i){	
		assert(buff[2*i].syscall_num == 158); 
		assert(buff[1+2*i].syscall_num == 64);
	}
	
	// Try reading 2 calls (Should succeed)
	res = get_syscalls_log(pid, 2, buff);	// 245
	assert(res == 0);

	// Check log
	for(i=0 ; i<2 ; ++i){	
		assert(buff[i].syscall_num == 245); 
	}
	
	res = disable_syscalls_logging(pid);
	assert(res == 0);
	
	return result;
}

int wrapupBuffer(){
	// Test variables
	int result = 1;
	struct syscall_log_info buff[9];
	int pid= getpid();
	int i=0;
	int res;
	
	// Begin logging. Add call #0
	enable_syscalls_logging(pid, 9);		// 243
		
	// Add calls #1-9 	
	for(i=0 ; i<4 ; ++i){	
		sched_yield();						// 158
		getppid();							// 64
	}
	
	getpid(); 								// 20
	
	// Try reading 9 calls (Should succeed)
	res = get_syscalls_log(pid, 9, buff);	// 245
	assert(res == 0);

	// Check log
	for(i=0 ; i<4 ; ++i){	
		//printf("buff[3*i].syscall_num = %d\n", buff[3*i].syscall_num);
		assert(buff[2*i].syscall_num == 158); 
		assert(buff[1+2*i].syscall_num == 64);
	}
	assert(buff[8].syscall_num == 20);
	
	res = disable_syscalls_logging(pid);
	assert(res == 0);
	
	return result;
}

int fillBuffer(){
	// Test variables
	int result = 1;
	struct syscall_log_info buff[10];
	int pid= getpid();
	int i=0;
	int res;
	
	// Begin logging. Add call #0
	enable_syscalls_logging(pid, 10);		// 243
		
	// Add calls #1-8 	
	for(i=0 ; i<4 ; ++i){	
		sched_yield();						// 158
		getpid(); 							// 20
	}
	res = get_syscalls_log(pid, 8, buff); 

	// Try reading 10 calls (Should fail)
	res = get_syscalls_log(pid, 10, buff);	// 245
	assert(res == -1);
	assert(errno == EINVAL);
	
	// Try reading 10 calls (Should succeed)
	res = get_syscalls_log(pid, 10, buff);	// 245
	assert(res == 0);
	
	// Check log
	assert(buff[0].syscall_num == 243); 	
	for(i=0 ; i<4 ; ++i){	
		assert(buff[1+2*i].syscall_num == 158); 
		assert(buff[2+2*i].syscall_num == 20); 
	}
	assert(buff[9].syscall_num == 245); 	
 	
	res = disable_syscalls_logging(pid);
	assert(res == 0);
	
	return result;
}

int checksleep(){
	// Test variables
	int result = 1;
	struct syscall_log_info buff[20];
	int pid= getpid();
	int res;
	int i;
	
	for (i=0 ; i<20 ; ++i){
		buff[i].syscall_num = 0;
		buff[i].syscall_res = 0;
		buff[i].time = 0;
	}
	
	// Begin logging
	enable_syscalls_logging(pid, 20);		// 243
		
	// Perform syscall
	sleep(1); 								// 20
	
	assert(get_syscalls_log(pid, 5, buff) == 0);

	assert(buff[0].syscall_num == 243);
	assert(buff[1].syscall_num == 175);
	assert(buff[2].syscall_num == 174);
	assert(buff[3].syscall_num == 175);
	assert(buff[4].syscall_num == 162);
	
	res = disable_syscalls_logging(pid);
	assert(res == 0);

	// Verify exactly 5
	enable_syscalls_logging(pid, 20);		// 243
	sleep(1); 								// 20
	assert(get_syscalls_log(pid, 6, buff) == -1);
	
	res = disable_syscalls_logging(pid);
	assert(res == 0);
	
	return result;
}

int main(){
	
	printf("Test begins\n");
	
	RUN_TEST(fillBuffer)
	RUN_TEST(fillBuffer)
	RUN_TEST(wrapupBuffer)
	RUN_TEST(wrapupBuffer)
	RUN_TEST(wrapupBufferB)
	RUN_TEST(checkReturnValues)
	RUN_TEST(checkSmallBuffer)
	RUN_TEST(checksleep)
	RUN_TEST(stressTest)
	
	printf("Test passed! :)\n");
	
	return 0;
}
