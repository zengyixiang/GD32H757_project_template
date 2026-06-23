#ifndef CLI_H
#define CLI_H

#include <stddef.h>

typedef int (*cli_read_byte_fn_t)(char *data);
typedef void (*cli_write_fn_t)(const char *data, size_t size);

typedef struct {
    cli_read_byte_fn_t read_byte;
    cli_write_fn_t write;
} cli_port_t;

void cli_init(const cli_port_t *port);
int cli_start(void);
void cli_poll(void);

#endif
