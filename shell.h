#pragma once

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>

typedef enum BOOL {
    FALSE,
    TRUE
} BOOL;
enum CONSTANTS {
    MAX_BUFF = 4096,
    ARGS_BASE_COUNT = 1000
};

