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
void SetBoardData(char *infoData);
void TransmissionInfoDataToMain(void);
void TransmissionDetailedDataToMain(void);
unsigned char XmitDongACRS1800(char *xmitData, char xmitType);
char ParsingDongACRS1800(char port, char *rcvData, unsigned char rcvCount, char xmitType);
unsigned char XmitDongACRS1800N(char *xmitData, char xmitType);
char ParsingDongACRS1800N(char port, char *rcvData, unsigned char rcvCount, char xmitType);
unsigned char XmitDongAMR(char *xmitData, char xmitType);
char ParsingDongAMR(char port, char *rcvData, unsigned char rcvCount, char xmitType);
unsigned char XmitMBRS1200R(char *xmitData, char xmitType);
char ParsingMBRS1200R(char port, char *rcvData, unsigned char rcvCount, char xmitType);
char ParsingSDPS48N(char port, char *rcvData, unsigned char rcvCount, char xmitType);
unsigned char XmitLORS4150(char *xmitData, char xmitType);
char ParsingLORS4150(char port, char *rcvData, unsigned char rcvCount, char xmitType);

/* USER CODE END PFP */

#endif /* __BOARD_H__ */
