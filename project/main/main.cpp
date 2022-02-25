#include <stdio.h>
#include <cstring>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs_flash.h"
#include "npk.h"
#include "TheThingsNetwork.h"

static const char *NPK_TAG = "NPK TASK";

// AppEUI (sometimes called JoinEUI)
const char *appEui = "0000000000000000";
// DevEUI
const char *devEui = "d5d8021885ff8b33";
// AppKey
const char *appKey = "74030c3db58941f9d79423195588c4e1";

// Pins and other resources
#define TTN_SPI_HOST      HSPI_HOST
#define TTN_SPI_DMA_CHAN  1
#define TTN_PIN_SPI_SCLK  4
#define TTN_PIN_SPI_MOSI  27
#define TTN_PIN_SPI_MISO  19
#define TTN_PIN_NSS       21
#define TTN_PIN_RXTX      TTN_NOT_CONNECTED
#define TTN_PIN_RST       14
#define TTN_PIN_DIO0      26
#define TTN_PIN_DIO1      35

static TheThingsNetwork ttn;
const unsigned TX_INTERVAL = 60;

TaskHandle_t npk_task_handle;

void sendMessage(uint8_t* data)
{
    printf("Sending message...\n");
    TTNResponseCode res = ttn.transmitMessage((uint8_t*) data, strlen((const char*) data));
    printf(res == kTTNSuccessfulTransmission ? "Message sent.\n" : "Transmission failed.\n");
}

void messageReceived(const uint8_t* message, size_t length, ttn_port_t port)
{
    printf("Message of %d bytes received on port %d:", length, port);
    for (int i = 0; i < length; i++)
        printf(" %02x", message[i]);
    printf("\n");
}

void NPKTask(void *pvParameter) {
    npk_uart_init();

    uint8_t rxData[NPK_RX_BUF_SIZE];

    ESP_LOGI(NPK_TAG, "Entering loop");
    while (1) {
        memset(rxData, 0, sizeof(rxData));
        npk_get_data(rxData, sizeof(rxData));

        uint8_t msgData[200] = {0};

        NPK_DATA *sensor_data = (NPK_DATA*) pvPortMalloc(sizeof(NPK_DATA));
        // TODO: Error checking improvement
        if (sensor_data != NULL)
            ESP_LOGI(NPK_TAG, "Malloc ok");

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

        sprintf((char*)msgData, 
                "{\"Moisure\":%d,\"Temperature\":%d,\"Conductivity\":%d,\"PH\":%d,\"Nitrogen\":%d,\"Phosphorus\":%d,\"Potassium\":%d}",
                sensor_data->moist,
                sensor_data->temp,
                sensor_data->cond,
                sensor_data->ph,
                sensor_data->nitro,
                sensor_data->phos,
                sensor_data->pota
               );

        ESP_LOGI(NPK_TAG, "%s", msgData);

        sendMessage(msgData);
        vPortFree((void*)sensor_data);
        vTaskDelay(TX_INTERVAL * pdMS_TO_TICKS(1000));
    }

    // Should not reach here
    vTaskSuspend(NULL);
}

extern "C" void app_main(void) {
    esp_err_t err;
    // Initialize the GPIO ISR handler service
    err = gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    ESP_ERROR_CHECK(err);

    // Initialize the NVS (non-volatile storage) for saving and restoring the keys
    err = nvs_flash_init();
    ESP_ERROR_CHECK(err);

    // Initialize SPI bus
    spi_bus_config_t spi_bus_config;
    spi_bus_config.miso_io_num = TTN_PIN_SPI_MISO;
    spi_bus_config.mosi_io_num = TTN_PIN_SPI_MOSI;
    spi_bus_config.sclk_io_num = TTN_PIN_SPI_SCLK;
    spi_bus_config.quadwp_io_num = -1;
    spi_bus_config.quadhd_io_num = -1;
    spi_bus_config.max_transfer_sz = 0;
    err = spi_bus_initialize(TTN_SPI_HOST, &spi_bus_config, TTN_SPI_DMA_CHAN);
    ESP_ERROR_CHECK(err);

    // Configure the SX127x pins
    ttn.configurePins(TTN_SPI_HOST, TTN_PIN_NSS, TTN_PIN_RXTX, TTN_PIN_RST, TTN_PIN_DIO0, TTN_PIN_DIO1);

    // The below line can be commented after the first run as the data is saved in NVS
    ttn.provision(devEui, appEui, appKey);

    // Register callback for received messages
    ttn.onMessage(messageReceived);

    printf("Joining...\n");
    if (ttn.join())
    {
        printf("Joined.\n");
	    xTaskCreate(&NPKTask, "NPKTask", 8192, NULL, 5, &npk_task_handle);
    }
    else
    {
        printf("Join failed. Goodbye\n");
    }
}
