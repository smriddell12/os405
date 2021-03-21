#include <stdio.h>
#include <stdlib.h>
#include "common.h"

int main(int argc, char *argv[]) {
    int num_loops = 10;
    if (argc == 1) {
	fprintf(stderr, "usage: cpu <string> [<num_loops>]\n");
	exit(1);
    }
    char *str = argv[1];
    if (argc == 3)  // 2nd param is num_loops
        num_loops = atoi(argv[2]);

    //while (1) {
    for (int i = 0; i < num_loops; i++) {
	printf("%s\n", str);
	Spin(1);
    }
    return 0;
}

