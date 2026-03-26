#include "ds3231.h"

static I2C_HandleTypeDef *i2c_rtc;

uint8_t DS3231_Init(I2C_HandleTypeDef *hi2c) {
    i2c_rtc = hi2c;
    return 1;
}

uint8_t DS3231_GetTime(DateTime *dt) {
    uint8_t buf[7];
    if (HAL_I2C_Mem_Read(i2c_rtc, DS3231_ADDR, DS3231_REG_SEC, 1, buf, 7, HAL_MAX_DELAY) != HAL_OK)
        return 0;
    dt->sec = (buf[0] & 0x7F) + 10 * ((buf[0] >> 4) & 0x07);
    dt->min = (buf[1] & 0x7F) + 10 * ((buf[1] >> 4) & 0x07);
    dt->hour = ((buf[2] & 0x1F) + 10 * ((buf[2] >> 4) & 0x03));
    dt->day = buf[4] & 0x07;
    dt->date = (buf[3] & 0x3F) + 10 * ((buf[3] >> 4) & 0x03);
    dt->month = (buf[5] & 0x1F) + 10 * ((buf[5] >> 4) & 0x01);
    dt->year = (buf[6] & 0xFF) + 10 * ((buf[6] >> 4) & 0x0F);
    dt->year += 2000;
    return 1;
}

