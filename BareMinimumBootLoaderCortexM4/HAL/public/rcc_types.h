#ifndef RCC_TYPES_H
#define RCC_TYPES_H

	#include "generic_types.h"

	/*The RCC peripheral has a set of registers*/
	typedef struct
	{
		volatile uint32_t rcc_cr;
		volatile uint32_t rcc_pllcfgr;
		volatile uint32_t rcc_cfgr;
		volatile uint32_t rcc_cir;
		volatile uint32_t rcc_ahb1rstr;
		volatile uint32_t rcc_ahb2rstr;
		volatile uint32_t rcc_ahb3rstr;
		uint32_t RESERVED0;
		volatile uint32_t rcc_apb1rstr;
		volatile uint32_t rcc_apb2rstr;
		uint32_t RESERVED1[2];
		volatile uint32_t rcc_ahb1enr;
		volatile uint32_t rcc_ahb2enr;
		volatile uint32_t rcc_ahb3enr;
		uint32_t RESERVED3;
		volatile uint32_t rcc_apb1enr;
		volatile uint32_t rcc_apb2enr;
		uint32_t RESERVED4[2];
		volatile uint32_t rcc_ahb1lpenr;
		volatile uint32_t rcc_ahb2lpenr;
		volatile uint32_t rcc_ahb3lpenr;
		uint32_t RESERVED5[2];
		volatile uint32_t rcc_apb1lpenr;
		volatile uint32_t rcc_apb2lpenr;
		volatile uint32_t rcc_bdcr;
		volatile uint32_t rcc_csr;
		uint32_t RESERVED6[2];
		volatile uint32_t rcc_sscgr;
		volatile uint32_t rcc_plli2scfgr;
		volatile uint32_t rcc_pllsaicfgr;
		volatile uint32_t rcc_dckcfgr;
		volatile uint32_t rcc_ckgatenr;
		volatile uint32_t rcc_dckcfgr2;
	}RCC_Reg_def_t;

#endif /* RCC_TYPES_H */
