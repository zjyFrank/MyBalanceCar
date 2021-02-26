#include <string.h>
#include "uart_dma.h"
#include "usb_uart.h"
#include "moto_ctrl.h"

//extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
//extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
//extern UART_HandleTypeDef huart5;
//extern DMA_HandleTypeDef hdma_usart1_rx;
extern DMA_HandleTypeDef hdma_usart2_rx;
//extern DMA_HandleTypeDef hdma_usart3_rx;
extern DMA_HandleTypeDef hdma_uart4_rx;
//extern DMA_HandleTypeDef hdma_uart5_rx;


uart_dma_t g_uart_dma[ MAX_UART_PORT_NUM ] ;
uint32_t g_dev_uart_map[ MAX_UART_PORT_NUM ] = { 0 , 1 , 0 , 0  };  // index is UART number, value is dev number
																																	  // UART2 <-> dev 1
uint32_t g_host_uart_map = 4 ;      // if use uart host to receive dev data set to uart port( > 0) ,  else 0
uint32_t g_host_sys_uart_map = 0 ;       // uart port to receive host command
uint32_t g_host_monitor_uart_map = 0 ;   // uart port to send system log

//uint32_t g_dev_uart_map[ MAX_UART_PORT_NUM ] = { 3, 1, 2, 4 };

//UserFunction BEGIN
extern float KP;
extern float KI;
extern float KD;
uint8_t direction;
void  MyPID(uint8_t *pBuf, int32_t length)
{
	direction = *pBuf;
}


void Uart_Rx_Data_Start( int32_t i , UART_HandleTypeDef * huart , DMA_HandleTypeDef * hdma_rx )
{
	g_uart_dma[ i ].huart       = huart ;
	g_uart_dma[ i ].hdma_rx     = hdma_rx ;	
	g_uart_dma[ i ].rx_buf_head = 0 ;
	g_uart_dma[ i ].rx_buf_tail = 0 ;
	g_uart_dma[ i ].rx_buf_full = 0 ;
	
	__HAL_UART_ENABLE_IT( g_uart_dma[ i ].huart , UART_IT_IDLE);    //使能空闲终中断
  HAL_UART_Receive_DMA( g_uart_dma[ i ].huart , 
												( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_tail ] ,
												UART_RX_BUF_SIZE );
}

void Uart_RxIDLE_Handler( int32_t i )
{
	int32_t temp ;
	
	if( __HAL_UART_GET_FLAG( g_uart_dma[ i ].huart , UART_FLAG_IDLE) != RESET )
	{
		__HAL_UART_CLEAR_IDLEFLAG( g_uart_dma[ i ].huart );
		__HAL_UART_CLEAR_PEFLAG( g_uart_dma[ i ].huart );

//		temp = g_uart_dma[ i ].huart->Instance->SR;  
//		temp = g_uart_dma[ i ].huart->Instance->DR;
		
		HAL_UART_DMAStop( g_uart_dma[ i ].huart );
		temp  = __HAL_DMA_GET_COUNTER( g_uart_dma[ i ].hdma_rx ) ;

		g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_tail ] =  UART_RX_BUF_SIZE - temp;                           
		if ( g_uart_dma[ i ].rx_buf_tail == ( MAX_UART_BUF_NUM - 1 ) )
			g_uart_dma[ i ].rx_buf_tail = 0 ;
		else
			g_uart_dma[ i ].rx_buf_tail ++ ;

//		LEDCtrl( g_dev_uart_map[ i ] , 2 );

		HAL_UART_Receive_DMA( g_uart_dma[ i ].huart , 
													( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_tail ] ,
													UART_RX_BUF_SIZE );
			
		if ( g_uart_dma[ i ].rx_buf_tail == g_uart_dma[ i ].rx_buf_head ) 
		{
			g_uart_dma[ i ].rx_buf_full = 1 ;
		}
	}	
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

  uint32_t i ;
	uint8_t err[ 256 ] ;
	
 	
	if(huart->Instance == USART1) {	
		i = 0 ;
	} else if(huart->Instance == USART2) {	
		i = 1 ;
	} else if(huart->Instance == USART3) {	
		i = 2 ;
	} else if(huart->Instance == UART4) {	
		i = 3 ;
	} else if(huart->Instance == UART5) {	
		i = 4 ;
	} else  
	{	
		return ;
	}
	//sprintf( ( char *)err , "uart %d : error code:%X\r\n",i+1 ,huart->ErrorCode ); 
	//SysLog( err ) ;			
	//SendUartDataToUART( g_host_monitor_uart_map ,  err );

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint32_t i ;

	if(huart->Instance == USART1) {	
		i = 0 ;
	} else if(huart->Instance == USART2) {	
		i = 1 ;
	} else if(huart->Instance == USART3) {	
		i = 2 ;
	} else if(huart->Instance == UART4) {	
		i = 3 ;
	} else if(huart->Instance == UART5) {	
		i = 4 ;
	} else  
	{	
		return ;
	}
	

	g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_tail ] =  UART_RX_BUF_SIZE ;                           
	if ( g_uart_dma[ i ].rx_buf_tail == ( MAX_UART_BUF_NUM - 1 ) )
		g_uart_dma[ i ].rx_buf_tail = 0 ;
	else
		g_uart_dma[ i ].rx_buf_tail ++ ;

	HAL_UART_Receive_DMA( g_uart_dma[ i ].huart , 
												( uint8_t *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_tail ] ,
												UART_RX_BUF_SIZE );

	if ( g_uart_dma[ i ].rx_buf_tail == g_uart_dma[ i ].rx_buf_head ) 
	{
		g_uart_dma[ i ].rx_buf_full = 1 ;
	}
//	LEDCtrl( g_dev_uart_map[ i ] , 2 );

}



void SendToDeviceUart( uint32_t channel , uint8_t * pBuf , int32_t len)
{
	uint32_t i ;
	for ( i = 0 ; i < MAX_UART_PORT_NUM ; i++ )
	{
		if ( g_dev_uart_map[ i ] == channel ) 
		{
				HAL_UART_Transmit( g_uart_dma[ i ].huart , (uint8_t *)&pBuf[0] , len , HAL_MAX_DELAY )	;
			  break ;
//				HAL_UART_Transmit_DMA( g_uart_dma[ i ].huart , (uint8_t *)&pBuf[0] , len  )	;
		}
	}
}

void SendUartDataToUART( uint32_t port , uint8_t * pBuf , uint32_t len )
{
	uint32_t i ;
	
  if ( port > 0 )
	{
		  i = port - 1 ;
			HAL_UART_Transmit( g_uart_dma[ i ].huart , (uint8_t *)&pBuf[0] , len , HAL_MAX_DELAY )	;
//				HAL_UART_Transmit_DMA( g_uart_dma[ i ].huart , (uint8_t *)&pBuf[0] , len  )	;
	}	
}

void StartAllUartDMAReceive( )
{
	uint32_t i ;
	for ( i = 0 ; i < MAX_UART_PORT_NUM ; i++ )
	{
		if ( g_dev_uart_map[ i ] > 0 || 
			  ( ( g_host_uart_map - 1 ) == i ) ||
        ( ( g_host_sys_uart_map - 1 ) == i )  )    		 
		{
			switch ( i )
			{
				case 0 :
//					Uart_Rx_Data_Start( i , &huart1 , &hdma_usart1_rx );
					break ;			
				case 1 :
					Uart_Rx_Data_Start( i , &huart2 , &hdma_usart2_rx );
					break ;			
				case 2 :
//					Uart_Rx_Data_Start( i , &huart3 , &hdma_usart3_rx );
					break ;			
				case 3 :
					Uart_Rx_Data_Start( i , &huart4 , &hdma_uart4_rx );
					break ;			
			}
		}
	}
}

void StopAllUart( void )
{
	uint32_t i ;
	for ( i = 0 ; i < MAX_UART_PORT_NUM ; i++ )
	{
		if ( g_dev_uart_map[ i ] > 0 || 
			  ( ( g_host_uart_map - 1 ) == i ) ||
        ( ( g_host_sys_uart_map - 1 ) == i ) 
		   ) 
		{
    	__HAL_UART_DISABLE_IT( g_uart_dma[ i ].huart , UART_IT_IDLE);    //使能空闲终中断
			HAL_UART_MspDeInit( g_uart_dma[ i ].huart ) ;
		}
	}
}

void CheckUartReceivedData( void )
{
	//static uint8_t rx_buf[ UART_RX_BUF_SIZE + 1 ] ;
	uint8_t * rx_buf ;
	int32_t len ;
	
	int32_t i ,j ;
 	for ( i = 0 ; i < MAX_UART_PORT_NUM ; i++ )
	{
		if ( g_dev_uart_map[ i ] > 0 )
		{
			while ( g_uart_dma[ i ].rx_buf_full == 1 || 
				   g_uart_dma[ i ].rx_buf_head != g_uart_dma[ i ].rx_buf_tail  )
			{
				len = g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_head ] ;
        
        rx_buf = g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_head ] ;
				//memcpy( rx_buf , ( const void *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_head ] , len  );
				
/*
				if ( g_esp8266.probe_mode == 1 )   // wifi data
				{
					ParseUartWifiDataRxBuf( rx_buf , len , g_dev_uart_map[ i ] );
				}
				else
				{
					if ( g_esp8266.flash_mode == 1 )
					{				
//						ParseUartFlashDataRxBuf( g_uart_dma[ i ].uart_rx_buf[ 1 ] , g_uart_dma[ i ].rx_buf_size );
             if ( g_esp8266.usb_host_mode == 1 ) 
						 {
						    SendUartDataToUSB( rx_buf , len );   // WIFI -> HOST USB
						 }
						 else if( g_esp8266.usb_host_mode == 0 && g_host_uart_map > 0 ) 
						 {
						    SendUartDataToUART( g_host_uart_map , rx_buf , len );  // DEV -> HOST UART
						 }
					}   				
				}
*/				
		    if( g_host_uart_map > 0 ) 
					SendUartDataToUART( g_host_uart_map , rx_buf , len );  // DEV -> HOST UART
			
			MyPID(rx_buf,len);		//处理数据函数-自定义
			
				if ( g_uart_dma[ i ].rx_buf_head == ( MAX_UART_BUF_NUM - 1 ) )
	  			g_uart_dma[ i ].rx_buf_head = 0 ;
		  	else
			  	g_uart_dma[ i ].rx_buf_head ++ ;
        g_uart_dma[ i ].rx_buf_full = 0 ;
				
			}			
		}
		
 		if ( ( g_host_uart_map - 1 ) == i )  // Host UART -> DEV
		{
			while ( g_uart_dma[ i ].rx_buf_full == 1 || 
				   g_uart_dma[ i ].rx_buf_head != g_uart_dma[ i ].rx_buf_tail  )
			{
				len = g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_head ] ;
				rx_buf = g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_head ] ;
				//memcpy( rx_buf , ( const void *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_head ] , len  );
				
/*
		    if ( g_esp8266.probe_mode == 1 )   // wifi data
		    {
		  	  if ( ParseHostWifiCtrlRxBuf( rx_buf , len  ) )
					{
						SendUartDataToUART( g_host_uart_map , "@OK|PROBE&" , strlen("@OK|PROBE&") );
					}
		    }
		    else if (  g_esp8266.usb_host_mode == 0 && g_esp8266.flash_mode == 1 )
		    {
  	  		SendToDeviceUart( g_esp8266.flash_channel , rx_buf , len ) ;
	    	}
*/
      	for ( j = 0 ; j < MAX_UART_PORT_NUM ; j++ )
	      {
		      if ( g_dev_uart_map[ j ] > 0 )
		      {
  	   		    SendToDeviceUart( g_dev_uart_map[ j ] , rx_buf , len ) ;
					}
				}
				
				if ( g_uart_dma[ i ].rx_buf_head == ( MAX_UART_BUF_NUM - 1 ) )
					g_uart_dma[ i ].rx_buf_head = 0 ;
				else
					g_uart_dma[ i ].rx_buf_head ++ ;
        g_uart_dma[ i ].rx_buf_full = 0 ;
			}
		}
		
 		if ( ( g_host_sys_uart_map - 1 ) == i )  // Host UART -> MCU
		{
			while ( g_uart_dma[ i ].rx_buf_full == 1 || 
				   g_uart_dma[ i ].rx_buf_head != g_uart_dma[ i ].rx_buf_tail  )
			{
				len = g_uart_dma[ i ].rx_buf_size[ g_uart_dma[ i ].rx_buf_head ] ;
				rx_buf = g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_head ] ;
				//memcpy( rx_buf , ( const void *)g_uart_dma[ i ].uart_rx_buf[ g_uart_dma[ i ].rx_buf_head ] , len  );

//				ParseHostSystemCtrlRxBuf( rx_buf , len  ) ;
				if ( g_uart_dma[ i ].rx_buf_head == ( MAX_UART_BUF_NUM - 1 ) )
					g_uart_dma[ i ].rx_buf_head = 0 ;
				else
					g_uart_dma[ i ].rx_buf_head ++ ;
        g_uart_dma[ i ].rx_buf_full = 0 ;			}
		}
	}

}

void ParseUartFlashDataRxBuf( uint8_t * pBuf , uint32_t Len )
{
	static uint32_t rx_start_flag = 0 ;	
	static uint8_t tx_buf[ UART_TX_BUF_SIZE ];
	static uint32_t tx_len  = 0 ;
	
	uint32_t i ;
	for ( i = 0 ; i < Len ; i ++ )
	{
		if ( pBuf[ i ] == 0xc0 )
		{
			if ( tx_len > 0 )
			{
				tx_buf[ tx_len++ ] = pBuf[ i ] ;
				SendUartDataToUSB( tx_buf , tx_len );
				rx_start_flag = 0 ;	
				tx_len  = 0 ;
			}
			else
			{
				tx_buf[ tx_len++ ] = pBuf[ i ] ;
				rx_start_flag = 1 ;	
			}
		}
		else
		{
			if ( rx_start_flag == 1 )
			{
				tx_buf[ tx_len++ ] = pBuf[ i ] ;				
			}
		}
	}	
}
