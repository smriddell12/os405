#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int global = 0;

int main(int argc, char *argv[]) {
    int num_loops = 10;
    if (argc == 1) { 
	fprintf(stderr, "usage: mem <value> [<num_loops>]\n"); 
	exit(1); 
    } 
    if (argc == 3)  // 2nd param is num_loops
        num_loops = atoi(argv[2]);
    int *p, x = 0; 
    p = malloc(sizeof(int));
    assert(p != NULL);
    printf("%s: PID: %d p: %p\n", argv[1], (int) getpid(), p);
    printf("%s: PID: %d &p: %p\n", argv[1], (int) getpid(), &p);
    printf("%s: PID: %d &x: %p\n", argv[1], (int) getpid(), &x);
    printf("%s: PID: %d &global: %p\n", argv[1], (int) getpid(), &global);
    *p = atoi(argv[1]); // assign value to addr stored in p
    //while (1) {
    for (int i = 0; i < num_loops; i++) {
	Spin(1);
	*p = *p + 1;
	++x;
        global++;
	printf("%s: PID: %d: &p: %p p: %p *p: %d\n", argv[1], getpid(), &p, p, *p);
	printf("%s: PID: %d: &x: %p x: %d\n", argv[1], getpid(), &x, x);
	printf("%s: PID: %d: &global: %p global: %d\n", argv[1], getpid(), &global, global);
    }
    return 0;
}

