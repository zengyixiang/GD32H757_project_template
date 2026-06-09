#ifndef LOG_H
#define LOG_H

#include "debug.h"

#define LOG_RAW(...) DEBUG_RAW(__VA_ARGS__)
#define LOG_E(...) DEBUG_E(__VA_ARGS__)
#define LOG_W(...) DEBUG_W(__VA_ARGS__)
#define LOG_I(...) DEBUG_I(__VA_ARGS__)
#define LOG_D(...) DEBUG_D(__VA_ARGS__)
#define LOG_E_HEX(data, len) DEBUG_E_HEX(data, len)
#define LOG_W_HEX(data, len) DEBUG_W_HEX(data, len)
#define LOG_I_HEX(data, len) DEBUG_I_HEX(data, len)
#define LOG_D_HEX(data, len) DEBUG_D_HEX(data, len)

void log_init(void);
void log_flush(void);

#endif
