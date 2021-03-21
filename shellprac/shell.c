#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define BUFFER_LEN 1024

void hello(){
    printf("Hello World!\n");
}

int main(){
    char line[BUFFER_LEN];
    char *argv[100];
    int argc;
        
    while (1){
        printf("Shell>> ");
	if (!fgets(line, BUFFER_LEN, stdin))
	    break;	
     	size_t length = strlen(line);
	if (line[length-1] == '\n')
	    line[length-1] = '\0';
        if (strcmp(line, "exit")==0)
	    break;
	
	//shell commands
	if (strcmp(line, "hello")==0){
	    hello();
	}
    }	
}
