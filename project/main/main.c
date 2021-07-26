#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void hulu_wurld(void *pvParameter) {
	while(1) {
		printf("Hulu Wurld\n");
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

void HULU(void *pvParameter) {
	while(1) {
		printf("HULU, once every 10 seconds\n");
		vTaskDelay(10000 / portTICK_RATE_MS);
	}
}

void app_main(void) {
	xTaskCreate(&hulu_wurld, "hulu wurld", 2048, NULL, 5, NULL);
	xTaskCreate(&HULU, "HULU", 2048, NULL, 1, NULL);
}
