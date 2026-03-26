#include "bme280.h"
#include <math.h>

static I2C_HandleTypeDef *i2c;

// Kalibrierungsdaten
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4;
    int16_t  dig_H5;
    int8_t   dig_H6;
} BME280_CalibData;

static BME280_CalibData calib;


static inline uint16_t read16(uint8_t *buf, uint8_t idx) {
    return (uint16_t)buf[idx] | ((uint16_t)buf[idx+1] << 8);
}

static inline int16_t read16s(uint8_t *buf, uint8_t idx) {
    return (int16_t)read16(buf, idx);
}

// Kalibrierungsdaten einlesen
static uint8_t read_calibration_data(void) {
    uint8_t buf[26];   
    if (HAL_I2C_Mem_Read(i2c, BME280_ADDR, BME280_REG_CALIB00, 1, buf, 26, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    calib.dig_T1 = read16(buf, 0);
    calib.dig_T2 = read16s(buf, 2);
    calib.dig_T3 = read16s(buf, 4);
    calib.dig_P1 = read16(buf, 6);
    calib.dig_P2 = read16s(buf, 8);
    calib.dig_P3 = read16s(buf, 10);
    calib.dig_P4 = read16s(buf, 12);
    calib.dig_P5 = read16s(buf, 14);
    calib.dig_P6 = read16s(buf, 16);
    calib.dig_P7 = read16s(buf, 18);
    calib.dig_P8 = read16s(buf, 20);
    calib.dig_P9 = read16s(buf, 22);

   
    uint8_t buf_h[7];
    if (HAL_I2C_Mem_Read(i2c, BME280_ADDR, BME280_REG_CALIB25, 1, buf_h, 7, HAL_MAX_DELAY) != HAL_OK)
        return 0;

    calib.dig_H1 = buf_h[0];
    calib.dig_H2 = read16s(buf_h, 1);
    calib.dig_H3 = buf_h[3];
    calib.dig_H4 = ((int16_t)buf_h[4] << 4) | (buf_h[5] & 0x0F);
    calib.dig_H5 = ((int16_t)buf_h[6] << 4) | (buf_h[5] >> 4);
    calib.dig_H6 = (int8_t)buf_h[7];   // Index 7 existiert nicht? Achtung!

    uint8_t buf_h_full[8];
    if (HAL_I2C_Mem_Read(i2c, BME280_ADDR, BME280_REG_CALIB25, 1, buf_h_full, 8, HAL_MAX_DELAY) != HAL_OK)
        return 0;
    calib.dig_H1 = buf_h_full[0];
    calib.dig_H2 = read16s(buf_h_full, 1);
    calib.dig_H3 = buf_h_full[3];
    calib.dig_H4 = ((int16_t)buf_h_full[4] << 4) | (buf_h_full[5] & 0x0F);
    calib.dig_H5 = ((int16_t)buf_h_full[6] << 4) | (buf_h_full[5] >> 4);
    calib.dig_H6 = (int8_t)buf_h_full[7];

    return 1;
}

static uint8_t check_chip_id(void) {
    uint8_t id;
    if (HAL_I2C_Mem_Read(i2c, BME280_ADDR, BME280_REG_ID, 1, &id, 1, HAL_MAX_DELAY) != HAL_OK)
        return 0;
    return (id == 0x60); 
}

// Warten bis Messung abgeschlossen 
static void wait_for_measurement(void) {
    uint8_t status;
    do {
        HAL_I2C_Mem_Read(i2c, BME280_ADDR, BME280_REG_STATUS, 1, &status, 1, HAL_MAX_DELAY);
    } while (status & BME280_STATUS_MEASURING);
}

uint8_t BME280_Init(I2C_HandleTypeDef *hi2c) {
    i2c = hi2c;

   
    if (!check_chip_id()) return 0;

    // Kalibrierungsdaten auslesen
    if (!read_calibration_data()) return 0;

    
    uint8_t reset_cmd = 0xB6;
    HAL_I2C_Mem_Write(i2c, BME280_ADDR, BME280_REG_RESET, 1, &reset_cmd, 1, HAL_MAX_DELAY);
    HAL_Delay(10);

 
    uint8_t config = BME280_OSRS_T_1 | BME280_OSRS_P_1 | BME280_MODE_NORMAL;
    HAL_I2C_Mem_Write(i2c, BME280_ADDR, BME280_REG_CTRL_MEAS, 1, &config, 1, HAL_MAX_DELAY);
    config = BME280_OSRS_H_1;
    HAL_I2C_Mem_Write(i2c, BME280_ADDR, BME280_REG_CTRL_HUM, 1, &config, 1, HAL_MAX_DELAY);

   
    config = 0x00;
    HAL_I2C_Mem_Write(i2c, BME280_ADDR, BME280_REG_CONFIG, 1, &config, 1, HAL_MAX_DELAY);

    return 1;
}

// Temperatur
uint8_t BME280_ReadData(float *temp, float *press, float *hum) {
    uint8_t data[8];  

    // Alle Sensor-Rohdaten auf einmal lesen
    if (HAL_I2C_Mem_Read(i2c, BME280_ADDR, BME280_REG_PRESS_MSB, 1, data, 8, HAL_MAX_DELAY) != HAL_OK)
        return 0;


    int32_t adc_P = ((int32_t)data[0] << 12) | ((int32_t)data[1] << 4) | ((int32_t)data[2] >> 4);
    int32_t adc_T = ((int32_t)data[3] << 12) | ((int32_t)data[4] << 4) | ((int32_t)data[5] >> 4);
    int32_t adc_H = ((int32_t)data[6] << 8) | (int32_t)data[7];

    // ----- Temperatur  -----
    int32_t var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) * ((int32_t)calib.dig_T2)) >> 11;
    int32_t var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) * ((int32_t)calib.dig_T3)) >> 14;
    int32_t t_fine = var1 + var2;

    // Temperatur in °C
    *temp = (t_fine * 5 + 128) >> 8;
    *temp /= 100.0f;

    // ----- Druck -----
    var1 = ((int32_t)t_fine >> 1) - (int32_t)64000;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)calib.dig_P6;
    var2 = var2 + ((var1 * (int32_t)calib.dig_P5) << 1);
    var2 = (var2 >> 2) + ((int32_t)calib.dig_P4 << 16);
    var1 = (((calib.dig_P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + (((int32_t)calib.dig_P2 * var1) >> 1)) >> 18;
    var1 = ((((32768 + var1)) * (int32_t)calib.dig_P1) >> 15);

    if (var1 == 0) return 0;   

    int32_t p = ((int32_t)1048576 - adc_P) - (var2 >> 12);
    p = (p << 13) / var1;
    var1 = ((int32_t)calib.dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int32_t)calib.dig_P8 * p) >> 19;
    p = ((p + var1 + var2) >> 8) + ((int32_t)calib.dig_P7 << 4);

    
    *press = p / 256.0f;  
   
    *press = p / 100.0f;

    // ----- Feuchtigkeit -----
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)calib.dig_H4) << 20) - (((int32_t)calib.dig_H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)calib.dig_H6)) >> 10) *
                    (((v_x1_u32r * ((int32_t)calib.dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                    ((int32_t)2097152)) * ((int32_t)calib.dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)calib.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    // Feuchtigkeit in %RH (0..100)
    *hum = (v_x1_u32r >> 12) / 1024.0f;

    return 1;
}
