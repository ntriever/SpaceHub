/**
  ******************************************************************************
  * @file           : board.h
  * @brief          : Header for board.c file.
  *                   This file contains defines the infomation on the board.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOARD_H__
#define __BOARD_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define NONE	0
#define ESC		0x1B

enum {
	NOT_OK,
	OK,
};

// Slot Board Type
enum {
	RS232 = 1,
	RS485,
	RS422,
	DI,			// Digital Input
	DO,			// Digital Output
	WIFI,			// Wi-Fi
	OUTDOOR = 0x31		// 옥외형 Temperature
};

// Slot Port Number
enum {
	COM1 = 1,
	COM2,
	COM3,
};

// Device Type
enum {
	TYPE_WATT_METER = 0,			// 전력량계
	TYPE_RECTIFIER,				// 정류기
	TYPE_AIR_CONDITIONER,			// 냉방기
	TYPE_TEMPERATURE_HUMIDITY,	// 온습도모듈
	TYPE_SENSOR,			// 하론모듈
	TYPE_BMU,					// BMU
	TYPE_ETC
};

// 장비명
enum {
	// 전력량계
	WATT_METER = 0x0001,
	SANGJEON_A = 0x0002,
	SANGJEON = 0x0003,
	// 정류기
	DONGA_CRS1800 = 0x0101,
	DONGA_CRS1800N,
	ANTS_CRS1800,
	DONGA_MR2,
	MBRS_1200R,
	MBRS_1200N,
	MBRS_1200,
	SARS_300 = 0x0180,
	EMERSON,
	CRS24_150,
	SDPS_48N,
	SWPS_4150W2,
	LORS_4150,
	LORS_460,
	NORS_4150,
	NORS_460,
	RECTIFIER_MODULE_A_NEO = 0x01F1,
	RECTIFIER_MODULE_A_5TEK,
	RECTIFIER_MODULE,
	// 냉방기
	VARIABLE_AIRCONDITIONER = 0x0201,
	NATURAL_AIRCONDITIONER,
	CHANGJO_AIRCONDITIONER,
	AIRCONDITIONER_MICOM,
	AIRCONDITIONER_MODULE = 0x022F,
	
	TEMPERATURE_HUMIDITY_MODULE_A_NEO = 0x0301,
	TEMPERATURE_HUMIDITY_MODULE_A_5TEK,
	TEMPERATURE_HUMIDITY_MODULE,
	TEMPERATURE_MODULE = 0x0321,
	
	HARON_MODULE = 0x0401,
	FIRE_MODULE = 0x0411,
	FLOOD_MODULE = 0x0421,
	PHOTOELECTRIC_MODULE = 0x0431,
	DOOR_SENSOR = 0x0441,
	LIGHT_CONTROL_MODULE = 0x0451,
	DOOR_STRIKE = 0x0461,
	
	BMU = 0x0501,
	
	GENERATOR = 0x0601,
	FAN_CONTROL = 0x0611,
	RF_READER = 0x0621
};

// Function Code
enum {
	REQUEST_INFO = 0x02,
	REQUEST_DATA = 0x03,
	SET_INFO = 0x06,
	CONTROL_DATA = 0x16,
	DOWNLOAD_READY = 0xF1,
	DOWNLOAD_START = 0xFF,
	DOWNLOAD_STOP = 0xF2
};

typedef struct {
	unsigned char portNumber;
	unsigned char type;
	unsigned short name;
	unsigned char id;
	unsigned short dataLength;
	unsigned short rcvLength;
} PortInfo;

typedef struct {
	unsigned char slotNumber;
	unsigned char type;
	unsigned char id;
	char version[5];
	unsigned char number_of_device[12];
	PortInfo portInfo[12];
} BoardInfo;

/* USER CODE END Private defines */

#endif /* __BOARD_H__ */
