//
// Created by kar-a on 26-09-2026.
//

#ifndef BOOT_DEFS_H
#define BOOT_DEFS_H

    /* BOOT LOADER APPLICATION STATES */
    #define AUTH_REQUEST        1
    #define TEST_DEVICE         2
    #define BOOTVER_REQUEST     3
    #define FLASH_REQUEST       4

    #define UNAUTHENTICATED     10
    #define AUTHENTICATED       11

    #define AUTH_MAGIC_NUMBER   0x12345678

#endif //BOOT_DEFS_H
