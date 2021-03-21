#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
int main(int argc, char *argv[]){
    int pipefds[2];
    char writemes[10];
    strcpy(writemes,argv[1]);
    char readmes[10];
    int retstatus = pipe(pipefds);
    if(retstatus == -1){
	printf("Pipe unsuccessful");
    }
    
    int f1pid = fork();
    int f2pid = fork();
    if (f1pid < 0 && f2pid < 0){
        fprintf(stderr, "fork() failed\n");
        exit(1);
    }
    else if(f1pid> 0 && f2pid > 0){
        printf("Parent of two children connected by pipe (pid:%d)\n",(int) getpid());
	printf("Parent of child one (pid:%d) and child two (pid:%d)\n",f1pid,f2pid);
    }
    else if(f1pid == 0 && f2pid > 0){
	printf("child 1: pipe reader: (pid:%d)\n",(int) getpid());
        read(pipefds[0], readmes, sizeof(readmes));
        int cl = close(pipefds[0]);
	if(cl == -1)
	    printf("pipe closing failed\n");
	else
	    printf("childpid: %d finished\n",(int) getpid());
    }
    else if(f1pid > 0 && f2pid == 0){
        printf("child 2: pipe writer:  (pid:%d)\n",(int) getpid());
	strcat(writemes,"suprisestring");
        write(pipefds[1],writemes,sizeof(writemes));
	printf("%s\n",writemes);
	int cl = close(pipefds[1]);
	if(cl == -1)
	    printf("pipe closing failed\n");
	else
	    printf("childpid: %d finished\n",(int) getpid());
    }
return 0;
}
