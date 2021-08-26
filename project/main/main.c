#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "lora.h"

static const char *TAG = "LORA TASK";

void LoraTask(void *pvParameter) {
    int txBytes;
    int rxBytes;

    InitGPIO();
    InitUART();

	ChangeMode(MODE_0_NORMAL);
	ChangeMode(MODE_3_SLEEP);

	while(1) {
        CheckParams();
        CheckVersion();
        ResetModule();
		vTaskDelay(5000 / portTICK_RATE_MS);
	}
}

void app_main(void) {
	xTaskCreate(&LoraTask, "LoraTask", 8192, NULL, 1, NULL);
}
