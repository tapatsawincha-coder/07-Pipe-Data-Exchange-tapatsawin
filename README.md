# System Programming Lab: pipe() — Data Exchange and Limitations

## 1. Learning Objectives
By the end of this lab, you should be able to:
- create an anonymous pipe with `pipe()` and fork a child process
- pass data from a parent process to a child process through a pipe
- implement bidirectional communication using two pipes
- detect end-of-stream by checking when `read()` returns 0
- handle `SIGPIPE` / `EPIPE` when there are no readers on a pipe
- close every unused file descriptor in each process

## 2. Repository Layout
- `src/pipe_lab.c`: source file you must edit (contains TODO sections)
- `include/pipe_lab.h`: function prototypes — do not change
- `scripts/`: test and grading scripts
- `tests/`: description of visible checks

## 3. What You Need To Implement
Complete the four TODO functions in `src/pipe_lab.c`.

### Task A — `task_one_way()`
One-way anonymous pipe from parent to child.

1. Create one pipe with `pipe()`
2. Fork one child process
3. **Child**: close the write end, read the message, print `child received: ping`, close the read end
4. **Parent**: close the read end, write `"ping"`, close the write end, call `waitpid()`, then print `parent: sent ping`

### Task B — `task_two_way()`
Bidirectional communication using **two** pipes (`p2c` and `c2p`).

1. Create two pipes: `p2c` (parent→child) and `c2p` (child→parent)
2. Fork one child process
3. **Child**: read the integer sent by the parent, multiply it by 2, print `child computed: 42`, send the result back through `c2p`
4. **Parent**: write the integer `21` into `p2c`, read the result from `c2p`, print `parent received: 42`

> **Key concept**: a single pipe is unidirectional; two pipes are needed for a round-trip.

### Task C — `task_eof()`
Implement EOF detection and handling.

### Task D — `task_broken_pipe()`
Handle EPIPE/SIGPIPE when writing to a closed pipe.
