#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 128
#define REQUIRED_ARGC 4

void parse_command(const char *cmd, char **args, char **cmd_copy_ptr) {
    *cmd_copy_ptr = strdup(cmd);
    if (!(*cmd_copy_ptr)) {
        perror("Error duplicating command string");
        exit(EXIT_FAILURE);
    }

    char *save_ptr;
    int arg_index = 0;

    args[arg_index] = strtok_r(*cmd_copy_ptr, " ", &save_ptr);
    while (args[arg_index] != NULL && arg_index < MAX_ARGS - 1) {
        arg_index++;
        args[arg_index] = strtok_r(NULL, " ", &save_ptr);
    }
    args[arg_index] = NULL;

    if (args[0] == NULL) {
        fprintf(stderr, "Error: Command parsing failed, no arguments found\n");
        free(*cmd_copy_ptr);
        exit(EXIT_FAILURE);
    }
}

void redir(const char *inp, const char *cmd, const char *out) {
    int inp_fd = strcmp(inp, "-") != 0 ? open(inp, O_RDONLY) : STDIN_FILENO;
    if (inp_fd < 0 && strcmp(inp, "-") != 0) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    int out_fd = strcmp(out, "-") != 0 ? open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644) : STDOUT_FILENO;
    if (out_fd < 0 && strcmp(out, "-") != 0) {
        perror("Error opening output file");
        if (inp_fd != STDIN_FILENO) close(inp_fd);
        exit(EXIT_FAILURE);
    }

    char **args = malloc(MAX_ARGS * sizeof(char *));
    if (!args) {
        perror("Error allocating memory for args");
        if (inp_fd != STDIN_FILENO) close(inp_fd);
        if (out_fd != STDOUT_FILENO) close(out_fd);
        exit(EXIT_FAILURE);
    }

    char *cmd_copy;
    parse_command(cmd, args, &cmd_copy);

    pid_t pid = fork();
    if (pid < 0) {
        perror("Error forking process");
        if (inp_fd != STDIN_FILENO) close(inp_fd);
        if (out_fd != STDOUT_FILENO) close(out_fd);
        free(cmd_copy);
        free(args);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        if (inp_fd != STDIN_FILENO) {
            if (dup2(inp_fd, STDIN_FILENO) < 0) {
                perror("Error redirecting stdin");
                exit(EXIT_FAILURE);
            }
            close(inp_fd);
        }

        if (out_fd != STDOUT_FILENO) {
            if (dup2(out_fd, STDOUT_FILENO) < 0) {
                perror("Error redirecting stdout");
                exit(EXIT_FAILURE);
            }
            close(out_fd);
        }

        execvp(args[0], args);
        perror("Error executing command");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("Error during waitpid()");
        } else if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            fprintf(stderr, "Child process did not exit successfully\n");
        }
    }

    if (inp_fd != STDIN_FILENO) close(inp_fd);
    if (out_fd != STDOUT_FILENO) close(out_fd);

    free(cmd_copy);
    free(args);
}

int main(int argc, char *argv[]) {
    if (argc != REQUIRED_ARGC) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return EXIT_FAILURE;
    }

    redir(argv[1], argv[2], argv[3]);
    return EXIT_SUCCESS;
}
