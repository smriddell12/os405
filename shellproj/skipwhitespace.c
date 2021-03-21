#include <stdio.h>
#include <string.h>

char buf[100]; // buffer for line
char whitespace[] = " \t\r\n\v";
char *words_on_line[10]; // 10 words on a line

int main(int argc, char **argv) {
    int stop = 0;
    while (1) {
        fgets(buf, 100, stdin);
        char *s = buf;
        char *end_buf = buf + strlen(buf);
        int eol = 0, i = 0;
        while (1) {
            while (s < end_buf && strchr(whitespace, *s))
                s++;
            if (*s == 0) // eol - done
                break;
            words_on_line[i++] = s;
            while (s < end_buf && !strchr(whitespace, *s))
                s++;
            *s = 0;
        }
        for (int j = 0; j < i; j++)
            printf("words_on_line[%d]: %s\n", j, words_on_line[j]);
        if (strcmp(words_on_line[0],"stop") == 0)
            break;
    }
}

