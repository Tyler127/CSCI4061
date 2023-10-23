# Terminal 1
    gcc -o p1 p1.c
    ./p1

# Terminal 2
    ps aux | grep ./p1 | grep -v grep
    get pid  of p1

# Terminal 3 
    gcc -o p2 p2.c
    ./p2 [pid of p1] Hello