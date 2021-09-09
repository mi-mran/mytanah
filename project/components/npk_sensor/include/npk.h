#ifndef NPK_H_
#define NPK_H_

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

// Function declarations
void npk_uart_init(void);
int npk_send(const char*, int);

#endif
