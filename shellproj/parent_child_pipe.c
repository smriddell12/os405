// Code from https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/, tweaked a bit
#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int main() { 
    // First pipe to send input string from parent to child
    // Second pipe to send concatenated string from child to parent
    int fd1[2]; // Used to store two ends of first pipe 
    int fd2[2]; // Used to store two ends of second pipe 
    char fixed_str[] = " CPSC 405";
    char input_str[100]; 
    pid_t p; 

    if (pipe(fd1)==-1) { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    if (pipe(fd2)==-1) { 
        fprintf(stderr, "Pipe Failed" ); 
        return 1; 
    } 
    scanf("%s", input_str); 
    p = fork(); 
    if (p < 0) { 
        fprintf(stderr, "fork Failed" ); 
        return 1; 
    } else if (p > 0) { // Parent process 
        char concat_str[100]; 
        close(fd1[0]); // Close read end of first pipe 
        write(fd1[1], input_str, strlen(input_str)+1); // write input string
        close(fd1[1]); // close write end of first pipe
        wait(NULL);    // Wait for child to send a string 
        close(fd2[1]); // Close write end of second pipe 

        read(fd2[0], concat_str, 100); // Read from child
        printf("Concatenated string: %s \n", concat_str); 
        close(fd2[0]);  // close reading end
    } else { // child process 
        close(fd1[1]); // Close write end of first pipe 
        char concat_str[100]; 
        read(fd1[0], concat_str, 100); // Read string using first pipe 

        int k = strlen(concat_str); // Concatenate a fixed string with it 
        for (int i=0; i<strlen(fixed_str); i++) 
            concat_str[k++] = fixed_str[i]; 
        concat_str[k] = '\0'; // string ends with '\0' 

        close(fd1[0]); // Close both reading ends 
        close(fd2[0]); 
        write(fd2[1], concat_str, strlen(concat_str)+1); // write concatenated string 
        close(fd2[1]); // Close write end of second pipe
        exit(0); 
    } 
} 

