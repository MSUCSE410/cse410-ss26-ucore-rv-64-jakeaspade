#ifndef PROC_H
#define PROC_H

#include "types.h"

#define NPROC (16)

// Safe upper limit on the number of processes to provide a safe array size for the process pool
// The actual maximum number of processes is configurable at compile time.
#define MAX_SYSCALL 500

// Saved registers for kernel context switches.
struct context {
	uint64 ra;
	uint64 sp;

	// callee-saved
	uint64 s0;
	uint64 s1;
	uint64 s2;
	uint64 s3;
	uint64 s4;
	uint64 s5;
	uint64 s6;
	uint64 s7;
	uint64 s8;
	uint64 s9;
	uint64 s10;
	uint64 s11;
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

typedef enum {
	Uninit,
	Ready,
	Running,
	Exited,
} TaskStatus;




// Every process in the Kernel needs its own memory
// Per-process state
struct proc {
	enum procstate state; // Process state
	int pid; // Process ID
	uint64 ustack; // Virtual address of user stack
	uint64 kstack; // Virtual address of kernel stack
	struct trapframe *trapframe; // data page for trampoline.S
	struct context context; // swtch() here to run process
	/*
	* LAB1: you may need to add some new fields here
	*/
	// 1
	unsigned int syscall_times[MAX_SYSCALL];  // syscall times for each syscall
	uint64 start_time; // process start time
};

/*
* LAB1: you may need to define struct for TaskInfo here
*/
// 1
// Define a struct for TaskInfo, which will be used to store information about the current process,
// including its status, the number of times it has called each syscall, and the total time it has been running.
// This struct will be used in the task info syscall to report this information to user space.
struct TaskInfo {
	TaskStatus status;
	unsigned int syscall_times[MAX_SYSCALL];
	int time;
};

struct proc *curr_proc();
void exit(int);
void proc_init();
void scheduler() __attribute__((noreturn));
void sched();
void yield();
struct proc *allocproc();
// swtch.S
void swtch(struct context *, struct context *);

#endif // PROC_H