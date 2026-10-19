#include "pipe_lab.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * Task A – one-way pipe
 *
 * The parent writes the string "ping" into a pipe.
 * The child reads it and prints exactly:
 *   child received: ping
 * The parent then waits for the child and prints exactly:
 *   parent: sent ping
 *
 * Required steps in the child:
 *   1. close fd[1] (unused write end)
 *   2. read the message into a buffer
 *   3. null-terminate the buffer and print "child received: <message>"
 *   4. close fd[0]
 *
 * Required steps in the parent:
 *   1. close fd[0] (unused read end)
 *   2. write the string "ping" into fd[1]
 *   3. close fd[1]
 *   4. waitpid() for the child
 *   5. print "parent: sent ping"
 */
int task_one_way(void) {
    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        /* TODO: implement child logic (steps 1-4 above) */
        (void)fd;
        fprintf(stderr, "TODO: task_one_way child not implemented\n");
        return 1;
    }

    /* TODO: implement parent logic (steps 1-5 above) */
    (void)fd;
    fprintf(stderr, "TODO: task_one_way parent not implemented\n");
    return 1;
}

/*
 * Task B – two-way pipe
 *
 * Pipes are unidirectional, so bidirectional communication needs TWO pipes:
 *   p2c[2]: parent writes → child reads
 *   c2p[2]: child writes  → parent reads
 *
 * The parent sends the integer 21 through p2c.
 * The child reads it, doubles it to 42, and prints exactly:
 *   child computed: 42
 * The child sends the result back through c2p.
 * The parent reads the result and prints exactly:
 *   parent received: 42
 *
 * Required: close every pipe end that is not used in each process.
 * Hint: write/read a raw int with write(fd, &val, sizeof(val)).
 */
int task_two_way(void) {
    int p2c[2], c2p[2];
    if (pipe(p2c) == -1 || pipe(c2p) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        /* TODO: implement child logic */
        (void)p2c; (void)c2p;
        fprintf(stderr, "TODO: task_two_way child not implemented\n");
        return 1;
    }

    /* TODO: implement parent logic */
    (void)p2c; (void)c2p;
    fprintf(stderr, "TODO: task_two_way parent not implemented\n");
    return 1;
}

/*
 * Task C – EOF limitation
 *
 * When every write end of a pipe is closed, the next read() returns 0.
 * This is how a reader detects end-of-stream (EOF).
 *
 * Sequence:
 *   parent: write "hello" (5 bytes) into the pipe, then CLOSE the write end
 *   child:  read in a loop until read() returns 0; track total bytes
 *           print exactly: child: got EOF after 5 bytes
 *
 * The loop must check the return value of read() on every iteration.
 * The child should NOT assume a single read delivers all the bytes.
 */
int task_eof(void) {
    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        /* TODO: close fd[1], read in a loop until read() == 0,
                 print "child: got EOF after <n> bytes", close fd[0] */
        (void)fd;
        fprintf(stderr, "TODO: task_eof child not implemented\n");
        return 1;
    }

    /* TODO: close fd[0], write "hello", close fd[1], waitpid */
    (void)fd;
    fprintf(stderr, "TODO: task_eof parent not implemented\n");
    return 1;
}

/*
 * Task D – SIGPIPE / EPIPE limitation
 *
 * Writing to a pipe with no readers delivers SIGPIPE to the writer,
 * killing it unless the signal is ignored.  With SIGPIPE ignored,
 * write() returns -1 and sets errno to EPIPE.
 *
 * A synchronisation pipe (sync_fd) is used so the parent only writes
 * after the child has already closed the read end of data_fd.
 *
 * Required sequence:
 *   child:  close data_fd[0] and data_fd[1]
 *           write one byte to sync_fd[1] (ack), close sync_fd[1], exit
 *   parent: close sync_fd[1]
 *           read one byte from sync_fd[0] (blocks until child sends ack)
 *           close sync_fd[0] and data_fd[0]
 *           call signal(SIGPIPE, SIG_IGN)
 *           write one byte to data_fd[1]; check if errno == EPIPE
 *           print exactly: parent: broken pipe detected
 *           close data_fd[1], waitpid
 */
int task_broken_pipe(void) {
    int data_fd[2];
    int sync_fd[2];
    if (pipe(data_fd) == -1 || pipe(sync_fd) == -1) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        /* TODO: implement child logic (see description above) */
        (void)data_fd; (void)sync_fd;
        fprintf(stderr, "TODO: task_broken_pipe child not implemented\n");
        return 1;
    }

    /* TODO: implement parent logic (see description above) */
    (void)data_fd; (void)sync_fd;
    fprintf(stderr, "TODO: task_broken_pipe parent not implemented\n");
    return 1;
}

/* ------------------------------------------------------------------ */
int main(void) {
    if (task_one_way() != 0)     { fprintf(stderr, "task_one_way failed\n");     return 1; }
    if (task_two_way() != 0)     { fprintf(stderr, "task_two_way failed\n");     return 1; }
    if (task_eof() != 0)         { fprintf(stderr, "task_eof failed\n");         return 1; }
    if (task_broken_pipe() != 0) { fprintf(stderr, "task_broken_pipe failed\n"); return 1; }
    printf("all tasks done\n");
    return 0;
}
