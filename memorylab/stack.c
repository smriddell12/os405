#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

char *locals[] = {
    "func2: r      ", 
    "func2: i      ", 
    "func2: fp     ", 
    "func2: retaddr", 
    "func1: q      ", 
    "func1: stop   ", 
    "func1: fp     ", 
    "func1: retaddr", 
    "main:         ", 
    "main:         ", 
    "main:         ", 
    "main:         ", 
    "main:  p      ", 
    "main:  xid    ", 
    "main:  pid    ", 
    "main:         ", 
    "main:         " };

int j = 0;

void func2(unsigned long *stop) {
    unsigned long r = 0x3;
    unsigned long *i;
    for (i = &r; i <= stop+2; i++){
        printf(" %s %p    0x%lx\n", locals[j], i, *i);
        j++;
    }
}

void func1(unsigned long *stop){
    unsigned long q = 0x2;
    func2(stop);
}


int main(){
    long pid = getpid();
    long xid = 0x12345678;
    unsigned long p = 0x111;
    func1(&p);
   back:
    printf("\n main:  p       %p    0x%lx\n", &p, p);
    printf(" func1: retaddr %p \n", &&back);
    printf(" pid: 0x%lx \n", pid);
    char command[50];
    printf("\n\n /proc/%ld/maps \n\n", pid);
    sprintf(command, "cat /proc/%ld/maps", pid);
    system(command);
    return 0;
}
