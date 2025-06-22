#include<stdint.h>

#define FLASH_BASE       0x08000000
#define FLASH_KEY1       0x45670123
#define FLASH_KEY2       0xCDEF89AB

#define FLASH_ACR        (*(volatile uint32_t*)0x40022000)
#define FLASH_KEYR       (*(volatile uint32_t*)0x40022004)
#define FLASH_SR         (*(volatile uint32_t*)0x4002200C)
#define FLASH_CR         (*(volatile uint32_t*)0x40022010)
#define FLASH_AR         (*(volatile uint32_t*)0x40022014)

#define FLASH_CR_PG      0x00000001
#define FLASH_CR_LOCK    0x00000080
#define FLASH_SR_BSY     0x00000001

void flash_unlock(void) {
    if (FLASH_CR & FLASH_CR_LOCK) {
        FLASH_KEYR = FLASH_KEY1;
        FLASH_KEYR = FLASH_KEY2;
    }
}

void flash_lock(void) {
    FLASH_CR |= FLASH_CR_LOCK;
}

void flash_write_word(uint32_t address, uint16_t data) {
    while (FLASH_SR & FLASH_SR_BSY); // Wait if busy

    FLASH_CR |= FLASH_CR_PG;         // Enable programming

    *(volatile uint16_t*)address = data;  // Write lower 16-bits
    while (FLASH_SR & FLASH_SR_BSY);      // Wait for completion

    FLASH_CR &= ~FLASH_CR_PG;        // Disable programming
}

int main(void) {
    flash_unlock();
    flash_write_word(FLASH_BASE + 0x1000, 0x1234); // Example address and data
    flash_lock();
}