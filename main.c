#include "wrappers.h"
int main() {
    char *input = NULL;
    char *token, *saveptr;
    size_t n_read_bytes = 0;
    while(1) {
        if (getline_w(&input, &n_read_bytes, stdin) == -1) {
            break;
        }
        size_t last = strcspn(input, "\n");
        input[last] = '\0';
#ifdef DEBUG
        puts(input);
        printf("Read: %zu\ncmd: %s\n", last, input);
#endif // DEBUG
        // Divide cmd1 | ... | cmdn into tokens
    
        token = strtok_r(input, "|", &saveptr);

        while(token) {
            process_token(token);

            token = strtok_r(NULL, "|", &saveptr);
        }
    }
    free(input);
}
