#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "lora.h"

void LoraTask(void *pvParameter) {
	static const char *TAG = "TX";

	// GPIO pins init
	gpio_set_direction(M0_PIN, GPIO_MODE_OUTPUT);
        gpio_set_direction(M1_PIN, GPIO_MODE_OUTPUT);
        gpio_set_direction(AUX_PIN, GPIO_MODE_INPUT);

        // UART pins init
        const int uart_num = UART_NUM_2;
        uart_config_t uart_config = {
                .baud_rate = 115200,
                .data_bits = UART_DATA_8_BITS,
                .parity = UART_PARITY_DISABLE,
                .stop_bits = UART_STOP_BITS_1,
                .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
                .rx_flow_ctrl_thresh = 122,
        };
        uart_param_config(uart_num, &uart_config);
        uart_set_pin(uart_num, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
        uart_driver_install(uart_num, RX_BUF_SIZE, 0, 0, NULL, 0);

        printf(" Init done\n");

	ChangeMode(MODE_3_SLEEP);
	vTaskDelay(200 / portTICK_RATE_MS);

	uint8_t *rxData = (uint8_t*) malloc(RX_BUF_SIZE+1);

	while(1) {
		char TX_data[] = {0xC1, 0xC1, 0xC1};
		sendData(TAG, TX_data, sizeof(TX_data));

		const int rxBytes = uart_read_bytes(uart_num, rxData, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
		if (rxBytes > 0) {
			rxData[rxBytes] = 0;
			printf("Read %d bytes: '%s'\n", rxBytes, rxData);
		}

		vTaskDelay(5000 / portTICK_RATE_MS);
	}
	free(rxData);
}

void app_main(void) {
	xTaskCreate(&LoraTask, "LoraTask", 2048, NULL, 1, NULL);
}
