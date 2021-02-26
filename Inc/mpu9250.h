#ifndef __MPU9250_H
#define	__MPU9250_H
#include "main.h"

/* MPU9250 Register Address ------------------------------------------------------------*/
//硬件ID 0x75 值是113
#define	SMPLRT_DIV		 0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG				 0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		 0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	 0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_CONFIG_2 0x1D 


#define	ACCEL_XOUT  	0x3B	//加速度计输出数据
#define	ACCEL_XOUT_H	0x3B	
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT  	0x3D	
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT  	0x3F	
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	ACCEL_OFFS_XOUT  	0x77	
#define	ACCEL_OFFS_XOUT_H	0x77
#define	ACCEL_OFFS_XOUT_L	0x78
#define	ACCEL_OFFS_YOUT  	0x7A	
#define	ACCEL_OFFS_YOUT_H	0x7A
#define	ACCEL_OFFS_YOUT_L	0x7B
#define	ACCEL_OFFS_ZOUT  	0x7D	
#define	ACCEL_OFFS_ZOUT_H	0x7D
#define	ACCEL_OFFS_ZOUT_L	0x7E


#define	TEMP_OUT   		0x41	//温度计计输出数据
#define	TEMP_OUT_H		0x41	//温度计计输出数据
#define	TEMP_OUT_L		0x42

#define	GYRO_XOUT  	  0x43	
#define	GYRO_XOUT_H		0x43	//陀螺仪输出数据
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT    	0x45
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT   	0x47	
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48

#define	GYRO_OFFS_XOUT  	  0x13	
#define	GYRO_OFFS_XOUT_H		0x13	//陀螺仪输出数据
#define	GYRO_OFFS_XOUT_L		0x14	
#define	GYRO_OFFS_YOUT    	0x15
#define	GYRO_OFFS_YOUT_H		0x15
#define	GYRO_OFFS_YOUT_L		0x16
#define	GYRO_OFFS_ZOUT   	  0x17	
#define	GYRO_OFFS_ZOUT_H		0x17
#define	GYRO_OFFS_ZOUT_L		0x18

#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	PWR_MGMT_2		0x6C	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I			0x75	//IIC地址寄存器(默认数值0x68，只读)硬件ID(寄存器值113)
#define	INT_PIN_CFG   0x37 


#define	MPU9250_ADDRESS	     0xD0	//从机地址
#define I2C_MST_CTRL         0x24
#define USER_CTRL            0x6A
#define I2C_MST_DELAY_CTRL   0x67

//--------------------i2c slv0-------------------------------//
#define I2C_SLV0_ADDR                       0x25  
#define I2C_SLV0_REG                        0x26
#define I2C_SLV0_CTRL                       0x27 
#define I2C_SLV0_DO                         0x63 //output reg

//--------------------AK8963 reg addr------------------------//
#define MPU9250_AK8963_ADDR                 0x0C  //AKM addr
#define AK8963_WHOAMI_REG                   0x00  //AKM ID addr
#define AK8963_WHOAMI_ID                    0x48  //ID
#define AK8963_ST1_REG                      0x02  //Data Status1
#define AK8963_ST2_REG                      0x09  //Data reading end register & check Magnetic sensor overflow occurred
#define AK8963_ST1_DOR                      0x02
#define AK8963_ST1_DRDY                     0x01 //Data Ready
#define AK8963_ST2_BITM                     0x10
#define AK8963_ST2_HOFL                     0x08 // Magnetic sensor overflow 
#define AK8963_CNTL1_REG                    0x0A
#define AK8963_CNTL2_REG                    0x0B
#define AK8963_CNTL2_SRST                   0x01 //soft Reset
#define AK8963_ASAX                         0x10 //X-axis sensitivity adjustment value 
#define AK8963_ASAY                         0x11 //Y-axis sensitivity adjustment value
#define AK8963_ASAZ                         0x12 //Z-axis sensitivity adjustment value

/****************MAG_Register Address***********************************/
//硬件ID 0x00 值是72
#define MAG_ADDRESS	  0x18 

#define MAG_OUT    		0x03
#define MAG_XOUT_L		0x03
#define MAG_XOUT_H		0x04
#define MAG_YOUT_L		0x05
#define MAG_YOUT_H		0x06
#define MAG_ZOUT_L		0x07
#define MAG_ZOUT_H		0x08



/****************BMP280_Register Address***********************************/
//硬件id 0xd0 值为88
#define BMP280_ADDR   	  0xEC//当SDO为0时地址为1110110  
  
#define BMP280_RESET_VALUE 0xB6  
  
/*calibration parameters */  
#define BMP280_DIG_T1_LSB_REG                0x88  
#define BMP280_DIG_T1_MSB_REG                0x89  
#define BMP280_DIG_T2_LSB_REG                0x8A  
#define BMP280_DIG_T2_MSB_REG                0x8B  
#define BMP280_DIG_T3_LSB_REG                0x8C  
#define BMP280_DIG_T3_MSB_REG                0x8D  
#define BMP280_DIG_P1_LSB_REG                0x8E  
#define BMP280_DIG_P1_MSB_REG                0x8F  
#define BMP280_DIG_P2_LSB_REG                0x90  
#define BMP280_DIG_P2_MSB_REG                0x91  
#define BMP280_DIG_P3_LSB_REG                0x92  
#define BMP280_DIG_P3_MSB_REG                0x93  
#define BMP280_DIG_P4_LSB_REG                0x94  
#define BMP280_DIG_P4_MSB_REG                0x95  
#define BMP280_DIG_P5_LSB_REG                0x96  
#define BMP280_DIG_P5_MSB_REG                0x97  
#define BMP280_DIG_P6_LSB_REG                0x98  
#define BMP280_DIG_P6_MSB_REG                0x99  
#define BMP280_DIG_P7_LSB_REG                0x9A  
#define BMP280_DIG_P7_MSB_REG                0x9B  
#define BMP280_DIG_P8_LSB_REG                0x9C  
#define BMP280_DIG_P8_MSB_REG                0x9D  
#define BMP280_DIG_P9_LSB_REG                0x9E  
#define BMP280_DIG_P9_MSB_REG                0x9F  
  
#define BMP280_CHIPID_REG                    0xD0  /*Chip ID Register */  
#define BMP280_RESET_REG                     0xE0  /*Softreset Register */  
#define BMP280_STATUS_REG                    0xF3  /*Status Register */  
#define BMP280_CTRLMEAS_REG                  0xF4  /*Ctrl Measure Register */  
#define BMP280_CONFIG_REG                    0xF5  /*Configuration Register */  
#define BMP280_PRESSURE_MSB_REG              0xF7  /*Pressure MSB Register */  
#define BMP280_PRESSURE_LSB_REG              0xF8  /*Pressure LSB Register */  
#define BMP280_PRESSURE_XLSB_REG             0xF9  /*Pressure XLSB Register */  
#define BMP280_TEMPERATURE_MSB_REG           0xFA  /*Temperature MSB Reg */  
#define BMP280_TEMPERATURE_LSB_REG           0xFB  /*Temperature LSB Reg */  
#define BMP280_TEMPERATURE_XLSB_REG          0xFC  /*Temperature XLSB Reg */ 

#define ACCEL_FULL_RANGE_2G                  0x0
#define ACCEL_FULL_RANGE_4G                  0x1
#define ACCEL_FULL_RANGE_8G                  0x2
#define ACCEL_FULL_RANGE_16G                 0x3
#define ACCEL_FULL_RANGE                     ACCEL_FULL_RANGE_4G

#define GYRO_FULL_RANGE_250                  0x0
#define GYRO_FULL_RANGE_500                  0x1
#define GYRO_FULL_RANGE_1000                 0x2
#define GYRO_FULL_RANGE_2000                 0x3
#define GYRO_FULL_RANGE                      GYRO_FULL_RANGE_500

#define GYRO_CONFIG_DATA                     GYRO_FULL_RANGE<<3 | 0x00 
#define ACCEL_CONFIG_DATA                    ACCEL_FULL_RANGE<<3 | 0x00 
#define PI                                   3.1415926535

//#define DeltaTime                            0.01              // 10ms 
#define DeltaTime                            0.005              // 5ms 

typedef struct {
	int16_t accel_x ;
	int16_t accel_y ;
	int16_t accel_z ;
	int16_t gyro_x ;
	int16_t gyro_y ;
	int16_t gyro_z ;
	
//	int16_t gyro_offs_x ;
//	int16_t gyro_offs_y ;
//	int16_t gyro_offs_z ;
//	int16_t accel_offs_x ;
//	int16_t accel_offs_y ;
//	int16_t accel_offs_z ;

	int16_t   accel_factor ;
	float   gyro_factor ;
	
	int16_t mag_x ;
	int16_t mag_y ;
	int16_t mag_z ;
	
	int16_t temperature ;
	int8_t  dev_id ;

	int16_t accel_scale_x ;
	int16_t accel_scale_y ;
	int16_t accel_scale_z ;

	float gyro_scale_x ;
	float gyro_scale_y ;
	float gyro_scale_z ;

//	float accel_real_x ;
//	float accel_real_y ;
//	float accel_real_z ;

//  float accel_real_offs_x ;
//	float accel_real_offs_y ;
//	float accel_real_offs_z ;
//	float gyro_real_offs_x ;
//	float gyro_real_offs_y ;
//	float gyro_real_offs_z ;

//	float gyro_real_x ;
//	float gyro_real_y ;
//	float gyro_real_z ;

	float temperature_real ;

	float angle_x ;
	float angle_y ;
	float angle_z ;
	
	float Angle_Kalman ;
	float Gyro_Kalman ;
	float Angle_Complement_1st ;
	float Angle_Complement_2nd ;
} mpu9250_t ;

void MPU9250_Init(void);
HAL_StatusTypeDef MPU9250_Mem_Write( uint8_t addr , uint8_t data );
HAL_StatusTypeDef MPU9250_Mem_Read( uint8_t addr , uint8_t *data );
HAL_StatusTypeDef MPU9250_I2C_Write( uint8_t i2c_addr , uint8_t reg_addr , uint8_t data );
int16_t MPU9250_Data_Read(unsigned char reg_addr );
void MPU9250_Get_Accel_Gyro_Temp( void );
void MPU9250_Data_Process( void );

extern mpu9250_t g_mpu9250 ;

#endif 


