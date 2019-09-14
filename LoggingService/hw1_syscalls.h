#include <errno.h>
#include <termios.h>

typedef struct syscall_log_info {
	int syscall_num;
	int syscall_res;
	int time;
} log_record;

int enable_syscalls_logging(pid_t pid, int size) {
	int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (243), "b" (pid), "c"(size)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res); 
		return -1;
	}
	return 0;
}

int disable_syscalls_logging(pid_t pid) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (244), "b" (pid)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res); 
		return -1;
	}
	return 0;
}

int get_syscalls_log(pid_t pid, int size, struct syscall_log_info* user_mem) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (245), "b" (pid), "c" (size), "d" (user_mem)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res); 
		return -1;
	}
	return 0;
}

