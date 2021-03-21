#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * $ gcc -o redirect_stdinout redirect_stdinput.c
 * $ ./redirect_stdinout infile outfile
 */

int main(int argc, char **argv) {
    int pid, status;
    int infd, outfd;

    if (argc != 3) {
        fprintf(stderr, "usage: %s input_file output_file\n", argv[0]);
        exit(1);
    }
    if ((infd = open(argv[1], O_RDONLY, 0644)) < 0) {
        perror(argv[1]);
        exit(1);
    }
    if ((outfd = open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
        perror(argv[2]);
        exit(1);
    }
    printf("After this message, stdout/stdin redirected to/from \"%s\" \"%s\".\n", argv[0], argv[1]);

    /*
     * stdin is fd 0, stdout is fd 1 
     * dup2 connects our file to fd 1 (stdout)
     */
    dup2(infd,  0);
    dup2(outfd, 1);

    char line[100];
    fgets(line, 100, stdin);
    printf("outfile: %s: data from infile: %s\n", argv[1], line);
    exit(0);
}

