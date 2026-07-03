#include "cli.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"

#include <stdint.h>
#include <string.h>

#define CLI_INPUT_BUFFER_SIZE 128U
#define CLI_TASK_STACK_WORDS  1024U
#define CLI_TASK_PRIORITY     1U
#define CLI_POLL_PERIOD_MS    10U

static cli_port_t cli_port;
static char cli_input_buffer[CLI_INPUT_BUFFER_SIZE];
static size_t cli_input_length;
static uint8_t cli_started;
static uint8_t cli_last_was_cr;

static void cli_write(const char *data)
{
    if((cli_port.write == 0) || (data == 0)) {
        return;
    }

    cli_port.write(data, strlen(data));
}

static void cli_write_prompt(void)
{
    cli_write("\r\n> ");
}

static void cli_process_command(void)
{
    BaseType_t more_data;
    char *output_buffer;

    output_buffer = FreeRTOS_CLIGetOutputBuffer();

    do {
        memset(output_buffer, 0, configCOMMAND_INT_MAX_OUTPUT_SIZE);
        more_data = FreeRTOS_CLIProcessCommand(cli_input_buffer,
                                               output_buffer,
                                               configCOMMAND_INT_MAX_OUTPUT_SIZE);
        if(output_buffer[0] != '\0') {
            cli_write(output_buffer);
        }
    } while(more_data != pdFALSE);
}

static void cli_handle_byte(char data)
{
    if((data == '\n') && (cli_last_was_cr != 0U)) {
        cli_last_was_cr = 0U;
        return;
    }

    if((data == '\r') || (data == '\n')) {
        cli_last_was_cr = (data == '\r') ? 1U : 0U;
        if(cli_input_length != 0U) {
            cli_write("\r\n");
            cli_input_buffer[cli_input_length] = '\0';
            cli_process_command();
            cli_input_length = 0U;
            cli_input_buffer[0] = '\0';
        }
        cli_write_prompt();
        return;
    }

    cli_last_was_cr = 0U;

    if((data == '\b') || (data == 0x7F)) {
        if(cli_input_length != 0U) {
            cli_input_length--;
            cli_input_buffer[cli_input_length] = '\0';
            cli_write("\b \b");
        }
        return;
    }

    if((data < ' ') || (data > '~')) {
        return;
    }

    if(cli_input_length < (CLI_INPUT_BUFFER_SIZE - 1U)) {
        cli_input_buffer[cli_input_length] = data;
        cli_input_length++;
        cli_input_buffer[cli_input_length] = '\0';
        if(cli_port.write != 0) {
            cli_port.write(&data, 1U);
        }
    }
}

static void cli_task(void *argument)
{
    (void)argument;

    cli_write("\r\nFreeRTOS CLI ready. Type 'help'.");
    cli_write_prompt();

    while(1) {
        if(cli_port.wait_byte != 0) {
            char data;

            if(cli_port.wait_byte(&data) != 0) {
                cli_handle_byte(data);
                cli_poll();
            }
        } else {
            cli_poll();
            vTaskDelay(pdMS_TO_TICKS(CLI_POLL_PERIOD_MS));
        }
    }
}

void cli_init(const cli_port_t *port)
{
    if(port == 0) {
        memset(&cli_port, 0, sizeof(cli_port));
        return;
    }

    cli_port = *port;
    cli_input_length = 0U;
    cli_last_was_cr = 0U;
    cli_input_buffer[0] = '\0';
}

void cli_poll(void)
{
    char data;

    if(cli_port.read_byte == 0) {
        return;
    }

    while(cli_port.read_byte(&data) != 0) {
        cli_handle_byte(data);
    }
}

int cli_start(void)
{
    if(cli_started != 0U) {
        return 0;
    }

    if((cli_port.read_byte == 0) || (cli_port.write == 0)) {
        return -1;
    }

    if(xTaskCreate(cli_task,
                   "cli_task",
                   CLI_TASK_STACK_WORDS,
                   NULL,
                   CLI_TASK_PRIORITY,
                   NULL) != pdPASS) {
        return -1;
    }

    cli_started = 1U;
    return 0;
}
