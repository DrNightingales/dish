#include "wrappers.h"
int
main()
{
    char *input = NULL;
    char *token = NULL;
    char *saveptr = NULL;
    size_t n_cmds = 0;
    size_t n_read_bytes = 0;
    size_t cmds_capacity = CMDS_BASE_COUNT;
    BOOL exit_cmd = FALSE;
    int redirect_input = STDIN_FILENO;
    int redirect_output = STDOUT_FILENO;

    pid_t *pids = (pid_t *) calloc(CMDS_BASE_COUNT, sizeof(pid_t));
    int **pipes = (int **) calloc(CMDS_BASE_COUNT, sizeof(int *));
    char **commands = (char **) calloc(CMDS_BASE_COUNT, sizeof(char *));
    if (!(pids && pipes && commands)) {
        colored_perror("critical: calloc");
        abort();
    }
    for (int k = 0; k < CMDS_BASE_COUNT; ++k) {
        pipes[k] = (int *) calloc(2, sizeof(int));
        if (!pipes[k]) {
            colored_perror("critical: calloc");
            abort();
        }
    }

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

            if (n_cmds == cmds_capacity - 1) {
                pids = (pid_t *) realloc(pids, cmds_capacity * 2 * sizeof(pid_t));
                pipes = (int **) realloc(pipes, cmds_capacity * 2 * sizeof(int *));
                commands = (char **) realloc(commands, cmds_capacity * 2 * sizeof(char *));
                if (!(pids && pipes && commands)) {
                    colored_perror("critical: realloc");
                    abort();
                }
                for (size_t k = cmds_capacity; k < cmds_capacity * 2; ++k) {
                    pipes[k] = (int *) calloc(2, sizeof(int));
                }
                cmds_capacity *= 2;
            }
            commands[n_cmds++] = token_stripped;

            token_stripped = NULL;
            token = strtok_r(NULL, "|", &saveptr);
        }

        for (size_t cmd_k = 0; cmd_k < n_cmds; ++cmd_k) {
            char *token_stripped = commands[cmd_k];

            if (strcmp(token_stripped, "exit") == 0) {
                free(token_stripped);
                commands[cmd_k] = NULL;
                token_stripped = NULL;
                exit_cmd = TRUE;
                break;
            }

            // TODO: Add error check
            if (pipe(pipes[cmd_k]) == -1) {
                colored_perror("critical: pipe");
                abort();
            }
            if (cmd_k == 0) {
                redirect_input = STDIN_FILENO;
            } else {
                redirect_input = pipes[cmd_k - 1][READ];
            }
            if (cmd_k == n_cmds - 1) {
                redirect_output = STDOUT_FILENO;
            } else {
                redirect_output = pipes[cmd_k][WRITE];
            }
            pid_t current = process_token(token_stripped, redirect_input, redirect_output);
            pids[cmd_k] = current;

            if (cmd_k > 0) {
                close(pipes[cmd_k][READ]);
            }
            if (cmd_k < n_cmds - 1) {
                close(pipes[cmd_k][WRITE]);
            }
        }
        for (size_t k = 0; k < n_cmds; ++k) {
            if (pids[k] > 0) {
                int status;

                if (waitpid(pids[k], &status, WUNTRACED) == -1) {
                    colored_perror("waitpid");
                }
            }
            pids[k] = 0;
            free(commands[k]);
            commands[k] = NULL;
        }
        n_cmds = 0;
    }

    for (size_t i = 0; i < cmds_capacity; ++i) {
        free(pipes[i]);
        pipes[i] = NULL;
    }
    free(commands);
    free(pipes);
    free(pids);
    free(input);
}
