/*
FILENAME: boot.c
DESCRIPTION: Boot implementation
*/

/************INCLUDES******************/
#include "generic_types.h"
#include  "gpio_proto.h"
#include  "gpio_types.h"
#include  "uart_proto.h"
#include  "boot_defs.h"

uint8_t user_request_process(gpio_config_t* OnBoardLed, uint8_t req);
#define RESET_HANDLER_ADDRESS 0x08004004
void (*func_ptr)(void);

int main() {
    uint32_t delay_cycles = DELAY_CYCLES(100);
    uint8_t uart_data = 0;

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
    USART_Init();

    // Startup Sequence - Blink the LED 2 Times

    GPIO_WritetoPin(test_pin, 1);
    while (delay_cycles > 0) {delay_cycles--;}
    GPIO_WritetoPin(test_pin, 0);
    delay_cycles = DELAY_CYCLES(100);
    while (delay_cycles > 0) {delay_cycles--;}
    GPIO_WritetoPin(test_pin, 1);
    delay_cycles = DELAY_CYCLES(100);
    while (delay_cycles > 0) {delay_cycles--;}
    GPIO_WritetoPin(test_pin, 0);

    while(1) {
        // Wait for uart data
        uart_data = uart_receive();
        user_request_process(&test_pin, uart_data);
        if(uart_data == FLASH_REQUEST) {
            uint32_t pointer = *(volatile uint32_t*)((RESET_HANDLER_ADDRESS));
		    func_ptr = (uint32_t *)pointer;
		    (func_ptr)();
        }
    }
    return 0;
}

/*

 */
