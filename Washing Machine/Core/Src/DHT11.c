#include "main.h"
#include "DHT11.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// extern TIM_HandleTypeDef htim2;
// extern void FND_update(uint16_t value);
extern void delay_us (unsigned long us);
extern volatile int TDHT11_timer_counter;
extern volatile int TIM10_10ms_counter;
extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);

extern uint8_t lcd_display_mode_flag;

void DHT11_main(void);
void DHT11_Init(void);
void DHT11_processing(void);
int dht11time = 150;		// ms


void DHT11_main(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;
	
	// HAL_TIM_Base_Start_IT(&htim2);
	DHT11_Init();
	
printf("DHT11_Init()");		// 테스트용

	while(1)
	{
		DHT11_trriger();
		DHT11_DataLine_Input();
		DHT11_dumi_read();
		
		i_RH = DHT11_rx_Data();
		d_RH = DHT11_rx_Data();
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();
		
		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
		printf("[Tmp]%d\n", (int)i_Tmp);
		printf("[Wet]%d\n", (int)i_RH);
//		FND_Update(i_Tmp*100 + i_RH);
		HAL_Delay(1500); 	// 초기화 루틴에는 hal_delay 존재해도 괜찮음.
	}
	
}


void DHT11_processing(void)
{
	uint8_t i_RH, d_RH, i_Tmp, d_Tmp;
	char lcd_buff[20];

	if (TIM10_10ms_counter >= dht11time)
	{
		TIM10_10ms_counter = 0;
		DHT11_trriger();
		DHT11_DataLine_Input();
		DHT11_dumi_read();

		i_RH = DHT11_rx_Data();
		d_RH = DHT11_rx_Data();
		i_Tmp = DHT11_rx_Data();
		d_Tmp = DHT11_rx_Data();

		DHT11_DataLine_Output();
		HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
		printf("[Tmp]%d\n",(int)i_Tmp);
		printf("[Wet]%d\n",(int)i_RH);
		sprintf(lcd_buff, "Tmp:%d Wet:%d", (int)i_Tmp,  (int)i_RH);

		if (lcd_display_mode_flag == 1)
		{
			move_cursor(0,0);		// 1번째 라인의 1번 칼럼
			lcd_string(lcd_buff);		move_cursor(0,0);		// 1번째 라인의 1번 칼럼
			lcd_string(lcd_buff);
		}
//		FND_update(i_Tmp*100 + i_RH);
	}
}

void DHT11_Init(void)
{
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
	HAL_Delay(3000);
	return;
}

void DHT11_trriger(void)
{
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_RESET);
	HAL_Delay(20);
	
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_DATA_RIN, GPIO_PIN_SET);
	delay_us(7);
	return;
}

void DHT11_DataLine_Input(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;			//Change Output to Input
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
	
	return;
}

void DHT11_DataLine_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	/*Configure GPIO pin : PH0 */
  GPIO_InitStruct.Pin = DHT11_DATA_RIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;			//Change Input to Output 
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
	
	return;
}

uint8_t DHT11_rx_Data(void)
{
	uint8_t rx_data = 0;
	
	for(int i = 0; i < 8; i++)		// 8비트를 읽어들인다.
	{
		//when Input Data == 0
		while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
#if 1
		delay_us(40);
#else  // org
		delay_us(16);
#endif
		rx_data<<=1;
		
		//when Input Data == 1
		if(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN))
		{
			rx_data |= 1;
		}
		while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	}
	return rx_data;
}

void DHT11_dumi_read(void)
{
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 0 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	while( 1 == HAL_GPIO_ReadPin(DHT11_PORT, DHT11_DATA_RIN) );
	return;
}
