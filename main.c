#include <monitor.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    /* Argument of help */
    if (argc > 1 && strcmp(argv[1], "-h") == 0) {
        usage(argv[0]);
        return 0;
    }
    int mode_allow = -1;
    char *liststr = NULL;
    int kill_mode = 0;
    int i = 1;
    /* Argument of deny or enable before '--' */
    while (i < argc && argv[i][0] == '-') {
        if (strncmp(argv[i], "-d:", 3) == 0) { mode_allow = 0; liststr = argv[i] + 3; i++; }
        else if (strncmp(argv[i], "-e:", 3) == 0) { mode_allow = 1; liststr = argv[i] + 3; i++; }
        else if (strcmp(argv[i], "-kill") == 0) { kill_mode = 1; i++; }
        else break;
    }
    /* After '--', run a program*/
    if (i < argc && strcmp(argv[i], "--") == 0) {
        i++;
    }
    
    if ((mode_allow == -1) || (i >= argc)) {
        usage(argv[0]); 
        return 1; 
    }

    /*========== LIST_SYSCALL ==========*/
    list_syscall rules;

    init_list(&rules);
    
    if (parse_list(liststr, &rules) != 0) {
        destroy_list(&rules);
        return 1;
    }

    log_init("monitor.log");
    LOG_INFO("saferun_ptrace starting (mode: %s)", mode_allow ? "allow" : "deny");
    fflush(stdout);

    pid_t child = fork();
    if (child < 0) {
        LOG_ERROR("fork failed: %s", strerror(errno));
        destroy_list(&rules);
        log_close();
        return 1;
    }

    if (child == 0) {
        /*========== CHILD PROCESS ==========*/
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) != 0) {
            perror("ptrace(TRACEME)");
            _exit(1);
        }
        raise(SIGSTOP);
        execvp(argv[i], &argv[i]);
        perror("execvp");
        _exit(127);
    } else {
        /*========== PARENT PROCESS ==========*/
        int status;
        if (waitpid(child, &status, 0) < 0) {
            LOG_ERROR("waitpid initial failed: %s", strerror(errno));
            destroy_list(&rules); 
            log_close();
            return 1;
        }
        if (!WIFSTOPPED(status)) {
            LOG_WARN("child did not stop as expected");
        }
        int rc = monitor_loop(child, mode_allow, &rules, kill_mode);

        destroy_list(&rules);
        log_close();
        return (rc == 0) ? 0 : 1;
    }
}
