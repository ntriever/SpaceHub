/**
  ******************************************************************************
  * @file           : queue.h
  * @brief          : Header for queue.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NT_QUEUE_H__
#define __NT_QUEUE_H__

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define QUEUE_SIZE	2048
#define NEXT(index)	((index + 1) % QUEUE_SIZE)  //원형 큐에서 인덱스를 변경하는 매크로 함수

typedef struct Queue //Queue 구조체 정의
{
	char buf[QUEUE_SIZE];//저장소
	unsigned short front; //꺼낼 인덱스(가장 오래전에 보관한 데이터가 있는 인덱스)
	unsigned short rear;//보관할 인덱스
} Queue;

/* USER CODE END Private defines */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void InitQueue(Queue *queue);//큐 초기화
int IsFull(Queue *queue); //큐가 꽉 찼는지 확인
int IsEmpty(Queue *queue); //큐가 비었는지 확인
void Enqueue(Queue *queue, char data); //큐에 보관
int Dequeue(Queue *queue); //큐에서 꺼냄

/* Private function prototypes -----------------------------------------------*/
/* USER CODE END PFP */

#endif /* __NT_QUEUE_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
