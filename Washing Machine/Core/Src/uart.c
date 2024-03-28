#include "uart.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void _exit(int status);

extern uint8_t rx_data;
extern uint8_t bt_rx_data;

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

//when we do not make circular queue, there will be a problem.
//HW와 SW의 만나는 약속 장소 : call back function.
//move from HAL_UART_RxCpltCallback of stm324xx_hal_uart to here
//UART로 1 byte가 수신되면 H/W가 call 해준다.
//UART RX INT가 발생 되면 이곳으로 자동적으로 들어온다.

void pc_command_processing(void);
void bt_command_processing(void);

extern void led_all_on(void);
extern void led_all_off(void);
extern void led_on_up(void);
extern void led_on_down(void);
extern void led_keepon_up(void);
extern void led_keepon_down(void);
extern void led_gradation(void);
extern void led_flower_on(void);
extern void led_flower_off(void);

extern int dht11time;
extern void DHT11_processing(void);

extern void ultrasonic_processing(void);

extern void set_rtc(char *date_time);

// Call back function : where HW and SW does meet
#if 1
#else		// original
#define COMMAND_LENGTH 40
volatile unsigned char rx_buff[COMMAND_LENGTH];		// reason for variable as volatile : seek memo behind the book
volatile int rx_index = 0;		// save place of rx_buffer
volatile int newline_detect_flag = 0;		// indicator of when meeting new line
#endif

#define COMMAND_LENGTH 40
volatile unsigned char rx_buff[COMMAND_LENGTH];		// reason for variable as volatile : seek memo behind the book
volatile int rx_index = 0;		// save place of rx_buffer
volatile int newline_detect_flag = 0;		// indicator of when meeting new line

volatile unsigned char bt_rx_buff[COMMAND_LENGTH];
volatile int bt_rx_index = 0;
volatile int bt_newline_detect_flag = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart == &huart3)
	{
		if (rx_index < COMMAND_LENGTH)	 //현재까지 들어온 byte가 20byte를 넘지 않으면
		{
			if (rx_data == '\n' || rx_data == '\r')
			{
				rx_buff[rx_index] = 0;
				newline_detect_flag = 1;
				rx_index = 0;		// for saving next message.
			}
			else
			{
				rx_buff[rx_index++] = rx_data;
			}
		}
		else
		{
			rx_index = 0;
			printf("Message Overflow !!! \n");
		}
		HAL_UART_Receive_IT(&huart3, &rx_data, 1);		// warning!!! 반드시 이 함수를 다시 호출해야 다음 INT가 발생한다.
	}

	if (huart == &huart6)
	{
		if (bt_rx_index < COMMAND_LENGTH)	 //현재까지 들어온 byte가 20byte를 넘지 않으면
		{
			if (bt_rx_data == '\n' || bt_rx_data == '\r')
			{
				bt_rx_buff[bt_rx_index] = 0;
				bt_newline_detect_flag = 1;
				bt_rx_index = 0;		// for saving next message.
			}
			else
			{
				bt_rx_buff[bt_rx_index++] = bt_rx_data;
			}
		}
		else
		{
			bt_rx_index = 0;
			printf("BT Message Overflow !!! \n");
		}
		HAL_UART_Receive_IT(&huart6, &bt_rx_data, 1);		// warning!!! 반드시 이 함수를 다시 호출해야 다음 INT가 발생한다.
	}
}

#if 0
void pc_command_processing(void)
{
	if (newline_detect_flag)		// when meet \n
	{
		newline_detect_flag = 0;
		printf("%s\n", rx_buff);
		if (!strncmp(rx_buff, "led_all_on", strlen("led_all_on")))		//if(strncmp(rx_buff, "ledallon",strlen("ledallon")) == NULL)
		{
			led_all_on();
			return;
		}
		if (!strncmp(rx_buff, "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;
		}
		if (!strncmp(rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if (!strncmp(rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
		if (!strncmp(rx_buff, "led_flower_on", strlen("led_flower_on")))
		{
			led_flower_on();
			return;
		}
		if (!strncmp(rx_buff, "led_flower_off", strlen("led_flower_off")))
		{
			led_flower_off();
			return;
		}
	}
}
#endif

void pc_command_processing(void)
{
	if (newline_detect_flag)		// when meet \n
	{
		newline_detect_flag = 0;
		printf("%s\n", rx_buff);

#if 0
		if (!strncmp(rx_buff, "dht11time150", strlen("dht11time150")))
		{
			dht11time = 150;	// 궁금점 : 왜 dht11time을 0으로 선언해놓으면(dht11.c에서) 동작하지 않을까?
			return;
		}
		if (!strncmp(rx_buff, "dht11time300", strlen("dht11time300")))
		{
			dht11time = 300;
			return;
		}
		if (!strncmp(rx_buff, "dht11time200", strlen("dht11time200")))
		{
			dht11time = 200;
			return;
		}
#endif

		// 동적 할당을 활용한 교수님 예시 코드
		if (!strncmp(rx_buff, "dht11time", strlen("dht11time")))
		{
			dht11time = atoi(rx_buff+9);
			return;
		}

		if (!strncmp(rx_buff, "setrtc", strlen("setrtc")))
		{
			set_rtc(rx_buff);
			return;
		}

	}
}

void pc_command_processing2(void)
{
	if (newline_detect_flag)		// when meet \n
	{
		newline_detect_flag = 0;
		printf("%s\n", rx_buff);

		if (!strncmp(rx_buff, "dht11_on", strlen("dht11_on")))
		{
			while(1)
			{
				DHT11_processing();
				if (!strncmp(rx_buff, "dht11_off", strlen("dht11_off"))) {
					break;
				}
				else if (!strncmp(rx_buff, "ultra_on", strlen("ultra_on"))) {
					break;
				}
			}
			return;
		}
		/*if (!strncmp(rx_buff, "dht11_off", strlen("dht11_off")))
		{
			return;
		}*/
		if (!strncmp(rx_buff, "ultra_on", strlen("ultra_on")))
		{
			while (1)
			{
				ultrasonic_processing();
				if (!strncmp(rx_buff, "ultra_off", strlen("ultra_off"))) {
					break;
				}
				else if (!strncmp(rx_buff, "dht11_on", strlen("dht11_on"))) {
					break;
				}
			}
			return;
		}
		/*
		if (!strncmp(rx_buff, "ultra_off", strlen("ultra_off")))
		{
			return;
		} */
	}
}

void bt_command_processing(void)		//손대지 맙시다
{
	if (bt_newline_detect_flag)		// when meet \n
	{
		bt_newline_detect_flag = 0;
		printf("%s\n", bt_rx_buff);
		if (!strncmp(bt_rx_buff, "dht11time150", strlen("dht11time150")))
		{
			DHT11_processing();
			return;
		}
		if (!strncmp(bt_rx_buff, "dht11time300", strlen("dht11time300")))
		{
			DHT11_processing();
			return;
		}
		if (!strncmp(bt_rx_buff, "dht11time300", strlen("dht11time300")))
		{
			DHT11_processing();
			return;
		}
	}
}

#if 0
void bt_command_processing(void)		//손대지 맙시다
{
	if (bt_newline_detect_flag)		// when meet \n
	{
		bt_newline_detect_flag = 0;
		printf("%s\n", bt_rx_buff);
		if (!strncmp(bt_rx_buff, "led_all_on", strlen("led_all_on")))		//if(strncmp(rx_buff, "ledallon",strlen("ledallon")) == NULL)
		{
			led_all_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_all_off", strlen("led_all_off")))
		{
			led_all_off();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_up", strlen("led_on_up")))
		{
			led_on_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_on_down", strlen("led_on_down")))
		{
			led_on_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keepon_up", strlen("led_keepon_up")))
		{
			led_keepon_up();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_keepon_down", strlen("led_keepon_down")))
		{
			led_keepon_down();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_flower_on", strlen("led_flower_on")))
		{
			led_flower_on();
			return;
		}
		if (!strncmp(bt_rx_buff, "led_flower_off", strlen("led_flower_off")))
		{
			led_flower_off();
			return;
		}
	}
}
#endif
