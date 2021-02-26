#ifndef __balance_car_H
#define __balance_car_H
#ifdef __cplusplus
 extern "C" {
#endif

#define IR_5_Pin GPIO_PIN_4
#define IR_5_GPIO_Port GPIOB
#define IR_4_Pin GPIO_PIN_3
#define IR_4_GPIO_Port GPIOB
#define IR_3_Pin GPIO_PIN_5
#define IR_3_GPIO_Port GPIOB
#define IR_2_Pin GPIO_PIN_15
#define IR_2_GPIO_Port GPIOA
#define IR_1_Pin GPIO_PIN_2
#define IR_1_GPIO_Port GPIOD

uint32_t ReadUserButton0( void );
uint32_t ReadUserButton1( void );
void LEDCtrl( uint32_t index , uint32_t mode );
void SetWorkMode( uint32_t mode);
void	UserTask( void );
void  IR_Sensor_Init( void );
void Get_IR_Sensor( void );

void StartMeasureSpeed( void );

typedef struct
{
	int32_t LeftSpeed ;
	int32_t RightSpeed ;
	int32_t LeftDistance ;
	int32_t RightDistance ;
	
} SpeedDataTypeDef ;

void Kalman_Filter(float angle, float gyro) ;
void Complementary_Filter_1st_Order (float angle, float gyro ) ; 
void Complementary_Filter_2nd_Order(float angle, float gyro ) ; 
void Complementary_Filter (float angle, float gyro ) ;

void SystemTimer1msCallback( void ) ;

extern SpeedDataTypeDef SpeedData;	 
extern uint32_t g_SysMode ;
#ifdef __cplusplus
}
#endif
#endif /*__balance_car_H*/
