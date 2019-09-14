#include <errno.h>
#include <termios.h>


int get_remaining_timeslice(pid_t pid) {
	int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (246), "b" (pid)
		:"memory"
		);
	if ((__res) < 0) {

		errno = (-__res); 
		return -1;
	}
	return __res;
}

int get_total_processor_usage(pid_t pid) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (247), "b" (pid)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res); 
		return -1;
	}
	return __res;
}

int get_total_time_in_runqueue(pid_t pid) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (248), "b" (pid)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res); 
		return -1;
	}
	return __res;
}

int sacrifice_timeslice(pid_t pid) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (249), "b" (pid)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res); 
		return -1;
	}
	return __res;
}


int search_pool_level(pid_t pid, int level) {
    int __res;
	__asm__(
		"int $0x80;"
		: "=a" (__res)
		: "0" (250), "b" (pid), "c" (level)
		:"memory"
		);
	if ((__res) < 0) {
		errno = (-__res); 
		return -1;
	}
	return __res;
}

