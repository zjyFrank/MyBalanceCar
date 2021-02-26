#include "main.h"
#include "usart.h"
#include "uart_osc.h"

typedef union 
{
	int16_t data16;
	int8_t data8[2];
} uart_osc_t;


uart_osc_t g_Uart_OSC_Data[4] ;

//crc16 
unsigned short CRC_CHECK(unsigned char *Buf, unsigned char CRC_CNT)
{
    unsigned short CRC_Temp;
    unsigned char i,j;
    CRC_Temp = 0xffff;

    for (i=0;i<CRC_CNT; i++){      
        CRC_Temp ^= Buf[i];
        for (j=0;j<8;j++) {
            if (CRC_Temp & 0x01)
                CRC_Temp = (CRC_Temp >>1 ) ^ 0xa001;
            else
                CRC_Temp = CRC_Temp >> 1;
        }
    }
    return(CRC_Temp);
}

void Uart_OSC_Output_Data(void)
{
  unsigned char databuf[10] = {0};
  unsigned char i;
  unsigned short CRC16 = 0;

  for(i=0;i<4;i++) 
  {
    databuf[i*2]   = g_Uart_OSC_Data[ i ].data8[0];
    databuf[i*2+1] = g_Uart_OSC_Data[ i ].data8[1];
  }
  
  CRC16 = CRC_CHECK(databuf,8);
  databuf[8] = CRC16%256;
  databuf[9] = CRC16/256;
  
  for(i=0;i<10;i++){
		HAL_UART_Transmit(&huart4, (uint8_t *)&databuf[i], 1, 0xFFFF);
		while(HAL_UART_GetState(&huart4) == HAL_UART_STATE_RESET) ;
	}
}

void Uart_OSC_ShowWave(int a,int b,int c,int d)
{
//  char databuf[128] ;
//	sprintf( databuf , "x=%d ,y=%d , z=%d \r\n" , a , b, c );	
//	HAL_UART_Transmit(&huart1, databuf, strlen( databuf ), 0xFFFF);
//	return ;
	g_Uart_OSC_Data[0].data16 = a;
	g_Uart_OSC_Data[1].data16 = b;
	g_Uart_OSC_Data[2].data16 = c;
	g_Uart_OSC_Data[3].data16 = d;
	Uart_OSC_Output_Data();
}

