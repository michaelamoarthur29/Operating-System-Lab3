// Program demonstrating IPC with fork() and pipe()
// Parent sends a string to child, and child appends "howard.edu"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipe_parent_to_child[2];
    int pipe_child_to_parent[2];
    pid_t pid;

    char base_domain[] = "howard.edu";
    char user_input[100];

    // Create both pipes
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        perror("Pipe creation failed");
        return 1;
    }

    printf("Enter text to concatenate: ");
    scanf("%99s", user_input);

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return 1;
    }

    if (pid > 0) {  
        // ---- Parent Process ----
        close(pipe_parent_to_child[0]);  // Close read end (unused)
        close(pipe_child_to_parent[1]);  // Close write end (unused)

        // Send user input to child
        write(pipe_parent_to_child[1], user_input, strlen(user_input) + 1);
        close(pipe_parent_to_child[1]);

        // Wait for child to finish
        wait(NULL);

        close(pipe_child_to_parent[0]);
    } else {  
        // ---- Child Process ----
        close(pipe_parent_to_child[1]);  // Close write end (unused)
        close(pipe_child_to_parent[0]);  // Close read end (unused)

        char buffer[200];
        read(pipe_parent_to_child[0], buffer, sizeof(buffer));

        // Append fixed string
        strncat(buffer, base_domain, sizeof(buffer) - strlen(buffer) - 1);

        printf("Result: %s\n", buffer);

        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);

        exit(0);
    }

    return 0;
}


  
