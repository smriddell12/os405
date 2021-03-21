#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    char *heap = malloc(20);
    *heap = 0x61;
    printf("heap   value: %p, heap   contents: 0x%x\n", heap, *heap);
    free(heap);
    // heapsy gets the address of heap, which we just freed
    // heapsy's content still has 0x61
    char *heapsy = malloc(20);
    printf("heapsy value: %p, heapsy contents: 0x%x\n", heapsy, *heapsy);
    *heapsy = 0x62;
    // heapsy's content is now 0x62
    printf("heapsy value: %p, heapsy contents: 0x%x\n", heapsy, *heapsy);
    // heap and heapsy point to the same memory
    // We freed heap, but we can still use it
    // When we change *heap, we also change *heapsy
    *heap = 0x63;
    printf("heap   value: %p, heap   contents: 0x%x\n", heap, *heap);
    printf("heapsy value: %p, heapsy contents: 0x%x\n", heapsy, *heapsy);

    // malloc memory for a long[5]
    unsigned long *heapl1 = (unsigned long*) malloc(5*sizeof(unsigned long));
    heapl1[0] = 5;
    heapl1[1] = 10;
    for (int i = -1; i < 5; i++) {
        printf("heapl1[%2d] address: %p, value:  0x%lx \n", i, heapl1+i, heapl1[i]);
    }

    // calloc memory for a long[5] 
    unsigned long *heapl2 = (unsigned long*) calloc(5, sizeof(unsigned long));
    heapl2[0] = 5;
    heapl2[1] = 10;
    for (int i = -1; i < 5; i++) {
        printf("heapl2[%2d] address: %p, value:  0x%lx \n", i, heapl2+i, heapl2[i]);
    }

    // free the calloced memory and print it
    free(heapl2);
    for (int i = -1; i < 5; i++) {
        printf("heapl2[%2d] address: %p, value:  0x%lx \n", i, heapl2+i, heapl2[i]);
    }

    // malloc memmory for a long[5]
    unsigned long *heapl3 = (unsigned long*) malloc(5*sizeof(unsigned long));    
    for (int i = -1; i < 5; i++) {
        printf("heapl3[%2d] address: %p, value:  0x%lx \n", i, heapl3+i, heapl3[i]);
    }

    return 0;
}

