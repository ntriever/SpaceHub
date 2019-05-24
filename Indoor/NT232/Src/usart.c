/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "nt_queue.h"
	  
Queue receiveQueue[3], mainReceiveQueue;
unsigned char receiveBuffer[3][2], mainReceiveBuffer[2];
extern unsigned short recvTime[3], mainRecvTime;

/* USER CODE END 0 */

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart7;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* UART4 init function */
void MX_UART4_Init(unsigned int baudrate)
{
	huart4.Instance = UART4;
	huart4.Init.BaudRate = baudrate;
	huart4.Init.WordLength = UART_WORDLENGTH_8B;
	huart4.Init.StopBits = UART_STOPBITS_1;
	huart4.Init.Parity = UART_PARITY_NONE;
	huart4.Init.Mode = UART_MODE_TX_RX;
	huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart4.Init.OverSampling = UART_OVERSAMPLING_16;
	if( HAL_UART_Init(&huart4) != HAL_OK ) {
		Error_Handler();
	}
}

/* UART7 init function */
void MX_UART7_Init(void)
{
	huart7.Instance = UART7;
	huart7.Init.BaudRate = 115200;
	huart7.Init.WordLength = UART_WORDLENGTH_8B;
	huart7.Init.StopBits = UART_STOPBITS_1;
	huart7.Init.Parity = UART_PARITY_NONE;
	huart7.Init.Mode = UART_MODE_TX_RX;
	huart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart7.Init.OverSampling = UART_OVERSAMPLING_16;
	if( HAL_UART_Init(&huart7) != HAL_OK ) {
		Error_Handler();
	}
}

/* USART1 init function */
void MX_USART1_UART_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if( HAL_UART_Init(&huart1) != HAL_OK ) {
		Error_Handler();
	}
}

/* USART2 init function */
void MX_USART2_UART_Init(unsigned int baudrate)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = baudrate;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if( HAL_UART_Init(&huart2) != HAL_OK ) {
		Error_Handler();
	}
}

/* USART3 init function */
void MX_USART3_UART_Init(unsigned int baudrate)
{
	huart3.Instance = USART3;
	huart3.Init.BaudRate = baudrate;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	if( HAL_UART_Init(&huart3) != HAL_OK ) {
		Error_Handler();
	}
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if( uartHandle->Instance == UART4 ) {
		/* USER CODE BEGIN UART4_MspInit 0 */

		/* USER CODE END UART4_MspInit 0 */
		/* UART4 clock enable */
		__HAL_RCC_UART4_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**UART4 GPIO Configuration    
		PA1     ------> UART4_RX
		PA12     ------> UART4_TX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_1;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_12;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF11_UART4;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* UART4 interrupt Init */
		HAL_NVIC_SetPriority(UART4_IRQn, 8, 0);
		HAL_NVIC_EnableIRQ(UART4_IRQn);
		/* USER CODE BEGIN UART4_MspInit 1 */

		/* USER CODE END UART4_MspInit 1 */
	}
	else if( uartHandle->Instance == UART7 ) {
		/* USER CODE BEGIN UART7_MspInit 0 */

		/* USER CODE END UART7_MspInit 0 */
		/* UART7 clock enable */
		__HAL_RCC_UART7_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**UART7 GPIO Configuration    
		PA8     ------> UART7_RX
		PA15     ------> UART7_TX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_15;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF8_UART7;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* UART7 interrupt Init */
		HAL_NVIC_SetPriority(UART7_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(UART7_IRQn);
		/* USER CODE BEGIN UART7_MspInit 1 */

		/* USER CODE END UART7_MspInit 1 */
	}
	else if( uartHandle->Instance == USART1 ) {
		/* USER CODE BEGIN USART1_MspInit 0 */

		/* USER CODE END USART1_MspInit 0 */
		/* USART1 clock enable */
		__HAL_RCC_USART1_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART1 GPIO Configuration    
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART1 interrupt Init */
		HAL_NVIC_SetPriority(USART1_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		/* USER CODE BEGIN USART1_MspInit 1 */

		/* USER CODE END USART1_MspInit 1 */
	}
	else if( uartHandle->Instance == USART2 ) {
		/* USER CODE BEGIN USART2_MspInit 0 */

		/* USER CODE END USART2_MspInit 0 */
		/* USART2 clock enable */
		__HAL_RCC_USART2_CLK_ENABLE();

		__HAL_RCC_GPIOA_CLK_ENABLE();
		/**USART2 GPIO Configuration    
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* USART2 interrupt Init */
		HAL_NVIC_SetPriority(USART2_IRQn, 6, 0);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
		/* USER CODE BEGIN USART2_MspInit 1 */

		/* USER CODE END USART2_MspInit 1 */
	}
	else if( uartHandle->Instance == USART3 ) {
		/* USER CODE BEGIN USART3_MspInit 0 */

		/* USER CODE END USART3_MspInit 0 */
		/* USART3 clock enable */
		__HAL_RCC_USART3_CLK_ENABLE();

		__HAL_RCC_GPIOC_CLK_ENABLE();
		__HAL_RCC_GPIOB_CLK_ENABLE();
		/**USART3 GPIO Configuration    
		PC5     ------> USART3_RX
		PB10     ------> USART3_TX 
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_5;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

		/* USART3 interrupt Init */
		HAL_NVIC_SetPriority(USART3_IRQn, 7, 0);
		HAL_NVIC_EnableIRQ(USART3_IRQn);
		/* USER CODE BEGIN USART3_MspInit 1 */

		/* USER CODE END USART3_MspInit 1 */
	}
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
	if( uartHandle->Instance == UART4 ) {
		/* USER CODE BEGIN UART4_MspDeInit 0 */

		/* USER CODE END UART4_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_UART4_CLK_DISABLE();

		/**UART4 GPIO Configuration    
		PA1     ------> UART4_RX
		PA12     ------> UART4_TX 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_12);

		/* UART4 interrupt Deinit */
		HAL_NVIC_DisableIRQ(UART4_IRQn);
		/* USER CODE BEGIN UART4_MspDeInit 1 */

		/* USER CODE END UART4_MspDeInit 1 */
	}
	else if( uartHandle->Instance == UART7 ) {
		/* USER CODE BEGIN UART7_MspDeInit 0 */

		/* USER CODE END UART7_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_UART7_CLK_DISABLE();

		/**UART7 GPIO Configuration    
		PA8     ------> UART7_RX
		PA15     ------> UART7_TX 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_15);

		/* UART7 interrupt Deinit */
		HAL_NVIC_DisableIRQ(UART7_IRQn);
		/* USER CODE BEGIN UART7_MspDeInit 1 */

		/* USER CODE END UART7_MspDeInit 1 */
	}
	else if( uartHandle->Instance == USART1 ) {
		/* USER CODE BEGIN USART1_MspDeInit 0 */

		/* USER CODE END USART1_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART1_CLK_DISABLE();

		/**USART1 GPIO Configuration    
		PA9     ------> USART1_TX
		PA10     ------> USART1_RX 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

		/* USART1 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART1_IRQn);
		/* USER CODE BEGIN USART1_MspDeInit 1 */

		/* USER CODE END USART1_MspDeInit 1 */
	}
	else if( uartHandle->Instance == USART2 ) {
		/* USER CODE BEGIN USART2_MspDeInit 0 */

		/* USER CODE END USART2_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART2_CLK_DISABLE();

		/**USART2 GPIO Configuration    
		PA2     ------> USART2_TX
		PA3     ------> USART2_RX 
		*/
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

		/* USART2 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART2_IRQn);
		/* USER CODE BEGIN USART2_MspDeInit 1 */

		/* USER CODE END USART2_MspDeInit 1 */
	}
	else if( uartHandle->Instance == USART3 ) {
		/* USER CODE BEGIN USART3_MspDeInit 0 */

		/* USER CODE END USART3_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_USART3_CLK_DISABLE();

		/**USART3 GPIO Configuration    
		PC5     ------> USART3_RX
		PB10     ------> USART3_TX 
		*/
		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_5);

		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10);

		/* USART3 interrupt Deinit */
		HAL_NVIC_DisableIRQ(USART3_IRQn);
		/* USER CODE BEGIN USART3_MspDeInit 1 */

		/* USER CODE END USART3_MspDeInit 1 */
	}
} 

/* USER CODE BEGIN 1 */
/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if( huart->Instance == USART2 ) {
		Enqueue(&receiveQueue[0], receiveBuffer[0][0]);
		HAL_UART_Receive_IT(&huart2, (uint8_t *)&receiveBuffer[0], 1);
		recvTime[0] = 0;
	}

	if( huart->Instance == USART3 ) {
		Enqueue(&receiveQueue[1], receiveBuffer[1][0]);
		HAL_UART_Receive_IT(&huart3, (uint8_t *)&receiveBuffer[1], 1);
		recvTime[1] = 0;
	}

	if( huart->Instance == UART4 ) {
		Enqueue(&receiveQueue[2], receiveBuffer[2][0]);
		HAL_UART_Receive_IT(&huart4, (uint8_t *)&receiveBuffer[2], 1);
		recvTime[2] = 0;
	}

	if( huart->Instance == UART7 ) {
		Enqueue(&mainReceiveQueue, mainReceiveBuffer[0]);
		HAL_UART_Receive_IT(&huart7, (uint8_t *)&mainReceiveBuffer, 1);
		mainRecvTime = 0;
	}
}

void MX_UART_Init(char port, unsigned short device)
{
	unsigned int baudrate;
	
	switch( device ) {
		case DONGA_CRS1800 :
		case DONGA_CRS1800N :
		case ANTS_CRS1800 :
		case EMERSON :
		case FAN_CONTROL :
		case AIRCONDITIONER_MICOM :
			baudrate = 9600;
			break;
		
		case DONGA_MR :
			baudrate = 38400;
			break;

		case MBRS_1200R :
		case MBRS_1200N :
		case MBRS_1200 :
			baudrate = 1200;
			break;
			
		case CRS24_150 :
		case SDPS_48N :
		case SWPS_4150W2 :
		case LORS_4150 :
		case LORS_460 :
		case NORS_4150 :
		case NORS_460 :
			baudrate = 57600;
			break;
	}
	
	if( port == COM1 )			MX_USART2_UART_Init(baudrate);
	else if( port == COM2 )		MX_USART3_UART_Init(baudrate);
	else if( port == COM3 )		MX_UART4_Init(baudrate);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
