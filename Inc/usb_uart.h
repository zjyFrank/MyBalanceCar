#ifndef __usb_uart_H
#define __usb_uart_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f1xx_hal.h"
#include "main.h"
#include "usbd_cdc_if.h"

#define USB_RX_BUF_SIZE    128
#define USB_TX_BUF_SIZE    128
#define MAX_USB_BUF_NUM    32	 

typedef struct 	
{
	uint8_t usb_rx_buf[ MAX_USB_BUF_NUM ][ USB_RX_BUF_SIZE + 1 ];
	uint32_t  rx_buf_size[ MAX_USB_BUF_NUM ] ;
	uint32_t  rx_buf_head ;
	uint32_t  rx_buf_tail ;
	uint32_t  rx_buf_full ;
	
}usb_uart_t;
	 
void RcvDataFromUSB( uint8_t* Buf, uint32_t Len );
void CheckUSBReceivedData( void );
void SendUartDataToUSB( uint8_t* Buf, uint32_t Len );
void Usb2UartInit( void );

#ifdef __cplusplus
}
#endif
#endif /*__usb_uart_H */
