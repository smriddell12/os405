#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>

#define FN_LEN 256
struct Options {
    bool using_h; // -h, hello world
    bool using_f; // -f, has a file optarg
    char filename[FN_LEN]; // -f optarg
};

static void init_opts(struct Options* opts) {
    opts->using_h = false;
    opts->using_f = false;
    for (int i = 0; i < FN_LEN; i++)
        opts->filename[i] = 0;
}

struct Options opts;

struct Options get_opts(int count, char* args[]) {
    init_opts(&opts);
    int opt;

    while ((opt = getopt(count, args, ":f:h")) != -1) {
        switch (opt) {
            case 'h': 
		opts.using_h = true; 
		break;
            case 'f': 
                opts.using_f = true;
                strcpy(opts.filename, optarg); 
                break;
            case ':': 
                printf("-f needs a value\n");
                break;
            case '?': 
                printf("Unknown option\n");
                exit(-1);
        }
    }

    return opts;
}

int main(int argc, char *argv[]) {
    struct Options o = get_opts(argc, argv);
    if(opts.using_h){
	printf("Hello World\n");
    }

    if (opts.using_f) {
        printf("Contents of %s:\n", opts.filename);
	char c[FN_LEN];
	FILE *fptr;
	fptr = fopen(opts.filename,"r");
	if((fptr) == NULL){
	    printf("Error- file not found.\n");
	    exit(0);
	}
	while(fgets(c,FN_LEN,fptr)){
	    printf("%s",c);
	}
	fclose(fptr);
    }
    
    if((argc - optind) !=1){
	printf("Error- command format is $ my_kill -options pid\n");
	exit(-1);
    }else{
	printf("my_kill pid: %d\n", getpid());
	int pid_to_kill = atoi(argv[optind]);
	int status = kill(pid_to_kill, SIGINT);
	int errnum = errno;
	if (status == -1) {
	fprintf(stderr, "Value of errno: %d\n", errno);
	perror("Error printed by perror");
	fprintf(stderr, "Error killing process: %s\n", strerror( errnum));

        }
    return 0;
    }
}


