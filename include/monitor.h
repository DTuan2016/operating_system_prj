#ifndef MONITOR_H
#define MONITOR_H
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <log.h>
#include <signal.h>
#include <errno.h>
#include <list_syscall.h> 
#include <syscall_table.h>

#define SYSCALL_STOP 0x80

void usage(char *p);
int monitor_loop(pid_t child, int mode_allow, list_syscall *list);

#endif /*MONITOR_H*/