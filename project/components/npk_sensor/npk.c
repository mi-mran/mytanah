#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "npk.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define TAG "RS485"

void npk_uart_init() {
    uart_config_t uart_config = {
        .baud_rate = NPK_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_APB,
    };

    // Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "Start initialization and configuration");

    // Install UART driver (we don't need an event queue here)
    // In this example we don't even use a buffer for sending data.
    ESP_ERROR_CHECK(uart_driver_install(NPK_UART_PORT_NUM, NPK_BUF_SIZE * 2, 0, 0, NULL, 0));

    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(NPK_UART_PORT_NUM, &uart_config));

    ESP_LOGI(TAG, "UART set pins, mode and install driver.");

    // Set UART pins as per KConfig settings
    ESP_ERROR_CHECK(uart_set_pin(NPK_UART_PORT_NUM, NPK_UART_TXD, NPK_UART_RXD, NPK_UART_RTS, UART_PIN_NO_CHANGE));

    // Set RS485 half duplex mode
    ESP_ERROR_CHECK(uart_set_mode(NPK_UART_PORT_NUM, UART_MODE_RS485_HALF_DUPLEX));

    // Set read timeout of UART TOUT feature
    ESP_ERROR_CHECK(uart_set_rx_timeout(NPK_UART_PORT_NUM, NPK_READ_TOUT));
}

int npk_send(const char* data, int size) {
    const int txBytes = uart_write_bytes(NPK_UART_PORT_NUM, data, size);
    return txBytes;
}

