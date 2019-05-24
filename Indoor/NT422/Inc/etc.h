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
void SetBoardData(unsigned char *infoData);
void TransmissionInfoDataToMain(void);
void TransmissionDetailedDataToMain(void);
unsigned char XmitSangjeonFive(char *xmitData);
char ParsingSangjeonFive(char port, char *rcvData);
unsigned char XmitSangjeonNet(char *xmitData);
char ParsingSangjeonNet(char port, char *rcvData, short rcvCount);
unsigned char XmitRectifierModuleNeo(char *xmitData);
char ParsingRectifierModuleNeo(char port, char *rcvData);
char ParsingRectifierModuleFive(char port, char *rcvData, short rcvCount);
unsigned char XmitRectifierModuleNet(char *xmitData);
char ParsingRectifierModuleNet(char port, char *rcvData, unsigned char rcvCount);
unsigned char XmitTemperatureHumidityModuleNeo(char *xmitData);
char ParsingTemperatureHumidityModuleNeo(char port, char *rcvData);
char ParsingTemperatureHumidityModuleFive(char port, char *rcvData, unsigned char rcvCount);
unsigned char XmitTemperatureHumidityModuleNet(char *xmitData);
char ParsingTemperatureHumidityModuleNet(char port, char *rcvData, unsigned char rcvCount);
unsigned char XmitGenerator(char *xmitData);
char ParsingGenerator(char port, char *rcvData);
char ParsingRfReader(char port, char *rcvData);

/* USER CODE END PFP */

#endif /* __BOARD_H__ */
