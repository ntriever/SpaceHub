
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

//���� ť - ����ũ�� ����, ���� ����
void InitQueue(Queue *queue)
{
	queue->front = queue->rear = 0; //front�� rear�� 0���� ����
}

int IsFull(Queue *queue)
{
	//���� ť���� �� á���� ������� üũ�� �� �ְ� rear ���� ������ �� ���¸� �����մϴ�.
	return NEXT(queue->rear) == queue->front;//���� rear�� front�� ������ �� �� ����
}

int IsEmpty(Queue *queue)
{
	return queue->front == queue->rear;    //front�� rear�� ������ �� ����
}

void Enqueue(Queue *queue, char data)
{
	if( IsFull(queue) ) { //ť�� �� á�� ��
//		printf("ť�� �� á��\n");
		return;
	}
	queue->buf[queue->rear] = data;//rear �ε����� ������ ����
	queue->rear = NEXT(queue->rear); //rear�� ���� ��ġ�� ����
}

int Dequeue(Queue *queue)
{
	int re = 0;

	if( IsEmpty(queue) ) { //ť�� ����� ��
//		printf("ť�� �����\n");
		return re;
	}
	re = queue->buf[queue->front];//front �ε����� ������ ���� re�� ����
	queue->front = NEXT(queue->front);//front�� ���� ��ġ�� ����

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
