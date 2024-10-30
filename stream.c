#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

char *get_full_path(char *command) {
    char *path_env = getenv("PATH");
    char *path = strtok(path_env, ":");
    static char full_path[1024];

    while (path != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, command);
        if (access(full_path, X_OK) == 0) {
            return full_path;
        }
        path = strtok(NULL, ":");
    }

    return NULL;
}

void redir(const char *inp, const char *cmd, const char *out) {
    char *args[128];
    char *command = strdup(cmd);
    int i = 0;
    args[i] = strtok(command, " ");
    while (args[i] != NULL && i < 127) {
        args[++i] = strtok(NULL, " ");
    }

    char *full_path = get_full_path(args[0]);
    if (full_path == NULL) {
        fprintf(stderr, "Error: Command not found\n");
        free(command);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        free(command);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        if (strcmp(inp, "-") != 0) {
            int fd_in = open(inp, O_RDONLY);
            if (fd_in < 0) {
                perror("open input file");
                free(command);
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        if (strcmp(out, "-") != 0) {
            int fd_out = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("open output file");
                free(command);
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        execv(full_path, args);
        perror("execv");
        free(command);
        exit(EXIT_FAILURE);
    } else {
        wait(NULL);
    }

    free(command);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <inp> <cmd> <out>\n", argv[0]);
        return EXIT_FAILURE;
    }

    redir(argv[1], argv[2], argv[3]);
    return EXIT_SUCCESS;
}
