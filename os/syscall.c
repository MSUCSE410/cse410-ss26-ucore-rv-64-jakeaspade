#include "syscall.h"
#include "defs.h"
#include "loader.h"
#include "syscall_ids.h"
#include "timer.h"
#include "trap.h"


uint64 sys_write(int fd, char *str, uint len)
{
	debugf("sys_write fd = %d str = %x, len = %d", fd, str, len);
	if (fd != STDOUT)
		return -1;
	for (int i = 0; i < len; ++i) {
		console_putchar(str[i]);
	}
	return len;
}

__attribute__((noreturn)) void sys_exit(int code)
{
	exit(code);
	__builtin_unreachable();
}

uint64 sys_sched_yield()
{
	yield();
	return 0;
}

uint64 sys_gettimeofday(TimeVal *val, int _tz)
{
	uint64 cycle = get_cycle();
	val->sec = cycle / CPU_FREQ;
	val->usec = (cycle % CPU_FREQ) * 1000000 / CPU_FREQ;
	return 0;
}

/*
* LAB1: you may need to define sys_task_info here
*/
// 5
// Implement the sys_task_info function, which retrieves information about the current process,
// including its status, the number of times it has called each syscall,
// and the total time it has been running. This information is stored in a TaskInfo struct that is passed as an argument to the syscall.
int sys_task_info(struct TaskInfo* taskinfo)
{
	struct proc* p = curr_proc();
	
	if (taskinfo == 0) {
		return -1;
	}

	taskinfo->status = 2; // default to Running
	
	for (int i = 0; i < MAX_SYSCALL; i++) {
		taskinfo->syscall_times[i] = p->syscall_times[i];
	}

	taskinfo->time = (int)((get_cycle() - p->start_time)/ (CPU_FREQ/1000));
	return 0;
}

extern char trap_page[];

void syscall()
{
	struct trapframe *trapframe = curr_proc()->trapframe;
	// get the current process to make using it easier
	struct proc *p = curr_proc();
	int id = trapframe->a7, ret;
	uint64 args[6] = { trapframe->a0, trapframe->a1, trapframe->a2,
			   trapframe->a3, trapframe->a4, trapframe->a5 };
	tracef("syscall %d args = [%x, %x, %x, %x, %x, %x]", id, args[0],
	       args[1], args[2], args[3], args[4], args[5]);
	/*
	* LAB1: you may need to update syscall counter for task info here
	*/
		//4
		// count the syscall times for the current process, which will be used in the task info syscall to report how many times each syscall has been called by the process
		if (id >= 0 && id < MAX_SYSCALL) {
			p->syscall_times[id]++;
		}
	switch (id) {
	case SYS_write:
		ret = sys_write(args[0], (char *)args[1], args[2]);
		break;
	case SYS_exit:
		sys_exit(args[0]);
		// __builtin_unreachable();
	case SYS_sched_yield:
		ret = sys_sched_yield();
		break;
	case SYS_gettimeofday:
		ret = sys_gettimeofday((TimeVal *)args[0], args[1]);
		break;
	/*
	* LAB1: you may need to add SYS_taskinfo case here
	*/
	// 6
	// Provide an implementation for the task info syscall, which retrieves information about the current process, including its status, the number of times it has called each syscall, and the total time it has been running. This information is stored in a TaskInfo struct that is passed as an argument to the syscall.
	case SYS_task_info:
		ret = sys_task_info((struct TaskInfo*)args[0]);
		break;
	default:
		ret = -1;
		errorf("unknown syscall %d", id);
	}
	trapframe->a0 = ret;
	tracef("syscall ret %d", ret);
}
