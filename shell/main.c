#include "wrappers.h"
int
main()
{
    char *input = NULL;
    char *token, *saveptr;
    pid_t *pids = (pid_t *) malloc(CMDS_BASE_COUNT * sizeof(pid_t));
    size_t ncmds = 0;
    size_t cmds_capacity = CMDS_BASE_COUNT;
    size_t n_read_bytes = 0;
    BOOL exit_cmd = FALSE;
    while (!exit_cmd) {
        printf("$> ");
        if (getline_w(&input, &n_read_bytes, stdin) == -1) {
            break;
        }
        size_t last = strcspn(input, "\n");
        input[last] = '\0';

#ifdef DEBUG
        puts(input);
        printf(CLR_RESET DEBUG_MSG "\n%s\nRead: %zu\ncmd: %s\n" CLR_RESET, input, last, input);
#endif // DEBUG

        // Divide cmd1 | ... | cmdn into tokens

        token = strtok_r(input, "|", &saveptr);

        while (token) {
            char *token_stripped = strip(token); // Calls upon malloc, free at the end
            if (strcmp(token_stripped, "exit") == 0) {
                free(token_stripped);
                exit_cmd = TRUE;
                break;
            }

            // TODO: Add error check
            pid_t current = process_token(token_stripped);
            if (current != -1) {
                if (ncmds + 1 == cmds_capacity) {
                    cmds_capacity *= 2;
                    pids = (pid_t *) realloc(pids, cmds_capacity * sizeof(pid_t));
                    if (!pids) {
                        colored_perror("critical: relloc");
                        exit(EXIT_FAILURE);
                    } else {
                        pids[ncmds++] = current;
                    }
                } else {
                }
            }

            free(token_stripped);
            token = strtok_r(NULL, "|", &saveptr);
        }
        for (size_t k = 0; k < ncmds; ++k) {
            int status;
            if (waitpid(pids[k], &status, WUNTRACED) == -1) {
                colored_perror("waitpid");
            }
            pids[k] = 0;
        }
        ncmds = 0;
    }
    free(pids);
    free(input);
}
