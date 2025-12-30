#ifndef SYSCALL_TABLE_H
#define SYSCALL_TABLE_H

typedef struct {
    const char *name;
    int nr;
} syscall_map;

const char* syscall_lookup_name(int nr);
int syscall_lookup_nr(const char *name);

#endif /*SYSCALL_TABLE_H*/