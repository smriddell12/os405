#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handler (int sig){
	printf("Floating point exception (signal): %d was caught!\n", sig);
	exit(1);
	return ;
}

int not_so_good(){
	int x = 0;
	return 1 % x;
}

int main(int argc, char *argv[]){
    if (argc > 1) { // catch FPE
        struct sigaction sa;
        printf("Catch a FPE.\n");
        sa.sa_handler = handler;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);

        /* now catch FPE signals */
        sigaction(SIGFPE, &sa, NULL);
    }
    else {
        printf("Default action for FPE.\n");
    }
    not_so_good();
    printf("Will probably not write this.\n");
    return 0;
}

