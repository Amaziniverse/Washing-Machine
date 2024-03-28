//서브 모터 동작
#include "servo_motor.h"

extern TIM_HandleTypeDef htim2;

void servo_motor_test_main(void);
// 8400000HZ / 1680 = 50000HZ
// T = 1/f = 1/50000 = 0.00002sec = 20microsec
// 2ms(180도 회전) : 0.00002 * 1000
// 1.5ms(90도 회전) : 0.00002 * 750
// 1ms (0도) : 0.00002 * 500
void servo_motor_test_main(void)
{
	while (1)
	{
		// 180도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 180);
		HAL_Delay(1000);		// 1초 유지

		// 90도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90);
		HAL_Delay(1000);		// 1초 유지

		// 0도 회전
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 10);
		HAL_Delay(1000);		// 1초 유지
	}
}
