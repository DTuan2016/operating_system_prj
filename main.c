#include <monitor.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        FILE *fp = fopen("docs.txt", "r");
        if (!fp) {
            perror("Cannot open docs/docs.txt");
            return 1;
        }

        printf(COLOR_CYAN); // Màu khung chính
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            printf("%s", line);
        }
        printf(COLOR_RESET);
        fclose(fp);
        return 0;
    }

    if (argc < 4) { usage(argv[0]); return 1; }

    /* parse CLI: accept -m mode (ignored except for compatibility),
       then either -d:list or -e:list, then optional '--', then program */
    int mode_allow = -1; /* 1 = allow, 0 = deny */
    char *liststr = NULL;
    int kill_mode = 0; /* 1 = kill on block, 0 = block and notify */
    int i = 1;

    /* optional -m mode */
    if (i < argc && strncmp(argv[i], "-m", 2) == 0) {
        /* if -m passed, accept either "-m" "ptrace" or "-mptrace" */
        if (strncmp(argv[i], "-m", 2) == 0 && argv[i][2] == ':') {
            /* not used, but keep compatibility parsing */
            i++;
        } else {
            /* skip '-m' and its argument if present */
            if (i + 1 < argc && strcmp(argv[i+1], "ptrace") == 0) i += 2;
            else i++; /* tolerate malformed */
        }
    }

    /* parse -d: or -e: or -kill until encountering '--' or program */
    while (i < argc && argv[i][0] == '-') {
        if (strncmp(argv[i], "-d:", 3) == 0) { mode_allow = 0; liststr = argv[i] + 3; i++; }
        else if (strncmp(argv[i], "-e:", 3) == 0) { mode_allow = 1; liststr = argv[i] + 3; i++; }
        else if (strcmp(argv[i], "-kill") == 0) { kill_mode = 1; i++; }
        else break;
    }
    if (i < argc && strcmp(argv[i], "--") == 0) {
        i++;
        /* Check for -kill after -- */
        if (i < argc && strcmp(argv[i], "-kill") == 0) {
            kill_mode = 1;
            i++;
        }
    }
    if (mode_allow == -1) { usage(argv[0]); return 1; }
    if (i >= argc) { usage(argv[0]); return 1; }

    /* parse list into numeric syscall IDs */
    list_syscall rules;
    init_list(&rules); // CORRECT: Initialize numlist structure
    if (parse_list(liststr, &rules) != 0) {
        destroy_list(&rules); // CORRECT: Clean up memory on failure
        return 1;
    }

    /* init logging; parent will log. open file before fork and flush before fork to avoid dup buffers */
    log_init("monitor.log");
    LOG_INFO("saferun_ptrace starting (mode: %s)", mode_allow ? "allow" : "deny");
    fflush(stdout);

    /* fork + trace */
    pid_t child = fork();
    if (child < 0) {
        LOG_ERROR("fork failed: %s", strerror(errno));
        destroy_list(&rules); // CORRECT: Clean up memory on failure
        log_close();
        return 1;
    }

    if (child == 0) {
        /* CHILD: request tracing by parent */
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) != 0) {
            perror("ptrace(TRACEME)");
            _exit(1);
        }
        /* Stop self so parent can set options and start tracing before exec */
        raise(SIGSTOP);
        execvp(argv[i], &argv[i]);
        perror("execvp");
        _exit(127);
    } else {
        /* PARENT: wait for child to stop (from raise(SIGSTOP)) */
        int status;
        if (waitpid(child, &status, 0) < 0) {
            LOG_ERROR("waitpid initial failed: %s", strerror(errno));
            destroy_list(&rules); // CORRECT: Clean up memory on failure
            log_close();
            return 1;
        }
        if (!WIFSTOPPED(status)) {
            LOG_WARN("child did not stop as expected");
        }

        /* start monitoring loop */
        int rc = monitor_loop(child, mode_allow, &rules, kill_mode);

        destroy_list(&rules); // CORRECT: Clean up memory before exit
        log_close();
        return (rc == 0) ? 0 : 1;
    }
}
