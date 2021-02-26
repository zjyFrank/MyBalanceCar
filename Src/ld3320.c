#include "spi.h"
#include "ld3320.h"

uint8_t g_AsrResult = 0 ;

SPI_HandleTypeDef * g_LD3320_spi = &hspi1 ;

HAL_StatusTypeDef LD3320_Reg_Read( uint8_t addr , uint8_t *data )
{
	HAL_StatusTypeDef ret ;

	uint8_t buf[ 2 ] ;
	
	buf[ 0 ] = 0x05 ;
	buf[ 1 ] = addr ;

  HAL_GPIO_WritePin( SPI1_CS_GPIO_Port , SPI1_CS_Pin, GPIO_PIN_RESET );	
	ret = HAL_SPI_Transmit( g_LD3320_spi , buf , 2 , HAL_MAX_DELAY )  ;
	if ( ret == HAL_OK ) 
	{
     ret = HAL_SPI_Receive( g_LD3320_spi , data , 1 , HAL_MAX_DELAY );
	}
  HAL_GPIO_WritePin( SPI1_CS_GPIO_Port , SPI1_CS_Pin, GPIO_PIN_SET );	
	return ret ;
}

HAL_StatusTypeDef LD3320_Reg_Write( uint8_t addr , uint8_t data )
{
	HAL_StatusTypeDef ret ;
	uint8_t buf[ 3 ] ;
	
	buf[ 0 ] = 0x04 ;
	buf[ 1 ] = addr ;
	buf[ 2 ] = data ;

  HAL_GPIO_WritePin( SPI1_CS_GPIO_Port , SPI1_CS_Pin, GPIO_PIN_RESET );	
	ret = HAL_SPI_Transmit( g_LD3320_spi , buf , 3 , HAL_MAX_DELAY ); 
  HAL_GPIO_WritePin( SPI1_CS_GPIO_Port , SPI1_CS_Pin, GPIO_PIN_SET );	
	return ret ;
}

void LD3320_Power( int on )
{
	if ( on == 1 ) 
	{
 	  HAL_GPIO_WritePin(AUDIO_PWR_EN_GPIO_Port, AUDIO_PWR_EN_Pin, GPIO_PIN_RESET);		
	}
	else
	{
 	  HAL_GPIO_WritePin(AUDIO_PWR_EN_GPIO_Port, AUDIO_PWR_EN_Pin, GPIO_PIN_SET);		
	}
  HAL_GPIO_WritePin( SPI1_CS_GPIO_Port , SPI1_CS_Pin, GPIO_PIN_SET );	
}

void LD3320_Init_Common()
{
	uint8_t buf[ 3 ] ;
	LD3320_Reg_Read( 0x06, buf);
	LD3320_Reg_Write(0x17, 0x35); 
	HAL_Delay( 10 );
	LD3320_Reg_Read( 0x06, buf);

	LD3320_Reg_Write(0x89, 0x03);  
	HAL_Delay(5);
	LD3320_Reg_Write(0xCF, 0x43);   
	HAL_Delay(5);
	LD3320_Reg_Write(0xCB, 0x02);

	/*PLL setting*/
	LD3320_Reg_Write(0x11, LD_PLL_11);       

	LD3320_Reg_Write(0x1E,0x00);
	LD3320_Reg_Write(0x19, LD_PLL_ASR_19); 
	LD3320_Reg_Write(0x1B, LD_PLL_ASR_1B);		
  LD3320_Reg_Write(0x1D, LD_PLL_ASR_1D);
	HAL_Delay( 10 );
	
	LD3320_Reg_Write(0xCD, 0x04);
	LD3320_Reg_Write(0x17, 0x4c); 
	HAL_Delay(5);
	LD3320_Reg_Write(0xB9, 0x00);
	LD3320_Reg_Write(0xCF, 0x4F); 
	LD3320_Reg_Write(0x6F, 0xFF); 
}
	
void LD3320_Init_ASR()
{
	LD3320_Init_Common();

	LD3320_Reg_Write(0xB3, 0x60);
	LD3320_Reg_Write(0xBD, 0x00);
	LD3320_Reg_Write(0x17, 0x48);
	HAL_Delay( 10 );

	LD3320_Reg_Write(0x3C, 0x80);    
	LD3320_Reg_Write(0x3E, 0x07);
	LD3320_Reg_Write(0x38, 0xff);    
	LD3320_Reg_Write(0x3A, 0x07);
	
	LD3320_Reg_Write(0x40, 0);          
	LD3320_Reg_Write(0x42, 8);
	LD3320_Reg_Write(0x44, 0);    
	LD3320_Reg_Write(0x46, 8); 
	HAL_Delay( 1 );
}


// Return 1: success.
uint8_t LD3320_Check_ASRBusyFlag_b2()
{
	uint8_t j;
	uint8_t flag = 0;
	uint8_t buf[ 4 ] ;

	for (j=0; j<10; j++)
	{
		if (LD3320_Reg_Read(0xb2 , buf ) == HAL_OK )
		{
			if ( buf[0] == 0x21 )
			{
			  flag = 1;
			  break;
			}
		}
		HAL_Delay(10);		
	}
	return flag;
}

void LD3320_AsrStart()
{
	LD3320_Init_ASR();
}

// Return 1: success.
uint8_t LD3320_AsrRun()
{
	LD3320_Reg_Write(0x35, MIC_VOL);
	LD3320_Reg_Write(0x1C, 0x09);
	LD3320_Reg_Write(0xBD, 0x20);
	LD3320_Reg_Write(0x08, 0x01);
	HAL_Delay( 1 );
	LD3320_Reg_Write(0x08, 0x00);
	HAL_Delay( 1 );

	if(LD3320_Check_ASRBusyFlag_b2() == 0)
	{
		return 0;
	}

	LD3320_Reg_Write(0xB2, 0xff);	
	LD3320_Reg_Write(0x37, 0x06);
	HAL_Delay( 5 );
	LD3320_Reg_Write(0x1C, 0x0b);
	LD3320_Reg_Write(0x29, 0x10);
	
	LD3320_Reg_Write(0xBD, 0x00);
	return 1;
}

uint8_t LD3320_AsrAddFixed()
{
	uint8_t k, flag;
	uint8_t nAsrAddLength;
	const char sRecog[5][13] = {"bei jing", "shou du", 
		"shang hai", "tian jin", "chong qing"};
	const uint8_t pCode[5] = {CODE_BEIJING, CODE_BEIJING, CODE_SHANGHAI, CODE_TIANJIN, CODE_CHONGQING};
		
	flag = 1;
	for (k=0; k<5; k++)
	{
			
		if(LD3320_Check_ASRBusyFlag_b2() == 0)
		{
			flag = 0;
			break;
		}
		
		LD3320_Reg_Write(0xc1, pCode[k] );
		LD3320_Reg_Write(0xc3, 0 );
		LD3320_Reg_Write(0x08, 0x04);
		HAL_Delay(1);
		LD3320_Reg_Write(0x08, 0x00);
		HAL_Delay(1);

		for (nAsrAddLength=0; nAsrAddLength<20; nAsrAddLength++)
		{
			if (sRecog[k][nAsrAddLength] == 0)
				break;
			LD3320_Reg_Write(0x5, sRecog[k][nAsrAddLength]);
		}
		LD3320_Reg_Write(0xb9, nAsrAddLength);
		LD3320_Reg_Write(0xb2, 0xff);
		LD3320_Reg_Write(0x37, 0x04);
	}
    return flag;
}

uint8_t LD3320_GetResult()
{
	uint8_t buf[ 4 ] ;

	if (LD3320_Reg_Read(0xc5 , buf ) == HAL_OK )
	{
		return buf[0] ;
	}
	else return 0;
}


void LD3320_Int()
{
	uint8_t buf[ 4 ] ;
	uint8_t nAsrResCount=0;
	uint8_t nAsrBusy=0;
	uint8_t nAsrStatus=0;
	uint8_t ucRegVal=0;

	if (LD3320_Reg_Read(0x2B , buf ) != HAL_OK ) return;
	ucRegVal = buf[0];

	LD3320_Reg_Write(0x29,0) ;
	LD3320_Reg_Write(0x02,0) ;
	if (LD3320_Reg_Read(0xb2 , buf ) != HAL_OK ) return;
	nAsrBusy = buf[0];
	if (LD3320_Reg_Read(0xbf , buf ) != HAL_OK ) return;
	nAsrStatus = buf[0];
	if((ucRegVal & 0x10) &&
		  nAsrBusy==0x21 && 
		  nAsrStatus==0x35)
	{
	  if (LD3320_Reg_Read(0xba , buf ) != HAL_OK ) return;
	  nAsrResCount = buf[0];
		if(nAsrResCount>0 && nAsrResCount<=4) 
		{
	      if (LD3320_Reg_Read(0xc5 , buf ) != HAL_OK ) return;
	      g_AsrResult = buf[0];
		}
		else
    {
				g_AsrResult=0;
		}	
	}
	else
	{
			g_AsrResult=0;
	}
			
 	LD3320_Reg_Write(0x2b, 0);
 	LD3320_Reg_Write(0x1C,0);
	return;
}

uint8_t RunASR()
{
	uint8_t i=0;
	uint8_t asrflag=0;
	for (i=0; i<5; i++)			
	{
		LD3320_AsrStart();
		HAL_Delay(100);
		if (LD3320_AsrAddFixed()==0)
		{
			LD3320_Power(0);
			HAL_Delay(100);			
			LD3320_Power(1);
			HAL_Delay(100);			
			continue;
		}
		HAL_Delay(10);
		if (LD3320_AsrRun() == 0)
		{
			LD3320_Power(0);
			HAL_Delay(100);			
			LD3320_Power(1);
			HAL_Delay(100);			
			continue;
		}

		asrflag=1;
		break;				
	}

	return asrflag;
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == GPIO_PIN_4 )
	{
		LD3320_Int();
		RunASR();
	}
}
