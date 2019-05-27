
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

#define XMIT_TIME		250

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
unsigned short xmitTime[3] = { 0, };
unsigned short recvTime[3] = { 0, }, mainRecvTime = 0;
char setupPortDevice = 0, commError[3][200];

extern BoardInfo myInfo;
extern Queue receiveQueue[3], mainReceiveQueue;
extern char receiveBuffer[3][2], mainReceiveBuffer[2];

extern char slotID;
extern char replyInfoData[35];
extern char replyDetailedData[220];
extern char replyData[16];
extern unsigned char com1Data[200][70];
extern unsigned char com2Data[200][70];
extern unsigned char com3Data[200][70];
extern unsigned short replyInfoLength, replyDetailedLength;

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
//	osThreadDef(mainTask, StartMainTask, osPriorityNormal, 0, 3072);
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
	osThreadDef(debugTask, StartDebugTask, osPriorityNormal, 0, 128);
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
void StartMainTask(void const * argument)
{
	/* USER CODE BEGIN StartMainTask */
	char parsingFlag = 0, headerCheck;
	unsigned char rcvData[2200], parsingData[2200], compareData[4];
	unsigned int rcvCount = 0, rcvLength;

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
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyInfoData[0], replyInfoLength, 1000);
					break;
					
				case REQUEST_DATA :
					TransmissionDetailedDataToMain();
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyDetailedData[0], replyDetailedLength, 1000);
					break;

				case SET_INFO :
					SetBoardData(&parsingData[0], rcvLength + 6);
					replyData[3] = SET_INFO;
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyData[0], 15, 1000);
					setupPortDevice = 1;
					break;

				case CONTROL_DATA :
					replyData[3] = CONTROL_DATA;
					HAL_UART_Transmit(&huart7, (uint8_t *)&replyData[0], 15, 1000);
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
	char rcvData[100], xmitData[15] = { 0, }, ret;
	unsigned char xmitNob, stx, etx;
	unsigned char checkSTX = NOT_OK, checkETX = NOT_OK;
	unsigned short rcvCount, mode, slaveAddress, slaveCount;

	InitQueue(&receiveQueue[0]);
	HAL_UART_Receive_IT(&huart2, (uint8_t *)&receiveBuffer[0], 1);

	rcvCount = 0;
	checkSTX = NOT_OK;
	checkETX = NOT_OK;
	mode = XMIT_MODE;
	
	slaveAddress = 0;
	slaveCount = myInfo.number_of_device[0] - 1;
	/* Infinite loop */
	while( true ) {
		if( mode == XMIT_MODE ) {
			slaveCount = (slaveCount + 1) % myInfo.number_of_device[0];
			slaveAddress = myInfo.portInfo[0][slaveCount].id;

			switch( myInfo.portInfo[0][slaveCount].name ) {
				case CHANGJO1_AIRCONDITIONER :
				case CHANGJO2_AIRCONDITIONER :
					xmitNob = XmitChangjo(&xmitData[0], slaveAddress);
					break;
					
				case VARIABLE_AIRCONDITIONER :
					xmitNob = XmitVariable(&xmitData[0], slaveAddress);
					break;

				case NATURAL_AIRCONDITIONER :
					xmitNob = XmitNatural(&xmitData[0], slaveAddress);
					break;

				case TEMPERATURE_MODULE :
					xmitNob = XmitTemperature(&xmitData[0], slaveAddress);
					break;
			}
			
			stx = xmitData[13];
			etx = xmitData[14];
			InitQueue(&receiveQueue[0]);
			if( xmitNob )	HAL_UART_Transmit(&huart2, (uint8_t *)&xmitData[0], xmitNob, 100);
			
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
					if( rcvCount < 100 ) {
						if( checkETX == NOT_OK ) {
							if( rcvCount == myInfo.portInfo[0][slaveCount].rcvLength )		checkETX = OK;
						}
						
						if( checkETX == OK ) {
							rcvData[rcvCount] = 0;
							switch( myInfo.portInfo[0][slaveCount].name ) {
								case CHANGJO1_AIRCONDITIONER :
									ret = ParsingChangjo1(COM1, &rcvData[0], rcvCount, slaveCount);
									break;

								case CHANGJO2_AIRCONDITIONER :
									ret = ParsingChangjo2(COM1, &rcvData[0], rcvCount, slaveCount);
									break;

								case VARIABLE_AIRCONDITIONER :
									ret = ParsingVariable(COM1, &rcvData[0], rcvCount, slaveCount);
									break;

								case NATURAL_AIRCONDITIONER :
									ret = ParsingNatural(COM1, &rcvData[0], rcvCount, slaveCount);
									break;

								case TEMPERATURE_MODULE :
									ret = ParsingTemperature(COM1, &rcvData[0], rcvCount, slaveCount);
									break;
							}

							if( ret == 1 )	commError[0][slaveCount] = 0;
							else {
								if( commError[0][slaveCount] < 10 )	commError[0][slaveCount] = commError[0][slaveCount] + 1;
							}

							mode = NEXT_MODE;
						}
					}
					else {
						if( commError[0][slaveCount] < 10 )	commError[0][slaveCount] = commError[0][slaveCount] + 1;
						mode = NEXT_MODE;
					}
				}
			}
		}
		
		// Communication Status Check
		if( xmitTime[0] >= XMIT_TIME ) {
			if( mode != XMIT_MODE ) {
				if( mode == RECEIVE_MODE ) {
					if( commError[0][slaveCount] < 10 )	commError[0][slaveCount] = commError[0][slaveCount] + 1;
				}

				mode = XMIT_MODE;
				xmitTime[0] = 0;
			}
		}
		
		if( commError[0][slaveCount] == 10 ) {
			memset(&com1Data[slaveCount][6], 0x00, myInfo.portInfo[0][slaveCount].dataLength);
			com1Data[slaveCount][6] = 0;
			com1Data[slaveCount][7] = 1;
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
	char rcvData[100], xmitData[15] = { 0, }, ret;
	unsigned char xmitNob, stx, etx;
	unsigned char checkSTX = NOT_OK, checkETX = NOT_OK;
	unsigned short rcvCount, mode, slaveAddress, slaveCount;

	InitQueue(&receiveQueue[1]);
	HAL_UART_Receive_IT(&huart3, (uint8_t *)&receiveBuffer[1], 1);

	rcvCount = 0;
	checkSTX = NOT_OK;
	checkETX = NOT_OK;
	mode = XMIT_MODE;
	
	slaveAddress = 0;
	slaveCount = myInfo.number_of_device[1] - 1;
	/* Infinite loop */
	while( true ) {
		if( mode == XMIT_MODE ) {
			slaveCount = (slaveCount + 1) % myInfo.number_of_device[1];
			slaveAddress = myInfo.portInfo[1][slaveCount].id;

			switch( myInfo.portInfo[1][slaveCount].name ) {
				case CHANGJO1_AIRCONDITIONER :
				case CHANGJO2_AIRCONDITIONER :
					xmitNob = XmitChangjo(&xmitData[0], slaveAddress);
					break;
					
				case VARIABLE_AIRCONDITIONER :
					xmitNob = XmitVariable(&xmitData[0], slaveAddress);
					break;

				case NATURAL_AIRCONDITIONER :
					xmitNob = XmitNatural(&xmitData[0], slaveAddress);
					break;

				case TEMPERATURE_MODULE :
					xmitNob = XmitTemperature(&xmitData[0], slaveAddress);
					break;
			}
			
			stx = xmitData[13];
			etx = xmitData[14];
			InitQueue(&receiveQueue[1]);
			if( xmitNob )	HAL_UART_Transmit(&huart3, (uint8_t *)&xmitData[0], xmitNob, 100);
			
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
					if( rcvCount < 100 ) {
						if( checkETX == NOT_OK ) {
							if( rcvCount == myInfo.portInfo[1][slaveCount].rcvLength )		checkETX = OK;
						}
						
						if( checkETX == OK ) {
							rcvData[rcvCount] = 0;
							switch( myInfo.portInfo[1][slaveCount].name ) {
								case CHANGJO1_AIRCONDITIONER :
									ret = ParsingChangjo1(COM2, &rcvData[0], rcvCount, slaveCount);
									break;

								case CHANGJO2_AIRCONDITIONER :
									ret = ParsingChangjo2(COM2, &rcvData[0], rcvCount, slaveCount);
									break;

								case VARIABLE_AIRCONDITIONER :
									ret = ParsingVariable(COM2, &rcvData[0], rcvCount, slaveCount);
									break;

								case NATURAL_AIRCONDITIONER :
									ret = ParsingNatural(COM2, &rcvData[0], rcvCount, slaveCount);
									break;

								case TEMPERATURE_MODULE :
									ret = ParsingTemperature(COM2, &rcvData[0], rcvCount, slaveCount);
									break;
							}

							if( ret == 1 )	commError[1][slaveCount] = 0;
							else {
								if( commError[1][slaveCount] < 10 )	commError[1][slaveCount] = commError[1][slaveCount] + 1;
							}

							mode = NEXT_MODE;
						}
					}
					else {
						if( commError[1][slaveCount] < 10 )	commError[1][slaveCount] = commError[1][slaveCount] + 1;
						mode = NEXT_MODE;
					}
				}
			}
		}
		
		// Communication Status Check
		if( xmitTime[1] >= XMIT_TIME ) {
			if( mode != XMIT_MODE ) {
				if( mode == RECEIVE_MODE ) {
					if( commError[1][slaveCount] < 10 )	commError[1][slaveCount] = commError[1][slaveCount] + 1;
				}

				mode = XMIT_MODE;
				xmitTime[1] = 0;
			}
		}
		
		if( commError[1][slaveCount] == 10 ) {
			memset(&com2Data[slaveCount][6], 0x00, myInfo.portInfo[1][slaveCount].dataLength);
			com2Data[slaveCount][6] = 0;
			com2Data[slaveCount][7] = 1;
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
	char rcvData[100], xmitData[15] = { 0, }, ret;
	unsigned char xmitNob, stx, etx;
	unsigned char checkSTX = NOT_OK, checkETX = NOT_OK;
	unsigned short rcvCount, mode, slaveAddress, slaveCount;

	InitQueue(&receiveQueue[2]);
	HAL_UART_Receive_IT(&huart4, (uint8_t *)&receiveBuffer[2], 1);

	rcvCount = 0;
	checkSTX = NOT_OK;
	checkETX = NOT_OK;
	mode = XMIT_MODE;
	
	slaveAddress = 0;
	slaveCount = myInfo.number_of_device[2] - 1;
	/* Infinite loop */
	while( true ) {
		if( mode == XMIT_MODE ) {
			slaveCount = (slaveCount + 1) % myInfo.number_of_device[2];
			slaveAddress = myInfo.portInfo[2][slaveCount].id;

			switch( myInfo.portInfo[2][slaveCount].name ) {
				case CHANGJO1_AIRCONDITIONER :
				case CHANGJO2_AIRCONDITIONER :
					xmitNob = XmitChangjo(&xmitData[0], slaveAddress);
					break;
					
				case VARIABLE_AIRCONDITIONER :
					xmitNob = XmitVariable(&xmitData[0], slaveAddress);
					break;

				case NATURAL_AIRCONDITIONER :
					xmitNob = XmitNatural(&xmitData[0], slaveAddress);
					break;

				case TEMPERATURE_MODULE :
					xmitNob = XmitTemperature(&xmitData[0], slaveAddress);
					break;
			}
			
			stx = xmitData[13];
			etx = xmitData[14];
			InitQueue(&receiveQueue[2]);
			if( xmitNob )	HAL_UART_Transmit(&huart4, (uint8_t *)&xmitData[0], xmitNob, 100);
			
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
					if( rcvCount < 100 ) {
						if( checkETX == NOT_OK ) {
							if( rcvCount == myInfo.portInfo[2][slaveCount].rcvLength )		checkETX = OK;
						}
						
						if( checkETX == OK ) {
							rcvData[rcvCount] = 0;
							switch( myInfo.portInfo[2][slaveCount].name ) {
								case CHANGJO1_AIRCONDITIONER :
									ret = ParsingChangjo1(COM3, &rcvData[0], rcvCount, slaveCount);
									break;

								case CHANGJO2_AIRCONDITIONER :
									ret = ParsingChangjo2(COM3, &rcvData[0], rcvCount, slaveCount);
									break;

								case VARIABLE_AIRCONDITIONER :
									ret = ParsingVariable(COM3, &rcvData[0], rcvCount, slaveCount);
									break;

								case NATURAL_AIRCONDITIONER :
									ret = ParsingNatural(COM3, &rcvData[0], rcvCount, slaveCount);
									break;

								case TEMPERATURE_MODULE :
									ret = ParsingTemperature(COM3, &rcvData[0], rcvCount, slaveCount);
									break;
							}

							if( ret == 1 )	commError[2][slaveCount] = 0;
							else {
								if( commError[2][slaveCount] < 10 )	commError[2][slaveCount] = commError[2][slaveCount] + 1;
							}

							mode = NEXT_MODE;
						}
					}
					else {
						if( commError[2][slaveCount] < 10 )	commError[2][slaveCount] = commError[2][slaveCount] + 1;
						mode = NEXT_MODE;
					}
				}
			}
		}
		
		// Communication Status Check
		if( xmitTime[2] >= XMIT_TIME ) {
			if( mode != XMIT_MODE ) {
				if( mode == RECEIVE_MODE ) {
					if( commError[2][slaveCount] < 10 )	commError[2][slaveCount] = commError[2][slaveCount] + 1;
				}

				mode = XMIT_MODE;
				xmitTime[2] = 0;
			}
		}
		
		if( commError[2][slaveCount] == 10 ) {
			memset(&com3Data[slaveCount][6], 0x00, myInfo.portInfo[2][slaveCount].dataLength);
			com3Data[slaveCount][6] = 0;
			com3Data[slaveCount][7] = 1;
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
	osThreadDef(mainTask, StartMainTask, osPriorityAboveNormal, 0, 3072);
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
	
	memset(&commError[0][0], 0x00, sizeof(commError));
	
	/* Infinite loop */
	while( true ) {
		if( recvTime[0] >= 60000 ) {
			osThreadTerminate(Com1TaskHandle);
			HAL_Delay(100);
			Com1TaskHandle = osThreadCreate(osThread(Com1Task), NULL);
			recvTime[0] = 0;
		}

		if( recvTime[1] >= 60000 ) {
			osThreadTerminate(Com2TaskHandle);
			HAL_Delay(100);
			Com2TaskHandle = osThreadCreate(osThread(Com2Task), NULL);
			recvTime[1] = 0;
		}

		if( recvTime[2] >= 60000 ) {
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
