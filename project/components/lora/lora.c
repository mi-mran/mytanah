#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "lora.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

const int uart_num = UART_NUM_2;

// Other Sleep mode commands
uint8_t SHOW_CFG[] = {0xC1,0xC1,0xC1};	    	// C1+C1+C1: Show current parameter configuration 
uint8_t	SHOW_VERSION[] = {0xC3,0xC3,0xC3};		// C3+C3+C3: Show version info
uint8_t	RESET_MODULE[] = {0xC4,0xC4,0xC4};		// C4+C4+C4: Reset module


void InitGPIO() {
	// GPIO pins init
	gpio_set_direction(M0_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(M1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(AUX_PIN, GPIO_MODE_INPUT);
}

void InitUART() {
    // UART pins init
    uart_config_t uart_config = {
            .baud_rate = 9600,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 122,
    };
    uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, RX_BUF_SIZE, 0, 0, NULL, 0);
}

int ReadAUX() {
	return gpio_get_level(AUX_PIN);
}

RET_STATUS WaitAUX() {
	uint8_t cnt = 0;
	RET_STATUS status = RET_SUCCESS;
	
	while((ReadAUX()==0) && (cnt++<TIME_OUT_CNT)) {
		vTaskDelay(100 / portTICK_RATE_MS);
	}	

	if (cnt >= TIME_OUT_CNT) {
		printf("AUX Timeout\n");
		status = RET_TIMEOUT;
	}
	else {
		printf("AUX HIGH\n");
	}

	return status;
}

void ChangeMode(int mode) {
	if(WaitAUX() != RET_SUCCESS) {
		return;
	}

	vTaskDelay(2 / portTICK_RATE_MS);	// wait 2ms after AUX outputs high

	switch(mode) {
		case MODE_0_NORMAL:
			gpio_set_level(M0_PIN, 0);
			gpio_set_level(M1_PIN, 0);
			printf("Changed to Mode 0\n");
			break;

		case MODE_1_WAKE_UP:
			gpio_set_level(M0_PIN, 1);
			gpio_set_level(M1_PIN, 0);
			printf("Changed to Mode 1\n");
			break;

		case MODE_2_PWR_SAVE:
			gpio_set_level(M0_PIN, 0);
			gpio_set_level(M1_PIN, 1);
			printf("Changed to Mode 2\n");
			break;

		case MODE_3_SLEEP:
			gpio_set_level(M0_PIN, 1);
			gpio_set_level(M1_PIN, 1);
			printf("Changed to Mode 3\n");
			break;

		default:
			printf("Invalid mode\n");
			return;
	}

	WaitAUX();
}	

int SendData(const char* data, int size) {
	const int txBytes = uart_write_bytes(UART_NUM_2, data, size);
	return txBytes;
}

void CheckParams() {
    int rxBytes;
    uint8_t rxData[RX_BUF_SIZE+1];

    SendData((const char*) SHOW_CFG, sizeof(SHOW_CFG));
    memset(rxData, 0, RX_BUF_SIZE+1);
    rxBytes = uart_read_bytes(uart_num, rxData, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            ESP_LOGI("CheckParams", "Read %d bytes", rxBytes);
            for (int i=0; i<=rxBytes; i++) {
                printf("%x\r\n", rxData[i]);
            }
        }
}

void CheckVersion() {
    int rxBytes;
    uint8_t rxData[RX_BUF_SIZE+1];

    SendData((const char*) SHOW_VERSION, sizeof(SHOW_VERSION));
    memset(rxData, 0, RX_BUF_SIZE+1);
    rxBytes = uart_read_bytes(uart_num, rxData, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            ESP_LOGI("CheckVersion", "Read %d bytes", rxBytes);
            for (int i=0; i<=rxBytes; i++) {
                printf("%x\r\n", rxData[i]);
            }
        }
}

void ResetModule() {
    SendData((const char*) RESET_MODULE, sizeof(RESET_MODULE));
    WaitAUX();
    ESP_LOGI("RESET", "Module Reset");
}

