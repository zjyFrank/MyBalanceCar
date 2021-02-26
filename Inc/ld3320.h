#ifndef __ld3320_H
#define __ld3320_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "stm32f1xx_hal.h"
#include "main.h"

	 
#define CLK_IN   		24.000	/* user need modify this value according to clock in */
#define LD_PLL_11			(uint8_t)((CLK_IN/2.0)-1)
#define LD_PLL_MP3_19		0x0f
#define LD_PLL_MP3_1B		0x18
#define LD_PLL_MP3_1D   	(uint8_t)(((90.0*((LD_PLL_11)+1))/(CLK_IN))-1)

#define LD_PLL_ASR_19 		(uint8_t)(CLK_IN*32.0/(LD_PLL_11+1) - 0.51)
#define LD_PLL_ASR_1B 		0x48
#define LD_PLL_ASR_1D 		0x1f

#define CODE_DEFAULT	0
#define CODE_BEIJING	1
#define CODE_SHANGHAI	3
#define CODE_TIANJIN	7
#define CODE_CHONGQING	8

#define MIC_VOL 0x30

void LD3320_Power( int on );
void LD3320_Init_Common( void );
void LD3320_Init_ASR( void );
uint8_t LD3320_Check_ASRBusyFlag_b2( void );
void LD3320_AsrStart( void );
uint8_t LD3320_AsrRun( void );
uint8_t LD3320_AsrAddFixed( void );
uint8_t LD3320_GetResult( void );
void LD3320_Int( void );

uint8_t RunASR (void);


#ifdef __cplusplus
}
#endif
#endif /*__ld3320_H */
