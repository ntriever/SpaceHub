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
#include "usart.h"

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
char slotID;
unsigned char replyInfoData[2000] = { 0, };
unsigned char replyDetailedData[6000] = { 0, };
unsigned char replyData[16] = { 0, };
unsigned char com1Data[200][70];
unsigned char com2Data[200][70];
unsigned char com3Data[200][70];
short replyInfoLength, replyDetailedLength;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  This function is a Set slaveAddress Data Length function.
  * @retval None
  */
unsigned short SetslaveAddressDataLength(unsigned short slaveAddressName)
{
	unsigned short length;
	
	switch( slaveAddressName ) {
		case VARIABLE_AIRCONDITIONER :
			length = 60;
			break;
			
		case NATURAL_AIRCONDITIONER :
			length = 16;
			break;
			
		case CHANGJO2_AIRCONDITIONER :
			length = 30;
			break;
			
		case CHANGJO1_AIRCONDITIONER :
			length = 8;
			break;
			
		case TEMPERATURE_MODULE :
			length = 6;
			break;

		default :
			length = 0;
			break;
	}
	
	return length;
}

/**
  * @brief  This function is a Set slaveAddress Data Length function.
  * @retval None
  */
unsigned short SetslaveAddressDataRcvLength(unsigned short slaveAddressName)
{
	unsigned short length;
	
	switch( slaveAddressName ) {
		case VARIABLE_AIRCONDITIONER :
			length = 63;
			break;
			
		case NATURAL_AIRCONDITIONER :
			length = 45;
			break;
			
		case CHANGJO2_AIRCONDITIONER :
			length = 44;
			break;
			
		case CHANGJO1_AIRCONDITIONER :
			length = 24;
			break;
			
		case TEMPERATURE_MODULE :
			length = 9;
			break;
			
		default :
			length = 0;
			break;
	}
	
	return length;
}

/**
  * @brief  This function is a CRC-16 function.
  * @retval None
  */
unsigned short CRC16(const char *nData, unsigned short wLength)
{
	static const unsigned short wCRCTable[] = {
		0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
		0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
		0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
		0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
		0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
		0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
		0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
		0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
		0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
		0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
		0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
		0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
		0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
		0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
		0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
		0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
		0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
		0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
		0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
		0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
		0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
		0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
		0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
		0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
		0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
		0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
		0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
		0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
		0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
		0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
		0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
		0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
	};

	char nTemp;
	unsigned short wCRCWord = 0xFFFF;

	while( wLength-- ) {
		nTemp = *nData++ ^ wCRCWord;
		wCRCWord >>= 8;
		wCRCWord ^= wCRCTable[nTemp];
	}

	return wCRCWord;
}
     
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
	short i;
	
	myInfo.slotNumber = slotID;
	myInfo.type = RS485;
	myInfo.id = ~GPIOC->IDR & 0x0F;
	memcpy(&myInfo.version[0], "F0001", 5);

	if( myInfo.id == 1 ) {
		myInfo.number_of_device[0] = 8;
		for( i = 0; i < myInfo.number_of_device[0]; i++ ) {
			myInfo.portInfo[0][i].portNumber = 1;
			myInfo.portInfo[0][i].type = TYPE_AIR_CONDITIONER;
			myInfo.portInfo[0][i].name = VARIABLE_AIRCONDITIONER;
			myInfo.portInfo[0][i].id = i + 1;
			myInfo.portInfo[0][i].dataLength = 60;
			myInfo.portInfo[0][i].rcvLength = 63;
		}
		MX_UART_Init(COM1, myInfo.portInfo[0][0].name);

		myInfo.number_of_device[1] = 8;
		for( i = 0; i < myInfo.number_of_device[1]; i++ ) {
			myInfo.portInfo[1][i].portNumber = 2;
			myInfo.portInfo[1][i].type = TYPE_AIR_CONDITIONER;
			myInfo.portInfo[1][i].name = NATURAL_AIRCONDITIONER;
			myInfo.portInfo[1][i].id = i + 1;
			myInfo.portInfo[1][i].dataLength = 16;
			myInfo.portInfo[1][i].rcvLength = 45;
		}
		MX_UART_Init(COM2, myInfo.portInfo[1][0].name);

		myInfo.number_of_device[2] = 0;
		myInfo.portInfo[2][0].portNumber = 3;
		myInfo.portInfo[2][0].type = TYPE_NONE;
		myInfo.portInfo[2][0].name = NAME_NONE;
		myInfo.portInfo[2][0].id = 0;
		myInfo.portInfo[2][0].dataLength = 0;
		myInfo.portInfo[2][0].rcvLength = 0;
	}
	else if( myInfo.id == 2 ) {
		myInfo.number_of_device[0] = 4;
		for( i = 0; i < myInfo.number_of_device[0]; i++ ) {
			myInfo.portInfo[0][i].portNumber = 1;
			myInfo.portInfo[0][i].type = TYPE_AIR_CONDITIONER;
			myInfo.portInfo[0][i].name = CHANGJO1_AIRCONDITIONER;
			myInfo.portInfo[0][i].id = i + 1;
			myInfo.portInfo[0][i].dataLength = 8;
			myInfo.portInfo[0][i].rcvLength = 24;
		}
		MX_UART_Init(COM1, myInfo.portInfo[0][0].name);

		myInfo.number_of_device[1] = 4;
		for( i = 0; i < myInfo.number_of_device[1]; i++ ) {
			myInfo.portInfo[1][i].portNumber = 2;
			myInfo.portInfo[1][i].type = TYPE_AIR_CONDITIONER;
			myInfo.portInfo[1][i].name = CHANGJO2_AIRCONDITIONER;
			myInfo.portInfo[1][i].id = i + 1;
			myInfo.portInfo[1][i].dataLength = 30;
			myInfo.portInfo[1][i].rcvLength = 44;
		}
		MX_UART_Init(COM2, myInfo.portInfo[1][0].name);
		
		myInfo.number_of_device[2] = 100;
		for( i = 0; i < myInfo.number_of_device[2]; i++ ) {
			myInfo.portInfo[2][i].portNumber = 3;
			myInfo.portInfo[2][i].type = TYPE_TEMPERATURE_HUMIDITY;
			myInfo.portInfo[2][i].name = TEMPERATURE_MODULE;
			myInfo.portInfo[2][i].id = i + 1;
			myInfo.portInfo[2][i].dataLength = 6;
			myInfo.portInfo[2][i].rcvLength = 9;
		}
		MX_UART_Init(COM3, myInfo.portInfo[2][0].name);
	}
}

/**
  * @brief  This function is a initialize data.
  * @retval None
  */
void InitializeData(void)
{
	short i;
	
	if( myInfo.id == 1 ) {
		for( i = 0; i < myInfo.number_of_device[0]; i++ ) {
			com1Data[i][0] = '#';
			com1Data[i][1] = '#';
			com1Data[i][2] = 1;
			com1Data[i][3] = myInfo.portInfo[0][i].type;
			com1Data[i][4] = myInfo.portInfo[0][i].name & 0x0FF;
			com1Data[i][5] = myInfo.portInfo[0][i].id;
		}
		
		for( i = 0; i < myInfo.number_of_device[1]; i++ ) {
			com2Data[i][0] = '#';
			com2Data[i][1] = '#';
			com2Data[i][2] = 2;
			com2Data[i][3] = myInfo.portInfo[1][i].type;
			com2Data[i][4] = myInfo.portInfo[1][i].name & 0x0FF;
			com2Data[i][5] = myInfo.portInfo[1][i].id;
		}
		
		for( i = 0; i < myInfo.number_of_device[2]; i++ ) {
			com3Data[i][0] = '#';
			com3Data[i][1] = '#';
			com3Data[i][2] = 3;
			com3Data[i][3] = myInfo.portInfo[2][i].type;
			com3Data[i][4] = myInfo.portInfo[2][i].name & 0x0FF;
			com3Data[i][5] = myInfo.portInfo[2][i].id;
		}
	}
	else if( myInfo.id == 2 ) {
		for( i = 0; i < myInfo.number_of_device[0]; i++ ) {
			com1Data[i][0] = '#';
			com1Data[i][1] = '#';
			com1Data[i][2] = 1;
			com1Data[i][3] = myInfo.portInfo[0][i].type;
			com1Data[i][4] = myInfo.portInfo[0][i].name & 0x0FF;
			com1Data[i][5] = myInfo.portInfo[0][i].id;
		}
		
		for( i = 0; i < myInfo.number_of_device[1]; i++ ) {
			com2Data[i][0] = '#';
			com2Data[i][1] = '#';
			com2Data[i][2] = 2;
			com2Data[i][3] = myInfo.portInfo[1][i].type;
			com2Data[i][4] = myInfo.portInfo[1][i].name & 0x0FF;
			com2Data[i][5] = myInfo.portInfo[1][i].id;
		}
		
		for( i = 0; i < myInfo.number_of_device[2]; i++ ) {
			com3Data[i][0] = '#';
			com3Data[i][1] = '#';
			com3Data[i][2] = 3;
			com3Data[i][3] = myInfo.portInfo[2][i].type;
			com3Data[i][4] = myInfo.portInfo[2][i].name & 0x0FF;
			com3Data[i][5] = myInfo.portInfo[2][i].id;
		}
	}
	
	//  설정 및 제어 응답
	memset(&replyData[0], 0x00, sizeof(replyData));
	replyData[0] = slotID + 0x30;
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

void CheckNumberOfslaveAddress(unsigned char *infoData, short length)
{
	short i, portNumber = 0, count;

	portNumber = infoData[24];
	for( i = 0; i < length; i++ ) {
		if( (infoData[i] == '#') && (infoData[i + 1] == '#') ) {
			myInfo.number_of_device[portNumber - 1] = count / 7;
			portNumber = infoData[i + 2];
			count = 0;
		}
		count++;
	}
}

/**
  * @brief  This function is a function to set the board.
  * @retval None
  */
void SetBoardData(unsigned char *infoData, short length)
{
	short i, j, index;

	CheckNumberOfslaveAddress(infoData, length);

	index = 0;
	for( i = 0; i < 3; i++ ) {
		for( j = 0; j < myInfo.number_of_device[i]; j++ ) {
			myInfo.portInfo[i][j].portNumber = infoData[24 + index];
			myInfo.portInfo[i][j].type = infoData[25 + (j * 7) + index];
			myInfo.portInfo[i][j].name = (infoData[25 + (j * 7) + index] << 8) | infoData[26 + (j * 7) + index];
			myInfo.portInfo[i][j].id = infoData[27 + (j * 7) + index];
			myInfo.portInfo[i][j].dataLength = SetslaveAddressDataLength(myInfo.portInfo[i][j].name);
			myInfo.portInfo[i][i].rcvLength = SetslaveAddressDataRcvLength(myInfo.portInfo[i][j].name);
		}
		index += (myInfo.number_of_device[i] * 7 + 3);
		
		if( myInfo.number_of_device[i] ) {
			if( i == 0 )		MX_UART_Init(COM1, myInfo.portInfo[0][0].name);
			else if( i == 1 )	MX_UART_Init(COM2, myInfo.portInfo[1][0].name);
			else if( i == 2 )	MX_UART_Init(COM3, myInfo.portInfo[2][0].name);
		}
	}

	for( i = 0; i < myInfo.number_of_device[0]; i++ ) {
		com1Data[i][0] = '#';
		com1Data[i][1] = '#';
		com1Data[i][2] = 1;
		com1Data[i][3] = myInfo.portInfo[0][i].type;
		com1Data[i][4] = myInfo.portInfo[0][i].name & 0x0FF;
		com1Data[i][5] = myInfo.portInfo[0][i].id;
	}

	for( i = 0; i < myInfo.number_of_device[1]; i++ ) {
		com2Data[i][0] = '#';
		com2Data[i][1] = '#';
		com2Data[i][2] = 2;
		com2Data[i][3] = myInfo.portInfo[1][i].type;
		com2Data[i][4] = myInfo.portInfo[1][i].name & 0x0FF;
		com2Data[i][5] = myInfo.portInfo[1][i].id;
	}

	for( i = 0; i < myInfo.number_of_device[2]; i++ ) {
		com3Data[i][0] = '#';
		com3Data[i][1] = '#';
		com3Data[i][2] = 3;
		com3Data[i][3] = myInfo.portInfo[2][i].type;
		com3Data[i][4] = myInfo.portInfo[2][i].name & 0x0FF;
		com3Data[i][5] = myInfo.portInfo[2][i].id;
	}
}

/**
  * @brief  This function is a Infomation data transmission function.
  * @retval Transmission Number of Byte
  */
void TransmissionInfoDataToMain(void)
{
	short index = 0, i, j;
	
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
	for( i = 0; i < 3; i++ ) {
		if( i == 0 )		memcpy(&replyInfoData[index], &com1Data[0][0], 6);
		else if( i == 1 )	memcpy(&replyInfoData[index], &com2Data[0][0], 6);
		else if( i == 2 )	memcpy(&replyInfoData[index], &com3Data[0][0], 6);
		index = index + 6;
		
		if( myInfo.number_of_device[i] ) {
			for( j = 1; j < myInfo.number_of_device[i]; j++ ) {
				if( i == 0 )		memcpy(&replyInfoData[index], &com1Data[j][3], 3);
				else if( i == 1 )	memcpy(&replyInfoData[index], &com2Data[j][3], 3);
				else if( i == 2 )	memcpy(&replyInfoData[index], &com3Data[j][3], 3);
				index = index + 3;
			}
		}
	}
	
	replyInfoLength = index;
	replyInfoData[4] = (index - 6) >> 8;				// Length High
	replyInfoData[5] = (index - 6) & 0x0FF;				// Length Low
}

/**
  * @brief  This function is a Detailed data transmission function.
  * @retval Transmission Number of Byte
  */
void TransmissionDetailedDataToMain(void)
{
	short index = 0, i, j;

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
	for( i = 0; i < 3; i++ ) {
		if( myInfo.number_of_device[i] ) {
			if( i == 0 )		memcpy(&replyDetailedData[index], &com1Data[0][0], 6 + myInfo.portInfo[i][0].dataLength);
			else if( i == 1 )	memcpy(&replyDetailedData[index], &com2Data[0][0], 6 + myInfo.portInfo[i][0].dataLength);
			else if( i == 2 )	memcpy(&replyDetailedData[index], &com3Data[0][0], 6 + myInfo.portInfo[i][0].dataLength);
			index = index + (6 + myInfo.portInfo[i][0].dataLength);
			
			for( j = 1; j < myInfo.number_of_device[i]; j++ ) {
				if( i == 0 )		memcpy(&replyDetailedData[index], &com1Data[j][3], 3 + myInfo.portInfo[i][j].dataLength);
				else if( i == 1 )	memcpy(&replyDetailedData[index], &com2Data[j][3], 3 + myInfo.portInfo[i][j].dataLength);
				else if( i == 2 )	memcpy(&replyDetailedData[index], &com3Data[j][3], 3 + myInfo.portInfo[i][j].dataLength);
				index = index + (3 + myInfo.portInfo[i][j].dataLength);
			}
		}
	}
	
	replyDetailedLength = index;
	replyDetailedData[4] = (index - 6) >> 8;			// Length High
	replyDetailedData[5] = (index - 6) & 0x0FF;			// Length Low
}

/**
  * @brief  This function is a ChangJo1 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitChangjo(char *xmitData, char slaveAddress)
{
	xmitData[0] = 'a' + slaveAddress - 1;
	xmitData[1] = '?';
	xmitData[2] = '0';
	xmitData[3] = 0;
	
	xmitData[13] = 0xFF;	// receive data stx
	xmitData[14] = 0;		// receive data etx
	
	return 3;
}

/**
  * @brief  This function is a ChangJo1 data parsing function.
  * @retval true or false
  */
char ParsingChangjo1(char port, char *rcvData, unsigned char rcvCount, char slaveAddress)
{
	unsigned short index = 6;
	unsigned char checksum = 0;

	for( char i = 0; i < (rcvCount - 1); i++ )	checksum += rcvData[i];
	if( checksum == rcvData[rcvCount - 1] ) {
		if( port == COM1 ) {
			com1Data[slaveAddress][index + 0] = 0;				// 상태
			com1Data[slaveAddress][index + 1] = 0;
			com1Data[slaveAddress][index + 2] = 0;				// AC 전원이상
			com1Data[slaveAddress][index + 3] = 0;
			if( rcvData[22] == '1' )	com1Data[slaveAddress][index + 3] = 1;
			com1Data[slaveAddress][index + 4] = 0;				// COMP 이상
			com1Data[slaveAddress][index + 5] = 0;
			if( (rcvData[20] & 0x08) || (rcvData[20] & 0x04) )	com1Data[slaveAddress][index + 5] = 1;
			com1Data[slaveAddress][index + 6] = 0;				// FAN 이상
			com1Data[slaveAddress][index + 7] = 0;
			if( (rcvData[20] & 0x02) || (rcvData[20] & 0x01) )	com1Data[slaveAddress][index + 7] = 1;
		}
		else if( port == COM2 ) {
			com2Data[slaveAddress][index + 0] = 0;				// 상태
			com2Data[slaveAddress][index + 1] = 0;
			com2Data[slaveAddress][index + 2] = 0;				// AC 전원이상
			com2Data[slaveAddress][index + 3] = 0;
			if( rcvData[22] == '1' )	com2Data[slaveAddress][index + 3] = 1;
			com2Data[slaveAddress][index + 4] = 0;				// COMP 이상
			com2Data[slaveAddress][index + 5] = 0;
			if( (rcvData[20] & 0x08) || (rcvData[20] & 0x04) )	com2Data[slaveAddress][index + 5] = 1;
			com2Data[slaveAddress][index + 6] = 0;				// FAN 이상
			com2Data[slaveAddress][index + 7] = 0;
			if( (rcvData[20] & 0x02) || (rcvData[20] & 0x01) )	com2Data[slaveAddress][index + 7] = 1;
		}
		else if( port == COM3 ) {
			com3Data[slaveAddress][index + 0] = 0;				// 상태
			com3Data[slaveAddress][index + 1] = 0;
			com3Data[slaveAddress][index + 2] = 0;				// AC 전원이상
			com3Data[slaveAddress][index + 3] = 0;
			if( rcvData[22] == '1' )	com3Data[slaveAddress][index + 3] = 1;
			com3Data[slaveAddress][index + 4] = 0;				// COMP 이상
			com3Data[slaveAddress][index + 5] = 0;
			if( (rcvData[20] & 0x08) || (rcvData[20] & 0x04) )	com3Data[slaveAddress][index + 5] = 1;
			com3Data[slaveAddress][index + 6] = 0;				// FAN 이상
			com3Data[slaveAddress][index + 7] = 0;
			if( (rcvData[20] & 0x02) || (rcvData[20] & 0x01) )	com3Data[slaveAddress][index + 7] = 1;
		}
		
		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a ChangJo2 data parsing function.
  * @retval true or false
  */
char ParsingChangjo2(char port, char *rcvData, unsigned char rcvCount, char slaveAddress)
{
	unsigned short index = 6;
	unsigned char checksum = 0;

	for( char i = 0; i < (rcvCount - 1); i++ )	checksum += rcvData[i];
	if( checksum == rcvData[rcvCount - 1] ) {
		if( port == COM1 ) {
			com1Data[slaveAddress][index + 0] = 0;				// 상태
			com1Data[slaveAddress][index + 1] = 0;
			com1Data[slaveAddress][index + 2] = 0;				// 장비번호
			com1Data[slaveAddress][index + 3] = rcvData[1];
			memcpy(&com1Data[slaveAddress][index + 4], &rcvData[2], 4);
			memcpy(&com1Data[slaveAddress][index + 8], &rcvData[10], 6);
			com1Data[slaveAddress][index + 14] = 0;				// 동작상태
			com1Data[slaveAddress][index + 15] = rcvData[26];
			memcpy(&com1Data[slaveAddress][index + 16], &rcvData[27], 12);
			
		}
		else if( port == COM2 ) {
			com2Data[slaveAddress][index + 0] = 0;				// 상태
			com2Data[slaveAddress][index + 1] = 0;
			com2Data[slaveAddress][index + 2] = 0;				// 장비번호
			com2Data[slaveAddress][index + 3] = rcvData[1];
			memcpy(&com2Data[slaveAddress][index + 4], &rcvData[2], 4);
			memcpy(&com2Data[slaveAddress][index + 8], &rcvData[10], 6);
			com2Data[slaveAddress][index + 14] = 0;				// 동작상태
			com2Data[slaveAddress][index + 15] = rcvData[26];
			memcpy(&com2Data[slaveAddress][index + 16], &rcvData[27], 12);
		}
		else if( port == COM3 ) {
			com3Data[slaveAddress][index + 0] = 0;				// 상태
			com3Data[slaveAddress][index + 1] = 0;
			com3Data[slaveAddress][index + 2] = 0;				// 장비번호
			com3Data[slaveAddress][index + 3] = rcvData[1];
			memcpy(&com3Data[slaveAddress][index + 4], &rcvData[2], 4);
			memcpy(&com3Data[slaveAddress][index + 8], &rcvData[10], 6);
			com3Data[slaveAddress][index + 14] = 0;				// 동작상태
			com3Data[slaveAddress][index + 15] = rcvData[26];
			memcpy(&com3Data[slaveAddress][index + 16], &rcvData[27], 12);
		}
		
		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a 가변용량제어냉방기 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitVariable(char *xmitData, char slaveAddress)
{
	unsigned short crc16;
	
	xmitData[0] = slaveAddress;					// modbus slave address
	xmitData[1] = READ_HOLDING_REGISTERS;		// function code
	xmitData[2] = 0;							// start address
	xmitData[3] = 201;
	xmitData[4] = 0;							// number of register
	xmitData[5] = 29;
	crc16 = CRC16(&xmitData[0], 6);
	xmitData[6] = crc16 & 0x0FF;
	xmitData[7] = crc16 >> 8;
	
	xmitData[13] = slaveAddress;			// receive data stx
	xmitData[14] = 0;							// receive data etx
	
	return 8;
}

/**
  * @brief  This function is a 가변용량제어냉방기 data parsing function.
  * @retval true or false
  */
char ParsingVariable(char port, char *rcvData, unsigned char rcvCount, char slaveAddress)
{
	unsigned short index = 6, crc16;
	char crcHigh, crcLow;

	crc16 = CRC16(&rcvData[0], rcvCount - 2);
	crcHigh = crc16 >> 8;
	crcLow = crc16 & 0x0FF;
	
	if( (rcvData[rcvCount - 2] == crcLow) && (rcvData[rcvCount - 1] == crcHigh) ) {
		if( port == COM1 ) {
			com1Data[slaveAddress][index + 0] = 0;				// 상태
			com1Data[slaveAddress][index + 1] = 0;
			memcpy(&com1Data[slaveAddress][index + 2], &rcvData[3], 58);
		}
		else if( port == COM2 ) {
			com2Data[slaveAddress][index + 0] = 0;				// 상태
			com2Data[slaveAddress][index + 1] = 0;
			memcpy(&com2Data[slaveAddress][index + 2], &rcvData[3], 58);
		}
		else if( port == COM3 ) {
			com3Data[slaveAddress][index + 0] = 0;				// 상태
			com3Data[slaveAddress][index + 1] = 0;
			memcpy(&com3Data[slaveAddress][index + 2], &rcvData[3], 58);
		}
		
		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a 자연공조냉방기 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitNatural(char *xmitData, char slaveAddress)
{
	unsigned short crc16;
	
	xmitData[0] = slaveAddress;					// modbus slave address
	xmitData[1] = READ_INPUT_REGISTERS;			// function code
	xmitData[2] = 0;							// start address
	xmitData[3] = 0;
	xmitData[4] = 0;							// number of register
	xmitData[5] = 20;
	crc16 = CRC16(&xmitData[0], 6);
	xmitData[6] = crc16 & 0x0FF;
	xmitData[7] = crc16 >> 8;
	
	xmitData[13] = slaveAddress;			// receive data stx
	xmitData[14] = 0;							// receive data etx
	
	return 8;
}

/**
  * @brief  This function is a 자연공조제어냉방기 data parsing function.
  * @retval true or false
  */
char ParsingNatural(char port, char *rcvData, unsigned char rcvCount, char slaveAddress)
{
	unsigned short index = 6, crc16;
	char crcHigh, crcLow, tmp;

	crc16 = CRC16(&rcvData[0], rcvCount - 2);
	crcHigh = crc16 >> 8;
	crcLow = crc16 & 0x0FF;
	
	if( (rcvData[rcvCount - 2] == crcLow) && (rcvData[rcvCount - 1] == crcHigh) ) {
		if( port == COM1 ) {
			com1Data[slaveAddress][index + 0] = 0;				// 상태
			com1Data[slaveAddress][index + 1] = 0;
			com1Data[slaveAddress][index + 2] = rcvData[41];	// 장비번호
			com1Data[slaveAddress][index + 3] = rcvData[42];
			com1Data[slaveAddress][index + 4] = rcvData[3];		// F/W 버전
			com1Data[slaveAddress][index + 5] = rcvData[4];
			memcpy(&com1Data[slaveAddress][index + 6], &rcvData[19], 8);	
			tmp = ((rcvData[40] << 5) & 0xC0);
			tmp |= ((rcvData[38] << 2) & 0x3C);
			tmp |= ((rcvData[35] >> 3) & 0x03);
			com1Data[slaveAddress][index + 14] = tmp;			// 상태 High
			tmp = ((rcvData[35] << 5) & 0xC0);
			tmp |= (rcvData[36] & 0x3F);
			com1Data[slaveAddress][index + 15] = tmp;			// 상태 Low
		}
		else if( port == COM2 ) {
			com2Data[slaveAddress][index + 0] = 0;				// 상태
			com2Data[slaveAddress][index + 1] = 0;
			com2Data[slaveAddress][index + 2] = rcvData[41];	// 장비번호
			com2Data[slaveAddress][index + 3] = rcvData[42];
			com2Data[slaveAddress][index + 4] = rcvData[3];		// F/W 버전
			com2Data[slaveAddress][index + 5] = rcvData[4];
			memcpy(&com2Data[slaveAddress][index + 6], &rcvData[19], 8);	
			tmp = ((rcvData[40] << 5) & 0xC0);
			tmp |= ((rcvData[38] << 2) & 0x3C);
			tmp |= ((rcvData[35] >> 3) & 0x03);
			com2Data[slaveAddress][index + 14] = tmp;			// 상태 High
			tmp = ((rcvData[35] << 5) & 0xC0);
			tmp |= (rcvData[36] & 0x3F);
			com2Data[slaveAddress][index + 15] = tmp;			// 상태 Low
		}
		else if( port == COM3 ) {
			com3Data[slaveAddress][index + 0] = 0;				// 상태
			com3Data[slaveAddress][index + 1] = 0;
			com3Data[slaveAddress][index + 2] = rcvData[41];	// 장비번호
			com3Data[slaveAddress][index + 3] = rcvData[42];
			com3Data[slaveAddress][index + 4] = rcvData[3];		// F/W 버전
			com3Data[slaveAddress][index + 5] = rcvData[4];
			memcpy(&com3Data[slaveAddress][index + 6], &rcvData[19], 8);	
			tmp = ((rcvData[40] << 5) & 0xC0);
			tmp |= ((rcvData[38] << 2) & 0x3C);
			tmp |= ((rcvData[35] >> 3) & 0x03);
			com3Data[slaveAddress][index + 14] = tmp;			// 상태 High
			tmp = ((rcvData[35] << 5) & 0xC0);
			tmp |= (rcvData[36] & 0x3F);
			com3Data[slaveAddress][index + 15] = tmp;			// 상태 Low
		}
		
		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a 온도모듈 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitTemperature(char *xmitData, char slaveAddress)
{
	unsigned short crc16;
	
	xmitData[0] = slaveAddress;					// modbus slave address
	xmitData[1] = READ_HOLDING_REGISTERS;		// function code
	xmitData[2] = 0;							// start address
	xmitData[3] = 21;
	xmitData[4] = 0;							// number of register
	xmitData[5] = 2;
	crc16 = CRC16(&xmitData[0], 6);
	xmitData[6] = crc16 & 0x0FF;
	xmitData[7] = crc16 >> 8;
	
	xmitData[13] = slaveAddress;			// receive data stx
	xmitData[14] = 0;							// receive data etx
	
	return 8;
}

/**
  * @brief  This function is a 온도모듈 data parsing function.
  * @retval true or false
  */
char ParsingTemperature(char port, char *rcvData, unsigned char rcvCount, char slaveAddress)
{
	unsigned short index = 6, crc16;
	char crcHigh, crcLow;

	crc16 = CRC16(&rcvData[0], rcvCount - 2);
	crcHigh = crc16 >> 8;
	crcLow = crc16 & 0x0FF;
	
	if( (rcvData[rcvCount - 2] == crcLow) && (rcvData[rcvCount - 1] == crcHigh) ) {
		if( port == COM1 ) {
			com1Data[slaveAddress][index + 0] = 0;				// 상태
			com1Data[slaveAddress][index + 1] = 0;
			com1Data[slaveAddress][index + 2] = rcvData[5];		// 온도
			com1Data[slaveAddress][index + 3] = rcvData[6];
			com1Data[slaveAddress][index + 4] = rcvData[3];		// 습도
			com1Data[slaveAddress][index + 5] = rcvData[4];
		}
		else if( port == COM2 ) {
			com2Data[slaveAddress][index + 0] = 0;				// 상태
			com2Data[slaveAddress][index + 1] = 0;
			com2Data[slaveAddress][index + 2] = rcvData[5];		// 온도
			com2Data[slaveAddress][index + 3] = rcvData[6];
			com2Data[slaveAddress][index + 4] = rcvData[3];		// 습도
			com2Data[slaveAddress][index + 5] = rcvData[4];
		}
		else if( port == COM3 ) {
			com3Data[slaveAddress][index + 0] = 0;				// 상태
			com3Data[slaveAddress][index + 1] = 0;
			com3Data[slaveAddress][index + 2] = rcvData[5];		// 온도
			com3Data[slaveAddress][index + 3] = rcvData[6];
			com3Data[slaveAddress][index + 4] = rcvData[3];		// 습도
			com3Data[slaveAddress][index + 5] = rcvData[4];
		}
		
		return 1;
	}
	else	return 0;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
