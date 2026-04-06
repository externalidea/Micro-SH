#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 64

/**
 * Reads a line of input from stdin and removes the newline character.
 * Returns 1 if input is valid, 0 otherwise.
 */
int read_input(char *buffer) {
    if (fgets(buffer, MAX_INPUT, stdin) == NULL) return 0;
    
    // Remove trailing newline character added by fgets
    buffer[strcspn(buffer, "\n")] = 0;
    
    return (strlen(buffer) > 0);
}

int main() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];

    while (1) {
        printf(">>> ");
        
        // Skip iteration if input is empty or failed
        if (!read_input(input)) continue;

        // --- PARSING SECTION ---
        // Tokenize input string into an array of arguments (split by space)
        int i = 0;
        args[i] = strtok(input, " ");
        while (args[i] != NULL && i < MAX_ARGS - 1) {
            args[++i] = strtok(NULL, " ");
        }

        // Handle empty command
        if (args[0] == NULL) continue;

        // --- COMMAND EXECUTION SECTION ---
        
        // 1. Built-in: "hello"
        if (strcmp(args[0], "hello") == 0) {
            printf("Hello from shell!\n");
        } 
        
        // 2. Built-in: "cd" (Must be executed by parent process using chdir)
        else if (strcmp(args[0], "cd") == 0) {
            if (args[1] == NULL) {
                fprintf(stderr, "cd: missing argument\n");
            } else if (chdir(args[1]) != 0) {
                perror("cd"); // Prints system error message
            }
        }
        
        // 3. External commands: "ls", "pwd", etc.
        else {
            pid_t pid = fork(); // Create a child process

            if (pid == 0) {
                // CHILD PROCESS
                // Replace current process image with the new program
                if (execvp(args[0], args) == -1) {
                    perror("Execution failed"); 
                }
                exit(EXIT_FAILURE); // Kill child if exec fails
            } 
            else if (pid < 0) {
                // Fork failed
                perror("Fork failed");
            } 
            else {
                // PARENT PROCESS
                // Wait for the child process to complete before showing prompt again
                wait(NULL); 
            }
        }
    }
    return 0;
}