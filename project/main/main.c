#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lora.h"

void HULU(void *pvParameter) {
	while(1) {
		printf("%d\n", Add(2, 10));
		vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

void app_main(void) {
	xTaskCreate(&HULU, "HULU", 2048, NULL, 1, NULL);
}
