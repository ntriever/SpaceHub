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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
unsigned short xmitTime = 0, mainRecvTime = 0;

extern BoardInfo myInfo;
extern Queue mainReceiveQueue;
extern unsigned char mainReceiveBuffer[2];

extern unsigned char slotID;
extern unsigned char replyInfoData[38];
extern unsigned char replyDetailedData[43];
extern unsigned char replyData[16];
extern unsigned char portData[4][8];
extern short replyInfoLength, replyDetailedLength;

/* USER CODE END Variables */

osThreadId mainTaskHandle;
osThreadId debugTaskHandle;
osThreadId inputTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* USER CODE END FunctionPrototypes */

void StartMainTask(void const * argument);
void StartDebugTask(void const * argument);
void StartInputTask(void const * argument);

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
//	osThreadDef(mainTask, StartMainTask, osPriorityHigh, 0, 512);
//	mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);

	/* definition and creation of debugTask */
	osThreadDef(debugTask, StartDebugTask, osPriorityBelowNormal, 0, 256);
	debugTaskHandle = osThreadCreate(osThread(debugTask), NULL);

	/* definition and creation of inputTask */
	osThreadDef(inputTask, StartInputTask, osPriorityNormal, 0, 256);
	inputTaskHandle = osThreadCreate(osThread(inputTask), NULL);

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
	unsigned char rcvData[100], parsingData[100], compareData[4];
	unsigned int rcvCount = 0, rcvLength;

	InitQueue(&mainReceiveQueue);
	HAL_UART_Receive_IT(&huart7, (uint8_t *)&mainReceiveBuffer, 1);

	headerCheck = NOT_OK;
	
	sprintf((char *)&compareData, "M%cT", slotID + 0x30);
	rcvCount = 0;

	/* Infinite loop */
	while( true) {
		if( !IsEmpty(&mainReceiveQueue) ) {
			rcvData[rcvCount++] = Dequeue(&mainReceiveQueue);

			if( headerCheck == NOT_OK ) {
				if( rcvCount == 3 ) {
					rcvData[rcvCount] = 0;
					if( memcmp(&rcvData[0], &compareData[0], 3) == 0 ) {
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
					HAL_UART_Transmit(&huart7, &replyInfoData[0], replyInfoLength, 1000);
					break;
					
				case REQUEST_DATA :
					TransmissionDetailedDataToMain();
					HAL_UART_Transmit(&huart7, &replyDetailedData[0], replyDetailedLength, 1000);
					break;

				case SET_INFO :
					SetBoardData(parsingData);
					replyData[3] = SET_INFO;
					HAL_UART_Transmit(&huart7, &replyData[0], 15, 1000);
					break;

				case CONTROL_DATA :
					replyData[3] = CONTROL_DATA;
					HAL_UART_Transmit(&huart7, &replyData[0], 15, 1000);
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
	/* definition and creation of mainTask */
	osThreadDef(mainTask, StartMainTask, osPriorityHigh, 0, 512);
	mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);

	/* Infinite loop */
	while( true ) {
		if( mainRecvTime >= 10000 ) {
			osThreadTerminate(mainTaskHandle);
			HAL_Delay(100);
			mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);
			mainRecvTime = 0;
		}

		osDelay(1);
	}
	/* USER CODE END StartDebugTask */
}

/* USER CODE BEGIN Header_StartInputTask */
/**
* @brief Function implementing the inputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInputTask */
void StartInputTask(void const * argument)
{
	/* USER CODE BEGIN StartInputTask */
	/* Infinite loop */
	while( true ) {
		if( portData[0][6] )	HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_SET);
		else					HAL_GPIO_WritePin(OUT0_GPIO_Port, OUT0_Pin, GPIO_PIN_RESET);
		if( portData[1][6] )	HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_SET);
		else					HAL_GPIO_WritePin(OUT1_GPIO_Port, OUT1_Pin, GPIO_PIN_RESET);
		if( portData[2][6] )	HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_SET);
		else					HAL_GPIO_WritePin(OUT2_GPIO_Port, OUT2_Pin, GPIO_PIN_RESET);
		if( portData[3][6] )	HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_SET);
		else					HAL_GPIO_WritePin(OUT3_GPIO_Port, OUT3_Pin, GPIO_PIN_RESET);

		osDelay(1);
	}
	/* USER CODE END StartInputTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
