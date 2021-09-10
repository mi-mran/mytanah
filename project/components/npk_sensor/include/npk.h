#ifndef NPK_H_
#define NPK_H_

#include <stdint.h>

// UART port and pin numbers
#define NPK_UART_PORT_NUM          (CONFIG_NPK_UART_PORT_NUM)
#define NPK_UART_TXD               (CONFIG_NPK_UART_TXD)
#define NPK_UART_RXD               (CONFIG_NPK_UART_RXD)
#define NPK_UART_RTS               (CONFIG_NPK_UART_RTS)

#define NPK_BUF_SIZE               (127)
#define NPK_BAUD_RATE              (CONFIG_NPK_UART_BAUD_RATE)

// Read packet timeout
#define NPK_PACKET_READ_TICS       (100 / portTICK_RATE_MS)

// Timeout threshold for UART = number of symbols (~10 tics) with unchanged state on receive pin
#define NPK_READ_TOUT             (3) // 3.5T * 8 = 28 ticks, TOUT=3 -> ~24..33 ticks

// NPK address code and function code
#define NPK_ADDR_CODE           0x01
#define NPK_FUNC_CODE           0x03

// NPK register addresses
#define NPK_ADDR_MOISTURE       0x00
#define NPK_ADDR_TEMP           0x01
#define NPK_ADDR_COND           0x02
#define NPK_ADDR_PH             0x03
#define NPK_ADDR_NITRO          0x04
#define NPK_ADDR_PHOS           0x05
#define NPK_ADDR_POTA           0x06
#define NPK_ADDR_SALIN          0x07
#define NPK_ADDR_TDS            0x08

// CRC
#define CRC_LOW_BYTE            0x04
#define CRC_HIGH_BYTE           0x08

// Modbus command to get moisture ... potassium
const uint8_t modbus_cmd[] = {NPK_ADDR_CODE,NPK_FUNC_CODE, 0x00, 0x00, 0x00, 0x07, CRC_LOW_BYTE, CRC_HIGH_BYTE};

// Function declarations
void npk_uart_init(void);
int npk_send(const char*, int);

#endif
