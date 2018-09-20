# Process
Working with processes on the Linux platform.

## Part1

- Write a program *prog* that repeatedly creates a sub-process using ***fork***, and waits until itterminates. In particular, each child process displays *'Child running'* and call exitimmediately.

- Modify *prog* so that each child process spawns another program b using ***execve***. The programb, execute the kernel command ***cd***, then executes the command ***pwd***, followed byexecuting the command ***ls*** and then immediately exits.


## Part2

Write a program that takes a single integer argument *n* form the command line and create a set of*2\*n* processes. Each process should display the phrase *“I am process x”*, where x is the *process ID*, andthen terminates.

## Background 
A process is a program in execution. A new process is created by the fork() system call. The return valuefor the fork() equals to ‘0’ if it is child process, while the(nonzero) process identifier is returned for theparent. One of the two processes typically uses the exec() system call to replace the process’s memoryspace with a new program. The parent can issue a wait() system call to wait until the termination of thechild, avoiding creating zombie process. Because the system call to exec() overlays the process’s addressspace with a new program, the call to exec() does not return control unless an error occurs.