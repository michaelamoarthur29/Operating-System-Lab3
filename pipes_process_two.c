// Demonstration of bidirectional communication between processes using fork() and pipe()
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int parent_to_child[2];
    int child_to_parent[2];
    pid_t pid;

    // Two constant strings for concatenation
    const char *suffix_child = "@howard.edu";
    const char *suffix_parent = "@gobison.org";
    char message[100];

    // Create two pipes
    if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter a base string: ");
    scanf("%99s", message);

    pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // ------------- CHILD PROCESS -------------
        close(parent_to_child[1]);  // Close unused write end
        close(child_to_parent[0]);  // Close unused read end

        char buffer[200];
        char extra[100];

        // Receive data from parent
        read(parent_to_child[0], buffer, sizeof(buffer));
        close(parent_to_child[0]);

        // Append child's domain
        strncat(buffer, suffix_child, sizeof(buffer) - strlen(buffer) - 1);

        printf("[Child] Modified string: %s\n", buffer);

        // Ask for an additional phrase
        printf("[Child] Enter another word to add: ");
        scanf("%99s", extra);

        // Add that too
        strncat(buffer, extra, sizeof(buffer) - strlen(buffer) - 1);

        // Send back to parent
        write(child_to_parent[1], buffer, strlen(buffer) + 1);
        close(child_to_parent[1]);

        exit(EXIT_SUCCESS);
    } 
    else {
        // ------------- PARENT PROCESS -------------
        close(parent_to_child[0]);  // Close unused read end
        close(child_to_parent[1]);  // Close unused write end

        // Send initial string to child
        write(parent_to_child[1], message, strlen(message) + 1);
        close(parent_to_child[1]);

        // Wait for the child to finish processing
        wait(NULL);

        char final_output[300];
        read(child_to_parent[0], final_output, sizeof(final_output));
        close(child_to_parent[0]);

        // Append parent’s suffix
        strncat(final_output, suffix_parent, sizeof(final_output) - strlen(final_output) - 1);

        printf("[Parent] Final concatenated result: %s\n", final_output);
    }

    return 0;
}
