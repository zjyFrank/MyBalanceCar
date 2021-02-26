#include "tim.h"
#include "esp8266.h"
#include "usb_uart.h"
#include "user_define.h"
#include "wifi_data.h"

/*
state   :  1 -- work , 0 -- flash esp8266
*/

esp8266_t g_esp8266 ;

extern IWDG_HandleTypeDef hiwdg;
extern wifi_data_t  g_wifi_data ;
extern uint32_t g_host_wifi_uart_map;

void ESP8266ModeCtrl( uint8_t state)
{

	HAL_GPIO_WritePin(WIFI1_RESET_GPIO_Port,WIFI1_RESET_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(WIFI2_RESET_GPIO_Port,WIFI2_RESET_Pin,GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(WIFI1_EN_GPIO_Port,WIFI1_EN_Pin ,GPIO_PIN_SET);
	HAL_GPIO_WritePin(WIFI2_EN_GPIO_Port,WIFI2_EN_Pin ,GPIO_PIN_SET);

	if(state == 1 ) 
	{
  	HAL_GPIO_WritePin( WIFI1_BOOT_GPIO_Port,WIFI1_BOOT_Pin, GPIO_PIN_SET );
  	HAL_GPIO_WritePin( WIFI2_BOOT_GPIO_Port,WIFI2_BOOT_Pin, GPIO_PIN_SET );
	}		
	else
	{
  	HAL_GPIO_WritePin( WIFI1_BOOT_GPIO_Port,WIFI1_BOOT_Pin, GPIO_PIN_RESET );
  	HAL_GPIO_WritePin( WIFI2_BOOT_GPIO_Port,WIFI2_BOOT_Pin, GPIO_PIN_RESET );
	}
	HAL_Delay( 400 ) ;

	HAL_GPIO_WritePin(WIFI1_RESET_GPIO_Port,WIFI1_RESET_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(WIFI2_RESET_GPIO_Port,WIFI2_RESET_Pin,GPIO_PIN_SET);	
}

void ESP8266Config( void )
{
	g_esp8266.probe_mode = 1 ;  // 1 probe data , 0 flash mode
	if ( g_host_wifi_uart_map > 0 )
	{
  	g_esp8266.usb_host_mode = 0 ;  // 1 USB interface  , 0 UART interface
	}
	else
	{
	   g_esp8266.usb_host_mode = 1 ;  // 1 USB interface  , 0 UART interface
	}
	g_esp8266.flash_mode = 0 ; // 0 select port mode , 1 flash mode
	g_esp8266.time_counter = 0 ;
	g_esp8266.flash_channel = 1 ;
	g_esp8266.led_rate = 5 ;
}

uint32_t StartWifiProbe( uint32_t mode )
{
	g_esp8266.probe_mode = mode ;
	ESP8266ModeCtrl( g_esp8266.probe_mode ) ;
	
	if ( g_esp8266.probe_mode == 0 )
	{
		if ( g_esp8266.usb_host_mode == 1 )  // USB probe mode
			USBMode( 2 );                      // flash with usb
		else
			USBMode( 0 );                      // flash with uart  	
	
 		HAL_TIM_Base_Start_IT( &htim6);	
		do
		{			
		  HAL_IWDG_Refresh( &hiwdg ) ;
			HAL_Delay(100);
		}while ( g_esp8266.flash_mode == 0 );
	}
  WiFiProbeInit();
	return 0 ;
}

void WifiProbeFlashSelect( void )
{
	  g_esp8266.time_counter++;
		if ( ( g_esp8266.time_counter % g_esp8266.led_rate ) == 0 )
		{
			if ( g_esp8266.flash_channel ==  1 ) 
			{
				LEDCtrl( 3 , 2 );
				LEDCtrl( 4 , 0 );
			}
			else if ( g_esp8266.flash_channel ==  2 ) 
			{
				LEDCtrl( 3 , 0 );
				LEDCtrl( 4 , 2 );
			}
		}
		
		if ( g_esp8266.flash_mode == 0 )
		{
			if ( ReadUserButton0() == 1 )
			{
				g_esp8266.time_counter = 0 ;
				if ( g_esp8266.flash_channel < TOTAL_WIFI_CHANNEL  ) g_esp8266.flash_channel++;
				else g_esp8266.flash_channel = 1 ;
			}
			else if ( g_esp8266.time_counter > 50 )
			{
				g_esp8266.flash_mode = 1 ;
				g_esp8266.led_rate = 1 ;
			}
		}
}

