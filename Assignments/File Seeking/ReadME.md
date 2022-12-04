# Assignment #5 – Unix/Linux File Seek
Problem 1 You are trying to get a job in a well-known database management company. During a job 
interview, a manager suggests you should be able to write a useless database access program named DB2 
(Dummy Bytes Too). The program should keep a table with 3 entries showing keywords to access 
Information in a file (table shown below). The only accepted input will be the keyword used to search a file 
for some information. The table will provide the byte location of the requested information, which will be 
some string with20 characters. Your program will access and display the 20 characters, starting at the 
position specified in the table. . Your mission is to write the DB2 program (it should be very short, less than 
50 lines for sure) that reads the input, opens the file, skips to the desired location and displays the 
information. You will hand in the printout of your source code and output or submit the source code through 
D2L on the due date. You must use the CYGWIN system or any UNIX/LINUX like system. WINDOWS 
solutions are not accepted since this is a test of your UNIX knowledge.

## Program Table 
Keyword Byte position
- intro 20
- pass 40
- fail 60
- next 80

## File contents
01234567890123456789About grades in thisYou got an A, workedYou get an F, failedLook for nursing ads!!!

## Example:
Input: pass\
Output: You got an A, worked
