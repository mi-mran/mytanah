#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_sleep.h"
//#include "lora.h"
#include "npk.h"

static const char *NPK_TAG = "NPK TASK";
//static const char *LORA_TAG = "LORA TASK";

TaskHandle_t npk_task_handle;
//QueueHandle_t data_queue;

void NPKTask(void *pvParameter) {
#if 0
    data_queue = xQueueCreate(10, sizeof(NPK_DATA));
    if (data_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue");
    }
#endif

    npk_uart_init();

    uint8_t rxData[NPK_RX_BUF_SIZE];

    ESP_LOGI(NPK_TAG, "Entering loop");
    while (1) {
        memset(rxData, 0, sizeof(rxData));
        npk_get_data(rxData, sizeof(rxData));

        NPK_DATA *sensor_data = (NPK_DATA*) pvPortMalloc(sizeof(NPK_DATA));
        if (sensor_data != NULL) {
            ESP_LOGI(NPK_TAG, "Malloc ok");
        }
        ESP_LOGI(NPK_TAG, "%p", sensor_data);
        sensor_data->moist = npk_parse_moist(rxData);
        sensor_data->temp = npk_parse_temp(rxData);
        sensor_data->cond = npk_parse_cond(rxData);
        sensor_data->ph = npk_parse_ph(rxData);
        sensor_data->nitro = npk_parse_nitro(rxData);
        sensor_data->phos = npk_parse_phos(rxData);
        sensor_data->pota = npk_parse_pota(rxData);

        ESP_LOGI(NPK_TAG, "moisture: %d", sensor_data->moist);
        ESP_LOGI(NPK_TAG, "temperature: %d", sensor_data->temp);
        ESP_LOGI(NPK_TAG, "e-conductivity: %d", sensor_data->cond);
        ESP_LOGI(NPK_TAG, "PH: %d", sensor_data->ph);
        ESP_LOGI(NPK_TAG, "Nitrogen: %d", sensor_data->nitro);
        ESP_LOGI(NPK_TAG, "Phosphorus: %d", sensor_data->phos);
        ESP_LOGI(NPK_TAG, "Potassium: %d", sensor_data->pota);
#if 0
        if (xQueueSend(data_queue, sensor_data, portMAX_DELAY) == pdPASS) {
            ESP_LOGI(TAG, "Sent data to queue successfully");
        }
        else {
            ESP_LOGE(TAG, "Failed to send data to queue");
        }
#endif

        vPortFree((void*)sensor_data);
        vTaskDelay(2000 / portTICK_RATE_MS);
    }

    // TBC, may need to unblock LoraTask from ISR when queue is not empty
    vTaskSuspend(NULL);
}

extern "C" void app_main(void) {
    // Task priority TBC
	//xTaskCreate(&LoraTask, "LoraTask", 8192, NULL, 3, &lora_task_handle);
	xTaskCreate(&NPKTask, "NPKTask", 8192, NULL, 5, &npk_task_handle);
}
