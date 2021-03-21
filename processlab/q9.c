#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
int main(int argc, char *argv[]){
    int pipe1[2], pipe2[2];			//space for pipes
    char termmes[50];				//word in from terminal
    strcpy(termmes,argv[1]);			
    char readmes1[50];				//space for read me messages
    
    int retstatus1 = pipe(pipe1);		//make pipe 1
    if(retstatus1 == -1)
	printf("Pipe 1 unsuccessful");

    int retstatus2 = pipe(pipe2);		//make pipe 2
    if(retstatus2 == -1)
        printf("Pipe 2 unsuccessful");

    int f1pid = fork();				//fork happens

    if (f1pid < 0){								//fork failed
        fprintf(stderr, "fork() failed\n");
        exit(1);
    }
    else if(f1pid != 0){							//parent
        close(pipe1[0]);
	close(pipe2[1]);
	printf("Parent connected to child (pid:%d)\n",(int) getpid());
    	write(pipe1[1],termmes,sizeof(termmes));
	read(pipe2[0],readmes1,sizeof(readmes1));
	printf("%s\n",readmes1);
    }
    else if(f1pid == 0){							//child
	close(pipe1[1]);
	close(pipe2[0]);
	printf("Child (pid:%d) reading from pipe\n",(int) getpid());
        read(pipe1[0], readmes1, sizeof(readmes1));
	strcat(readmes1,"fixedstring");
	write(pipe2[1], readmes1, sizeof(readmes1));
    }
return 0;
}

