#include "ultrasonic.h"

extern volatile int TIM10_10ms_ultrasonic;

volatile int distance;		// 거리를 픅정한 펄스 갯수를 저장하는 변수
volatile int ic_cpt_finish_flag = 0;		// 초음파 거리 측정 완료 indicator 변수
volatile uint8_t is_first_capture = 0;		// 0 : 상승 에지, 1: 하강 에지
extern void move_cursor(uint8_t row, uint8_t column);
extern void lcd_string(uint8_t *str);
extern void buzzer_alert(void);

extern uint8_t lcd_display_mode_flag;

void ultrasonic_processing(void);

// 1. Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx.hal_tim.c 에 가서
// HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) 를 잘라내기 해서 가져온다.
// 2. 초음파 센서의 ECHO핀의 상승 에지와 하강 에지 발생시 이곳으로 들어온다.
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM3)
	{
		if (is_first_capture == 0)		// 상승 에지
		{
			__HAL_TIM_SET_COUNTER(htim, 0);		// clear HW counter
			is_first_capture = 1;		// 상승 에지를 만났다는 flag 변수 indicator를 set
		}
		else if (is_first_capture == 1)		// 하강 에지
		{
			is_first_capture = 0;		// 다음 echo 펄스를 count하기 위해 변수 초기화
			distance = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);		// 현재까지 count한 변수 수를 distance에 저장
			ic_cpt_finish_flag = 1;		// 초음파 측정 완료
		}
	}
}

void WM_ultrasonic_processing(void)
{
	int dis;

	if (TIM10_10ms_ultrasonic >= 100)		// 1초가 되다
	{
		TIM10_10ms_ultrasonic = 0;
		make_trigger();
		if (ic_cpt_finish_flag)		// 초음파 측정이 완료 되었는지 확인 = if (ic_cpt_finish_flag >= 1)
		{
			ic_cpt_finish_flag = 0;		// 다음 캡쳐를 위해 clear
			dis = distance;
			dis = dis * 0.034 / 2;		// 1마이크로 센티미터가 0.034cm 이동, 나누기 2한 것은 왕복 거리이기 때문. 편도 거리를 찾기 위해

			if (dis <= 5)
				buzzer_alert();
		}
	}
}

void ultrasonic_processing(void)
{
	int dis;
	char lcd_buff[20];

	if (TIM10_10ms_ultrasonic >= 100)		// 1초가 되다
	{
		TIM10_10ms_ultrasonic = 0;
		make_trigger();
		if (ic_cpt_finish_flag)		// 초음파 측정이 완료 되었는지 확인 = if (ic_cpt_finish_flag >= 1)
		{
			ic_cpt_finish_flag = 0;		// 다음 캡쳐를 위해 clear
			dis = distance;
			dis = dis * 0.034 / 2;		// 1마이크로 센티미터가 0.034cm 이동, 나누기 2한 것은 왕복 거리이기 때문. 편도 거리를 찾기 위해
			printf("dis : %dcm\n", dis);

			if (lcd_display_mode_flag == 2)
			{
				sprintf(lcd_buff, "dis:%dcm", dis);
				move_cursor(0,0);		// 1번째 라인의 1번 칼럼
				lcd_string(lcd_buff);
			}
		}
	}
}

void make_trigger()
{
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);
	delay_us(2);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 1);
	delay_us(10);
	HAL_GPIO_WritePin(ULTRASONIC_TRIGGER_GPIO_Port, ULTRASONIC_TRIGGER_Pin, 0);
}
