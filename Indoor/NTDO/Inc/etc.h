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

/* USER CODE END PFP */

#endif /* __BOARD_H__ */
