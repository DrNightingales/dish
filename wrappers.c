#include "wrappers.h"

ssize_t
getline_w(char **restrict lineptr, 
               size_t *restrict n,
               FILE *restrict stream) {
    BOOL should_free = FALSE;
    ssize_t nread;
    if (lineptr == NULL) {
       should_free = TRUE;
    } 
    if ((nread = getline(lineptr, n, stream)) == -1) {
        if (errno != 0) {
            perror("getline");
        }
        if (should_free) {
            free(lineptr);
        }
        return nread;
    }
    return nread;

}

void
process_token(char* token) {
    char** args = (char**)malloc(ARGS_BASE_COUNT*sizeof(char**));
    if (!args) {
        perror("malloc");
    }
    size_t argc = 0;
    char *subtoken = NULL;
    char *saveptr = NULL;
    
    subtoken = strtok_r(token," ", &saveptr);
    while (subtoken) {
        args[argc++] = subtoken; 
        subtoken = strtok_r(NULL," ", &saveptr);
    }    

   free(args);
}
