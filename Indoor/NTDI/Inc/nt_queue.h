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
#define NEXT(index)	((index + 1) % QUEUE_SIZE)  //���� ť���� �ε����� �����ϴ� ��ũ�� �Լ�

typedef struct Queue //Queue ����ü ����
{
	char buf[QUEUE_SIZE];//�����
	unsigned short front; //���� �ε���(���� �������� ������ �����Ͱ� �ִ� �ε���)
	unsigned short rear;//������ �ε���
} Queue;

/* USER CODE END Private defines */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

void InitQueue(Queue *queue);//ť �ʱ�ȭ
int IsFull(Queue *queue); //ť�� �� á���� Ȯ��
int IsEmpty(Queue *queue); //ť�� ������� Ȯ��
void Enqueue(Queue *queue, char data); //ť�� ����
int Dequeue(Queue *queue); //ť���� ����

/* Private function prototypes -----------------------------------------------*/
/* USER CODE END PFP */

#endif /* __NT_QUEUE_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
