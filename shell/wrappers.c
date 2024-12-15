#include "wrappers.h"

ssize_t
getline_w(char **restrict lineptr, size_t *restrict n, FILE *restrict stream)
{
    BOOL should_free = FALSE;
    ssize_t n_read = 0;

    if (lineptr == NULL) {
        should_free = TRUE;
    }
    if ((n_read = getline(lineptr, n, stream)) == -1) {
        if (errno != 0) {
            colored_perror("getline");
        }
        if (should_free) {
            free(lineptr);
        }
    }
    return n_read;
}

pid_t
process_token(char *token, int redirect_input, int redirect_output)
{
    size_t argc = 0;
    size_t args_capacity = ARGS_BASE_COUNT;
    char *subtoken = NULL;
    char *saveptr = NULL;
    char *file_in = NULL;
    char *file_out = NULL;
    BOOL append = FALSE;
    int pipe_fd[] = {redirect_input, redirect_output};

    char **args = (char **) calloc(ARGS_BASE_COUNT, sizeof(char **));
    if (!args) {
        colored_perror("malloc");
    }

    subtoken = strtok_r(token, " ", &saveptr);
    while (subtoken) {
        if (strcmp(subtoken, ">") == 0) {
            // Output redirection
            subtoken = strtok_r(NULL, " ", &saveptr);
            file_out = subtoken;
            append = FALSE;
        } else if (strcmp(subtoken, ">>") == 0) {
            // Append redirection
            subtoken = strtok_r(NULL, " ", &saveptr);
            file_out = subtoken;
            append = TRUE;
        } else if (strcmp(subtoken, "<") == 0) {
            // Input redirection
            subtoken = strtok_r(NULL, " ", &saveptr);
            file_in = subtoken;
        } else {
            if (argc == args_capacity - 2) {
                args = (char **) realloc(args, 2 * args_capacity * sizeof(char *));
                if (!args) {
                    colored_perror("critical: realloc");
                    abort();
                }
                for (size_t i = argc; i < args_capacity * 2; ++i) {
                    args[i] = NULL;
                }
                args_capacity *= 2;
            }
            args[argc++] = subtoken;
        }
        subtoken = strtok_r(NULL, " ", &saveptr);
    }

    char *bin = find_binary(args[0]);
    if (!bin) {
        free(args);
        return -1;
    }

    pid_t pid = spawn(bin, args, file_in, file_out, append, pipe_fd);
    free(args);
    free(bin);

    return pid;
}

/*
    find_binaries looks for the name of the binary
    This function checks local binaries first, since it the the priority of the task

    This function creates allocates memory;
    It is the callers function duty to free it

    Returns pointer to a buffer in case of success
    NULL if fails
*/
char *
find_binary(const char *bin_name)
{

    size_t len = 0;
    char *res = NULL;
    const char *prefixes[] = {"./bin/", "./", "/usr/bin/", "/bin/"};

    for (size_t i = 0; i < sizeof(prefixes) / sizeof(char *); ++i) {
        size_t len = strlen(prefixes[i]) + strlen(bin_name);
        char *res = (char *) malloc(len);
        if (!res) {
            colored_perror("malloc");
            return NULL;
        }

        snprintf(res, len, "%s%s", prefixes[i], bin_name);

        if (access(res, F_OK) != -1) {
            return res;
        }

        free(res);
    }

    res = strdup(bin_name);
    len = strlen(res) + 1;
    if (!res) {
        colored_perror("malloc");
        return NULL;
    }
    snprintf(res, len, "%s", bin_name);
    return res;
}

int
spawn(const char *file, char *const argv[], char *file_in, char *file_out, BOOL append, int *pipe_fd)
{
    if (file_in && pipe_fd[READ] != STDERR_FILENO) {
        // Error: not possible
    }
    if (file_out && pipe_fd[WRITE]) {
        // Error: not supported yet
    }
    pid_t pid = 0;
#ifdef DEBUG
    printf(DEBUG_MSG "\nSpawning: %s with args:\n" CLR_RESET, file);
    int i = 1;
    while (argv[i]) {
        printf(CLR_BLUE "\t%s\n" CLR_RESET, argv[i]);
        i++;
    }
    printf("\n");
#endif

    switch (pid = fork()) {
    case -1:
        colored_perror("fork");
        return -1;
    case 0:
        // Input redirection to a file
        if (file_in) {
            int fd_in = open(file_in, O_RDONLY);
            if (fd_in == -1) {
                colored_perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // Input redirect io to a pipe
        if (pipe_fd[READ] != STDIN_FILENO) {
            dup2(pipe_fd[READ], STDIN_FILENO);
            close(pipe_fd[READ]);
        }

        // Output redirection to a file
        if (file_out) {
            int fd_out = open(file_out, O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC), STD_MODE);
            if (fd_out == -1) {
                colored_perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        // Output redirection to a pipe
        if (pipe_fd[WRITE] != STDOUT_FILENO) {
            dup2(pipe_fd[WRITE], STDOUT_FILENO);
            close(pipe_fd[WRITE]);
        }

        if (execvp(file, argv)) {
            colored_perror("exec");
            return -1;
        }
        return 0;
    default:
        return pid;
    }
}

void
colored_perror(const char *msg)
{
    fprintf(stderr, CLR_RESET CLR_RED "%s: %s\n" CLR_RESET, msg, strerror(errno));
}

char *
strip(const char *str)
{
    if (str == NULL) {
        return NULL;
    }

    size_t length = strlen(str);
    const char *start = str;
    const char *end = str + (length - 1);

    while (*start && isspace((unsigned char) *start)) {
        start++;
    }

    while (end > start && isspace((unsigned char) *end)) {
        end--;
    }

    length = end - start + 1;

    char *stripped = (char *) malloc(length + 1);
    if (stripped == NULL) {
        colored_perror("critical: malloc");
        abort();
    }

    strncpy(stripped, start, length);
    stripped[length] = '\0';
    return stripped;
}
