#ifndef PIPE_LAB_H
#define PIPE_LAB_H

/* Task A: one-way pipe (parent sends "ping", child prints received message) */
int task_one_way(void);

/* Task B: two-way pipe (two anonymous pipes; parent sends 21, child returns 42) */
int task_two_way(void);

/* Task C: EOF – read() returns 0 when all write ends of a pipe are closed */
int task_eof(void);

/* Task D: EPIPE – write() fails when all read ends of a pipe are closed */
int task_broken_pipe(void);

#endif