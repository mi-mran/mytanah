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
    int slen = 0;
    const char *txData = "Hello Device 2";
    char *string;

    InitGPIO();
    InitUART();

    SetHead(CMD_CFG_SAVE);
    SetOption(0x01, 0x01, 0x00, 0x01, 0x00);
#if RX_DEVICE
    SetAddrCh(RX_DEVICE_ADDR, RX_DEVICE_CH);
#else
    SetAddrCh(TX_DEVICE_ADDR, TX_DEVICE_CH);
#endif
    
    ChangeMode(MODE_3_SLEEP);
    CheckParams();
    UpdateParams();
    ChangeMode(MODE_0_NORMAL);
                
	while(1) {
#if RX_DEVICE
        uart_get_buffered_data_len(uart_num, &rxData_len);        
        if (rxData_len > 0) {
            slen = 0;
            rxBytes = uart_read_bytes(uart_num, rxBuf, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
            string = (char *) malloc(rxBytes);

            for (int i=0; i<rxBytes; i++) {
                string[slen++] = rxBuf[i];
            }

            ESP_LOGW(TAG, "%d bytes received", rxBytes);
            ESP_LOGW(TAG, "%d string length", slen);
            ESP_LOGI(TAG, "%s", string);
            memset(string, '0', strlen(string) * sizeof(char));
            free(string);
        }

#else
        TransmitData(RX_DEVICE_ADDR, RX_DEVICE_CH, txData);
#endif

		vTaskDelay(3000 / portTICK_RATE_MS);
	}
}

void app_main(void) {
	xTaskCreate(&LoraTask, "LoraTask", 8192, NULL, 1, NULL);
}
