#pragma once
#include "shell.h"

ssize_t getline_w(char **restrict lineptr,
                  size_t *restrict n,
                  FILE *restrict stream);

void
process_token(char* token);
