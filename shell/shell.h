#pragma once

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>
#include "colors.h"
typedef enum BOOL
{
    FALSE,
    TRUE
} BOOL;
enum CONSTANTS
{
    READ = 0,
    WRITE = 1,
    MAX_BUFF = 4096,
    ARGS_BASE_COUNT = 128,
    CMDS_BASE_COUNT = 128,
    STD_MODE = 0644
};
