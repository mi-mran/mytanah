#ifndef NPK_H_
#define NPK_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
#define CRC_BYTE_LOW            0x04
#define CRC_BYTE_HIGH           0x08

// Others
#define NPK_RX_BUF_SIZE         2048

// Function declarations
void npk_uart_init(void);
void npk_get_data(uint8_t*, uint16_t);
uint16_t npk_parse_moist(uint8_t*);
uint16_t npk_parse_temp(uint8_t*);
uint16_t npk_parse_cond(uint8_t*);
uint16_t npk_parse_ph(uint8_t*);
uint16_t npk_parse_nitro(uint8_t*);
uint16_t npk_parse_phos(uint8_t*);
uint16_t npk_parse_pota(uint8_t*);

// NPK Data Struct
typedef struct {
    uint16_t moist;
    uint16_t temp;
    uint16_t cond;
    uint16_t ph;
    uint16_t nitro;
    uint16_t phos;
    uint16_t pota;
} NPK_DATA;

// Globals
extern const uint8_t modbus_cmd[];

#ifdef __cplusplus
} // extern "C"
#endif 

#endif // NPK_H_
