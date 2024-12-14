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
        return n_read;
    }
    return n_read;
}

pid_t
process_token(char *token)
{
    size_t argc = 0;
    char *subtoken = NULL;
    char *saveptr = NULL;

    char **args = (char **) malloc(ARGS_BASE_COUNT * sizeof(char **));
    if (!args) {
        colored_perror("malloc");
    }
    memset(args, '\0', ARGS_BASE_COUNT * sizeof(char **));

    subtoken = strtok_r(token, " ", &saveptr);
    while (subtoken) {
        args[argc++] = subtoken;
        subtoken = strtok_r(NULL, " ", &saveptr);
    }

    char *bin = find_binary(args[0]);
    if (!bin) {
        free(args);
        return -1;
    }

    pid_t pid = spawn(bin, args);
    free(args);
    free(bin);

    return pid;
}

char *
find_binary(const char *bin_name)
{
    /*
    This function creates allocates memory;
    It is the callers function duty to free it

    Returns pointer to a buffer in case of success
    NULL if fails
    */

    // This function checks local binaries first, since it the the priority of the task
    size_t len = 0;
    char *res = NULL;
    const char *prefixes[] = {"/", "./", "./bin/", "/usr/bin/", "/bin/"};

    for (size_t i = 0; i < sizeof(prefixes) / sizeof(char *); ++i) {
        size_t len = sizeof(prefixes[i]) + strlen(bin_name);
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
spawn(const char *file, char *const argv[])
{
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

    const char *start = str;
    const char *end = str + strlen(str) - 1;

    while (*start && isspace((unsigned char) *start)) {
        start++;
    }

    while (end > start && isspace((unsigned char) *end)) {
        end--;
    }

    size_t length = end - start + 1;

    char *stripped = (char *) malloc(length + 1);
    if (stripped == NULL) {
        colored_perror("critical: malloc");
        exit(EXIT_FAILURE);
    }

    strncpy(stripped, start, length);
    stripped[length] = '\0';
    return stripped;
}
