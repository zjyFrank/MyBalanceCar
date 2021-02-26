/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_PB1_Pin GPIO_PIN_13
#define USER_PB1_GPIO_Port GPIOC
#define MT1_A_Pin GPIO_PIN_0
#define MT1_A_GPIO_Port GPIOC
#define MT1_B_Pin GPIO_PIN_1
#define MT1_B_GPIO_Port GPIOC
#define MT2_B_Pin GPIO_PIN_2
#define MT2_B_GPIO_Port GPIOC
#define MT2_A_Pin GPIO_PIN_3
#define MT2_A_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_0
#define LED2_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOA
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_INT_Pin GPIO_PIN_4
#define SPI1_INT_GPIO_Port GPIOC
#define SPI1_INT_EXTI_IRQn EXTI4_IRQn
#define AUDIO_PWR_EN_Pin GPIO_PIN_5
#define AUDIO_PWR_EN_GPIO_Port GPIOC
#define BT_PWR_EN_Pin GPIO_PIN_0
#define BT_PWR_EN_GPIO_Port GPIOB
#define BT_BOOT_Pin GPIO_PIN_1
#define BT_BOOT_GPIO_Port GPIOB
#define BT_NRST_Pin GPIO_PIN_2
#define BT_NRST_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define MT2_S0_Pin GPIO_PIN_6
#define MT2_S0_GPIO_Port GPIOC
#define MT2_S1_Pin GPIO_PIN_7
#define MT2_S1_GPIO_Port GPIOC
#define MT2_EN_Pin GPIO_PIN_8
#define MT2_EN_GPIO_Port GPIOC
#define MT1_EN_Pin GPIO_PIN_9
#define MT1_EN_GPIO_Port GPIOC
#define MT1_S1_Pin GPIO_PIN_8
#define MT1_S1_GPIO_Port GPIOA
#define MT1_S0_Pin GPIO_PIN_9
#define MT1_S0_GPIO_Port GPIOA
#define AUDIO_NRST_Pin GPIO_PIN_10
#define AUDIO_NRST_GPIO_Port GPIOA
#define DOT_LAT_Pin GPIO_PIN_15
#define DOT_LAT_GPIO_Port GPIOA
#define USER_PB0_Pin GPIO_PIN_12
#define USER_PB0_GPIO_Port GPIOC
#define DOT_EN_Pin GPIO_PIN_2
#define DOT_EN_GPIO_Port GPIOD
#define DOT_SHIFT_Pin GPIO_PIN_4
#define DOT_SHIFT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
