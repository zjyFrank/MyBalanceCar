#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "main.h"
#include "tim.h"
#include "mpu9250.h"
#include "moto_ctrl.h"
#include "balance_car.h"

#define IR0	     0x0000//0000000000000000
#define IR1      0x0004//0000000000000100
#define IR2      0x8000//1000000000000000
#define IR3      0x0020//0000000000100000
#define IR4		 0x0008//0000000000001000
#define IR5		 0x0010//0000000000010000
#define IR12	 0x4004//0100000000000100
#define IR23	 0x4020//0100000000100000
#define IR34	 0x0028//0000000000101000
#define IR45	 0x0018//0000000000011000
#define IR123	 0x8024//1000000000100100
#define IR234	 0x8028//1000000000101000
#define IR345	 0x0038//0000000000111000
#define IR1234	 0x802c//1000000000101100
#define IR2345	 0x8038//1000000000111000
#define IR12345	 0x803c//1000000000111100


int my_angle;
int16_t PWM ;
float ctrl ;
extern uint8_t direction;
int i_stop;
int pwm_Velocity;
int pwm_Turn;
int pwm_xunji;
float angle;
float Movement;
float kp=8,ki=0.04;
int velocity(int encoder_left,int encoder_right)
{
	static float Velocity,Encoder_Least,Encoder;
	static float Encoder_Integral;
	
	
	if(direction=='1') Movement=5; //===如果前进标志位置 1 位移为负
	else if(direction=='2') Movement=-5; //===如果后退标志位置 1 位移为正
	else if(direction=='0'){Movement=0;pwm_xunji=0;}
	Encoder_Least =(encoder_left+encoder_right)-Movement; //获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零）
	Encoder *= 0.8; //===一阶低通滤波器
	Encoder += Encoder_Least*0.2; //===一阶低通滤波器
	Encoder_Integral +=Encoder; //===积分出位移 积分时间：10ms
	Encoder_Integral=Encoder_Integral-Movement; //接收遥控器数据，控制前进后退
	if(Encoder_Integral>1000) Encoder_Integral=1000; //积分限幅
	if(Encoder_Integral<-1000) Encoder_Integral=-1000; //限制遥控最大速度
	Velocity=Encoder*kp+Encoder_Integral*ki; //===速度控制
	if(direction==0) Encoder_Integral=0; //电机关闭后清除积分
	return Velocity;
}

int turn(int encoder_left,int encoder_right,float gyro)
{
	float Turn_Amplitude=0;
	float Turn,Kp=0.02,Bias;
	if(direction=='3') angle=100;
	else if(direction=='4') angle=-100;
	else angle=0;
	Bias=gyro-angle;
	Turn=-Bias*Kp;
	Turn+=angle;
	return Turn;
}

void xunji(void)
{

	
	if(g_moto_ctrl.ir_sensor ==0x0001) {pwm_xunji=250;}       //00001       IR5-IR1           白色0 黑色1
	else if(g_moto_ctrl.ir_sensor ==0x0003) {pwm_xunji=160;}  
	else if(g_moto_ctrl.ir_sensor ==0x0007) {pwm_xunji=130;}
	else if(g_moto_ctrl.ir_sensor ==0x000F) {pwm_xunji=100;} 
	else if(g_moto_ctrl.ir_sensor ==0x000E) {pwm_xunji=0;Movement=13;}
	else if(g_moto_ctrl.ir_sensor ==0x001E) {pwm_xunji=-100;} 
	else if(g_moto_ctrl.ir_sensor ==0x001C) {pwm_xunji=-130;} 
	else if(g_moto_ctrl.ir_sensor ==0x0018) {pwm_xunji=-160;} 
	else if(g_moto_ctrl.ir_sensor ==0x0010) {pwm_xunji=-250;} 
	else if(g_moto_ctrl.ir_sensor ==0x0006) {pwm_xunji=130;}  
	else if(g_moto_ctrl.ir_sensor ==0x000C) {pwm_xunji=-130;} 
	else if(g_moto_ctrl.ir_sensor ==0x0002) {pwm_xunji=160;} 
	else if(g_moto_ctrl.ir_sensor ==0x0004) {pwm_xunji=0;Movement=13;}
	else if(g_moto_ctrl.ir_sensor ==0x0008) {pwm_xunji=-160;} 

//		case 0x0003: midspeedleft();break;        //00011
//		case 0x0007: lowspeedleft();break;       //00111
//		case 0x000F: lowspeedleft();break;				//01111
//		case 0x000E: backwardfortrace();break;						  //01110
//		case 0x001E:  lowspeedright();break;      //11110
//		case 0x001C:	lowspeedright();break;		  //11100
//		case 0x0018: midspeedright();break;				//11000
//		case 0x0010: highspeedright();break;			//10000
//case 0x0006: backwardfortrace();break;             //00110                //2
//		case 0x000C: backwardfortrace();break;            //01100
//		
//		
//		case 0x0002: backwardfortrace();break;       //00010                    //1
//		case 0x0004: backwardfortrace();break;            //00100
//		case 0x0008: backwardfortrace();break;			 //01000
//    if(g_moto_ctrl.ir_sensor ==(IR1|IR2|IR12|IR23|IR234|IR1234) )
//	{
//		left_forward(800);
//		right_forward(0);
//	}
//	else if(g_moto_ctrl.ir_sensor ==(IR4|IR5|IR34|IR45|IR345|IR2345))
//	{
//		direction=4;
//	}
//	else if(g_moto_ctrl.ir_sensor ==(IR3|IR234|IR12345))
//	{
//		direction=1;
//	}
//	
//	else if(g_moto_ctrl.ir_sensor==IR0)
//	{
//		int a;
//	}
}
moto_ctrl_t g_moto_ctrl ;
void Moto_Balance_PID_Ctrl( )
{
	float KP = 11; 
    float KI = 0 ; 
    float KD = 0.85;
	const int16_t MOTO_THRESHOLD = 550 ;  // PWM = 18KHz

  static float angle_D = 0 ;
  static float angle_D_last = 0 ;

	
	float angle_diff = 0 ;
	int16_t dir ;
	
	
	angle_diff += g_mpu9250.Angle_Complement_1st  ;
	pwm_Velocity = velocity(SpeedData.LeftSpeed,SpeedData.RightSpeed);
	pwm_Turn = turn(SpeedData.LeftSpeed,SpeedData.RightSpeed,g_mpu9250.gyro_z);


	ctrl = g_mpu9250.Angle_Complement_1st   * KP 
	        + angle_diff * KI 
	        + g_mpu9250.gyro_scale_y * KD ;

  PWM =  MOTO_THRESHOLD + abs((int)ctrl  - (int)pwm_Velocity );

  g_moto_ctrl.right_pwm = PWM ;
  

  if ( ((int)ctrl  - (int)pwm_Velocity) <0 )
  {
    left_forward( PWM -(int)pwm_Turn-pwm_xunji);
    right_forward( PWM+(int)pwm_Turn +pwm_xunji);
  }
  else
  {
    left_backward( PWM+(int)pwm_Turn );
    right_backward( PWM -(int)pwm_Turn);
  }
//if ( 550+(int)pwm_Turn <0 )
//  {
//    left_forward( 550+(int)pwm_Turn);
//    right_forward( 550-(int)pwm_Turn );
//  }
//  else
//  {
//    left_backward( 550+ (int)pwm_Turn );
//    right_backward(550-(int)pwm_Turn);
//  }

 if(direction=='5') xunji();
  
}



void Moto_Ctrl_Init( void ) 
{
	memset( &g_moto_ctrl , 0 , sizeof( moto_ctrl_t ) );
}

void right_backward(uint16_t value)
{		
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);
	
		HAL_GPIO_WritePin(MT1_A_GPIO_Port, MT1_A_Pin, GPIO_PIN_SET);		
		HAL_GPIO_WritePin(MT1_B_GPIO_Port, MT1_B_Pin, GPIO_PIN_RESET);
			
		TIM_SetCompare4(TIM8, value);
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);	
}

void right_forward(uint16_t value)
{
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);
		HAL_GPIO_WritePin(MT1_A_GPIO_Port, MT1_A_Pin, GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(MT1_B_GPIO_Port, MT1_B_Pin, GPIO_PIN_SET);
		
		TIM_SetCompare4(TIM8, value);		
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
}

void left_backward(uint16_t value)
{
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(MT2_B_GPIO_Port, MT2_B_Pin, GPIO_PIN_SET);		
		HAL_GPIO_WritePin(MT2_A_GPIO_Port, MT2_A_Pin, GPIO_PIN_RESET);
				
	  TIM_SetCompare3(TIM8, value);		
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);	
}

void left_forward(uint16_t value)
{
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
		HAL_GPIO_WritePin(MT2_B_GPIO_Port, MT2_B_Pin, GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(MT2_A_GPIO_Port, MT2_A_Pin, GPIO_PIN_SET);
					
		TIM_SetCompare3(TIM8, value);
		HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);	
}

void Motor_Stop(void)
{
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_3);
		HAL_TIM_PWM_Stop(&htim8, TIM_CHANNEL_4);
		HAL_GPIO_WritePin(MT1_A_GPIO_Port, MT2_A_Pin, GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(MT1_B_GPIO_Port, MT2_B_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MT2_A_GPIO_Port, MT2_A_Pin, GPIO_PIN_RESET);		
		HAL_GPIO_WritePin(MT2_B_GPIO_Port, MT2_B_Pin, GPIO_PIN_RESET);
}


void TIM_SetCompare3(TIM_TypeDef* TIMx, uint16_t Compare)
{
  /* Check the parameters */
  //assert_param(IS_TIM_LIST8_PERIPH(TIMx));
  /* Set the Capture Compare1 Register value */
  TIMx->CCR3 = Compare;
}

void TIM_SetCompare4(TIM_TypeDef* TIMx, uint16_t Compare)
{
  /* Check the parameters */
  //assert_param(IS_TIM_LIST6_PERIPH(TIMx));
  /* Set the Capture Compare2 Register value */
  TIMx->CCR4 = Compare;
}

/////////////////////////////////////////////////////////////////

