#ifndef LIST_SYSCALL_H
#define LIST_SYSCALL_H
#include <stdlib.h>
#include <log.h>
#include <syscall_table.h>

typedef struct {
    long *value;
    size_t num_sys;
} list_syscall;

void init_list(list_syscall *list);
void destroy_list(list_syscall *list);
int push_to_list(list_syscall *list, long value);
int has_in_list(list_syscall *list, long value);
int parse_list(char *s, list_syscall *output);
#endif /*LIST_SYSCALL_H*/