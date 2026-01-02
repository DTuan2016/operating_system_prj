#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/utsname.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/syscall.h>


int main() {
    printf("==== SAFERUN SYSCALL TEST PROGRAM ====\n");

    /* 1. getpid */
    printf("\n[TEST] getpid()\n");
    pid_t pid = getpid();
    printf("  -> pid = %d\n", pid);

    /* 2. open / close */
    printf("\n[TEST] open(\"testfile.txt\")\n");
    int fd = open("testfile.txt", O_RDONLY);
    if (fd < 0)
        perror("  -> open failed");
    else {
        printf("  -> open success, fd=%d\n", fd);
        close(fd);
    }

    /* 3. write */
    printf("\n[TEST] write(STDOUT)\n");
    write(STDOUT_FILENO, "Hello from test.c\n", 18);

    /* 4. read */
    printf("\n[TEST] read(STDIN)\n");
    char buf[16];
    int r = read(STDIN_FILENO, buf, sizeof(buf));
    printf("  -> read returned %d\n", r);

    // printf("\n[TEST] read() (Testing from file instead of STDIN to avoid hanging)\n");
    // fd = open("testfile.txt", O_RDONLY);
    // char buf[16];
    // if (fd >= 0) {
    //     int r = read(fd, buf, sizeof(buf));
    //     printf("  -> read returned %d bytes\n", r);
    //     close(fd);
    //     unlink("test_temp.txt"); // Xóa file tạm
    // }

    /* 5. mmap / munmap */
    printf("\n[TEST] mmap()\n");
    void *p = mmap(NULL, 4096, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED)
        perror("  -> mmap failed");
    else {
        strcpy(p, "Hello mmap");
        printf("  -> mmap success: %s\n", (char*)p);
        munmap(p, 4096);
    }

    /* 6. socket */
    printf("\n[TEST] socket(AF_INET, SOCK_STREAM)\n");
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        perror("  -> socket failed");
    else {
        printf("  -> socket success, fd=%d\n", s);
        close(s);
    }

    /* 7. kill */
    printf("\n[TEST] kill(getpid(), 0)\n");
    if (kill(getpid(), 0) < 0)
        perror("  -> kill failed");
    else
        printf("  -> kill success\n");

    /* 8. getppid*/
    printf("\n[TEST] getppid()\n");
    printf("  -> ppid = %d\n", getppid());

    /* 9. getuid */
    printf("\n[TEST] getuid()\n");
    printf("  -> uid = %u\n", getuid());

    /* 10. getgid */
    printf("\n[TEST] getgid()\n");
    printf("  -> gid = %u\n", getgid());

    /* 11. pipe (pipe2) */
    printf("\n[TEST] pipe()\n");
    int pipefd[2];
    if (pipe(pipefd) == 0) {
        printf("  -> pipe success\n");
        close(pipefd[0]); close(pipefd[1]);
    } else {
        perror("  -> pipe failed");
    }

    /* 12. dup */
    printf("\n[TEST] dup(STDOUT)\n");
    int d = dup(STDOUT_FILENO);
    if (d >= 0) {
        printf("  -> dup success, fd=%d\n", d);
        close(d);
    } else {
        perror("  -> dup failed");
    }

    /* 13. fcntl */
    printf("\n[TEST] fcntl(F_GETFL)\n");
    int flags = fcntl(STDOUT_FILENO, F_GETFL);
    if (flags != -1) printf("  -> flags = %d\n", flags);
    else perror("  -> fcntl failed");

    /* 14. lseek */
    printf("\n[TEST] lseek()\n");
    int fd_null = open("/dev/null", O_RDONLY);
    if (fd_null >= 0) {
        if (lseek(fd_null, 0, SEEK_SET) != (off_t)-1) printf("  -> lseek success\n");
        else perror("  -> lseek failed");
        close(fd_null);
    }

    /* 15. access */
    printf("\n[TEST] access(\"/tmp\")\n");
    if (access("/tmp", F_OK) == 0) printf("  -> /tmp exists\n");
    else perror("  -> access failed");

    /* 16. readlink */
    printf("\n[TEST] readlink(/proc/self/exe)\n");
    char linkbuf[256];
    ssize_t len = readlink("/proc/self/exe", linkbuf, sizeof(linkbuf) - 1);
    if (len != -1) {
        linkbuf[len] = '\0';
        printf("  -> exe path: %s\n", linkbuf);
    } else {
        perror("  -> readlink failed");
    }

    /* 17. uname */
    printf("\n[TEST] uname()\n");
    struct utsname uts;
    if (uname(&uts) == 0) printf("  -> OS: %s %s\n", uts.sysname, uts.release);
    else perror("  -> uname failed");

    /* 18. getrlimit (prlimit64)*/
    printf("\n[TEST] getrlimit(RLIMIT_NOFILE)\n");
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == 0) printf("  -> rlimit: cur=%ld\n", rl.rlim_cur);
    else perror("  -> getrlimit failed");

    /* 19. nanosleep (clock_nanosleep)*/
    printf("\n[TEST] nanosleep()\n");
    struct timespec req = {0, 100000000}; // ngủ 0.1 giây
    if (nanosleep(&req, NULL) == 0) printf("  -> nanosleep done\n");
    else perror("  -> nanosleep failed");

    /* 20. fork / clone */
    printf("\n[TEST] fork()\n");
    pid_t c = fork();
    if (c < 0) {
        perror("  -> fork failed");
    } else if (c == 0) {
        printf("  -> child process\n");
        _exit(0);
    } else {
        wait(NULL);
        printf("  -> parent process done\n");
    }

    printf("\n==== TEST DONE ====\n");
    return 0;
}

    