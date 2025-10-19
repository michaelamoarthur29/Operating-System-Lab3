#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <search_word>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int pipe_cat_to_grep[2];
    int pipe_grep_to_sort[2];
    pid_t pid_grep, pid_sort;

    // Create first pipe
    if (pipe(pipe_cat_to_grep) == -1) {
        perror("pipe_cat_to_grep failed");
        exit(EXIT_FAILURE);
    }

    // Fork for grep process
    pid_grep = fork();
    if (pid_grep < 0) {
        perror("fork for grep failed");
        exit(EXIT_FAILURE);
    }

    if (pid_grep == 0) {
        // ---- Inside grep process ----

        // Create second pipe for grep → sort
        if (pipe(pipe_grep_to_sort) == -1) {
            perror("pipe_grep_to_sort failed");
            exit(EXIT_FAILURE);
        }

        pid_sort = fork();
        if (pid_sort < 0) {
            perror("fork for sort failed");
            exit(EXIT_FAILURE);
        }

        if (pid_sort == 0) {
            // ---- Inside sort process ----
            dup2(pipe_grep_to_sort[0], STDIN_FILENO);  // input from grep
            close(pipe_grep_to_sort[1]);
            execlp("sort", "sort", NULL);
            perror("exec sort failed");
            exit(EXIT_FAILURE);
        } else {
            // ---- grep process ----
            dup2(pipe_cat_to_grep[0], STDIN_FILENO);   // input from cat
            dup2(pipe_grep_to_sort[1], STDOUT_FILENO); // output to sort

            close(pipe_cat_to_grep[1]);
            close(pipe_grep_to_sort[0]);

            execlp("grep", "grep", argv[1], NULL);
            perror("exec grep failed");
            exit(EXIT_FAILURE);
        }
    } 
    else {
        // ---- Parent process: cat ----
        close(pipe_cat_to_grep[0]);  // Close unused read end
        dup2(pipe_cat_to_grep[1], STDOUT_FILENO); // Redirect output to pipe

        execlp("cat
