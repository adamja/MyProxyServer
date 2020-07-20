
1. Run script description
There has been a run script created that will automatically:
    • build the program files
    • setup folders for clients, servers and proxy with all required folders and files - including: file1, file2, file3, file4, file5 for testing to all servers
    • move the program files into the respective files. eg. server.out file is contained within ‘server1/’ folder
    • open a terminal for proxy with a timeout of 180s (proxy port is 51020)
    • open 3 terminals running servers 1 to 3 with ports of 51021, 51022, 51023 respectively


2. Starting in main directory
You must start in the main program directory:
    • cd to the directory of the program: 'acc/assignment'
eg. $ cd acc/assignment/


3. Run the run script 'run.sh'
$ ./run.sh


4. How to run the CLIENT
    • cd to client dir
eg. $ cd client1
    • run 'client.out' with proxy ip and port passed as args
eg. $ ./client.out 127.0.0.1 51020

* note: there are 3 client files set up by default.


5. How to run the SERVER manually (optional)
    • cd to server dir
eg. $ cd server1
    • run 'server.out' with port of the server passed as arg
eg. $ ./server.out 51021


6. How to run the PROXY (optional)
    • cd to proxy dir
eg. $ cd proxy
    • run 'proxy.out' with client timeout passed as arg
eg. $ ./proxy.out 180


7. How to build the program files (optional)
From the base dir execute the 'make' command and it will build all 3 client, server and proxy and store the output
in the 'obj' directory.
    eg. $ make