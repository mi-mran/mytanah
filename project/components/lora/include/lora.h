#ifndef LORA_H_
#define LORA_H_

// Return Status

typedef enum {
	RET_SUCCESS = 0,
	RET_ERROR_UNKNOWN,
	RET_NOT_SUPPORT,
  	RET_NOT_IMPLEMENT,
  	RET_NOT_INITIAL,
  	RET_INVALID_PARAM,
  	RET_DATA_SIZE_NOT_MATCH,
  	RET_BUF_TOO_SMALL,
  	RET_TIMEOUT,
  	RET_HW_ERROR,
} RET_STATUS;

// Operating Modes

#define MODE_0_NORMAL			0 		// UART and wireless channel open, transparent transmission is on
#define MODE_1_WAKE_UP			1 		// UART and wireless channel open, able to wake mode2 reciver
#define MODE_2_PWR_SAVE			2 		// UART close, wirelesss channel in air-awaken mode
#define MODE_3_SLEEP			3 		// Sleep mode, use this mode for receiving parameter setting commands

// HEAD

#define	CMD_CFG_SAVE			0xC0		// C0+5 bytes: configure parameters, save after power down
#define	CMD_CFG_LOSE 			0xC2		// C2+5 bytes: configure parameters, don't save after power down

// SPED - UART parity bit (Bit 7, 6)

#define UART_PB_8N1				0x00
#define UART_PB_8O1				0x01
#define UART_PB_8E1				0x02

// SPED - TTL UART baud rate (Bit 5, 4, 3)
#define UART_1200			0x00
#define UART_2400			0x01
#define UART_4800			0x02
#define UART_9600			0x03
#define UART_19200			0x04
#define UART_38400			0x05
#define UART_57600			0x06
#define UART_115200			0x07

// SPED - Air data rate (Bit 2, 1, 0)

#define ADR_300				0x00
#define ADR_1200			0x01
#define ADR_2400			0x02
#define ADR_4800			0x03
#define ADR_9600			0x04
#define ADR_19200			0x05

// OPTN - Fixed transmission enable bit (Bit 7)

#define TT_MODE				0x00
#define FT_MODE				0x01

// OPTN - IO drive mode (Bit 6)

#define OC_MODE				0x00
#define PP_MODE				0x01

// OPTN - Wireless wake-up time (Bit 5, 4, 3)

#define WAKE_UP_TIME_250		0x00
#define WAKE_UP_TIME_500		0x01
#define WAKE_UP_TIME_750		0x02
#define WAKE_UP_TIME_1000		0x03
#define WAKE_UP_TIME_1250		0x04
#define WAKE_UP_TIME_1500		0x05
#define WAKE_UP_TIME_1750		0x06
#define WAKE_UP_TIME_2000		0x07

// OPTN - FEC switch (Bit 2)

#define FEC_OFF				0x00
#define FEC_ON				0x01

// OPTN - Transmission power approx (Bit 1, 0)

#define TP20				0x00
#define TP17				0x01
#define TP14				0x02
#define TP10				0x03

#if 0
#pragma pack(push, 1)

// SPED_struct_t

typedef struct {
	uint8_t air_bps : 3; //bit 0-2
  	uint8_t uart_bps: 3; //bit 3-5
  	uint8_t uart_fmt: 2; //bit 6-7
} SPED_struct_t;

// OPTN_struct_t

typedef struct {
  uint8_t tsmt_pwr    : 2; //bit 0-1
  uint8_t enFWC       : 1; //bit 2
  uint8_t wakeup_time : 3; //bit 3-5
  uint8_t drive_mode  : 1; //bit 6
  uint8_t trsm_mode   : 1; //bit 7
} OPTN_struct_t;

// Sleep mode command config

typedef struct {
	uint8_t HEAD;
	uint8_t ADDH;			// 0x00 - 0xFF
	uint8_t ADDL;			// 0x00 - 0xFF
	SPED_struct_t SPED;
	uint8_t CHAN;			// 0 - 31, corresponds to 900MHz - 931MHz
	OPTN_struct_t OPTN;
} CMD_Config_t;

#pragma pack(pop)
#endif

// Position of parameters in Params array

enum params {
    POS_HEAD = 0,
    POS_ADDH = 1,
    POS_ADDL = 2,
    POS_SPED = 3,
    POS_CHAN = 4,
    POS_OPTN = 5
};

// Misc

#define M0_PIN          23
#define M1_PIN          22
#define AUX_PIN         21
#define RX_BUF_SIZE     2048
#define TIME_OUT_CNT    100

// Function Prototypes

void InitGPIO(void);
void InitUART(void);
RET_STATUS WaitAUX(void);
void ChangeMode(int);
int SendData(const char*, int);
void CheckParams(void);
void CheckVersion(void);
void ResetModule(void);
void SetHead(uint8_t);
void SetAddrCh(uint16_t, uint8_t);
void SetSpeed(uint8_t, uint8_t, uint8_t);
void SetOption(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
void UpdateParams(void);

// Globals

extern uint8_t Params[];
extern const int uart_num;

#endif
