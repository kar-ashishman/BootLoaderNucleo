/*
FILENAME: boot.c
DESCRIPTION: Boot implementation
*/

/************INCLUDES******************/
#include "generic_types.h"
#include  "gpio_proto.h"
#include  "gpio_types.h"

int main() {
    gpio_config_t test_pin =  {
        .port_id = GPIO_ID_A,
        .mode = GPIO_MODE_OUT,
        .op_type = GPIO_OP_PUSH_PULL,
        .op_speed = GPIO_LOW_SPEED,
        .push_pull = GPIO_PULL_UP,
        .alt_func = GPIO_AF7,
        .enable = GPIO_ENABLE,
        .pin_number = 5
    };
    GPIO_Init(test_pin);

    while(1) {
        GPIO_WritetoPin(test_pin, 1);
        for (int i = 0; i < 50000; i++) {}
        GPIO_WritetoPin(test_pin, 0);
        for (int i = 0; i < 50000; i++) {}
    }
    return 0;
}

/*

 */