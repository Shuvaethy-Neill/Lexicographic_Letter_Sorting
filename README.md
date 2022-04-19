# Lexicographic_Letter_Sorting
**Concurrent program using Linux semaphores and shared memory!**

--------------------------------------------
DESCRIPTION
--------------------------------------------
This concurrent program consists of three processes that sort a list of seven distinct letters (inputted by the user) 
into lexicographic order and converts any uppercase letters to lowercase. This design uses **Linux shared memory** and 
**Linux semaphores** to do so and ensure mutual exclusion.

The parent process will fork three child processes P1, P2, P3. Each of the child processes is associated with letters in the list.
P1 is associated with letter 1, 2, 3. P2 is associated with letter 3,4, 5. P3 is associated with letter 5, 6, 7. Every child 
process repeatedly checks its three letters until the entire list of letters has been sorted. When a process finds that two of 
the three letters it is associated with is out of lexicographic order, it swaps the position of both of them. When the sorting
is complete, the list of letters is printed in the correct lexicographic order.

The program can also be run in debug mode. When in this mode, the program will print additional information about the sorting 
process. Each iteration of the child processes will print out "Process Pi: performed swapping" or "Process Pi: No swapping"
(where i = 1, 2, or 3 to identify the child process).

If a user enters a duplicate letter for the letter list or a number other than 0/1 for the debug mode, the program will end and the 
user will need to try again with a valid input.


#### Input of the program:
- list of seven distinct letters

#### Output of the program:
- the sorted list of letters in lexicographic order
- debugging statements of the sorting process (if debug mode has been chosen)

#### The following three files are included:  
- CSORT.c
- shared_memory.h
- Makefile

--------------------------------------------
INSTALLATION & SETUP
--------------------------------------------
You must have access to a Linux operating system in order to execute this program. 

If you do not have Linux installed on your machine, you can also create and use a virtual machine:
1. Download Oracle VirtualBox and create virtual machine
    - Download link: https://www.virtualbox.org/wiki/Download_Old_Builds_6_0
2. Download Fedora 30 ISO in the newly created virtual machine to be able to run Linux
    - Download link: https://archives.fedoraproject.org/pub/archive/fedora/linux/releases/30/Workstation/x86_64/iso/

--------------------------------------------
USAGE
--------------------------------------------
Navigate to the directory in which these files are stored. You can do this using the cd command:

    cd directory_name
    
Ensure that you have the CSORT.c source file, supporting shared_memory.h header file, and Makefile stored in the same directory. You 
can verify this by using the ls command and ensuring that the output displays 'CSORT.c', 'shared_memory.h' and 'Makefile'. 

You will need to first compile the file. Since the compile command is in the provided Makefile, you will just need to type the following 
in your terminal:

    make
    
This will create an executable file 'CSORT' that you can run (as per the command written in the Makefile).

To run the program after compilation enter the following:

    ./CSORT

The first expected output resulting from the execution of the program will be a prompt for you to enter either 0 or 1 depending on if 
you would like to enable the debug mode.

After entering either 0 or 1, the next expected output will be a prompt for you to enter 7 distinct letters (hit enter after each letter). 
You may enter uppercase or lowercase letters.

An example of the required format for input is:

    X
    A
    z
    M
    W
    y
    D

An example output for the inputted letter list above (without debug mode) would be:

    Sorted list in lexicographic order: a,d,m,w,x,y,z

An example output for the inputted letter list above (with debug mode) would be:
```
Process P1: performed swapping
Process P1: No swapping
Process P1: No swapping
Process P1: No swapping
Process P2: performed swapping
Process P2: performed swapping
Process P1: performed swapping
Process P2: performed swapping
Process P3: performed swapping
Process P3: performed swapping
Process P3: performed swapping
Process P1: No swapping
Process P1: No swapping
Process P1: No swapping
Process P2: performed swapping
Process P1: No swapping
Process P1: No swapping
Process P2: performed swapping
Process P3: No swapping
Process P3: No swapping
Process P1: performed swapping
Process P3: No swapping
Process P3: No swapping
Process P3: No swapping
Process P1: No swapping
Process P1: No swapping
Process P2: No swapping
Process P1: No swapping
Process P3: No swapping
Process P1: No swapping
Process P2: No swapping

Sorted list in lexicographic order: a,d,m,w,x,y,z
```
**Note:** *The debug statements will vary! We cannot precisely predict the order of when processes will proceed.*
