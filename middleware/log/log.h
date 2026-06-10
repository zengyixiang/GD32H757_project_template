#ifndef PROJECT_LOG_H
#define PROJECT_LOG_H

#include <stddef.h>

#include "elog.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*log_output_fn_t)(const char *data, size_t size);

void log_set_output(log_output_fn_t output);
int log_init(void);
void log_flush(void);

#ifdef __cplusplus
}
#endif

#endif /* PROJECT_LOG_H */
