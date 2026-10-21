#include "generic_types.h"
#include "boot_defs.h"
#include "uart_proto.h"
#include "gpio_types.h"
#include "gpio_proto.h"
#include "flash_proto.h"
#include "flash_types.h"
#include "stm32f446xx.h"

uint8_t application_auth_state = UNAUTHENTICATED;

unsigned char check_crc(unsigned int data, unsigned char framechkseq) {
    /*
     * Check if the CRC is valid for the given data.
        * :param data: The input data 32 bit long.
        * :param framechkseq: The frame check sequence, which is a 1 byte long value and should be >= 0x80.
     * :return: True if the CRC is valid, False otherwise.
     */
    unsigned char cnt = 31, bitpos;
    unsigned int rem, num;
    if (data < framechkseq) {
        if(data == (framechkseq & 0x7f)) return 1;
        else return 0;
    }
    /* Find first non zero bit */
    while((data & (1 << cnt)) == 0) cnt--;
    /* initialize bit position */
    bitpos = cnt - 7;
    num = (data & (0xFFU << bitpos)) >> bitpos;
    rem = framechkseq;
    while(num >= (1 << 7)){
        rem = num ^ framechkseq;
        while(bitpos > 0) {
            bitpos --;
            rem = (rem << 1) | ((data & (1 << bitpos)) >> bitpos);
            if (rem  >= (1 << 7)) break;
        }
        num = rem & 0xFF;
    }
    return (rem==0);
}

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

    keyCalculated = keyCalculated | (uint8_t)uart_receive();
    keyCalculated = keyCalculated | ((uint8_t)uart_receive() << 8);
    keyCalculated = keyCalculated | ((uint8_t)uart_receive() << 16);
    keyCalculated = keyCalculated | ((uint8_t)uart_receive() << 24);

    // Check if key match
    application_auth_state = key == keyCalculated ? AUTHENTICATED : UNAUTHENTICATED;
}

void copy_sector(uint32_t source, uint32_t dest){
	//copies contents from primary to flash to backup
	for(int i = 0; i<SIZE_PRIMARY_FLASH;i++){
		uint8_t val = *((uint8_t*)(source+i));
		app_write_byte(dest + i, val);
	}
}

void erase_primary(){
	//erase flash sectors 1-5
	for(uint8_t i=1;i<6;i++){
	    flash_sector_erase(i);
	}
}

void flash()
{
    uint16_t flash_size = 0;
    uint8_t buffer[4] = {0};
    uint32_t num;
    uint32_t offset = 0;
    uint8_t data = 1;
    uint8_t error_counter = 0;

    if (application_auth_state == AUTHENTICATED) {
        // unlock write into the flash memory
        flash_write_unlock();
        //flash copy from sector 1-5 to sector 7
    }
    else {
        // send NACK
        uart_write(255);
        return;
    }

    //Receive the size of the flash file
    for(int i = 0;i<2; i++)
    {
        buffer[i] = uart_receive();
    }
    flash_size = ((uint16_t)buffer[0]<<8) | buffer[1];


    //erase sector 6 and 7 - to copy backup
    flash_sector_erase(6);
    flash_sector_erase(7);

    //copy flash from primary to backup
    copy_sector((uint32_t)SECTOR_1_ADDR,(uint32_t)SECTOR_6_ADDR);

    //erase primary
    erase_primary();

    //send ACK to send data
    uart_write(0x0);

    while(flash_size!=0){
        for(int i = 0; i < 4; i++){
            //Receive the data
            buffer[i] = uart_receive();
        }
        num = ((uint32_t)buffer[0] << 24) | ((uint32_t)buffer[1] << 16) |
        		        ((uint32_t)buffer[2] << 8) | (uint32_t)buffer[3];

        if(check_crc(num,0b10101010)){

        	// reset error counter after we receive good frame
            error_counter = 0;
            //CRC is 7 bits long, so we need to shift the number 7 bits to the right to get the original data
            num = num >> 7;
            //Extract data
            buffer[0] = (num & 0xFF0000)>>16;
            buffer[1] = (num & 0x00FF00)>>8;
            buffer[2] = num & 0x0000FF;
            app_write_byte(SECTOR_1_ADDR + offset, buffer[2]);
            offset++;
            app_write_byte(SECTOR_1_ADDR + offset, buffer[1]);
            offset++;
            app_write_byte(SECTOR_1_ADDR + offset, buffer[0]);
            offset++;
            uart_write(0);
        }
        else{
            if(error_counter > 5){
            	//erase primary flash
            	erase_primary();
            	copy_sector((uint32_t)SECTOR_6_ADDR,(uint32_t)SECTOR_1_ADDR);
                uart_write(255); // send NACK
                return; // abort flashing since didn't receive correct data after 5 attempts
            }
            error_counter++;
            uart_write(254); // send NACK 254 indicating retry
            continue;//don't decrement flash_size, so that the client can send the same data again
        }
        flash_size--;
    }
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
            flash();
            break;
        default:
            uart_write(255); // sending -1 as unknown response
            break;
    }
    return 0;
}
