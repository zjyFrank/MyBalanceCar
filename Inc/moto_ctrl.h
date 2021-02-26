#ifndef _MOTO_CTRL_H
#define _MOTO_CTRL_H
#include "stm32f1xx_hal.h"

#define IR_NO_LINE     0         // 0000 0000
#define IR_ALL_DARK    174       // 1010 1110
#define IR_M_ONE       4         // 0000 0100
#define IR_MR_ONE      8         // 0000 1000
#define IR_R_ONE       2         // 0000 0010
#define IR_ML_ONE      128       // 1000 0000
#define IR_L_ONE       32        // 0010 0000

#define IR_MR_TWO      12        // 0000 1100
#define IR_R_TWO       10        // 0000 1010
#define IR_ML_TWO      132       // 1000 0100
#define IR_L_TWO       160       // 1010 0000

typedef struct {
	  uint16_t ir_sensor;
    int16_t left_pwm ;
    int16_t right_pwm ;
} moto_ctrl_t ;	


void Moto_Balance_PID_Ctrl( void );

void SpeedCaptureTimeOut( int32_t channel );
void Moto_Ctrl_Init( void ) ;
void Motor_Stop(void);
void left_forward(uint16_t value);
void left_backward(uint16_t value);
void right_forward(uint16_t value);
void right_backward(uint16_t value);

void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare);
void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare);

void Speed_Driection_Calculate( int32_t channel  , int32_t start1 , int32_t end1 , int32_t start2 , int32_t end2 );

extern moto_ctrl_t g_moto_ctrl ;

#endif
