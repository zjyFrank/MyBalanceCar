#include "main.h"
#include "spi.h"
#include "math.h"
#include "MPU9250.h"
#include "data_process.h"

/*
 * 函数名：void InitMPU6050(void)
 * 描述  ：初始化Mpu6050
 * 输入  ：无
 * 输出  ：无
 * 调用  ：外部调用
 */

SPI_HandleTypeDef * g_MPU9250_spi = &hspi2 ;

mpu9250_t g_mpu9250 ;


HAL_StatusTypeDef MPU9250_Mem_Write( uint8_t addr , uint8_t data )
{
	HAL_StatusTypeDef ret ;
	uint8_t buf[ 2 ] ;
	
	buf[ 0 ] = addr ;
	buf[ 1 ] = data ;
  HAL_GPIO_WritePin( SPI2_CS_GPIO_Port , SPI2_CS_Pin, GPIO_PIN_RESET );	
	ret = HAL_SPI_Transmit( g_MPU9250_spi , buf , 2 , HAL_MAX_DELAY ); 
  HAL_GPIO_WritePin( SPI2_CS_GPIO_Port , SPI2_CS_Pin, GPIO_PIN_SET );	
	return ret ;
}

HAL_StatusTypeDef MPU9250_Mem_Read( uint8_t addr , uint8_t *data )
{
	HAL_StatusTypeDef ret ;
	uint8_t addr_rd ;
	
	addr_rd = addr | 0x80 ;

  HAL_GPIO_WritePin( SPI2_CS_GPIO_Port , SPI2_CS_Pin, GPIO_PIN_RESET );	
	ret = HAL_SPI_Transmit( g_MPU9250_spi , &addr_rd , 1 , HAL_MAX_DELAY )  ;
	if ( ret == HAL_OK ) 
	{
		 ret = HAL_SPI_Receive( g_MPU9250_spi , data , 1 , HAL_MAX_DELAY );
	}
  HAL_GPIO_WritePin( SPI2_CS_GPIO_Port , SPI2_CS_Pin, GPIO_PIN_SET );	
	return ret ;
}

HAL_StatusTypeDef MPU9250_I2C_Write( uint8_t i2c_addr , uint8_t reg_addr , uint8_t data )
{
	HAL_StatusTypeDef ret ;
	ret = MPU9250_Mem_Write( I2C_SLV0_ADDR , MPU9250_AK8963_ADDR ) ;
	if ( ret != HAL_OK ) 
	{
		return ret;
	}
	
	ret = MPU9250_Mem_Write( I2C_SLV0_REG  , reg_addr ) ;
	if ( ret != HAL_OK ) 
	{
		return ret;
	}

	ret = MPU9250_Mem_Write( I2C_SLV0_DO   , data ) ;
	if ( ret != HAL_OK ) 
	{
		return ret;
	}
	return ret;

	HAL_Delay( 100 );
}

int16_t MPU9250_Data_Read(unsigned char reg_addr )
{
	uint8_t H,L;
	
	MPU9250_Mem_Read( reg_addr  , (uint8_t *)&H );
	MPU9250_Mem_Read( reg_addr+1, (uint8_t *)&L );
	
	return (H<<8)+L;   //合成数据
}

void MPU9250_Init(void)
{
	memset( &g_mpu9250 , 0 , sizeof( mpu9250_t ) );

	/***********************MPU9250_SET********************************************/
	MPU9250_Mem_Write( PWR_MGMT_1, 0x00 );
	MPU9250_Mem_Write( PWR_MGMT_2, 0x00 );
	MPU9250_Mem_Write( CONFIG, 0x06 );
	
	MPU9250_Mem_Write( INT_PIN_CFG, 0x30 );
	MPU9250_Mem_Write( I2C_MST_CTRL,0x4d );
	MPU9250_Mem_Write( USER_CTRL ,0x10 );
	MPU9250_Mem_Write( I2C_MST_DELAY_CTRL ,0x01 );
	MPU9250_Mem_Write( I2C_SLV0_CTRL ,0x81 );

	
	MPU9250_Mem_Write( SMPLRT_DIV, 0x07 );
	MPU9250_Mem_Write( GYRO_CONFIG, GYRO_CONFIG_DATA );
	MPU9250_Mem_Write( ACCEL_CONFIG, ACCEL_CONFIG_DATA );

	
	/***********************MAG_SET********************************************/
//	MPU9250_I2C_Write( MPU9250_AK8963_ADDR , AK8963_CNTL2_REG, AK8963_CNTL2_SRST );
//	MPU9250_I2C_Write( MPU9250_AK8963_ADDR , AK8963_CNTL1_REG, 0x12 );

	
	/***************************BMP280_SET*************************************/
//	MPU9250_I2C_Write( BMP280_ADDR , 0xF4, 0x25 );	
	
	MPU9250_Mem_Read( WHO_AM_I , &g_mpu9250.dev_id );
	
	switch ( ACCEL_FULL_RANGE )
	{
		case ACCEL_FULL_RANGE_2G : 
//    	g_mpu9250.accel_factor = 16384 ;
    	g_mpu9250.accel_factor = 14 - 10 ;   // 2^14
		  break ;
		case ACCEL_FULL_RANGE_4G : 
//    	g_mpu9250.accel_factor = 8192 ;
    	g_mpu9250.accel_factor = 13 - 10 ;   // 2^13
		  break ;
		case ACCEL_FULL_RANGE_8G : 
//    	g_mpu9250.accel_factor = 4096 ;
    	g_mpu9250.accel_factor = 12 - 10 ;   // 2^12
		  break ;
		case ACCEL_FULL_RANGE_16G : 
//    	g_mpu9250.accel_factor = 2048 ;
    	g_mpu9250.accel_factor = 11 - 10;   // 2^11
		  break ;
	}

	
	switch ( GYRO_FULL_RANGE )
	{
		case GYRO_FULL_RANGE_250 : 
    	g_mpu9250.gyro_factor = 131 ;
		  break ;
		case GYRO_FULL_RANGE_500 : 
    	g_mpu9250.gyro_factor = 65.5 ;
		  break ;
		case GYRO_FULL_RANGE_1000 : 
    	g_mpu9250.gyro_factor = 32.8 ;
		  break ;
		case GYRO_FULL_RANGE_2000 : 
    	g_mpu9250.gyro_factor = 16.4 ;
		  break ;
	}
}

void MPU9250_Get_Accel_Gyro_Temp( void )
{
	g_mpu9250.accel_x = MPU9250_Data_Read( ACCEL_XOUT ) ;
	g_mpu9250.accel_y = MPU9250_Data_Read( ACCEL_YOUT ) ;
	g_mpu9250.accel_z = MPU9250_Data_Read( ACCEL_ZOUT ) ;
	
//	g_mpu9250.accel_offs_x = MPU9250_Data_Read( ACCEL_OFFS_XOUT ) ;
//	g_mpu9250.accel_offs_y = MPU9250_Data_Read( ACCEL_OFFS_YOUT ) ;
//	g_mpu9250.accel_offs_z = MPU9250_Data_Read( ACCEL_OFFS_ZOUT ) ;

//	g_mpu9250.gyro_x = MPU9250_Data_Read( GYRO_XOUT ) ;
	g_mpu9250.gyro_y = MPU9250_Data_Read( GYRO_YOUT ) ;
	g_mpu9250.gyro_z = MPU9250_Data_Read( GYRO_ZOUT ) ;

//	g_mpu9250.temperature = MPU9250_Data_Read( TEMP_OUT ) ;
	
//	g_mpu9250.gyro_offs_x = MPU9250_Data_Read( GYRO_OFFS_XOUT ) ;
//	g_mpu9250.gyro_offs_y = MPU9250_Data_Read( GYRO_OFFS_YOUT ) ;
//	g_mpu9250.gyro_offs_z = MPU9250_Data_Read( GYRO_OFFS_ZOUT ) ;
	
//	g_mpu9250.accel_real_offs_x =  g_mpu9250.accel_offs_x  * 0.00098 ;
//	g_mpu9250.accel_real_offs_y =  g_mpu9250.accel_offs_y  * 0.00098 ;
//	g_mpu9250.accel_real_offs_z =  g_mpu9250.accel_offs_z  * 0.00098 ;
//	g_mpu9250.gyro_real_offs_x =  g_mpu9250.gyro_offs_x * 4 / ( 2 ^ GYRO_FULL_RANGE ) / 131.072 ;
//	g_mpu9250.gyro_real_offs_y =  g_mpu9250.gyro_offs_y * 4 / ( 2 ^ GYRO_FULL_RANGE ) / 131.072 ;
//	g_mpu9250.gyro_real_offs_z =  g_mpu9250.gyro_offs_z * 4 / ( 2 ^ GYRO_FULL_RANGE ) / 131.072 ;	
}

void MPU9250_Data_Process( void )
{
//	g_mpu9250.accel_scale_x =  g_mpu9250.accel_x  >> g_mpu9250.accel_factor ;
//	g_mpu9250.accel_scale_y =  g_mpu9250.accel_y  >> g_mpu9250.accel_factor ;
//	g_mpu9250.accel_scale_z =  g_mpu9250.accel_z  >> g_mpu9250.accel_factor ;

//	g_mpu9250.gyro_scale_x =  g_mpu9250.gyro_x  >> g_mpu9250.gyro_factor ;
	g_mpu9250.gyro_scale_y =  -1 * g_mpu9250.gyro_y  / g_mpu9250.gyro_factor ;   // direction must be same as angle
//	g_mpu9250.gyro_scale_z =  g_mpu9250.gyro_z  >> g_mpu9250.gyro_factor ;
	
//	g_mpu9250.temperature_real = g_mpu9250.temperature / 333.87 + 21;
	
	g_mpu9250.angle_x = atan2( g_mpu9250.accel_x , g_mpu9250.accel_z ) * 180.0 / PI;
//	g_mpu9250.angle_x = g_mpu9250.accel_x * 180 / 16384 / 3.1415926535 ; 
//	g_mpu9250.angle_y = atan2( g_mpu9250.accel_y , g_mpu9250.accel_z ) * 180.0 / PI;
//	g_mpu9250.angle_z = atan2( g_mpu9250.accel_z , g_mpu9250.accel_y ) * 180.0 / PI;

	Complementary_Filter_1st_Order( g_mpu9250.angle_x , g_mpu9250.gyro_scale_y );
//	Complementary_Filter_2nd_Order( g_mpu9250.angle_x , g_mpu9250.gyro_scale_y );
//	Kalman_Filter( g_mpu9250.angle_x , g_mpu9250.gyro_scale_y );
//   Complementary_Filter ( g_mpu9250.angle_x , g_mpu9250.gyro_scale_y );
}



