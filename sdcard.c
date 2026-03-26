#include "ff.h"
#include "sd_card.h"

FATFS fs;
FIL file;

uint8_t SDCard_Init(SPI_HandleTypeDef *hspi) {
  
    if (BSP_SD_Init() != MSD_OK) return 0;
    if (f_mount(&fs, "0:", 1) != FR_OK) return 0;
    return 1;
}

uint8_t SDCard_WriteLog(const char *data, uint32_t len) {
    UINT bw;
    if (f_open(&file, "log.csv", FA_OPEN_ALWAYS | FA_WRITE) != FR_OK) return 0;
    f_lseek(&file, f_size(&file));   
    if (f_write(&file, data, len, &bw) != FR_OK || bw != len) return 0;
    f_close(&file);
    return 1;
}
