/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>

#include "board.h"
#include "etc.h"
#include "nt_queue.h"
#include "usart.h"
	  
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
enum {
	XMIT_MODE = 1,
	RECEIVE_MODE,
	NEXT_MODE
};

#define XMIT_TIME		500

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
unsigned short xmitTime[3] = { 0, };
unsigned short recvTime[3] = { 0, }, mainRecvTime = 0;
char setupPortDevice = 0;

extern BoardInfo myInfo;
extern Queue receiveQueue[3], mainReceiveQueue;
extern unsigned char receiveBuffer[3][2], mainReceiveBuffer[2];

extern unsigned char slotID;
extern unsigned char replyInfoData[32];
extern unsigned char replyDetailedData[219];
extern unsigned char replyData[16];
extern unsigned char comData[3][69];
extern short replyInfoLength, replyDetailedLength;

/* USER CODE END Variables */

osThreadId mainTaskHandle;
osThreadId Com1TaskHandle;
osThreadId Com2TaskHandle;
osThreadId Com3TaskHandle;
osThreadId debugTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartMainTask(void const * argument);
void StartCom1Task(void const * argument);
void StartCom2Task(void const * argument);
void StartCom3Task(void const * argument);
void StartDebugTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
	/* USER CODE BEGIN Init */
	HAL_Delay(500);
	ReadBoardID();
	InitializeBoardInfo();
	InitializeData();

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the thread(s) */
	/* definition and creation of mainTask */
//	osThreadDef(mainTask, StartMainTask, osPriorityAboveNormal, 0, 512);
//	mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);

	/* definition and creation of Com1Task */
//	osThreadDef(Com1Task, StartCom1Task, osPriorityNormal, 0, 256);
//	Com1TaskHandle = osThreadCreate(osThread(Com1Task), NULL);

	/* definition and creation of Com2Task */
//	osThreadDef(Com2Task, StartCom2Task, osPriorityNormal, 0, 256);
//	Com2TaskHandle = osThreadCreate(osThread(Com2Task), NULL);

	/* definition and creation of Com3Task */
//	osThreadDef(Com3Task, StartCom3Task, osPriorityNormal, 0, 256);
//	Com3TaskHandle = osThreadCreate(osThread(Com3Task), NULL);

	/* definition and creation of debugTask */
	osThreadDef(debugTask, StartDebugTask, osPriorityBelowNormal, 0, 256);
	debugTaskHandle = osThreadCreate(osThread(debugTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the mainTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void const *argument)
{
	/* USER CODE BEGIN StartMainTask */
	char parsingFlag = 0, headerCheck = 0;
	char rcvData[100], parsingData[100], compareData[4];
	unsigned int rcvCount = 0, rcvLength = 0;

	InitQueue(&mainReceiveQueue);
	HAL_UART_Receive_IT(&huart7, (uint8_t *)&mainReceiveBuffer, 1);

	headerCheck = NOT_OK;
	
	sprintf((char *)&compareData, "M%cT", slotID + 0x30);
	rcvCount = 0;

	/* Infinite loop */
	while( true ) {
		if( !IsEmpty(&mainReceiveQueue) ) {
			rcvData[rcvCount++] = Dequeue(&mainReceiveQueue);

			if( headerCheck == NOT_OK ) {
				if( rcvCount == 3 ) {
					rcvData[rcvCount] = 0;
					if( memcmp((char *)&rcvData, (char *)&compareData, 3) == 0 ) {
						headerCheck = OK;
					}
					else {
						rcvData[0] = rcvData[1];
						rcvData[1] = rcvData[2];
						rcvCount = 2;
					}
				}
			}
			else {
				if( rcvCount <= 6 ) {
					if( rcvCount == 6 )	rcvLength = (rcvData[4] << 8) | rcvData[5];
				}
				else {
					if( rcvCount == (rcvLength + 6) ) {
						memcpy(&parsingData[0], &rcvData[0], rcvLength + 6);
						parsingFlag = 1;
						headerCheck = NOT_OK;
					}
				}
			}
		}

		if( parsingFlag ) {
			switch( parsingData[3] ) {
				case REQUEST_INFO :
					TransmissionInfoDataToMain();
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyInfoData, replyInfoLength, 1000);
					break;
					
				case REQUEST_DATA :
					TransmissionDetailedDataToMain();
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyDetailedData, replyDetailedLength, 1000);
					break;

				case SET_INFO :
					SetBoardData(&parsingData[0]);
					replyData[3] = SET_INFO;
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyData, 15, 1000);
					setupPortDevice = 1;
					break;

				case CONTROL_DATA :
					replyData[3] = CONTROL_DATA;
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyData, 15, 1000);
					break;

				case DOWNLOAD_READY :
					break;

				case DOWNLOAD_START :
					break;

				case DOWNLOAD_STOP :
					break;
			}
			rcvCount = 0;
			parsingFlag = 0;
		}

		osDelay(1);
	}
	/* USER CODE END StartMainTask */
}

/* USER CODE BEGIN Header_StartCom1Task */
/**
* @brief Function implementing the Com1Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCom1Task */
void StartCom1Task(void const * argument)
{
	/* USER CODE BEGIN StartCom1Task */
	char rcvData[300], xmitData[15] = { 0, }, ret;
	unsigned char xmitNob, stx, etx, xmitType = 0;
	unsigned char checkSTX = NOT_OK, checkETX = NOT_OK;
	unsigned short rcvCount, mode, commError = 0;

	InitQueue(&receiveQueue[0]);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&receiveBuffer[0], 1);

	mode = XMIT_MODE;
	
	/* Infinite loop */
	while( true ) {
		if( mode == XMIT_MODE ) {
			switch( myInfo.portInfo[0].name ) {
				case DONGA_CRS1800 :
				case ANTS_CRS1800 :
					xmitType = xmitType ^ 0x01;
					xmitNob = XmitDongACRS1800(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case DONGA_CRS1800N :
					xmitType = (xmitType + 1) % 3;
					xmitNob = XmitDongACRS1800N(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case DONGA_MR :
					xmitType = (xmitType + 1) % 4;
					xmitNob = XmitDongAMR(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case MBRS_1200R :
				case MBRS_1200N :
				case MBRS_1200 :
					xmitType = xmitType ^ 0x01;
					xmitNob = XmitMBRS1200R(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case EMERSON :
					break;
						
				case CRS24_150 :
					break;
						
				case SDPS_48N :
					// 500ms마다 정류기로부터 데이타 수신
					xmitNob = 0;
					stx = 0x02;
					etx = 0x03;
					break;
					
				case SWPS_4150W2 :
					break;
						
				case LORS_4150 :
					break;
					
				case LORS_460 :
					break;
					
				case NORS_4150 :
					break;
					
				case NORS_460 :
					break;
			}
			
			if( xmitNob )	HAL_UART_Transmit(&huart2, (uint8_t *)&xmitData, xmitNob, 100);
			
			checkSTX = NOT_OK;
			checkETX = NOT_OK;
			rcvCount = 0;
			mode = RECEIVE_MODE;
			
			xmitTime[0] = 0;			
		}
		else if( mode == RECEIVE_MODE ) {
			if( !IsEmpty(&receiveQueue[0]) ) {
				if( checkSTX == NOT_OK ) {
					rcvData[0] = Dequeue(&receiveQueue[0]);
					if( rcvData[0] == stx ) {
						checkSTX = OK;
						rcvCount = 1;
					}
				}
				else {
					rcvData[rcvCount++] = Dequeue(&receiveQueue[0]);
					if( rcvCount < 300 ) {
						if( checkETX == NOT_OK ) {
							if( rcvData[rcvCount - 1] == etx )		checkETX = OK;
						}
						else {
							switch( myInfo.portInfo[0].name ) {
								case DONGA_CRS1800 :
								case ANTS_CRS1800 :
									rcvData[rcvCount] = 0;
									ret = ParsingDongACRS1800(COM1, rcvData, rcvCount, xmitType);
									break;
									
								case DONGA_CRS1800N :
									rcvData[rcvCount] = 0;
									ret = ParsingDongACRS1800N(COM1, rcvData, rcvCount, xmitType);
									break;
									
								case DONGA_MR :
									rcvData[rcvCount] = 0;
									ret = ParsingDongAMR(COM1, rcvData, rcvCount, xmitType);
									break;
									
								case MBRS_1200R :
								case MBRS_1200N :
								case MBRS_1200 :
									rcvData[rcvCount] = 0;
									ret = ParsingMBRS1200R(COM1, rcvData, rcvCount, xmitType);
									break;
									
								case EMERSON :
									break;
										
								case CRS24_150 :
									break;
						
								case SDPS_48N :
									rcvData[rcvCount] = 0;
									ret = ParsingSDPS48N(COM1, rcvData, rcvCount, xmitType);
									break;
						
								case SWPS_4150W2 :
									break;
						
								case LORS_4150 :
									break;
									
								case LORS_460 :
									break;
									
								case NORS_4150 :
									break;
									
								case NORS_460 :
									break;
							}

							if( ret == 1 )	commError = 0;
							else {
								if( commError < 10 )	commError = commError + 1;
							}

							mode = NEXT_MODE;
						}
					}
					else {
						if( commError < 10 )	commError = commError + 1;
						mode = NEXT_MODE;
					}
				}
			}
		}
		
		// COmmunication Status Check
		if( xmitTime[0] >= XMIT_TIME ) {
			if( mode != XMIT_MODE ) {
				if( mode == RECEIVE_MODE ) {
					if( commError < 10 )	commError = commError + 1;
				}

				mode = XMIT_MODE;
				xmitTime[0] = 0;
			}
		}
		
		if( commError == 10 ) {
			memset(&comData[0][6], 0x00, myInfo.portInfo[0].dataLength);
			comData[0][6] = 0;
			comData[0][7] = 1;
		}

		osDelay(1);
	}
	/* USER CODE END StartCom1Task */
}

/* USER CODE BEGIN Header_StartCom2Task */
/**
* @brief Function implementing the Com2Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCom2Task */
void StartCom2Task(void const * argument)
{
	/* USER CODE BEGIN StartCom2Task */
	char rcvData[300], xmitData[15] = { 0, }, ret;
	unsigned char xmitNob, stx, etx, xmitType = 0;
	unsigned char checkSTX = NOT_OK, checkETX = NOT_OK;
	unsigned short rcvCount, mode, commError = 0;

	InitQueue(&receiveQueue[1]);
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&receiveBuffer[1], 1);

	mode = XMIT_MODE;
	
	/* Infinite loop */
	while( true ) {
		if( mode == XMIT_MODE ) {
			switch( myInfo.portInfo[1].name ) {
				case DONGA_CRS1800 :
				case ANTS_CRS1800 :
					xmitNob = XmitDongACRS1800(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case DONGA_CRS1800N :
					xmitNob = XmitDongACRS1800N(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case DONGA_MR :
					xmitNob = XmitDongAMR(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case MBRS_1200R :
				case MBRS_1200N :
				case MBRS_1200 :
					xmitNob = XmitMBRS1200R(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case EMERSON :
					break;
						
				case CRS24_150 :
					break;
						
				case SDPS_48N :
					// 500ms마다 정류기로부터 데이타 수신
					xmitNob = 0;
					stx = 0x02;
					etx = 0x03;
					break;
					
				case SWPS_4150W2 :
					break;
						
				case LORS_4150 :
					break;
					
				case LORS_460 :
					break;
					
				case NORS_4150 :
					break;
					
				case NORS_460 :
					break;
			}
			
			if( xmitNob )	HAL_UART_Transmit(&huart3, (uint8_t *)&xmitData, xmitNob, 100);
			
			checkSTX = NOT_OK;
			checkETX = NOT_OK;
			rcvCount = 0;
			mode = RECEIVE_MODE;
			
			xmitTime[1] = 0;			
		}
		else if( mode == RECEIVE_MODE ) {
			if( !IsEmpty(&receiveQueue[1]) ) {
				if( checkSTX == NOT_OK ) {
					rcvData[0] = Dequeue(&receiveQueue[1]);
					if( rcvData[0] == stx ) {
						checkSTX = OK;
						rcvCount = 1;
					}
				}
				else {
					rcvData[rcvCount++] = Dequeue(&receiveQueue[1]);
					if( rcvCount < 300 ) {
						if( checkETX == NOT_OK ) {
							if( rcvData[rcvCount - 1] == etx )		checkETX = OK;
						}
						else {
							switch( myInfo.portInfo[1].name ) {
								case DONGA_CRS1800 :
								case ANTS_CRS1800 :
									rcvData[rcvCount] = 0;
									ret = ParsingDongACRS1800(COM2, rcvData, rcvCount, xmitType);
									xmitType = xmitType ^ 0x01;
									break;
									
								case DONGA_CRS1800N :
									rcvData[rcvCount] = 0;
									ret = ParsingDongACRS1800N(COM2, rcvData, rcvCount, xmitType);
									xmitType = (xmitType + 1) % 3;
									break;
									
								case DONGA_MR :
									rcvData[rcvCount] = 0;
									ret = ParsingDongAMR(COM2, rcvData, rcvCount, xmitType);
									xmitType = (xmitType + 1) % 4;
									break;
									
								case MBRS_1200R :
								case MBRS_1200N :
								case MBRS_1200 :
									rcvData[rcvCount] = 0;
									ret = ParsingMBRS1200R(COM2, rcvData, rcvCount, xmitType);
									xmitType = xmitType ^ 0x01;
									break;
									
								case EMERSON :
									break;
										
								case CRS24_150 :
									break;
						
								case SDPS_48N :
									rcvData[rcvCount] = 0;
									ret = ParsingSDPS48N(COM2, rcvData, rcvCount, xmitType);
									break;
						
								case SWPS_4150W2 :
									break;
						
								case LORS_4150 :
									break;
									
								case LORS_460 :
									break;
									
								case NORS_4150 :
									break;
									
								case NORS_460 :
									break;
							}

							if( ret == 1 )	commError = 0;
							else {
								if( commError < 10 )	commError = commError + 1;
							}

							mode = NEXT_MODE;
						}
					}
					else {
						if( commError < 10 )	commError = commError + 1;
						mode = NEXT_MODE;
					}
				}
			}
		}
		
		// COmmunication Status Check
		if( xmitTime[1] >= XMIT_TIME ) {
			if( mode != XMIT_MODE ) {
				if( mode == RECEIVE_MODE ) {
					if( commError < 10 )	commError = commError + 1;
				}

				mode = XMIT_MODE;
				xmitTime[1] = 0;
			}
		}
		
		if( commError == 10 ) {
			memset(&comData[1][6], 0x00, myInfo.portInfo[1].dataLength);
			comData[1][6] = 0;
			comData[1][7] = 1;
		}

		osDelay(1);
	}
	/* USER CODE END StartCom2Task */
}

/* USER CODE BEGIN Header_StartCom3Task */
/**
* @brief Function implementing the Com3Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCom3Task */
void StartCom3Task(void const * argument)
{
	/* USER CODE BEGIN StartCom3Task */
	char rcvData[300], xmitData[15] = { 0, }, ret;
	unsigned char xmitNob, stx, etx, xmitType = 0;
	unsigned char checkSTX = NOT_OK, checkETX = NOT_OK;
	unsigned short rcvCount, mode, commError = 0;

	InitQueue(&receiveQueue[2]);
	HAL_UART_Receive_IT(&huart4, (uint8_t *)&receiveBuffer[2], 1);

	mode = XMIT_MODE;
	
	/* Infinite loop */
	while( true ) {
		if( mode == XMIT_MODE ) {
			switch( myInfo.portInfo[2].name ) {
				case DONGA_CRS1800 :
				case ANTS_CRS1800 :
					xmitNob = XmitDongACRS1800(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case DONGA_CRS1800N :
					xmitNob = XmitDongACRS1800N(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case DONGA_MR :
					xmitNob = XmitDongAMR(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case MBRS_1200R :
				case MBRS_1200N :
				case MBRS_1200 :
					xmitNob = XmitMBRS1200R(xmitData, xmitType);
					stx = xmitData[13];
					etx = xmitData[14];
					break;
					
				case EMERSON :
					break;
						
				case CRS24_150 :
					break;
						
				case SDPS_48N :
					// 500ms마다 정류기로부터 데이타 수신
					xmitNob = 0;
					stx = 0x02;
					etx = 0x03;
					break;
					
				case SWPS_4150W2 :
					break;
						
				case LORS_4150 :
					break;
					
				case LORS_460 :
					break;
					
				case NORS_4150 :
					break;
					
				case NORS_460 :
					break;
			}
			
			if( xmitNob )	HAL_UART_Transmit(&huart4, (uint8_t *)&xmitData, xmitNob, 100);
			
			checkSTX = NOT_OK;
			checkETX = NOT_OK;
			rcvCount = 0;
			mode = RECEIVE_MODE;
			
			xmitTime[2] = 0;			
		}
		else if( mode == RECEIVE_MODE ) {
			if( !IsEmpty(&receiveQueue[2]) ) {
				if( checkSTX == NOT_OK ) {
					rcvData[0] = Dequeue(&receiveQueue[2]);
					if( rcvData[0] == stx ) {
						checkSTX = OK;
						rcvCount = 1;
					}
				}
				else {
					rcvData[rcvCount++] = Dequeue(&receiveQueue[2]);
					if( rcvCount < 300 ) {
						if( checkETX == NOT_OK ) {
							if( rcvData[rcvCount - 1] == etx )		checkETX = OK;
						}
						else {
							switch( myInfo.portInfo[2].name ) {
								case DONGA_CRS1800 :
								case ANTS_CRS1800 :
									rcvData[rcvCount] = 0;
									ret = ParsingDongACRS1800(COM3, rcvData, rcvCount, xmitType);
									xmitType = xmitType ^ 0x01;
									break;
									
								case DONGA_CRS1800N :
									rcvData[rcvCount] = 0;
									ret = ParsingDongACRS1800N(COM3, rcvData, rcvCount, xmitType);
									xmitType = (xmitType + 1) % 3;
									break;
									
								case DONGA_MR :
									rcvData[rcvCount] = 0;
									ret = ParsingDongAMR(COM3, rcvData, rcvCount, xmitType);
									xmitType = (xmitType + 1) % 4;
									break;
									
								case MBRS_1200R :
								case MBRS_1200N :
								case MBRS_1200 :
									rcvData[rcvCount] = 0;
									ret = ParsingMBRS1200R(COM3, rcvData, rcvCount, xmitType);
									xmitType = xmitType ^ 0x01;
									break;
									
								case EMERSON :
									break;
										
								case CRS24_150 :
									break;
						
								case SDPS_48N :
									rcvData[rcvCount] = 0;
									ret = ParsingSDPS48N(COM3, rcvData, rcvCount, xmitType);
									break;
						
								case SWPS_4150W2 :
									break;
						
								case LORS_4150 :
									break;
									
								case LORS_460 :
									break;
									
								case NORS_4150 :
									break;
									
								case NORS_460 :
									break;
							}

							if( ret == 1 )	commError = 0;
							else {
								if( commError < 10 )	commError = commError + 1;
							}

							mode = NEXT_MODE;
						}
					}
					else {
						if( commError < 10 )	commError = commError + 1;
						mode = NEXT_MODE;
					}
				}
			}
		}
		
		// COmmunication Status Check
		if( xmitTime[2] >= XMIT_TIME ) {
			if( mode != XMIT_MODE ) {
				if( mode == RECEIVE_MODE ) {
					if( commError < 10 )	commError = commError + 1;
				}

				mode = XMIT_MODE;
				xmitTime[2] = 0;
			}
		}
		
		if( commError == 10 ) {
			memset(&comData[2][6], 0x00, myInfo.portInfo[2].dataLength);
			comData[2][6] = 0;
			comData[2][7] = 1;
		}

		osDelay(1);
	}
	/* USER CODE END StartCom3Task */
}

/* USER CODE BEGIN Header_StartDebugTask */
/**
* @brief Function implementing the debugTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugTask */
void StartDebugTask(void const * argument)
{
	/* USER CODE BEGIN StartDebugTask */
	osThreadDef(mainTask, StartMainTask, osPriorityAboveNormal, 0, 512);
	mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);

	/* definition and creation of Com1Task */
	osThreadDef(Com1Task, StartCom1Task, osPriorityNormal, 0, 256);
	Com1TaskHandle = osThreadCreate(osThread(Com1Task), NULL);

	/* definition and creation of Com2Task */
	osThreadDef(Com2Task, StartCom2Task, osPriorityNormal, 0, 256);
	Com2TaskHandle = osThreadCreate(osThread(Com2Task), NULL);

	/* definition and creation of Com3Task */
	osThreadDef(Com3Task, StartCom3Task, osPriorityNormal, 0, 256);
	Com3TaskHandle = osThreadCreate(osThread(Com3Task), NULL);
	
	/* Infinite loop */
	while( true ) {
		if( recvTime[0] >= 10000 ) {
			osThreadTerminate(Com1TaskHandle);
			HAL_Delay(100);
			Com1TaskHandle = osThreadCreate(osThread(Com1Task), NULL);
			recvTime[0] = 0;
		}

		if( recvTime[1] >= 10000 ) {
			osThreadTerminate(Com2TaskHandle);
			HAL_Delay(100);
			Com2TaskHandle = osThreadCreate(osThread(Com2Task), NULL);
			recvTime[1] = 0;
		}

		if( recvTime[2] >= 10000 ) {
			osThreadTerminate(Com3TaskHandle);
			HAL_Delay(100);
			Com3TaskHandle = osThreadCreate(osThread(Com3Task), NULL);
			recvTime[2] = 0;
		}
		
		if( mainRecvTime >= 10000 ) {
			osThreadTerminate(mainTaskHandle);
			HAL_Delay(100);
			mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);
			mainRecvTime = 0;
		}

		if( setupPortDevice == 1 ) {
			setupPortDevice = 0;

			osThreadTerminate(Com1TaskHandle);
			osThreadTerminate(Com2TaskHandle);
			osThreadTerminate(Com3TaskHandle);
			HAL_Delay(100);
			Com1TaskHandle = osThreadCreate(osThread(Com1Task), NULL);
			Com2TaskHandle = osThreadCreate(osThread(Com2Task), NULL);
			Com3TaskHandle = osThreadCreate(osThread(Com3Task), NULL);

			recvTime[0] = 0;
			recvTime[1] = 0;
			recvTime[2] = 0;
		}

		osDelay(1);
	}
	/* USER CODE END StartDebugTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
