#include "uart_cmd.h"
#include "logger.h"
#include <string.h>

static UART_HandleTypeDef *huart;
static char rx_buffer[64];
static uint8_t rx_index = 0;

void UART_Command_Init(UART_HandleTypeDef *huart_ptr) {
    huart = huart_ptr;
    HAL_UART_Receive_IT(huart, (uint8_t*)rx_buffer, 1);   // receive  char ------>>> each
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (rx_buffer[0] == '\r' || rx_buffer[0] == '\n') {
            rx_buffer[rx_index] = '\0';
            // Process command
            if (strcmp((char*)rx_buffer, "status") == 0) {
                char msg[] = "Logging active\r\n";
                HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            } else if (strncmp((char*)rx_buffer, "interval ", 9) == 0) {
                int new_interval = atoi((char*)rx_buffer + 9);
                if (new_interval > 0 && new_interval <= 3600) {
                    SetLogInterval(new_interval);
                    char ack[32];
                    snprintf(ack, sizeof(ack), "Interval set to %d sec\r\n", new_interval);
                    HAL_UART_Transmit(huart, (uint8_t*)ack, strlen(ack), HAL_MAX_DELAY);
                } else {
                    char err[] = "Invalid interval (1-3600)\r\n";
                    HAL_UART_Transmit(huart, (uint8_t*)err, strlen(err), HAL_MAX_DELAY);
                }
            }
            rx_index = 0;
        } else if (rx_index < sizeof(rx_buffer)-1) {
            rx_buffer[rx_index++] = rx_buffer[0];
        }
        HAL_UART_Receive_IT(huart, (uint8_t*)rx_buffer, 1);
    }
}
