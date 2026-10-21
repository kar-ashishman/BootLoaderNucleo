/*
 * flash_proto.h
 *
 *  Created on: Jul 23, 2025
 *      Author: niral
 */

#ifndef PUBLIC_FLASH_PROTO_H_
#define PUBLIC_FLASH_PROTO_H_

#include"generic_types.h"

/* Function to unlock the write access to flash memory */
void flash_write_unlock();

/* Function to erase a specific sector of flash memory */
void flash_sector_erase(uint8_t sector_num);
void flash_lock();

#endif /* PUBLIC_FLASH_PROTO_H_ */
