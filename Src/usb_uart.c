#include "usbd_cdc_if.h"
#include "usb_uart.h"
#include "uart_dma.h"

extern USBD_HandleTypeDef hUsbDeviceFS;

usb_uart_t g_usb_uart ;


void Usb2UartInit( void )
{
	g_usb_uart.rx_buf_full = 0 ;
	g_usb_uart.rx_buf_tail = 0 ;
	g_usb_uart.rx_buf_head = 0 ;
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, (uint8_t *)g_usb_uart.usb_rx_buf[ g_usb_uart.rx_buf_tail ] );
//  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
}

void RcvDataFromUSB( uint8_t* Buf, uint32_t Len )
{
	
	g_usb_uart.rx_buf_size[ g_usb_uart.rx_buf_tail ]  = Len ;
	
	if ( g_usb_uart.rx_buf_tail == ( MAX_USB_BUF_NUM - 1 ) )
		g_usb_uart.rx_buf_tail = 0 ;
	else
		g_usb_uart.rx_buf_tail ++ ;
	if ( g_usb_uart.rx_buf_tail == g_usb_uart.rx_buf_head ) 
	{
		g_usb_uart.rx_buf_full = 1 ;
	}

  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, (uint8_t *)g_usb_uart.usb_rx_buf[ g_usb_uart.rx_buf_tail ] );
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
}

void SendUartDataToUSB( uint8_t* Buf, uint32_t Len )
{
  uint8_t result = USBD_OK;
	do 
	{
		result = CDC_Transmit_FS( Buf , Len  ) ;
		if ( result == USBD_FAIL ) 
		{
      break;			
		}
		else if( result == USBD_BUSY ) 
		{
      break;			
		}
  } while ( result != USBD_OK ) ;
	HAL_Delay( 10 );
}

void CheckUSBReceivedData( void )
{
//	static uint8_t rx_buf[ USB_RX_BUF_SIZE ] ;
	uint8_t * rx_buf ;
	uint32_t len ;
	
	while ( g_usb_uart.rx_buf_full == 1 || 
			 g_usb_uart.rx_buf_head != g_usb_uart.rx_buf_tail  )
	{
		len = g_usb_uart.rx_buf_size[ g_usb_uart.rx_buf_head ] ;
//		 memcpy( rx_buf , (const void *)g_usb_uart.usb_rx_buf[ g_usb_uart.rx_buf_head ] , len  );
		rx_buf = g_usb_uart.usb_rx_buf[ g_usb_uart.rx_buf_head ] ;
/*
		if ( g_esp8266.probe_mode == 1 )    // wifi data
		{
			if ( ParseHostWifiCtrlRxBuf( rx_buf  , len  ) )
			{
				SendUartDataToUSB( "@OK|PROBE&" , strlen( "@OK|PROBE&" ) );
			}
		}
		else 
		{
			if ( g_esp8266.flash_mode == 1 )  // flash mode
		  {
			  SendToDeviceUart( g_esp8266.flash_channel , rx_buf , len ) ;
			}
		}
*/
		if ( g_usb_uart.rx_buf_head == ( MAX_USB_BUF_NUM - 1 ) )
			g_usb_uart.rx_buf_head = 0 ;
		else
			g_usb_uart.rx_buf_head ++ ;
		g_usb_uart.rx_buf_full = 0 ;
	}
}

