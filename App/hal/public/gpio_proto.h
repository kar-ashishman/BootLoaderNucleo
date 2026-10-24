/*
 * gpio_proto.h
 *
 *  Created on: Jun 17, 2025
 *      Author: niral
 */

#ifndef PUBLIC_GPIO_PROTO_H_
#define PUBLIC_GPIO_PROTO_H_


#include<stdint.h>
#include"gpio_types.h"

void GPIO_Clock(gpio_enable_t enordis, gpio_port_def_t GPIO_ID);
void GPIO_Init(gpio_config_t gpio);
/*
uint8_t GPIO_ReadfromInput_pin(GPIO_Reg_def_t *pGPIOx,uint8_t GPIO_PinNumber);*/
void GPIO_WritetoPin(gpio_config_t gpio,uint8_t val);
void delay(uint32_t time);

#endif /* PUBLIC_GPIO_PROTO_H_ */
