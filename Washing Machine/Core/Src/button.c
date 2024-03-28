//버튼 제어
#include "button.h"
#include "i2c_lcd.h"

int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);

// first state of button table... for reset BUTTON
char button_status[BUTTON_NUMBER] = {BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE, BUTTON_RELEASE};

extern RTC_HandleTypeDef hrtc;
extern void lcd_string(uint8_t *str);
extern void WM_dcmotor_control_auto(void);

RTC_TimeTypeDef mTime;

uint8_t lcd_display_mode_flag = 0;
uint8_t auto_power = 0;

/* original code
void lcd_display_mode_select(void)
{
	char lcd_buff[40];
	if (get_button(GPIOC, GPIO_PIN_13, 4) == BUTTON_PRESS)
	{
		lcd_command(CLEAR_DISPLAY);
		lcd_display_mode_flag++;
		lcd_display_mode_flag %= 4;

		if (lcd_display_mode_flag == 3)
		{
			HAL_RTC_GetTime(&hrtc, &mTime, RTC_FORMAT_BCD);
			sprintf(lcd_buff, "TIME:%02d:%02d:%02d",
					bin2dec(mTime.Hours), bin2dec(mTime.Minutes), bin2dec(mTime.Seconds));
			move_cursor(1,0);
			lcd_string(lcd_buff);
			move_cursor(1,6);		// 시간 정보 필드로 커서를 이동
		}
	}
} */


void WM_lcd_display(void)
{
	uint8_t lcd_buff[40];

	if (get_button(GPIOC, GPIO_PIN_13, 4) == BUTTON_PRESS)
	{
		lcd_command(CLEAR_DISPLAY);
		lcd_display_mode_flag++;
		lcd_display_mode_flag %= 3;			// 이 변수가 1 아니면 2 아니면 0

		if (lcd_display_mode_flag == 1)			// 0이 아닐 때
		{
			lcd_command(CLEAR_DISPLAY);
			move_cursor(0,0);
			sprintf((char*) lcd_buff, " Select.");
			lcd_string(lcd_buff);

			move_cursor(1,0);
			sprintf((char*) lcd_buff, "1.Auto 2.Menu");
			lcd_string(lcd_buff);
		}
		else if (lcd_display_mode_flag == 2)
		{
			lcd_command(CLEAR_DISPLAY);
			move_cursor(0,0);
			sprintf((char*) lcd_buff, " Auto Mode.");
			lcd_string(lcd_buff);

			move_cursor(1,0);
			sprintf((char*) lcd_buff, "Press BTN0.");
			lcd_string(lcd_buff);

			if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
				WM_dcmotor_control_auto();
		}
		else
		{
			lcd_command(CLEAR_DISPLAY);
			move_cursor(0,0);
			sprintf((char*) lcd_buff, " Select Menu.");
			lcd_string(lcd_buff);
		}
	}
}


// get_button(gpio, pin, button_num) for check press button or not
// totally press and release, than return BUTTON_RELEASE(1)
int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number)
{
	unsigned char curr_state;

	curr_state = HAL_GPIO_ReadPin(GPIO, GPIO_PIN);		// 0 or 1
	// now button gets press, but first state

	if (curr_state == BUTTON_PRESS && button_status[button_number] == BUTTON_RELEASE)
	{
		HAL_Delay(80);
		button_status[button_number] = BUTTON_PRESS;
		return BUTTON_RELEASE;		// button gets press, but for now admit noise state
	}
	else if (curr_state == BUTTON_RELEASE && button_status[button_number] == BUTTON_PRESS)
	// before button's state and also release button now,
	{
		button_status[button_number] = BUTTON_RELEASE;		// reset button_status table
		HAL_Delay(30);
		return BUTTON_PRESS;		// 1. admit pressing and releasing button once
	}
	return BUTTON_RELEASE;
}



/*
 // um
	uint8_t  lcd_buff[40];

	if (get_button(GPIOC, GPIO_PIN_13, 4) == BUTTON_PRESS)
	{
		lcd_command(CLEAR_DISPLAY);
		lcd_display_mode_flag++;
		lcd_display_mode_flag %= 4;

		if (lcd_display_mode_flag == 1)
		{
			move_cursor(0,0);
			sprintf( (char *) lcd_buff, "  Hello.");
			lcd_string(lcd_buff);
			HAL_Delay(2000);
			sprintf((char*) lcd_buff, "Select menu.");
			move_cursor(1,0);
			lcd_string(lcd_buff);
			HAL_Delay(3000);

			lcd_command(CLEAR_DISPLAY);

			move_cursor(0,0);
			sprintf((char*) lcd_buff, "  1.laundry");
			lcd_string(lcd_buff);
			move_cursor(1,0);
			sprintf((char*) lcd_buff, "2.rinse");
			lcd_string(lcd_buff);
			HAL_Delay(3000);

			lcd_command(CLEAR_DISPLAY);

			move_cursor(0,0);
			sprintf((char*) lcd_buff, "  3.dehydration");
			lcd_string(lcd_buff);
			HAL_Delay(3000);

			lcd_command(CLEAR_DISPLAY);

			move_cursor(0,0);
			sprintf(lcd_buff, "  Button 1, 2, 3");
			lcd_string(lcd_buff);
			move_cursor(1,0);
			sprintf(lcd_buff, "Button 0.Stop");
			lcd_string(lcd_buff);
		}

		if (lcd_display_mode_flag == 3)
		{
			lcd_command(CLEAR_DISPLAY);
			//모터도 끄고, 부저도 끄고, fnd도 끄고
			dcmotor_pwm_init();

			move_cursor(0,0);
			sprintf(lcd_buff, " Good bye.");
			lcd_string(lcd_buff);
			HAL_Delay(4000);

			lcd_command(CLEAR_DISPLAY);
			lcd_display_mode_flag = 0;
		}
	}
#endif
}
 */
