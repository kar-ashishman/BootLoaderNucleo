#ifndef GPIO_TYPES_H
#define GPIO_TYPES_H

	#include "generic_types.h"

	/* GPIO register definition
	*
	* This structure represents the memory-mapped registers of a GPIO
	* port. The order of members must match the register offsets
	* defined in the STM32F446 reference manual.
	*
	*/
	typedef struct
	{
		volatile uint32_t gpio_moder;
		volatile uint32_t gpio_otyper;
		volatile uint32_t gpio_ospeedr;
		volatile uint32_t gpio_pupdr;
		volatile uint32_t gpio_idr;
		volatile uint32_t gpio_odr;
		volatile uint32_t gpio_bsrr;
		volatile uint32_t gpio_lckr;
		volatile uint32_t gpio_aflr;
		volatile uint32_t gpio_afrh;
	}GPIO_Reg_def_t;

	/* GPIO Ports  */
	typedef enum
	{
		GPIO_ID_A,
		GPIO_ID_B,
		GPIO_ID_C,
		GPIO_ID_D,
		GPIO_ID_E,
		GPIO_ID_F,
		GPIO_ID_G,
		GPIO_ID_H
	}gpio_port_def_t;

	/*Enable/disable to clock */
	typedef enum
	{
		GPIO_DISABLE,
		GPIO_ENABLE
	}gpio_enable_t;

	/* GPIO Port mode register */
	typedef enum
	{
		GPIO_MODE_IN,
		GPIO_MODE_OUT,
		GPIO_MODE_ALTFUN,
		GPIO_MODE_ANALOG
	}gpio_moder_t;

	/* GPIO Output type register */
	typedef enum
	{
		GPIO_OP_PUSH_PULL,
		GPIO_OP_OPEN_DRAIN
	}gpio_otyper_t;

	/* GPIO port output speed register */
	typedef enum
	{
		GPIO_LOW_SPEED,
		GPIO_MEDIUM_SPEED,
		GPIO_HIGH_SPEED,
		GPIO_VERY_HIGH_SPEED
	}gpio_ospeedr_t;

	/*GPIO Pull-up and Pull-down register */
	typedef enum
	{
		GPIO_NO_PULLUP_PULLDOWN,
		GPIO_PULL_UP,
		GPIO_PULL_DOWN,
		GPIO_RESERVED
	}gpio_pupdr_t;

	/* GPIO Alternate functionality register*/
	typedef enum
	{
		GPIO_AF0,
		GPIO_AF1,
		GPIO_AF2,
		GPIO_AF3,
		GPIO_AF4,
		GPIO_AF5,
		GPIO_AF6,
		GPIO_AF7,
		GPIO_AF8,
		GPIO_AF9,
		GPIO_AF10,
		GPIO_AF11,
		GPIO_AF12,
		GPIO_AF13,
		GPIO_AF14,
		GPIO_AF15
	}gpio_alt_func_t;

	/*GPIO configuration used by an application */
	typedef struct {
		gpio_port_def_t  port_id;
		gpio_moder_t     mode;
		gpio_otyper_t    op_type;
		gpio_ospeedr_t   op_speed;
		gpio_pupdr_t     push_pull;
		gpio_alt_func_t  alt_func;
		gpio_enable_t    enable;
		uint8_t          pin_number;
	}gpio_config_t;

#endif /* GPIO_TYPES_H */
