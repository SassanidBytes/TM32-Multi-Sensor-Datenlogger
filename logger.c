#include "logger.h"
#include "bme280.h"
#include "ds3231.h"
#include "sdcard.h"

char log_buffer[256];

void Logger_Init(void) {
    // Optional: Write CSV header
    SDCard_WriteLog("Timestamp,Temperature(C),Pressure(hPa),Humidity(%)\r\n", 50);
}

void Logger_LogData(void) {
    float temp, press, hum;
    DateTime dt;

    if (!BME280_ReadData(&temp, &press, &hum)) {
       
        return;
    }
    if (!DS3231_GetTime(&dt)) {
        
    }

    int len = snprintf(log_buffer, sizeof(log_buffer),
                       "%04d-%02d-%02d %02d:%02d:%02d,%.2f,%.2f,%.2f\r\n",
                       dt.year, dt.month, dt.date,
                       dt.hour, dt.min, dt.sec,
                       temp, press/100.0f, hum);
    SDCard_WriteLog(log_buffer, len);
}
