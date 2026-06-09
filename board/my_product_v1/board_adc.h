#ifndef BOARD_ADC_H
#define BOARD_ADC_H

#include <stdint.h>

typedef enum {
    BOARD_ADC_VBAT = 0,
    BOARD_ADC_TEMP,
    BOARD_ADC_COUNT
} board_adc_id_t;

void board_adc_init(void);
uint16_t board_adc_read(board_adc_id_t id);

#endif
