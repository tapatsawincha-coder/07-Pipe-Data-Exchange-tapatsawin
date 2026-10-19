#include "pipe_lab.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

/* Transfer exactly size bytes, handling interrupted/partial operations. */
static int transfer(int fd, void *buffer, size_t size, int writing)
{
    char *p = buffer;

    while (size > 0) {
        ssize_t n = writing ? write(fd, p, size) : read(fd, p, size);

        if (n < 0 && errno == EINTR)
            continue;

        if (n <= 0)
            return 1;

        p += n;
        size -= (size_t)n;
    }

    return 0;
}

static int wait_child(pid_t pid)
{
    int status;

    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR)
            return 1;
    }

    return !WIFEXITED(status) || WEXITSTATUS(status) != 0;
}

/* Task A: parent -> child */
int task_one_way(void)
{
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    fflush(NULL);
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (pid == 0) {
        char message[5] = {0};

        close(fd[1]);
        int failed = transfer(fd[0], message, 4, 0);
        close(fd[0]);

        if (!failed)
            printf("child received: %s\n", message);

        fflush(stdout);
        _exit(failed);
    }

    char message[] = "ping";

    close(fd[0]);
    int failed = transfer(fd[1], message, 4, 1);
    close(fd[1]);

    int child_failed = wait_child(pid);
    if (failed || child_failed)
        return 1;

    printf("parent: sent ping\n");
    return 0;
}

/* Task B: parent -> child -> parent */
int task_two_way(void)
{
    int p2c[2], c2p[2];

    if (pipe(p2c) == -1) {
        perror("pipe");
        return 1;
    }

    if (pipe(c2p) == -1) {
        perror("pipe");
        close(p2c[0]);
        close(p2c[1]);
        return 1;
    }

    fflush(NULL);
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        close(p2c[0]);
        close(p2c[1]);
        close(c2p[0]);
        close(c2p[1]);
        return 1;
    }

    if (pid == 0) {
        int value;

        close(p2c[1]);
        close(c2p[0]);

        int failed = transfer(p2c[0], &value, sizeof(value), 0);
        close(p2c[0]);

        if (!failed) {
            value *= 2;
            printf("child computed: %d\n", value);
            fflush(stdout);
            failed = transfer(c2p[1], &value, sizeof(value), 1);
        }

        close(c2p[1]);
        _exit(failed);
    }

    int value = 21;

    close(p2c[0]);
    close(c2p[1]);

    int failed = transfer(p2c[1], &value, sizeof(value), 1);
    close(p2c[1]);

    if (!failed)
        failed = transfer(c2p[0], &value, sizeof(value), 0);

    close(c2p[0]);

    int child_failed = wait_child(pid);
    if (failed || child_failed)
        return 1;

    printf("parent received: %d\n", value);
    return 0;
}

/* Task C: read until EOF */
int task_eof(void)
{
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    fflush(NULL);
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return 1;
    }

    if (pid == 0) {
        char buffer[16];
        size_t total = 0;
        int failed = 0;

        close(fd[1]);

        for (;;) {
            ssize_t n = read(fd[0], buffer, sizeof(buffer));

            if (n > 0) {
                total += (size_t)n;
            } else if (n == 0) {
                printf("child: got EOF after %zu bytes\n", total);
                break;
            } else if (errno != EINTR) {
                perror("read");
                failed = 1;
                break;
            }
        }

        close(fd[0]);
        fflush(stdout);
        _exit(failed);
    }

    char message[] = "hello";

    close(fd[0]);
    int failed = transfer(fd[1], message, 5, 1);
    close(fd[1]); /* Closing the last write end allows EOF. */

    int child_failed = wait_child(pid);
    return failed || child_failed;
}

/* Task D: writing when no readers exist */
int task_broken_pipe(void)
{
    int data_fd[2], sync_fd[2];

    if (pipe(data_fd) == -1) {
        perror("pipe");
        return 1;
    }

    if (pipe(sync_fd) == -1) {
        perror("pipe");
        close(data_fd[0]);
        close(data_fd[1]);
        return 1;
    }

    fflush(NULL);
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        close(data_fd[0]);
        close(data_fd[1]);
        close(sync_fd[0]);
        close(sync_fd[1]);
        return 1;
    }

    if (pid == 0) {
        char ack = 'x';

        close(data_fd[0]);
        close(data_fd[1]);
        close(sync_fd[0]);

        int failed = transfer(sync_fd[1], &ack, 1, 1);
        close(sync_fd[1]);
        _exit(failed);
    }

    char ack;
    int failed;

    close(sync_fd[1]);
    failed = transfer(sync_fd[0], &ack, 1, 0);
    close(sync_fd[0]);
    close(data_fd[0]);

    if (!failed) {
        void (*old_handler)(int) = signal(SIGPIPE, SIG_IGN);

        if (old_handler == SIG_ERR) {
            perror("signal");
            failed = 1;
        } else {
            ssize_t n;

            do {
                n = write(data_fd[1], "x", 1);
            } while (n == -1 && errno == EINTR);

            if (n == -1 && errno == EPIPE)
                printf("parent: broken pipe detected\n");
            else
                failed = 1;

            if (signal(SIGPIPE, old_handler) == SIG_ERR)
                failed = 1;
        }
    }

    close(data_fd[1]);

    int child_failed = wait_child(pid);
    return failed || child_failed;
}

int main(void)
{
    if (task_one_way() != 0) {
        fprintf(stderr, "task_one_way failed\n");
        return 1;
    }

    if (task_two_way() != 0) {
        fprintf(stderr, "task_two_way failed\n");
        return 1;
    }

    if (task_eof() != 0) {
        fprintf(stderr, "task_eof failed\n");
        return 1;
    }

    if (task_broken_pipe() != 0) {
        fprintf(stderr, "task_broken_pipe failed\n");
        return 1;
    }

    printf("all tasks done\n");
    return 0;
}