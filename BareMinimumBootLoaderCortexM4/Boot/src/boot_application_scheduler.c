#include "generic_types.h"
#include "boot_defs.h"
#include "uart_proto.h"
#include "gpio_types.h"
#include "gpio_proto.h"

uint8_t application_auth_state = UNAUTHENTICATED;


void test_device_hdlr(gpio_config_t* OnBoardLed) {
    uint32_t delay_cycles = DELAY_CYCLES(1000);
    uint8_t i = 0;

    while (i < 5) {
        // OFF
        GPIO_WritetoPin(*OnBoardLed, 0);
        while (delay_cycles > 0) {delay_cycles--;}

        // ON
        GPIO_WritetoPin(*OnBoardLed, 1);
        delay_cycles = DELAY_CYCLES(1000);
        while (delay_cycles > 0) {delay_cycles--;}
        i ++;
    }
}

void auth_request_handler() {
    uint32_t keyCalculated = 0;
    uint32_t key = 0;
    uint32_t seed = 0x78654321;
    // send a random 4 byte seed
    uart_write((seed & 0xFF000000) >> 24);
    uart_write((seed & 0xFF0000) >> 16);
    uart_write((seed & 0xFF00) >> 8);
    uart_write((seed & 0xFF));

    // calculate key
    key = seed ^ AUTH_MAGIC_NUMBER;

    // receive key
    key = key | ((uint8_t)uart_receive() << 24);
    key = key | ((uint8_t)uart_receive() << 16);
    key = key | ((uint8_t)uart_receive() << 8);
    key = key | (uint8_t)uart_receive();

    // Check if key match
    application_auth_state = key == keyCalculated ? AUTHENTICATED : UNAUTHENTICATED;
}


int8_t user_request_process(gpio_config_t* OnBoardLed, uint8_t req) {
    switch (req) {
        case AUTH_REQUEST:
            uart_write(0);
            auth_request_handler();
            if (application_auth_state == UNAUTHENTICATED) uart_write(255); // NACK
            else uart_write(0); // ACK
            break;
        case TEST_DEVICE:
            uart_write(0); // ACK
            test_device_hdlr(OnBoardLed);
            uart_write(0); // INDICATE DEVICE TESTING IS OVER
            break;
        case BOOTVER_REQUEST:
            uart_write(0);
            uart_write(18);
            break;
        case FLASH_REQUEST:
            uart_write(0);
            break;
        default:
            uart_write(255); // sending -1 as unknown response
            break;
    }
    return 0;
}
