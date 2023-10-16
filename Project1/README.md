Project Group Number:
    3

Group Members:
    Gianni Guadagno - guada028
    Jackson Reichow - reich656
    Tyler Larson - lars6653

Testing Machine:
    login01.cselabs.umn.edu

Changes to Makefile or Existing Files:
    No changes to Makefile

Contributions:
    All: Helped implement partition_file_data together
    Tyler: Process Tree / Merkle Tree Implementation Plan, double hash part
    Gianni: Process Tree / Merkle Tree Implementation Plan, leaf node case
    Jackson: Rest of readme, not leaf node case

Assumptions:
n/a

AI Help:
-only used for small debugging problems with child_processes.c

Process Tree / Merkle Tree Implementation Plan:

    id = input id from child process call
    left id = 2 * id + 1
    right id = 2 * id + 2

    if n-1 <= id <= 2n-2 is a leaf node:
        create string for block file using id 
        create buffer to store hash
        hash using the block file string. store in buffer
            if error such as file DNE for the leaf node if leaf node id > N:
                exit the process (hash block handles error)
        create a new file name string to store the hash in
        write the hash to the new file
        close new file
        exit the process

    if id < n-1 is not leaf node:
        left child = fork()
        if left child:
            exec() with ./child_process and left id
            ./child_process <blocks_folder> <hashes_folder> <N> <child_id>\n");
        right child = fork()
        if right child:
            exec() with ./child_process and right id
        
    wait(left child)
    wait(right child)
    
    create string of left hash file location using left id
    create string of right hash file location using right id
    left hash file = open(left child hash location string)
    right hash file = open(right child hash location string)

    if error opening either left or right file due not DNE:
        hash itself instead
        exit process

    create buffer for left hash
    create buffer for right hash
    read left hash into left hash buffer
    read right hash into right hash buffer

    close left hash file
    close right hash file


    combo hash = create a buffer for combo hash
    compute_dual_hash(combo hash, left hash buffer, right hash buffer)

    make string for output file name using id 
    create a new file using the string ^
    write combo hash to the file
    close the file

Changes made implementation:
followed the outline created, some small changes, nothing major
