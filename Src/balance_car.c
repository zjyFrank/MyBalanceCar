#include <stdio.h>
#include "main.h"
#include "balance_car.h"
#include "usart.h"
#include "tim.h"
#include "spi.h"
#include "gpio.h"
#include "mpu9250.h"
#include "uart_osc.h"
#include "moto_ctrl.h"
#include "dot_matrix.h"
#include "uart_dma.h"

#define SPEED_CNT_FULL   10000
#define SPEED_CNT_INIT   30000

SpeedDataTypeDef SpeedData;
uint32_t g_SysMode = 1 ;  //启动模式选择

int fputc( int ch , FILE * stream )
{
	HAL_UART_Transmit( &huart4 , (uint8_t *)&ch , 1 , HAL_MAX_DELAY); 
	return ch;
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Speed sensors decode by hardware                                         //
// Tim2 MT1 sensor , Tim3 MT2 sensor                                        //
// Tim7 calculate speed and distance by 100ms                               //
//////////////////////////////////////////////////////////////////////////////

void InitSpeedAndDistance( void )
{
	SpeedData.LeftSpeed = 0;
	SpeedData.RightSpeed = 0;
	SpeedData.LeftDistance = 0;
	SpeedData.RightDistance = 0;
}

void StartMeasureSpeed( void )
{	
	InitSpeedAndDistance();
	__HAL_TIM_SET_COUNTER(&htim1,SPEED_CNT_INIT);
	__HAL_TIM_SET_COUNTER(&htim3,SPEED_CNT_INIT);
	
	HAL_TIM_Encoder_Start(&htim1,TIM_CHANNEL_ALL);	
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);	
	HAL_TIM_Base_Start_IT(&htim7);
}

void StopMeasureSpeed( void )
{	
	InitSpeedAndDistance();
	HAL_TIM_Base_Stop_IT(&htim7);

	__HAL_TIM_SET_COUNTER(&htim1,SPEED_CNT_INIT);
	__HAL_TIM_SET_COUNTER(&htim3,SPEED_CNT_INIT);
	
	HAL_TIM_Encoder_Stop(&htim1,TIM_CHANNEL_ALL);	
	HAL_TIM_Encoder_Stop(&htim3,TIM_CHANNEL_ALL);	
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint16_t left , right ;

  if ( &htim7 == htim ) //1s
	{
		right  = __HAL_TIM_GET_COUNTER(&htim1);
		left   = __HAL_TIM_GET_COUNTER(&htim3);
		__HAL_TIM_SET_COUNTER(&htim1,SPEED_CNT_INIT );
		__HAL_TIM_SET_COUNTER(&htim3,SPEED_CNT_INIT );
		
		SpeedData.LeftSpeed =  ( SPEED_CNT_INIT - left ) *0.93369 ; // D=68mm , 3.1415*68/4/13/4.4
    SpeedData.LeftDistance += SpeedData.LeftSpeed;
// 		printf("left=%d , " , left );		
// 		printf("right=%d\n" , right );		
		
		SpeedData.RightSpeed =  ( SPEED_CNT_INIT - right ) *0.93369 ;
    SpeedData.RightDistance += SpeedData.RightSpeed;
		printf("IR=%x\n",g_moto_ctrl.ir_sensor);
//	  printf("L=%d,D=%d;R=%d,D=%d\n", 
//		SpeedData.LeftSpeed , SpeedData.LeftDistance , SpeedData.RightSpeed , SpeedData.RightDistance);
	}
	else if ( &htim6 == htim ) //5ms 72000/7200/50 = 200Hz
	{
    SystemTimer1msCallback();
	}
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Data process                                                             //
// Kalman_Filter                                                            //
// Complementary_Filter                                                     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

void Kalman_Filter(float angle, float gyro)  
{ 
	static const float Q_angle=0.001;  
	static const float Q_gyro=0.003;
	static const char  C_0 = 1;
	static const float R_angle=0.5;

	static float Q_bias, angle_err;
	static float PCt_0, PCt_1, E;
	static float K_0, K_1, t_0, t_1;
	static float Pdot[4] ={0,0,0,0};
	static float PP[2][2] = { { 1, 0 },{ 0, 1 } };
  
	g_mpu9250.Angle_Kalman += (gyro - Q_bias) * DeltaTime;       
      
  Pdot[0] = Q_angle - PP[0][1] - PP[1][0];     
  Pdot[1] = -PP[1][1];  
  Pdot[2] = -PP[1][1];  
  Pdot[3] = Q_gyro;  
      
  PP[0][0] += Pdot[0] * DeltaTime;   
  PP[0][1] += Pdot[1] * DeltaTime;     
  PP[1][0] += Pdot[2] * DeltaTime;  
  PP[1][1] += Pdot[3] * DeltaTime;  
      
  angle_err = angle - g_mpu9250.Angle_Kalman ;   
      
  PCt_0 = C_0 * PP[0][0];  
  PCt_1 = C_0 * PP[1][0];  
      
  E = R_angle + C_0 * PCt_0;  
      
  K_0 = PCt_0 / E;  
  K_1 = PCt_1 / E;  
      
  t_0 = PCt_0;  
  t_1 = C_0 * PP[0][1];  
      
  PP[0][0] -= K_0 * t_0;     
  PP[0][1] -= K_0 * t_1;  
  PP[1][0] -= K_1 * t_0;  
  PP[1][1] -= K_1 * t_1;  
      
  g_mpu9250.Angle_Kalman += K_0 * angle_err; 
  Q_bias  += K_1 * angle_err;   
  g_mpu9250.Gyro_Kalman = gyro - Q_bias ;   
}  

void Complementary_Filter_1st_Order (float angle, float gyro )  
{  
    const float K1 =0.01;
    g_mpu9250.Angle_Complement_1st = K1 * angle + ( 1-K1 ) * ( g_mpu9250.Angle_Complement_1st + gyro * DeltaTime );  
}  

void Complementary_Filter_2nd_Order(float angle, float gyro )  
{  
    const float K1 =0.2;  
    float x1,x2,y1;
	
    x1 = ( angle - g_mpu9250.Angle_Complement_2nd ) * ( 1 - K1 ) * ( 1 - K1 ) ;  
    y1 = y1 + x1 * DeltaTime ;  
    x2 = y1 + 2 * (1-K1) * ( angle - g_mpu9250.Angle_Complement_2nd ) + gyro ;   
    g_mpu9250.Angle_Complement_2nd = g_mpu9250.Angle_Complement_2nd + x2 * DeltaTime;  
}  

void Complementary_Filter (float angle, float gyro )
{
   g_mpu9250.Angle_Complement_1st += ( ( angle - g_mpu9250.Angle_Complement_1st) * 0.3 + gyro ) * 0.01 ;  
}


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// common process                                                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////
uint32_t ReadUserButton0( void )
{
	static uint8_t btn0_down = 0 ;
	static uint8_t btn0_up = 1 ;
	if ( HAL_GPIO_ReadPin( USER_PB0_GPIO_Port , USER_PB0_Pin ) == GPIO_PIN_RESET )
	{
		if ( btn0_down == 1 && btn0_up == 1 )
		{
			btn0_up = 0 ;
			return 1 ;
		}
		else
		{
			btn0_down = 1 ;
    	HAL_Delay( 50 );
		}
	}
	else
	{
		btn0_down = 0;
		btn0_up = 1 ;
	}
	return 0 ;
}

uint32_t ReadUserButton1( void )
{
	static uint8_t btn1_down = 0 ;
	static uint8_t btn1_up = 1 ;
	if ( HAL_GPIO_ReadPin( USER_PB1_GPIO_Port , USER_PB1_Pin ) == GPIO_PIN_RESET )
	{
		if ( btn1_down == 1 && btn1_up == 1 )
		{
			btn1_up = 0 ;
			return 1 ;
		}
		else
		{
			btn1_down = 1 ;
    	HAL_Delay( 50 );
		}
	}
	else
	{
		btn1_down = 0;
		btn1_up = 1 ;
	}
	return 0 ;
}

// index 0 : all , 1~4 : LED1~4 
// mode 0 : off , 1 : on , 2 : toggle
void LEDCtrl( uint32_t index , uint32_t mode )
{
	 switch( index )
	 {
		 case 0 :
			 if ( mode ==  0 ) 
			 {
  				HAL_GPIO_WritePin( LED1_GPIO_Port , LED1_Pin, GPIO_PIN_SET );
  				HAL_GPIO_WritePin( LED2_GPIO_Port , LED2_Pin, GPIO_PIN_SET );
			 }
			 else if ( mode ==  1 )
			 {
  				HAL_GPIO_WritePin( LED1_GPIO_Port , LED1_Pin, GPIO_PIN_RESET );
 				  HAL_GPIO_WritePin( LED2_GPIO_Port , LED2_Pin, GPIO_PIN_RESET );
			 }
			 break;
		 case 1 :
			 if ( mode ==  0 ) 
			 {
  				HAL_GPIO_WritePin( LED1_GPIO_Port , LED1_Pin, GPIO_PIN_SET );
			 }
			 else if ( mode ==  1 )
			 {
  				HAL_GPIO_WritePin( LED1_GPIO_Port , LED1_Pin, GPIO_PIN_RESET );
			 }
			 else if ( mode ==  2 )
			 {
  				HAL_GPIO_TogglePin( LED1_GPIO_Port , LED1_Pin );
			 }
			 break;
		 case 2 :
			 if ( mode ==  0 ) 
			 {
  				HAL_GPIO_WritePin( LED2_GPIO_Port , LED2_Pin, GPIO_PIN_SET );
			 }
			 else if ( mode ==  1 )
			 {
  				HAL_GPIO_WritePin( LED2_GPIO_Port , LED2_Pin, GPIO_PIN_RESET );
			 }
			 else if ( mode ==  2 )
			 {
  				HAL_GPIO_TogglePin( LED2_GPIO_Port , LED2_Pin );
			 }
			 break;

		 default :
			 break;
	 }
}

void SleepSystem( void )
{
//  MX_GPIO_DeInit();
	
	HAL_PWR_EnterSLEEPMode( PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFE );
}

	
void StopSystem( void )
{
	HAL_PWR_EnterSTOPMode( PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI );
}

void Get_IR_Sensor( void )
{ 
	uint16_t  data1 , data2 ,data3 ,data4,data5;
//  data0 = GPIOD->IDR & 0x0004;	  //  PD2           -> IR 1
//  data1 = GPIOB->IDR & 0x0038;	  //  PB5,PB4,PB3   -> IR 3,5,4                  0011 1000
//  data2 = GPIOA->IDR & 0x8000;	  //  PA15          -> IR 2
	data1 = GPIOD->IDR & 0x0004;	  //  PD2           -> IR 1         11111 11111       0000 0100
	data2 = GPIOA->IDR & 0x8000;	  //  PA15          -> IR 2
	data3 = GPIOB->IDR & 0x0020;	  //  PB5           -> IR 3
	data4 = GPIOB->IDR & 0x0008;	  //  PB3           -> IR 4
	data5 = GPIOB->IDR & 0x0010;	  //  PB4           -> IR 5
	//g_moto_ctrl.ir_sensor = data1 | data2 | data0 ;	
	
	if(data1!=0x0000) data1=0x0001;   //0000 0001
	if(data2!=0x0000) data2=0x0002;   //0000 0010
	if(data3!=0x0000) data3=0x0004;   //0000 0100
	if(data4!=0x0000) data4=0x0008;   //0000 1000
	if(data5!=0x0000) data5=0x0010;	  //0001 0000
	g_moto_ctrl.ir_sensor= data5 | data4 | data3 | data2 | data1;
}

void IR_Sensor_Init( void )
{
  GPIO_InitTypeDef GPIO_InitStruct;

  HAL_SPI_MspDeInit( &hspi3 );
  HAL_GPIO_WritePin(IR_1_GPIO_Port, IR_1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IR_2_GPIO_Port, IR_2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IR_3_GPIO_Port, IR_3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IR_4_GPIO_Port, IR_4_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(IR_5_GPIO_Port, IR_5_Pin, GPIO_PIN_RESET);
	
  HAL_GPIO_DeInit(IR_3_GPIO_Port, IR_3_Pin);
  HAL_GPIO_DeInit(IR_4_GPIO_Port, IR_4_Pin);
  HAL_GPIO_DeInit(IR_5_GPIO_Port, IR_5_Pin);

  /*Configure GPIO pins : PBPin PBPin */
  GPIO_InitStruct.Pin = IR_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IR_1_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = IR_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IR_2_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = IR_3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IR_3_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = IR_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IR_4_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = IR_5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IR_5_GPIO_Port, &GPIO_InitStruct);
	
}

void SetWorkMode( uint32_t mode)
{
		switch ( mode )
		{
			case 0 :   // balance control
				LEDCtrl( 1 , 0 );
				LEDCtrl( 2 , 1 );
				
				HAL_TIM_PWM_MspInit(&htim8);
				HAL_TIM_Base_Start_IT(&htim6);
        StartMeasureSpeed();
			  IR_Sensor_Init();
				break;
			case 1 :   // dot matrix
				LEDCtrl( 1 , 1 );
				LEDCtrl( 2 , 0 );
        StartMeasureSpeed();
				HAL_TIM_Base_Stop_IT(&htim6);
				Motor_Stop();
				HAL_TIM_PWM_MspDeInit(&htim8);
				dot_matrix_init();
				break ;
			case 2 :   // show osc wave
				LEDCtrl( 1 , 0 );
				LEDCtrl( 2 , 1 );
				Motor_Stop();
				HAL_TIM_PWM_MspDeInit(&htim8);
				IR_Sensor_Init();
				HAL_TIM_Base_Start_IT(&htim6);
        StopMeasureSpeed();
				break ;
		}
}

void	UserTask( void )
{
	if ( ReadUserButton0() == 1 )
	{ 
		LEDCtrl( 0 , 0 );
		HAL_TIM_Base_Stop_IT(&htim6);
		HAL_TIM_Base_Stop_IT(&htim7);
		HAL_TIM_PWM_MspDeInit(&htim8);
		HAL_PWR_EnterSTANDBYMode();			
	}
	
	if ( ReadUserButton1() == 1 )
	{ 
	   	g_SysMode++;
	  	if ( g_SysMode > 2 ) g_SysMode = 0 ;
		  SetWorkMode( g_SysMode );		
	}

	switch ( g_SysMode )
	{
		case 0 :
//		  printf("L=%d,D=%d;R=%d,D=%d\n", 
//			SpeedData.LeftSpeed , SpeedData.LeftDistance , SpeedData.RightSpeed , SpeedData.RightDistance);
			break;
		case 1 :   // dot matrix
			show_dot_matrix();
			break;
		case 2 :
			Uart_OSC_ShowWave( g_mpu9250.accel_x , g_mpu9250.accel_y , g_mpu9250.accel_z, g_mpu9250.gyro_y  ) ;
//			Uart_OSC_ShowWave( g_mpu9250.angle_x , g_mpu9250.gyro_scale_y , g_mpu9250.Angle_Complement_1st, g_moto_ctrl.ir_sensor  ) ;
//		Uart_OSC_ShowWave( g_mpu9250.angle_x , g_mpu9250.gyro_scale_y  , g_mpu9250.Angle_Kalman , g_mpu9250.Angle_Complement_1st ) ;
			break;
	}
  CheckUartReceivedData();
}

void SystemTimer1msCallback( void ) 
{
	static uint32_t i = 0 ;
	
	if ( i >= 99 ) i = 0 ;
	else i ++ ;
		
	Get_IR_Sensor();
  MPU9250_Get_Accel_Gyro_Temp();
  MPU9250_Data_Process();
  Moto_Balance_PID_Ctrl();	
	switch ( g_SysMode )
	{
		case 0 :   // banlance car
	    if ( i == 99 ) LEDCtrl( 2 , 2 );
		  break;
		case 2 :
	    if ( g_mpu9250.angle_x > 0 ) LEDCtrl( 2 , 1 );
	    else if ( g_mpu9250.angle_x < 0	) LEDCtrl( 2 , 0 );
      break ;
	}
}

