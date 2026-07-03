#define LOG_TAG "app.init"
#define LOG_LVL ELOG_LVL_INFO

#include "log.h"
#include "app_init.h"

#include "app_debug.h"
#include "app_event.h"
#include "board.h"
#include "board_uart.h"
#include "cli.h"
#include "comm_service.h"
#include "display_service.h"
#include "fatfs_stub.h"
#include "key_service.h"
#include "project_config.h"
#include "sensor_service.h"
#include "shell.h"
#include "storage.h"
#include "storage_service.h"
#include "upgrade_service.h"

static int app_cli_read_byte(char *data)
{
    return board_uart_read_byte(data);
}

static int app_cli_wait_byte(char *data)
{
    return board_uart_wait_byte(data);
}

static void app_cli_write(const char *data, size_t size)
{
    board_uart_write_buffer(data, (int)size);
}

static const cli_port_t app_cli_port = {
    .read_byte = app_cli_read_byte,
    .wait_byte = app_cli_wait_byte,
    .write = app_cli_write
};

void app_init(void)
{
    app_debug_init();
    log_i("Board: %s", PROJECT_BOARD_NAME);
    if(board_hw_version_is_valid()) {
        log_i("HW version: %u", (unsigned int)board_hw_version_get());
    }
    app_event_init();
    storage_init();
    shell_init();
    cli_init(&app_cli_port);
    fatfs_mount("0:");
    key_service_init();
    comm_service_init();
    storage_service_init();
    sensor_service_init();
    display_service_init();
    upgrade_service_init();
}
