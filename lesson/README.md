# System Programming Lab: pipe() and FIFO

## 1. Learning Objectives
By the end of this lab, students should be able to:
- create a unidirectional pipe with `pipe()`
- send data from a parent to a child process
- close unused file descriptors correctly
- wait for child termination with `waitpid()`

## 2. What Is In This Folder
- `pipeRead.c`, `pipeWrite.c`, `fifo.c`, `popenRead.c`, `popenWrite.c`: instructor demos
- `starter/`: GitHub Classroom starter template for students

## 3. GitHub Classroom Starter
Copy `starter/` into a dedicated template repository, then select that repository as the starter code for your GitHub Classroom assignment.