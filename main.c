
// mein main.c
//*********************************************//
#include "main.h"
#include "bme280.h"
#include "ds3231.h"
#include "sdcard.h"
#include "uart_cmd.h"
#include "logger.h"
//*********************************************//




TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;

volatile uint8_t logging_trigger = 0;
volatile uint32_t log_interval_sec = 10;   // default 10 sek

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_TIM2_Init();

    // Init modules
    BME280_Init(&hi2c1);
    DS3231_Init(&hi2c1);
    SDCard_Init(&hspi1);
    UART_Command_Init(&huart1);
    Logger_Init();

    // Start timer
    HAL_TIM_Base_Start_IT(&htim2);

    // Main loop
    while (1) {
        UART_Command_Process();
        if (logging_trigger) {
            logging_trigger = 0;
            Logger_LogData();
        }
    }
}

// Timer interrupt callback (every log_interval_sec)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        logging_trigger = 1;
    }
}

// Timer configuration: prescaler and period for 1 second base
void MX_TIM2_Init(void) {
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7200 - 1;   // 72 MHz / 7200 = 10 kHz
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 10000 - 1;     // 10 kHz / 10000 = 1 Hz (1 sec)
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim2);
  
}


void SetLogInterval(uint32_t seconds) {
    log_interval_sec = seconds;
   
    __HAL_TIM_SET_AUTORELOAD(&htim2, seconds * 10000 - 1);
}
