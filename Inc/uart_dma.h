#ifndef __uart_dma_H
#define __uart_dma_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f1xx_hal.h"
#include "main.h"

#define UART_RX_BUF_SIZE    128
#define UART_TX_BUF_SIZE    128
#define MAX_UART_PORT_NUM   4	 
#define MAX_UART_BUF_NUM    8	 
	 
typedef struct 	
{
	UART_HandleTypeDef * huart ;
	DMA_HandleTypeDef * hdma_rx ;
	DMA_HandleTypeDef * hdma_tx ;
	uint8_t uart_rx_buf[ MAX_UART_BUF_NUM ][ UART_RX_BUF_SIZE + 1 ] ;
	uint32_t  rx_buf_size[ MAX_UART_BUF_NUM ] ;
	uint32_t  rx_buf_head ;
	uint32_t  rx_buf_tail ;
	uint32_t  rx_buf_full ;	
}uart_dma_t;

void Uart_Rx_Data_Start( int32_t i , UART_HandleTypeDef * huart , DMA_HandleTypeDef * hdma_rx );
void Uart_RxIDLE_Handler( int32_t i );
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void StartAllUartDMAReceive( void );
void StopAllUart( void );
	 
void SendToDeviceUart( uint32_t channel , uint8_t * pBuf , int32_t len);
void CheckUartReceivedData( void );

void ParseUartFlashDataRxBuf( uint8_t * pBuf , uint32_t Len );
void SendUartDataToUART( uint32_t port , uint8_t * pBuf , uint32_t len );

#ifdef __cplusplus
}
#endif
#endif /*__uart_dma_H */
