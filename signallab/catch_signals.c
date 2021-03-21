#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int volatile count;

void handler (int sig){
    printf("Signal %d  ouch, that hurt!\n", sig);
    count++;
}

int main(){
    struct sigaction sa;
    int pid = getpid();
    printf("Ok, let's go, kill me (%d) if you can!\n", pid);
    sa.sa_handler = handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) != 0){
    //if (sigaction(SIGUSR1, &sa, NULL) != 0){ // SIGUSR1 == 30
        return(1);
    }

    while (count != 4){
       /* do nothing */
    }
    printf("I've had enough!\n");
    return 0;
}

