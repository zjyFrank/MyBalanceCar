#ifndef __esp8266_H
#define __esp8266_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f1xx_hal.h"
#include "main.h"

#define UART_RX_BUF_SIZE    128
#define UART_TX_BUF_SIZE    128
#define MAX_UART_PORT_NUM   2	 
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
	uint8_t   tx_busy;
}uart_dma_t;

typedef struct
{
	uint8_t work_mode  ;       // 0 Disable, 1 Program mode, 2 WiFi mode
	UART_HandleTypeDef * host_uart ;
	UART_HandleTypeDef * esp8266_uart ;
	uart_dma_t uart_dma_rx[ MAX_UART_PORT_NUM ] ;
}esp8266_t ;

void Uart_RxIDLE_Handler( UART_HandleTypeDef * huart );

void ESP8266ModeCtrl( uint8_t state);
void ESP8266ModeCheck( void );
void Host_ESP8266_UART_INIT( void );
void Host_ESP8266_UART_Tranceiver( void );


#ifdef __cplusplus
}
#endif
#endif /*__esp8266_H */
