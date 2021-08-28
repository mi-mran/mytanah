#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "lora.h"

static const char *TAG = "LORA TASK";

void LoraTask(void *pvParameter) {
    uint8_t rxBuf[RX_BUF_SIZE+1];
    size_t rxData_len;
    int rxBytes;
    uint8_t *txData = "Hello Device 2";

    InitGPIO();
    InitUART();

    SetHead(CMD_CFG_SAVE);
    SetAddrCh(0xAAAA, 0x0F);
    
    ChangeMode(MODE_3_SLEEP);
    CheckParams();
    UpdateParams();
    ChangeMode(MODE_0_NORMAL);
                
	while(1) {
        uart_get_buffered_data_len(uart_num, &rxData_len);        
        if (rxData_len > 0) {
            rxBytes = uart_read_bytes(uart_num, rxBuf, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
            for (int i=0; i<rxBytes-1; i++) {
                ESP_LOGI(TAG, "%d", rxBuf[i]);
            }
        }

		vTaskDelay(3000 / portTICK_RATE_MS);
	}
}

void app_main(void) {
	xTaskCreate(&LoraTask, "LoraTask", 8192, NULL, 1, NULL);
}
