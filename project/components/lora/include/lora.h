#ifndef LORA_H_
#define LORA_H_

// Operating Modes

#define MODE_0_NORMAL			0 		// UART and wireless channel open, transparent transmission is on
#define MODE_1_WAKE_UP			1 		// UART and wireless channel open, able to wake mode2 reciver
#define MODE_2_PWR_SAVE			2 		// UART close, wirelesss channel in air-awaken mode
#define MODE_3_SLEEP			3 		// Sleep mode, use this mode for receiving parameter setting commands

// HEAD

#define	CMD_CFG_SAVE			0xC0,		// C0+5 bytes: configure parameters, save after power down
#define	CMD_CFG_LOSE 			0xC2,		// C2+5 bytes: configure parameters, don't save after power down

// SPED - UART parity bit (Bit 7, 6)

#define 8_N1				0
#define 8_O1				1
#define 8_E1				2

// SPED - TTL UART baud rate (Bit 5, 4, 3)
#define UART_1200			0
#define UART_2400			1
#define UART_4800			2
#define UART_9600			3
#define UART_19200			4
#define UART_38400			5
#define UART_57600			6
#define UART_115200			7

// SPED - Air data rate (Bit 2, 1, 0)

#define ADR_300				0
#define ADR_1200			1
#define ADR_2400			2
#define ADR_4800			3
#define ADR_9600			4
#define ADR_19200			5

// OPTN - Fixed transmission enable bit (Bit 7)

#define TT_MODE				0
#define FT_MODE				1

// OPTN - IO drive mode (Bit 6)

#define OC_MODE				0
#define PP_MODE				1

// OPTN - Wireless wake-up time (Bit 5, 4, 3)

#define WAKE_UP_TIME_250		0
#define WAKE_UP_TIME_500		1
#define WAKE_UP_TIME_750		2
#define WAKE_UP_TIME_1000		3
#define WAKE_UP_TIME_1250		4
#define WAKE_UP_TIME_1500		5
#define WAKE_UP_TIME_1750		6
#define WAKE_UP_TIME_2000		7

// OPTN - FEC switch (Bit 2)

#define FEC_OFF				0
#define FEC_ON				1

// OPTN - Transmission power approx (Bit 1, 0)

#define TP20				0
#define TP17				1
#define TP14				2
#define TP10				3

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

// Other Sleep mode commands

#define	SHOW_CFG 			0xC1,		// C1+C1+C1: Show current parameter configuration 
#define	SHOW_VERSION	 		0xC3,		// C3+C3+C3: Show version info
#define	RESET_MODULE			0xC4		// C4+C4+C4: Reset module


#endif
