#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    printf("Test program starting...\n");

    // Gọi một vài syscall cơ bản
    printf("My PID: %d\n", getpid());

    // Gọi fork() để test chính sách chặn
    pid_t pid = fork();

    if (pid == 0) {
        // Child process
        printf("Child process running! (PID=%d)\n", getpid());
        _exit(0);
    } else if (pid > 0) {
        // Parent process
        printf("Parent process: created child %d\n", pid);
    } else {
        perror("fork");
    }

    printf("Test program finished.\n");
    return 0;
}
