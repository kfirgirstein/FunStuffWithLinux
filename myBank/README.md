# Bank - HW2
## 046209 - Operating system structure
A bank that holds accounts and a variable number of ATMs
## Description
In this program, we implemented a bank where accounts are held and a variable
number of ATMs through which these accounts can be accessed.
This solution is as parallel as possible.
The purpose of this exercise is to use threads with the help of the pthreads library
The program will execute various transactions that will be typed by the user

* Every entity in the program (bank, ATMs ...) runs independent 
* All actions are documented to log.txt 
* Maintenance accounts on AccountController, 
    * RWLock implemented inside to Synchronize Accounts List
* Synchronized multithreaded logger with Singleton Design pattern
* RWLock implemented inside Account to Synchronize Accounts balance

## Usage
`./Bank <Number of ATMs – N> <ATM_1_input_file> <ATM_2_input_file>…<ATM_N_input_file>`

Input file example:

>     O 12345 0000 100
>     W 12345 0000 50
>     D 12345 0000 12
>     O 12346 1234 45
>     W 12345 0000 35



## Author

| #       |              Name |             email |
|---------|-------------------|------------------ |
|Student 1|  Kfir Girstein | kfirgirstein@campus.technion.ac.il |
|Student 2|  Gad Sheffer | gadsheffer@campus.technion.ac.il |