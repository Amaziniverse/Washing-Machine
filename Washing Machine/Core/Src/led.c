#include "led.h"
#include "button.h"

#define LED_ALL_UP 0
#define LED_ALL_DOWN 1
#define FLOWER_ON 2
#define FLOWER_OFF 3
#define LED_KEEPON_UP 4
#define LED_KEEPON_DOWN 5
#define LED_ON_UP 6
#define LED_ON_DOWN 7

void led_main(void);
void led_all_on(void);
void led_all_off(void);
void led_on_up(void);
void led_on_down(void);
void led_keepon_up(void);
void led_keepon_down(void);
void led_gradation(void);
void led_flower_on(void);
void led_flower_off(void);

void button0_led_all_on_off_toggle(void);
void button0_toggle(void);
void button01_led_bar(void);

extern volatile int tlms_counter;		// volatile : disable optimize

int func_index = 0;


void button0_led_all_on_off_toggle(void)
{
	static int button0_count = 0;		// declare variable as static, it works like local variable

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 2;
		if (button0_count)
		{
			led_all_on();
		}
		else
		{
			led_all_off();
		}
	}
}

void button0_toggle()
{
	// press once : turn on all led
	// press twice : turn off all led
	// press third : turn on flower
	// press fourth : turn of flower
	// press fifth : led_keepon
	// press sixth : led_keepdown
	// press seventh : go to press once action, again

	static int button0_count = 0;

	void (*ButtonFunc[]) () =
	{
			NULL, led_all_on, led_all_off, led_flower_on, led_flower_off, led_on_up, led_on_down
	};

	if (get_button(BUTTON0_GPIO_Port, BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		button0_count++;
		button0_count %= 7;

		if (button0_count == 0)
			button0_count++;

		ButtonFunc[button0_count] ();

#if 0
		if (button0_count % 6 == 1)
			led_all_on();
		if (button0_count % 6 == 2)
			led_all_off();
		if (button0_count % 6 == 3)
			led_flower_on();
		if (button0_count % 6 == 4)
			led_flower_off();
		if (button0_count % 6 == 5)
			led_on_up();
		if (button0_count % 6 == 0)
			led_on_down();
#endif
	}
}

void button01_led_bar()
{
	static int button_count = 0;

	if (get_button(BUTTON0_GPIO_Port,BUTTON0_Pin, 0) == BUTTON_PRESS)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<button_count, 1);
		button_count++;
	}

	if (get_button(BUTTON1_GPIO_Port,BUTTON1_Pin, 1) == BUTTON_PRESS)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01<<button_count, 0);
		button_count--;
	}

	if (button_count < 0)
		button_count = 0;
}


void led_main(void)
{
	while(1)
	{
		if (tlms_counter >= 200)
		{
		}
#if 0
		if (tlms_counter >= 200)
		{
			tlms_counter = 0;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		}
#endif

#if 0
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
		HAL_Delay(200);
#endif

#if 0
		button0_led_all_on_off_toggle();
		button0_toggle();
#endif

#if 0
		// Phase#5
		led_flower_on();
		led_flower_off();
#endif

#if 0
		// Phase#4
		led_keepon_up();
		led_all_off();
		led_keepon_down();
		led_all_off();
#endif

#if 0
		// Phase#3
		switch (func_index)
		{
		case 0 : LED_ON_UP;
			led_on_up();
			break;
		case 1 : LED_ON_DOWN;
			led_on_down();
			break;
		default :
			break;
		}
//		led_on_up();
//		led_on_down();
#endif

#if 0
		// Phase#2
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_ALL);
		HAL_Delay(500);
#endif

#if 0
		// Phase#1
		led_all_on();
		HAL_Delay(300);
		led_all_off();
		HAL_Delay(300);
#endif
	}
}

void led_all_on(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 1);
	HAL_GPIO_WritePin(GPIOD, 0xff, 1);
}

void led_all_off(void)
{
//	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, 0);
	HAL_GPIO_WritePin(GPIOD, 0xff, 0);
}

void led_on_up(void)	//0 1 2 3 4 5 6 7
{
	static int i = 0;


	for (i = 0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}

#if 1		//original
#else
	if (t1ms_count >= 200)
	{
		t1ms_count = 0;
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;
		if (i >= 8)
		{
			i = 0;
			func_index = LED_ON_DOWN;
		}
	}
#endif
}

void led_on_down(void)		//7 6 5 4 3 2 1 0
{
	static int i = 0;


	for (int i = 0; i < 8; i++)
	{
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200);
	}
#if 1		//original
#else
	if (t1ms_count >= 200)
	{
		t1ms_count = 0;
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		i++;
		if (i >= 8)
		{
			i = 0;
			func_index = LED_ON_UP;
		}
	}
#endif
}

void led_keepon_up(void)
{
	static int i = 0;


	led_all_off();
	for (int i=0 ; i<8 ; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << i, 1);
		HAL_Delay(200);
	}
#if 1		//original
#else
	if (t1ms_count >= 200)
	{
		t1ms_count = 0;
		led_all_off();
		HAL_GPIO_WritePin(GPIOD, 0X01 << i, 1);
		i++;
		if (i >= 8)
		{

		}
	}
#endif
}

void led_keepon_down(void)
{
	for (int i=0 ; i<8 ; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x80 >> i, 1);
		HAL_Delay(200);
	}
}

void led_flower_on(void)
{
	int i, delay = 100;

	led_all_off();

	for (i=0 ; i<4 ; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << (4 + i), 1);
		HAL_GPIO_WritePin(GPIOD, 0x01 << (3 - i), 1);
		HAL_Delay(delay);
		delay += 100;
	}
}

void led_flower_off(void)
{
	int i, delay = 400;

	led_all_on();

	for (i=0 ; i<4 ; i++)
	{
		HAL_GPIO_WritePin(GPIOD, 0x01 << (0 + i), 0);
		HAL_GPIO_WritePin(GPIOD, 0x01 << (7 - i), 0);
		HAL_Delay(delay);
		delay -= 100;
	}
}

void led_gradation(void)
{
	uint16_t temp[8] = {0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080};
	int i;

	i = 0;
	while(1)
	{
		HAL_GPIO_WritePin(GPIOD, temp[i], 1);
		HAL_Delay(200);
		HAL_GPIO_WritePin(GPIOD, temp[i], 0);
		i++;
		if (i == 8)
		{
			i = 0;
		}
	}
}
