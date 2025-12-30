#include <monitor.h>
#include <string.h>

void usage(char *p){
    fprintf(stderr,
        "Usage: %s (-d:sys1,sys2 | -e:sys1,sys2) -- program [args...]\n"
        "  -d: deny list (block listed syscalls)\n"
        "  -e: allow list (only listed syscalls allowed; others blocked)\n"
        "  -m mode (optional, 'ptrace' default)\n"
        "Examples:\n"
        "  %s -d:open,fork -- /bin/ls -l\n"
        "  %s -e:read,write,exit -- /path/to/suspected\n",
        p, p, p);
}

int monitor_loop(pid_t child, int mode_allow, list_syscall *list){
    int status;
    struct user_regs_struct regs;
    int in_syscall = 0;

    if(ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD) != 0){
        LOG_WARN("PTRACE_SYSCALL init: %s", strerror(errno));
        return -1;
    }

    if(ptrace(PTRACE_SYSCALL, child, NULL, NULL) != 0){
        LOG_ERROR("PTRACE_SYSCALL start failed: %s", strerror(errno));
        return -1;
    }

    while(1){
        if(waitpid(child, &status, 0) < 0){
            if(errno == EINTR){
                continue;
            }
            LOG_ERROR("waitpid failed: %s", strerror(errno));
            return -1;
        }

        if(WIFEXITED(status)){
            LOG_INFO("Child exited with code %d", WEXITSTATUS(status));
            return 0;
        }

        if(WIFSIGNALED(status)){
            LOG_INFO("Child killed by signal %d", WTERMSIG(status));
        }

        if(!WIFSTOPPED(status)){
            continue;
        }

        int sig = WSTOPSIG(status);
        if(sig == SIGTRAP){
            if(ptrace(PTRACE_SYSCALL, child, NULL, NULL) != 0){
                LOG_ERROR("PTRACE_SYSCALL after SIGTRAP: %s", strerror(errno));
                return -1;
            }
            continue;
        }

        if(sig == (SIGTRAP | SYSCALL_STOP) || (sig & SYSCALL_STOP)){
            if(ptrace(PTRACE_GETREGS, child, NULL, &regs) != 0){
                LOG_ERROR("PTRACE_GETREGS failed: %s", strerror(errno));
                return -1;
            }

            if(!in_syscall){
                long sc = regs.orig_rax;

                const char *name = syscall_lookup_name(sc); 
                if(!name) name = "unknown";

                LOG_INFO("PID %d syscall #%ld (%s)", child, sc, name);

                int block = 0;

                if(mode_allow){
                    if(!has_in_list(list, sc)){
                        block = 1;
                    }
                }
                else{
                    if(has_in_list(list, sc)){
                        block = 1;
                    }
                }

                if(block){
                    LOG_ERROR("Policy block syscall #%ld (%s) - killing child %ld", sc, name, (long)child);
                    // if(ptrace(PTRACE_KILL, child, NULL, NULL) != 0){
                    //     LOG_WARN("ptrace(KILL) failed: %s", strerror(errno));
                    //     kill(child, SIGKILL);
                    // }
                    // kill(child, SIGKILL);
                    // waitpid(child, &status, 0);
                    // return 1;
                    regs.rax = -EPERM;                        // trả lỗi cho user program
                    ptrace(PTRACE_SETREGS, child, NULL, &regs);

                    in_syscall = 1;                            // đánh dấu đang ở exit-phase của syscall

                    ptrace(PTRACE_SYSCALL, child, NULL, NULL); // tiếp tục theo dõi
                    continue;                          
                }
                in_syscall = 1;
            }
            else{
                in_syscall = 0;
            }

            if(ptrace(PTRACE_SYSCALL, child, NULL, NULL) != 0) {
                LOG_ERROR("PTRACE_SYSCALL loop: %s", strerror(errno));
                return -1;
            }
        }
        else{
            if (ptrace(PTRACE_CONT, child, NULL, (void*)(long)sig)!= 0){
                LOG_ERROR("PTRACE_SYSCALL forward: %s", strerror(errno));
                return -1;
            }
        }
    }
}