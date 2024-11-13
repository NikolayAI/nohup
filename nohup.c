#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    char *cmd = argv[1];

    if (!cmd) {
        puts("You should pass the command as a first argument");
        exit(EXIT_FAILURE);
    }

    pid_t pid;

    if ((pid = fork()) < 0) {
        perror("Error fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    if (setsid() == -1) {
        perror("Error set session id");
        exit(EXIT_FAILURE);
    };

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    if ((pid = fork()) < 0) {
        perror("Error fork");
        exit(EXIT_FAILURE);
    }

    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);
    chdir("/");

    size_t execv_args_length = sizeof(&argv);
    char *execv_programm = argv[1];
    char *execv_args[execv_args_length - 1];

    for (size_t i = 1; i < execv_args_length; i++) {
        execv_args[i - 1] = argv[i];
    }

    const char *null_file_path = "/dev/null";
    int null_fd;

    if ((null_fd = open(null_file_path, O_RDWR)) == -1) {
        perror("Error open file");
        exit(EXIT_FAILURE);
    }

    dup2(null_fd, STDIN_FILENO);
    dup2(null_fd, STDOUT_FILENO);
    dup2(null_fd, STDERR_FILENO);

    close(null_fd);

    if (execvp(execv_programm, execv_args)) {
        perror("Error exec");
        exit(EXIT_FAILURE);
    }

    return 0;
};
