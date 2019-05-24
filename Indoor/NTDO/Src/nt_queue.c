
/**
  ******************************************************************************
  * @file           : queue.c
  * @brief          : Circular queue with auto-expansion
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/* USER CODE BEGIN Includes */
#include "nt_queue.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* Private variables ---------------------------------------------------------*/
/* USER CODE END PV */

/* USER CODE END PFP */
/* USER CODE BEGIN 0 */

//원형 큐 - 버퍼크기 고정, 정수 보관
void InitQueue(Queue *queue)
{
	queue->front = queue->rear = 0; //front와 rear를 0으로 설정
}

int IsFull(Queue *queue)
{
	//원형 큐에서 꽉 찼는지 비었는지 체크할 수 있게 rear 다음 공간은 빈 상태를 유지합니다.
	return NEXT(queue->rear) == queue->front;//다음 rear가 front와 같으면 꽉 찬 상태
}

int IsEmpty(Queue *queue)
{
	return queue->front == queue->rear;    //front와 rear가 같으면 빈 상태
}

void Enqueue(Queue *queue, char data)
{
	if( IsFull(queue) ) { //큐가 꽉 찼을 때
//		printf("큐가 꽉 찼음\n");
		return;
	}
	queue->buf[queue->rear] = data;//rear 인덱스에 데이터 보관
	queue->rear = NEXT(queue->rear); //rear를 다음 위치로 설정
}

int Dequeue(Queue *queue)
{
	int re = 0;

	if( IsEmpty(queue) ) { //큐가 비었을 때
//		printf("큐가 비었음\n");
		return re;
	}
	re = queue->buf[queue->front];//front 인덱스에 보관한 값을 re에 설정
	queue->front = NEXT(queue->front);//front를 다음 위치로 설정

	return re;
}

/* USER CODE END 0 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
