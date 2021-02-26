#include "bt_uart.h"
#include <string.h>
#include "uart_dma.h"
#include "usb_uart.h"

void EnableBT( void )
{
 	HAL_GPIO_WritePin(BT_PWR_EN_GPIO_Port, BT_PWR_EN_Pin, GPIO_PIN_RESET);
	HAL_Delay( 500 );
	HAL_GPIO_WritePin(BT_NRST_GPIO_Port, BT_NRST_Pin, GPIO_PIN_RESET);
	HAL_Delay( 200 );
	HAL_GPIO_WritePin(BT_NRST_GPIO_Port, BT_NRST_Pin, GPIO_PIN_SET);	
}

