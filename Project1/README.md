Project Group Number:
    3

Group Members:
    Gianni Guadagno - guada028
    Jackson Reichow - reich656
    Tyler Larson - lars6653

Testing Machine:
    csel-remote-lnx-01.cselabs.umn.edu

Changes to Makefile or Existing Files:
    no changes to Makefile

Contributions Outline:
    All: helped implement void partition_file_data together
    Tyler: Process Tree / Merkle Tree Implementation Plan
    Gianni: Process Tree / Merkle Tree Implementation Plan
    Jackson: Group members, Testing Machine, Changed to Make or Existing Files.

Process Tree / Merkle Tree Implementation Plan:

    

     // TODO: If the current process is a leaf process, read in the associated block file 
    // and compute the hash of the block.



    // TODO: If the current process is not a leaf process, spawn two child processes using  
    // exec() and ./child_process. 

    // TODO: Wait for the two child processes to finish

    // TODO: Retrieve the two hashes from the two child processes from output/hashes/
    // and compute and output the hash of the concatenation of the two hashes.
    














    WRONG:

    while id < n - 1: 
        left pid = fork ()
        if left pid = 0:
            id = 2*id + 1
        right pid = fork()
        if right pid = 0
            id = 2*id + 2

    if id != 0:
        if id <= n:
            - create file name string for current id
            - create a buffer for the hash
            - pass the buffer and the name of file to hash to hash   function
            - 






    
