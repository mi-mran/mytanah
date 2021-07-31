#include <stdio.h>
#include <stdint.h>
#include "lora.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define M0_PIN		23
#define M1_PIN		22
#define AUX_PIN		21

#define BUF_SIZE	2048
#define TIME_OUT_CNT	100

void Init() {
	// GPIO pins
	gpio_set_direction(M0_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(M1_PIN, GPIO_MODE_OUTPUT);
	gpio_set_direction(AUX_PIN, GPIO_MODE_INPUT);

	// UART pins
	const int uart_num = UART_NUM_0;
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    
		.rx_flow_ctrl_thresh = 122, 
	};
	uart_param_config(uart_num, &uart_config);
	uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	uart_driver_install(uart_num, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
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
		printf("AUX LOW\n");
	}

	return status;
}

void ChangeMode(int mode) {
	if(WaitAUX() != RET_SUCCESS) {
		return;
	}

	switch(mode) {
		case MODE_0_NORMAL:
			gpio_set_level(M0_PIN, 0);
			gpio_set_level(M1_PIN, 0);
			printf("Changed to Mode 0");
			break;

		case MODE_1_WAKE_UP:
			gpio_set_level(M0_PIN, 1);
			gpio_set_level(M1_PIN, 0);
			printf("Changed to Mode 1");
			break;

		case MODE_2_PWR_SAVE:
			gpio_set_level(M0_PIN, 0);
			gpio_set_level(M1_PIN, 1);
			printf("Changed to Mode 2");
			break;

		case MODE_3_SLEEP:
			gpio_set_level(M0_PIN, 1);
			gpio_set_level(M1_PIN, 1);
			printf("Changed to Mode 3");
			break;

		default:
			printf("Invalid mode\n")
			return;
	}

	WaitAUX();
}	












