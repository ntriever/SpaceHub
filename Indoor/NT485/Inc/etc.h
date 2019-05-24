/**
  ******************************************************************************
  * @file           : etc.h
  * @brief          : Header for etc.c file.
  *                   This file contains defines the infomation on the board.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ETC_H__
#define __ETC_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
/* USER CODE END Private defines */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void ReadBoardID(void);
void InitializeBoardInfo(void);
void InitializeData(void);
void SetBoardData(unsigned char *infoData, short length);
void TransmissionInfoDataToMain(void);
void TransmissionDetailedDataToMain(void);
unsigned char XmitChangjo(char *xmitData, char slaveAddress);
char ParsingChangjo1(char port, char *rcvData, unsigned char rcvCount, char slaveAddress);
unsigned char XmitChangjo2(char *xmitData, char slaveAddress);
char ParsingChangjo2(char port, char *rcvData, unsigned char rcvCount, char slaveAddress);
unsigned char XmitVariable(char *xmitData, char slaveAddress);
char ParsingVariable(char port, char *rcvData, unsigned char rcvCount, char slaveAddress);
unsigned char XmitNatural(char *xmitData, char slaveAddress);
char ParsingNatural(char port, char *rcvData, unsigned char rcvCount, char slaveAddress);
unsigned char XmitTemperature(char *xmitData, char slaveAddress);
char ParsingTemperature(char port, char *rcvData, unsigned char rcvCount, char slaveAddress);

/* USER CODE END PFP */

#endif /* __BOARD_H__ */
