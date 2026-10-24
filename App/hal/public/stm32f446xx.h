/*
 * stm32f446xx.h
 *
 *  Created on: Jun 17, 2025
 *      Author: niral
 */

/* File defines
 * Base addresses of all the peripherals and Buses at the controller level */

#ifndef PUBLIC_STM32F446XX_H_
#define PUBLIC_STM32F446XX_H_

#include <stdint.h>
#include"gpio_types.h"


/*Base peripheral of Buses*/
#define AHB1_BASE_ADDRESS 0x40020000U
#define AHB2_BASE_ADDRESS 0x50000000U
#define APB1_BASE_ADDRESS 0x40000000U
#define APB2_BASE_ADDRESS 0x40010000U
/*Base address of the GPIO peripherals
 *Connected to AHB1 Bus */
#define GPIOA_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x0000)
#define GPIOB_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x0400)
#define GPIOC_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x0800)
#define GPIOD_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x0C00)
#define GPIOE_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x1000)
#define GPIOF_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x1400)
#define GPIOG_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x1800)
#define GPIOH_BASE_ADDRESS AHB1_BASE_ADDRESS + (0x1C00)

#define TIM8_BASE_ADDRESS  0x40010400

/*USART2 Base address */
#define USART2_BASE_ADDRESS (APB1_BASE_ADDRESS + (0x4400))

/*RCC Base memory address*/
#define RCC_BASE_ADDRESS 0x40023800U

/*RCC base address type-casted to the type RCC_Reg_def_t */
#define RCC ((volatile RCC_Reg_def_t*)RCC_BASE_ADDRESS)



#endif /* PUBLIC_STM32F446XX_H_ */
