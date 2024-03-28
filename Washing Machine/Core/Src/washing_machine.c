//세탁기 동작
#include "washing_machine.h"

// 헤더 파일로 포함시켜 놓아서 따로 extern 선언 안 해도 될 줄 알았는데, 하지 않으면 warning이 뜨더라
//extern int get_button(GPIO_TypeDef *GPIO, uint16_t GPIO_PIN, uint8_t button_number);
extern void WM_lcd_display(void);
extern void WM_dcmotor_control(void);
extern void WM_fnd4digit_sec_clock(void);
extern void WM_ultrasonic_processing(void);
extern void dcmotor_pwm_control(void);

extern uint8_t lcd_display_mode_flag;

void washing_machine_main(void)
{
	WM_lcd_display();
	WM_dcmotor_control();
//	WM_fnd4digit_sec_clock();
	WM_ultrasonic_processing();
//	dcmotor_pwm_control();
}
