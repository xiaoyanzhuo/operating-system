# Operating System
Mainly includes programming practices: process, synchronization of threads(mutux and semaphore), memory management and client/server model. 

## Process
Work with processes, using *folk, execve*, create mutiple process and print process ID, etc. 


## Multi-threads (Synchronization)
Multithreading program solving the provider-buyer (or producer-consumer) problem to understand the synchronization
of threads with two methods, i.e., *mutex, semaphore*. Make use of *pthreads* of Linux. 

## Memory Management
Implement a memory management system(MMS). MMS will handle all requests of allocation of memory
space by different users. Memory allocation algorithms implemented: *First Fit, Best Fit, Worst Fit*.


## Client/Server

Implement a simple client-server model to exchange files/commands over a network to mimic some part of the functionality of a distributed scheduler (*First Come First Served*) as well as to mimic a Remote Procedure Call (RPC), using sockets programming to implement a simple RPC framework. 

- Serve serves multiple clients concurrently; 
- Add timeout in client

## Usage
- Review "*.md"(such as *Process.md* in *Process* dir) to get details and background information about the program. 
- To run program:
	- Run *Makefile* to obtain exectuable file
	- Run exectuable file to observe results
	- More details can be found *ReadMe.txt* for each program.

