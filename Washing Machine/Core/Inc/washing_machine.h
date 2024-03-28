#include "main.h"
/*
#include "i2c_lcd.h"
#include "button.h"
#include "dcmotor.h"
//#include "servo_motor.h"
#include "internal_rtc.h"
#include "buzzer.h"
#include "fnd4digit.h"
#include "ultrasonic.h"
*/

// 1. 메인
// 2. lcd display
// 3. button
// 4. 메인 모터
// 5. 서브 모터, 안 써서 주석 처리
// 6. 날짜 시간 카운트
// 7. 버저 = 사운드
// 8. fnd display
// 9. 거리 감지

//#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdio.h>

#include "led.h"
//dcmotor 에서 버튼에 따라
//크고 끄는 기능 때문에
