#ifndef GPIO_PROTO_H
#define GPIO_PROTO_H

    #include "generic_types.h"
    #include "gpio_types.h"

    /* GPIO_Clock() : To enable clock for a given GPIO Port */
    void GPIO_Clock(gpio_enable_t enordis, gpio_port_def_t GPIO_ID);

    /* GPIO_Init() : To initialize a GPIO port, and set a specific functionality for a pin */
    void GPIO_Init(gpio_config_t gpio);

    /* GPIO_WritetoPin() : API used to write into a GPIO Pin */
    void GPIO_WritetoPin(gpio_config_t gpio, uint8_t val);

#endif /* GPIO_PROTO_H */
