#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "proc.h"
#include "split.h"

#define BUFFER_LEN 100

int main() {
    bootstrap();
    char line[BUFFER_LEN];
    while (fgets(line, BUFFER_LEN, stdin) != NULL) {
        line[strlen(line)-1] = 0; // remove \n from line
        if (line[0] == '#')
            continue;
        struct proc *p = NULL;
        int num;
        char **proc_stuff = split(line, " ", &num);
	if (strcmp(proc_stuff[0],"fork") == 0)
            forq(proc_stuff[1], atoi(proc_stuff[2]), get_pid(proc_stuff[3]));
        else if (strcmp(proc_stuff[0],"kill") == 0)
            kill(get_pid(proc_stuff[1]));
        else if (strcmp(proc_stuff[0],"printpid") == 0) {
            p = find_proc(get_pid(proc_stuff[1]));
            if (p != NULL)
                printf("procname: %s, pid: %d\n", proc_stuff[1], p->pid);
            else
                printf("procname: %s not found\n", proc_stuff[1]);
        }
        else if (strcmp(proc_stuff[0],"printppid") == 0) {
            p = find_proc(get_pid(proc_stuff[1]));
            if (p != NULL)
                printf("procname: %s, ppid: %d\n", proc_stuff[1], p->parent->pid);
            else
                printf("procname: %s not found\n", proc_stuff[1]);
        }
        else if (strcmp(proc_stuff[0],"printprocs") == 0){
            print_procs();
	}
    }
}
            
    


