#ifndef SD_CARD_H
#define SD_CARD_H

#include "stm32f1xx_hal.h"
#include "ff.h"   // FATFS

uint8_t SDCard_Init(SPI_HandleTypeDef *hspi);
uint8_t SDCard_WriteLog(const char *data, uint32_t len);

#endif