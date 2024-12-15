#pragma once
#include "shell.h"

ssize_t getline_w(char **restrict lineptr, size_t *restrict n, FILE *restrict stream);
int spawn(const char *file, char *const argv[], char *file_in, char *file_out, BOOL append, int *pipe_fd);
pid_t process_token(char *token, int redirect_input, int redirect_output);
char *find_binary(const char *bin_name);
void colored_perror(const char *msg);
char *strip(const char *str);
