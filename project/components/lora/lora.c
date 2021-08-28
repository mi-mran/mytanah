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

// If not configured using the Set__ functions before sending, it will use default parameters.
uint8_t Params[6] = {0xC0, 0x00, 0x00, 0x1A, 0x17, 0x44};


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
    WaitAUX();
	const int txBytes = uart_write_bytes(UART_NUM_2, data, size);
	return txBytes;
}

void CheckParams() {
    int rxBytes;
    uint8_t rxData[RX_BUF_SIZE+1];

    //ChangeMode(MODE_3_SLEEP);
    SendData((const char*) SHOW_CFG, sizeof(SHOW_CFG));
    memset(rxData, 0, RX_BUF_SIZE+1);
    rxBytes = uart_read_bytes(uart_num, rxData, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            ESP_LOGI("CheckParams", "Read %d bytes", rxBytes);
            for (int i=0; i<=rxBytes-1; i++) {
                printf("%x\r\n", rxData[i]);
            }
        }
}

void CheckVersion() {
    int rxBytes;
    uint8_t rxData[RX_BUF_SIZE+1];

    //ChangeMode(MODE_3_SLEEP);
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
    //ChangeMode(MODE_3_SLEEP);
    SendData((const char*) RESET_MODULE, sizeof(RESET_MODULE));
    WaitAUX();
    ESP_LOGI("RESET", "Module Reset");
}

/*
 * Set save params option after power down
 *
 * params
 *     head: 0xC0 for save, 0xC2 for don't save
 */
void SetHead(uint8_t head) {
    Params[POS_HEAD] = head;
}

/*
 * Set Address and Channel for lora module by updating global array Params
 *
 * params
 *     address: 0x0000 to 0xFFFF
 *     channel: 0x00 to 0x1F
 */
void SetAddrCh(uint16_t address, uint8_t channel) {
    uint8_t ADDR_H, ADDR_L;
    ADDR_H = (address >> 8) & 0xFF;
    ADDR_L = address & 0xFF;

    Params[POS_ADDH] = ADDR_H;
    Params[POS_ADDL] = ADDR_L;
    Params[POS_CHAN] = channel;
}

/*
 * Set Speed for lora module by udpating global array Params
 *
 * params
 *     uart_parity: UART parity bit to set UART mode
 *     uart_baudrate: UART Baudrate for UART communications
 *     air_datarate: Air data rate for wireless communications
 */
void SetSpeed(uint8_t uart_parity, uint8_t uart_baudrate, uint8_t air_datarate) {
    uint8_t speed;
    speed = (uart_parity << 6) | (uart_baudrate << 3) | (air_datarate);

    Params[POS_SPED] = speed;
}

/*
 * Set other Options for lora module by updating global array Params
 *
 * params
 *     tx_mode: Transmission mode (Transparent/Fixed)
 *     drive_mode: Configure internal pull-up resistor
 *     wakeup_time: Wireless wakeup time
 *     fec: FEC switch
 *     tx_pow: Transmission power
 */
void SetOption(uint8_t tx_mode, uint8_t drive_mode, uint8_t wakeup_time, uint8_t fec, uint8_t tx_pow) {
    uint8_t option;
    option = (tx_mode << 7) |
        (drive_mode << 6) |
        (wakeup_time << 3) |
        (fec << 2 ) |
        (tx_pow);

    Params[POS_OPTN] = option;
}

/*
 * Update the working parameters of the lora module.
 *
 * Note: To configure these parameters, use the Set__ options before calling this function
 *       If no parameters are configured beforehand, default parameters will be used
 */
void UpdateParams() {
    int rxBytes;
    uint8_t rxData[RX_BUF_SIZE+1];

    SendData((const char*) Params, sizeof(Params));
    rxBytes = uart_read_bytes(uart_num, rxData, RX_BUF_SIZE, 1000 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            ESP_LOGI("UpdateParams", "Read %d bytes", rxBytes);
            for (int i=0; i<=rxBytes-1; i++) {
                printf("%x\r\n", rxData[i]);
            }
        }
}

/*
 * Transmit data wirelessly using Fixed Transmission mode
 *
 * params
 *      target_addr: Target address
 *      target_ch: Target channel
 *      data: Data to send
 */
void TransmitData(uint16_t target_addr, uint8_t target_ch, const char *data) {

}
