// Tensor VM - log.h

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#define ERROR	-1
#define INFO	 0
#define WARN	 1
#define DEBUG	 2

extern bool log_enabled;

#define Log(level, log_str, ...) if (log_enabled) printf("[ \033[;%dm%s\t\033[0m] <%s>\t| %d\t: \033[;97m" log_str "\033[0m\n", level + 92, #level, __func__, __LINE__, ##__VA_ARGS__)

#endif
