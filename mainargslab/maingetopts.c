#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define FN_LEN 256
struct Options {
    bool using_h; // -h, human readable
    bool using_a; // -a, print all
    bool using_d; // -d, list dirs only
    bool using_f; // -f, has a file optarg
    char filename[FN_LEN]; // -f optarg
};

static void init_opts(struct Options* opts) {
    opts->using_h = false;
    opts->using_a = false;
    opts->using_d = false;
    opts->using_f = false;
    for (int i = 0; i < FN_LEN; i++)
        opts->filename[i] = 0;
}

struct Options opts;

struct Options get_opts(int count, char* args[]) {
    init_opts(&opts);
    int opt;

    while ((opt = getopt(count, args, ":f:had")) != -1) {
        switch (opt) {
            case 'h': opts.using_h = true; break;
            case 'a': opts.using_a = true; break;
            case 'd': opts.using_d = true; break;
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
    printf("-h: %s\n", opts.using_h ? "true" : "false");
    printf("-a: %s\n", opts.using_a ? "true" : "false");
    printf("-d: %s\n", opts.using_d ? "true" : "false");
    if (opts.using_f) {
        printf("-f: %s\n", opts.filename);
    }
}

