The MMS includes two files: Makefile and lab3_mms.c

Run codes as follwing steps in terminal:

1) find the directory of Lab3. (cd ‘Lab3’)
2) run command "make" to create executable file of the c program.
3) run command “./lab3_mms.out 20 1 0 0 
(20 for threads#; 1 for first fit; 0 for no defragment; 0 for no request size large than Max Size.)

Then the program will return the results.

Note:
(1)All these parameters can be changed as the usage as following: 
Usage: <threads #N > <fit_type> <Defragement_enable>
<fit_type: 1. First_fit, 2. Best_fit, 3. Worst-fit>
<Defragement_enable: 0. No Defragement, 1. Defragement>
<Request_size large Max_Size: 0. No, 1. Yes>

i.e. 20 2 1 0: 20 threads, Best fit, Defragment enabled, no request size larger than Max Size. 

(2)Thought the threads number has no special limitation, to observe results better, the suggestion for first parameter of threads number is larger than 10, i.e. 20, 30 or 50. The threads number of screenshots in the report is 20. 


