#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int loop = 1;

// create SIGINT Handler - pgm1.c 
void ctrlcHandler(int signo) {
    fprintf(stdout, "Set loop\n");
    fflush(stdout);
    loop = 0;
}

int main(int argc, char *argv[]) {
    struct sigaction action;

    action.sa_handler = ctrlcHandler;
    action.sa_flags = 0;

    // use action.sa_mask instead of set variable - pgm2.c
    if (sigemptyset(&action.sa_mask) == -1 || // clear set
        sigaddset(&action.sa_mask, SIGINT) == -1 || // add SIGINT to mask
        sigaction(SIGINT, &action, NULL) == -1 ) { // set SIGNINT sigaction
        perror("Failed to set SIGINT");
        return -1;
    }


    if (sigprocmask(SIG_BLOCK, &action.sa_mask, NULL) == -1) {     // block SIGINT (Ctrl - C)
        perror("Failed to block signal mask");
        return -1;
    }

    int counter = 1;
    while (loop) {
        fprintf(stdout, "counter: %d\n", counter);
        fflush(stdout);

        if (counter == 5) {
            if (sigprocmask(SIG_UNBLOCK, &action.sa_mask, NULL) == -1) { // unblock Ctrl-C
                perror("Failed to unblock signal mask");
                return -1;
            }
        }
        counter++;
        sleep(2);
    }

    fprintf(stdout, "Gracefully exiting...\n");
    return 0;
}
