#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "npk.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define TAG "RS485"

// Modbus command to get moisture ... potassium
uint8_t modbus_cmd[] = {NPK_ADDR_CODE, NPK_FUNC_CODE, 0x00, 0x00, 0x00, 0x07, CRC_BYTE_LOW, CRC_BYTE_HIGH};

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

    gpio_pad_select_gpio(18);
    gpio_set_direction(18, GPIO_MODE_OUTPUT);
}

void npk_get_data(uint8_t *rxBuf, uint16_t size) {
    uint8_t txBytes;
    //uint8_t rxBytes;
    size_t rxData_len;

    uart_set_rts(NPK_UART_PORT_NUM, 1);
    gpio_set_level(18, 1);
    txBytes = uart_write_bytes(NPK_UART_PORT_NUM, (const char*) modbus_cmd, sizeof(modbus_cmd));

    if (txBytes == 8) {
        uart_set_rts(NPK_UART_PORT_NUM, 0);
        gpio_set_level(18, 0);
    }

    // wait until cache is not empty
    do {
        uart_get_buffered_data_len(NPK_UART_PORT_NUM, &rxData_len);
        ESP_LOGW("DEBUG", "len: %d", rxData_len);
        vTaskDelay(300/portTICK_RATE_MS);
    } while (!(rxData_len > 0));
    
    //rxBytes = uart_read_bytes(NPK_UART_PORT_NUM, rxBuf, size, 1000 / portTICK_RATE_MS); 
    uart_read_bytes(NPK_UART_PORT_NUM, rxBuf, size, 1000 / portTICK_RATE_MS); 
}

uint16_t npk_parse_moist(uint8_t *rxBuf) {
    uint8_t moist_high;
    uint8_t moist_low;
    uint16_t moist;

    moist_high = rxBuf[3];
    moist_low = rxBuf[4];

    moist = (moist_high << 8) | (moist_low);
    return moist;
}

uint16_t npk_parse_temp(uint8_t *rxBuf) {
    uint8_t temp_high;
    uint8_t temp_low;
    uint16_t temp;

    temp_high = rxBuf[5];
    temp_low = rxBuf[6];

    temp = (temp_high << 8) | (temp_low);
    temp /= 10;
    return temp;
}

uint16_t npk_parse_cond(uint8_t *rxBuf) {
    uint8_t cond_high;
    uint8_t cond_low;
    uint16_t cond;

    cond_high = rxBuf[7];
    cond_low = rxBuf[8];

    cond = (cond_high << 8) | (cond_low);
    return cond;
}

uint16_t npk_parse_ph(uint8_t *rxBuf) {
    uint8_t ph_high;
    uint8_t ph_low;
    uint16_t ph;

    ph_high = rxBuf[9];
    ph_low = rxBuf[10];

    ph = (ph_high << 8) | (ph_low);
    ph /= 10;
    return ph;
}

uint16_t npk_parse_nitro(uint8_t *rxBuf) {
    uint8_t nitro_high;
    uint8_t nitro_low;
    uint16_t nitro;

    nitro_high = rxBuf[11];
    nitro_low = rxBuf[12];

    nitro = (nitro_high << 8) | (nitro_low);
    return nitro;
}

uint16_t npk_parse_phos(uint8_t *rxBuf) {
    uint8_t phos_high;
    uint8_t phos_low;
    uint16_t phos;

    phos_high = rxBuf[13];
    phos_low = rxBuf[14];

    phos = (phos_high << 8) | (phos_low);
    return phos;
}

uint16_t npk_parse_pota(uint8_t *rxBuf) {
    uint8_t pota_high;
    uint8_t pota_low;
    uint16_t pota;

    pota_high = rxBuf[15];
    pota_low = rxBuf[16];

    pota = (pota_high << 8) | (pota_low);
    return pota;
}

// TODO: CRC check
