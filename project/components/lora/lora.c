#include <stdio.h>
#include <stdint.h>
#include "lora.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

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

int sendData(const char* logName, const char* data, int size) {
	const int txBytes = uart_write_bytes(UART_NUM_2, data, size);
	ESP_LOGI(logName, "Wrote %d bytes", txBytes);
	return txBytes;
}










