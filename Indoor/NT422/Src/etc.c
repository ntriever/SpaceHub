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
unsigned char slotID;
unsigned char replyInfoData[32] = { 0, };
unsigned char replyDetailedData[219] = { 0, };
unsigned char replyData[16] = { 0, };
unsigned char comData[3][69];
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
		case SANGJEON_FIVE :
		case SANGJEON_NET :
			length = 14;
			break;
			
		case SARS_300 :
			length = 62;
			break;
			
		case RECTIFIER_MODULE_NEO :
		case RECTIFIER_MODULE_FIVE :
		case RECTIFIER_MODULE_NET :
			length = 4;
			break;
			
		case TEMPERATURE_HUMIDITY_MODULE_NEO :
		case TEMPERATURE_HUMIDITY_MODULE_FIVE :
		case TEMPERATURE_HUMIDITY_MODULE_NET :
			length = 6;
			break;

		case BMU :
			break;
			
		case GENERATOR :
			length = 8;
			break;
			
		case RF_READER :
			length = 10;
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
		case SANGJEON_FIVE :
			length = 14;
			break;
			
		case SANGJEON_NET :
			length = 16;
			break;
			
		case SARS_300 :
			length = 62;
			break;
			
		case RECTIFIER_MODULE_NEO :
			length = 8;
			break;
			
		case RECTIFIER_MODULE_FIVE :
		case RECTIFIER_MODULE_NET :
			length = 7;
			break;
			
		case TEMPERATURE_HUMIDITY_MODULE_NEO :
			length = 6;
			break;
			
		case TEMPERATURE_HUMIDITY_MODULE_FIVE :
			length = 12;
			break;
			
		case TEMPERATURE_HUMIDITY_MODULE_NET :
			length = 13;
			break;

		case BMU :
			break;
			
		case GENERATOR :
			length = 7;
			break;
			
		case RF_READER :
			length = 12;
			break;

		default :
			length = 0;
			break;
	}
	
	return length;
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
	// 플래시메모리 데이타 확인 및 적용
	// 포트 초기화 : 통신속도 및 설정
	// 연결 장비에 따른 데이타 사이즈 설정
	
	myInfo.slotNumber = slotID;
	myInfo.type = RS422;
	myInfo.id = ~GPIOC->IDR & 0x0F;
	memcpy(&myInfo.version[0], "F0001", 5);

	if( myInfo.id == 1 ) {
		myInfo.number_of_device[0] = 1;
		myInfo.portInfo[0].portNumber = 1;
		myInfo.portInfo[0].type = TYPE_ETC;
		myInfo.portInfo[0].name = RF_READER;
		myInfo.portInfo[0].id = 1;
		myInfo.portInfo[0].dataLength = 10;
		myInfo.portInfo[0].rcvLength = 12;

		myInfo.number_of_device[1] = 1;
		myInfo.portInfo[1].portNumber = 2;
		myInfo.portInfo[1].type = TYPE_RECTIFIER;
		myInfo.portInfo[1].name = RECTIFIER_MODULE_FIVE;
		myInfo.portInfo[1].id = 1;
		myInfo.portInfo[1].dataLength = 4;
		myInfo.portInfo[1].rcvLength = 7;

		myInfo.number_of_device[2] = 1;
		myInfo.portInfo[2].portNumber = 3;
		myInfo.portInfo[2].type = TYPE_RECTIFIER;
		myInfo.portInfo[2].name = RECTIFIER_MODULE_FIVE;
		myInfo.portInfo[2].id = 1;
		myInfo.portInfo[2].dataLength = 4;
		myInfo.portInfo[2].rcvLength = 7;
	}
	else if( myInfo.id == 2 ) {
		myInfo.number_of_device[0] = 1;
		myInfo.portInfo[0].portNumber = 1;
		myInfo.portInfo[0].type = TYPE_WATT_METER;
		myInfo.portInfo[0].name = SANGJEON_FIVE;
		myInfo.portInfo[0].id = 1;
		myInfo.portInfo[0].dataLength = 14;
		myInfo.portInfo[0].rcvLength = 14;

		myInfo.number_of_device[1] = 1;
		myInfo.portInfo[1].portNumber = 2;
		myInfo.portInfo[1].type = TYPE_TEMPERATURE_HUMIDITY;
		myInfo.portInfo[1].name = TEMPERATURE_HUMIDITY_MODULE_FIVE;
		myInfo.portInfo[1].id = 1;
		myInfo.portInfo[1].dataLength = 6;
		myInfo.portInfo[1].rcvLength = 12;

		myInfo.number_of_device[2] = 1;
		myInfo.portInfo[2].portNumber = 3;
		myInfo.portInfo[2].type = TYPE_ETC;
		myInfo.portInfo[2].name = GENERATOR;
		myInfo.portInfo[2].id = 1;
		myInfo.portInfo[2].dataLength = 8;
		myInfo.portInfo[2].rcvLength = 7;
	}
}

/**
  * @brief  This function is a initialize data.
  * @retval None
  */
void InitializeData(void)
{
	short i;
	
	memset(&comData[0][0], 0x00, sizeof(comData));
	for( i = 0; i < 3; i++ ) {
		comData[i][0] = '#';
		comData[i][1] = '#';
		comData[i][2] = i + 1;
		comData[i][3] = myInfo.portInfo[i].type;
		comData[i][4] = myInfo.portInfo[i].name & 0x0FF;
		comData[i][5] = myInfo.portInfo[i].id;
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

	for( i = 0; i < 3; i++ ) {
		myInfo.number_of_device[i] = infoData[31 + (i * 10)];
		myInfo.portInfo[i].portNumber = infoData[24 + (i * 10)];
		myInfo.portInfo[i].type = infoData[25 + (i * 10)];
		myInfo.portInfo[i].name = (infoData[25 + (i * 10)] << 8) | infoData[26 + (i * 10)];
		myInfo.portInfo[i].id = infoData[27 + (i * 10)];
		myInfo.portInfo[i].dataLength = SetslaveAddressDataLength(myInfo.portInfo[i].name);
		myInfo.portInfo[i].rcvLength = SetslaveAddressDataRcvLength(myInfo.portInfo[i].name);
		
		if( myInfo.number_of_device[i] )	MX_UART_Init(i + 1, myInfo.portInfo[i].name);
	}

	memset(&comData, 0x00, sizeof(comData));
	for( i = 0; i < 3; i++ ) {
		comData[i][0] = '#';
		comData[i][1] = '#';
		comData[i][2] = i + 1;
		comData[i][3] = myInfo.portInfo[i].type;
		comData[i][4] = myInfo.portInfo[i].name & 0x0FF;
		comData[i][5] = myInfo.portInfo[i].id;
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
	for( i = 0; i < 3; i++ ) {
		if( myInfo.number_of_device[i] ) {
			memcpy(&replyInfoData[index], &comData[i], 6);
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
	for( i = 0; i < 3; i++ ) {
		if( myInfo.number_of_device[i] ) {
			memcpy(&replyDetailedData[index], &comData[i][0], 6 + myInfo.portInfo[i].dataLength);
			index = index + (6 + myInfo.portInfo[i].dataLength);
		}
	}
	replyDetailedLength = index;
	replyDetailedData[4] = (index - 6) >> 8;			// Length High
	replyDetailedData[5] = (index - 6) & 0x0FF;			// Length Low
}

/**
  * @brief  This function is a 파이브텍 A망 상전 모듈 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitSangjeonFive(char *xmitData)
{
	xmitData[0] = 0xF1;
	xmitData[1] = 0;
	
	xmitData[13] = 0xF1;	// receive data stx
	xmitData[14] = 0xF2;	// receive data etx
	
	return 1;
}

/**
  * @brief  This function is a 파이브텍 A망 상전 모듈 data parsing function.
  * @retval true or false
  */
char ParsingSangjeonFive(char port, char *rcvData)
{
	short index = 6;

	comData[port - 1][index + 0] = 0;							// 상태
	comData[port - 1][index + 1] = 0;
	memcpy(&comData[port - 1][index + 2], &rcvData[1], 12);		// 상전 Data
		
	return 1;
}

/**
  * @brief  This function is a 파이브텍 전송망 상전 모듈 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitSangjeonNet(char *xmitData)
{
	xmitData[0] = 0x02;
	xmitData[1] = 0x10;
	xmitData[2] = 0x03;
	xmitData[3] = 0;
	
	xmitData[13] = 0x02;	// receive data stx
	xmitData[14] = 0x03;	// receive data etx
	
	return 3;
}

/**
  * @brief  This function is a 파이브텍 A망 상전 모듈 data parsing function.
  * @retval true or false
  */
char ParsingSangjeonNet(char port, char *rcvData, short rcvCount)
{
	short index = 6;
	unsigned char checksum = 0;

	for( short i = 0; i < (rcvCount - 2); i++ )	checksum ^= rcvData[i];
	if( checksum == rcvData[rcvCount - 2] ) {
		comData[port - 1][index + 0] = 0;							// 상태
		comData[port - 1][index + 1] = 0;
		memcpy(&comData[port - 1][index + 2], &rcvData[2], 12);		// 상전 Data
		
		return 1;
	}
	
	return 0;
}

/**
  * @brief  This function is a 네오 정류기 모듈 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitRectifierModuleNeo(char *xmitData)
{
	xmitData[0] = 0x05;
	xmitData[1] = 0;
	
	xmitData[13] = 0xF1;	// receive data stx
	xmitData[14] = 0xF2;	// receive data etx
	
	return 1;
}

/**
  * @brief  This function is a 네오 정류기 모듈 data parsing function.
  * @retval true or false
  */
char ParsingRectifierModuleNeo(char port, char *rcvData)
{
	short index = 6, voltage;
	
	comData[port - 1][index + 0] = 0;						// 상태
	comData[port - 1][index + 1] = 0;
	voltage = (rcvData[1] << 8) | rcvData[2];
	voltage = (voltage * 310) / 4096;
	comData[port - 1][index + 2] = voltage >> 8;
	comData[port - 1][index + 3] = voltage & 0x0FF;
		
	return 1;
}

/**
  * @brief  This function is a 파이브텍 A망 정류기 모듈 data parsing function.
  * @retval true or false
  */
char ParsingRectifierModuleFive(char port, char *rcvData, short rcvCount)
{
	short index = 6, voltage;
	unsigned char checksum = 0;
	
	for( short i = 0; i < (rcvCount - 2); i++ )	checksum ^= rcvData[i];
	if( checksum == rcvData[rcvCount - 2] ) {
		comData[port - 1][index + 0] = 0;						// 상태
		comData[port - 1][index + 1] = 0;
		voltage = (rcvData[1] - 0x30) * 1000 + (rcvData[2] - 0x30) * 100 + (rcvData[3] - 0x30) * 10 + (rcvData[4] - 0x30);
		comData[port - 1][index + 2] = voltage >> 8;
		comData[port - 1][index + 3] = voltage & 0x0FF;
		
		return 1;
	}
	
	return 0;
}

/**
  * @brief  This function is a 파이브텍 전송망 정류기 모듈 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitRectifierModuleNet(char *xmitData)
{
	xmitData[0] = 0x02;
	xmitData[1] = 0x20;
	xmitData[2] = 0x03;
	xmitData[3] = 0;
	
	xmitData[13] = 0x02;	// receive data stx
	xmitData[14] = 0x03;	// receive data etx
	
	return 3;
}

/**
  * @brief  This function is a 파이브텍 전송망 정류기 모듈 data parsing function.
  * @retval true or false
  */
char ParsingRectifierModuleNet(char port, char *rcvData, unsigned char rcvCount)
{
	short index = 6, voltage;
	unsigned char checksum = 0;
	
	for( short i = 0; i < (rcvCount - 2); i++ )	checksum ^= rcvData[i];
	if( checksum == rcvData[rcvCount - 2] ) {
		comData[port - 1][index + 0] = 0;						// 상태
		comData[port - 1][index + 1] = 0;
		voltage = (rcvData[2] - 0x30) * 100 + (rcvData[3] - 0x30) * 10 + (rcvData[4] - 0x30);
		comData[port - 1][index + 2] = voltage >> 8;
		comData[port - 1][index + 3] = voltage & 0x0FF;
		
		return 1;
	}
	
	return 0;
}

/**
  * @brief  This function is a 네오 온습도 모듈 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitTemperatureHumidityModuleNeo(char *xmitData)
{
	xmitData[0] = 0xF1;
	xmitData[1] = 0;
	
	xmitData[13] = 0x02;	// receive data stx
	xmitData[14] = 0x03;	// receive data etx
	
	return 1;
}

/**
  * @brief  This function is a 네오 온습도 모듈 data parsing function.
  * @retval true or false
  */
char ParsingTemperatureHumidityModuleNeo(char port, char *rcvData)
{
	short index = 6, temperature, humidity;

	comData[port - 1][index + 0] = 0;		// 상태
	comData[port - 1][index + 1] = 0;

	temperature = (rcvData[1] << 8) | rcvData[2];
	temperature = ((temperature * 1000) / 4096) - 20;
	comData[port - 1][index + 2] = temperature >> 8;		// 온도
	comData[port - 1][index + 3] = temperature & 0x0FF;
	
	humidity = (rcvData[3] << 8) | rcvData[4];
	humidity = ((humidity * 1000) / 4096) + 70;
	comData[port - 1][index + 4] = humidity >> 8;			// 습도
	comData[port - 1][index + 5] = humidity & 0x0FF;

	return 1;
}

/**
  * @brief  This function is a 파이브텍 A망 온습도 모듈 data parsing function.
  * @retval true or false
  */
char ParsingTemperatureHumidityModuleFive(char port, char *rcvData, unsigned char rcvCount)
{
	short index = 6, temperature, humidity;
	unsigned char checksum = 0;
	
	for( short i = 0; i < (rcvCount - 2); i++ )	checksum ^= rcvData[i];
	if( checksum == rcvData[rcvCount - 2] ) {
		comData[port - 1][index + 0] = 0;		// 상태
		comData[port - 1][index + 1] = 0;

		temperature = (rcvData[2] - 0x30) * 1000 + (rcvData[3] - 0x30) * 100 + (rcvData[4] - 0x30) * 10 + (rcvData[5] - 0x30);
		if( rcvData[1] == '-' )		temperature *= (-1);
		comData[port - 1][index + 2] = temperature >> 8;			// 온도
		comData[port - 1][index + 3] = temperature & 0x0FF;
		
		humidity = (rcvData[6] - 0x30) * 1000 + (rcvData[7] - 0x30) * 100 + (rcvData[8] - 0x30) * 10 + (rcvData[9] - 0x30);
		comData[port - 1][index + 4] = humidity >> 8;		// 습도
		comData[port - 1][index + 5] = humidity & 0x0FF;

		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a 파이브텍 전송망 온습도 모듈 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitTemperatureHumidityModuleNet(char *xmitData)
{
	xmitData[0] = 0x02;
	xmitData[1] = 0x30;
	xmitData[2] = 0x03;
	xmitData[3] = 0;
	
	xmitData[13] = 0x02;	// receive data stx
	xmitData[14] = 0x03;	// receive data etx
	
	return 3;
}

/**
  * @brief  This function is a 파이브텍 전송망 온습도 모듈 data parsing function.
  * @retval true or false
  */
char ParsingTemperatureHumidityModuleNet(char port, char *rcvData, unsigned char rcvCount)
{
	short index = 6, temperature, humidity;
	unsigned char checksum = 0;
	
	for( short i = 0; i < (rcvCount - 2); i++ )	checksum ^= rcvData[i];
	if( checksum == rcvData[rcvCount - 2] ) {
		comData[port - 1][index + 0] = 0;		// 상태
		comData[port - 1][index + 1] = 0;

		temperature = (rcvData[3] - 0x30) * 1000 + (rcvData[4] - 0x30) * 100 + (rcvData[5] - 0x30) * 10 + (rcvData[6] - 0x30);
		if( rcvData[2] == '-' )		temperature *= (-1);
		comData[port - 1][index + 2] = temperature >> 8;			// 온도
		comData[port - 1][index + 3] = temperature & 0x0FF;
		
		humidity = (rcvData[7] - 0x30) * 1000 + (rcvData[8] - 0x30) * 100 + (rcvData[9] - 0x30) * 10 + (rcvData[10] - 0x30);
		comData[port - 1][index + 4] = humidity >> 8;		// 습도
		comData[port - 1][index + 5] = humidity & 0x0FF;

		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a 발동발전기 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitGenerator(char *xmitData)
{
	xmitData[0] = 0xF2;
	xmitData[1] = 0;
	
	xmitData[13] = 0xF1;	// receive data stx
	xmitData[14] = 0xF2;	// receive data etx
	
	return 1;
}

/**
  * @brief  This function is a 발동발전기 data parsing function.
  * @retval true or false
  */
char ParsingGenerator(char port, char *rcvData)
{
	short index = 6;

	comData[port - 1][index + 0] = 0;							// 상태
	comData[port - 1][index + 1] = 0;
	comData[port - 1][index + 2] = (rcvData[1] - 0x30) ^ 0x01;
	comData[port - 1][index + 3] = 0;
	comData[port - 1][index + 4] = 0;
	comData[port - 1][index + 5] = 0;
	if( rcvData[3] == '0' )			comData[port - 1][index + 3] = 1;
	else if( rcvData[3] == '2' )	comData[port - 1][index + 4] = 1;
	else if( rcvData[3] == '1' )	comData[port - 1][index + 5] = 1;
	comData[port - 1][index + 6] = rcvData[4] - 0x30;
	comData[port - 1][index + 7] = (rcvData[5] - 0x30) ^ 0x01;
		
	return 1;
}

/**
  * @brief  This function is a RF Reader data parsing function.
  * @retval true or false
  */
char ParsingRfReader(char port, char *rcvData)
{
	short index = 6;
	
	comData[port - 1][index + 0] = 0;						// 상태
	comData[port - 1][index + 1] = 0;
	memcpy(&comData[port - 1][index + 2], &rcvData[2], 8);	// RFID Data
	
	return 1;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
