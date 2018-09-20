The RPC includes two directories of s and c: In server dir /s, it contains ‘/test’,  ‘Makefile’ and ‘Server.c’; in client dir /c, it contains ‘Makefile’ and ‘Client.c’ 

Note: server hostname in this experiment is set to anaconda2.uml.edu. 

Run codes as follwing steps in terminal:

1) log in to client host(not anaconda2): (e.g.ssh username@anaconda14.uml.edu);
2) find the directory of ‘/Lab4/c’.
3) run command "make" to create executable file of the c program(client)

4) log in to server host(anaconda2.uml.edu). (ssh username@anaconda2.uml.edu)
5) find the directory of /Lab4/s. (cd /Lab4/s) 
6) run command "make" to create executable file of the c program.(server)

7) run command “./server ” at server host;
8) run command “./client” at client host;

Then the program will return the results.

9) run command ‘list’ at client;
10) run command ‘cd test’ (‘/test’ for testing ‘cd’, changing directory) at client;
11) run command ‘list’ again to observe the list of files in new directory;
12) run command ‘do_nothing’ at client;
13) run command ‘change’ or ’do_something’ to test error command.

before run ‘quit’, in order to observe two different clients can be served simultaneously:
14) log in to another client host(not anaconda2): (e.g.ssh username@anaconda10.uml.edu);
15) find the directory of ‘/Lab3/c’, run ‘./client’
16) repeat step 9,10, 11, 12, 13 (or selected one of them)
17) run command ‘quit’ on client hosts(anaconda14, anaconda10 in this case)

you can observe two clients quit, serve still working and if you wish you can log in other clients to test，similar steps for the above two clients.

