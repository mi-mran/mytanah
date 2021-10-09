#include <stdio.h>
#include <string.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "lmic.h"
#include "npk.h"

extern "C" {void app_main(void);}

// Pin mapping
// TODO: Change the pin mapping
lmic_pinmap lmic_pins = {
    .nss = 6,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 5,
    .dio = {2, 3, 4},
};

// TODO: Change APPEUI, DEVEUI, APPKEY to our own keys

/*
 * This EUI must be in little-endian format, so least-significant-byte
 * first. When copying an EUI from ttnctl output, this means to reverse
 * the bytes. For TTN issued EUIs the last bytes should be 0xD5, 0xB3,
 * 0x70.
 */

static const u1_t APPEUI[8]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getArtEui (u1_t* buf) { memcpy(buf, APPEUI, 8);}

// This should also be in little endian format, see above.
static const u1_t DEVEUI[8]={ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
void os_getDevEui (u1_t* buf) { memcpy(buf, DEVEUI, 8);}

/*
 * This key should be in big endian format (or, since it is not really a
 * number but a block of memory, endianness does not really apply). In
 * practice, a key taken from ttnctl can be copied as-is.
 * The key shown here is the semtech default key.
 */
static const u1_t APPKEY[16] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };
void os_getDevKey (u1_t* buf) {  memcpy(buf, APPKEY, 16);}

static const char *TAG = "[SendData]";
static const char *EV_TAG = "[Event]";

static osjob_t SendDataJob;

void SendData(osjob_t* SendDataJob) {
    npk_uart_init();

    uint8_t rxData[NPK_RX_BUF_SIZE];
    memset(rxData, 0, sizeof(rxData));
    npk_get_data(rxData, sizeof(rxData));

    // Data to send
    NPK_DATA *sensor_data = (NPK_DATA*) malloc(sizeof(NPK_DATA));
    sensor_data->moist = npk_parse_moist(rxData);
    sensor_data->temp = npk_parse_temp(rxData);
    sensor_data->cond = npk_parse_cond(rxData);
    sensor_data->ph = npk_parse_ph(rxData);
    sensor_data->nitro = npk_parse_nitro(rxData);
    sensor_data->phos = npk_parse_phos(rxData);
    sensor_data->pota = npk_parse_pota(rxData);

    // Check if there is a current TX/RX job running
    if (LMIC.opmode & OP_TXRXPEND)
        ESP_LOGI(TAG, "There is a TX/RX job running, cancelling transmission");
    else {
        // Prepare upstream data tx at the next possible time
        // TODO: Check the type-casting
        // TODO: Check which port to send
        LMIC_setTxData2(1, (uint8_t*) sensor_data, sizeof(sensor_data), 0);
        ESP_LOGI(TAG, "Packet queued");
    }

    free(sensor_data);
}

void onEvent (ev_t ev) {
    ESP_LOGI(EV_TAG, "systick: %d", os_getTime());
    switch(ev) {
        case EV_SCAN_TIMEOUT:
            ESP_LOGI(EV_TAG, "EV_SCAN_TIMEOUT");
            break;
        case EV_BEACON_FOUND:
            ESP_LOGI(EV_TAG, "EV_BEACON_FOUND");
            break;
        case EV_BEACON_MISSED:
            ESP_LOGI(EV_TAG, "EV_BEACON_MISSED");
            break;
        case EV_BEACON_TRACKED:
            ESP_LOGI(EV_TAG, "EV_BEACON_TRACKED");
            break;
        case EV_JOINING:
            ESP_LOGI(EV_TAG, "EV_JOINING");
            break;
        case EV_JOINED:
            ESP_LOGI(EV_TAG, "EV_JOINED");
            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            ESP_LOGI(EV_TAG, "EV_RFU1");
            break;
        case EV_JOIN_FAILED:
            ESP_LOGI(EV_TAG, "EV_JOIN_FAILED");
            break;
        case EV_REJOIN_FAILED:
            ESP_LOGI(EV_TAG, "EV_REJOIN_FAILED");
            break;
        case EV_TXCOMPLETE:
            ESP_LOGI(EV_TAG, "EV_TXCOMPLETE (includes waiting for RX windows)");
            if (LMIC.txrxFlags & TXRX_ACK)
              ESP_LOGI(EV_TAG, "Received ack");
            if (LMIC.dataLen) {
              ESP_LOGI(EV_TAG, "Received ");
              ESP_LOGI(EV_TAG, "%d", LMIC.dataLen);
              ESP_LOGI(EV_TAG, " bytes of payload");
            }
            // Schedule next transmission every 60 seconds
            // TODO: Set correct transmission interval
            os_setTimedCallback(&SendDataJob, os_getTime()+sec2osticks(60), SendData);
            break;
        case EV_LOST_TSYNC:
            ESP_LOGI(EV_TAG, "EV_LOST_TSYNC");
            break;
        case EV_RESET:
            ESP_LOGI(EV_TAG, "EV_RESET");
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            ESP_LOGI(EV_TAG, "EV_RXCOMPLETE");
            break;
        case EV_LINK_DEAD:
            ESP_LOGI(EV_TAG, "EV_LINK_DEAD");
            break;
        case EV_LINK_ALIVE:
            ESP_LOGI(EV_TAG, "EV_LINK_ALIVE");
            break;
         default:
            ESP_LOGI(EV_TAG, "Unknown event");
            break;
    }
}

void app_main(void) {
    npk_uart_init();

    // LMIC Init
    os_init();

    // Reset the MAC state. Session and pending data transfers will be discarded
    LMIC_reset();

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // Start SendData (Sending automatically starts OTAA too)
    SendData(&SendDataJob);

    while(1) {
        os_runloop_once();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
