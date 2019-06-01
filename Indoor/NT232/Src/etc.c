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
unsigned char useModule[3] = { 0, };
short replyInfoLength, replyDetailedLength;
unsigned short crc_table[256];
unsigned char crcHigh, crcLow;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void crc_check(unsigned char len, unsigned char *ptr) 
{ 
	unsigned char a, b, i, j; 
	unsigned int crc_accum = 0; 

	for( j = 0; j < len; j++ ) { 
		i = ((int)(crc_accum >> 8) ^ *ptr++) & 0xFF; 
		crc_accum = (crc_accum << 8) ^ crc_table[i]; 
	} 
	a = crc_accum / 256;
	b = crc_accum % 256; 
	
	if( a == 0xC5 || a == 0xCA )	a += 0x30; 
	if( b == 0xC5 || b == 0xCA )	b += 0x30; 
	
	crcHigh = a;
	crcLow = b; 
}

void get_crc_table() 
{ 
	unsigned int i, j, crc_accum;

	for( i = 0; i < 256; i++ ) { 
		crc_accum = ((unsigned int)i << 8); 
		for( j = 0; j < 8; j++ ) { 
			if( crc_accum & 0x8000L )	crc_accum = (crc_accum << 1) ^ 0x8005; 
			else						crc_accum = (crc_accum << 1); 
		}
		
		crc_table[i] = crc_accum; 
	} 
} 

/**
  * @brief  This function is a Set slaveAddress Data Length function.
  * @retval None
  */
/*
unsigned short SetslaveAddressDataRcvLength(unsigned short slaveAddressName)
{
	unsigned short length;
	
	switch( slaveAddressName ) {
		case DONGA_CRS1800 :
		case DONGA_CRS1800N :
		case ANTS_CRS1800 :
		case DONGA_MR :
		case EMERSON :
		case FAN_CONTROL :
		case AIRCONDITIONER_MICOM :
			break;
		
		case MBRS_1200R :
		case MBRS_1200N :
		case MBRS_1200 :
			break;
			
		case CRS24_150 :
		case SDPS_48N :
		case SWPS_4150W2 :
		case LORS_4150 :
		case LORS_460 :
		case NORS_4150 :
		case NORS_460 :
			break;

		default :
			length = 0;
			break;
	}
	
	return length;
}
*/
/* USER CODE END 0 */

/**
  * @brief  This function is a function that reads the board ID.
  * @retval None
  */
void ReadBoardID(void)
{
	short count = 0;
	int oldAdc = 0, nowAdc = -1;

	get_crc_table();
	
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
	myInfo.slotNumber = slotID;
	myInfo.type = RS232;
	myInfo.id = ~GPIOC->IDR & 0x0F;
	memcpy(&myInfo.version[0], "F0001", 5);

	if( myInfo.id == 1 ) {
		myInfo.number_of_device[0] = 1;
		myInfo.portInfo[0].portNumber = 1;
		myInfo.portInfo[0].type = TYPE_RECTIFIER;
		myInfo.portInfo[0].name = DONGA_CRS1800;
		myInfo.portInfo[0].id = 1;
		myInfo.portInfo[0].dataLength = 62;
		myInfo.portInfo[0].rcvLength = 0;
		if( myInfo.number_of_device[0] )	MX_UART_Init(COM1, myInfo.portInfo[0].name);

		myInfo.number_of_device[1] = 1;
		myInfo.portInfo[1].portNumber = 2;
		myInfo.portInfo[1].type = TYPE_RECTIFIER;
		myInfo.portInfo[1].name = DONGA_MR;
		myInfo.portInfo[1].id = 1;
		myInfo.portInfo[1].dataLength = 62;
		myInfo.portInfo[1].rcvLength = 0;
		if( myInfo.number_of_device[1] )	MX_UART_Init(COM2, myInfo.portInfo[1].name);
		
		myInfo.number_of_device[2] = 1;
		myInfo.portInfo[2].portNumber = 3;
		myInfo.portInfo[2].type = TYPE_RECTIFIER;
		myInfo.portInfo[2].name = DONGA_CRS1800;
		myInfo.portInfo[2].id = 1;
		myInfo.portInfo[2].dataLength = 62;
		myInfo.portInfo[2].rcvLength = 0;
		if( myInfo.number_of_device[2] )	MX_UART_Init(COM3, myInfo.portInfo[2].name);
	}
	else if( myInfo.id == 2 ) {
		myInfo.number_of_device[0] = 1;
		myInfo.portInfo[0].portNumber = 1;
		myInfo.portInfo[0].type = TYPE_RECTIFIER;
		myInfo.portInfo[0].name = DONGA_CRS1800;
		myInfo.portInfo[0].id = 1;
		myInfo.portInfo[0].dataLength = 62;
		myInfo.portInfo[0].rcvLength = 0;
		if( myInfo.number_of_device[0] )	MX_UART_Init(COM1, myInfo.portInfo[0].name);

		myInfo.number_of_device[1] = 1;
		myInfo.portInfo[1].portNumber = 2;
		myInfo.portInfo[1].type = TYPE_RECTIFIER;
		myInfo.portInfo[1].name = DONGA_CRS1800;
		myInfo.portInfo[1].id = 1;
		myInfo.portInfo[1].dataLength = 62;
		myInfo.portInfo[1].rcvLength = 0;
		if( myInfo.number_of_device[1] )	MX_UART_Init(COM2, myInfo.portInfo[1].name);
		
		myInfo.number_of_device[2] = 1;
		myInfo.portInfo[2].portNumber = 3;
		myInfo.portInfo[2].type = TYPE_RECTIFIER;
		myInfo.portInfo[2].name = DONGA_CRS1800;
		myInfo.portInfo[2].id = 1;
		myInfo.portInfo[2].dataLength = 62;
		myInfo.portInfo[2].rcvLength = 0;
		if( myInfo.number_of_device[2] )	MX_UART_Init(COM3, myInfo.portInfo[2].name);
	}
	else if( myInfo.id == 3 ) {
		myInfo.number_of_device[0] = 1;
		myInfo.portInfo[0].portNumber = 1;
		myInfo.portInfo[0].type = TYPE_RECTIFIER;
		myInfo.portInfo[0].name = DONGA_CRS1800;
		myInfo.portInfo[0].id = 1;
		myInfo.portInfo[0].dataLength = 62;
		myInfo.portInfo[0].rcvLength = 0;
		if( myInfo.number_of_device[0] )	MX_UART_Init(COM1, myInfo.portInfo[0].name);

		myInfo.number_of_device[1] = 1;
		myInfo.portInfo[1].portNumber = 2;
		myInfo.portInfo[1].type = TYPE_RECTIFIER;
		myInfo.portInfo[1].name = DONGA_MR;
		myInfo.portInfo[1].id = 1;
		myInfo.portInfo[1].dataLength = 62;
		myInfo.portInfo[1].rcvLength = 0;
		if( myInfo.number_of_device[1] )	MX_UART_Init(COM2, myInfo.portInfo[1].name);
		
		myInfo.number_of_device[2] = 0;
		myInfo.portInfo[2].portNumber = 3;
		myInfo.portInfo[2].type = TYPE_NONE;
		myInfo.portInfo[2].name = NAME_NONE;
		myInfo.portInfo[2].id = 0;
		myInfo.portInfo[2].dataLength = 0;
		myInfo.portInfo[2].rcvLength = 0;
		if( myInfo.number_of_device[2] )	MX_UART_Init(COM2, myInfo.portInfo[2].name);
	}
}

/**
  * @brief  This function is a initialize data.
  * @retval None
  */
void InitializeData(void)
{
	short i;

	memset(&comData, 0x00, sizeof(comData));
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
void SetBoardData(char *infoData)
{
	short i;
	
	for( i = 0; i < 3; i++ ) {
		myInfo.number_of_device[i] = infoData[31 + (i * 10)];
		myInfo.portInfo[i].portNumber = infoData[24 + (i * 10)];
		myInfo.portInfo[i].type = infoData[25 + (i * 10)];
		myInfo.portInfo[i].name = (infoData[25 + (i * 10)] << 8) | infoData[26 + (i * 10)];
		myInfo.portInfo[i].id = infoData[27 + (i * 10)];
		myInfo.portInfo[i].dataLength = 62;
		myInfo.portInfo[i].rcvLength = 0;
		
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
		memcpy(&replyInfoData[index], &comData[i], 6);
		index = index + 6;
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
		memcpy(&replyDetailedData[index], &comData[i], 6 + myInfo.portInfo[i].dataLength);
		index = index + (6 + myInfo.portInfo[i].dataLength);
	}
	replyDetailedLength = index;
	replyDetailedData[4] = (index - 6) >> 8;			// Length High
	replyDetailedData[5] = (index - 6) & 0x0FF;			// Length Low
}

/**
  * @brief  This function is a DongA CRS1800 data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitDongACRS1800(char *xmitData, char xmitType)
{
	xmitData[0] = 0xC5;			// STX
	xmitData[1] = 0x01;			// Destination Address
	xmitData[2] = 0x00;			// Source Address
	xmitData[3] = 0x01;			// Length
	if( xmitType == 0 )			xmitData[4] = 'R';			// Command
	else if( xmitType == 1 )	xmitData[4] = 'I';			// Command
	else						xmitData[4] = 'M';			// Command
	xmitData[5] = 0xCA;			// ETX
	xmitData[6] = 0;			// checksum
	xmitData[7] = 0;			// CRCL
	
	for( char i = 0; i < 6; i++ )	xmitData[6] += xmitData[i];
	xmitData[6] = 0x100 - xmitData[6];
	
	xmitData[13] = 0xC5;		// receive data stx
	xmitData[14] = 0xCA;		// receive data etx
	
	return 7;
}

/**
  * @brief  This function is a DongA CRS1800 data parsing function.
  * @retval true or false
  */
char ParsingDongACRS1800(char port, char *rcvData, unsigned char rcvCount, char xmitType)
{
	short index = 6;
	unsigned char checksum = 0;
	short tbuf, tbuf1, tbuf2;

	for( char i = 0; i < rcvCount; i++ )	checksum += rcvData[i];
	if( checksum == 0 ) {
		if( xmitType == 0 ) {
			comData[port - 1][index + 0] = 0;				// 정류기 상태
			comData[port - 1][index + 1] = 0;
			comData[port - 1][index + 2] = 0;				// 정류기 펌웨어 버전
			comData[port - 1][index + 3] = 0;
			tbuf = (rcvData[9] - 0x30) * 100 + (rcvData[10] - 0x30) * 10 + (rcvData[11] - 0x30);
			comData[port - 1][index + 4] = tbuf >> 8;		// AC 전압
			comData[port - 1][index + 5] = tbuf & 0x0FF;
			tbuf = (rcvData[5] - 0x30) * 100 + (rcvData[6] - 0x30) * 10 + (rcvData[7] - 0x30);
			comData[port - 1][index + 6] = tbuf >> 8;		// 출력 전압
			comData[port - 1][index + 7] = tbuf & 0x0FF;
			tbuf1 = (rcvData[18] - 0x30) * 1000 + (rcvData[19] - 0x30) * 100 + (rcvData[20] - 0x30) * 10 + (rcvData[21] - 0x30);
			comData[port - 1][index + 8] = tbuf1 >> 8;		// 출력 전류
			comData[port - 1][index + 9] = tbuf1 & 0x0FF;
			tbuf2 = (rcvData[23] - 0x30) * 1000 + (rcvData[24] - 0x30) * 100 + (rcvData[25] - 0x30) * 10 + (rcvData[26] - 0x30);
			if( rcvData[22] == '-' )	tbuf2 *= -1;
			comData[port - 1][index + 12] = tbuf2 >> 8;		// 축전지 전류
			comData[port - 1][index + 13] = tbuf2 & 0x0FF;
			if( useModule[port - 1] )	tbuf = (tbuf1 + tbuf2) / useModule[port - 1];
			comData[port - 1][index + 10] = tbuf >> 8;		// 모듈당 부하 전류
			comData[port - 1][index + 11] = tbuf & 0x0FF;
			comData[port - 1][index + 14] = 0;				// 정류기 경보 상태
			comData[port - 1][index + 15] = 0;
			if( rcvData[34] & 0x01 )	comData[port - 1][index + 15] |= 0x01;
			if( rcvData[34] & 0x08 )	comData[port - 1][index + 15] |= 0x04;
			if( rcvData[34] & 0x10 )	comData[port - 1][index + 15] |= 0x02;
			if( rcvData[34] & 0x40 )	comData[port - 1][index + 15] |= 0x08;
			if( rcvData[37] & 0x80 )	comData[port - 1][index + 15] |= 0x80;
			comData[port - 1][index + 16] = 0;				// 모듈 상태
			comData[port - 1][index + 17] = rcvData[37] & 0x7F;
			comData[port - 1][index + 18] = rcvData[36];
			comData[port - 1][index + 19] = rcvData[35];
		}
		else if( xmitType == 1 ) {
			comData[port - 1][index + 58] = 0;									// 충전 모드 상태
			if( rcvData[25] == 0 )		comData[port - 1][index + 59] = 1;		// 시뮬레이터 AUTO 이면 
			else if( rcvData[25] == 1 ) comData[port - 1][index + 59] = 3;		// 시뮬레이터 Manual FL 이면
			else if( rcvData[25] == 2 )	comData[port - 1][index + 59] = 4;		// 시뮬레이터 Manual EQ 이면
                        
			memset(&comData[port - 1][index + 60], 0x00, 3);
		}
		else if( xmitType == 2 ) {
			comData[port - 1][index + 20] = 0;				// 모듈 실탈장 상태
			comData[port - 1][index + 21] = 0;
			comData[port - 1][index + 22] = 0;
			comData[port - 1][index + 23] = 0;
			useModule[port - 1] = 0;
			for( int i = 0; i < 8; i++ ) {
				if( (rcvData[5 + i] >= 'R') && (rcvData[5 + i] <= 'T') ) {
					comData[port - 1][index + 23] |= 0x01 << i;
					useModule[port - 1]++;
				}
				if( (rcvData[13 + i] >= 'R') && (rcvData[13 + i] <= 'T') ) {
					comData[port - 1][index + 22] |= 0x01 << i;
					useModule[port - 1]++;
				}
				if( i < 2 ) {
					if( (rcvData[21 + i] >= 'R') || (rcvData[21 + i] <= 'T') ) {
						comData[port - 1][index + 21] |= 0x01 << i;
						useModule[port - 1]++;
					}
				}
			}

			memset(&comData[port - 1][index + 24], 0x00, 34);
		}

		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a DongA CRS1800N data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitDongACRS1800N(char *xmitData, char xmitType)
{
	xmitData[0] = 0xC5;			// STX
	xmitData[1] = 0x01;			// Destination Address
	xmitData[2] = 0x00;			// Source Address
	xmitData[3] = 0x01;			// Length
	if( xmitType == 0 )			xmitData[4] = 'R';			// Command
	else if( xmitType == 1 )	xmitData[4] = 'I';			// Command
	else						xmitData[4] = 'M';			// Command
	xmitData[5] = 0xCA;			// ETX
	xmitData[6] = 0;			// checksum
	xmitData[7] = 0;			// CRCL
	
	for( char i = 0; i < 6; i++ )	xmitData[6] += xmitData[i];
	xmitData[6] = 0x100 - xmitData[6];
	
	xmitData[13] = 0xC5;		// receive data stx
	xmitData[14] = 0xCA;		// receive data etx
	
	return 7;
}

/**
  * @brief  This function is a DongA CRS1800N data parsing function.
  * @retval true or false
  */
char ParsingDongACRS1800N(char port, char *rcvData, unsigned char rcvCount, char xmitType)
{
	short index = 6;
	unsigned char checksum = 0;
	short tbuf, tbuf1, tbuf2;

	for( char i = 0; i < rcvCount; i++ )	checksum += rcvData[i];
	if( checksum == 0 ) {
		if( xmitType == 0 ) {
			comData[port - 1][index + 0] = 0;				// 정류기 상태
			comData[port - 1][index + 1] = 0;
			comData[port - 1][index + 2] = 0;				// 정류기 펌웨어 버전
			comData[port - 1][index + 3] = 0;
			tbuf = (rcvData[9] - 0x30) * 100 + (rcvData[10] - 0x30) * 10 + (rcvData[11] - 0x30);
			comData[port - 1][index + 4] = tbuf >> 8;		// AC 전압
			comData[port - 1][index + 5] = tbuf & 0x0FF;
			tbuf = (rcvData[5] - 0x30) * 100 + (rcvData[6] - 0x30) * 10 + (rcvData[7] - 0x30);
			comData[port - 1][index + 6] = tbuf >> 8;		// 출력 전압
			comData[port - 1][index + 7] = tbuf & 0x0FF;
			tbuf1 = (rcvData[18] - 0x30) * 1000 + (rcvData[19] - 0x30) * 100 + (rcvData[20] - 0x30) * 10 + (rcvData[21] - 0x30);
			comData[port - 1][index + 8] = tbuf1 >> 8;		// 출력 전류
			comData[port - 1][index + 9] = tbuf1 & 0x0FF;
			tbuf2 = (rcvData[23] - 0x30) * 1000 + (rcvData[24] - 0x30) * 100 + (rcvData[25] - 0x30) * 10 + (rcvData[26] - 0x30);
			if( rcvData[22] == '-' )	tbuf2 *= -1;
			comData[port - 1][index + 12] = tbuf2 >> 8;		// 축전지 전류
			comData[port - 1][index + 13] = tbuf2 & 0x0FF;
			if( useModule[port - 1] ) 	tbuf = (tbuf1 + tbuf2) / useModule[port - 1];
			comData[port - 1][index + 10] = tbuf >> 8;		// 모듈당 부하 전류
			comData[port - 1][index + 11] = tbuf & 0x0FF;
			comData[port - 1][index + 14] = 0;				// 정류기 경보 상태
			comData[port - 1][index + 15] = 0;
			if( rcvData[34] & 0x01 )	comData[port - 1][index + 15] |= 0x01;
			if( rcvData[34] & 0x04 )	comData[port - 1][index + 15] |= 0x20;
			if( rcvData[34] & 0x08 )	comData[port - 1][index + 15] |= 0x04;
			if( rcvData[34] & 0x10 )	comData[port - 1][index + 15] |= 0x02;
			if( rcvData[34] & 0x40 )	comData[port - 1][index + 15] |= 0x08;
			if( rcvData[38] & 0x20 )	comData[port - 1][index + 16] |= 0x01;
			comData[port - 1][index + 16] = 0;				// 모듈 상태
			comData[port - 1][index + 17] = rcvData[37] & 0x3F;
			comData[port - 1][index + 18] = rcvData[36];
			comData[port - 1][index + 19] = rcvData[35];

			// 충전 발생 일시
			sprintf((char *)&comData[port - 1][index + 26], "%4d%02d%02d%02d%02d%02d", (rcvData[40] << 8) | rcvData[41], rcvData[42], rcvData[43], rcvData[44], rcvData[45], rcvData[46]);
			// 충전 재가동 일시
			sprintf((char *)&comData[port - 1][index + 40], "%4d%02d%02d%02d%02d%02d", (rcvData[47] << 8) | rcvData[48], rcvData[49], rcvData[50], rcvData[51], rcvData[52], rcvData[53]);
			// 이상 감지 전류
			comData[port - 1][index + 54] = 0;
			comData[port - 1][index + 55] = rcvData[54];
		}
		else if( xmitType == 1 ) {
			comData[port - 1][index + 24] = 0;				// 충전 말기 전류
			comData[port - 1][index + 25] = rcvData[30];
			comData[port - 1][index + 56] = 0;				// 충전 감시모드 사용상태
			comData[port - 1][index + 57] = rcvData[31];
			comData[port - 1][index + 58] = 0;									// 충전모드
			if( rcvData[25] == 0 )		comData[port - 1][index + 59] = 1;		// 시뮬레이터 AUTO 이면 
			else if( rcvData[25] == 1 ) comData[port - 1][index + 59] = 3;		// 시뮬레이터 Manual FL 이면
			else if( rcvData[25] == 2 )	comData[port - 1][index + 59] = 4;		// 시뮬레이터 Manual EQ 이면
		}
		else if( xmitType == 2 ) {
			comData[port - 1][index + 20] = 0;				// 모듈 실탈장 상태
			comData[port - 1][index + 21] = 0;
			comData[port - 1][index + 22] = 0;
			comData[port - 1][index + 23] = 0;
			useModule[port - 1] = 0;
			for( int i = 0; i < 8; i++ ) {
				if( (rcvData[5 + i] >= 'R') && (rcvData[5 + i] <= 'T') ) {
					comData[port - 1][index + 23] |= 0x01 << i;
					useModule[port - 1]++;
				}
				if( (rcvData[13 + i] >= 'R') || (rcvData[13 + i] <= 'T') ) {
					comData[port - 1][index + 22] |= 0x01 << i;
					useModule[port - 1]++;
				}
				if( i < 2 ) {
					if( (rcvData[21 + i] >= 'R') || (rcvData[21 + i] <= 'T') ) {
						comData[port - 1][index + 21] |= 0x01 << i;
						useModule[port - 1]++;
					}
				}
			}
			comData[port - 1][index + 60] = 0;		// 전압제어수행중
			comData[port - 1][index + 61] = 0;
		}

		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a DongA MR data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitDongAMR(char *xmitData, char xmitType)
{
	unsigned char checksum;
	
	xmitData[0] = 0xC5;				// STX
	xmitData[1] = 5;			// Length
	if( (xmitType == 0) || (xmitType == 1) ) {		// Real-time Total Data / Rectifier Unit
		xmitData[2] = 'r';			// Request to PC
		xmitData[3] = 'T';			// Real-time Data
		if( xmitType == 0 )
			xmitData[4] = 'T';		// Total Data
		else	xmitData[4] = 'R';	// Rectifier Unit
		xmitData[5] = 'X';			// Request to PC
	}
	else if( (xmitType == 2) || (xmitType == 3) ) {
		xmitData[2] = 'i';			// Request to PC
		xmitData[3] = 'O';			// Operation Data
		if( xmitType == 2 ) {
			xmitData[4] = 'U';		// Rectifier Module Use
			xmitData[5] = 'S';
		}
		else {
			xmitData[4] = 'C';		// Rectifier Module Use
			xmitData[5] = 'M';
		}
	}
	xmitData[6] = '/';			// Message End Code
	xmitData[7] = 0xCA;			// ETX
	checksum = xmitData[2] + xmitData[3] + xmitData[4] + xmitData[5] + xmitData[6];
	xmitData[8] = checksum;		// checksum
	
	xmitData[13] = 0xC5;		// receive data stx
	xmitData[14] = 0xCA;		// receive data etx
	
	return 9;
}

/**
  * @brief  This function is a DongA MR data parsing function.
  * @retval true or false
  */
char ParsingDongAMR(char port, char *rcvData, unsigned char rcvCount, char xmitType)
{
	short index = 6;
	unsigned char checksum = 0;
	short tbuf;

	for( char i = 2; i < (rcvCount - 2); i++ )	checksum += rcvData[i];
	if( checksum == rcvData[rcvCount - 1] ) {
		if( xmitType == 0 ) {
			comData[port - 1][index + 0] = 0;				// 정류기 상태
			comData[port - 1][index + 1] = 0;
			comData[port - 1][index + 2] = 0;				// 정류기 펌웨어 버전
			comData[port - 1][index + 3] = 0;
			tbuf = (rcvData[20] - 0x30) * 100 + (rcvData[21] - 0x30) * 10 + (rcvData[22] - 0x30);
			comData[port - 1][index + 4] = tbuf >> 8;		// AC 전압
			comData[port - 1][index + 5] = tbuf & 0x0FF;
			tbuf = (rcvData[41] - 0x30) * 100 + (rcvData[42] - 0x30) * 10 + (rcvData[43] - 0x30);
			comData[port - 1][index + 6] = tbuf >> 8;		// 출력 전압
			comData[port - 1][index + 7] = tbuf & 0x0FF;
			tbuf = (rcvData[44] - 0x30) * 1000 + (rcvData[45] - 0x30) * 100 + (rcvData[46] - 0x30) * 10 + (rcvData[47] - 0x30);
			comData[port - 1][index + 8] = tbuf >> 8;		// 출력 전류
			comData[port - 1][index + 9] = tbuf & 0x0FF;
			tbuf = (rcvData[49] - 0x30) * 1000 + (rcvData[50] - 0x30) * 100 + (rcvData[51] - 0x30) * 10 + (rcvData[52] - 0x30);
			if( rcvData[48] == '-' )	tbuf *= -1;
			comData[port - 1][index + 12] = tbuf >> 8;		// 축전지 전류
			comData[port - 1][index + 13] = tbuf & 0x0FF;
			comData[port - 1][index + 14] = 0;				// 정류기 경보 상태
			comData[port - 1][index + 15] = 0;
			if( rcvData[7] )	comData[port - 1][index + 15] |= 0x01;
		}
		else if( xmitType == 1 ) {
			tbuf = (rcvData[5] - 0x30) * 10 + (rcvData[6] - 0x30);
			unsigned short moduleStatus = 0, loadCurrent = 0;
			useModule[port - 1] = 0;
			for( short i = 0; i < tbuf; i++ ) {
				if( (rcvData[13 + (i * 7)] & 0x80) == 0x00 ) {
					useModule[port - 1]++;
					if( rcvData[13 + (i * 7)] & 0x40 )	moduleStatus |= (0x01 << i);
					loadCurrent += (rcvData[10 + (i * 7)] - 0x30) * 100 + (rcvData[11 + (i * 7)] - 0x30) * 10 + (rcvData[12 + (i * 7)] - 0x30);
				}
			}
			tbuf = loadCurrent / useModule[port - 1];
			comData[port - 1][index + 10] = tbuf >> 8;		// 모듈당 부하 전류
			comData[port - 1][index + 11] = tbuf & 0x0FF;

			comData[port - 1][index + 16] = (unsigned char)(moduleStatus >> 24) & 0x0FF;	// 모듈 상태
			comData[port - 1][index + 17] = (unsigned char)(moduleStatus >> 16) & 0x0FF;
			comData[port - 1][index + 18] = (unsigned char)(moduleStatus >> 8) & 0x0FF;
			comData[port - 1][index + 19] = (unsigned char)(moduleStatus & 0x0FF);
		}
		else if( xmitType == 2 ) {
			unsigned int module_use = 0;
			useModule[port - 1] = 0;
			for( short i = 0; i < (rcvData[1] - 4); i++ ) {
				if( rcvData[5 + i] == 'O' ) {
					module_use |= (0x01 << i);
//					useModule[port - 1]++;
				}
			}
			comData[port - 1][index + 20] = (unsigned char)(module_use >> 24) & 0x0FF;	// 모듈 실탈장 상태
			comData[port - 1][index + 21] = (unsigned char)(module_use >> 16) & 0x0FF;
			comData[port - 1][index + 22] = (unsigned char)(module_use >> 8) & 0x0FF;
			comData[port - 1][index + 23] = (unsigned char)(module_use & 0x0FF);
		}
		else {
			memset(&comData[port - 1][index + 24], 0x00, 34);
			
			comData[port - 1][index + 58] = 0;		// 충전모드
			comData[port - 1][index + 59] = rcvData[5] + 1;
			comData[port - 1][index + 60] = 0;		// 전압제어수행중
			comData[port - 1][index + 61] = 0;
		}
		
		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a MBRS1200R data transmission function.
  * @retval Transmission Number of Byte
  */
unsigned char XmitMBRS1200R(char *xmitData, char xmitType)
{
	xmitData[0] = 0xC5;			// STX
	xmitData[1] = 0x01;			// Destination Address
	xmitData[2] = 0x00;			// Source Address
	xmitData[3] = 0x01;			// Length
	if( xmitType == 0 )	xmitData[4] = 'O';			// Command
	else				xmitData[4] = 'R';			// Command
	xmitData[5] = 0xCA;			// ETX
	xmitData[6] = 0;			// checksum
	xmitData[7] = 0;			// CRCL
	
	for( char i = 0; i < 6; i++ )	xmitData[6] += xmitData[i];
	xmitData[6] = 0x100 - xmitData[6];
	
	xmitData[13] = 0xC5;		// receive data stx
	xmitData[14] = 0xCA;		// receive data etx
	
	return 7;
}

/**
  * @brief  This function is a MBRS1200R data parsing function.
  * @retval true or false
  */
char ParsingMBRS1200R(char port, char *rcvData, unsigned char rcvCount, char xmitType)
{
	unsigned short index = 6;
	unsigned char checksum = 0;
	float tbuf, tbuf1, tbuf2;

	for( char i = 0; i < rcvCount; i++ )	checksum += rcvData[i];
	if( checksum == 0 ) {
		if( xmitType == 0 ) {
			comData[port - 1][index + 20] = 0;				// 모듈 상태
			comData[port - 1][index + 21] = 0;
			comData[port - 1][index + 22] = 0;
			comData[port - 1][index + 23] = 0;
			useModule[port - 1] = 0;
			for( int i = 0; i < 8; i++ ) {
				if( rcvData[5 + i] == 'O' )		comData[port - 1][index + 23] |= 0x01 << i;
				else	useModule[port - 1]++;
				if( rcvData[13 + i] == 'O' )	comData[port - 1][index + 22] |= 0x01 << i;
				else	useModule[port - 1]++;
				if( rcvData[21 + i] == 'O' )	comData[port - 1][index + 21] |= 0x01 << i;
				else	useModule[port - 1]++;
			}
		}
		else if( xmitType == 1 ) {
			comData[port - 1][index + 0] = 0;				// 정류기 상태
			comData[port - 1][index + 1] = 0;
			comData[port - 1][index + 2] = 0;				// 정류기 펌웨어 버전
			comData[port - 1][index + 3] = 0;
			comData[port - 1][index + 4] = 0;				// AC 전압
			comData[port - 1][index + 5] = 0;
			tbuf = (1.002 * (174. + (float)rcvData[6])) / 10;
			if( tbuf < 17.5 )	tbuf = 0.0;
			comData[port - 1][index + 6] = (short)(tbuf * 100.) >> 8;		// 출력 전압
			comData[port - 1][index + 7] = (short)(tbuf * 100.) & 0x0FF;
			tbuf1 = (float)rcvData[7] * 10.241 / 2;
			comData[port - 1][index + 8] = (short)(tbuf1 * 10.) >> 8;		// 출력 전류
			comData[port - 1][index + 9] = (short)(tbuf1 * 10.) & 0x0FF;
			tbuf2 = (float)rcvData[8] * 10.241 / 2;
			comData[port - 1][index + 12] = (short)(tbuf2 * 10.) >> 8;		// 축전지 전류
			comData[port - 1][index + 13] = (short)(tbuf2 * 10.) & 0x0FF;
			comData[port - 1][index + 10] = 0;				// 모듈당 부하 전류
			comData[port - 1][index + 11] = 0;
			if( useModule[port - 1] ) {
				tbuf = (short)((tbuf1 + tbuf2) * 10.) / useModule[port - 1];
				comData[port - 1][index + 10] = (short)tbuf >> 8;		// 모듈당 부하 전류
				comData[port - 1][index + 11] = (short)tbuf & 0x0FF;
			}
			comData[port - 1][index + 14] = 0;				// 정류기 경보 상태
			comData[port - 1][index + 15] = 0;
			if( rcvData[5] == 'A' )		comData[port - 1][index + 15] |= 0x01;
			for( char i = 0; i < 16; i++ ) {
				if( rcvData[12] == ('A' + i) ) {
					comData[port - 1][index + 15] |= (i << 1);
					break;
				}
			}
/*
			comData[port - 1][index + 16] = 0;				// 모듈 실탈장 상태
			comData[port - 1][index + 17] = 0;
			comData[port - 1][index + 18] = 0;
			comData[port - 1][index + 19] = 0;
			for( char i = 0; i < 16; i++ ) {
				if( rcvData[9] == ('A' + i) ) {
					comData[port - 1][index + 19] |= i;
					break;
				}
			}
			for( char i = 0; i < 16; i++ ) {
				if( rcvData[10] == ('A' + i) ) {
					comData[port - 1][index + 19] |= (i << 4);
					break;
				}
			}
			for( char i = 0; i < 16; i++ ) {
				if( rcvData[11] == ('A' + i) ) {
					comData[port - 1][index + 18] |= i;
					break;
				}
			}
*/			
			memset(&comData[port - 1][index + 24], 0x00, 38);
			
			if( rcvData[5] == 'B' )			comData[port - 1][index + 58] = 2;
			else if( rcvData[5] == 'C' )	comData[port - 1][index + 58] = 1;
			else if( rcvData[5] == 'D' )	comData[port - 1][index + 58] = 4;
			else if( rcvData[5] == 'E' )	comData[port - 1][index + 58] = 3;
		}
		
		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a SDPS-48N data parsing function.
  * @retval true or false
  */
char ParsingSDPS48N(char port, char *rcvData, unsigned char rcvCount, char xmitType)
{
	unsigned short index = 6;
	unsigned char checksum = 0;
	short tbuf, tbuf1, tbuf2;

	for( char i = 1; i < (rcvCount - 2); i++ )	checksum += rcvData[i];
	if( checksum == rcvData[rcvCount - 1] ) {
		if( rcvData[1] == 'R' ) {
			comData[port - 1][index + 0] = 0;				// 정류기 상태
			comData[port - 1][index + 1] = 0;
			comData[port - 1][index + 2] = rcvData[92];		// 정류기 펌웨어 버전
			comData[port - 1][index + 3] = rcvData[93];
			tbuf = (rcvData[2] - 0x30) * 100 + (rcvData[3] - 0x30) * 10 + (rcvData[4] - 0x30);
			comData[port - 1][index + 4] = tbuf >> 8;		// AC 전압
			comData[port - 1][index + 5] = tbuf & 0x0FF;
			tbuf = (rcvData[11] - 0x30) * 100 + (rcvData[12] - 0x30) * 10 + (rcvData[13] - 0x30);
			comData[port - 1][index + 6] = tbuf >> 8;		// 출력 전압
			comData[port - 1][index + 7] = tbuf & 0x0FF;
			tbuf1 = (rcvData[14] - 0x30) * 100 + (rcvData[15] - 0x30) * 10 + (rcvData[16] - 0x30);
			comData[port - 1][index + 8] = tbuf1 >> 8;		// 출력 전류
			comData[port - 1][index + 9] = tbuf1 & 0x0FF;
			tbuf2 = (rcvData[18] - 0x30) * 100 + (rcvData[19] - 0x30) * 10 + (rcvData[20] - 0x30);
			if( rcvData[17] == '-' )	tbuf2 = (-1) * tbuf2;
			comData[port - 1][index + 12] = tbuf2 >> 8;		// 축전지 전류
			comData[port - 1][index + 13] = tbuf2 & 0x0FF;
			tbuf = (rcvData[88] - 0x30) * 10 + (rcvData[89] - 0x30);
			tbuf = (tbuf1 + tbuf2) / tbuf;
			comData[port - 1][index + 10] = tbuf >> 8;		// 모듈당 부하 전류
			comData[port - 1][index + 11] = tbuf & 0x0FF;
			comData[port - 1][index + 58] = 0;				// 충전모드
			comData[port - 1][index + 59] = (rcvData[66] - 0x30) + 5;
		}
		else if( rcvData[1] == 'K' ) {
			comData[port - 1][index + 14] = 0;				// 정류기 경보 상태
			comData[port - 1][index + 15] = 0;
			comData[port - 1][index + 15] = rcvData[2] & 0x01;
			comData[port - 1][index + 15] |= (rcvData[2] >> 1) & 0x06;
			comData[port - 1][index + 14] = (rcvData[3] << 2) & 0x1C;
			comData[port - 1][index + 16] = 0;				// 모듈 상태
			comData[port - 1][index + 17] = 0;
			comData[port - 1][index + 18] = rcvData[8] & 0x03;
			comData[port - 1][index + 19] = (rcvData[5] << 4) | (rcvData[4] & 0x0F);
			comData[port - 1][index + 20] = 0;				// 모듈 실탈장 상태
			comData[port - 1][index + 21] = 0;
			comData[port - 1][index + 22] = (~rcvData[8] >> 2) & 0x03;
			comData[port - 1][index + 23] = (~rcvData[7] << 4) | (~rcvData[6] & 0x0F);
		}
		
		return 1;
	}
	else	return 0;
}

/**
  * @brief  This function is a LORS-4150 data transmission function.
  * @retval Transmission Number of Byte
  */
/*
unsigned char XmitLORS4150(char *xmitData, char xmitType)
{
	xmitData[0] = 0xC5;			// STX
	xmitData[1] = 0x20;			// Destination Address
	xmitData[2] = 0x00;			// Source Address
	xmitData[3] = 0x01;			// Length
	if( xmitType == 0 )			xmitData[4] = 0x07;			// Command
	else						xmitData[4] = 'M';			// Command
	xmitData[5] = 0xCA;			// ETX
	
	crc_check(6, &xmitData[0]);
	xmitData[6] = crcHigh;		// CRCH
	xmitData[7] = crcLow;		// CRCL
	
	xmitData[13] = 0xC5;		// receive data stx
	xmitData[14] = 0xCA;		// receive data etx
	
	return 8;
}
*/
/**
  * @brief  This function is a LORS-4150 data parsing function.
  * @retval true or false
  */
/*
char ParsingLORS4150(char port, char *rcvData, unsigned char rcvCount, char xmitType)
{
	short index = 6;
	unsigned char checksum = 0;
	short tbuf, tbuf1, tbuf2;

	for( char i = 0; i < rcvCount; i++ )	checksum += rcvData[i];
	if( checksum == 0 ) {
		if( xmitType == 0 ) {
			comData[port - 1][index + 0] = 0;				// 정류기 상태
			comData[port - 1][index + 1] = 0;
			comData[port - 1][index + 2] = 0;				// 정류기 펌웨어 버전
			comData[port - 1][index + 3] = 0;
			tbuf = (rcvData[9] - 0x30) * 100 + (rcvData[10] - 0x30) * 10 + (rcvData[11] - 0x30);
			comData[port - 1][index + 4] = tbuf >> 8;		// AC 전압
			comData[port - 1][index + 5] = tbuf & 0x0FF;
			tbuf = (rcvData[5] - 0x30) * 100 + (rcvData[6] - 0x30) * 10 + (rcvData[7] - 0x30);
			comData[port - 1][index + 6] = tbuf >> 8;		// 출력 전압
			comData[port - 1][index + 7] = tbuf & 0x0FF;
			tbuf1 = (rcvData[18] - 0x30) * 1000 + (rcvData[19] - 0x30) * 100 + (rcvData[20] - 0x30) * 10 + (rcvData[21] - 0x30);
			comData[port - 1][index + 8] = tbuf1 >> 8;		// 출력 전류
			comData[port - 1][index + 9] = tbuf1 & 0x0FF;
			tbuf2 = (rcvData[23] - 0x30) * 1000 + (rcvData[24] - 0x30) * 100 + (rcvData[25] - 0x30) * 10 + (rcvData[26] - 0x30);
			if( rcvData[22] == '-' )	tbuf2 *= -1;
			comData[port - 1][index + 12] = tbuf2 >> 8;		// 축전지 전류
			comData[port - 1][index + 13] = tbuf2 & 0x0FF;
			tbuf = (tbuf1 + tbuf2) / rcvData[38];
			comData[port - 1][index + 10] = tbuf >> 8;		// 모듈당 부하 전류
			comData[port - 1][index + 11] = tbuf & 0x0FF;
			comData[port - 1][index + 14] = 0;				// 정류기 경보 상태
			comData[port - 1][index + 15] = 0;
			if( rcvData[34] & 0x01 )	comData[port - 1][index + 15] |= 0x01;
			if( rcvData[34] & 0x04 )	comData[port - 1][index + 15] |= 0x20;
			if( rcvData[34] & 0x08 )	comData[port - 1][index + 15] |= 0x04;
			if( rcvData[34] & 0x10 )	comData[port - 1][index + 15] |= 0x02;
			if( rcvData[34] & 0x40 )	comData[port - 1][index + 15] |= 0x08;
			if( rcvData[38] & 0x20 )	comData[port - 1][index + 16] |= 0x01;
			comData[port - 1][index + 16] = 0;				// 모듈 상태
			comData[port - 1][index + 17] = rcvData[37] & 0x3F;
			comData[port - 1][index + 18] = rcvData[36];
			comData[port - 1][index + 19] = rcvData[35];

			// 충전 발생 일시
			sprintf((char *)&comData[port - 1][index + 26], "%4d%02d%02d%02d%02d%02d", (rcvData[40] << 8) | rcvData[41], rcvData[42], rcvData[43], rcvData[44], rcvData[45], rcvData[46]);
			// 충전 재가동 일시
			sprintf((char *)&comData[port - 1][index + 40], "%4d%02d%02d%02d%02d%02d", (rcvData[47] << 8) | rcvData[48], rcvData[49], rcvData[50], rcvData[51], rcvData[52], rcvData[53]);
			// 이상 감지 전류
			comData[port - 1][index + 54] = 0;
			comData[port - 1][index + 55] = rcvData[54];
		}
		else if( xmitType == 1 ) {
			comData[port - 1][index + 24] = 0;				// 충전 말기 전류
			comData[port - 1][index + 25] = rcvData[30];
			comData[port - 1][index + 56] = 0;				// 충전 감시모드 사용상태
			comData[port - 1][index + 57] = rcvData[31];
		}
		else if( xmitType == 2 ) {
			comData[port - 1][index + 20] = 0;				// 모듈 실탈장 상태
			comData[port - 1][index + 21] = 0;
			comData[port - 1][index + 22] = 0;
			comData[port - 1][index + 23] = 0;
			for( int i = 0; i < 8; i++ ) {
				if( (rcvData[5 + i] < 'R') || (rcvData[5 + i] > 'T') )		comData[port - 1][index + 23] |= 0x01 << i;
				if( (rcvData[13 + i] < 'R') || (rcvData[13 + i] > 'T') )	comData[port - 1][index + 22] |= 0x01 << i;
				if( i < 2 ) {
					if( (rcvData[21 + i] < 'R') || (rcvData[21 + i] > 'T') )	comData[port - 1][index + 21] |= 0x01 << i;
				}
			}
			comData[port - 1][index + 58] = 0;		// 충전모드
			comData[port - 1][index + 59] = 0;
			comData[port - 1][index + 60] = 0;		// 전압제어수행중
			comData[port - 1][index + 61] = 0;
		}

		return 1;
	}
	else	return 0;
}
*/
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
