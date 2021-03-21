#include <stdio.h>
#include <stdlib.h>

int global = 5;
const int constant = 5;
int main(int argc, char *argv[]) {
    startofmain:
    printf("location of code      : %p\n", main);
    printf("size of code          : %lx\n",  &&endofmain-&&startofmain);
    printf("end of code           : %p\n", &&endofmain);
    printf("location of globals   : %p\n", &global);
    printf("location of constants : %p\n", &constant);
    int stack = 3;
    printf("location of stack     : %p\n", &stack);
    int *heap = malloc(100e6);
    printf("location of heap      : %p\n", heap);
    //int *p = &global - 400;
    //*p = 5;
    int *p = &constant;
    *p = 5;
    endofmain:
    return 0;
}
