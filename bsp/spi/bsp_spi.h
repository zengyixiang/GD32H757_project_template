#ifndef BSP_SPI_H
#define BSP_SPI_H

void bsp_spi_init(void);
int bsp_spi_transfer(const unsigned char *tx, unsigned char *rx, int size);

#endif
