/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : etc.c
  * @brief          : Code for User Application
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "etc.h"
#include "main.h"
#include "adc.h"
#include "board.h"

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
BoardInfo myInfo;
unsigned char slotID;
unsigned char replyInfoData[38] = { 0, };
unsigned char replyDetailedData[43] = { 0, };
unsigned char replyData[16] = { 0, };
unsigned char portData[4][8];
short replyInfoLength, replyDetailedLength;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  This function is a function that reads the board ID.
  * @retval None
  */
void ReadBoardID(void)
{
	short count = 0;
	int oldAdc = 0, nowAdc = -1;

	while( count < 10000 ) {
		HAL_ADC_Start(&hadc1);
		if( HAL_ADC_PollForConversion(&hadc1, 1000000) == HAL_OK) {
			if( nowAdc < 0 )	nowAdc = HAL_ADC_GetValue(&hadc1);
			else {
				oldAdc = nowAdc;
				nowAdc = HAL_ADC_GetValue(&hadc1);
				if( nowAdc == oldAdc ) {
					if( (nowAdc > 3700) && (nowAdc < 3800) )		slotID = 1;
					else if( (nowAdc > 3300) && (nowAdc < 3400) )	slotID = 2;
					else if( (nowAdc > 2900) && (nowAdc < 3030) )	slotID = 3;
					else if( (nowAdc > 2580) && (nowAdc < 2700) )	slotID = 4;
					else if( (nowAdc > 2200) && (nowAdc < 2300) )	slotID = 5;
					else if( (nowAdc > 1800) && (nowAdc < 1900) )	slotID = 6;
					else if( (nowAdc > 1400) && (nowAdc < 1530) )	slotID = 7;
					else if( (nowAdc > 1080) && (nowAdc < 1200) )	slotID = 8;
					else if( (nowAdc >  700) && (nowAdc <  800) )	slotID = 9;
					else if( (nowAdc >  300) && (nowAdc <  420) )	slotID = 10;
					else if( (nowAdc >=   0) && (nowAdc <   10) )	slotID = 11;
					break;
				}
			}
		}
		HAL_ADC_Stop(&hadc1);
	}
	if( count >= 10000 ) {
		printf("A/D Converter Channel Error.....\r\n");
	}

	printf("[Board Information]\r\n");
	printf(" Slot Number : %02d\r\n\n", slotID);
}

/**
  * @brief  This function is a initialize board information.
  * @retval None
  */
void InitializeBoardInfo(void)
{
	// 플래시메모리 데이타 확인 및 적용
	// 포트 초기화 : 통신속도 및 설정
	// 연결 장비에 따른 데이타 사이즈 설정
	
	myInfo.slotNumber = slotID;
	myInfo.type = DO;
	myInfo.id = ~GPIOC->IDR & 0x0F;
	memcpy(&myInfo.version[0], "F0001", 5);

	myInfo.number_of_device[0] = 0;
	myInfo.portInfo[0].portNumber = 1;
	myInfo.portInfo[0].type = TYPE_SENSOR;
	myInfo.portInfo[0].name = DOOR_STRIKE;
	myInfo.portInfo[0].id = 1;
	myInfo.portInfo[0].dataLength = 1;
	myInfo.portInfo[0].rcvLength = 0;

	myInfo.number_of_device[1] = 0;
	myInfo.portInfo[1].portNumber = 2;
	myInfo.portInfo[1].type = TYPE_SENSOR;
	myInfo.portInfo[1].name = LIGHT_CONTROL_MODULE;
	myInfo.portInfo[1].id = 1;
	myInfo.portInfo[1].dataLength = 1;
	myInfo.portInfo[1].rcvLength = 0;

	myInfo.number_of_device[2] = 0;
	myInfo.portInfo[2].portNumber = 3;
	myInfo.portInfo[2].type = TYPE_SENSOR;
	myInfo.portInfo[2].name = DOOR_STRIKE;
	myInfo.portInfo[2].id = 1;
	myInfo.portInfo[2].dataLength = 1;
	myInfo.portInfo[2].rcvLength = 0;

	myInfo.number_of_device[3] = 0;
	myInfo.portInfo[3].portNumber = 4;
	myInfo.portInfo[3].type = TYPE_SENSOR;
	myInfo.portInfo[3].name = LIGHT_CONTROL_MODULE;
	myInfo.portInfo[3].id = 1;
	myInfo.portInfo[3].dataLength = 1;
	myInfo.portInfo[3].rcvLength = 0;
}

/**
  * @brief  This function is a initialize data.
  * @retval None
  */
void InitializeData(void)
{
	short i;
	
	// 변경된 사항에 따라 버퍼 및 설정 초기화

	memset(&portData[0][0], 0x00, sizeof(portData));
	memset(&portData[0][0], 0x00, sizeof(portData));
	for( i = 0; i < 4; i++ ) {
		if( myInfo.number_of_device[i] ) {
			portData[i][0] = '#';
			portData[i][1] = '#';
			portData[i][2] = i + 1;
			portData[i][3] = myInfo.portInfo[i].type;
			portData[i][4] = myInfo.portInfo[i].name & 0x0FF;
			portData[i][5] = myInfo.portInfo[i].id;
		}
		else {
			portData[i][0] = '#';
			portData[i][1] = '#';
			portData[i][2] = i + 1;
			portData[i][3] = 0;//myInfo.portInfo[i].type;
			portData[i][4] = 0;//myInfo.portInfo[i].name & 0x0FF;
			portData[i][5] = 0;//myInfo.portInfo[i].id;
		}
	}

	//  설정 및 제어 응답
	memset(&replyData[0], 0x00, sizeof(replyData));
	replyData[0] = myInfo.slotNumber + 0x30;
	replyData[1] = 'M';
	replyData[2] = 'T';
	replyData[3] = 0x06;
	replyData[4] = 0;
	replyData[5] = 9;
	replyData[6] = myInfo.type;
	replyData[7] = myInfo.id;
	memcpy(&replyDetailedData[8], &myInfo.version[0], 5);
	replyData[13] = 0;
	replyData[14] = 0;
}

/**
  * @brief  This function is a function to set the board.
  * @retval None
  */
void SetBoardData(unsigned char *infoData)
{
	short i;
	
	for( i = 0; i < 4; i++ ) {
		if( infoData[31 + (i * 10)] == 1 ) {
			myInfo.number_of_device[i] = 1;
			myInfo.portInfo[i].portNumber = infoData[24 + (i * 10)];
			myInfo.portInfo[infoData[24 + (i * 10)] - 1].type = infoData[25 + (i * 10)];
			myInfo.portInfo[infoData[24 + (i * 10)] - 1].name = (infoData[25 + (i * 10)] << 8) | infoData[26 + (i * 10)];
			myInfo.portInfo[infoData[24 + (i * 10)] - 1].id = infoData[27 + (i * 10)];
		}
		else	myInfo.number_of_device[i] = 0;
	}

	memset(&portData[0][0], 0x00, sizeof(portData));
	for( i = 0; i < 4; i++ ) {
		if( myInfo.number_of_device[i] ) {
			portData[i][0] = '#';
			portData[i][1] = '#';
			portData[i][2] = i + 1;
			portData[i][3] = myInfo.portInfo[i].type;
			portData[i][4] = myInfo.portInfo[i].name & 0x0FF;
			portData[i][5] = myInfo.portInfo[i].id;
		}
		else {
			portData[i][0] = '#';
			portData[i][1] = '#';
			portData[i][2] = i + 1;
			portData[i][3] = 0;//myInfo.portInfo[i].type;
			portData[i][4] = 0;//myInfo.portInfo[i].name & 0x0FF;
			portData[i][5] = 0;//myInfo.portInfo[i].id;
		}
	}
}

void TransmissionInfoDataToMain(void)
{
	short index = 0, i;
	
	//  보드 정보 요청 응답
	memset(&replyInfoData[0], 0x00, sizeof(replyInfoData));
	replyInfoData[index++] = myInfo.slotNumber + 0x30;
	replyInfoData[index++] = 'M';
	replyInfoData[index++] = 'T';
	replyInfoData[index++] = REQUEST_INFO;
	replyInfoData[index++] = 0;							// Length High
	replyInfoData[index++] = 0;							// Length Low
	replyInfoData[index++] = myInfo.type;
	replyInfoData[index++] = myInfo.id;
	for( i = 0; i < 5; i++ )	replyInfoData[index++] = myInfo.version[i];
	for( i = 0; i < 4; i++ ) {
		if( myInfo.number_of_device[i] ) {
			memcpy(&replyInfoData[index], &portData[i], 6);
			index = index + 6;
		}
	}
	replyInfoLength = index;
	replyInfoData[4] = (index - 6) >> 8;				// Length High
	replyInfoData[5] = (index - 6) & 0x0FF;				// Length Low
}

void TransmissionDetailedDataToMain(void)
{
	short index = 0, i;

	//  보드 상세 정보 요청 응답
	memset(&replyDetailedData[0], 0x00, sizeof(replyDetailedData));
	replyDetailedData[index++] = myInfo.slotNumber + 0x30;
	replyDetailedData[index++] = 'M';
	replyDetailedData[index++] = 'T';
	replyDetailedData[index++] = REQUEST_DATA;
	replyDetailedData[index++] = 0;						// Length High
	replyDetailedData[index++] = 0;						// Length Low
	replyDetailedData[index++] = myInfo.type;
	replyDetailedData[index++] = myInfo.id;
	for( i = 0; i < 5; i++ )	replyDetailedData[index++] = myInfo.version[i];
	replyDetailedData[index++] = 0;
	for( i = 0; i < 4; i++ ) {
		if( myInfo.number_of_device[i] ) {
			memcpy(&replyDetailedData[index], &portData[i], 6 + myInfo.portInfo[i].dataLength);
			index = index + (6 + myInfo.portInfo[i].dataLength);
		}
	}
	replyDetailedLength = index;
	replyDetailedData[4] = (index - 6) >> 8;			// Length High
	replyDetailedData[5] = (index - 6) & 0x0FF;			// Length Low
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
