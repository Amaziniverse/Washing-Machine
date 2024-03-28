#include "dcmotor.h"
#include "button.h"
#include "led.h"
#include "i2c_lcd.h"
#include "buzzer.h"

extern TIM_HandleTypeDef htim4;
extern void lcd_string(uint8_t *str);
extern uint8_t lcd_display_mode_flag;
extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);

extern void buzzer_start(void);
extern void buzzer_end(void);
extern

void dcmotor_pwm_control(void);
void laundry_control(void);
void rinse_control(void);
void dry_control(void);
void WM_dcmotor_control_laudry1(void);
void WM_dcmotor_control_laudry2(void);
void WM_dcmotor_control_rinse1(void);
void WM_dcmotor_control_rinse2(void);
void WM_dcmotor_control_dry1(void);
void WM_dcmotor_control_dry2(void);

uint8_t pwm_start_flag = 0;		// dcmotor start/stop indicator
uint16_t CCR_UP_Value = 0;		// pwm up control
uint16_t CCR_DOWN_Value = 0;		// pwm down control
uint8_t forward_backward_dcmotor = 0;


// BUTTON0 : start/stop : LED1 toggle
// BUTTON1 : speed-up : LED2 toggle
// BUTTON2 : speed-down : LED3 toggle
// BUTTON3 : 정회전 역회전

/*
void dcmotor_pwm_control(void)
{
	// start/stop : LED1 toggle
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);		// LD1

//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);

		if (!pwm_start_flag)
		{
			pwm_start_flag = 1;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}
	}

	// speed-up : LED2 toggle
	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);		// LD2
		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_UP_Value += 10;
		if (CCR_UP_Value > 100)
		{
			CCR_UP_Value = 100;
		}
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);
	}

	// speed-down : LED3 toggle
	if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);		// LD3
		CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_DOWN_Value -= 10;
		if (CCR_DOWN_Value < 60)
		{
			CCR_DOWN_Value = 60;
		}
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);
	}

	// 정회전 역회전
	if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		if (!forward_backward_dcmotor)
		{
			forward_backward_dcmotor = 1;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}
		else
		{
			forward_backward_dcmotor = 0;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
	}
}
*/

// washing machine
// BTN0 : 재생-일시정지
// BTN1 : 세탁 : LED1 toggle
// BTN2 : 헹굼 : LED2 toggle
// BTN3 : 탈수 : LED3 toggle
#define LAUNDRY_SELECT  1
#define RINSE_SELECT   2
#define DRY_SELECT     3
uint8_t mode_select = 0;
uint8_t laundry_power = 0;
uint8_t rinse_power = 0;
uint8_t dry_power = 0;
uint8_t control_select = 0;


void WM_dcmotor_control(void)
{
	uint8_t btn_buff[40];

	if (lcd_display_mode_flag == 0)   // if BTN4가
	{
		// 세탁 : LED1 toggle
		if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
		{
			laundry_power++;
			laundry_power%=2;

			if (laundry_power)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);		// LD1
				lcd_command(CLEAR_DISPLAY);
				move_cursor(0,0);
				sprintf((char*)btn_buff, "  laundry.");
				lcd_string(btn_buff);
				mode_select = LAUNDRY_SELECT;
			}
		}

		// 헹굼 : LED2 toggle
		if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
		{
			rinse_power++;
			rinse_power%=2;

			if (rinse_power)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);		// LD2
				lcd_command(CLEAR_DISPLAY);
				move_cursor(0,0);
				sprintf((char*)btn_buff, "  rinse.");
				lcd_string(btn_buff);
				mode_select = RINSE_SELECT;
			}
		}

		// 탈수 : LED3 toggle
		if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
		{
			dry_power++;
			dry_power%=2;

			if (dry_power)
			{
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);		// LD3
				lcd_command(CLEAR_DISPLAY);
				move_cursor(0,0);
				sprintf((char*)btn_buff, "  dry.");
				lcd_string(btn_buff);
				mode_select = DRY_SELECT;
			}
		}

		switch(mode_select)
		{
		case LAUNDRY_SELECT :
			laundry_control();
			break;
		case RINSE_SELECT :
			rinse_control();
			break;
		case DRY_SELECT :
			dry_control();
			break;
		}
	}
}

void laundry_control(void)
{
	uint8_t lcd_buff[20];

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		control_select++;
		control_select %= 2;

		lcd_command(CLEAR_DISPLAY);
		move_cursor(0,0);

		if (control_select)
		{
			sprintf((char*)lcd_buff, "  laundry: NORMAL");
			if (!laundry_power)
				WM_dcmotor_control_laudry1();
		}

		else
		{
			sprintf((char*)lcd_buff, "  laundry: STRONG");
			if (!laundry_power)
				WM_dcmotor_control_laudry2();
		}

		lcd_string(lcd_buff);
	}
}

void rinse_control(void)
{
	uint8_t lcd_buff[20];

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		control_select++;
		control_select %= 2;

		lcd_command(CLEAR_DISPLAY);
		move_cursor(0,0);

		if (control_select)
		{
			sprintf((char*)lcd_buff, "  rinse: ONCE");
			if (!rinse_power)
				WM_dcmotor_control_rinse1();
		}

		else
		{
			sprintf((char*)lcd_buff, "  rinse: TWICE");
			if (!rinse_power)
				WM_dcmotor_control_rinse2();
		}

		lcd_string(lcd_buff);
	}
}

void dry_control(void)
{
	uint8_t lcd_buff[20];

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		control_select++;
		control_select %=2;

		lcd_command(CLEAR_DISPLAY);
		move_cursor(0,0);

		if (control_select)
		{
			sprintf((char*)lcd_buff, "  dry: NORMAL");
			if (!dry_power)
				WM_dcmotor_control_dry1();
		}

		else
		{
			sprintf((char*)lcd_buff, "  dry: STRONG");
			if (!dry_power)
				WM_dcmotor_control_dry2();
		}

		lcd_string(lcd_buff);
	}
}

void WM_dcmotor_control_auto(void)		// 자동
{
	if (!pwm_start_flag)
	{
		pwm_start_flag = 1;
		buzzer_start();
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
	}

}

void WM_dcmotor_control_laudry1(void)		// 표준 세탁
{
	if (!pwm_start_flag)
	{
		pwm_start_flag = 1;
		buzzer_start();
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
	}
}

void WM_dcmotor_control_laudry2(void)		// 강력 세탁
{
	if (!pwm_start_flag)
	{
		pwm_start_flag = 1;
		buzzer_start();
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
	}
}

void WM_dcmotor_control_rinse1(void)		// 1회 = 5초
{
	if (!pwm_start_flag)
	{
		pwm_start_flag = 1;
		buzzer_start();
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
	}
}

void WM_dcmotor_control_rinse2(void)		// 2회 = 10초
{
	if (!pwm_start_flag)
	{
		pwm_start_flag = 1;
		buzzer_start();
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
	}
}

void WM_dcmotor_control_dry1(void)		// 표준 헹굼
{
	if (!pwm_start_flag)
	{
		pwm_start_flag = 1;
		buzzer_start();
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
	}
}

void WM_dcmotor_control_dry2(void)		// 강력 헹굼
{
	if (!pwm_start_flag)
	{
		pwm_start_flag = 1;
		buzzer_start();
		HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
	}
}



#if 0  // um
	char btn_buff[40];
	// start/stop
	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
//		HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);

		if (!pwm_start_flag)
		{
			pwm_start_flag = 1;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
		else
		{
			pwm_start_flag = 0;
			HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}
	}

	// 세탁 : LED1 toggle
	if (get_button(BUTTON1_GPIO_Port, BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);		// LD1
		lcd_command(CLEAR_DISPLAY);
		move_cursor(0,0);
		sprintf(btn_buff, "  laundry.");
		lcd_string(btn_buff);

		CCR_UP_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_UP_Value += 10;
		if (CCR_UP_Value > 100)
		{
			CCR_UP_Value = 100;
		}
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_UP_Value);
	}

	// 헹굼 : LED2 toggle
	if (get_button(BUTTON2_GPIO_Port, BUTTON2_Pin, 2) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);		// LD2
		lcd_command(CLEAR_DISPLAY);
		move_cursor(0,0);
		sprintf(btn_buff, "  rinse.");
		lcd_string(btn_buff);

		CCR_DOWN_Value = __HAL_TIM_GET_COMPARE(&htim4, TIM_CHANNEL_1);
		CCR_DOWN_Value -= 10;
		if (CCR_DOWN_Value < 60)
		{
			CCR_DOWN_Value = 60;
		}
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_DOWN_Value);
	}

	// 탈수 : LED3 toggle
	if (get_button(BUTTON3_GPIO_Port, BUTTON3_Pin, 3) == BUTTON_PRESS)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);		// LD3
		lcd_command(CLEAR_DISPLAY);
		move_cursor(0,0);
		sprintf(btn_buff, "  dehydration.");
		lcd_string(btn_buff);

		if (!forward_backward_dcmotor)
		{
			forward_backward_dcmotor = 1;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 0);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 1);
		}
		else
		{
			forward_backward_dcmotor = 0;
			HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, 1);
			HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9, 0);
		}
	}
#endif

